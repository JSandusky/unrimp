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
#include "RendererRuntime/Core/Manager.h"
#include "RendererRuntime/Core/Thread/ThreadPool.h"


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
	*    Thread manager
	*
	*  @remarks
	*    The data parallel thread pool is handy for situations were data can be processed in parallel (not task parallel). Example use-cases:
	*    - Frustum culling
	*    - Animation update
	*    - Particles update
	*
	*    Usage example: TODO(co) Streamline by using e.g. a template
	*    // Items which are going to be data-parallel-processed
	*    typedef std::vector<Item> Items;
	*    Items items;
	*
	*    // Worker function
	*    void updateItem(Items::iterator start, Items::iterator end)
	*    {
	*        while (start != end)
	*        {
	*            // ... do work...
	*            ++start;
	*        }
	*    }
	*
	*    // Setup calculation threads
	*    ThreadPool<void>& threadPool = ... get thread pool instance...
	*    size_t itemCount = items.size();
	*    size_t splitCount = 1;	// Package size for each thread to work on (will change when maximum number of threads is reached)
	*    const size_t threadCount = threadPool.getThreadCountAndSplitCount(itemCount, splitCount);
	*    Items::iterator startIterator = items.begin();
	*    Items::iterator endIterator = items.end();
	*    for (size_t i = 0; i < threadCount; ++i)
	*    {
	*        const size_t numberOfItemsToProcess = (i >= threadCount - 1) ? itemCount : splitCount;	// The last thread has to do all the rest of the remaining work
	*        threadPool.queueTask(std::bind(updateItem, startIterator, startIterator + numberOfItemsToProcess));
	*        itemCount -= splitCount;
	*        startIterator += splitCount;
	*    }
	*
	*    // Wait that all worker threads have done their part of the calculation
	*    threadPool.process();
	*/
	class ThreadManager : private Manager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline ThreadPool<void>& getDataParallelThreadPool();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline ThreadManager();
		inline ~ThreadManager();
		ThreadManager(const ThreadManager&) = delete;
		ThreadManager& operator=(const ThreadManager&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		ThreadPool<void> mDataParallelThreadPool;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/Thread/ThreadManager.inl"
