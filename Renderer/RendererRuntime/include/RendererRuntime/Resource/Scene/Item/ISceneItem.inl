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
#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline SceneResource& ISceneItem::getSceneResource() const
	{
		return mSceneResource;
	}

	inline bool ISceneItem::hasParentSceneNode() const
	{
		return (nullptr != mParentSceneNode);
	}

	inline SceneNode* ISceneItem::getParentSceneNode()
	{
		return mParentSceneNode;
	}

	inline const SceneNode* ISceneItem::getParentSceneNode() const
	{
		return mParentSceneNode;
	}

	inline SceneNode& ISceneItem::getParentSceneNodeSafe()
	{
		assert(nullptr != mParentSceneNode);
		return *mParentSceneNode;
	}

	inline const SceneNode& ISceneItem::getParentSceneNodeSafe() const
	{
		assert(nullptr != mParentSceneNode);
		return *mParentSceneNode;
	}


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	inline void ISceneItem::onAttachedToSceneNode(SceneNode& sceneNode)
	{
		assert(nullptr == mParentSceneNode);
		mParentSceneNode = &sceneNode;
	}

	inline void ISceneItem::onDetachedFromSceneNode(SceneNode&)
	{
		assert(nullptr != mParentSceneNode);
		mParentSceneNode = nullptr;
	}

	inline void ISceneItem::setVisible(bool)
	{
		// Nothing here
	}

	inline const RenderableManager* ISceneItem::getRenderableManager() const
	{
		return nullptr;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
