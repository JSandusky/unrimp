/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/RenderQueue/RenderQueue.h"
#include "RendererRuntime/RenderQueue/RenderableManager.h"
#include "RendererRuntime/Core/Math/Transform.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/PassBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/InstanceBufferManager.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void RenderQueue::addRenderablesFromRenderableManager(uint32_t, const RenderableManager& renderableManager)
	{
		// TODO(co) Add hash for queued renderables sorting
		for (const Renderable& renderable : renderableManager.getRenderables())
		{
			mQueuedRenderables.emplace_back(renderable, renderableManager);
		}
	}

	void RenderQueue::draw()
	{
		// TODO(co) This is just a dummy implementation

		const MaterialResources& materialResources = mRendererRuntime.getMaterialResourceManager().getMaterialResources();
		const MaterialBlueprintResourceManager& materialBlueprintResourceManager = mRendererRuntime.getMaterialBlueprintResourceManager();
		MaterialBlueprintResource* currentlyBoundMaterialBlueprintResource = nullptr;
		InstanceBufferManager& instanceBufferManager = materialBlueprintResourceManager.getInstanceBufferManager();
		const MaterialTechniqueId materialTechniqueId = "Default";

		for (const QueuedRenderable& queuedRenderable : mQueuedRenderables)
		{
			// Sanity checks
			assert(nullptr != queuedRenderable.renderable);
			assert(nullptr != queuedRenderable.renderableManager);
			const Renderable&		 renderable		   = *queuedRenderable.renderable;
			const RenderableManager& renderableManager = *queuedRenderable.renderableManager;

			// Setup input assembly (IA): Set the used vertex array
			Renderer::IVertexArrayPtr vertexArrayPtr = renderable.getVertexArrayPtr();
			if (nullptr != vertexArrayPtr)
			{
				Renderer::IRenderer& renderer = vertexArrayPtr->getRenderer();
				renderer.iaSetVertexArray(vertexArrayPtr);

				// Material resource
				const MaterialResource* materialResource = materialResources.tryGetElementById(renderable.getMaterialResourceId());
				if (nullptr != materialResource)
				{
					MaterialTechnique* materialTechnique = materialResource->getMaterialTechniqueById(materialTechniqueId);
					if (nullptr != materialTechnique)
					{
						MaterialBlueprintResource* materialBlueprintResource = static_cast<MaterialBlueprintResource*>(materialBlueprintResourceManager.tryGetResourceByResourceId(materialTechnique->getMaterialBlueprintResourceId()));
						if (nullptr != materialBlueprintResource && materialBlueprintResource->isFullyLoaded())
						{
							// TODO(co) Pass shader properties (later on we cache as much as possible of this work inside the renderable)
							ShaderProperties shaderProperties;
							DynamicShaderPieces dynamicShaderPieces[NUMBER_OF_SHADER_TYPES];
							{ // Gather shader properties from static material properties generating shader combinations
								const MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector = materialResource->getSortedPropertyVector();
								const size_t numberOfMaterialProperties = sortedMaterialPropertyVector.size();
								for (size_t i = 0; i < numberOfMaterialProperties; ++i)
								{
									const MaterialProperty& materialProperty = sortedMaterialPropertyVector[i];
									if (materialProperty.getUsage() == MaterialProperty::Usage::SHADER_COMBINATION)
									{
										switch (materialProperty.getValueType())
										{
											case MaterialPropertyValue::ValueType::BOOLEAN:
												shaderProperties.setPropertyValue(materialProperty.getMaterialPropertyId(), materialProperty.getBooleanValue());
												break;

											case MaterialPropertyValue::ValueType::INTEGER:
												shaderProperties.setPropertyValue(materialProperty.getMaterialPropertyId(), materialProperty.getIntegerValue());
												break;

											case MaterialPropertyValue::ValueType::UNKNOWN:
											case MaterialPropertyValue::ValueType::INTEGER_2:
											case MaterialPropertyValue::ValueType::INTEGER_3:
											case MaterialPropertyValue::ValueType::INTEGER_4:
											case MaterialPropertyValue::ValueType::FLOAT:
											case MaterialPropertyValue::ValueType::FLOAT_2:
											case MaterialPropertyValue::ValueType::FLOAT_3:
											case MaterialPropertyValue::ValueType::FLOAT_4:
											case MaterialPropertyValue::ValueType::FLOAT_3_3:
											case MaterialPropertyValue::ValueType::FLOAT_4_4:
											case MaterialPropertyValue::ValueType::FILL_MODE:
											case MaterialPropertyValue::ValueType::CULL_MODE:
											case MaterialPropertyValue::ValueType::CONSERVATIVE_RASTERIZATION_MODE:
											case MaterialPropertyValue::ValueType::DEPTH_WRITE_MASK:
											case MaterialPropertyValue::ValueType::STENCIL_OP:
											case MaterialPropertyValue::ValueType::COMPARISON_FUNC:
											case MaterialPropertyValue::ValueType::BLEND:
											case MaterialPropertyValue::ValueType::BLEND_OP:
											case MaterialPropertyValue::ValueType::FILTER_MODE:
											case MaterialPropertyValue::ValueType::TEXTURE_ADDRESS_MODE:
											case MaterialPropertyValue::ValueType::TEXTURE_ASSET_ID:
											case MaterialPropertyValue::ValueType::COMPOSITOR_TEXTURE_REFERENCE:
											default:
												assert(false);	// TODO(co) Error handling
												break;
										}
									}
								}
							}
							materialBlueprintResource->optimizeShaderProperties(shaderProperties);

							Renderer::IPipelineStatePtr pipelineStatePtr = materialBlueprintResource->getPipelineStateCacheManager().getPipelineStateCacheByCombination(shaderProperties, dynamicShaderPieces, false);
							if (nullptr != pipelineStatePtr)
							{
								// Expensive state change: Handle material blueprint resource switches
								// -> Render queue should be sorted by material blueprint resource first to reduce those expensive state changes
								if (currentlyBoundMaterialBlueprintResource != materialBlueprintResource)
								{
									currentlyBoundMaterialBlueprintResource = materialBlueprintResource;

									// Fill the pass buffer manager
									{ // TODO(co) Just a dummy usage for now
										PassBufferManager* passBufferManager = materialBlueprintResource->getPassBufferManager();
										if (nullptr != passBufferManager)
										{
											passBufferManager->resetCurrentPassBuffer();

											// TODO(co) Camera usage
											const Transform worldSpaceToViewSpaceTransform;
											passBufferManager->fillBuffer(worldSpaceToViewSpaceTransform);
										}
									}

									// Bind the material blueprint resource and instance buffer manager to the used renderer
									materialBlueprintResource->bindToRenderer();
									instanceBufferManager.bindToRenderer(*materialBlueprintResource);
								}

								// Cheap state change: Bind the material technique to the used renderer
								materialTechnique->bindToRenderer(mRendererRuntime);

								// Set the used pipeline state object (PSO)
								renderer.setPipelineState(pipelineStatePtr);

								{ // Fill the instance buffer manager
									PassBufferManager* passBufferManager = materialBlueprintResource->getPassBufferManager();
									if (nullptr != passBufferManager)
									{
										const MaterialBlueprintResource::UniformBuffer* instanceUniformBuffer = materialBlueprintResource->getInstanceUniformBuffer();
										const MaterialBlueprintResource::TextureBuffer* instanceTextureBuffer = materialBlueprintResource->getInstanceTextureBuffer();
										instanceBufferManager.fillBuffer(*passBufferManager, instanceUniformBuffer, instanceTextureBuffer, renderableManager.getTransform(), *materialTechnique);
									}
								}

								// Setup input assembly (IA): Set the primitive topology used for draw calls
								renderer.iaSetPrimitiveTopology(renderable.getPrimitiveTopology());

								// Render the specified geometric primitive, based on indexing into an array of vertices
								renderer.drawIndexed(Renderer::IndexedIndirectBuffer(renderable.getNumberOfIndices(), 1, renderable.getStartIndexLocation()));
							}
						}
					}
				}
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
