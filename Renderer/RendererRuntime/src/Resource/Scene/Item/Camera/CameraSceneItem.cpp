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
#include "RendererRuntime/Resource/Scene/Item/Camera/CameraSceneItem.h"
#include "RendererRuntime/Resource/Scene/Loader/SceneFileFormat.h"
#include "RendererRuntime/Resource/Scene/SceneNode.h"
#include "RendererRuntime/Core/Math/Math.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4324)	// warning C4324: '<x>': structure was padded due to alignment specifier
	#include <glm/gtc/matrix_transform.hpp>
PRAGMA_WARNING_POP

#include <tuple>
#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const SceneItemTypeId CameraSceneItem::TYPE_ID("CameraSceneItem");
	const float CameraSceneItem::DEFAULT_FOV_Y  = glm::radians(45.0f);
	const float CameraSceneItem::DEFAULT_NEAR_Z = 0.1f;
	const float CameraSceneItem::DEFAULT_FAR_Z  = 5000.0f;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	const Transform& CameraSceneItem::getWorldSpaceToViewSpaceTransform() const
	{
		const SceneNode* parentSceneNode = getParentSceneNode();
		return (nullptr != parentSceneNode) ? parentSceneNode->getGlobalTransform() : Transform::IDENTITY;
	}

	const Transform& CameraSceneItem::getPreviousWorldSpaceToViewSpaceTransform() const
	{
		const SceneNode* parentSceneNode = getParentSceneNode();
		return (nullptr != parentSceneNode) ? parentSceneNode->getPreviousGlobalTransform() : Transform::IDENTITY;
	}

	const glm::mat4& CameraSceneItem::getWorldSpaceToViewSpaceMatrix() const
	{
		// Calculate the world space to view space matrix (Aka "view matrix")
		if (!mHasCustomWorldSpaceToViewSpaceMatrix)
		{
			const Transform& worldSpaceToViewSpaceTransform = getWorldSpaceToViewSpaceTransform();
			mWorldSpaceToViewSpaceMatrix = glm::lookAt(worldSpaceToViewSpaceTransform.position, worldSpaceToViewSpaceTransform.position + worldSpaceToViewSpaceTransform.rotation * Math::VEC3_FORWARD, Math::VEC3_UP);
		}

		// Done
		return mWorldSpaceToViewSpaceMatrix;
	}

	void CameraSceneItem::getPreviousWorldSpaceToViewSpaceMatrix(glm::mat4& previousWorldSpaceToViewSpaceMatrix) const
	{
		// Calculate the previous world space to view space matrix (Aka "view matrix")
		const Transform& worldSpaceToViewSpaceTransform = getPreviousWorldSpaceToViewSpaceTransform();
		previousWorldSpaceToViewSpaceMatrix = glm::lookAt(worldSpaceToViewSpaceTransform.position, worldSpaceToViewSpaceTransform.position + worldSpaceToViewSpaceTransform.rotation * Math::VEC3_FORWARD, Math::VEC3_UP);
	}

	const glm::mat4& CameraSceneItem::getViewSpaceToClipSpaceMatrix(float aspectRatio) const
	{
		// Calculate the view space to clip space matrix (aka "projection matrix")
		if (!mHasCustomViewSpaceToClipSpaceMatrix)
		{
			mViewSpaceToClipSpaceMatrix = glm::perspective(mFovY, aspectRatio, mNearZ, mFarZ);
		}

		// Done
		return mViewSpaceToClipSpaceMatrix;
	}


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	void CameraSceneItem::deserialize(uint32_t numberOfBytes, const uint8_t*)
	{
		assert(sizeof(v1Scene::CameraItem) == numberOfBytes);
		std::ignore = numberOfBytes;

		// No FOV Y, near z and far z deserialization by intent, those are usually application controlled values
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CameraSceneItem::CameraSceneItem(SceneResource& sceneResource) :
		ISceneItem(sceneResource),
		mFovY(DEFAULT_FOV_Y),
		mNearZ(DEFAULT_NEAR_Z),
		mFarZ(DEFAULT_FAR_Z),
		mWorldSpaceToViewSpaceMatrix(Math::MAT4_IDENTITY),
		mViewSpaceToClipSpaceMatrix(Math::MAT4_IDENTITY),
		mHasCustomWorldSpaceToViewSpaceMatrix(false),
		mHasCustomViewSpaceToClipSpaceMatrix(false)
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
