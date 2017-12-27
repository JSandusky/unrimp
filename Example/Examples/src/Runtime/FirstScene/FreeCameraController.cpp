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
#include "PrecompiledHeader.h"
#include "Runtime/FirstScene/FreeCameraController.h"
#include "Framework/IApplication.h"

#include <RendererRuntime/Core/Math/Math.h>
#include <RendererRuntime/Core/Math/EulerAngles.h>
#include <RendererRuntime/Resource/Scene/SceneNode.h>
#include <RendererRuntime/Resource/Scene/Item/Camera/CameraSceneItem.h>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		static const float MOVEMENT_SPEED		= 3.0f;
		static const float FAST_MOVEMENT_FACTOR	= 10.0f;
		static const float SLOW_MOVEMENT_FACTOR	= 0.1f;
		static const float MOUSE_WHEEL_FACTOR	= 2.0f;
		static const float ROTATION_SPEED		= 0.2f;
		static const float SLOW_ROTATION_FACTOR	= 0.2f;
		static const float ZOOM_SPEED			= 4.0f;
		static const float ZOOM_FOV_Y			= glm::radians(10.0f);


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FreeCameraController::FreeCameraController(RendererRuntime::CameraSceneItem& cameraSceneItem) :
	IController(cameraSceneItem),
	mOriginalFovY(cameraSceneItem.getFovY())
{
	// Nothing here
}

FreeCameraController::~FreeCameraController()
{
	// Nothing here
}


