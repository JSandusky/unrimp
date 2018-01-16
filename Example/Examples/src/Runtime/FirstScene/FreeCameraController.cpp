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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "PrecompiledHeader.h"
#include "Runtime/FirstScene/FreeCameraController.h"
#include "Framework/IApplication.h"

#include <RendererRuntime/Core/Math/Math.h>
#include <RendererRuntime/Core/Math/EulerAngles.h>
#include <RendererRuntime/Resource/Scene/SceneNode.h>
#include <RendererRuntime/Resource/Scene/Item/Camera/CameraSceneItem.h>

#include <PLInput/Input.h>


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
		static const float MOUSE_WHEEL_FACTOR	= 0.02f;
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
FreeCameraController::FreeCameraController(PLInput::InputManager& inputManager, RendererRuntime::CameraSceneItem& cameraSceneItem) :
	IController(cameraSceneItem),
	mVirtualStandardController(new PLInput::VirtualStandardController(inputManager)),
	mOriginalFovY(cameraSceneItem.getFovY())
{
	// Nothing here
}

FreeCameraController::~FreeCameraController()
{
	delete mVirtualStandardController;
}


//[-------------------------------------------------------]
//[ Public virtual IController methods                    ]
//[-------------------------------------------------------]
void FreeCameraController::onUpdate(float pastSecondsSinceLastFrame)
{
	mMouseControlInProgress = false;

	// Sanity check: No negative time, no useless update calls
	assert(pastSecondsSinceLastFrame > 0.0f);

	RendererRuntime::SceneNode* sceneNode = mCameraSceneItem.getParentSceneNode();
	if (nullptr != sceneNode)
	{
		// Get the current local transform
		const RendererRuntime::Transform& transform = sceneNode->getTransform();
		glm::vec3 newPosition = transform.position;
		glm::quat newRotation = transform.rotation;

		{ // Movement
			// Get the movement speed
			float movementSpeed = pastSecondsSinceLastFrame * ::detail::MOVEMENT_SPEED;
			{
				// Ridiculous speed up
				if (mVirtualStandardController->Run.isPressed() && mVirtualStandardController->Sneak.isPressed())
				{
					movementSpeed *= ::detail::FAST_MOVEMENT_FACTOR * ::detail::FAST_MOVEMENT_FACTOR;
				}
				else
				{
					// Speed up
					if (mVirtualStandardController->Run.isPressed())
					{
						movementSpeed *= ::detail::FAST_MOVEMENT_FACTOR;
					}

					// Slow down
					if (mVirtualStandardController->Sneak.isPressed())
					{
						movementSpeed *= ::detail::SLOW_MOVEMENT_FACTOR;
					}
				}
			}

			// Get the movement vector
			glm::vec3 movementVector = RendererRuntime::Math::VEC3_ZERO;
			{
				{ // Move forward/backward
					const glm::vec3 forwardVector = transform.rotation * RendererRuntime::Math::VEC3_FORWARD;
					if (mVirtualStandardController->Forward.isPressed())
					{
						movementVector += forwardVector * movementSpeed;
					}
					if (mVirtualStandardController->Backward.isPressed())
					{
						movementVector -= forwardVector * movementSpeed;
					}
					if (0.0f != mVirtualStandardController->MouseWheel.getValue())
					{
						movementVector += forwardVector * mVirtualStandardController->MouseWheel.getValue() * ::detail::MOUSE_WHEEL_FACTOR * movementSpeed;
					}
					movementVector += forwardVector * (mVirtualStandardController->TransZ.isRelativeValue() ? mVirtualStandardController->TransZ.getValue() : mVirtualStandardController->TransZ.getValue() * movementSpeed);
				}

				{ // Strafe left/right
					const glm::vec3 rightVector = transform.rotation * RendererRuntime::Math::VEC3_RIGHT;
					if (mVirtualStandardController->StrafeLeft.isPressed())
					{
						movementVector -= rightVector * movementSpeed;
					}
					if (mVirtualStandardController->StrafeRight.isPressed())
					{
						movementVector += rightVector * movementSpeed;
					}
					movementVector -= rightVector * (mVirtualStandardController->TransX.isRelativeValue() ? mVirtualStandardController->TransX.getValue() : mVirtualStandardController->TransX.getValue() * movementSpeed);
				}

				{ // Strafe up/down
					const glm::vec3 upVector = transform.rotation * RendererRuntime::Math::VEC3_UP;
					if (mVirtualStandardController->Up.isPressed())
					{
						movementVector += upVector * movementSpeed;
					}
					if (mVirtualStandardController->Down.isPressed())
					{
						movementVector -= upVector * movementSpeed;
					}
					movementVector += upVector * (mVirtualStandardController->TransY.isRelativeValue() ? mVirtualStandardController->TransY.getValue() : mVirtualStandardController->TransY.getValue() * movementSpeed);
				}
			}

			// Update the camera scene node position
			newPosition += movementVector;
		}

		// Look around
		const bool absoluteRotation = (!mVirtualStandardController->RotX.isRelativeValue() && !mVirtualStandardController->RotY.isRelativeValue());
		if (mVirtualStandardController->Rotate.isPressed() || absoluteRotation)
		{
			mMouseControlInProgress = true;
			const float rotateX = mVirtualStandardController->RotX.getValue();
			const float rotateY = mVirtualStandardController->RotY.getValue();
			if (0.0f != rotateX || 0.0f != rotateY)
			{
				// Get the rotation speed
				// -> Slow down
				float rotationSpeed = ::detail::ROTATION_SPEED;
				if (mVirtualStandardController->RotateSlow.isPressed())
				{
					rotationSpeed *= ::detail::SLOW_ROTATION_FACTOR;
				}

				// Do we need to take the current time difference into account?
				float rotationSpeedX = rotationSpeed;
				float rotationSpeedY = rotationSpeed;
				if (!mVirtualStandardController->RotX.isRelativeValue())
				{
					rotationSpeedX *= pastSecondsSinceLastFrame;
				}
				if (!mVirtualStandardController->RotY.isRelativeValue())
				{
					rotationSpeedY *= pastSecondsSinceLastFrame;
				}

				// Calculate yaw and pitch from transformation
				// -> GLM 0.9.9.0 "glm::yaw()" and "glm::pitch" behave odd, so "RendererRuntime::EulerAngles::matrixToEuler()" is used instead
				// -> See discussion at https://github.com/g-truc/glm/issues/569
				float yaw = 0.0f, pitch = 0.0f, roll = 0.0f;
				{
					const glm::vec3 eulerAngles = RendererRuntime::EulerAngles::matrixToEuler(glm::mat3_cast(transform.rotation));
					yaw = glm::degrees(eulerAngles.x);
					pitch = glm::degrees(eulerAngles.y);
					roll = glm::degrees(eulerAngles.z);
				}

				// Apply rotation change
				if (0.0f != rotateX)
				{
					// X rotation axis: Update yaw (also called 'heading', change is turning to the left or right) - in degrees
					yaw += rotateX * rotationSpeedX;

					// Limit the yaw (too huge values may cause problems, so, bring them into a well known interval)
					yaw = RendererRuntime::Math::wrapToInterval(yaw, 0.0f, 360.0f);
				}
				if (0.0f != rotateY)
				{
					// Y rotation axis: Update pitch (also called 'bank', change is moving the nose down and the tail up or vice-versa) - in degrees
					pitch += rotateY * rotationSpeedY;

					// Limit the pitch (no full 90� to avoid dead angles)
					pitch = glm::clamp(pitch, -89.9f, +89.9f);
				}

				// Update the camera scene node rotation
				newRotation = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f));
			}
		}

		// Zoom
		if (mVirtualStandardController->Zoom.isPressed())
		{
			mMouseControlInProgress = true;

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
}
