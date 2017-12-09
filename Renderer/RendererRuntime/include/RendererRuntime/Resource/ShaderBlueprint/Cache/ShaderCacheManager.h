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
#include "RendererRuntime/Core/Manager.h"
#include "RendererRuntime/Core/Platform/PlatformTypes.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderType.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4548)	// warning C4548: expression before comma has no effect; expected expression with side-effect
	PRAGMA_WARNING_DISABLE_MSVC(5039)	// warning C5039: '_Thrd_start': pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior may occur if this function throws an exception.
	#include <mutex>
	#include <unordered_map>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IShaderLanguage;
}
namespace RendererRuntime
{
	class IFile;
	class ShaderCache;
	class PipelineStateSignature;
	class MaterialBlueprintResource;
	class ShaderBlueprintResourceManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t ShaderCacheId;			///< Shader cache identifier, often but not always identical to the shader combination ID
	typedef uint32_t ShaderSourceCodeId;	///< Shader source code identifier, result of hashing the build shader source code


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/*
	*  @brief
	*    Shader cache manager
	*
	*  @see
	*    - See "RendererRuntime::PipelineStateCacheManager" for additional information
	*/
	class ShaderCacheManager : private Manager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class ShaderBlueprintResourceManager;	// Is creating and using a shader cache manager instance
		friend class PipelineStateCompiler;				// Is tightly interacting with the shader cache manager


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the owner shader blueprint resource manager
		*
		*  @return
		*    The owner shader blueprint resource manager
		*/
		inline ShaderBlueprintResourceManager& getShaderBlueprintResourceManager() const;

		/**
		*  @brief
		*    Get shader cache by pipeline state signature and shader type; synchronous processing
		*
		*  @param[in] pipelineStateSignature
		*    Pipeline state signature to use
		*  @param[in] materialBlueprintResource
		*    Material blueprint resource
		*  @param[in] shaderLanguage
		*    Shader language
		*  @param[in] shaderType
		*    Shader type
		*
		*  @return
		*    The shader cache, null pointer on error
		*/
		ShaderCache* getShaderCache(const PipelineStateSignature& pipelineStateSignature, const MaterialBlueprintResource& materialBlueprintResource, Renderer::IShaderLanguage& shaderLanguage, ShaderType shaderType);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline explicit ShaderCacheManager(ShaderBlueprintResourceManager& shaderBlueprintResourceManager);
		inline ~ShaderCacheManager();
		explicit ShaderCacheManager(const ShaderCacheManager&) = delete;
		ShaderCacheManager& operator=(const ShaderCacheManager&) = delete;
		// TODO(co) Make this private. See "TODO(co) Do only clear the influenced shader cache entries" for context.
	public:
		void clearCache();
	private:
		void loadCache(IFile& file);
		inline bool doesCacheNeedSaving() const;
		void saveCache(IFile& file);


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::unordered_map<ShaderCacheId, ShaderCache*>		  ShaderCacheByShaderCacheId;
		typedef std::unordered_map<ShaderSourceCodeId, ShaderCacheId> ShaderCacheByShaderSourceCodeId;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		ShaderBlueprintResourceManager& mShaderBlueprintResourceManager;	///< Owner shader blueprint resource manager
		ShaderCacheByShaderCacheId		mShaderCacheByShaderCacheId;		///< Manages the shader cache instances
		ShaderCacheByShaderSourceCodeId	mShaderCacheByShaderSourceCodeId;	///< Shader source code ID to shader cache ID mapping
		bool							mCacheNeedsSaving;					///< "true" if a cache needs saving due to changes during runtime, else "false"
		std::mutex						mMutex;								///< Mutex due to "RendererRuntime::PipelineStateCompiler" interaction, no too fine granular lock/unlock required because usually it's only asynchronous or synchronous processing, not both at one and the same time


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderCacheManager.inl"
