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
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/ProgramCache.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderBuilder.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	ProgramCache::ProgramCache(ProgramCacheManager& programCacheManager, const ShaderProperties& shaderProperties) :
		mProgramCacheManager(programCacheManager),
		mShaderProperties(shaderProperties)
	{
		const MaterialBlueprintResource& materialBlueprintResource = programCacheManager.getPipelineStateCacheManager().getMaterialBlueprintResource();
		const Renderer::IRootSignaturePtr rootSignaturePtr = materialBlueprintResource.getRootSignaturePtr();
		Renderer::IRenderer& renderer = rootSignaturePtr->getRenderer();

		// Decide which shader language should be used (for example "GLSL" or "HLSL")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer.getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			// TODO(co) Asynchronous program cache generation, use fallback while the program cache is not available

			// TODO(co) Use shader cache

			// Get the required resource manager instances
			const IRendererRuntime& rendererRuntime = programCacheManager.getPipelineStateCacheManager().getMaterialBlueprintResource().getMaterialBlueprintResourceManager().getRendererRuntime();
			const ShaderPieceResourceManager& shaderPieceResourceManager = rendererRuntime.getShaderPieceResourceManager();
			const ShaderBlueprintResources& shaderBlueprintResources = rendererRuntime.getShaderBlueprintResourceManager().getShaderBlueprintResources();

			// Create the vertex shader
			Renderer::IVertexShader* vertexShader = nullptr;
			{
				const ShaderBlueprintResource* shaderBlueprintResource = shaderBlueprintResources.tryGetElementById(materialBlueprintResource.getShaderBlueprintResourceId(ShaderType::Vertex));
				if (nullptr != shaderBlueprintResource)
				{
					ShaderBuilder shaderBuilder;
					vertexShader = shaderLanguage->createVertexShaderFromSourceCode(shaderBuilder.createSourceCode(shaderPieceResourceManager, *shaderBlueprintResource, shaderProperties).c_str());
				}
				else
				{
					// TODO(co) Error handling
					assert(false);
				}
			}

			// Create the fragment shader
			Renderer::IFragmentShader* fragmentShader = nullptr;
			{
				const ShaderBlueprintResource* shaderBlueprintResource = shaderBlueprintResources.tryGetElementById(materialBlueprintResource.getShaderBlueprintResourceId(ShaderType::Fragment));
				if (nullptr != shaderBlueprintResource)
				{
					ShaderBuilder shaderBuilder;
					fragmentShader = shaderLanguage->createFragmentShaderFromSourceCode(shaderBuilder.createSourceCode(shaderPieceResourceManager, *shaderBlueprintResource, shaderProperties).c_str());
				}
				else
				{
					// TODO(co) Error handling
					assert(false);
				}
			}

			// Create the program
			mProgramPtr = shaderLanguage->createProgram(*rootSignaturePtr, materialBlueprintResource.getVertexAttributes(), vertexShader, fragmentShader);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
