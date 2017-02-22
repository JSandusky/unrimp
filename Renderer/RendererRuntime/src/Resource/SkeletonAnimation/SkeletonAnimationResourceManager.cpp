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
#include "RendererRuntime/Resource/SkeletonAnimation/SkeletonAnimationResourceManager.h"
#include "RendererRuntime/Resource/SkeletonAnimation/Loader/SkeletonAnimationResourceLoader.h"
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <unordered_set>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	// TODO(co) Work-in-progress
	SkeletonAnimationResource* SkeletonAnimationResourceManager::getSkeletonAnimationResourceByAssetId(AssetId assetId) const
	{
		const uint32_t numberOfElements = mSkeletonAnimationResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			SkeletonAnimationResource& skeletonAnimationResource = mSkeletonAnimationResources.getElementByIndex(i);
			if (skeletonAnimationResource.getAssetId() == assetId)
			{
				return &skeletonAnimationResource;
			}
		}

		// There's no skeleton animation resource using the given asset ID
		return nullptr;
	}

	SkeletonAnimationResourceId SkeletonAnimationResourceManager::loadSkeletonAnimationResourceByAssetId(AssetId assetId, IResourceListener* resourceListener, bool reload)
	{
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		if (nullptr != asset)
		{
			// Get or create the instance
			SkeletonAnimationResource* skeletonAnimationResource = getSkeletonAnimationResourceByAssetId(assetId);

			// Create the resource instance
			bool load = reload;
			if (nullptr == skeletonAnimationResource)
			{
				skeletonAnimationResource = &mSkeletonAnimationResources.addElement();
				skeletonAnimationResource->setResourceManager(this);
				skeletonAnimationResource->setAssetId(assetId);
				load = true;
			}
			if (nullptr != skeletonAnimationResource && nullptr != resourceListener)
			{
				skeletonAnimationResource->connectResourceListener(*resourceListener);
			}

			// Load the resource, if required
			if (load)
			{
				// Prepare the resource loader
				SkeletonAnimationResourceLoader* skeletonAnimationResourceLoader = static_cast<SkeletonAnimationResourceLoader*>(acquireResourceLoaderInstance(SkeletonAnimationResourceLoader::TYPE_ID));
				skeletonAnimationResourceLoader->initialize(*asset, *skeletonAnimationResource);

				// Commit resource streamer asset load request
				ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
				resourceStreamerLoadRequest.resource = skeletonAnimationResource;
				resourceStreamerLoadRequest.resourceLoader = skeletonAnimationResourceLoader;
				mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);
			}

			// Done
			return skeletonAnimationResource->getId();
		}

		// Error!
		return getUninitialized<SkeletonAnimationResourceId>();
	}

	SkeletonAnimationResourceId SkeletonAnimationResourceManager::createSkeletonAnimationResourceByAssetId(AssetId assetId)
	{
		// Skeleton animation resource is not allowed to exist, yet
		assert(nullptr == getSkeletonAnimationResourceByAssetId(assetId));

		// Create the skeleton animation resource instance
		SkeletonAnimationResource& skeletonAnimationResource = mSkeletonAnimationResources.addElement();
		skeletonAnimationResource.setResourceManager(this);
		skeletonAnimationResource.setAssetId(assetId);

		// Done
		setResourceLoadingState(skeletonAnimationResource, IResource::LoadingState::LOADED);
		return skeletonAnimationResource.getId();
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void SkeletonAnimationResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		const uint32_t numberOfElements = mSkeletonAnimationResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			if (mSkeletonAnimationResources.getElementByIndex(i).getAssetId() == assetId)
			{
				loadSkeletonAnimationResourceByAssetId(assetId, nullptr, true);
				break;
			}
		}
	}

	void SkeletonAnimationResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	IResourceLoader* SkeletonAnimationResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		IResourceLoader* resourceLoader = IResourceManager::acquireResourceLoaderInstance(resourceLoaderTypeId);

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			// We only support our own skeleton animation format
			assert(resourceLoaderTypeId == SkeletonAnimationResourceLoader::TYPE_ID);
			resourceLoader = new SkeletonAnimationResourceLoader(*this, mRendererRuntime);
			mUsedResourceLoaderInstances.push_back(resourceLoader);
		}

		// Done
		return resourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
