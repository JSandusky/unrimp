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
#include "RendererRuntime/Resource/Compositor/CompositorResourceManager.h"
#include "RendererRuntime/Resource/Compositor/CompositorResource.h"
#include "RendererRuntime/Resource/Compositor/Pass/CompositorPassFactory.h"
#include "RendererRuntime/Resource/Compositor/Loader/CompositorResourceLoader.h"
#include "RendererRuntime/Resource/ResourceStreamer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <assert.h>


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
	CompositorResourceId CompositorResourceManager::loadCompositorResourceByAssetId(AssetId assetId, bool reload, IResourceListener* resourceListener)
	{
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		if (nullptr != asset)
		{
			// Get or create the instance
			CompositorResource* compositorResource = nullptr;
			{
				const uint32_t numberOfElements = mCompositorResources.getNumberOfElements();
				for (uint32_t i = 0; i < numberOfElements; ++i)
				{
					CompositorResource& currentCompositorResource = mCompositorResources.getElementByIndex(i);
					if (currentCompositorResource.getAssetId() == assetId)
					{
						compositorResource = &currentCompositorResource;

						// Get us out of the loop
						i = numberOfElements;
					}
				}
			}

			// Create the resource instance
			bool load = reload;
			if (nullptr == compositorResource)
			{
				compositorResource = &mCompositorResources.addElement();
				compositorResource->setAssetId(assetId);
				compositorResource->setResourceListener(resourceListener);
				load = true;
			}

			// Load the resource, if required
			if (load)
			{
				// Prepare the resource loader
				CompositorResourceLoader* compositorResourceLoader = static_cast<CompositorResourceLoader*>(acquireResourceLoaderInstance(CompositorResourceLoader::TYPE_ID));
				compositorResourceLoader->initialize(*asset, *compositorResource);

				// Commit resource streamer asset load request
				ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
				resourceStreamerLoadRequest.resource = compositorResource;
				resourceStreamerLoadRequest.resourceLoader = compositorResourceLoader;
				mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);
			}

			// Done
			return compositorResource->getId();
		}

		// Error!
		return getUninitialized<CompositorResourceId>();
	}

	void CompositorResourceManager::setCompositorPassFactory(const ICompositorPassFactory* compositorPassFactory)
	{
		// There must always be a valid compositor pass factory instance
		mCompositorPassFactory = (nullptr != compositorPassFactory) ? compositorPassFactory : &::detail::defaultCompositorPassFactory;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void CompositorResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		const uint32_t numberOfElements = mCompositorResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			if (mCompositorResources.getElementByIndex(i).getAssetId() == assetId)
			{
				loadCompositorResourceByAssetId(assetId, true);
				break;
			}
		}
	}

	void CompositorResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	CompositorResourceManager::CompositorResourceManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mCompositorPassFactory(&::detail::defaultCompositorPassFactory)
	{
		// Nothing in here
	}

	IResourceLoader* CompositorResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		IResourceLoader* resourceLoader = IResourceManager::acquireResourceLoaderInstance(resourceLoaderTypeId);

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			// We only support our own compositor format
			assert(resourceLoaderTypeId == CompositorResourceLoader::TYPE_ID);
			resourceLoader = new CompositorResourceLoader(*this, mRendererRuntime);
			mUsedResourceLoaderInstances.push_back(resourceLoader);
		}

		// Done
		return resourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
