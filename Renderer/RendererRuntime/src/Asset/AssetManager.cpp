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
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/Asset/AssetPackage.h"
#include "RendererRuntime/Asset/Serializer/AssetPackageSerializer.h"
#include "RendererRuntime/Core/File/IFileManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <cassert>
#include <algorithm>


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

	AssetPackage& AssetManager::addAssetPackage(AssetPackageId assetPackageId)
	{
		assert(nullptr == tryGetAssetPackageById(assetPackageId) && "Asset package ID is already used");
		AssetPackage* assetPackage = new AssetPackage(assetPackageId);
		mAssetPackageVector.push_back(assetPackage);
		return *assetPackage;
	}

	void AssetManager::addAssetPackageByFilename(AssetPackageId assetPackageId, const char* filename)
	{
		assert(nullptr == tryGetAssetPackageById(assetPackageId) && "Asset package ID is already used");
		IFileManager& fileManager = mRendererRuntime.getFileManager();
		IFile* file = fileManager.openFile(IFileManager::FileMode::READ, filename);
		if (nullptr != file)
		{
			AssetPackage* assetPackage = new AssetPackage(assetPackageId);
			AssetPackageSerializer().loadAssetPackage(*assetPackage, *file);
			mAssetPackageVector.push_back(assetPackage);
			fileManager.closeFile(*file);
		}
		else
		{
			// Error! This is horrible. No assets.
			assert(false);
		}
	}

	AssetPackage* AssetManager::tryGetAssetPackageById(AssetPackageId assetPackageId) const
	{
		AssetPackageVector::const_iterator iterator = std::find_if(mAssetPackageVector.cbegin(), mAssetPackageVector.cend(),
			[assetPackageId](const AssetPackage* assetPackage) { return (assetPackage->getAssetPackageId() == assetPackageId); }
			);
		return (iterator != mAssetPackageVector.cend()) ? *iterator : nullptr;
	}

	AssetPackage& AssetManager::getAssetPackageById(AssetPackageId assetPackageId) const
	{
		AssetPackageVector::const_iterator iterator = std::find_if(mAssetPackageVector.cbegin(), mAssetPackageVector.cend(),
			[assetPackageId](const AssetPackage* assetPackage) { return (assetPackage->getAssetPackageId() == assetPackageId); }
			);
		assert(iterator != mAssetPackageVector.cend() && "Unknown asset package ID");
		return **iterator;
	}

	void AssetManager::removeAssetPackage(AssetPackageId assetPackageId)
	{
		AssetPackageVector::const_iterator iterator = std::find_if(mAssetPackageVector.cbegin(), mAssetPackageVector.cend(),
			[assetPackageId](const AssetPackage* assetPackage) { return (assetPackage->getAssetPackageId() == assetPackageId); }
			);
		assert(iterator != mAssetPackageVector.cend() && "Unknown asset package ID");
		delete *iterator;
		mAssetPackageVector.erase(iterator);
	}

	const Asset* AssetManager::tryGetAssetByAssetId(AssetId assetId) const
	{
		// Search inside all mounted asset packages, later added asset packages cover old ones
		const size_t numberOfAssetPackages = mAssetPackageVector.size();
		for (size_t i = 0; i < numberOfAssetPackages; ++i)
		{
			const Asset* asset = mAssetPackageVector[i]->tryGetAssetByAssetId(assetId);
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
