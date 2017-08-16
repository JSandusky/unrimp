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
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'Concurrency::details::_ExceptionHolder': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'Concurrency::details::_ExceptionHolder': assignment operator was implicitly defined as deleted

#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <functional>
// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4355)	// warning C4355: 'this': used in base member initializer list
	PRAGMA_WARNING_DISABLE_MSVC(5026)	// warning C5026: 'Concurrency::details::_RunAllParam<Concurrency::details::_Unit_type>': move constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5027)	// warning C5027: 'Concurrency::details::_RunAllParam<Concurrency::details::_Unit_type>': move assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	#include <future>
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
		/// Uninitialized number of threads means to use as many threads as there are hardware threads on the system
		ThreadPool(size_t threads = getUninitialized<size_t>());
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


// Reset warning manipulations we did above
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/Thread/ThreadPool.inl"
