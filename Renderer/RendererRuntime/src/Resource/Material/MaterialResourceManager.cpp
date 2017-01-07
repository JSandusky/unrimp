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
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/Material/Loader/MaterialResourceLoader.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/Detail/ResourceStreamer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const MaterialTechniqueId MaterialResourceManager::DEFAULT_MATERIAL_TECHNIQUE_ID("Default");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	// TODO(co) Work-in-progress
	MaterialResource* MaterialResourceManager::getMaterialResourceByAssetId(AssetId assetId) const
	{
		const uint32_t numberOfElements = mMaterialResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			MaterialResource& materialResource = mMaterialResources.getElementByIndex(i);
			if (materialResource.getAssetId() == assetId)
			{
				return &materialResource;
			}
		}

		// There's no material resource using the given asset ID
		return nullptr;
	}

	// TODO(co) Work-in-progress
	MaterialResourceId MaterialResourceManager::loadMaterialResourceByAssetId(AssetId assetId, IResourceListener* resourceListener, bool reload)
	{
		MaterialResourceId materialResourceId = getUninitialized<MaterialResourceId>();

		// Get or create the instance
		MaterialResource* materialResource = nullptr;
		{
			const uint32_t numberOfElements = mMaterialResources.getNumberOfElements();
			for (uint32_t i = 0; i < numberOfElements; ++i)
			{
				MaterialResource& currentMaterialResource = mMaterialResources.getElementByIndex(i);
				if (currentMaterialResource.getAssetId() == assetId)
				{
					materialResource = &currentMaterialResource;

					// Get us out of the loop
					i = numberOfElements;
				}
			}
		}

		// Create the resource instance
		const Asset* asset = mRendererRuntime.getAssetManager().getAssetByAssetId(assetId);
		bool load = (reload && nullptr != asset);
		if (nullptr == materialResource && nullptr != asset)
		{
			materialResource = &mMaterialResources.addElement();
			materialResource->setResourceManager(this);
			materialResource->setAssetId(assetId);
			load = true;
		}
		if (nullptr != materialResource)
		{
			if (nullptr != resourceListener)
			{
				materialResource->connectResourceListener(*resourceListener);
			}
			materialResourceId = materialResource->getId();
		}

		// Load the resource, if required
		if (load)
		{
			// Prepare the resource loader
			MaterialResourceLoader* materialResourceLoader = static_cast<MaterialResourceLoader*>(acquireResourceLoaderInstance(MaterialResourceLoader::TYPE_ID));
			materialResourceLoader->initialize(*asset, *materialResource);

			// Commit resource streamer asset load request
			ResourceStreamer::LoadRequest resourceStreamerLoadRequest;
			resourceStreamerLoadRequest.resource = materialResource;
			resourceStreamerLoadRequest.resourceLoader = materialResourceLoader;
			mRendererRuntime.getResourceStreamer().commitLoadRequest(resourceStreamerLoadRequest);
		}

		// Done
		return materialResourceId;
	}

	MaterialResourceId MaterialResourceManager::createMaterialResourceByAssetId(AssetId assetId, AssetId materialBlueprintAssetId, MaterialTechniqueId materialTechniqueId)
	{
		// Material resource is not allowed to exist, yet
		assert(isUninitialized(getMaterialResourceIdByAssetId(assetId)));

		// Create the material resource instance
		MaterialResource& materialResource = mMaterialResources.addElement();
		materialResource.setResourceManager(this);
		materialResource.setAssetId(assetId);

		{ // Setup material resource instance
			// Copy over the material properties of the material blueprint resource
			MaterialBlueprintResourceManager& materialBlueprintResourceManager = mRendererRuntime.getMaterialBlueprintResourceManager();
			const MaterialBlueprintResourceId materialBlueprintResourceId = materialBlueprintResourceManager.loadMaterialBlueprintResourceByAssetId(materialBlueprintAssetId);
			MaterialBlueprintResource* materialBlueprintResource = materialBlueprintResourceManager.getMaterialBlueprintResources().tryGetElementById(materialBlueprintResourceId);
			if (nullptr != materialBlueprintResource)
			{
				materialResource.mMaterialProperties = materialBlueprintResource->mMaterialProperties;

				// Create default material technique
				materialResource.mSortedMaterialTechniqueVector.push_back(new MaterialTechnique(materialTechniqueId, materialResource, materialBlueprintResourceId));
			}
			else
			{
				// Error!
				assert(false);
			}
		}

		// Done
		setResourceLoadingState(materialResource, IResource::LoadingState::LOADED);
		return materialResource.getId();
	}

	MaterialResourceId MaterialResourceManager::createMaterialResourceByCloning(MaterialResourceId parentMaterialResourceId, AssetId assetId)
	{
		assert(mMaterialResources.isElementIdValid(parentMaterialResourceId));
		assert(mMaterialResources.getElementById(parentMaterialResourceId).getLoadingState() == IResource::LoadingState::LOADED);

		// Create the material resource instance
		MaterialResource& materialResource = mMaterialResources.addElement();
		materialResource.setResourceManager(this);
		materialResource.setAssetId(assetId);
		materialResource.setParentMaterialResourceId(parentMaterialResourceId);

		// Done
		setResourceLoadingState(materialResource, IResource::LoadingState::LOADED);
		return materialResource.getId();
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	void MaterialResourceManager::reloadResourceByAssetId(AssetId assetId)
	{
		// TODO(co) Experimental implementation (take care of resource cleanup etc.)
		const uint32_t numberOfElements = mMaterialResources.getNumberOfElements();
		for (uint32_t i = 0; i < numberOfElements; ++i)
		{
			if (mMaterialResources.getElementByIndex(i).getAssetId() == assetId)
			{
				loadMaterialResourceByAssetId(assetId, nullptr, true);
				break;
			}
		}
	}

	void MaterialResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	IResourceLoader* MaterialResourceManager::acquireResourceLoaderInstance(ResourceLoaderTypeId resourceLoaderTypeId)
	{
		// Can we recycle an already existing resource loader instance?
		IResourceLoader* resourceLoader = IResourceManager::acquireResourceLoaderInstance(resourceLoaderTypeId);

		// We need to create a new resource loader instance
		if (nullptr == resourceLoader)
		{
			// We only support our own material format
			assert(resourceLoaderTypeId == MaterialResourceLoader::TYPE_ID);
			resourceLoader = new MaterialResourceLoader(*this, mRendererRuntime);
			mUsedResourceLoaderInstances.push_back(resourceLoader);
		}

		// Done
		return resourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
