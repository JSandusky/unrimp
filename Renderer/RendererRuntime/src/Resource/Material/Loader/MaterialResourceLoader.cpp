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
#include "RendererRuntime/Resource/Material/Loader/MaterialResourceLoader.h"
#include "RendererRuntime/Resource/Material/Loader/MaterialFileFormat.h"
#include "RendererRuntime/Resource/Material/MaterialTechnique.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId MaterialResourceLoader::TYPE_ID("material");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void MaterialResourceLoader::onDeserialization(IFile& file)
	{
		// Read in the file format header
		FileFormatHeader fileFormatHeader;
		file.read(&fileFormatHeader, sizeof(FileFormatHeader));
		assert(v1Material::FORMAT_TYPE == fileFormatHeader.formatType);
		assert(v1Material::FORMAT_VERSION == fileFormatHeader.formatVersion);

		// Tell the memory mapped file about the LZ4 compressed data
		mMemoryFile.setLz4CompressedDataByFile(file, fileFormatHeader.numberOfCompressedBytes, fileFormatHeader.numberOfDecompressedBytes);
	}

	void MaterialResourceLoader::onProcessing()
	{
		// Decompress LZ4 compressed data
		mMemoryFile.decompress();

		// Read in the material header
		v1Material::MaterialHeader materialHeader;
		mMemoryFile.read(&materialHeader, sizeof(v1Material::MaterialHeader));

		{ // Read techniques
			mNumberOfTechniques = materialHeader.numberOfTechniques;

			// Allocate memory for the temporary data
			if (mMaximumNumberOfMaterialTechniques < mNumberOfTechniques)
			{
				delete [] mMaterialTechniques;
				mMaximumNumberOfMaterialTechniques = mNumberOfTechniques;
				mMaterialTechniques = new v1Material::Technique[mMaximumNumberOfMaterialTechniques];
			}

			// Read already sorted techniques
			mMemoryFile.read(mMaterialTechniques, sizeof(v1Material::Technique) * mNumberOfTechniques);
		}

		// Read properties
		// TODO(co) Get rid of the evil const-cast
		MaterialProperties::SortedPropertyVector& sortedPropertyVector = const_cast<MaterialProperties::SortedPropertyVector&>(mMaterialResource->mMaterialProperties.getSortedPropertyVector());
		sortedPropertyVector.resize(materialHeader.numberOfProperties);
		mMemoryFile.read(sortedPropertyVector.data(), sizeof(MaterialProperty) * materialHeader.numberOfProperties);
	}

	bool MaterialResourceLoader::onDispatch()
	{
		// TODO(co) Material resource update
		mMaterialResource->releaseTextures();
		mMaterialResource->destroyAllMaterialTechniques();

		{ // Request all referenced material blueprint resources to be loaded now
			MaterialBlueprintResourceManager& materialBlueprintResourceManager = mRendererRuntime.getMaterialBlueprintResourceManager();
			const v1Material::Technique* v1MaterialTechnique = mMaterialTechniques;
			mMaterialBlueprintResourceIds.resize(mNumberOfTechniques);
			for (size_t i = 0; i < mNumberOfTechniques; ++i, ++v1MaterialTechnique)
			{
				materialBlueprintResourceManager.loadMaterialBlueprintResourceByAssetId(v1MaterialTechnique->materialBlueprintAssetId, mMaterialBlueprintResourceIds[i]);
				assert(isInitialized(mMaterialBlueprintResourceIds[i]));
			}
		}

		// Fully loaded?
		return isFullyLoaded();
	}

	bool MaterialResourceLoader::isFullyLoaded()
	{
		{ // Fully loaded?
			const MaterialBlueprintResourceManager& materialBlueprintResourceManager = mRendererRuntime.getMaterialBlueprintResourceManager();
			const v1Material::Technique* v1MaterialTechnique = mMaterialTechniques;
			for (size_t i = 0; i < mNumberOfTechniques; ++i, ++v1MaterialTechnique)
			{
				if (IResource::LoadingState::LOADED != materialBlueprintResourceManager.getResourceByResourceId(mMaterialBlueprintResourceIds[i]).getLoadingState())
				{
					// Not fully loaded
					return false;
				}
			}
		}

		{ // Create the material techniques (list is already sorted)
			MaterialResource::SortedMaterialTechniqueVector& sortedMaterialTechniqueVector = mMaterialResource->mSortedMaterialTechniqueVector;
			const v1Material::Technique* v1MaterialTechnique = mMaterialTechniques;
			for (size_t i = 0; i < mNumberOfTechniques; ++i, ++v1MaterialTechnique)
			{
				sortedMaterialTechniqueVector.push_back(new MaterialTechnique(v1MaterialTechnique->materialTechniqueId, *mMaterialResource, mMaterialBlueprintResourceIds[i]));
			}
		}

		// Fully loaded
		return true;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	MaterialResourceLoader::~MaterialResourceLoader()
	{
		// Free temporary data
		delete [] mMaterialTechniques;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
