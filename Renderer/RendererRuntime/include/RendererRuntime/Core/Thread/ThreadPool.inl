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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	template <typename RetType>
	ThreadPool<RetType>::ThreadPool(size_t threads)
	{
		if (isUninitialized(threads))
		{
			threads = std::thread::hardware_concurrency();
		}
		if (threads == 0)
		{
			threads = 1;
		}
		this->threads = threads;
	}

	template <typename RetType>
	ThreadPool<RetType>::~ThreadPool()
	{
		// If thread is running then wait for it to complete
		if (thread.joinable())
		{
			thread.join();
		}
	}

	template <typename RetType>
	void ThreadPool<RetType>::queueTask(const Task &task)
	{
		tasks.push(task);
	}

	template <typename RetType>
	void ThreadPool<RetType>::queueTask(Task &&task)
	{
		tasks.emplace(std::move(task));
	}

	template <typename RetType>
	void ThreadPool<RetType>::process(Callback callback)
	{
		if (callback)
		{
			thread = std::thread([&]{
				_process();
				callback();
			});
		}
		else
		{
			_process();
		}
	}

	template <typename RetType>
	void ThreadPool<RetType>::_process()
	{
		std::lock_guard<std::mutex> lock(processMutex);
		futuresDone.clear();

		while (!tasks.empty())
		{
			const size_t taskAmount = tasks.size();
			const size_t amount = (threads > taskAmount ? taskAmount : threads);

			for (size_t i = 0; i < amount; ++i)
			{
				auto task = tasks.front();
				tasks.pop();

				auto future = std::async(std::launch::async, [=] {
					return task();
					});
				futuresPending.emplace_back(std::move(future));
			}

			for (auto &future : futuresPending)
			{
				future.wait();
				futuresDone.emplace_back(std::move(future));
			}

			futuresPending.clear();
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
