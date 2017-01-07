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
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/Asset/AssetPackage.h"
#include "RendererRuntime/Asset/Serializer/AssetPackageSerializer.h"

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void AssetManager::clear()
	{
		const size_t numberOfAssetPackages = mAssetPackageVector.size();
		for (size_t i = 0; i < numberOfAssetPackages; ++i)
		{
			delete mAssetPackageVector[i];
		}
		mAssetPackageVector.clear();
	}

	void AssetManager::addAssetPackageByFilename(const char* filename)
	{
		try
		{
			std::ifstream inputFileStream(filename, std::ios::binary);

			if (!inputFileStream)
			{
				RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load asset package. Could not open file: %s", filename);
				return;
			}
			mAssetPackageVector.push_back(AssetPackageSerializer().loadAssetPackage(inputFileStream));
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load asset package \"%s\": %s", filename, e.what());
		}
	}

	const Asset* AssetManager::getAssetByAssetId(AssetId assetId) const
	{
		// Search inside all mounted asset packages, later added asset packages cover old ones
		const size_t numberOfAssetPackages = mAssetPackageVector.size();
		for (size_t i = 0; i < numberOfAssetPackages; ++i)
		{
			const Asset* asset = mAssetPackageVector[i]->getAssetByAssetId(assetId);
			if (nullptr != asset)
			{
				return asset;
			}
		}

		// Sorry, the given asset ID is unknown
		return nullptr;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
