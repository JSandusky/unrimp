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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Asset/Asset.h"

#include <deque>
#include <mutex>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <condition_variable>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IResource;
	class IResourceLoader;
	class IRendererRuntime;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId ResourceLoaderTypeId;	///< Resource loader type identifier, internally just a POD "uint32_t", usually created by hashing the file format extension (if the resource loader is processing file data in the first place)


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Resource streamer responsible for getting the resource data into memory
	*
	*  @remarks
	*    By default, asynchronous resource streaming is used. This is also known as
	*    - Asynchronous content streaming
	*    - Asynchronous asset loading
	*    - Asynchronous data streaming
	*    - Background resource loading
	*
	*    A resource must master the following stages in order to archive the inner wisdom:
	*    1. Asynchronous deserialization
	*    2. Asynchronous processing
	*    3. Synchronous dispatch, e.g. to the renderer backend
	*
	*  @todo
	*    - TODO(co) It might make sense to use lock-free-queues in here
	*/
	class ResourceStreamer
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		struct LoadRequest
		{
			// Data provided from the outside
			const Asset*		 asset;					///< Used asset, must be valid
			ResourceLoaderTypeId resourceLoaderTypeId;	///< Must be valid
			bool				 reload;				///< "true" if the resource is new in memory, else "false" for reload an already loaded resource (and e.g. update cache entries)
			IResource*			 resource;				///< Must be valid, do not destroy the instance
			// In flight data
			mutable IResourceLoader* resourceLoader;	///< Null pointer at first, must be valid as soon as the load request is in flight, do not destroy the instance

			// Methods
			inline LoadRequest(const Asset& _asset, ResourceLoaderTypeId _resourceLoaderTypeId, bool _reload, IResource& _resource) :
				asset(&_asset),
				resourceLoaderTypeId(_resourceLoaderTypeId),
				reload(_reload),
				resource(&_resource),
				resourceLoader(nullptr)
			{
				// Nothing here
			}
		};


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		void commitLoadRequest(const LoadRequest& loadRequest);
		void flushAllQueues();

		/**
		*  @brief
		*    Resource streamer update performing dispatch to e.g. the renderer backend
		*
		*  @note
		*    - Call this once per frame
		*/
		void dispatch();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit ResourceStreamer(IRendererRuntime& rendererRuntime);
		~ResourceStreamer();
		ResourceStreamer(const ResourceStreamer&) = delete;
		ResourceStreamer& operator=(const ResourceStreamer&) = delete;
		void deserializationThreadWorker();
		void processingThreadWorker();
		void finalizeLoadRequest(const LoadRequest& loadRequest);


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::vector<IResourceLoader*> ResourceLoaders;
		typedef std::deque<LoadRequest> LoadRequests;
		struct ResourceLoaderType
		{
			uint32_t		numberOfInstances;
			ResourceLoaders	freeResourceLoaders;
			LoadRequests	waitingLoadRequests;
		};
		typedef std::unordered_map<uint32_t, ResourceLoaderType> ResourceLoaderTypeManager;	///< Key = "RendererRuntime::ResourceLoaderTypeId"


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime& mRendererRuntime;	///< Renderer runtime instance, do not destroy the instance
		std::mutex		  mResourceManagerMutex;
		// Resource streamer stage: 1. Asynchronous deserialization
		std::atomic<bool>		    mShutdownDeserializationThread;
		std::mutex					mDeserializationMutex;
		std::condition_variable		mDeserializationConditionVariable;
		LoadRequests				mDeserializationQueue;
		ResourceLoaderTypeManager	mResourceLoaderTypeManager;	// Do only touch if "mResourceManagerMutex" is locked
		std::atomic<uint32_t>		mDeserializationWaitingQueueRequests;
		std::thread					mDeserializationThread;
		// Resource streamer stage: 2. Asynchronous processing
		std::atomic<bool>		mShutdownProcessingThread;
		std::mutex				mProcessingMutex;
		std::condition_variable mProcessingConditionVariable;
		LoadRequests			mProcessingQueue;
		std::thread				mProcessingThread;
		// Resource streamer stage: 3. Synchronous dispatch to e.g. the renderer backend
		std::mutex	 mDispatchMutex;
		LoadRequests mDispatchQueue;
		LoadRequests mFullyLoadedWaitingQueue;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
