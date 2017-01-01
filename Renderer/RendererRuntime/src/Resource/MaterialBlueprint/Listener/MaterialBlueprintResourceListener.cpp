/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/MaterialBlueprintResourceListener.h"
#include "RendererRuntime/Resource/Material/MaterialTechnique.h"
#include "RendererRuntime/Resource/Scene/Item/CameraSceneItem.h"
#include "RendererRuntime/Resource/Scene/Node/ISceneNode.h"
#include "RendererRuntime/Core/Math/Transform.h"
#include "RendererRuntime/Core/Math/Math.h"
#include "RendererRuntime/Vr/IVrManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <Renderer/Public/Renderer.h>

#include <imgui/imgui.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4324)	// warning C4324: '<x>': structure was padded due to alignment specifier
	#include <glm/gtc/type_ptr.hpp>
	#include <glm/gtc/matrix_transform.hpp>
	#include <glm/gtx/quaternion.hpp>
PRAGMA_WARNING_POP


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
		#define DEFINE_CONSTANT(name) static const RendererRuntime::StringId name(#name);
			// Pass
			DEFINE_CONSTANT(WORLD_SPACE_TO_VIEW_SPACE_MATRIX)
			DEFINE_CONSTANT(WORLD_SPACE_TO_VIEW_SPACE_QUATERNION)
			DEFINE_CONSTANT(WORLD_SPACE_TO_CLIP_SPACE_MATRIX)
			DEFINE_CONSTANT(IMGUI_OBJECT_SPACE_TO_CLIP_SPACE_MATRIX)
			DEFINE_CONSTANT(VIEW_SPACE_SUN_LIGHT_DIRECTION)
			DEFINE_CONSTANT(INVERSE_VIEWPORT_SIZE)

			// Instance
			DEFINE_CONSTANT(INSTANCE_INDICES)
			DEFINE_CONSTANT(WORLD_POSITION_MATERIAL_INDEX)
		#undef DEFINE_CONSTANT


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Private virtual RendererRuntime::IMaterialBlueprintResourceListener methods ]
	//[-------------------------------------------------------]
	void MaterialBlueprintResourceListener::beginFillPass(IRendererRuntime& rendererRuntime, const Renderer::IRenderTarget& renderTarget, const CameraSceneItem* cameraSceneItem, PassBufferManager::PassData& passData)
	{
		// Remember the pass data memory address of the current scope
		mPassData = &passData;

		// Get the render target with and height
		renderTarget.getWidthAndHeight(mRenderTargetWidth, mRenderTargetHeight);

		// Get the aspect ratio
		const float aspectRatio = static_cast<float>(mRenderTargetWidth) / mRenderTargetHeight;

		// Get camera settings
		const float fovY  = (nullptr != cameraSceneItem) ? cameraSceneItem->getFovY()  : CameraSceneItem::DEFAULT_FOV_Y;
		const float nearZ = (nullptr != cameraSceneItem) ? cameraSceneItem->getNearZ() : CameraSceneItem::DEFAULT_NEAR_Z;
		const float farZ  = (nullptr != cameraSceneItem) ? cameraSceneItem->getFarZ()  : CameraSceneItem::DEFAULT_FAR_Z;

		// Calculate required matrices basing whether or not the VR-manager is currently running
		glm::mat4 viewTranslateMatrix;
		glm::mat4 viewSpaceToClipSpaceMatrix;
		const IVrManager& vrManager = rendererRuntime.getVrManager();
		const Transform& worldSpaceToViewSpaceTransform = (nullptr != cameraSceneItem && nullptr != cameraSceneItem->getParentSceneNode()) ? cameraSceneItem->getParentSceneNode()->getTransform() : Transform::IDENTITY;
		if (vrManager.isRunning() && VrEye::UNKNOWN != getCurrentRenderedVrEye())
		{
			const IVrManager::VrEye vrEye = static_cast<IVrManager::VrEye>(getCurrentRenderedVrEye());
			viewSpaceToClipSpaceMatrix = vrManager.getHmdViewSpaceToClipSpaceMatrix(vrEye, nearZ, farZ);
			viewTranslateMatrix = vrManager.getHmdEyeSpaceToHeadSpaceMatrix(vrEye) * vrManager.getHmdPoseMatrix();

			// Calculate the final matrices
			mPassData->worldSpaceToViewSpaceMatrix = glm::translate(glm::mat4(1.0f), worldSpaceToViewSpaceTransform.position) * glm::toMat4(worldSpaceToViewSpaceTransform.rotation);
		}
		else
		{
			viewSpaceToClipSpaceMatrix = glm::perspective(fovY, aspectRatio, nearZ, farZ);

			// Calculate the final matrices
			mPassData->worldSpaceToViewSpaceMatrix = glm::lookAt(worldSpaceToViewSpaceTransform.position, worldSpaceToViewSpaceTransform.position + worldSpaceToViewSpaceTransform.rotation * Math::FORWARD_VECTOR, Math::UP_VECTOR);
		}
		mPassData->worldSpaceToViewSpaceQuaternion = glm::quat(mPassData->worldSpaceToViewSpaceMatrix);
		mPassData->worldSpaceToClipSpaceMatrix = viewSpaceToClipSpaceMatrix * viewTranslateMatrix * mPassData->worldSpaceToViewSpaceMatrix;
	}

	bool MaterialBlueprintResourceListener::fillPassValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes)
	{
		bool valueFilled = true;

		// Resolve the reference value
		// TODO(co) Add more of those standard property values
		if (::detail::WORLD_SPACE_TO_VIEW_SPACE_MATRIX == referenceValue)
		{
			assert(sizeof(float) * 4 * 4 == numberOfBytes);
			memcpy(buffer, glm::value_ptr(mPassData->worldSpaceToViewSpaceMatrix), numberOfBytes);
		}
		else if (::detail::WORLD_SPACE_TO_VIEW_SPACE_QUATERNION == referenceValue)
		{
			assert(sizeof(float) * 4 == numberOfBytes);
			memcpy(buffer, glm::value_ptr(mPassData->worldSpaceToViewSpaceQuaternion), numberOfBytes);
		}
		else if (::detail::WORLD_SPACE_TO_CLIP_SPACE_MATRIX == referenceValue)
		{
			assert(sizeof(float) * 4 * 4 == numberOfBytes);
			memcpy(buffer, glm::value_ptr(mPassData->worldSpaceToClipSpaceMatrix), numberOfBytes);
		}
		else if (::detail::IMGUI_OBJECT_SPACE_TO_CLIP_SPACE_MATRIX == referenceValue)
		{
			assert(sizeof(float) * 4 * 4 == numberOfBytes);
			const ImGuiIO& imGuiIo = ImGui::GetIO();
			const float objectSpaceToClipSpaceMatrix[4][4] =
			{
				{  2.0f / imGuiIo.DisplaySize.x, 0.0f,                          0.0f, 0.0f },
				{  0.0f,                         2.0f / -imGuiIo.DisplaySize.y, 0.0f, 0.0f },
				{  0.0f,                         0.0f,                          0.5f, 0.0f },
				{ -1.0f,                         1.0f,                          0.5f, 1.0f }
			};
			memcpy(buffer, objectSpaceToClipSpaceMatrix, numberOfBytes);
		}
		else if (::detail::VIEW_SPACE_SUN_LIGHT_DIRECTION == referenceValue)
		{
			assert(sizeof(float) * 3 == numberOfBytes);
			glm::vec3 viewSpaceSunLightDirection(0.5f, 0.5f, 1.0f);	// TODO(co) This is just a test, needs to be filled from the outside
			viewSpaceSunLightDirection = mPassData->worldSpaceToViewSpaceQuaternion * glm::normalize(viewSpaceSunLightDirection);
			memcpy(buffer, glm::value_ptr(viewSpaceSunLightDirection), numberOfBytes);
		}
		else if (::detail::INVERSE_VIEWPORT_SIZE == referenceValue)
		{
			assert(sizeof(float) * 2 == numberOfBytes);
			float* floatBuffer = reinterpret_cast<float*>(buffer);

			// 0 = Inverse viewport width
			// 1 = Inverse viewport height
			floatBuffer[0] = 1.0f / static_cast<float>(mRenderTargetWidth);
			floatBuffer[1] = 1.0f / static_cast<float>(mRenderTargetHeight);
		}
		else
		{
			// Value not filled
			valueFilled = false;
		}

		// Done
		return valueFilled;
	}

	bool MaterialBlueprintResourceListener::fillInstanceValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes)
	{
		bool valueFilled = true;
		std::ignore = numberOfBytes;

		// Resolve the reference value
		if (::detail::INSTANCE_INDICES == referenceValue)
		{
			assert(sizeof(uint32_t) * 4 == numberOfBytes);
			uint32_t* integerBuffer = reinterpret_cast<uint32_t*>(buffer);

			// 0 = x = The instance texture buffer start index
			integerBuffer[0] = 0;

			// 1 = y = The assigned material slot inside the material uniform buffer
			integerBuffer[1] = mMaterialTechnique->getAssignedMaterialSlot();

			// 2 = z = The custom parameters start index inside the instance texture buffer
			integerBuffer[2] = 0;

			// 3 = w = Unused
			integerBuffer[3] = 0;
		}
		else if (::detail::WORLD_POSITION_MATERIAL_INDEX == referenceValue)
		{
			assert(sizeof(uint32_t) * 4 == numberOfBytes);
			uint32_t* integerBuffer = reinterpret_cast<uint32_t*>(buffer);

			// 0 = World space x position
			// 1 = World space y position
			// 2 = World space z position
			*reinterpret_cast<float*>(integerBuffer)	 = mObjectSpaceToWorldSpaceTransform->position.x;
			*reinterpret_cast<float*>(integerBuffer + 1) = mObjectSpaceToWorldSpaceTransform->position.y;
			*reinterpret_cast<float*>(integerBuffer + 2) = mObjectSpaceToWorldSpaceTransform->position.z;

			// 3 = w = The assigned material slot inside the material uniform buffer
			integerBuffer[3] = mMaterialTechnique->getAssignedMaterialSlot();
		}
		else
		{
			// Value not filled
			valueFilled = false;
		}

		// Done
		return valueFilled;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
