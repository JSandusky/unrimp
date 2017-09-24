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
#include "RendererRuntime/Asset/AssetPackage.h"
#include "RendererRuntime/Core/Math/Math.h"

#include <cassert>
#include <algorithm>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Structures                                            ]
		//[-------------------------------------------------------]
		struct OrderByAssetId
		{
			inline bool operator()(const RendererRuntime::Asset& left, RendererRuntime::AssetId right) const
			{
				return (left.assetId < right);
			}

			inline bool operator()(RendererRuntime::AssetId left, const RendererRuntime::Asset& right) const
			{
				return (left < right.assetId);
			}
		};


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void AssetPackage::addAsset(AssetId assetId, const char* assetFilename)
	{
		assert(nullptr == tryGetAssetByAssetId(assetId) && "Asset ID is already used");
		assert((strlen(assetFilename) <= Asset::MAXIMUM_ASSET_FILENAME_LENGTH) && "The asset filename is too long");
		SortedAssetVector::const_iterator iterator = std::lower_bound(mSortedAssetVector.cbegin(), mSortedAssetVector.cend(), assetId, ::detail::OrderByAssetId());
		Asset& asset = *mSortedAssetVector.insert(iterator, Asset());
		asset.assetId = assetId;
		strncpy(asset.assetFilename, assetFilename, Asset::MAXIMUM_ASSET_FILENAME_LENGTH);
	}

	const Asset* AssetPackage::tryGetAssetByAssetId(AssetId assetId) const
	{
		SortedAssetVector::const_iterator iterator = std::lower_bound(mSortedAssetVector.cbegin(), mSortedAssetVector.cend(), assetId, ::detail::OrderByAssetId());
		return (iterator != mSortedAssetVector.end() && iterator->assetId == assetId) ? &(*iterator) : nullptr;
	}

	bool AssetPackage::validateIntegrity() const
	{
		for (const Asset& asset : mSortedAssetVector)
		{
			if (Math::calculateFileFNV1a64ByFilename(asset.assetFilename) != asset.fileHash)
			{
				// Invalid integrity
				return false;
			}
		}

		// Valid integrity
		return true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
