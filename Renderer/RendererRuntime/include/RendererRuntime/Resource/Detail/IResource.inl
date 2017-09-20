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
#include "RendererRuntime/Core/GetUninitialized.h"

#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline IResourceManager& IResource::getResourceManager() const
	{
		assert(nullptr != mResourceManager);
		return *mResourceManager;
	}

	template <typename T> T& IResource::getResourceManager() const
	{
		assert(nullptr != mResourceManager);
		return *static_cast<T*>(mResourceManager);
	}

	inline ResourceId IResource::getId() const
	{
		return mResourceId;
	}

	inline AssetId IResource::getAssetId() const
	{
		return mAssetId;
	}

	inline ResourceLoaderTypeId IResource::getResourceLoaderTypeId() const
	{
		return mResourceLoaderTypeId;
	}

	inline IResource::LoadingState IResource::getLoadingState() const
	{
		return mLoadingState;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline IResource::IResource() :
		mResourceManager(nullptr),
		mResourceId(getUninitialized<ResourceId>()),
		mAssetId(getUninitialized<AssetId>()),
		mResourceLoaderTypeId(getUninitialized<ResourceLoaderTypeId>()),
		mLoadingState(LoadingState::UNLOADED)
	{
		// Nothing here
	}

	inline IResource::~IResource()
	{
		// Sanity checks
		assert(nullptr == mResourceManager);
		assert(isUninitialized(mResourceId));
		assert(isUninitialized(mAssetId));
		assert(isUninitialized(mResourceLoaderTypeId));
		assert(LoadingState::UNLOADED == mLoadingState);
		assert(mSortedResourceListeners.empty());
	}

	inline void IResource::setResourceManager(IResourceManager* resourceManager)
	{
		mResourceManager = resourceManager;
	}

	inline void IResource::setAssetId(AssetId assetId)
	{
		mAssetId = assetId;
	}

	inline void IResource::setResourceLoaderTypeId(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		mResourceLoaderTypeId = resourceLoaderTypeId;
	}

	inline void IResource::initializeElement(ResourceId resourceId)
	{
		// Sanity checks
		assert(nullptr == mResourceManager);
		assert(isUninitialized(mResourceId));
		assert(isUninitialized(mAssetId));
		assert(isUninitialized(mResourceLoaderTypeId));
		assert(LoadingState::UNLOADED == mLoadingState);
		assert(mSortedResourceListeners.empty());

		// Set data
		mResourceId = resourceId;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
