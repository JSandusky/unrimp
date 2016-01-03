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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/ProgramCacheManager.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class ShaderProperties;
	class MaterialProperties;
	class PipelineStateCache;
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
	*    Pipeline state cache manager
	*
	*  @remarks
	*    The pipeline state cache is the top of the shader related cache hierarchy and maps to Direct3D 12, AMD Mantle,
	*    Apple Metal and other rendering APIs using pipeline state objects (PSO). The next cache hierarchy level
	*    is the program cache which maps to linked monolithic OpenGL programs and is also nice as a collection
	*    of shader compiler results which are fed into pipeline states. The lowest cache hierarchy level is the
	*    shader cache (vertex shader, fragment shader etc.) which handles the binary results of the shader compiler.
	*
	*    Sum up of the cache hierarchy:
	*    - Pipeline state cache: Maps to Direct3D 12, AMD Mantle, Apple Metal etc.
	*    - Program cache: Maps to linked monolithic OpenGL programs
	*    - Shader cache: Maps to Direct3D 9 - 11, separate OpenGL shader objects and is still required for Direct3D 12
	*      and other similar designed APIs because the binary shaders are required when creating pipeline state objects
	*
	*  @todo
	*    - TODO(co) Direct3D 12: Pipeline state object: Add support for "GetCachedBlob" (super efficient material cache), see https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/Samples/D3D12PipelineStateCache/src/PSOLibrary.cpp
	*/
	class PipelineStateCacheManager : private Manager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class MaterialBlueprintResource;	///< Is creating and using a program cache manager instance


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the owner material blueprint resource
		*
		*  @return
		*    The owner material blueprint resource
		*/
		inline MaterialBlueprintResource& getMaterialBlueprintResource() const;

		/**
		*  @brief
		*    Return the program cache manager
		*
		*  @return
		*    The program cache manager
		*/
		inline ProgramCacheManager& getProgramCacheManager();

		// TODO(co)
		Renderer::IPipelineStatePtr getPipelineStateObjectPtr(const ShaderProperties& shaderProperties, const MaterialProperties& materialProperties);
		void clearCache();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit PipelineStateCacheManager(MaterialBlueprintResource& materialBlueprintResource);
		inline ~PipelineStateCacheManager();
		PipelineStateCacheManager(const PipelineStateCacheManager&) = delete;
		PipelineStateCacheManager& operator=(const PipelineStateCacheManager&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		MaterialBlueprintResource& mMaterialBlueprintResource;	///< Owner material blueprint resource
		ProgramCacheManager		   mProgramCacheManager;

		// TODO(co) Pipeline state cache management
		PipelineStateCache* mPipelineStateCache;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateCacheManager.inl"
