/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceResource.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorContextData.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeInstance.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeResource.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeResourceManager.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ICompositorPassFactory.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ICompositorResourcePass.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ICompositorInstancePass.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ShadowMap/CompositorInstancePassShadowMap.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ShadowMap/CompositorResourcePassShadowMap.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/InstanceBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/IndirectBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/LightBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/PassBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/Scene/SceneNode.h"
#include "RendererRuntime/Resource/Scene/SceneResource.h"
#include "RendererRuntime/Resource/Scene/Item/Camera/CameraSceneItem.h"
#include "RendererRuntime/Resource/Scene/Item/Mesh/SkeletonMeshSceneItem.h"
#include "RendererRuntime/Resource/Scene/Culling/SceneCullingManager.h"
#include "RendererRuntime/Core/Renderer/FramebufferManager.h"
#include "RendererRuntime/Core/Renderer/RenderTargetTextureManager.h"
#include "RendererRuntime/Vr/IVrManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <algorithm>


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
		mNumberOfMultisamples(1),
		mCurrentlyUsedNumberOfMultisamples(1),
		mResolutionScale(1.0f),
		mRenderTargetWidth(getUninitialized<uint32_t>()),
		mRenderTargetHeight(getUninitialized<uint32_t>()),
		mExecutionRenderTarget(nullptr),
		mCompositorWorkspaceResourceId(getUninitialized<CompositorWorkspaceResourceId>()),
		mFramebufferManagerInitialized(false),
		mCompositorInstancePassShadowMap(nullptr)
	{
		rendererRuntime.getCompositorWorkspaceResourceManager().loadCompositorWorkspaceResourceByAssetId(compositorWorkspaceAssetId, mCompositorWorkspaceResourceId, this);
	}

	CompositorWorkspaceInstance::~CompositorWorkspaceInstance()
	{
		// Cleanup
		destroySequentialCompositorNodeInstances();
	}

	void CompositorWorkspaceInstance::setNumberOfMultisamples(uint8_t numberOfMultisamples)
	{
		// Sanity checks
		assert(numberOfMultisamples == 1 || numberOfMultisamples == 2 || numberOfMultisamples == 4 || numberOfMultisamples == 8);
		assert(numberOfMultisamples <= mRendererRuntime.getRenderer().getCapabilities().maximumNumberOfMultisamples);

		// Set the value
		mNumberOfMultisamples = numberOfMultisamples;
	}

	const CompositorWorkspaceInstance::RenderQueueIndexRange* CompositorWorkspaceInstance::getRenderQueueIndexRangeByRenderQueueIndex(uint8_t renderQueueIndex) const
	{
		for (const RenderQueueIndexRange& renderQueueIndexRange : mRenderQueueIndexRanges)
		{
			if (renderQueueIndex >= renderQueueIndexRange.minimumRenderQueueIndex && renderQueueIndex <= renderQueueIndexRange.maximumRenderQueueIndex)
			{
				return &renderQueueIndexRange;
			}
		}
		return nullptr;
	}

	const ICompositorInstancePass* CompositorWorkspaceInstance::getFirstCompositorInstancePassByCompositorPassTypeId(CompositorPassTypeId compositorPassTypeId) const
	{
		for (const CompositorNodeInstance* compositorNodeInstance : mSequentialCompositorNodeInstances)
		{
			for (const ICompositorInstancePass* compositorInstancePass : compositorNodeInstance->mCompositorInstancePasses)
			{
				if (compositorInstancePass->getCompositorResourcePass().getTypeId() == compositorPassTypeId)
				{
					return compositorInstancePass;
				}
			}
		}

		// No compositor instance pass with the provided compositor pass type ID found
		return nullptr;
	}

	void CompositorWorkspaceInstance::executeVr(Renderer::IRenderTarget& renderTarget, CameraSceneItem* cameraSceneItem, const LightSceneItem* lightSceneItem)
	{
		// Decide whether or not the VR-manager is used for rendering
		IVrManager& vrManager = mRendererRuntime.getVrManager();
		if (vrManager.isRunning())
		{
			// Update the VR-manager just before rendering
			vrManager.updateHmdMatrixPose(cameraSceneItem);

			// Execute the compositor workspace instance
			vrManager.executeCompositorWorkspaceInstance(*this, renderTarget, cameraSceneItem, lightSceneItem);
		}
		else
		{
			// Execute the compositor workspace instance
			execute(renderTarget, cameraSceneItem, lightSceneItem);
		}
	}

	void CompositorWorkspaceInstance::execute(Renderer::IRenderTarget& renderTarget, const CameraSceneItem* cameraSceneItem, const LightSceneItem* lightSceneItem, bool singlePassStereoInstancing)
	{
		// We could directly clear the render queue index ranges renderable managers as soon as the frame rendering has been finished to avoid evil dangling pointers,
		// but on the other hand a responsible user might be interested in the potentially on-screen renderable managers to perform work which should only be performed
		// on potentially on-screen stuff
		// -> Ensure that this clear step is really always performed when calling this execute method (evil dangling alert)
		clearRenderQueueIndexRangesRenderableManagers();

		// Is the compositor workspace resource ready?
		const CompositorWorkspaceResource* compositorWorkspaceResource = mRendererRuntime.getCompositorWorkspaceResourceManager().tryGetById(mCompositorWorkspaceResourceId);
		if (nullptr != compositorWorkspaceResource && compositorWorkspaceResource->getLoadingState() == IResource::LoadingState::LOADED)
		{
			// Tell the global material properties managed by the material blueprint resource manager about the number of multisamples
			// -> Since there can be multiple compositor workspace instances we can't do this once inside "RendererRuntime::CompositorWorkspaceInstance::setNumberOfMultisamples()"
			MaterialBlueprintResourceManager& materialBlueprintResourceManager = mRendererRuntime.getMaterialBlueprintResourceManager();
			materialBlueprintResourceManager.getGlobalMaterialProperties().setPropertyById("GlobalNumberOfMultisamples", MaterialPropertyValue::fromInteger((mNumberOfMultisamples == 1) ? 0 : mNumberOfMultisamples));

			// Add reference to the render target
			renderTarget.addReference();
			mExecutionRenderTarget = &renderTarget;

			// Get the main render target size
			uint32_t renderTargetWidth  = 1;
			uint32_t renderTargetHeight = 1;
			renderTarget.getWidthAndHeight(renderTargetWidth, renderTargetHeight);

			{ // Do we need to destroy previous framebuffers and render target textures?
				bool destroy = false;
				if (mCurrentlyUsedNumberOfMultisamples != mNumberOfMultisamples)
				{
					mCurrentlyUsedNumberOfMultisamples = mNumberOfMultisamples;
					destroy = true;
				}
				{
					const uint32_t currentRenderTargetWidth  = static_cast<uint32_t>(static_cast<float>(renderTargetWidth) * mResolutionScale);
					const uint32_t currentRenderTargetHeight = static_cast<uint32_t>(static_cast<float>(renderTargetHeight) * mResolutionScale);
					if (mRenderTargetWidth != currentRenderTargetWidth || mRenderTargetHeight != currentRenderTargetHeight)
					{
						mRenderTargetWidth  = currentRenderTargetWidth;
						mRenderTargetHeight = currentRenderTargetHeight;
						destroy = true;
					}
				}
				if (destroy)
				{
					destroyFramebuffersAndRenderTargetTextures();
				}
			}

			// Create framebuffers and render target textures, if required
			if (!mFramebufferManagerInitialized)
			{
				createFramebuffersAndRenderTargetTextures(renderTarget);
			}

			// Begin scene rendering
			// -> Required for Direct3D 9 and Direct3D 12
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 3
			Renderer::IRenderer& renderer = renderTarget.getRenderer();
			if (renderer.beginScene())
			{
				const CompositorContextData compositorContextData(this, cameraSceneItem, singlePassStereoInstancing, lightSceneItem, mCompositorInstancePassShadowMap);
				if (nullptr != cameraSceneItem)
				{
					// Gather render queue index ranges renderable managers
					cameraSceneItem->getSceneResource().getSceneCullingManager().gatherRenderQueueIndexRangesRenderableManagers(renderTarget, compositorContextData, mRenderQueueIndexRanges);

					// Fill the light buffer manager
					materialBlueprintResourceManager.getLightBufferManager().fillBuffer(cameraSceneItem->getSceneResource(), mCommandBuffer);
				}

				// Begin debug event
				COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(mCommandBuffer)

				// Set the current render target
				Renderer::Command::SetRenderTarget::create(mCommandBuffer, &renderTarget);

				// Set the viewport and scissor rectangle
				// -> Since Direct3D 12 is command list based, the viewport and scissor rectangle must be set in every draw call to work with all supported renderer APIs
				Renderer::Command::SetViewportAndScissorRectangle::create(mCommandBuffer, 0, 0, renderTargetWidth, renderTargetHeight);

				{ // Fill command buffer
					Renderer::IRenderTarget* currentRenderTarget = &renderTarget;
					for (const CompositorNodeInstance* compositorNodeInstance : mSequentialCompositorNodeInstances)
					{
						currentRenderTarget = &compositorNodeInstance->fillCommandBuffer(*currentRenderTarget, compositorContextData, mCommandBuffer);
					}
				}

				// End debug event
				COMMAND_END_DEBUG_EVENT(mCommandBuffer)

				{ // Submit command buffer to the renderer backend
					// The command buffer is about to be submitted, inform everyone who cares about this
					materialBlueprintResourceManager.onPreCommandBufferExecution();

					// Submit command buffer to the renderer backend
					mCommandBuffer.submitAndClear(renderer);

					// The command buffer has been submitted, inform everyone who cares about this
					for (const CompositorNodeInstance* compositorNodeInstance : mSequentialCompositorNodeInstances)
					{
						compositorNodeInstance->onPostCommandBufferExecution();
					}
					{
						const uint32_t numberOfResources = materialBlueprintResourceManager.getNumberOfResources();
						for (uint32_t i = 0; i < numberOfResources; ++i)
						{
							PassBufferManager* passBufferManager = materialBlueprintResourceManager.getByIndex(i).getPassBufferManager();
							if (nullptr != passBufferManager)
							{
								passBufferManager->onPostCommandBufferExecution();
							}
						}
					}
				}

				// End scene rendering
				// -> Required for Direct3D 9 and Direct3D 12
				// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 3
				renderer.endScene();
			}

			// In case the render target is a swap chain, present the content of the current back buffer
			if (renderTarget.getResourceType() == Renderer::ResourceType::SWAP_CHAIN)
			{
				static_cast<Renderer::ISwapChain&>(renderTarget).present();
			}

			// Release reference from the render target
			mExecutionRenderTarget = nullptr;
			renderTarget.releaseReference();
		}
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::IResourceListener methods ]
	//[-------------------------------------------------------]
	void CompositorWorkspaceInstance::onLoadingStateChange(const IResource& resource)
	{
		// Destroy the previous stuff
		destroySequentialCompositorNodeInstances();

		// Handle loaded state
		if (resource.getLoadingState() == IResource::LoadingState::LOADED)
		{
			// TODO(co) Just a first test, need to complete and refine the implementation
			CompositorNodeResourceManager& compositorNodeResourceManager = mRendererRuntime.getCompositorNodeResourceManager();
			const ICompositorPassFactory& compositorPassFactory = compositorNodeResourceManager.getCompositorPassFactory();
			RenderTargetTextureManager& renderTargetTextureManager = mRendererRuntime.getCompositorWorkspaceResourceManager().getRenderTargetTextureManager();
			FramebufferManager& framebufferManager = mRendererRuntime.getCompositorWorkspaceResourceManager().getFramebufferManager();

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
				CompositorNodeResourceId compositorNodeResourceId = getUninitialized<CompositorNodeResourceId>();
				compositorNodeResourceManager.loadCompositorNodeResourceByAssetId(compositorNodeAssetIds[nodeIndex], compositorNodeResourceId);
				CompositorNodeResource& compositorNodeResource = compositorNodeResourceManager.getById(compositorNodeResourceId);

				// TODO(co) Ensure compositor node resource loading is done. Such blocking waiting is no good thing.
				compositorNodeResource.enforceFullyLoaded();

				// Add render target textures and framebuffers (doesn't directly allocate renderer resources, just announces them)
				for (const CompositorRenderTargetTexture& compositorRenderTargetTexture : compositorNodeResource.getRenderTargetTextures())
				{
					renderTargetTextureManager.addRenderTargetTexture(compositorRenderTargetTexture.getAssetId(), compositorRenderTargetTexture.getRenderTargetTextureSignature());
				}
				for (const CompositorFramebuffer& compositorFramebuffer : compositorNodeResource.getFramebuffers())
				{
					framebufferManager.addFramebuffer(compositorFramebuffer.getCompositorFramebufferId(), compositorFramebuffer.getFramebufferSignature());
				}

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
									// Create the compositor instance pass
									ICompositorInstancePass* compositorInstancePass = compositorPassFactory.createCompositorInstancePass(*compositorResourcePass, *compositorNodeInstance);
									if (compositorResourcePass->getTypeId() == CompositorResourcePassShadowMap::TYPE_ID)
									{
										assert(nullptr == mCompositorInstancePassShadowMap);
										mCompositorInstancePassShadowMap = static_cast<CompositorInstancePassShadowMap*>(compositorInstancePass);
									}
									compositorNodeInstance->mCompositorInstancePasses.push_back(compositorInstancePass);

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

			// Tell all compositor node instances that the compositor workspace instance loading has been finished
			for (const CompositorNodeInstance* compositorNodeInstance : mSequentialCompositorNodeInstances)
			{
				compositorNodeInstance->compositorWorkspaceInstanceLoadingFinished();
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
		mCompositorInstancePassShadowMap = nullptr;

		// Destroy framebuffers and render target textures
		destroyFramebuffersAndRenderTargetTextures();
	}

	void CompositorWorkspaceInstance::createFramebuffersAndRenderTargetTextures(const Renderer::IRenderTarget& mainRenderTarget)
	{
		assert(!mFramebufferManagerInitialized);
		FramebufferManager& framebufferManager = mRendererRuntime.getCompositorWorkspaceResourceManager().getFramebufferManager();
		for (const CompositorNodeInstance* compositorNodeInstance : mSequentialCompositorNodeInstances)
		{
			for (ICompositorInstancePass* compositorInstancePass : compositorNodeInstance->mCompositorInstancePasses)
			{
				const CompositorFramebufferId compositorFramebufferId = compositorInstancePass->getCompositorResourcePass().getCompositorTarget().getCompositorFramebufferId();
				if (isInitialized(compositorFramebufferId))
				{
					compositorInstancePass->mRenderTarget = framebufferManager.getFramebufferByCompositorFramebufferId(compositorFramebufferId, mainRenderTarget, mCurrentlyUsedNumberOfMultisamples, mResolutionScale);
				}
			}
		}
		mFramebufferManagerInitialized = true;
	}

	void CompositorWorkspaceInstance::destroyFramebuffersAndRenderTargetTextures()
	{
		// All compositor instance passes need to forget about the render targets
		for (CompositorNodeInstance* compositorNodeInstance : mSequentialCompositorNodeInstances)
		{
			for (ICompositorInstancePass* compositorInstancePass : compositorNodeInstance->mCompositorInstancePasses)
			{
				compositorInstancePass->mRenderTarget = nullptr;
				compositorInstancePass->mNumberOfExecutionRequests = 0;
			}
		}

		// Destroy renderer resources of framebuffers and render target textures
		CompositorWorkspaceResourceManager& compositorWorkspaceResourceManager = mRendererRuntime.getCompositorWorkspaceResourceManager();
		compositorWorkspaceResourceManager.getFramebufferManager().clearRendererResources();
		compositorWorkspaceResourceManager.getRenderTargetTextureManager().clearRendererResources();
		mFramebufferManagerInitialized = false;
	}

	void CompositorWorkspaceInstance::clearRenderQueueIndexRangesRenderableManagers()
	{
		// Forget about all previously gathered renderable managers
		for (RenderQueueIndexRange& renderQueueIndexRange : mRenderQueueIndexRanges)
		{
			renderQueueIndexRange.renderableManagers.clear();
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
