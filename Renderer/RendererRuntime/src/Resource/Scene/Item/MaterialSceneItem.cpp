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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/Scene/Item/MaterialSceneItem.h"
#include "RendererRuntime/Resource/Scene/Loader/SceneFileFormat.h"
#include "RendererRuntime/Resource/Scene/SceneResource.h"
#include "RendererRuntime/Resource/Scene/SceneNode.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <tuple>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	void MaterialSceneItem::deserialize(uint32_t numberOfBytes, const uint8_t* data)
	{
		// Sanity check
		assert(sizeof(v1Scene::MaterialItem) <= numberOfBytes);
		std::ignore = numberOfBytes;

		// Read data
		const v1Scene::MaterialItem* materialItem = reinterpret_cast<const v1Scene::MaterialItem*>(data);
		assert(sizeof(v1Scene::MaterialItem) + sizeof(MaterialProperty) * materialItem->numberOfMaterialProperties == numberOfBytes);
		mMaterialAssetId = materialItem->materialAssetId;
		mMaterialTechniqueId = materialItem->materialTechniqueId;
		mMaterialBlueprintAssetId = materialItem->materialBlueprintAssetId;

		{ // Read material properties
			// TODO(co) Get rid of the evil const-cast
			MaterialProperties::SortedPropertyVector& sortedPropertyVector = const_cast<MaterialProperties::SortedPropertyVector&>(mMaterialProperties.getSortedPropertyVector());
			sortedPropertyVector.resize(materialItem->numberOfMaterialProperties);
			memcpy(reinterpret_cast<char*>(sortedPropertyVector.data()), data + sizeof(v1Scene::MaterialItem), sizeof(MaterialProperty) * materialItem->numberOfMaterialProperties);
		}

		// Sanity checks
		assert(isInitialized(mMaterialAssetId) || isInitialized(mMaterialBlueprintAssetId));
		assert(!(isInitialized(mMaterialAssetId) && isInitialized(mMaterialBlueprintAssetId)));
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::IResourceListener methods ]
	//[-------------------------------------------------------]
	void MaterialSceneItem::onLoadingStateChange(const IResource& resource)
	{
		assert(resource.getAssetId() == mMaterialAssetId);
		if (resource.getLoadingState() == IResource::LoadingState::LOADED)
		{
			createMaterialResource(resource.getId());
		}
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	MaterialSceneItem::~MaterialSceneItem()
	{
		if (isInitialized(mMaterialResourceId))
		{
			// Destroy the material resource we created
			getSceneResource().getRendererRuntime().getMaterialResourceManager().destroyMaterialResource(mMaterialResourceId);
		}
	}

	void MaterialSceneItem::initialize()
	{
		// Sanity checks
		assert(isInitialized(mMaterialAssetId) || isInitialized(mMaterialBlueprintAssetId));
		assert(!(isInitialized(mMaterialAssetId) && isInitialized(mMaterialBlueprintAssetId)));

		// Get parent material resource ID and initiate creating the material resource
		MaterialResourceManager& materialResourceManager = getSceneResource().getRendererRuntime().getMaterialResourceManager();
		if (isInitialized(mMaterialAssetId))
		{
			// Get or load material resource
			MaterialResourceId materialResourceId = getUninitialized<MaterialResourceId>();
			materialResourceManager.loadMaterialResourceByAssetId(mMaterialAssetId, materialResourceId, this);
		}
		else
		{
			// Get or load material blueprint resource
			const AssetId materialBlueprintAssetId = mMaterialBlueprintAssetId;
			if (isInitialized(materialBlueprintAssetId))
			{
				MaterialResourceId parentMaterialResourceId = materialResourceManager.getMaterialResourceIdByAssetId(materialBlueprintAssetId);
				if (isUninitialized(parentMaterialResourceId))
				{
					parentMaterialResourceId = materialResourceManager.createMaterialResourceByAssetId(materialBlueprintAssetId, materialBlueprintAssetId, mMaterialTechniqueId);
				}
				createMaterialResource(parentMaterialResourceId);
			}
		}
	}

	void MaterialSceneItem::createMaterialResource(MaterialResourceId parentMaterialResourceId)
	{
		// Sanity checks
		assert(isUninitialized(mMaterialResourceId));
		assert(isInitialized(parentMaterialResourceId));

		// Each material user instance must have its own material resource since material property values might vary
		const IRendererRuntime& rendererRuntime = getSceneResource().getRendererRuntime();
		MaterialResourceManager& materialResourceManager = rendererRuntime.getMaterialResourceManager();
		mMaterialResourceId = materialResourceManager.createMaterialResourceByCloning(parentMaterialResourceId);

		{ // Set material properties
			const MaterialProperties::SortedPropertyVector& sortedPropertyVector = mMaterialProperties.getSortedPropertyVector();
			if (!sortedPropertyVector.empty())
			{
				MaterialResource& materialResource = materialResourceManager.getById(mMaterialResourceId);
				for (const MaterialProperty& materialProperty : sortedPropertyVector)
				{
					if (materialProperty.isOverwritten())
					{
						materialResource.setPropertyById(materialProperty.getMaterialPropertyId(), materialProperty);
					}
				}
			}
		}

		// Tell the world
		onMaterialResourceCreated();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
