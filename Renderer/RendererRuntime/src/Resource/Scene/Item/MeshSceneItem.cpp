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
#include "RendererRuntime/Resource/Scene/Item/MeshSceneItem.h"
#include "RendererRuntime/Resource/Scene/ISceneResource.h"
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/Resource/Scene/Loader/SceneFileFormat.h"
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
			// TODO(co) Get rid of the evil const-cast
			const_cast<MeshResource&>(getSceneResource().getRendererRuntime().getMeshResourceManager().getMeshResources().getElementById(meshResourceId)).connectResourceListener(*this);
		}
	}

	void MeshSceneItem::setMeshResourceIdByAssetId(AssetId meshAssetId)
	{
		disconnectFromAllResources();
		mMeshResourceId = getSceneResource().getRendererRuntime().getMeshResourceManager().loadMeshResourceByAssetId(meshAssetId, this);
	}

	void MeshSceneItem::deserialize(uint32_t numberOfBytes, const uint8_t* data)
	{
		assert(sizeof(v1Scene::MeshItem) == numberOfBytes);
		setMeshResourceIdByAssetId(reinterpret_cast<const v1Scene::MeshItem*>(data)->meshAssetId);
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::IResourceListener methods ]
	//[-------------------------------------------------------]
	void MeshSceneItem::onLoadingStateChange(const IResource& resource)
	{
		if (resource.getLoadingState() == IResource::LoadingState::LOADED)
		{
			// Set material resource ID of each sub-mesh
			const SubMeshes& subMeshes = static_cast<const MeshResource&>(resource).getSubMeshes();
			const size_t numberOfSubMeshes = subMeshes.size();
			mMaterialResourceIds.resize(numberOfSubMeshes);
			for (size_t subMeshIndex = 0; subMeshIndex < numberOfSubMeshes; ++subMeshIndex)
			{
				mMaterialResourceIds[subMeshIndex] = subMeshes[subMeshIndex].getMaterialResourceId();
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
