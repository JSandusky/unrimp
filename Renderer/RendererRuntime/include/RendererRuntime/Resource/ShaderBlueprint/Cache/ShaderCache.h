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
#include <Renderer/Public/Renderer.h>

#include "RendererRuntime/Core/StringId.h"

#include <vector>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t			 ShaderCacheId;	///< Shader cache identifier, identical to the shader combination ID
	typedef StringId			 AssetId;		///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>"
	typedef std::vector<AssetId> AssetIds;


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class ShaderCache
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class ShaderCacheManager;	// Is creating and managing shader cache instances
		friend class PipelineStateCompiler;	// Is creating shader cache instances


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the shader cache ID
		*
		*  @return
		*    The shader cache ID
		*/
		inline ShaderCacheId getShaderCacheId() const;

		/**
		*  @brief
		*    Return master shader cache
		*
		*  @return
		*    The master shader cache, can be a null pointer, don't destroy the instance
		*/
		inline ShaderCache* getMasterShaderCache() const;

		/**
		*  @brief
		*    Return shader bytecode
		*
		*  @return
		*    The shader bytecode
		*/
		inline const Renderer::ShaderBytecode& getShaderBytecode() const;

		/**
		*  @brief
		*    Return shader
		*
		*  @return
		*    The shader
		*/
		inline Renderer::IShaderPtr getShaderPtr() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline ShaderCache();
		inline explicit ShaderCache(ShaderCacheId shaderCacheId);
		inline ShaderCache(ShaderCacheId shaderCacheId, ShaderCache* masterShaderCache);
		inline ~ShaderCache();
		explicit ShaderCache(const ShaderCache&) = delete;
		ShaderCache& operator=(const ShaderCache&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		ShaderCacheId			 mShaderCacheId;
		ShaderCache*			 mMasterShaderCache;		///< If there's a master shader cache instance, we don't own the references shader but only redirect to it (multiple shader combinations resulting in same shader source code topic), don't destroy the instance
		AssetIds				 mAssetIds;					///< List of IDs of the assets (shader blueprint, shader piece) which took part in the shader cache creation
		uint64_t				 mCombinedAssetFileHashes;	///< Combination of the file hash of all assets (shader blueprint, shader piece) which took part in the shader cache creation
		Renderer::ShaderBytecode mShaderBytecode;
		Renderer::IShaderPtr	 mShaderPtr;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderCache.inl"
