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
#include "RendererRuntime/Resource/ShaderBlueprint/Loader/ShaderBlueprintResourceLoader.h"
#include "RendererRuntime/Resource/ShaderBlueprint/Loader/ShaderBlueprintFileFormat.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/Resource/ShaderPiece/ShaderPieceResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Asset/IFile.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <unordered_set>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId ShaderBlueprintResourceLoader::TYPE_ID("shader_blueprint");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void ShaderBlueprintResourceLoader::onDeserialization(IFile& file)
	{
		// Read in the shader blueprint header
		v1ShaderBlueprint::Header shaderBlueprintHeader;
		file.read(&shaderBlueprintHeader, sizeof(v1ShaderBlueprint::Header));

		// Allocate more temporary memory, if required
		if (mMaximumNumberOfIncludeShaderPieceAssetIds < shaderBlueprintHeader.numberOfIncludeShaderPieceAssetIds)
		{
			mMaximumNumberOfIncludeShaderPieceAssetIds = shaderBlueprintHeader.numberOfIncludeShaderPieceAssetIds;
			delete [] mIncludeShaderPieceAssetIds;
			mIncludeShaderPieceAssetIds = new AssetId[mMaximumNumberOfIncludeShaderPieceAssetIds];
		}
		if (mMaximumNumberOfShaderSourceCodeBytes < shaderBlueprintHeader.numberOfShaderSourceCodeBytes)
		{
			mMaximumNumberOfShaderSourceCodeBytes = shaderBlueprintHeader.numberOfShaderSourceCodeBytes;
			delete [] mShaderSourceCode;
			mShaderSourceCode = new char[mMaximumNumberOfShaderSourceCodeBytes];
		}

		// Read the asset IDs of the shader pieces to include
		file.read(mIncludeShaderPieceAssetIds, sizeof(AssetId) * shaderBlueprintHeader.numberOfIncludeShaderPieceAssetIds);
		mShaderBlueprintResource->mIncludeShaderPieceResourceIds.resize(shaderBlueprintHeader.numberOfIncludeShaderPieceAssetIds);

		{ // Read the referenced shader properties
			ShaderProperties::SortedPropertyVector& sortedPropertyVector = mShaderBlueprintResource->mReferencedShaderProperties.getSortedPropertyVector();
			sortedPropertyVector.resize(shaderBlueprintHeader.numberReferencedShaderProperties);
			file.read(sortedPropertyVector.data(), sizeof(ShaderProperties::Property) * shaderBlueprintHeader.numberReferencedShaderProperties);
		}

		// Read the shader blueprint ASCII source code
		file.read(mShaderSourceCode, shaderBlueprintHeader.numberOfShaderSourceCodeBytes);
		mShaderBlueprintResource->mShaderSourceCode.assign(mShaderSourceCode, mShaderSourceCode + shaderBlueprintHeader.numberOfShaderSourceCodeBytes);
	}

	bool ShaderBlueprintResourceLoader::onDispatch()
	{
		{ // Read the shader piece resources to include
			ShaderPieceResourceManager& shaderPieceResourceManager = mRendererRuntime.getShaderPieceResourceManager();
			ShaderBlueprintResource::IncludeShaderPieceResourceIds& includeShaderPieceResourceIds = mShaderBlueprintResource->mIncludeShaderPieceResourceIds;
			const size_t numberOfShaderPieceResources = includeShaderPieceResourceIds.size();
			const AssetId* includeShaderPieceAssetIds = mIncludeShaderPieceAssetIds;
			for (size_t i = 0; i < numberOfShaderPieceResources; ++i, ++includeShaderPieceAssetIds)
			{
				includeShaderPieceResourceIds[i] = shaderPieceResourceManager.loadShaderPieceResourceByAssetId(*includeShaderPieceAssetIds);
			}
		}

		{ // TODO(co) Cleanup: Get all influenced material blueprint resources
			const ShaderBlueprintResourceId shaderBlueprintResourceId = mShaderBlueprintResource->getId();
			typedef std::unordered_set<MaterialBlueprintResource*> MaterialBlueprintResourcePointers;
			MaterialBlueprintResourcePointers materialBlueprintResourcePointers;
			const MaterialBlueprintResources& materialBlueprintResources = mRendererRuntime.getMaterialBlueprintResourceManager().getMaterialBlueprintResources();
			const uint32_t numberOfElements = materialBlueprintResources.getNumberOfElements();
			for (uint32_t i = 0; i < numberOfElements; ++i)
			{
				MaterialBlueprintResource& materialBlueprintResource = materialBlueprintResources.getElementByIndex(i);
				for (uint8_t shaderType = 0; shaderType < NUMBER_OF_SHADER_TYPES; ++shaderType)
				{
					if (materialBlueprintResource.getShaderBlueprintResourceId(static_cast<ShaderType>(shaderType)) == shaderBlueprintResourceId)
					{
						materialBlueprintResourcePointers.insert(&materialBlueprintResource);
						break;
					}
				}
			}
			for (MaterialBlueprintResource* materialBlueprintResource : materialBlueprintResourcePointers)
			{
				materialBlueprintResource->getPipelineStateCacheManager().clearCache();
				materialBlueprintResource->getPipelineStateCacheManager().getProgramCacheManager().clearCache();
			}

			// TODO(co) Do only clear the influenced shader cache entries
			mRendererRuntime.getShaderBlueprintResourceManager().getShaderCacheManager().clearCache();
		}

		// Fully loaded?
		return isFullyLoaded();
	}

	bool ShaderBlueprintResourceLoader::isFullyLoaded()
	{
		const ShaderPieceResourceManager& shaderPieceResourceManager = mRendererRuntime.getShaderPieceResourceManager();
		for (ShaderPieceResourceId shaderPieceResourceId : mShaderBlueprintResource->mIncludeShaderPieceResourceIds)
		{
			if (isInitialized(shaderPieceResourceId) && IResource::LoadingState::LOADED != shaderPieceResourceManager.getResourceByResourceId(shaderPieceResourceId).getLoadingState())
			{
				// Not fully loaded
				return false;
			}
		}

		// Fully loaded
		return true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
