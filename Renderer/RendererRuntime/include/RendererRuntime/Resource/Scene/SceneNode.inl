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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline const Transform& SceneNode::getTransform() const
	{
		return mTransform;
	}

	inline void SceneNode::setTransform(const Transform& transform)
	{
		mTransform = transform;
		updateGlobalTransformRecursive();
	}

	inline void SceneNode::setPosition(const glm::vec3& position)
	{
		mTransform.position = position;
		updateGlobalTransformRecursive();
	}

	inline void SceneNode::setRotation(const glm::quat& rotation)
	{
		mTransform.rotation = rotation;
		updateGlobalTransformRecursive();
	}

	inline void SceneNode::setPositionRotation(const glm::vec3& position, const glm::quat& rotation)
	{
		mTransform.position = position;
		mTransform.rotation = rotation;
		updateGlobalTransformRecursive();
	}

	inline void SceneNode::setScale(const glm::vec3& scale)
	{
		mTransform.scale = scale;
		updateGlobalTransformRecursive();
	}

	inline const Transform& SceneNode::getGlobalTransform() const
	{
		return mGlobalTransform;
	}

	inline const Transform& SceneNode::getPreviousGlobalTransform() const
	{
		return mPreviousGlobalTransform;
	}

	inline const SceneNode::AttachedSceneNodes& SceneNode::getAttachedSceneNodes() const
	{
		return mAttachedSceneNodes;
	}

	inline const SceneNode::AttachedSceneItems& SceneNode::getAttachedSceneItems() const
	{
		return mAttachedSceneItems;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline SceneNode::SceneNode(const Transform& transform) :
		mParentSceneNode(nullptr),
		mTransform(transform),
		mGlobalTransform(transform),
		mPreviousGlobalTransform(transform)
	{
		// Nothing here
	}

	inline SceneNode::~SceneNode()
	{
		detachAllSceneNodes();
		detachAllSceneItems();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
