/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
#include "RendererRuntime/Resource/Scene/Item/MeshSceneItem.h"
#include "RendererRuntime/Resource/Scene/ISceneResource.h"
#include "RendererRuntime/Resource/Scene/Node/ISceneNode.h"
#include "RendererRuntime/Resource/Scene/Loader/SceneFileFormat.h"
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const SceneItemTypeId MeshSceneItem::TYPE_ID("MeshSceneItem");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void MeshSceneItem::setMeshResourceId(MeshResourceId meshResourceId)
	{
		disconnectFromAllResources();
		mMeshResourceId = meshResourceId;
		if (isInitialized(meshResourceId))
		{
			getSceneResource().getRendererRuntime().getMeshResourceManager().getMeshResources().getElementById(meshResourceId).connectResourceListener(*this);
		}
	}

	void MeshSceneItem::setMeshResourceIdByAssetId(AssetId meshAssetId)
	{
		disconnectFromAllResources();
		mMeshResourceId = getSceneResource().getRendererRuntime().getMeshResourceManager().loadMeshResourceByAssetId(meshAssetId, this);
	}

	void MeshSceneItem::setMaterialResourceIdOfSubMesh(uint32_t subMeshIndex, MaterialResourceId materialResourceId)
	{
		assert(subMeshIndex < mRenderableManager.getRenderables().size());
		mRenderableManager.getRenderables()[subMeshIndex].setMaterialResourceId(getSceneResource().getRendererRuntime().getMaterialResourceManager(), materialResourceId);
	}

	void MeshSceneItem::setMaterialResourceIdOfAllSubMeshes(MaterialResourceId materialResourceId)
	{
		const MaterialResourceManager& materialResourceManager = getSceneResource().getRendererRuntime().getMaterialResourceManager();
		for (Renderable& renderable : mRenderableManager.getRenderables())
		{
			renderable.setMaterialResourceId(materialResourceManager, materialResourceId);
		}
	}


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	void MeshSceneItem::deserialize(uint32_t numberOfBytes, const uint8_t* data)
	{
		// Sanity check
		assert(sizeof(v1Scene::MeshItem) <= numberOfBytes);
		const v1Scene::MeshItem* meshItem = reinterpret_cast<const v1Scene::MeshItem*>(data);
		assert((sizeof(v1Scene::MeshItem) + sizeof(AssetId) * meshItem->numberOfSubMeshMaterialAssetIds) == numberOfBytes);
		std::ignore = numberOfBytes;

		// Read data
		setMeshResourceIdByAssetId(meshItem->meshAssetId);
		if (meshItem->numberOfSubMeshMaterialAssetIds > 0)
		{
			mSubMeshMaterialAssetIds.resize(meshItem->numberOfSubMeshMaterialAssetIds);
			memcpy(mSubMeshMaterialAssetIds.data(), data + sizeof(v1Scene::MeshItem), sizeof(AssetId) * meshItem->numberOfSubMeshMaterialAssetIds);
		}
		else
		{
			mSubMeshMaterialAssetIds.clear();
		}
	}

	void MeshSceneItem::onAttachedToSceneNode(ISceneNode& sceneNode)
	{
		mRenderableManager.setTransform(&sceneNode.getTransform());

		// Call the base implementation
		ISceneItem::onAttachedToSceneNode(sceneNode);
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::IResourceListener methods ]
	//[-------------------------------------------------------]
	void MeshSceneItem::onLoadingStateChange(const IResource& resource)
	{
		if (resource.getLoadingState() == IResource::LoadingState::LOADED)
		{
			// If mesh resource loading has been finished, setup the renderable manager
			if (resource.getId() == mMeshResourceId)
			{
				RenderableManager::Renderables& renderables = mRenderableManager.getRenderables();
				renderables.clear();

				// Get mesh resource instance
				const IRendererRuntime& rendererRuntime = getSceneResource().getRendererRuntime();
				const MeshResource* meshResource = rendererRuntime.getMeshResourceManager().getMeshResources().tryGetElementById(mMeshResourceId);
				if (nullptr != meshResource)
				{
					// Get vertex array instance
					const Renderer::IVertexArrayPtr vertexArrayPtr = meshResource->getVertexArrayPtr();

					// Set material resource ID of each sub-mesh
					MaterialResourceManager& materialResourceManager = rendererRuntime.getMaterialResourceManager();
					const SubMeshes& subMeshes = static_cast<const MeshResource&>(resource).getSubMeshes();
					const size_t numberOfSubMeshes = subMeshes.size();
					renderables.reserve(numberOfSubMeshes);
					for (size_t i = 0; i < numberOfSubMeshes; ++i)
					{
						const SubMesh& subMesh = subMeshes[i];
						renderables.emplace_back(mRenderableManager, vertexArrayPtr, subMesh.getPrimitiveTopology(), true, subMesh.getStartIndexLocation(), subMesh.getNumberOfIndices(), materialResourceManager, subMesh.getMaterialResourceId());
					}

					// Handle overwritten sub-meshes
					// -> In case the overwritten material resource is not yet fully loaded, the original material resource of the sub-mesh is temporarily used
					// -> In case there are more overwritten sub-meshes as there are sub-meshes, be error tolerant here (mesh assets might have been changed, but not updated scene assets in use)
					if (!mSubMeshMaterialAssetIds.empty())
					{
						const uint32_t numberOfMaterials = std::min(mSubMeshMaterialAssetIds.size(), mRenderableManager.getRenderables().size());
						for (size_t i = 0; i < numberOfMaterials; ++i)
						{
							if (isInitialized(mSubMeshMaterialAssetIds[i]))
							{
								materialResourceManager.loadMaterialResourceByAssetId(mSubMeshMaterialAssetIds[i], this);
							}
						}
					}
				}

				// Finalize the renderable manager by updating cached renderables data
				mRenderableManager.updateCachedRenderablesData();
			}
			else
			{
				// Overwritten sub-mesh material loaded now?
				// -> In case there are more overwritten sub-meshes as there are sub-meshes, be error tolerant here (mesh assets might have been changed, but not updated scene assets in use)
				const uint32_t numberOfMaterials = std::min(mSubMeshMaterialAssetIds.size(), mRenderableManager.getRenderables().size());
				bool updateCachedRenderablesDataRequired = false;
				for (uint32_t i = 0; i < numberOfMaterials; ++i)
				{
					if (resource.getAssetId() == mSubMeshMaterialAssetIds[i])
					{
						mRenderableManager.getRenderables()[i].setMaterialResourceId(getSceneResource().getRendererRuntime().getMaterialResourceManager(), resource.getId());

						// Don't break, multiple sub-meshes might use one and the same material resource
						updateCachedRenderablesDataRequired = true;
					}
				}

				// Finalize the renderable manager by updating cached renderables data
				if (updateCachedRenderablesDataRequired)
				{
					mRenderableManager.updateCachedRenderablesData();
				}
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
