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
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeResourceManager.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/CompositorPassFactory.h"
#include "RendererRuntime/Resource/CompositorNode/Loader/CompositorNodeResourceLoader.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceResourceManager.h"
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/Core/Renderer/FramebufferManager.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global variables                                      ]
		//[-------------------------------------------------------]
		static const RendererRuntime::CompositorPassFactory defaultCompositorPassFactory;


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
	CompositorNodeResourceId CompositorNodeResourceManager::loadCompositorNodeResourceByAssetId(AssetId assetId, IResourceListener* resourceListener, bool reload)
	{
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		if (nullptr != asset)
		{
			// Get or create the instance
			CompositorNodeResource* compositorNodeResource = nullptr;
			{
				const uint32_t numberOfElements = mCompositorNodeResources.getNumberOfElements();
				for (uint32_t i = 0; i < numberOfElements; ++i)
				{
					CompositorNodeResource& currentCompositorNodeResource = mCompositorNodeResources.getElementByIndex(i);
					if (currentCompositorNodeResource.getAssetId() == assetId)
					{
						compositorNodeResource = &currentCompositorNodeResource;

						// Get us out of the loop
						i = numberOfElements;
					}
				}
			}

			// Create the resource instance
			bool load = reload;
			if (nullptr == compositorNodeResource)
			{
				compositorNodeResource = &mCompositorNodeResources.addElement();
				compositorNodeResource->setResourceManager(this);
				compositorNodeResource->setAssetId(assetId);
				load = true;
			}
			if (nullptr != compositorNodeResource && nullptr != resourceListener)
			{
				compositorNodeResource->connectResourceListener(*resourceListener);
			}

			// Load the resource, if required
			if (load)
			{
				// Prepare the resource loader
				CompositorNodeResourceLoader* compositorNodeResourceLoader = static_cast<CompositorNodeResourceLoader*>(acquireResourceLoaderInstance(CompositorNodeResourceLoader::TYPE_ID));
				compositorNodeResourceLoader->initialize(*asset, *compositorNodeResource);

				// Commit resource streamer asset load request
				ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
				resourceStreamerLoadRequest.resource = compositorNodeResource;
				resourceStreamerLoadRequest.resourceLoader = compositorNodeResourceLoader;
				mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);
			}

			// Done
			return compositorNodeResource->getId();
		}

		// Error!
		return getUninitialized<CompositorNodeResourceId>();
	}

	void CompositorNodeResourceManager::setCompositorPassFactory(const ICompositorPassFactory* compositorPassFactory)
	{
		// There must always be a valid compositor pass factory instance
		mCompositorPassFactory = (nullptr != compositorPassFactory) ? compositorPassFactory : &::detail::defaultCompositorPassFactory;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void CompositorNodeResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		const uint32_t numberOfElements = mCompositorNodeResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			if (mCompositorNodeResources.getElementByIndex(i).getAssetId() == assetId)
			{
				loadCompositorNodeResourceByAssetId(assetId, nullptr, true);

				{ // Reload all compositor workspace resources using this compositor node resource
					CompositorWorkspaceResourceManager& compositorWorkspaceResourceManager = mRendererRuntime.getCompositorWorkspaceResourceManager();
					const CompositorWorkspaceResources& compositorWorkspaceResources = compositorWorkspaceResourceManager.getCompositorWorkspaceResources();
					const uint32_t numberOfCompositorWorkspaceResources = compositorWorkspaceResources.getNumberOfElements();
					for (uint32_t compositorWorkspaceResourceIndex = 0; compositorWorkspaceResourceIndex < numberOfCompositorWorkspaceResources; ++compositorWorkspaceResourceIndex)
					{
						const CompositorWorkspaceResource& compositorWorkspaceResource = compositorWorkspaceResources.getElementByIndex(compositorWorkspaceResourceIndex);
						const CompositorWorkspaceResource::CompositorNodeAssetIds& compositorNodeAssetIds = compositorWorkspaceResource.getCompositorNodeAssetIds();
						for (AssetId currentAssetId : compositorNodeAssetIds)
						{
							if (currentAssetId == assetId)
							{
								compositorWorkspaceResourceManager.reloadResourceByAssetId(compositorWorkspaceResource.getAssetId());
								break;
							}
						}
					}
				}

				break;
			}
		}
	}

	void CompositorNodeResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	CompositorNodeResourceManager::CompositorNodeResourceManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mCompositorPassFactory(&::detail::defaultCompositorPassFactory),
		mFramebufferManager(new FramebufferManager(mRendererRuntime.getRenderer()))
	{
		// Nothing here
	}

	CompositorNodeResourceManager::~CompositorNodeResourceManager()
	{
		delete mFramebufferManager;
	}

	IResourceLoader* CompositorNodeResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		IResourceLoader* resourceLoader = IResourceManager::acquireResourceLoaderInstance(resourceLoaderTypeId);

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			// We only support our own compositor format
			assert(resourceLoaderTypeId == CompositorNodeResourceLoader::TYPE_ID);
			resourceLoader = new CompositorNodeResourceLoader(*this, mRendererRuntime);
			mUsedResourceLoaderInstances.push_back(resourceLoader);
		}

		// Done
		return resourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
