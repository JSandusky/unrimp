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
#include "RendererRuntime/Resource/Scene/Factory/SceneFactory.h"
#include "RendererRuntime/Resource/Scene/SceneResource.h"
#include "RendererRuntime/Resource/Scene/Node/SceneNode.h"
#include "RendererRuntime/Resource/Scene/Item/MeshSceneItem.h"
#include "RendererRuntime/Resource/Scene/Item/LightSceneItem.h"
#include "RendererRuntime/Resource/Scene/Item/CameraSceneItem.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::ISceneFactory methods ]
	//[-------------------------------------------------------]
	ISceneResource* SceneFactory::createSceneResource(SceneResourceTypeId sceneResourceTypeId, IRendererRuntime& rendererRuntime, ResourceId resourceId) const
	{
		ISceneResource* sceneResource = nullptr;

		// Evaluate the scene node type
		if (sceneResourceTypeId == SceneResource::TYPE_ID)
		{
			sceneResource = new SceneResource(rendererRuntime, resourceId);
		}

		// Done
		return sceneResource;
	}

	ISceneNode* SceneFactory::createSceneNode(SceneNodeTypeId sceneNodeTypeId, const Transform& transform) const
	{
		ISceneNode* sceneNode = nullptr;

		// Evaluate the scene node type
		if (sceneNodeTypeId == SceneNode::TYPE_ID)
		{
			sceneNode = new SceneNode(transform);
		}

		// Done
		return sceneNode;
	}

	ISceneItem* SceneFactory::createSceneItem(const SceneItemTypeId& sceneItemTypeId, ISceneResource& sceneResource) const
	{
		ISceneItem* sceneItem = nullptr;

		// Evaluate the scene item type, sorted by usual frequency
		if (sceneItemTypeId == MeshSceneItem::TYPE_ID)
		{
			sceneItem = new MeshSceneItem(sceneResource);
		}
		else if (sceneItemTypeId == LightSceneItem::TYPE_ID)
		{
			sceneItem = new LightSceneItem(sceneResource);
		}
		else if (sceneItemTypeId == CameraSceneItem::TYPE_ID)
		{
			sceneItem = new CameraSceneItem(sceneResource);
		}

		// Done
		return sceneItem;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
