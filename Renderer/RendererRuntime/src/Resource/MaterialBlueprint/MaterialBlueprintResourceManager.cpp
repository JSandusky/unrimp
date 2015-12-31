/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Loader/MaterialBlueprintResourceLoader.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/ResourceStreamer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <assert.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	// TODO(co) Work-in-progress
	MaterialBlueprintResource* MaterialBlueprintResourceManager::loadMaterialBlueprintResourceByAssetId(AssetId assetId, bool reload)
	{
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		if (nullptr != asset)
		{
			// Get or create the instance
			MaterialBlueprintResource* materialBlueprintResource = nullptr;
			const size_t numberOfResources = mResources.size();
			for (size_t i = 0; i < numberOfResources; ++i)
			{
				MaterialBlueprintResource* currentMaterialBlueprintResource = mResources[i];
				if (currentMaterialBlueprintResource->getResourceId() == assetId)
				{
					materialBlueprintResource = currentMaterialBlueprintResource;

					// Get us out of the loop
					i = mResources.size();
				}
			}

			// Create the resource instance
			bool load = reload;
			if (nullptr == materialBlueprintResource)
			{
				materialBlueprintResource = new MaterialBlueprintResource(*this, assetId);
				mResources.push_back(materialBlueprintResource);
				load = true;
			}

			// Load the resource, if required
			if (load)
			{
				// Prepare the resource loader
				MaterialBlueprintResourceLoader* materialBlueprintResourceLoader = static_cast<MaterialBlueprintResourceLoader*>(acquireResourceLoaderInstance(MaterialBlueprintResourceLoader::TYPE_ID));
				materialBlueprintResourceLoader->initialize(*asset, *materialBlueprintResource);

				// Commit resource streamer asset load request
				ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
				resourceStreamerLoadRequest.resource = materialBlueprintResource;
				resourceStreamerLoadRequest.resourceLoader = materialBlueprintResourceLoader;
				mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);
			}

			// TODO(co) No raw pointers in here
			return materialBlueprintResource;
		}

		// Error!
		return nullptr;
	}

	const MaterialProperty* MaterialBlueprintResourceManager::getGlobalMaterialPropertyById(MaterialPropertyId materialPropertyId) const
	{
		SortedGlobalMaterialPropertyVector::const_iterator iterator = std::lower_bound(mSortedGlobalMaterialPropertyVector.cbegin(), mSortedGlobalMaterialPropertyVector.cend(), materialPropertyId, detail::OrderByMaterialPropertyId());
		return (iterator != mSortedGlobalMaterialPropertyVector.end() && iterator._Ptr->getMaterialPropertyId() == materialPropertyId) ? iterator._Ptr : nullptr;
	}

	void MaterialBlueprintResourceManager::setGlobalMaterialPropertyById(MaterialPropertyId materialPropertyId, const MaterialPropertyValue& materialPropertyValue)
	{
		const MaterialProperty materialProperty(materialPropertyId, MaterialProperty::Usage::DYNAMIC, materialPropertyValue);
		SortedGlobalMaterialPropertyVector::iterator iterator = std::lower_bound(mSortedGlobalMaterialPropertyVector.begin(), mSortedGlobalMaterialPropertyVector.end(), materialPropertyId, detail::OrderByMaterialPropertyId());
		if (iterator == mSortedGlobalMaterialPropertyVector.end() || iterator->getMaterialPropertyId() != materialPropertyId)
		{
			// Add new global material property
			mSortedGlobalMaterialPropertyVector.insert(iterator, materialProperty);
		}
		else
		{
			// Just update the global material property value
			*iterator = materialProperty;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void MaterialBlueprintResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		for (size_t i = 0; i < mResources.size(); ++i)
		{
			MaterialBlueprintResource* materialBlueprintResource = mResources[i];
			if (materialBlueprintResource->getResourceId() == assetId)
			{
				loadMaterialBlueprintResourceByAssetId(assetId, true);

				// TODO(co) Cleanup: Update all influenced material resources, probably also other material stuff has to be updated
				materialBlueprintResource->mTextures.clear();
				for (auto materialResource : mRendererRuntime.getMaterialResourceManager().mResources)
				{
					if (materialResource->getMaterialBlueprintResource() == materialBlueprintResource)
					{
						materialResource->releasePipelineState();
					}
				}

				break;
			}
		}
	}

	void MaterialBlueprintResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	MaterialBlueprintResourceManager::MaterialBlueprintResourceManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime)
	{
		// Nothing in here
	}

	MaterialBlueprintResourceManager::~MaterialBlueprintResourceManager()
	{
		// Nothing in here
	}

	IResourceLoader* MaterialBlueprintResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		IResourceLoader* resourceLoader = IResourceManager::acquireResourceLoaderInstance(resourceLoaderTypeId);

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			// We only support our own material blueprint format
			assert(resourceLoaderTypeId == MaterialBlueprintResourceLoader::TYPE_ID);
			resourceLoader = new MaterialBlueprintResourceLoader(*this, mRendererRuntime);
			mUsedResourceLoaderInstances.push_back(resourceLoader);
		}

		// Done
		return resourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
