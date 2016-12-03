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
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceInstance.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceResourceManager.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeInstance.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeResource.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeResourceManager.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ICompositorPassFactory.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ICompositorResourcePass.h"
#include "RendererRuntime/RenderQueue/IndirectBufferManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <Renderer/Public/Renderer.h>


// Disable warnings
// TODO(co) See "RendererRuntime::CompositorWorkspaceInstance::CompositorWorkspaceInstance()": How the heck should we avoid such a situation without using complicated solutions like a pointer to an instance? (= more individual allocations/deallocations)
#pragma warning(disable: 4355)	// warning C4355: 'this': used in base member initializer list


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	CompositorWorkspaceInstance::CompositorWorkspaceInstance(IRendererRuntime& rendererRuntime, AssetId compositorWorkspaceAssetId) :
		mRendererRuntime(rendererRuntime),
		mIndirectBufferManager(*(new IndirectBufferManager(rendererRuntime))),
		mExecutionRenderTarget(nullptr),
		mCompositorWorkspaceResourceId(rendererRuntime.getCompositorWorkspaceResourceManager().loadCompositorWorkspaceResourceByAssetId(compositorWorkspaceAssetId, this))
	{
		// Nothing here
	}

	CompositorWorkspaceInstance::~CompositorWorkspaceInstance()
	{
		// Cleanup
		destroySequentialCompositorNodeInstances();
		delete &mIndirectBufferManager;
	}

	void CompositorWorkspaceInstance::execute(Renderer::IRenderTarget& renderTarget, CameraSceneItem* cameraSceneItem)
	{
		// Is the compositor workspace resource ready?
		const CompositorWorkspaceResource* compositorWorkspaceResource = mRendererRuntime.getCompositorWorkspaceResourceManager().getCompositorWorkspaceResources().tryGetElementById(mCompositorWorkspaceResourceId);
		if (nullptr != compositorWorkspaceResource && compositorWorkspaceResource->getLoadingState() == IResource::LoadingState::LOADED)
		{
			// Add reference to the render target
			renderTarget.addReference();
			mExecutionRenderTarget = &renderTarget;

			// Begin scene rendering
			// -> Required for Direct3D 9 and Direct3D 12
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			Renderer::IRenderer& renderer = renderTarget.getRenderer();
			if (renderer.beginScene())
			{
				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

				// Make the main swap chain to the current render target
				renderer.omSetRenderTarget(&renderTarget);

				{ // Since Direct3D 12 is command list based, the viewport and scissor rectangle must be set in every draw call to work with all supported renderer APIs
					// Get the window size
					uint32_t width  = 0;
					uint32_t height = 0;
					renderTarget.getWidthAndHeight(width, height);

					// Set the viewport and scissor rectangle
					renderer.rsSetViewportAndScissorRectangle(0, 0, width, height);
				}

				// Draw
				for (const CompositorNodeInstance* compositorNodeInstance : mSequentialCompositorNodeInstances)
				{
					compositorNodeInstance->execute(cameraSceneItem);
				}

				// End scene rendering
				// -> Required for Direct3D 9 and Direct3D 12
				// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
				renderer.endScene();

				// The frame has ended, inform everyone who cares about this
				for (const CompositorNodeInstance* compositorNodeInstance : mSequentialCompositorNodeInstances)
				{
					compositorNodeInstance->frameEnded();
				}
				mIndirectBufferManager.freeAllUsedIndirectBuffers();
			}

			// End debug event
			RENDERER_END_DEBUG_EVENT(&renderer)

			// In case the render target is a swap chain, present the content of the current back buffer
			if (renderTarget.getResourceType() == Renderer::ResourceType::SWAP_CHAIN)
			{
				static_cast<Renderer::ISwapChain&>(renderTarget).present();
			}

			// Release reference from the render target
			mExecutionRenderTarget = nullptr;
			renderTarget.release();
		}
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::IResourceListener methods ]
	//[-------------------------------------------------------]
	void CompositorWorkspaceInstance::onLoadingStateChange(const IResource& resource)
	{
		if (resource.getLoadingState() == IResource::LoadingState::LOADED)
		{
			// TODO(co) Just a first test
			CompositorNodeResourceManager& compositorNodeResourceManager = mRendererRuntime.getCompositorNodeResourceManager();
			const ICompositorPassFactory& compositorPassFactory = compositorNodeResourceManager.getCompositorPassFactory();

			destroySequentialCompositorNodeInstances();

			// For render queue index ranges gathering and merging
			typedef std::pair<uint8_t, uint8_t> LocalRenderQueueIndexRange;
			typedef std::vector<LocalRenderQueueIndexRange> LocalRenderQueueIndexRanges;
			LocalRenderQueueIndexRanges individualRenderQueueIndexRanges;

			// Compositor node resources
			const CompositorWorkspaceResource::CompositorNodeAssetIds& compositorNodeAssetIds = static_cast<const CompositorWorkspaceResource&>(resource).getCompositorNodeAssetIds();
			const size_t numberOfCompositorResourceNodes = compositorNodeAssetIds.size();
			for (size_t nodeIndex = 0; nodeIndex < numberOfCompositorResourceNodes; ++nodeIndex)
			{
				// Get the compositor node resource instance
				const CompositorNodeResourceId compositorNodeResourceId = compositorNodeResourceManager.loadCompositorNodeResourceByAssetId(compositorNodeAssetIds[nodeIndex]);
				CompositorNodeResource& compositorNodeResource = compositorNodeResourceManager.getCompositorNodeResources().getElementById(compositorNodeResourceId);

				// TODO(co) Ensure compositor node resource loading is done. Such blocking waiting is no good thing.
				compositorNodeResource.enforceFullyLoaded();

				// Create the compositor node instance
				CompositorNodeInstance* compositorNodeInstance = new CompositorNodeInstance(compositorNodeResourceId, *this);
				mSequentialCompositorNodeInstances.push_back(compositorNodeInstance);

				{ // Compositor node resource targets
					const CompositorNodeResource::CompositorTargets& compositorTargets = compositorNodeResource.getCompositorTargets();
					const size_t numberOfCompositorTargets = compositorTargets.size();
					for (size_t targetIndex = 0; targetIndex < numberOfCompositorTargets; ++targetIndex)
					{
						// Get the compositor node resource target instance
						const CompositorTarget& compositorTarget = compositorTargets[targetIndex];

						{ // Compositor node resource target passes
							const CompositorTarget::CompositorResourcePasses& compositorResourcePasses = compositorTarget.getCompositorResourcePasses();
							const size_t numberOfCompositorResourcePasses = compositorResourcePasses.size();
							for (size_t passIndex = 0; passIndex < numberOfCompositorResourcePasses; ++passIndex)
							{
								// Get the compositor resource target instance
								const ICompositorResourcePass* compositorResourcePass = compositorResourcePasses[passIndex];

								// Create the compositor instance pass
								if (nullptr != compositorResourcePass)
								{
									compositorNodeInstance->mCompositorInstancePasses.push_back(compositorPassFactory.createCompositorInstancePass(*compositorResourcePass, *compositorNodeInstance));

									// Gather render queue index range
									uint8_t minimumRenderQueueIndex = 0;
									uint8_t maximumRenderQueueIndex = 0;
									if (compositorResourcePass->getRenderQueueIndexRange(minimumRenderQueueIndex, maximumRenderQueueIndex))
									{
										individualRenderQueueIndexRanges.emplace_back(minimumRenderQueueIndex, maximumRenderQueueIndex);
									}
								}
							}
						}
					}
				}
			}

			// Merge the render queue index ranges using the algorithm described at http://stackoverflow.com/a/5276789
			if (!individualRenderQueueIndexRanges.empty())
			{
				LocalRenderQueueIndexRanges mergedRenderQueueIndexRanges;
				mergedRenderQueueIndexRanges.reserve(individualRenderQueueIndexRanges.size());
				std::sort(individualRenderQueueIndexRanges.begin(), individualRenderQueueIndexRanges.end());
				LocalRenderQueueIndexRanges::iterator iterator = individualRenderQueueIndexRanges.begin();
				LocalRenderQueueIndexRange current = *(iterator)++;
				while (iterator != individualRenderQueueIndexRanges.end())
				{
					if (current.second >= iterator->first)
					{
						current.second = std::max(current.second, iterator->second);
					}
					else
					{
						mergedRenderQueueIndexRanges.push_back(current);
						current = *iterator;
					}
					++iterator;
				}
				mergedRenderQueueIndexRanges.push_back(current);

				// Fill our final render queue index ranges data structure
				mRenderQueueIndexRanges.reserve(mergedRenderQueueIndexRanges.size());
				for (const LocalRenderQueueIndexRange& localRenderQueueIndexRange : mergedRenderQueueIndexRanges)
				{
					mRenderQueueIndexRanges.emplace_back(localRenderQueueIndexRange.first, localRenderQueueIndexRange.second);
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void CompositorWorkspaceInstance::destroySequentialCompositorNodeInstances()
	{
		for (CompositorNodeInstance* compositorNodeInstance : mSequentialCompositorNodeInstances)
		{
			delete compositorNodeInstance;
		}
		mSequentialCompositorNodeInstances.clear();
		mRenderQueueIndexRanges.clear();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
