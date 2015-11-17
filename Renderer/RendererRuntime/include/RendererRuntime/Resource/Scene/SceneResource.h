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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Export.h"
#include "RendererRuntime/Resource/IResource.h"
#include "RendererRuntime/Core/Manager.h"
#include "RendererRuntime/Asset/Asset.h"

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class SceneNode;
	class Transform;
	class ISceneItem;
	class SceneItemMesh;
	class SceneItemCamera;
	class IRendererRuntime;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class SceneResource : public IResource
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;
		friend class SceneResourceManager;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::vector<SceneNode*>  SceneNodes;
		typedef std::vector<ISceneItem*> SceneItems;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		virtual ~SceneResource();
		inline void destroyAllSceneNodesAndItems();

		//[-------------------------------------------------------]
		//[ Node                                                  ]
		//[-------------------------------------------------------]
		RENDERERRUNTIME_API_EXPORT SceneNode* createSceneNode(const Transform& transform);
		RENDERERRUNTIME_API_EXPORT void destroySceneNode(SceneNode& sceneNode);
		RENDERERRUNTIME_API_EXPORT void destroyAllSceneNodes();
		inline const SceneNodes& getSceneNodes() const;

		//[-------------------------------------------------------]
		//[ Item                                                  ]
		//[-------------------------------------------------------]
		RENDERERRUNTIME_API_EXPORT SceneItemCamera* createSceneItemCamera();
		RENDERERRUNTIME_API_EXPORT void destroySceneItemCamera(SceneItemCamera& sceneItemCamera);
		RENDERERRUNTIME_API_EXPORT SceneItemMesh* createSceneItemMesh(AssetId meshAssetId);
		RENDERERRUNTIME_API_EXPORT void destroySceneItemMesh(SceneItemMesh& sceneItemMesh);
		RENDERERRUNTIME_API_EXPORT void destroyAllSceneItems();
		inline const SceneItems& getSceneItems() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		SceneResource(IRendererRuntime& rendererRuntime, ResourceId resourceId);
		SceneResource(const SceneResource&) = delete;
		SceneResource& operator=(const SceneResource&) = delete;
		void destroySceneItem(ISceneItem& sceneItem);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime& mRendererRuntime;	///< Renderer runtime instance, do not destroy the instance
		SceneNodes		  mSceneNodes;
		SceneItems		  mSceneItems;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Scene/SceneResource.inl"
