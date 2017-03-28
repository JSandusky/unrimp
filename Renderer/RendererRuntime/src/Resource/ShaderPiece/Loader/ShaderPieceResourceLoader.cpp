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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/ShaderPiece/Loader/ShaderPieceResourceLoader.h"
#include "RendererRuntime/Resource/ShaderPiece/Loader/ShaderPieceFileFormat.h"
#include "RendererRuntime/Resource/ShaderPiece/ShaderPieceResource.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResource.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Core/File/IFile.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId ShaderPieceResourceLoader::TYPE_ID("shader_piece");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void ShaderPieceResourceLoader::onDeserialization(IFile& file)
	{
		// Read in the shader piece header
		v1ShaderPiece::Header shaderPieceHeader;
		file.read(&shaderPieceHeader, sizeof(v1ShaderPiece::Header));
		assert(v1ShaderPiece::FORMAT_TYPE == shaderPieceHeader.formatType);
		assert(v1ShaderPiece::FORMAT_VERSION == shaderPieceHeader.formatVersion);

		// Allocate more temporary memory, if required
		if (mMaximumNumberOfShaderSourceCodeBytes < shaderPieceHeader.numberOfShaderSourceCodeBytes)
		{
			mMaximumNumberOfShaderSourceCodeBytes = shaderPieceHeader.numberOfShaderSourceCodeBytes;
			delete [] mShaderSourceCode;
			mShaderSourceCode = new char[mMaximumNumberOfShaderSourceCodeBytes];
		}

		// Read the shader piece ASCII source code
		file.read(mShaderSourceCode, shaderPieceHeader.numberOfShaderSourceCodeBytes);
		mShaderPieceResource->mShaderSourceCode.assign(mShaderSourceCode, mShaderSourceCode + shaderPieceHeader.numberOfShaderSourceCodeBytes);
	}

	bool ShaderPieceResourceLoader::onDispatch()
	{
		{ // TODO(co) Cleanup: Get all influenced material blueprint resources
			const ShaderPieceResourceId shaderPieceResourceId = mShaderPieceResource->getId();
			const ShaderBlueprintResourceManager& shaderBlueprintResourceManager = mRendererRuntime.getShaderBlueprintResourceManager();
			typedef std::unordered_set<MaterialBlueprintResource*> MaterialBlueprintResourcePointers;
			MaterialBlueprintResourcePointers materialBlueprintResourcePointers;
			const MaterialBlueprintResourceManager& materialBlueprintResourceManager = mRendererRuntime.getMaterialBlueprintResourceManager();
			const uint32_t numberOfElements = materialBlueprintResourceManager.getNumberOfResources();
			for (uint32_t i = 0; i < numberOfElements; ++i)
			{
				MaterialBlueprintResource& materialBlueprintResource = materialBlueprintResourceManager.getByIndex(i);
				for (uint8_t shaderType = 0; shaderType < NUMBER_OF_SHADER_TYPES; ++shaderType)
				{
					const ShaderBlueprintResourceId shaderBlueprintResourceId = materialBlueprintResource.getShaderBlueprintResourceId(static_cast<ShaderType>(shaderType));
					if (isInitialized(shaderBlueprintResourceId))
					{
						const ShaderBlueprintResource::IncludeShaderPieceResourceIds& includeShaderPieceResourceIds = shaderBlueprintResourceManager.getById(shaderBlueprintResourceId).getIncludeShaderPieceResourceIds();
						if (std::find(includeShaderPieceResourceIds.cbegin(), includeShaderPieceResourceIds.cend(), shaderPieceResourceId) != includeShaderPieceResourceIds.cend())
						{
							materialBlueprintResourcePointers.insert(&materialBlueprintResource);
							break;
						}
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

		// Fully loaded
		return true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
