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
#include "RendererRuntime/Resource/CompositorNode/Pass/Scene/CompositorInstancePassScene.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Scene/CompositorResourcePassScene.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeInstance.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceInstance.h"
#include "RendererRuntime/Resource/Scene/ISceneResource.h"
#include "RendererRuntime/Resource/Scene/Node/ISceneNode.h"
#include "RendererRuntime/Resource/Scene/Item/MeshSceneItem.h"
#include "RendererRuntime/Resource/Scene/Item/CameraSceneItem.h"
#include "RendererRuntime/Resource/Mesh/MeshResource.h"
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/PassUniformBufferManager.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		// TODO(co) Just a first quick'n'dirty placeholder for the real implementation using a render queue

		void draw(const RendererRuntime::IRendererRuntime& rendererRuntime, RendererRuntime::CameraSceneItem& cameraSceneItem)
		{
			Renderer::IRenderer& renderer = rendererRuntime.getRenderer();
			const RendererRuntime::MeshResources& meshResources = rendererRuntime.getMeshResourceManager().getMeshResources();
			const RendererRuntime::MaterialResources& materialResources = rendererRuntime.getMaterialResourceManager().getMaterialResources();
			const RendererRuntime::MaterialBlueprintResourceManager& materialBlueprintResourceManager = rendererRuntime.getMaterialBlueprintResourceManager();
			RendererRuntime::MaterialBlueprintResource* currentlyBoundMaterialBlueprintResource = nullptr;

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

			// Loop through all scene nodes
			const RendererRuntime::ISceneResource::SceneNodes& sceneNodes = cameraSceneItem.getSceneResource().getSceneNodes();
			const size_t numberOfSceneNodes = sceneNodes.size();
			for (size_t sceneNodeIndex = 0; sceneNodeIndex < numberOfSceneNodes; ++sceneNodeIndex)
			{
				RendererRuntime::ISceneNode* sceneNode = sceneNodes[sceneNodeIndex];
				const RendererRuntime::Transform& transform = sceneNode->getTransform();

				// Loop through all scene items attached to the current scene node
				const RendererRuntime::ISceneNode::AttachedSceneItems& attachedSceneItems = sceneNode->getAttachedSceneItems();
				const size_t numberOfAttachedSceneItems = attachedSceneItems.size();
				for (size_t attachedSceneItemIndex = 0; attachedSceneItemIndex < numberOfAttachedSceneItems; ++attachedSceneItemIndex)
				{
					RendererRuntime::ISceneItem* sceneItem = attachedSceneItems[attachedSceneItemIndex];
					if (sceneItem->getSceneItemTypeId() == RendererRuntime::MeshSceneItem::TYPE_ID)
					{
						// Draw mesh instance
						RendererRuntime::MeshSceneItem* meshSceneItem = static_cast<RendererRuntime::MeshSceneItem*>(sceneItem);
						const RendererRuntime::MeshResource* meshResource = meshResources.tryGetElementById(meshSceneItem->getMeshResourceId());
						if (nullptr != meshResource)
						{
							// Setup input assembly (IA): Set the used vertex array
							Renderer::IVertexArrayPtr vertexArrayPtr = meshResource->getVertexArrayPtr();
							if (nullptr != vertexArrayPtr)
							{
								renderer.iaSetVertexArray(vertexArrayPtr);
								const uint32_t numberOfVertices = meshResource->getNumberOfVertices();

								// Loop through all sub-meshes
								const RendererRuntime::SubMeshes& subMeshes = meshResource->getSubMeshes();
								const uint32_t numberOfSubMeshes = meshSceneItem->getNumberOfSubMeshes();
								for (uint32_t subMeshIndex = 0; subMeshIndex < numberOfSubMeshes; ++subMeshIndex)
								{
									// Material resource
									const RendererRuntime::MaterialResource* materialResource = materialResources.tryGetElementById(meshSceneItem->getMaterialResourceIdOfSubMesh(subMeshIndex));
									if (nullptr != materialResource)
									{
										RendererRuntime::MaterialTechnique* materialTechnique = materialResource->getMaterialTechniqueById("Default");
										if (nullptr != materialTechnique)
										{
											RendererRuntime::MaterialBlueprintResource* materialBlueprintResource = static_cast<RendererRuntime::MaterialBlueprintResource*>(materialBlueprintResourceManager.tryGetResourceByResourceId(materialTechnique->getMaterialBlueprintResourceId()));
											if (nullptr != materialBlueprintResource && materialBlueprintResource->isFullyLoaded())
											{
												// TODO(co) Pass shader properties (later on we cache as much as possible of this work inside the renderable)
												RendererRuntime::ShaderProperties shaderProperties;
												RendererRuntime::DynamicShaderPieces dynamicShaderPieces[RendererRuntime::NUMBER_OF_SHADER_TYPES];
												{ // Gather shader properties from static material properties generating shader combinations
													const RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector = materialResource->getSortedPropertyVector();
													const size_t numberOfMaterialProperties = sortedMaterialPropertyVector.size();
													for (size_t i = 0; i < numberOfMaterialProperties; ++i)
													{
														const RendererRuntime::MaterialProperty& materialProperty = sortedMaterialPropertyVector[i];
														if (materialProperty.getUsage() == RendererRuntime::MaterialProperty::Usage::SHADER_COMBINATION)
														{
															switch (materialProperty.getValueType())
															{
																case RendererRuntime::MaterialPropertyValue::ValueType::BOOLEAN:
																	shaderProperties.setPropertyValue(materialProperty.getMaterialPropertyId(), materialProperty.getBooleanValue());
																	break;

																case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER:
																	shaderProperties.setPropertyValue(materialProperty.getMaterialPropertyId(), materialProperty.getIntegerValue());
																	break;

																case RendererRuntime::MaterialPropertyValue::ValueType::UNKNOWN:
																case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_2:
																case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_3:
																case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_4:
																case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT:
																case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_2:
																case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_3:
																case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_4:
																case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_3_3:
																case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_4_4:
																case RendererRuntime::MaterialPropertyValue::ValueType::FILL_MODE:
																case RendererRuntime::MaterialPropertyValue::ValueType::CULL_MODE:
																case RendererRuntime::MaterialPropertyValue::ValueType::CONSERVATIVE_RASTERIZATION_MODE:
																case RendererRuntime::MaterialPropertyValue::ValueType::DEPTH_WRITE_MASK:
																case RendererRuntime::MaterialPropertyValue::ValueType::STENCIL_OP:
																case RendererRuntime::MaterialPropertyValue::ValueType::COMPARISON_FUNC:
																case RendererRuntime::MaterialPropertyValue::ValueType::BLEND:
																case RendererRuntime::MaterialPropertyValue::ValueType::BLEND_OP:
																case RendererRuntime::MaterialPropertyValue::ValueType::FILTER_MODE:
																case RendererRuntime::MaterialPropertyValue::ValueType::TEXTURE_ADDRESS_MODE:
																case RendererRuntime::MaterialPropertyValue::ValueType::TEXTURE_ASSET_ID:
																case RendererRuntime::MaterialPropertyValue::ValueType::COMPOSITOR_TEXTURE_REFERENCE:
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

														// Fill the unknown uniform buffers
														materialBlueprintResource->fillUnknownUniformBuffers();

														// Fill the pass uniform buffer
														{ // TODO(co) Just a dummy usage for now
															RendererRuntime::PassUniformBufferManager* passUniformBufferManager = materialBlueprintResource->getPassUniformBufferManager();
															if (nullptr != passUniformBufferManager)
															{
																passUniformBufferManager->resetCurrentPassBuffer();

																// TODO(co) Camera usage
																const RendererRuntime::Transform worldSpaceToViewSpaceTransform;
																passUniformBufferManager->fillBuffer(worldSpaceToViewSpaceTransform);
															}
														}

														// Bind the material blueprint resource to the used renderer
														materialBlueprintResource->bindToRenderer();
													}

													// Cheap state change: Bind the material technique to the used renderer
													materialTechnique->bindToRenderer(rendererRuntime);

													// Set the used pipeline state object (PSO)
													renderer.setPipelineState(pipelineStatePtr);

													// Fill the instance uniform buffer
													materialBlueprintResource->fillInstanceUniformBuffer(transform, *materialTechnique);

													// Setup input assembly (IA): Set the primitive topology used for draw calls
													const RendererRuntime::SubMesh& subMesh = subMeshes[subMeshIndex];
													renderer.iaSetPrimitiveTopology(subMesh.getPrimitiveTopology());

													// Render the specified geometric primitive, based on indexing into an array of vertices
													renderer.drawIndexed(subMesh.getStartIndexLocation(), subMesh.getNumberOfIndices(), 0, 0, numberOfVertices);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}

			// End debug event
			RENDERER_END_DEBUG_EVENT(&renderer)
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::ICompositorInstancePass methods ]
	//[-------------------------------------------------------]
	void CompositorInstancePassScene::execute(CameraSceneItem* cameraSceneItem)
	{
		// TODO(co) Just a first test
		if (nullptr != cameraSceneItem)
		{
			::detail::draw(getCompositorNodeInstance().getCompositorWorkspaceInstance().getRendererRuntime(), *cameraSceneItem);
		}
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CompositorInstancePassScene::CompositorInstancePassScene(const CompositorResourcePassScene& compositorResourcePassScene, const CompositorNodeInstance& compositorNodeInstance) :
		ICompositorInstancePass(compositorResourcePassScene, compositorNodeInstance)
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
