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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderCacheManager.h"
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderCache.h"
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderBuilder.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateSignature.h"
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
	ShaderCache* ShaderCacheManager::getShaderCache(const PipelineStateSignature& pipelineStateSignature, const MaterialBlueprintResource& materialBlueprintResource, Renderer::IShaderLanguage& shaderLanguage, ShaderType shaderType)
	{
		ShaderCache* shaderCache = nullptr;

		// Get the shader blueprint resource ID
		const ShaderBlueprintResourceId shaderBlueprintResourceId = materialBlueprintResource.getShaderBlueprintResourceId(shaderType);
		if (isInitialized(shaderBlueprintResourceId))
		{
			// Get the shader cache identifier, often but not always identical to the shader combination ID
			const ShaderCacheId shaderCacheId = pipelineStateSignature.getShaderCombinationId(shaderType);

			// Does the shader cache already exist?
			std::unique_lock<std::mutex> mutexLock(mMutex);
			ShaderCacheByShaderCacheId::const_iterator shaderCacheIdIterator = mShaderCacheByShaderCacheId.find(shaderCacheId);
			if (shaderCacheIdIterator != mShaderCacheByShaderCacheId.cend())
			{
				shaderCache = shaderCacheIdIterator->second;
			}
			else
			{
				// Try to create the new program cache instance
				const ShaderBlueprintResource* shaderBlueprintResource = mShaderBlueprintResourceManager.getShaderBlueprintResources().tryGetElementById(shaderBlueprintResourceId);
				if (nullptr != shaderBlueprintResource)
				{
					// Build the shader source code
					ShaderBuilder shaderBuilder;
					const std::string& sourceCode = shaderBuilder.createSourceCode(mShaderBlueprintResourceManager.getRendererRuntime().getShaderPieceResourceManager(), *shaderBlueprintResource, pipelineStateSignature.getShaderProperties());
					if (sourceCode.empty())
					{
						// TODO(co) Error handling
						assert(false);
					}
					else
					{
						// Generate the shader source code ID
						// -> Especially in complex shaders, there are situations where different shader combinations result in one and the same shader source code
						// -> Shader compilation is considered to be expensive, so we need to be pretty sure that we really need to perform this heavy work
						const ShaderSourceCodeId shaderSourceCodeId = Math::calculateFNV1a(reinterpret_cast<const uint8_t*>(sourceCode.c_str()), sourceCode.size());
						ShaderCacheByShaderSourceCodeId::const_iterator shaderSourceCodeIdIterator = mShaderCacheByShaderSourceCodeId.find(shaderSourceCodeId);
						if (shaderSourceCodeIdIterator != mShaderCacheByShaderSourceCodeId.cend() && nullptr != shaderSourceCodeIdIterator->second->getShaderPtr().getPointer())
						{
							// Reuse already existing shader instance
							// -> We still have to create a shader cache instance so we don't need to build the shader source code again next time
							shaderCache = new ShaderCache(shaderCacheId, shaderSourceCodeIdIterator->second);
							mShaderCacheByShaderCacheId.emplace(shaderCacheId, shaderCache);
						}
						else
						{
							// Create the shader instance
							Renderer::IShader* shader = nullptr;
							switch (shaderType)
							{
								case ShaderType::Vertex:
									shader = shaderLanguage.createVertexShaderFromSourceCode(materialBlueprintResource.getVertexAttributes(), sourceCode.c_str());
									break;

								case ShaderType::TessellationControl:
									shader = shaderLanguage.createTessellationControlShaderFromSourceCode(sourceCode.c_str());
									break;

								case ShaderType::TessellationEvaluation:
									shader = shaderLanguage.createTessellationEvaluationShaderFromSourceCode(sourceCode.c_str());
									break;

								case ShaderType::Geometry:
									// TODO(co) "RendererRuntime::ShaderCacheManager::getShaderCache()" needs to provide additional geometry shader information
									// shader = shaderLanguage.createGeometryShaderFromSourceCode(sourceCode.c_str());
									break;

								case ShaderType::Fragment:
									shader = shaderLanguage.createFragmentShaderFromSourceCode(sourceCode.c_str());
									break;
							}

							// Create the new shader cache instance
							if (nullptr != shader)
							{
								shaderCache = new ShaderCache(shaderCacheId, *shader);
								mShaderCacheByShaderCacheId.emplace(shaderCacheId, shaderCache);
								mShaderCacheByShaderSourceCodeId.emplace(shaderSourceCodeId, shaderCache);
							}
							else
							{
								// TODO(co) Error handling
								assert(false);
							}
						}
					}
				}
				else
				{
					// TODO(co) Error handling
					assert(false);
				}
			}
		}

		// Done
		return shaderCache;
	}

	void ShaderCacheManager::clearCache()
	{
		std::unique_lock<std::mutex> mutexLock(mMutex);
		for (auto& shaderCacheElement : mShaderCacheByShaderCacheId)
		{
			delete shaderCacheElement.second;
		}
		mShaderCacheByShaderCacheId.clear();
		mShaderCacheByShaderSourceCodeId.clear();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
