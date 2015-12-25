/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintResourceLoader.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintFileFormat.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId MaterialBlueprintResourceLoader::TYPE_ID("material_blueprint");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	ResourceLoaderTypeId MaterialBlueprintResourceLoader::getResourceLoaderTypeId() const
	{
		return TYPE_ID;
	}

	void MaterialBlueprintResourceLoader::onDeserialization()
	{
		// TODO(co) Error handling
		try
		{
			std::ifstream inputFileStream(mAsset.assetFilename, std::ios::binary);

			{ // Read in the material blueprint header
				v1MaterialBlueprint::Header materialBlueprintHeader;
				inputFileStream.read(reinterpret_cast<char*>(&materialBlueprintHeader), sizeof(v1MaterialBlueprint::Header));
			}

			{ // Read in the root signature
				// Read in the root signature header
				v1MaterialBlueprint::RootSignatureHeader rootSignatureHeader;
				inputFileStream.read(reinterpret_cast<char*>(&rootSignatureHeader), sizeof(v1MaterialBlueprint::RootSignatureHeader));

				// Allocate memory for the temporary data
				if (mMaximumNumberOfRootParameters < rootSignatureHeader.numberOfRootParameters)
				{
					delete [] mRootParameters;
					mMaximumNumberOfRootParameters = rootSignatureHeader.numberOfRootParameters;
					mRootParameters = new Renderer::RootParameter[mMaximumNumberOfRootParameters];
				}
				if (mMaximumNumberOfDescriptorRanges < rootSignatureHeader.numberOfDescriptorRanges)
				{
					delete [] mDescriptorRanges;
					mMaximumNumberOfDescriptorRanges = rootSignatureHeader.numberOfDescriptorRanges;
					mDescriptorRanges = new Renderer::DescriptorRange[mMaximumNumberOfDescriptorRanges];
				}

				// Load in the root parameters
				inputFileStream.read(reinterpret_cast<char*>(mRootParameters), sizeof(Renderer::RootParameter) * rootSignatureHeader.numberOfRootParameters);

				// Load in the descriptor ranges
				inputFileStream.read(reinterpret_cast<char*>(mDescriptorRanges), sizeof(Renderer::DescriptorRange) * rootSignatureHeader.numberOfDescriptorRanges);

				// Prepare our temporary root signature
				mRootSignature.numberOfParameters	  = rootSignatureHeader.numberOfRootParameters;
				mRootSignature.parameters			  = mRootParameters;
				mRootSignature.numberOfStaticSamplers = rootSignatureHeader.numberOfStaticSamplers;
				mRootSignature.staticSamplers		  = nullptr;	// TODO(co) Add support for static samplers
				mRootSignature.flags				  = static_cast<Renderer::RootSignatureFlags::Enum>(rootSignatureHeader.flags);

				{ // Tell the temporary root signature about the descriptor ranges
					Renderer::DescriptorRange* descriptorRange = mDescriptorRanges;
					for (uint32_t i = 0; i < rootSignatureHeader.numberOfRootParameters; ++i)
					{
						Renderer::RootParameter& rootParameter = mRootParameters[i];
						if (Renderer::RootParameterType::DESCRIPTOR_TABLE == rootParameter.parameterType)
						{
							rootParameter.descriptorTable.descriptorRanges = descriptorRange;
							descriptorRange += rootParameter.descriptorTable.numberOfDescriptorRanges;
						}
					}
				}
			}

			// TODO(co) The first few bytes are unused and there are probably byte alignment issues which can come up. On the other hand, this solution is wonderful simple.
			// Read in the pipeline state
			inputFileStream.read(reinterpret_cast<char*>(&mMaterialBlueprintResource->mPipelineState), sizeof(Renderer::PipelineState));

			{ // Read in the shader blueprints
				v1MaterialBlueprint::ShaderBlueprints shaderBlueprints;
				inputFileStream.read(reinterpret_cast<char*>(&shaderBlueprints), sizeof(v1MaterialBlueprint::ShaderBlueprints));
				mVertexShaderBlueprintAssetId				  = shaderBlueprints.vertexShaderBlueprintAssetId;
				mTessellationControlShaderBlueprintAssetId	  = shaderBlueprints.tessellationControlShaderBlueprintAssetId;
				mTessellationEvaluationShaderBlueprintAssetId = shaderBlueprints.tessellationEvaluationShaderBlueprintAssetId;
				mGeometryShaderBlueprintAssetId				  = shaderBlueprints.geometryShaderBlueprintAssetId;
				mFragmentShaderBlueprintAssetId				  = shaderBlueprints.fragmentShaderBlueprintAssetId;
			}

			// TODO(co) The rest of the material blueprint
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load material blueprint asset %d: %s", mAsset.assetId, e.what());
		}
	}

	void MaterialBlueprintResourceLoader::onProcessing()
	{
		// Nothing here
	}

	void MaterialBlueprintResourceLoader::onRendererBackendDispatch()
	{
		// Create the root signature
		mMaterialBlueprintResource->mRootSignature = mRendererRuntime.getRenderer().createRootSignature(mRootSignature);
		mMaterialBlueprintResource->mRootSignature->addReference();

		// Get the used shader blueprint resources
		ShaderBlueprintResourceManager& shaderBlueprintResourceManager = mRendererRuntime.getShaderBlueprintResourceManager();
		mMaterialBlueprintResource->mVertexShaderBlueprint				   = shaderBlueprintResourceManager.loadShaderBlueprintResourceByAssetId(mVertexShaderBlueprintAssetId);
		mMaterialBlueprintResource->mTessellationControlShaderBlueprint    = shaderBlueprintResourceManager.loadShaderBlueprintResourceByAssetId(mTessellationControlShaderBlueprintAssetId);
		mMaterialBlueprintResource->mTessellationEvaluationShaderBlueprint = shaderBlueprintResourceManager.loadShaderBlueprintResourceByAssetId(mTessellationEvaluationShaderBlueprintAssetId);
		mMaterialBlueprintResource->mGeometryShaderBlueprint			   = shaderBlueprintResourceManager.loadShaderBlueprintResourceByAssetId(mGeometryShaderBlueprintAssetId);
		mMaterialBlueprintResource->mFragmentShaderBlueprint			   = shaderBlueprintResourceManager.loadShaderBlueprintResourceByAssetId(mFragmentShaderBlueprintAssetId);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
