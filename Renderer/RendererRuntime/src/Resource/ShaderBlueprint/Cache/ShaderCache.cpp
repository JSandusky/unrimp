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
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderCache.h"
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderBuilder.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	ShaderCache::ShaderCache(ShaderCacheManager& shaderCacheManager, const ShaderProperties& shaderProperties) :
		mShaderCacheManager(shaderCacheManager),
		mShaderProperties(shaderProperties)
	{
		// TODO(co)
		/*
		const MaterialBlueprintResource& materialBlueprintResource = programCacheManager.getPipelineStateCacheManager().getMaterialBlueprintResource();
		const Renderer::IRootSignaturePtr rootSignaturePtr = materialBlueprintResource.getRootSignaturePtr();
		Renderer::IRenderer& renderer = rootSignaturePtr->getRenderer();

		// Decide which shader language should be used (for example "GLSL" or "HLSL")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer.getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			// TODO(co) Asynchronous program cache generation, use fallback while the program cache is not available

			// TODO(co) Use shader cache

			// Create the vertex shader
			Renderer::IVertexShader* vertexShader = nullptr;
			{
				ShaderBuilder shaderBuilder;
				vertexShader = shaderLanguage->createVertexShaderFromSourceCode(materialBlueprintResource.getVertexAttributes(), shaderBuilder.createSourceCode(*materialBlueprintResource.mVertexShaderBlueprint, shaderProperties).c_str());
			}

			// Create the fragment shader
			Renderer::IFragmentShader* fragmentShader = nullptr;
			{
				ShaderBuilder shaderBuilder;
				fragmentShader = shaderLanguage->createFragmentShaderFromSourceCode(shaderBuilder.createSourceCode(*materialBlueprintResource.mFragmentShaderBlueprint, shaderProperties).c_str());
			}

			// Create the program
			mProgramPtr = shaderLanguage->createProgram(*rootSignaturePtr, materialBlueprintResource.getVertexAttributes(), vertexShader, fragmentShader);
		}
		*/
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
