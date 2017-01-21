/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
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
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceResourceManager.h"
#include "RendererRuntime/Resource/CompositorWorkspace/Loader/CompositorWorkspaceResourceLoader.h"
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/Core/Renderer/FramebufferManager.h"
#include "RendererRuntime/Core/Renderer/RenderTargetTextureManager.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	CompositorWorkspaceResourceId CompositorWorkspaceResourceManager::loadCompositorWorkspaceResourceByAssetId(AssetId assetId, IResourceListener* resourceListener, bool reload)
	{
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		if (nullptr != asset)
		{
			// Get or create the instance
			CompositorWorkspaceResource* compositorWorkspaceResource = nullptr;
			{
				const uint32_t numberOfElements = mCompositorWorkspaceResources.getNumberOfElements();
				for (uint32_t i = 0; i < numberOfElements; ++i)
				{
					CompositorWorkspaceResource& currentCompositorWorkspaceResource = mCompositorWorkspaceResources.getElementByIndex(i);
					if (currentCompositorWorkspaceResource.getAssetId() == assetId)
					{
						compositorWorkspaceResource = &currentCompositorWorkspaceResource;

						// Get us out of the loop
						i = numberOfElements;
					}
				}
			}

			// Create the resource instance
			bool load = reload;
			if (nullptr == compositorWorkspaceResource)
			{
				compositorWorkspaceResource = &mCompositorWorkspaceResources.addElement();
				compositorWorkspaceResource->setResourceManager(this);
				compositorWorkspaceResource->setAssetId(assetId);
				load = true;
			}
			if (nullptr != compositorWorkspaceResource && nullptr != resourceListener)
			{
				compositorWorkspaceResource->connectResourceListener(*resourceListener);
			}

			// Load the resource, if required
			if (load)
			{
				// Prepare the resource loader
				CompositorWorkspaceResourceLoader* compositorWorkspaceResourceLoader = static_cast<CompositorWorkspaceResourceLoader*>(acquireResourceLoaderInstance(CompositorWorkspaceResourceLoader::TYPE_ID));
				compositorWorkspaceResourceLoader->initialize(*asset, *compositorWorkspaceResource);

				// Commit resource streamer asset load request
				ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
				resourceStreamerLoadRequest.resource = compositorWorkspaceResource;
				resourceStreamerLoadRequest.resourceLoader = compositorWorkspaceResourceLoader;
				mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);
			}

			// Done
			return compositorWorkspaceResource->getId();
		}

		// Error!
		return getUninitialized<CompositorWorkspaceResourceId>();
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void CompositorWorkspaceResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		const uint32_t numberOfElements = mCompositorWorkspaceResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			if (mCompositorWorkspaceResources.getElementByIndex(i).getAssetId() == assetId)
			{
				loadCompositorWorkspaceResourceByAssetId(assetId, nullptr, true);
				break;
			}
		}
	}

	void CompositorWorkspaceResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	CompositorWorkspaceResourceManager::CompositorWorkspaceResourceManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mRenderTargetTextureManager(new RenderTargetTextureManager(mRendererRuntime)),
		mFramebufferManager(new FramebufferManager(*mRenderTargetTextureManager))
	{
		// Nothing here
	}

	CompositorWorkspaceResourceManager::~CompositorWorkspaceResourceManager()
	{
		delete mFramebufferManager;
		delete mRenderTargetTextureManager;
	}

	IResourceLoader* CompositorWorkspaceResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		IResourceLoader* resourceLoader = IResourceManager::acquireResourceLoaderInstance(resourceLoaderTypeId);

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			// We only support our own compositor workspace format
			assert(resourceLoaderTypeId == CompositorWorkspaceResourceLoader::TYPE_ID);
			resourceLoader = new CompositorWorkspaceResourceLoader(*this, mRendererRuntime);
			mUsedResourceLoaderInstances.push_back(resourceLoader);
		}

		// Done
		return resourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
