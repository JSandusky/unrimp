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
#include "RendererRuntime/Resource/Scene/ISceneResource.h"
#include "RendererRuntime/Resource/Scene/SceneResourceManager.h"
#include "RendererRuntime/Resource/Scene/Node/SceneNode.h"
#include "RendererRuntime/Resource/Scene/Item/ISceneItem.h"
#include "RendererRuntime/Resource/Scene/Factory/ISceneFactory.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ISceneResource::~ISceneResource()
	{
		// Destroy all scene nodes and scene instances
		destroyAllSceneNodesAndItems();
		deinitializeElement();
	}

	ISceneNode* ISceneResource::createSceneNode(const Transform& transform)
	{
		assert(nullptr != mSceneFactory);
		ISceneNode* sceneNode = mSceneFactory->createSceneNode(SceneNode::TYPE_ID, transform);
		mSceneNodes.push_back(sceneNode);
		return sceneNode;
	}

	void ISceneResource::destroySceneNode(ISceneNode& sceneNode)
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

	void ISceneResource::destroyAllSceneNodes()
	{
		const size_t numberOfSceneNodes = mSceneNodes.size();
		for (size_t i = 0; i < numberOfSceneNodes; ++i)
		{
			delete mSceneNodes[i];
		}
		mSceneNodes.clear();
	}

	ISceneItem* ISceneResource::createSceneItem(SceneItemTypeId sceneItemTypeId, ISceneNode& sceneNode)
	{
		assert(nullptr != mSceneFactory);
		ISceneItem* sceneItem = mSceneFactory->createSceneItem(sceneItemTypeId, *this);
		if (nullptr != sceneItem)
		{
			sceneNode.attachSceneItem(*sceneItem);
			mSceneItems.push_back(sceneItem);
		}
		else
		{
			// TODO(co) Error handling
		}
		return sceneItem;
	}

	void ISceneResource::destroySceneItem(ISceneItem& sceneItem)
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

	void ISceneResource::destroyAllSceneItems()
	{
		const size_t numberOfSceneItems = mSceneItems.size();
		for (size_t i = 0; i < numberOfSceneItems; ++i)
		{
			delete mSceneItems[i];
		}
		mSceneItems.clear();
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	ISceneResource::ISceneResource(IRendererRuntime& rendererRuntime, ResourceId resourceId) :
		mRendererRuntime(rendererRuntime),
		mSceneFactory(&mRendererRuntime.getSceneResourceManager().getSceneFactory())
	{
		initializeElement(resourceId);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
