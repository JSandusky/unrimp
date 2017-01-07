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
#include "RendererRuntime/Resource/Material/Loader/MaterialResourceLoader.h"
#include "RendererRuntime/Resource/Material/Loader/MaterialFileFormat.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
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
	const ResourceLoaderTypeId MaterialResourceLoader::TYPE_ID("material");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void MaterialResourceLoader::onDeserialization()
	{
		// TODO(co) Error handling
		try
		{
			std::ifstream inputFileStream(mAsset.assetFilename, std::ios::binary);

			if (!inputFileStream)
			{
				// TODO(sw) the getId is needed because clang3.9/gcc 4.9 cannot determine to use the uint32_t conversion operator on it when passed to a printf method: error: cannot pass non-trivial object of type 'AssetId' (aka 'RendererRuntime::StringId') to variadic function; expected type from format string was 'int' [-Wnon-pod-varargs]
				RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load material asset %u. Could not open file: %s", mAsset.assetId.getId(), mAsset.assetFilename);
				return;
			}

			// Read in the material header
			v1Material::Header materialHeader;
			inputFileStream.read(reinterpret_cast<char*>(&materialHeader), sizeof(v1Material::Header));

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
				inputFileStream.read(reinterpret_cast<char*>(mMaterialTechniques), sizeof(v1Material::Technique) * mNumberOfTechniques);
			}

			// Read properties
			// TODO(co) Get rid of the evil const-cast
			MaterialProperties::SortedPropertyVector& sortedPropertyVector = const_cast<MaterialProperties::SortedPropertyVector&>(mMaterialResource->mMaterialProperties.getSortedPropertyVector());
			sortedPropertyVector.resize(materialHeader.numberOfProperties);
			inputFileStream.read(reinterpret_cast<char*>(sortedPropertyVector.data()), sizeof(MaterialProperty) * materialHeader.numberOfProperties);
		}
		catch (const std::exception& e)
		{
			// TODO(sw) the getId is needed because clang3.9/gcc 4.9 cannot determine to use the uint32_t conversion operator on it when passed to a printf method: error: cannot pass non-trivial object of type 'AssetId' (aka 'RendererRuntime::StringId') to variadic function; expected type from format string was 'int' [-Wnon-pod-varargs]
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load material asset %u: %s", mAsset.assetId.getId(), e.what());
		}
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
				mMaterialBlueprintResourceIds[i] = materialBlueprintResourceManager.loadMaterialBlueprintResourceByAssetId(v1MaterialTechnique->materialBlueprintAssetId);
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
