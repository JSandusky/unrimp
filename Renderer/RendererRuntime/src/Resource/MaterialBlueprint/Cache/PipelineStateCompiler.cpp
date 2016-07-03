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


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateCompiler.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateCache.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/ProgramCache.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderType.h"
#include "RendererRuntime/Core/Platform/PlatformManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <cassert>


// Disable warnings
// TODO(co) See "RendererRuntime::PipelineStateCompiler::PipelineStateCompiler()": How the heck should we avoid such a situation without using complicated solutions like a pointer to an instance? (= more individual allocations/deallocations)
#pragma warning(disable: 4355)	// warning C4355: 'this': used in base member initializer list


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void PipelineStateCompiler::setAsynchronousCompilationEnabled(bool enabled)
	{
		// State change?
		if (mAsynchronousCompilationEnabled != enabled)
		{
			mAsynchronousCompilationEnabled = enabled;

			// Ensure the internal queues are flushes so that we can guarantee that everything is synchronous available
			if (!mAsynchronousCompilationEnabled)
			{
				flushAllQueues();
			}
		}
	}

	void PipelineStateCompiler::setNumberOfCompilerThreads(uint32_t numberOfCompilerThreads)
	{
		if (mNumberOfCompilerThreads != numberOfCompilerThreads)
		{
			// Compiler threads shutdown
			mShutdownCompilerThread = true;
			mCompilerConditionVariable.notify_all();
			for (std::thread& thread : mCompilerThreads)
			{
				thread.join();
			}

			// Create the compiler threads crunching the shaders into microcode
			mNumberOfCompilerThreads = numberOfCompilerThreads;
			mCompilerThreads.clear();
			mCompilerThreads.reserve(mNumberOfCompilerThreads);
			mShutdownCompilerThread = false;
			for (uint32_t i = 0; i < mNumberOfCompilerThreads; ++i)
			{
				mCompilerThreads.push_back(std::thread(&PipelineStateCompiler::compilerThreadWorker, this));
			}
		}
	}

	void PipelineStateCompiler::rendererBackendDispatch()
	{
		// Synchronous dispatch
		// TODO(co) Add maximum dispatch time budget
		// TODO(co) More clever mutex usage in order to reduce pipeline state compiler stalls due to synchronization
		std::lock_guard<std::mutex> dispatchMutexLock(mDispatchMutex);
		while (!mDispatchQueue.empty())
		{
			// Get the compiler request
			CompilerRequest compilerRequest(mDispatchQueue.back());
			mDispatchQueue.pop_back();

			// TODO(co)
			//PipelineStateCache& pipelineStateCache = compilerRequest.pipelineStateCache;
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	PipelineStateCompiler::PipelineStateCompiler(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mOptimization(Optimization::Level2),
		mAsynchronousCompilationEnabled(false),
		mNumberOfCompilerThreads(0),
		mShutdownBuilderThread(false),
		mBuilderThread(&PipelineStateCompiler::builderThreadWorker, this),
		mShutdownCompilerThread(false)
	{
		// Create and start the threads
		setNumberOfCompilerThreads(2);
	}

	PipelineStateCompiler::~PipelineStateCompiler()
	{
		// Builder thread shutdown
		mShutdownBuilderThread = true;
		mBuilderConditionVariable.notify_one();
		mBuilderThread.join();

		// Compiler threads shutdown
		setNumberOfCompilerThreads(0);
	}

	void PipelineStateCompiler::addAsynchronousCompilerRequest(PipelineStateCache& pipelineStateCache)
	{
		// Push the load request into the builder queue
		std::unique_lock<std::mutex> builderMutexLock(mBuilderMutex);
		mBuilderQueue.emplace_back(CompilerRequest(pipelineStateCache));
		builderMutexLock.unlock();
		mBuilderConditionVariable.notify_one();
	}

	void PipelineStateCompiler::instantSynchronousCompilerRequest(MaterialBlueprintResource& materialBlueprintResource, PipelineStateCache& pipelineStateCache)
	{
		// Get the program cache
		const ProgramCache* programCache = materialBlueprintResource.getPipelineStateCacheManager().getProgramCacheManager().getProgramCacheByPipelineStateSignature(pipelineStateCache.getPipelineStateSignature());
		if (nullptr != programCache)
		{
			Renderer::IProgramPtr programPtr = programCache->getProgramPtr();
			if (nullptr != programPtr)
			{
				Renderer::IRootSignaturePtr rootSignaturePtr = materialBlueprintResource.getRootSignaturePtr();

				// Start with the pipeline state of the material blueprint resource
				Renderer::PipelineState pipelineState = materialBlueprintResource.getPipelineState();

				// Setup the dynamic part of the pipeline state
				pipelineState.rootSignature	   = rootSignaturePtr;
				pipelineState.program		   = programPtr;
				pipelineState.vertexAttributes = materialBlueprintResource.getVertexAttributes();

				// Create the pipeline state object (PSO)
				pipelineStateCache.mPipelineStateObjectPtr = rootSignaturePtr->getRenderer().createPipelineState(pipelineState);
			}
		}
	}

	void PipelineStateCompiler::flushQueue(std::mutex& mutex, const CompilerRequests& compilerRequests)
	{
		bool everythingFlushed = false;
		do
		{
			{ // Process
				std::lock_guard<std::mutex> compilerMutexLock(mutex);
				everythingFlushed = compilerRequests.empty();
			}
			rendererBackendDispatch();

			// Wait for a moment to not totally pollute the CPU
			if (!everythingFlushed)
			{
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(1ms);
			}
		} while (!everythingFlushed);
	}

	void PipelineStateCompiler::builderThreadWorker()
	{
		PlatformManager::setCurrentThreadName("Renderer runtime: Pipeline state compiler stage: 1. Asynchronous shader building");
		while (!mShutdownBuilderThread)
		{
			// Continue as long as there's a compiler request left inside the queue, if it's empty go to sleep
			std::unique_lock<std::mutex> builderMutexLock(mBuilderMutex);
			mBuilderConditionVariable.wait(builderMutexLock);
			while (!mBuilderQueue.empty() && !mShutdownBuilderThread)
			{
				// Get the compiler request
				CompilerRequest compilerRequest(mBuilderQueue.back());
				mBuilderQueue.pop_back();
				builderMutexLock.unlock();

				// Do the work: Building the shader source code for the required combination
				bool compilationRequired = false;
				for (int i = 0; i < NUMBER_OF_SHADER_TYPES; ++i)
				{
					// TODO(co)
				}

				// Do we need to compile at least one shader of the request?
				if (compilationRequired)
				{
					// Push the compiler request into the queue of the asynchronous shader compilation
					std::unique_lock<std::mutex> compilerMutexLock(mCompilerMutex);
					mCompilerQueue.emplace_back(compilerRequest);
					compilerMutexLock.unlock();
					mCompilerConditionVariable.notify_one();
				}
				else
				{
					// Push the compiler request into the queue of the synchronous shader dispatch
					std::lock_guard<std::mutex> dispatchMutexLock(mDispatchMutex);
					mDispatchQueue.emplace_back(compilerRequest);
				}

				// We're ready for the next round
				builderMutexLock.lock();
			}
		}
	}

	void PipelineStateCompiler::compilerThreadWorker()
	{
		PlatformManager::setCurrentThreadName("Renderer runtime: Pipeline state compiler stage: 2. Asynchronous shader compilation");
		while (!mShutdownCompilerThread)
		{
			// Continue as long as there's a compiler request left inside the queue, if it's empty go to sleep
			std::unique_lock<std::mutex> compilerMutexLock(mCompilerMutex);
			mCompilerConditionVariable.wait(compilerMutexLock);
			while (!mCompilerQueue.empty() && !mShutdownCompilerThread)
			{
				// Get the compiler request
				CompilerRequest compilerRequest(mCompilerQueue.back());
				mCompilerQueue.pop_back();
				compilerMutexLock.unlock();

				// Do the work: Compiling the shader source code it in order to get the shader microcode
				for (int i = 0; i < NUMBER_OF_SHADER_TYPES; ++i)
				{
					// TODO(co)
				}

				{ // Push the compiler request into the queue of the synchronous shader dispatch
					std::lock_guard<std::mutex> dispatchMutexLock(mDispatchMutex);
					mDispatchQueue.emplace_back(compilerRequest);
				}

				// We're ready for the next round
				compilerMutexLock.lock();
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
