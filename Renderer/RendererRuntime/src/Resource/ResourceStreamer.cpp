/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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

// TODO(co) Can we do somthing about the warning which does not involve using "std::thread"-pointers?
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
		loadRequest.resourceLoader->onDeserialization();
		loadRequest.resourceLoader->onProcessing();
		loadRequest.resourceLoader->onRendererBackendDispatch();
	}

	void ResourceStreamer::update()
	{
		// TODO(co)
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
		mDeserializationThread.join();
		mProcessingThread.join();
	}

	void ResourceStreamer::deserializationThreadWorker()
	{
		// TODO(co)
		while (!mShutdownDeserializationThread)
		{
			int ii = 0;
		}
	}

	void ResourceStreamer::processingThreadWorker()
	{
		// TODO(co)
		while (!mShutdownProcessingThread)
		{
			int ii = 0;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
