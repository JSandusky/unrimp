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

			// Read in the material blueprint header
			v1MaterialBlueprint::Header materialBlueprintHeader;
			inputFileStream.read(reinterpret_cast<char*>(&materialBlueprintHeader), sizeof(v1MaterialBlueprint::Header));

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
		// TODO(co) Create dynamic material blueprint specific root signature, currently hard coded to have something to start with
		{ // Create the root signature
			Renderer::DescriptorRangeBuilder ranges[6];
			ranges[0].initialize(Renderer::DescriptorRangeType::CBV, 1, 0, "UniformBlockDynamicVs", 0);
			ranges[1].initializeSampler(1, 0);
			ranges[2].initialize(Renderer::DescriptorRangeType::SRV, 1, 0, "DiffuseMap", 0);
			ranges[3].initialize(Renderer::DescriptorRangeType::SRV, 1, 1, "EmissiveMap", 0);
			ranges[4].initialize(Renderer::DescriptorRangeType::SRV, 1, 2, "NormalMap", 0);
			ranges[5].initialize(Renderer::DescriptorRangeType::SRV, 1, 3, "SpecularMap", 0);

			Renderer::RootParameterBuilder rootParameters[6];
			rootParameters[0].initializeAsDescriptorTable(1, &ranges[0], Renderer::ShaderVisibility::VERTEX);
			rootParameters[1].initializeAsDescriptorTable(1, &ranges[1], Renderer::ShaderVisibility::FRAGMENT);
			rootParameters[2].initializeAsDescriptorTable(1, &ranges[2], Renderer::ShaderVisibility::FRAGMENT);
			rootParameters[3].initializeAsDescriptorTable(1, &ranges[3], Renderer::ShaderVisibility::FRAGMENT);
			rootParameters[4].initializeAsDescriptorTable(1, &ranges[4], Renderer::ShaderVisibility::FRAGMENT);
			rootParameters[5].initializeAsDescriptorTable(1, &ranges[5], Renderer::ShaderVisibility::FRAGMENT);

			// Setup
			Renderer::RootSignatureBuilder rootSignature;
			rootSignature.initialize(sizeof(rootParameters) / sizeof(Renderer::RootParameter), rootParameters, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			// Create the instance
			mMaterialBlueprintResource->mRootSignature = mRendererRuntime.getRenderer().createRootSignature(rootSignature);
			mMaterialBlueprintResource->mRootSignature->addReference();
		}

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
