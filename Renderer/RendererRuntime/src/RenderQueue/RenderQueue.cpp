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
#include "RendererRuntime/RenderQueue/IndirectBufferManager.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/PassBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/InstanceBufferManager.h"
#include "RendererRuntime/Core/Math/Transform.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <algorithm>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		const int DepthBits = 15;


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		// Flip the float to deal with negative & positive numbers
		// - See "Rough sorting by depth" - http://aras-p.info/blog/2014/01/16/rough-sorting-by-depth/
		inline uint32_t floatFlip(uint32_t f)
		{
			const uint32_t mask = -int(f >> 31) | 0x80000000;
			return (f ^ mask);
		}

		// Taking highest 10 bits for rough sort of floats.
		// - 0.01 maps to 752; 0.1 to 759; 1.0 to 766; 10.0 to 772;
		// - 100.0 to 779 etc. Negative numbers go similarly in 0..511 range.
		// - See "Rough sorting by depth" - http://aras-p.info/blog/2014/01/16/rough-sorting-by-depth/
		inline uint32_t depthToBits(float depth)
		{
			union { float f; uint32_t i; } f2i;
			f2i.f = depth;
			f2i.i = floatFlip(f2i.i);			// Flip bits to be sortable
			return (f2i.i >> (32 - DepthBits));	// Take highest n-bits
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
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RenderQueue::RenderQueue(IndirectBufferManager& indirectBufferManager, uint8_t minimumRenderQueueIndex, uint8_t maximumRenderQueueIndex, bool transparentPass, bool doSort) :
		mRendererRuntime(indirectBufferManager.getRendererRuntime()),
		mIndirectBufferManager(indirectBufferManager),
		mMinimumRenderQueueIndex(minimumRenderQueueIndex),
		mMaximumRenderQueueIndex(maximumRenderQueueIndex),
		mTransparentPass(transparentPass),
		mDoSort(doSort)
	{
		assert(mMaximumRenderQueueIndex >= mMinimumRenderQueueIndex);
		mQueues.resize(static_cast<size_t>(mMaximumRenderQueueIndex - mMinimumRenderQueueIndex + 1));
	}

	void RenderQueue::clear()
	{
		for (Queue& queue : mQueues)
		{
			queue.queuedRenderables.clear();
			queue.sorted = false;
		}
	}

	void RenderQueue::addRenderablesFromRenderableManager(const RenderableManager& renderableManager)
	{
		// Quantize the cached distance to camera
		const uint32_t quantizedDepth = ::detail::depthToBits(renderableManager.getCachedDistanceToCamera());

		// Register the renderables inside our renderables queue
		for (const Renderable& renderable : renderableManager.getRenderables())
		{
			// It's valid if one or more renderables inside a renderable manager don't fall into the range processed by this render queue
			// -> At least one renderable should fall into the range processed by this render queue or the render queue is used wrong
			const uint8_t renderQueueIndex = renderable.getRenderQueueIndex();
			if (renderQueueIndex >= mMinimumRenderQueueIndex && renderQueueIndex <= mMaximumRenderQueueIndex)
			{
				// Get the precalculated static part of the sorting key
				// -> Sort renderables back-to-front (for transparency) or front-to-back (for occlusion efficiency)
				// TODO(co) Depending on "mTransparentPass" the sorting key is used
				uint64_t sortingKey = renderable.getSortingKey();

				// The quantized depth is a dynamic part which is set now
				sortingKey = quantizedDepth;	// TODO(co) Just bits influenced

				// Register the renderable inside our renderables queue
				Queue& queue = mQueues[static_cast<size_t>(renderQueueIndex - mMinimumRenderQueueIndex)];
				assert(!queue.sorted);	// Ensure render queue is still in filling state and not already in rendering state
				queue.queuedRenderables.emplace_back(renderable, sortingKey);
			}
		}
	}

	void RenderQueue::fillCommandBuffer(const Renderer::IRenderTarget& renderTarget, Renderer::CommandBuffer& commandBuffer)
	{
		// Begin debug event
		COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(commandBuffer)

		// TODO(co) This is just a dummy implementation

		const MaterialResources& materialResources = mRendererRuntime.getMaterialResourceManager().getMaterialResources();
		const MaterialBlueprintResourceManager& materialBlueprintResourceManager = mRendererRuntime.getMaterialBlueprintResourceManager();
		MaterialBlueprintResource* currentlyBoundMaterialBlueprintResource = nullptr;
		InstanceBufferManager& instanceBufferManager = materialBlueprintResourceManager.getInstanceBufferManager();
		const MaterialTechniqueId materialTechniqueId = "Default";

		// Process all render queues
		// -> When adding renderables from renderable manager we could build up a minimum/maximum used render queue index to sometimes reduce
		//    the number of iterations. On the other hand, there are usually much more renderables added as iterations in here so this possible
		//    optimization might be a fact a performance degeneration while at the same time increasing the code complexity. So, not implemented by intent.
		for (Queue& queue : mQueues)
		{
			QueuedRenderables& queuedRenderables = queue.queuedRenderables;
			if (!queuedRenderables.empty())
			{
				// Sort queued renderables
				if (!queue.sorted && mDoSort)
				{
					// TODO(co) Exploit temporal coherence across frames then use insertion sorts as explained by L. Spiro in
					// http://www.gamedev.net/topic/661114-temporal-coherence-and-render-queue-sorting/?view=findpost&p=5181408
					// Keep a list of sorted indices from the previous frame (one per camera).
					// If we have the sorted list "5, 1, 4, 3, 2, 0":
					// * If it grew from last frame, append: 5, 1, 4, 3, 2, 0, 6, 7 and use insertion sort.
					// * If it's the same, leave it as is, and use insertion sort just in case.
					// * If it's shorter, reset the indices 0, 1, 2, 3, 4; probably use quicksort or other generic sort
					// TODO(co) Use radix sort? ( https://www.quora.com/What-is-the-most-efficient-way-to-sort-a-million-32-bit-integers )
					std::sort(queuedRenderables.begin(), queuedRenderables.end());
					queue.sorted = true;
				}

				// Inject queued renderables into the renderer
				for (const QueuedRenderable& queuedRenderable : queuedRenderables)
				{
					assert(nullptr != queuedRenderable.renderable);
					const Renderable& renderable = *queuedRenderable.renderable;

					// Setup input assembly (IA): Set the used vertex array
					Renderer::IVertexArrayPtr vertexArrayPtr = renderable.getVertexArrayPtr();
					if (nullptr != vertexArrayPtr)
					{
						Renderer::Command::SetVertexArray::create(commandBuffer, vertexArrayPtr);

						// Material resource
						const MaterialResource* materialResource = materialResources.tryGetElementById(renderable.getMaterialResourceId());
						if (nullptr != materialResource)
						{
							MaterialTechnique* materialTechnique = materialResource->getMaterialTechniqueById(materialTechniqueId);
							if (nullptr != materialTechnique)
							{
								MaterialBlueprintResource* materialBlueprintResource = static_cast<MaterialBlueprintResource*>(materialBlueprintResourceManager.tryGetResourceByResourceId(materialTechnique->getMaterialBlueprintResourceId()));
								if (nullptr != materialBlueprintResource && IResource::LoadingState::LOADED == materialBlueprintResource->getLoadingState())
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
													passBufferManager->fillBuffer(renderTarget, worldSpaceToViewSpaceTransform);
												}
											}

											// Bind the material blueprint resource and instance buffer manager to the used renderer
											materialBlueprintResource->fillCommandBuffer(commandBuffer);
											instanceBufferManager.fillCommandBuffer(*materialBlueprintResource, commandBuffer);
										}

										// Cheap state change: Bind the material technique to the used renderer
										materialTechnique->fillCommandBuffer(mRendererRuntime, commandBuffer);

										// Set the used pipeline state object (PSO)
										Renderer::Command::SetPipelineState::create(commandBuffer, pipelineStatePtr);

										{ // Fill the instance buffer manager
											PassBufferManager* passBufferManager = materialBlueprintResource->getPassBufferManager();
											if (nullptr != passBufferManager)
											{
												const MaterialBlueprintResource::UniformBuffer* instanceUniformBuffer = materialBlueprintResource->getInstanceUniformBuffer();
												const MaterialBlueprintResource::TextureBuffer* instanceTextureBuffer = materialBlueprintResource->getInstanceTextureBuffer();
												instanceBufferManager.fillBuffer(*passBufferManager, instanceUniformBuffer, instanceTextureBuffer, renderable.getRenderableManager().getTransform(), *materialTechnique, commandBuffer);
											}
										}

										// Setup input assembly (IA): Set the primitive topology used for draw calls
										Renderer::Command::SetPrimitiveTopology::create(commandBuffer, renderable.getPrimitiveTopology());

										// Render the specified geometric primitive, based on indexing into an array of vertices
										if (renderable.getDrawIndexed())
										{
											Renderer::Command::DrawIndexed::create(commandBuffer, renderable.getNumberOfIndices(), 1, renderable.getStartIndexLocation());
										}
										else
										{
											Renderer::Command::Draw::create(commandBuffer, renderable.getNumberOfIndices(), 1, renderable.getStartIndexLocation());
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
		COMMAND_END_DEBUG_EVENT(commandBuffer)
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
