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
#include "RendererRuntime/Resource/Detail/IResource.h"
#include "RendererRuntime/Resource/IResourceListener.h"

#include <algorithm>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		inline bool orderByResourceListener(RendererRuntime::IResourceListener* left, RendererRuntime::IResourceListener* right)
		{
			return (left < right);
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
	void IResource::connectResourceListener(IResourceListener& resourceListener)
	{
		SortedResourceListeners::iterator iterator = std::lower_bound(mSortedResourceListeners.begin(), mSortedResourceListeners.end(), &resourceListener, ::detail::orderByResourceListener);
		if (iterator == mSortedResourceListeners.end() || *iterator != &resourceListener)
		{
			mSortedResourceListeners.insert(iterator, &resourceListener);
			resourceListener.mResourceConnections.emplace_back(IResourceListener::ResourceConnection(mResourceManager, mResourceId));
			resourceListener.onLoadingStateChange(*this);
		}
	}

	void IResource::disconnectResourceListener(IResourceListener& resourceListener)
	{
		SortedResourceListeners::iterator iterator = std::lower_bound(mSortedResourceListeners.begin(), mSortedResourceListeners.end(), &resourceListener, ::detail::orderByResourceListener);
		if (iterator != mSortedResourceListeners.end() && *iterator == &resourceListener)
		{
			{ // TODO(co) If this turns out to be a performance problem, we might want to use e.g. a sorted vector
				const IResourceListener::ResourceConnection resourceConnection(mResourceManager, mResourceId);
				IResourceListener::ResourceConnections::iterator connectionIterator = std::find_if(resourceListener.mResourceConnections.begin(), resourceListener.mResourceConnections.end(),
					[resourceConnection](const IResourceListener::ResourceConnection& currentResourceConnection) { return (currentResourceConnection.resourceManager == resourceConnection.resourceManager && currentResourceConnection.resourceId == resourceConnection.resourceId); }
					);
				assert(connectionIterator != resourceListener.mResourceConnections.end());
				resourceListener.mResourceConnections.erase(connectionIterator);
			}
			mSortedResourceListeners.erase(iterator);
		}
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	IResource& IResource::operator=(IResource&& resource)
	{
		// Swap data
		std::swap(mResourceManager,			resource.mResourceManager);
		std::swap(mResourceId,				resource.mResourceId);
		std::swap(mAssetId,					resource.mAssetId);
		std::swap(mLoadingState,			resource.mLoadingState);
		std::swap(mSortedResourceListeners,	resource.mSortedResourceListeners);	// This is fine, resource listeners store a resource ID instead of a raw pointer

		// Done
		return *this;
	}

	void IResource::setLoadingState(LoadingState loadingState)
	{
		mLoadingState = loadingState;
		for (IResourceListener* resourceListener : mSortedResourceListeners)
		{
			resourceListener->onLoadingStateChange(*this);
		}
	}

	void IResource::deinitializeElement()
	{
		// Disconnect all resource listeners
		const IResourceListener::ResourceConnection resourceConnection(mResourceManager, mResourceId);
		for (IResourceListener* resourceListener : mSortedResourceListeners)
		{
			// TODO(co) If this turns out to be a performance problem, we might want to use e.g. a sorted vector
			IResourceListener::ResourceConnections::iterator connectionIterator = std::find_if(resourceListener->mResourceConnections.begin(), resourceListener->mResourceConnections.end(),
				[resourceConnection](const IResourceListener::ResourceConnection& currentResourceConnection) { return (currentResourceConnection.resourceManager == resourceConnection.resourceManager && currentResourceConnection.resourceId == resourceConnection.resourceId); }
				);
			assert(connectionIterator != resourceListener->mResourceConnections.end());
			resourceListener->mResourceConnections.erase(connectionIterator);
		}

		// Reset everything
		mResourceManager = nullptr;
		setUninitialized(mResourceId);
		setUninitialized(mAssetId);
		mLoadingState = LoadingState::UNLOADED;
		mSortedResourceListeners.clear();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
