/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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


// Disable warnings
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'Concurrency::details::_ExceptionHolder': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'Concurrency::details::_ExceptionHolder': assignment operator was implicitly defined as deleted


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <future>
#include <functional>


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
	*  @note
	*    - Meant for data-parallel use-cases
	*    - Implementation from https://github.com/netromdk/threadpool
	*/
	template <typename RetType> ///< Return type of tasks
	class ThreadPool
	{


	public:
		using Task = std::function<RetType()>;
		using Callback = std::function<void()>;


	private:
		using TaskQueue = std::queue<Task>;
		using FutVec = std::vector<std::future<RetType>>;


	public:
		/// ~0 (== -1 == size_t max_value) threads means to use as many threads as there are hardware threads on
		/// the system.
		ThreadPool(size_t threads = ~0);
		~ThreadPool();

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


// Reset warning manipulations we did above
PRAGMA_WARNING_POP
