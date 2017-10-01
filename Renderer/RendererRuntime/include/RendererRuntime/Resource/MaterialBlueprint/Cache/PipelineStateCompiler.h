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
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderType.h"

#include <deque>
#include <mutex>
// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(5026)	// warning C5026: 'std::_Generic_error_category': move constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5027)	// warning C5027: 'std::_Generic_error_category': move assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'std::codecvt_base': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'std::codecvt<char16_t,char,_Mbstatet>': assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4774)	// warning C4774: 'sprintf_s' : format string expected in argument 3 is not a string literal
	#include <vector>
	#include <string>
	#include <atomic>	// For "std::atomic<>"
PRAGMA_WARNING_POP
#include <thread>
#include <condition_variable>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IProgram;
	class IPipelineState;
}
namespace RendererRuntime
{
	class ShaderCache;
	class IRendererRuntime;
	class PipelineStateCache;
	class MaterialBlueprintResource;
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
	*    Pipeline state compiler class
	*
	*  @remarks
	*    A pipeline state must master the following stages in order to archive the inner wisdom:
	*    1. Asynchronous shader building
	*    2. Asynchronous shader compilation
	*    3. Synchronous renderer backend dispatch TODO(co) Asynchronous renderer backend dispatch if supported by the renderer API
	*
	*  @note
	*    - Takes care of asynchronous pipeline state compilation
	*/
	class PipelineStateCompiler
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;
		friend class PipelineStateCacheManager;	// Only the pipeline state cache manager is allowed to commit compiler requests


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline bool isAsynchronousCompilationEnabled() const;
		void setAsynchronousCompilationEnabled(bool enabled);
		inline uint32_t getNumberOfCompilerThreads() const;
		void setNumberOfCompilerThreads(uint32_t numberOfCompilerThreads);
		inline void flushBuilderQueue();
		inline void flushCompilerQueue();
		inline void flushAllQueues();
		void dispatch();


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		struct CompilerRequest
		{
			// Input
			PipelineStateCache& pipelineStateCache;
			// Internal
			ShaderCache*			  shaderCache[NUMBER_OF_SHADER_TYPES];
			std::string				  shaderSourceCode[NUMBER_OF_SHADER_TYPES];
			Renderer::IPipelineState* pipelineStateObject;

			explicit CompilerRequest(PipelineStateCache& _pipelineStateCache) :
				pipelineStateCache(_pipelineStateCache),
				pipelineStateObject(nullptr)
			{
				for (uint8_t i = 0; i < NUMBER_OF_SHADER_TYPES; ++i)
				{
					shaderCache[i] = nullptr;
				}
			}
			explicit CompilerRequest(const CompilerRequest& compilerRequest) :
				pipelineStateCache(compilerRequest.pipelineStateCache),
				pipelineStateObject(compilerRequest.pipelineStateObject)
			{
				for (uint8_t i = 0; i < NUMBER_OF_SHADER_TYPES; ++i)
				{
					shaderCache[i]		= compilerRequest.shaderCache[i];
					shaderSourceCode[i] = compilerRequest.shaderSourceCode[i];
				}
			}
			CompilerRequest& operator=(const CompilerRequest&) = delete;
		};

		typedef std::vector<std::thread> CompilerThreads;
		typedef std::deque<CompilerRequest> CompilerRequests;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit PipelineStateCompiler(IRendererRuntime& rendererRuntime);
		explicit PipelineStateCompiler(const PipelineStateCompiler&) = delete;
		~PipelineStateCompiler();
		PipelineStateCompiler& operator=(const PipelineStateCompiler&) = delete;
		void addAsynchronousCompilerRequest(PipelineStateCache& pipelineStateCache);
		void instantSynchronousCompilerRequest(MaterialBlueprintResource& materialBlueprintResource, PipelineStateCache& pipelineStateCache);
		void flushQueue(std::mutex& mutex, const CompilerRequests& compilerRequests);
		void builderThreadWorker();
		void compilerThreadWorker();
		Renderer::IPipelineState* createPipelineState(const RendererRuntime::MaterialBlueprintResource& materialBlueprintResource, uint32_t serializedPipelineStateHash, Renderer::IProgram& program) const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime& mRendererRuntime;	///< Renderer runtime instance, do not destroy the instance
		bool			  mAsynchronousCompilationEnabled;
		uint32_t		  mNumberOfCompilerThreads;

		// Asynchronous building (moderate cost)
		std::atomic<bool>		mShutdownBuilderThread;
		std::mutex				mBuilderMutex;
		std::condition_variable	mBuilderConditionVariable;
		CompilerRequests		mBuilderQueue;
		std::thread				mBuilderThread;

		// Asynchronous compilation (nuts cost)
		std::atomic<bool>		mShutdownCompilerThread;
		std::mutex				mCompilerMutex;
		std::condition_variable	mCompilerConditionVariable;
		CompilerRequests		mCompilerQueue;
		CompilerThreads			mCompilerThreads;

		// Synchronous dispatch
		std::mutex		 mDispatchMutex;
		CompilerRequests mDispatchQueue;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateCompiler.inl"
