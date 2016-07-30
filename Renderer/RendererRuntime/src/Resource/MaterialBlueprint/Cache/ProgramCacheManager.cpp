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
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/ProgramCacheManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateSignature.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/ProgramCache.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderBuilder.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/Core/Math/Math.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProgramCache* ProgramCacheManager::getProgramCacheByPipelineStateSignature(const PipelineStateSignature& pipelineStateSignature)
	{
		// Generate the program cache ID
		ProgramCacheId programCacheId = Math::FNV1a_INITIAL_HASH;
		for (uint8_t i = 0; i < NUMBER_OF_SHADER_TYPES; ++i)
		{
			const ShaderCombinationId shaderCombinationId = pipelineStateSignature.getShaderCombinationId(static_cast<ShaderType>(i));
			if (isInitialized(shaderCombinationId))
			{
				programCacheId = Math::calculateFNV1a(reinterpret_cast<const uint8_t*>(&shaderCombinationId), sizeof(ShaderCombinationId), programCacheId);
			}
		}

		{ // Does the program cache already exist?
			ProgramCacheById::const_iterator iterator = mProgramCacheById.find(programCacheId);
			if (iterator != mProgramCacheById.cend())
			{
				return iterator->second;
			}
		}

		// Create the new program cache instance
		ProgramCache* programCache = new ProgramCache(programCacheId);
		mProgramCacheById.insert(std::make_pair(programCacheId, programCache));

		// Create the renderer program: Decide which shader language should be used (for example "GLSL" or "HLSL")
		const MaterialBlueprintResource& materialBlueprintResource = mPipelineStateCacheManager.getMaterialBlueprintResource();
		const Renderer::IRootSignaturePtr rootSignaturePtr = materialBlueprintResource.getRootSignaturePtr();
		Renderer::IRenderer& renderer = rootSignaturePtr->getRenderer();
		Renderer::IShaderLanguagePtr shaderLanguage(renderer.getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			// TODO(co) Use shader cache

			// Get the required resource manager instances
			const IRendererRuntime& rendererRuntime = mPipelineStateCacheManager.getMaterialBlueprintResource().getResourceManager<MaterialBlueprintResourceManager>().getRendererRuntime();
			const ShaderPieceResourceManager& shaderPieceResourceManager = rendererRuntime.getShaderPieceResourceManager();
			const ShaderBlueprintResources& shaderBlueprintResources = rendererRuntime.getShaderBlueprintResourceManager().getShaderBlueprintResources();

			// Create the shaders
			Renderer::IShader* shader[NUMBER_OF_SHADER_TYPES] = {};
			for (uint8_t i = 0; i < NUMBER_OF_SHADER_TYPES; ++i)
			{
				const ShaderBlueprintResource* shaderBlueprintResource = shaderBlueprintResources.tryGetElementById(materialBlueprintResource.getShaderBlueprintResourceId(static_cast<ShaderType>(i)));
				if (nullptr != shaderBlueprintResource)
				{
					ShaderBuilder shaderBuilder;
					const std::string sourceCode = shaderBuilder.createSourceCode(shaderPieceResourceManager, *shaderBlueprintResource, pipelineStateSignature.getShaderProperties());
					switch (static_cast<ShaderType>(i))
					{
						case ShaderType::Vertex:
							shader[i] = shaderLanguage->createVertexShaderFromSourceCode(materialBlueprintResource.getVertexAttributes(), sourceCode.c_str());
							break;

						case ShaderType::TessellationControl:
							shader[i] = shaderLanguage->createTessellationControlShaderFromSourceCode(sourceCode.c_str());
							break;

						case ShaderType::TessellationEvaluation:
							shader[i] = shaderLanguage->createTessellationEvaluationShaderFromSourceCode(sourceCode.c_str());
							break;

						case ShaderType::Geometry:
							// TODO(co) "RendererRuntime::ProgramCacheManager::getProgramCacheByPipelineStateSignature()" needs to provide additional geometry shader information
							// shader[i] = shaderLanguage->createGeometryShaderFromSourceCode(sourceCode.c_str());
							break;

						case ShaderType::Fragment:
							shader[i] = shaderLanguage->createFragmentShaderFromSourceCode(sourceCode.c_str());
							break;
					}
				}
			}

			// Create the program
			programCache->mProgramPtr = shaderLanguage->createProgram(*rootSignaturePtr, materialBlueprintResource.getVertexAttributes(),
				static_cast<Renderer::IVertexShader*>(shader[static_cast<int>(ShaderType::Vertex)]),
				static_cast<Renderer::ITessellationControlShader*>(shader[static_cast<int>(ShaderType::TessellationControl)]),
				static_cast<Renderer::ITessellationEvaluationShader*>(shader[static_cast<int>(ShaderType::TessellationEvaluation)]),
				static_cast<Renderer::IGeometryShader*>(shader[static_cast<int>(ShaderType::Geometry)]),
				static_cast<Renderer::IFragmentShader*>(shader[static_cast<int>(ShaderType::Fragment)]));
		}

		// Done
		return programCache;
	}

	void ProgramCacheManager::clearCache()
	{
		for (auto& programCacheElement : mProgramCacheById)
		{
			delete programCacheElement.second;
		}
		mProgramCacheById.clear();
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	ProgramCacheManager::~ProgramCacheManager()
	{
		clearCache();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
