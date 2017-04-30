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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/Scene/SceneNode.h"
#include "RendererRuntime/Resource/Scene/Item/ISceneItem.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void SceneNode::attachSceneNode(SceneNode& sceneNode)
	{
		// TODO(co) Need to guarantee that one scene node is only attached to one scene node at the same time
		mAttachedSceneNodes.push_back(&sceneNode);
		sceneNode.mParentSceneNode = this;
		sceneNode.updateGlobalTransformRecursive();
	}

	void SceneNode::detachAllSceneNodes()
	{
		for (SceneNode* sceneNode : mAttachedSceneNodes)
		{
			sceneNode->mParentSceneNode = nullptr;
			sceneNode->updateGlobalTransformRecursive();
		}
		mAttachedSceneNodes.clear();
	}

	void SceneNode::setVisible(bool visible)
	{
		setSceneItemsVisible(visible);
		for (SceneNode* sceneNode : mAttachedSceneNodes)
		{
			sceneNode->setVisible(visible);
		}
	}

	void SceneNode::attachSceneItem(ISceneItem& sceneItem)
	{
		// TODO(co) Need to guarantee that one scene item is only attached to one scene node at the same time
		mAttachedSceneItems.push_back(&sceneItem);
		sceneItem.onAttachedToSceneNode(*this);
	}

	void SceneNode::detachAllSceneItems()
	{
		for (ISceneItem* sceneItem : mAttachedSceneItems)
		{
			sceneItem->onDetachedFromSceneNode(*this);
		}
		mAttachedSceneItems.clear();
	}

	void SceneNode::setSceneItemsVisible(bool visible)
	{
		for (ISceneItem* sceneItem : mAttachedSceneItems)
		{
			sceneItem->setVisible(visible);
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void SceneNode::updateGlobalTransformRecursive()
	{
		// Update this node
		if (nullptr != mParentSceneNode)
		{
			mGlobalTransform = mParentSceneNode->mGlobalTransform;
			mGlobalTransform += mTransform;
		}
		else
		{
			mGlobalTransform = mTransform;
		}

		// Update attached scene nodes
		for (SceneNode* sceneNode : mAttachedSceneNodes)
		{
			sceneNode->mGlobalTransform = mGlobalTransform;
			sceneNode->mGlobalTransform += sceneNode->mTransform;
			sceneNode->updateGlobalTransformRecursive();
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
