/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
#include "RendererRuntime/Core/NonCopyable.h"

#include <deque>
#include <mutex>
#include <atomic>
#include <thread>
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
	*/
	class ResourceStreamer : private NonCopyable
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
			IResource*		 resource;			///< Must be valid, do not destroy the instance
			IResourceLoader* resourceLoader;	///< Must be valid, do not destroy the instance
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


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::deque<LoadRequest> LoadRequests;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime& mRendererRuntime;	///< Renderer runtime instance, do not destroy the instance
		// Resource streamer stage: 1. Asynchronous deserialization
		std::atomic<bool>		  mShutdownDeserializationThread;
		std::mutex				  mDeserializationMutex;
		std::condition_variable	  mDeserializationConditionVariable;
		LoadRequests			  mDeserializationQueue;
		std::thread				  mDeserializationThread;
		// Resource streamer stage: 2. Asynchronous processing
		std::atomic<bool>		  mShutdownProcessingThread;
		std::mutex				  mProcessingMutex;
		std::condition_variable	  mProcessingConditionVariable;
		LoadRequests			  mProcessingQueue;
		std::thread				  mProcessingThread;
		// Resource streamer stage: 3. Synchronous dispatch to e.g. the renderer backend
		std::mutex				  mDispatchMutex;
		LoadRequests			  mDispatchQueue;
		LoadRequests			  mFullyLoadedWaitingQueue;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Detail/ResourceStreamer.inl"
