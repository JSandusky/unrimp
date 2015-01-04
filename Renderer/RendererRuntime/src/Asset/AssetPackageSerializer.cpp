/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "RendererRuntime/Asset/AssetPackageSerializer.h"
#include "RendererRuntime/Asset/AssetPackage.h"

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4548)	// warning C4548: expression before comma has no effect; expected expression with side-effect
	#include <fstream>
#pragma warning(pop)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	// TODO(co) Work-in-progress
	AssetPackage* AssetPackageSerializer::loadAssetPackage(std::istream& istream)
	{
		AssetPackage* assetPackage = new AssetPackage;

		// Read in the asset package header
		#pragma pack(push)
		#pragma pack(1)
			struct AssetPackageHeader
			{
				uint32_t formatType;
				uint16_t formatVersion;
				uint32_t numberOfAssets;
			};
		#pragma pack(pop)
		AssetPackageHeader assetPackageHeader;
		istream.read(reinterpret_cast<char*>(&assetPackageHeader), sizeof(AssetPackageHeader));

		// Read in the asset package content in one single burst
		AssetPackage::SortedAssetVector& sortedAssetVector = assetPackage->getWritableSortedAssetVector();
		sortedAssetVector.resize(assetPackageHeader.numberOfAssets);
		istream.read(reinterpret_cast<char*>(sortedAssetVector.data()), sizeof(AssetPackage::Asset) * assetPackageHeader.numberOfAssets);

		// Done
		return assetPackage;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
