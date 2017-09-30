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
#include "RendererToolkit/AssetCompiler/IAssetCompiler.h"
#include "RendererToolkit/Helper/StringHelper.h"

#include <RendererRuntime/Core/Math/Math.h>
#include <RendererRuntime/Asset/AssetPackage.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	void IAssetCompiler::outputAsset(const RendererRuntime::IFileManager& fileManager, const std::string& assetIdAsString, const std::string& outputAssetFilename, RendererRuntime::AssetPackage& outputAssetPackage)
	{
		// Sanity check
		if (outputAssetFilename.size() > RendererRuntime::Asset::MAXIMUM_ASSET_FILENAME_LENGTH)
		{
			throw std::runtime_error("The output asset filename \"" + outputAssetFilename + "\" exceeds the length limit of " + std::to_string(RendererRuntime::Asset::MAXIMUM_ASSET_FILENAME_LENGTH));
		}

		// Output asset
		RendererRuntime::Asset outputAsset;
		outputAsset.assetId = StringHelper::getAssetIdByString(assetIdAsString.c_str());
		outputAsset.fileHash = RendererRuntime::Math::calculateFileFNV1a64ByFilename(fileManager, outputAssetFilename);
		strcpy(outputAsset.assetFilename, outputAssetFilename.c_str());
		outputAssetPackage.getWritableSortedAssetVector().push_back(outputAsset);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
