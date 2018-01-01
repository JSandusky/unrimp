/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "RendererRuntime/Core/GetUninitialized.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline AssetPackage::AssetPackage() :
		mAssetPackageId(getUninitialized<AssetPackageId>())
	{
		// Nothing here
	}

	inline AssetPackage::AssetPackage(AssetPackageId assetPackageId) :
		mAssetPackageId(assetPackageId)
	{
		// Nothing here
	}

	inline AssetPackage::~AssetPackage()
	{
		// Nothing here
	}

	inline AssetPackageId AssetPackage::getAssetPackageId() const
	{
		return mAssetPackageId;
	}

	inline void AssetPackage::clear()
	{
		mSortedAssetVector.clear();
	}

	inline const AssetPackage::SortedAssetVector& AssetPackage::getSortedAssetVector() const
	{
		return mSortedAssetVector;
	}

	inline VirtualFilename AssetPackage::tryGetVirtualFilenameByAssetId(AssetId assetId) const
	{
		const Asset* asset = tryGetAssetByAssetId(assetId);
		return (nullptr != asset) ? asset->virtualFilename : nullptr;
	}

	inline AssetPackage::SortedAssetVector& AssetPackage::getWritableSortedAssetVector()
	{
		return mSortedAssetVector;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
