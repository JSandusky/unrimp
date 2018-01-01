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
#include "RendererRuntime/Resource/Scene/Item/Mesh/MeshSceneItem.h"
#include "RendererRuntime/Resource/Scene/Loader/SceneFileFormat.h"
#include "RendererRuntime/Resource/Scene/Culling/SceneItemSet.h"
#include "RendererRuntime/Resource/Scene/SceneResource.h"
#include "RendererRuntime/Resource/Scene/SceneNode.h"
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/Resource/Mesh/MeshResource.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	#include <glm/gtx/component_wise.hpp>
PRAGMA_WARNING_POP

#include <tuple>
#include <algorithm>


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
		if (isInitialized(mMeshResourceId))
		{
			disconnectFromResourceById(mMeshResourceId);
		}
		mMeshResourceId = meshResourceId;
		if (isInitialized(meshResourceId))
		{
			getSceneResource().getRendererRuntime().getMeshResourceManager().getResourceByResourceId(meshResourceId).connectResourceListener(*this);
		}
	}

	void MeshSceneItem::setMeshResourceIdByAssetId(AssetId meshAssetId)
	{
		if (isInitialized(mMeshResourceId))
		{
			disconnectFromResourceById(mMeshResourceId);
		}
		getSceneResource().getRendererRuntime().getMeshResourceManager().loadMeshResourceByAssetId(meshAssetId, mMeshResourceId, this);
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

	void MeshSceneItem::onAttachedToSceneNode(SceneNode& sceneNode)
	{
		mRenderableManager.setTransform(&sceneNode.getGlobalTransform());

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
				const MeshResource* meshResource = rendererRuntime.getMeshResourceManager().tryGetById(mMeshResourceId);
				if (nullptr != meshResource)
				{
					// TODO(co) The following is just for culling kickoff and won't stay this way
					// Set scene item set bounding data
					if (nullptr != mSceneItemSet)
					{
						const SceneNode* parentSceneNode = getParentSceneNode();

						{ // Set minimum object space bounding box corner position
							const glm::vec3& minimumBoundingBoxPosition = meshResource->getMinimumBoundingBoxPosition();
							mSceneItemSet->minimumX[mSceneItemSetIndex] = minimumBoundingBoxPosition.x;
							mSceneItemSet->minimumY[mSceneItemSetIndex] = minimumBoundingBoxPosition.y;
							mSceneItemSet->minimumZ[mSceneItemSetIndex] = minimumBoundingBoxPosition.z;
						}

						{ // Set maximum object space bounding box corner position
							const glm::vec3& maximumBoundingBoxPosition = meshResource->getMaximumBoundingBoxPosition();
							mSceneItemSet->maximumX[mSceneItemSetIndex] = maximumBoundingBoxPosition.x;
							mSceneItemSet->maximumY[mSceneItemSetIndex] = maximumBoundingBoxPosition.y;
							mSceneItemSet->maximumZ[mSceneItemSetIndex] = maximumBoundingBoxPosition.z;
						}

						{ // Set world space center position of bounding sphere
							const glm::vec3& boundingSpherePosition = meshResource->getBoundingSpherePosition();
							if (nullptr != parentSceneNode)
							{
								const glm::vec3& position = parentSceneNode->getTransform().position;
								const glm::vec3& scale = parentSceneNode->getTransform().scale;
								mSceneItemSet->spherePositionX[mSceneItemSetIndex] = boundingSpherePosition.x * scale.x + position.x;
								mSceneItemSet->spherePositionY[mSceneItemSetIndex] = boundingSpherePosition.y * scale.y + position.y;
								mSceneItemSet->spherePositionZ[mSceneItemSetIndex] = boundingSpherePosition.z * scale.z + position.z;
							}
							else
							{
								mSceneItemSet->spherePositionX[mSceneItemSetIndex] = boundingSpherePosition.x;
								mSceneItemSet->spherePositionY[mSceneItemSetIndex] = boundingSpherePosition.y;
								mSceneItemSet->spherePositionZ[mSceneItemSetIndex] = boundingSpherePosition.z;
							}
						}

						{ // Set negative world space radius of bounding sphere
							float boundingSphereRadius = meshResource->getBoundingSphereRadius();
							if (nullptr != parentSceneNode)
							{
								boundingSphereRadius *= glm::compMax(parentSceneNode->getTransform().scale);
							}
							mSceneItemSet->negativeRadius[mSceneItemSetIndex] = -boundingSphereRadius;
						}
					}

					// Get vertex array instance
					const Renderer::IVertexArrayPtr vertexArrayPtr = meshResource->getVertexArrayPtr();

					// Set material resource ID of each sub-mesh
					const SkeletonResourceId skeletonResourceId = meshResource->getSkeletonResourceId();
					MaterialResourceManager& materialResourceManager = rendererRuntime.getMaterialResourceManager();
					const SubMeshes& subMeshes = static_cast<const MeshResource&>(resource).getSubMeshes();
					const size_t numberOfSubMeshes = subMeshes.size();
					renderables.reserve(numberOfSubMeshes);
					for (size_t i = 0; i < numberOfSubMeshes; ++i)
					{
						const SubMesh& subMesh = subMeshes[i];
						renderables.emplace_back(mRenderableManager, vertexArrayPtr, true, subMesh.getStartIndexLocation(), subMesh.getNumberOfIndices(), materialResourceManager, subMesh.getMaterialResourceId(), skeletonResourceId);
					}

					// Handle overwritten sub-meshes
					// -> In case the overwritten material resource is not yet fully loaded, the original material resource of the sub-mesh is temporarily used
					// -> In case there are more overwritten sub-meshes as there are sub-meshes, be error tolerant here (mesh assets might have been changed, but not updated scene assets in use)
					if (!mSubMeshMaterialAssetIds.empty())
					{
						const uint32_t numberOfMaterials = static_cast<uint32_t>(std::min(mSubMeshMaterialAssetIds.size(), mRenderableManager.getRenderables().size()));
						for (size_t i = 0; i < numberOfMaterials; ++i)
						{
							if (isInitialized(mSubMeshMaterialAssetIds[i]))
							{
								MaterialResourceId materialResourceId = getUninitialized<MaterialResourceId>();
								materialResourceManager.loadMaterialResourceByAssetId(mSubMeshMaterialAssetIds[i], materialResourceId, this);
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
				const uint32_t numberOfMaterials = static_cast<uint32_t>(std::min(mSubMeshMaterialAssetIds.size(), mRenderableManager.getRenderables().size()));
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
