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
#include "RendererRuntime/Resource/Scene/SceneResource.h"
#include "RendererRuntime/Resource/Scene/SceneNode.h"
#include "RendererRuntime/Resource/Scene/SceneItemMesh.h"
#include "RendererRuntime/Resource/Scene/SceneItemCamera.h"
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SceneResource::~SceneResource()
	{
		// Destroy all scene nodes and scene instances
		destroyAllSceneNodesAndItems();
	}

	SceneNode* SceneResource::createSceneNode(const Transform& transform)
	{
		SceneNode* sceneNode = new SceneNode(transform);
		mSceneNodes.push_back(sceneNode);
		return sceneNode;
	}

	void SceneResource::destroySceneNode(SceneNode& sceneNode)
	{
		SceneNodes::iterator iterator = std::find(mSceneNodes.begin(), mSceneNodes.end(), &sceneNode);
		if (iterator != mSceneNodes.end())
		{
			mSceneNodes.erase(iterator);
			delete &sceneNode;
		}
		else
		{
			// TODO(co) Error handling
		}
	}

	void SceneResource::destroyAllSceneNodes()
	{
		const size_t numberOfSceneNodes = mSceneNodes.size();
		for (size_t i = 0; i < numberOfSceneNodes; ++i)
		{
			delete mSceneNodes[i];
		}
		mSceneNodes.clear();
	}

	SceneItemCamera* SceneResource::createSceneItemCamera()
	{
		SceneItemCamera* sceneItemCamera = new SceneItemCamera(*this);
		mSceneItems.push_back(sceneItemCamera);
		return sceneItemCamera;
	}

	void SceneResource::destroySceneItemCamera(SceneItemCamera& sceneItemCamera)
	{
		destroySceneItem(sceneItemCamera);
	}

	SceneItemMesh* SceneResource::createSceneItemMesh(AssetId meshAssetId)
	{
		// Create mesh instance
		// TODO(co) Performance: Cache mesh resource manager and renderer instance inside the scene resource manager
		MeshResource* meshResource = mRendererRuntime.getMeshResourceManager().loadMeshResourceByAssetId(mRendererRuntime.getRenderer(), meshAssetId);
		return (nullptr != meshResource) ? new SceneItemMesh(*this, *meshResource) : nullptr;
	}

	void SceneResource::destroySceneItemMesh(SceneItemMesh& sceneItemMesh)
	{
		destroySceneItem(sceneItemMesh);
	}

	void SceneResource::destroyAllSceneItems()
	{
		const size_t numberOfSceneItems = mSceneItems.size();
		for (size_t i = 0; i < numberOfSceneItems; ++i)
		{
			delete mSceneItems[i];
		}
		mSceneItems.clear();
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	SceneResource::SceneResource(IRendererRuntime& rendererRuntime, ResourceId resourceId) :
		IResource(resourceId),
		mRendererRuntime(rendererRuntime)
	{
		// Nothing in here
	}

	void SceneResource::destroySceneItem(ISceneItem& sceneItem)
	{
		SceneItems::iterator iterator = std::find(mSceneItems.begin(), mSceneItems.end(), &sceneItem);
		if (iterator != mSceneItems.end())
		{
			mSceneItems.erase(iterator);
			delete &sceneItem;
		}
		else
		{
			// TODO(co) Error handling
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
