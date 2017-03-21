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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/StringId.h"
#include "RendererRuntime/Core/Math/Transform.h"
#include "RendererRuntime/Resource/Scene/Item/LightSceneItem.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId SceneItemTypeId;	///< Scene item type identifier, internally just a POD "uint32_t"
	typedef StringId AssetId;			///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>"


	// -> Scene file format content:
	//    - Scene header
	namespace v1Scene
	{


		//[-------------------------------------------------------]
		//[ Definitions                                           ]
		//[-------------------------------------------------------]
		static const uint32_t FORMAT_TYPE	 = StringId("Scene");
		static const uint32_t FORMAT_VERSION = 1;

		#pragma pack(push)
		#pragma pack(1)
			struct Header
			{
				uint32_t formatType;
				uint32_t formatVersion;
			};

			struct Nodes
			{
				uint32_t numberOfNodes;
			};

			struct Node
			{
				Transform transform;
				uint32_t  numberOfItems;
			};

			struct ItemHeader
			{
				SceneItemTypeId typeId;
				uint32_t		numberOfBytes;
			};

			struct CameraItem
			{
			};

			struct LightItem
			{
				LightSceneItem::LightType lightType		   = LightSceneItem::LightType::POINT;
				float					  color[3]		   = { 1.0f, 1.0f, 1.0f };
				float					  radius		   = 1.0f;
				float					  innerAngle       = 40.0f;
				float					  outerAngle	   = 50.0f;
				float					  nearClipDistance = 0.0f;
			};

			struct MeshItem
			{
				AssetId  meshAssetId;
				uint32_t numberOfSubMeshMaterialAssetIds = 0;
			};

			struct SkeletonMeshItem	// : public MeshItem -> Not derived by intent to be able to reuse the mesh item serialization 1:1
			{
				AssetId skeletonAnimationAssetId;
			};
		#pragma pack(pop)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
	} // v1Scene
} // RendererRuntime
