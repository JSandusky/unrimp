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
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateCache.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/PipelineStateCacheManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Cache/ProgramCache.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	PipelineStateCache::PipelineStateCache(PipelineStateCacheManager& pipelineStateCacheManager, const ShaderProperties& shaderProperties, const MaterialProperties& materialProperties) :
		mPipelineStateCacheManager(pipelineStateCacheManager),
		mShaderProperties(shaderProperties),
		mMaterialProperties(materialProperties)
	{
		// Gather shader properties from static material properties
		ShaderProperties finalShaderProperties = shaderProperties;
		{
			const MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector = materialProperties.getSortedPropertyVector();
			const size_t numberOfMaterialProperties = sortedMaterialPropertyVector.size();
			for (size_t i = 0; i < numberOfMaterialProperties; ++i)
			{
				const MaterialProperty& materialProperty = sortedMaterialPropertyVector[i];
				if (materialProperty.getUsage() == MaterialProperty::Usage::STATIC)
				{
					finalShaderProperties.setPropertyValue(materialProperty.getMaterialPropertyId(), materialProperty.getBooleanValue());
				}
			}
		}

		// Get the program cache
		const ProgramCache* programCache = pipelineStateCacheManager.getProgramCacheManager().getProgramCache(finalShaderProperties);
		if (nullptr != programCache)
		{
			Renderer::IProgramPtr programPtr = programCache->getProgramPtr();
			if (nullptr != programPtr)
			{
				const MaterialBlueprintResource& materialBlueprintResource = pipelineStateCacheManager.getMaterialBlueprintResource();
				Renderer::IRootSignaturePtr rootSignaturePtr = materialBlueprintResource.getRootSignaturePtr();

				// Start with the pipeline state of the material blueprint resource
				mPipelineState = materialBlueprintResource.getPipelineState();

				// Setup the dynamic part of the pipeline state
				mPipelineState.rootSignature	= rootSignaturePtr;
				mPipelineState.program			= programPtr;
				mPipelineState.vertexAttributes = materialBlueprintResource.getVertexAttributes();

				// Create the pipeline state object (PSO)
				// TODO(co) Asynchronous pipeline state object generation, use fallback while the pipeline state object is not available
				mPipelineStateObjectPtr = rootSignaturePtr->getRenderer().createPipelineState(mPipelineState);
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
