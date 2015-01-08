/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "RendererRuntime/Resource/ResourceStreamer.h"
#include "RendererRuntime/Resource/IResourceLoader.h"
#include "RendererRuntime/Resource/IResourceManager.h"
#include "RendererRuntime/Resource/IResource.h"

// TODO(co) Can we do something about the warning which does not involve using "std::thread"-pointers?
#pragma warning(disable: 4355)	// warning C4355: 'this': used in base member initializer list


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void ResourceStreamer::commitLoadRequest(const LoadRequest& loadRequest)
	{
		// Update the resource loading state
		loadRequest.resource->mLoadingState = IResource::LoadingState::LOADING;

		// Push the load request into the queue of the first resource streamer pipeline stage
		// -> Resource streamer stage: 1. Asynchronous deserialization
		std::unique_lock<std::mutex> deserializationMutexLock(mDeserializationMutex);
		mDeserializationQueue.push(loadRequest);
		deserializationMutexLock.unlock();
		mDeserializationConditionVariable.notify_one();
	}

	void ResourceStreamer::rendererBackendDispatch()
	{
		// Resource streamer stage: 3. Synchronous renderer backend dispatch

		// Continue as long as there's a load request left inside the queue
		bool stillInTimeBudget = true;	// TODO(co) Add a maximum time budget so we're not blocking too long (the show must go on)
		while (stillInTimeBudget)
		{
			// Get the load request
			std::unique_lock<std::mutex> rendererBackendDispatchMutexLock(mRendererBackendDispatchMutex);
			if (mRendererBackendDispatchQueue.empty())
			{
				break;
			}
			LoadRequest loadRequest = mRendererBackendDispatchQueue.front();
			mRendererBackendDispatchQueue.pop();
			rendererBackendDispatchMutexLock.unlock();

			// Do the work
			IResourceLoader* resourceLoader = loadRequest.resourceLoader;
			resourceLoader->onRendererBackendDispatch();

			// Update the resource loading state
			loadRequest.resource->mLoadingState = IResource::LoadingState::LOADED;

			// Release the resource loader instance
			resourceLoader->getResourceManager().releaseResourceLoaderInstance(*resourceLoader);
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	ResourceStreamer::ResourceStreamer(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mShutdownDeserializationThread(false),
		mShutdownProcessingThread(false),
		mDeserializationThread(&ResourceStreamer::deserializationThreadWorker, this),
		mProcessingThread(&ResourceStreamer::processingThreadWorker, this)
	{
		// Nothing here
	}

	ResourceStreamer::~ResourceStreamer()
	{
		// Deserialization thread and processing thread shutdown
		mShutdownDeserializationThread = true;
		mShutdownProcessingThread = true;
		mDeserializationConditionVariable.notify_one();
		mProcessingConditionVariable.notify_one();
		mDeserializationThread.join();
		mProcessingThread.join();
	}

	void ResourceStreamer::deserializationThreadWorker()
	{
		// Resource streamer stage: 1. Asynchronous deserialization
		while (!mShutdownDeserializationThread)
		{
			// Continue as long as there's a load request left inside the queue, if it's empty go to sleep
			std::unique_lock<std::mutex> deserializationMutexLock(mDeserializationMutex);
			mDeserializationConditionVariable.wait(deserializationMutexLock);
			while (!mDeserializationQueue.empty() && !mShutdownDeserializationThread)
			{
				// Get the load request
				LoadRequest loadRequest = mDeserializationQueue.front();
				mDeserializationQueue.pop();
				deserializationMutexLock.unlock();

				// Do the work
				loadRequest.resourceLoader->onDeserialization();

				{ // Push the load request into the queue of the next resource streamer pipeline stage
				  // -> Resource streamer stage: 2. Asynchronous processing
					std::unique_lock<std::mutex> processingMutexLock(mProcessingMutex);
					mProcessingQueue.push(loadRequest);
					processingMutexLock.unlock();
					mProcessingConditionVariable.notify_one();
				}

				// We're ready for the next round
				deserializationMutexLock.lock();
			}
		}
	}

	void ResourceStreamer::processingThreadWorker()
	{
		// Resource streamer stage: 2. Asynchronous processing
		while (!mShutdownProcessingThread)
		{
			// Continue as long as there's a load request left inside the queue, if it's empty go to sleep
			std::unique_lock<std::mutex> processingMutexLock(mProcessingMutex);
			mProcessingConditionVariable.wait(processingMutexLock);
			while (!mProcessingQueue.empty() && !mShutdownProcessingThread)
			{
				// Get the load request
				LoadRequest loadRequest = mProcessingQueue.front();
				mProcessingQueue.pop();
				processingMutexLock.unlock();

				// Do the work
				loadRequest.resourceLoader->onProcessing();

				{ // Push the load request into the queue of the next resource streamer pipeline stage
				  // -> Resource streamer stage: 3. Synchronous renderer backend dispatch
					std::lock_guard<std::mutex> rendererBackendDispatchMutexLock(mRendererBackendDispatchMutex);
					mRendererBackendDispatchQueue.push(loadRequest);
				}

				// We're ready for the next round
				processingMutexLock.lock();
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
