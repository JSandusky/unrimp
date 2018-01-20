/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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


// TODO(co) Code-style adoptions and extensions


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/GetUninitialized.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4355)	// warning C4355: 'this': used in base member initializer list
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'Concurrency::details::_ExceptionHolder': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'Concurrency::details::_ExceptionHolder': assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(5026)	// warning C5026: 'Concurrency::details::_RunAllParam<Concurrency::details::_Unit_type>': move constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5027)	// warning C5027: 'Concurrency::details::_RunAllParam<Concurrency::details::_Unit_type>': move assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5039)	// warning C5039: '_Thrd_start': pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior may occur if this function throws an exception.
	#include <mutex>
	#include <queue>
	#include <thread>
	#include <vector>
	#include <future>
	#include <functional>
PRAGMA_WARNING_POP


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
	*    Generic usable thread pool to avoid recreation of threads each tick
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
	*    for (size_t threadIndex = 0; threadIndex < threadCount; ++threadIndex)
	*    {
	*        const size_t numberOfItemsToProcess = (threadIndex >= threadCount - 1) ? itemCount : splitCount;	// The last thread has to do all the rest of the remaining work
	*        threadPool.queueTask(std::bind(updateItem, startIterator, startIterator + numberOfItemsToProcess));
	*        itemCount -= splitCount;
	*        startIterator += splitCount;
	*    }
	*
	*    // Wait that all worker threads have done their part of the calculation
	*    threadPool.process();
	*
	*  @note
	*    - Meant for data-parallel use-cases
	*    - Implementation from https://github.com/netromdk/threadpool
	*/
	template <typename RetType> ///< Return type of tasks
	class ThreadPool final
	{


	public:
		using Task = std::function<RetType()>;
		using Callback = std::function<void()>;


	private:
		using TaskQueue = std::queue<Task>;
		using FutVec = std::vector<std::future<RetType>>;


	public:
		/// Uninitialized number of threads means to use as many threads as there are hardware threads on the system
		explicit ThreadPool(size_t threads = getUninitialized<size_t>());
		explicit ThreadPool(const ThreadPool& threadPool) = delete;
		~ThreadPool();
		const ThreadPool& operator= (const ThreadPool& threadPool) = delete;

		void queueTask(const Task &task);
		void queueTask(Task &&task);

		/// Process all enqueued tasks.
		/** If no completion callback is defined then it
		  will block until done, otherwise it returns immediately and the callback
		  will be invoked when done. */
		void process(Callback callback = Callback());

		size_t getThreadCount() const { return threads; }
		FutVec &getFutures() { return futuresDone; }

		size_t getThreadCountAndSplitCount(size_t itemCount, size_t& splitCount) const
		{
			size_t threadCount = static_cast<size_t>(std::ceil(itemCount / static_cast<float>(splitCount)));
			if (threadCount > threads)
			{
				// Clamp thread count to maximum threads
				threadCount = threads;

				// Calculate new package size per thread
				splitCount = itemCount / threads;
			}
			return threadCount;
		}

	private:
		void _process();

		size_t threads;
		std::thread thread;
		std::mutex processMutex;
		TaskQueue tasks;
		FutVec futuresDone, futuresPending;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/Thread/ThreadPool.inl"
