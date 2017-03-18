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
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	template <class TYPE, class LOADER_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS>
	inline ResourceManagerTemplate<TYPE, LOADER_TYPE, ID_TYPE, MAXIMUM_NUMBER_OF_ELEMENTS>::ResourceManagerTemplate(IRendererRuntime& rendererRuntime, IResourceManager& resourceManager) :
		ResourceManagerTemplateBase(rendererRuntime, resourceManager)
	{
		// Nothing here
	}

	template <class TYPE, class LOADER_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS>
	inline ResourceManagerTemplate<TYPE, LOADER_TYPE, ID_TYPE, MAXIMUM_NUMBER_OF_ELEMENTS>::~ResourceManagerTemplate()
	{
		// Nothing here
	}

	template <class TYPE, class LOADER_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS>
	inline LOADER_TYPE* ResourceManagerTemplate<TYPE, LOADER_TYPE, ID_TYPE, MAXIMUM_NUMBER_OF_ELEMENTS>::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		LOADER_TYPE* resourceLoader = static_cast<LOADER_TYPE*>(ResourceManagerTemplateBase::acquireResourceLoaderInstance(resourceLoaderTypeId));

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			// We only support our own format
			assert(resourceLoaderTypeId == LOADER_TYPE::TYPE_ID);
			resourceLoader = new LOADER_TYPE(mResourceManager, mRendererRuntime);
			mUsedResourceLoaderInstances.push_back(resourceLoader);
		}

		// Done
		return resourceLoader;
	}

	template <class TYPE, class LOADER_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS>
	TYPE* ResourceManagerTemplate<TYPE, LOADER_TYPE, ID_TYPE, MAXIMUM_NUMBER_OF_ELEMENTS>::getResourceByAssetId(AssetId assetId) const
	{
		// TODO(co) Implement more efficient solution later on
		const uint32_t numberOfElements = mResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			TYPE& resource = mResources.getElementByIndex(i);
			if (resource.getAssetId() == assetId)
			{
				return &resource;
			}
		}

		// There's no resource using the given asset ID
		return nullptr;
	}

	template <class TYPE, class LOADER_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS>
	inline TYPE& ResourceManagerTemplate<TYPE, LOADER_TYPE, ID_TYPE, MAXIMUM_NUMBER_OF_ELEMENTS>::createEmptyResourceByAssetId(AssetId assetId)
	{
		// Resource is not allowed to exist, yet
		assert(nullptr == getResourceByAssetId(assetId));

		// Create the resource instance
		TYPE& resource = mResources.addElement();
		resource.setResourceManager(&mResourceManager);
		resource.setAssetId(assetId);

		// Done
		return resource;
	}

	template <class TYPE, class LOADER_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS>
	inline void ResourceManagerTemplate<TYPE, LOADER_TYPE, ID_TYPE, MAXIMUM_NUMBER_OF_ELEMENTS>::loadResourceByAssetId(AssetId assetId, ID_TYPE& resourceId, IResourceListener* resourceListener, bool reload)
	{
		// Get or create the instance
		TYPE* resource = getResourceByAssetId(assetId);

		// Create the resource instance
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		bool load = (reload && nullptr != asset);
		if (nullptr == resource && nullptr != asset)
		{
			resource = &mResources.addElement();
			resource->setResourceManager(&mResourceManager);
			resource->setAssetId(assetId);
			load = true;
		}

		// Before connecting a resource listener, ensure we set the output resource ID at once so it can already directly be used inside the resource listener
		if (nullptr != resource)
		{
			resourceId = resource->getId();
			if (nullptr != resourceListener)
			{
				resource->connectResourceListener(*resourceListener);
			}
		}
		else
		{
			resourceId = getUninitialized<ID_TYPE>();
		}

		// Load the resource, if required
		if (load)
		{
			// Prepare the resource loader
			LOADER_TYPE* resourceLoader = acquireResourceLoaderInstance(LOADER_TYPE::TYPE_ID);
			resourceLoader->initialize(*asset, *resource);

			// Commit resource streamer asset load request
			ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
			resourceStreamerLoadRequest.resource = resource;
			resourceStreamerLoadRequest.resourceLoader = resourceLoader;
			mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);
		}
	}

	template <class TYPE, class LOADER_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS>
	inline void ResourceManagerTemplate<TYPE, LOADER_TYPE, ID_TYPE, MAXIMUM_NUMBER_OF_ELEMENTS>::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		const uint32_t numberOfElements = mResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			if (mResources.getElementByIndex(i).getAssetId() == assetId)
			{
				ID_TYPE resourceId = getUninitialized<ID_TYPE>();
				loadResourceByAssetId(assetId, resourceId, nullptr, true);
				break;
			}
		}
	}

	template <class TYPE, class LOADER_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS>
	inline typename ResourceManagerTemplate<TYPE, LOADER_TYPE, ID_TYPE, MAXIMUM_NUMBER_OF_ELEMENTS>::Resources& ResourceManagerTemplate<TYPE, LOADER_TYPE, ID_TYPE, MAXIMUM_NUMBER_OF_ELEMENTS>::getResources()
	{
		return mResources;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime