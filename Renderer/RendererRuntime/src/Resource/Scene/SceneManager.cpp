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
#include "RendererRuntime/Resource/Scene/SceneManager.h"
#include "RendererRuntime/Resource/Scene/SceneNode.h"
#include "RendererRuntime/Resource/Scene/SceneMesh.h"
#include "RendererRuntime/Resource/Scene/SceneCamera.h"
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
	SceneNode* SceneManager::createSceneNode(const Transform& transform)
	{
		return new SceneNode(transform);
	}

	void SceneManager::destroySceneNode(SceneNode& sceneNode)
	{
		delete &sceneNode;
	}

	SceneCamera* SceneManager::createSceneCamera()
	{
		return new SceneCamera();
	}

	void SceneManager::destroySceneCamera(SceneCamera& sceneCamera)
	{
		delete &sceneCamera;
	}

	SceneMesh* SceneManager::createSceneMesh(AssetId meshAssetId)
	{
		// Create mesh instance
		// TODO(co) Performance: Cache mesh resource manager and renderer instance inside the scene manager
		MeshResource* meshResource = mRendererRuntime.getMeshResourceManager().loadMeshResourceByAssetId(mRendererRuntime.getRenderer(), meshAssetId);
		return (nullptr != meshResource) ? new SceneMesh(*meshResource) : nullptr;
	}

	void SceneManager::destroySceneMesh(SceneMesh& sceneMesh)
	{
		delete &sceneMesh;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	SceneManager::SceneManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime)
	{
		// Nothing in here
	}

	SceneManager::~SceneManager()
	{
		// Nothing in here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