//[-------------------------------------------------------]
//[ Public virtual IController methods                    ]
//[-------------------------------------------------------]
void FreeCameraController::onUpdate(float pastSecondsSinceLastFrame)
{
	// Sanity check: No negative time, no useless update calls
	assert(pastSecondsSinceLastFrame > 0.0f);

	RendererRuntime::SceneNode* sceneNode = mCameraSceneItem.getParentSceneNode();
	// TODO(co) Currently disabled early escape tests for velocity buffer (used e.g. for motion blur) kickoff. Need a general strategy how to update scene graph related data (also considering multi-threading, maybe communicate exclusively over a command buffer here as well?).
	// if (nullptr != sceneNode && (!mPressedKeys.empty() || !mPressedMouseButtons.empty()))
	if (nullptr != sceneNode)
	{
		// Get the current local transform
		const RendererRuntime::Transform& transform = sceneNode->getTransform();
		glm::vec3 newPosition = transform.position;
		glm::quat newRotation = transform.rotation;

		// Keyboard
		if (!mPressedKeys.empty() || 0.0f != mMouseWheelDelta)
		{
			// Get the movement speed
			float movementSpeed = pastSecondsSinceLastFrame * ::detail::MOVEMENT_SPEED;
			{
				// Ridiculous speed up = "left shift"-key and "left strg"-key both pressed
				if (isKeyPressed(IApplication::LEFT_SHIFT_KEY) && isKeyPressed(IApplication::LEFT_STRG_KEY))
				{
					movementSpeed *= ::detail::FAST_MOVEMENT_FACTOR * ::detail::FAST_MOVEMENT_FACTOR;
				}
				else
				{
					// Speed up
					if (isKeyPressed(IApplication::LEFT_SHIFT_KEY))
					{
						movementSpeed *= ::detail::FAST_MOVEMENT_FACTOR;
					}

					// Slow down
					if (isKeyPressed(IApplication::LEFT_STRG_KEY))
					{
						movementSpeed *= ::detail::SLOW_MOVEMENT_FACTOR;
					}
				}
			}

			// Get the movement vector
			glm::vec3 movementVector = RendererRuntime::Math::VEC3_ZERO;
			{
				// Move forward
				if (isKeyPressed(IApplication::W_KEY) || isKeyPressed(IApplication::ARROW_UP_KEY))
				{
					movementVector += transform.rotation * RendererRuntime::Math::VEC3_FORWARD;
				}

				// Strafe left
				if (isKeyPressed(IApplication::A_KEY) || isKeyPressed(IApplication::ARROW_LEFT_KEY))
				{
					movementVector -= transform.rotation * RendererRuntime::Math::VEC3_RIGHT;
				}

				// Move backward
				if (isKeyPressed(IApplication::S_KEY) || isKeyPressed(IApplication::ARROW_DOWN_KEY))
				{
					movementVector -= transform.rotation * RendererRuntime::Math::VEC3_FORWARD;
				}

				// Strafe right
				if (isKeyPressed(IApplication::D_KEY) || isKeyPressed(IApplication::ARROW_RIGHT_KEY))
				{
					movementVector += transform.rotation * RendererRuntime::Math::VEC3_RIGHT;
				}

				// Strafe up
				if (isKeyPressed(IApplication::PAGE_UP_KEY))
				{
					movementVector += transform.rotation * RendererRuntime::Math::VEC3_UP;
				}

				// Strafe down
				if (isKeyPressed(IApplication::PAGE_DOWN_KEY))
				{
					movementVector -= transform.rotation * RendererRuntime::Math::VEC3_UP;
				}

				// Mouse wheel: Move forward/backward
				if (0.0f != mMouseWheelDelta)
				{
					movementVector += transform.rotation * RendererRuntime::Math::VEC3_FORWARD * mMouseWheelDelta * ::detail::MOUSE_WHEEL_FACTOR;
					mMouseWheelDelta = 0.0f;
				}
			}

			// Update the camera scene node position
			newPosition += movementVector * movementSpeed;
		}

		// Mouse: Camera look around = right mouse button pressed down + mouse move
		if (isMouseButtonPressed(1) && (0 != mMouseMoveX || 0 != mMouseMoveY))
		{
			// Get the rotation speed
			// -> Slow down
			float rotationSpeed = ::detail::ROTATION_SPEED;
			if (isKeyPressed(IApplication::Q_KEY))
			{
				rotationSpeed *= ::detail::SLOW_ROTATION_FACTOR;
			}

			// Calculate yaw and pitch from transformation
			// -> GLM 0.9.9.0 "glm::yaw()" and "glm::pitch" behave odd, so "RendererRuntime::EulerAngles::matrixToEuler()" is used instead
			// -> See discussion at https://github.com/g-truc/glm/issues/569
			float yaw = 0.0f, pitch = 0.0f;
			{
				const glm::vec3 eulerAngles = RendererRuntime::EulerAngles::matrixToEuler(glm::mat3_cast(transform.rotation));
				yaw = glm::degrees(eulerAngles.x);
				pitch = glm::degrees(eulerAngles.y);
			}

			// Apply rotation change
			if (0 != mMouseMoveX)
			{
				// X rotation axis: Update yaw (also called 'heading', change is turning to the left or right) - in degrees
				yaw += mMouseMoveX * rotationSpeed;

				// Limit the yaw (too huge values may cause problems, so, bring them into a well known interval)
				yaw = RendererRuntime::Math::wrapToInterval(yaw, 0.0f, 360.0f);
			}
			if (0 != mMouseMoveY)
			{
				// Y rotation axis: Update pitch (also called 'bank', change is moving the nose down and the tail up or vice-versa) - in degrees
				pitch += mMouseMoveY * rotationSpeed;

				// Limit the pitch (no full 90° to avoid dead angles)
				pitch = glm::clamp(pitch, -89.9f, +89.9f);
			}

			// Update the camera scene node rotation
			newRotation = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f));
		}

		// Mouse: Zoom = middle mouse button pressed
		if (isMouseButtonPressed(2))
		{
			// Zoom in
			float fovY = mCameraSceneItem.getFovY();
			if (fovY > ::detail::ZOOM_FOV_Y)
			{
				fovY -= pastSecondsSinceLastFrame * ::detail::ZOOM_SPEED;
				if (fovY < ::detail::ZOOM_FOV_Y)
				{
					fovY = ::detail::ZOOM_FOV_Y;
				}
				mCameraSceneItem.setFovY(fovY);
			}
		}
		else
		{
			// Zoom out to original FOV-y
			float fovY = mCameraSceneItem.getFovY();
			if (fovY < mOriginalFovY)
			{
				fovY += pastSecondsSinceLastFrame * ::detail::ZOOM_SPEED;
				if (fovY > mOriginalFovY)
				{
					fovY = mOriginalFovY;
				}
				mCameraSceneItem.setFovY(fovY);
			}
		}

		// Tell the camera scene node about the new transform
		sceneNode->setPositionRotation(newPosition, newRotation);
	}

	// Call the base implementation
	IController::onUpdate(pastSecondsSinceLastFrame);
}
