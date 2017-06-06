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
#include "RendererRuntime/DebugGui/DebugGuiHelper.h"
#include "RendererRuntime/Core/Math/Transform.h"
#include "RendererRuntime/Core/Math/EulerAngles.h"
#include "RendererRuntime/Resource/Scene/SceneNode.h"
#include "RendererRuntime/Resource/Scene/SceneResource.h"
#include "RendererRuntime/Resource/Scene/Item/Camera/CameraSceneItem.h"
#include "RendererRuntime/Resource/Scene/Item/Mesh/SkeletonMeshSceneItem.h"
#include "RendererRuntime/Resource/Skeleton/SkeletonResourceManager.h"
#include "RendererRuntime/Resource/Skeleton/SkeletonResource.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <imguizmo/ImGuizmo.h>

#include <imgui/imgui.h>

#include <string>
#ifdef ANDROID
	#include <cstdio>	// For implementing own "std::to_string()"
#endif


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		std::string own_to_string(uint32_t value)
		{
			// Own "std::to_string()" implementation similar to the one used in GCC STL
			// -> We need it on Android because GNU STL doesn't implement it, which is part of the Android NDK
			// -> We need to support GNU STL because Qt-runtime uses Qt Android which currently only supports GNU STL as C++ runtime
			#ifdef ANDROID
				// We convert only an "uint32_t"-value which has a maximum value of 4294967295 -> 11 characters so with 16 we are on the safe side
				const size_t bufferSize = 16;
				char buffer[bufferSize] = {0};
				const int length = snprintf(buffer, bufferSize, "%u", value);
				return std::string(buffer, buffer + length);
			#else
				return std::to_string(value);
			#endif
		}

		bool objectSpaceToScreenSpacePosition(const glm::vec3& objectSpacePosition, const glm::mat4& objectSpaceToClipSpaceMatrix, ImVec2& screenSpacePosition)
		{
			glm::vec4 position = objectSpaceToClipSpaceMatrix * glm::vec4(objectSpacePosition, 1.0f);
			if (position.z < 0.0f)
			{
				// Behind camera
				return false;
			}
			position *= 0.5f / position.w;
			position += glm::vec4(0.5f, 0.5f, 0.0f, 0.0f);
			position.y = 1.0f - position.y;
			const ImGuiIO& imGuiIO = ImGui::GetIO();
			position.x *= imGuiIO.DisplaySize.x;
			position.y *= imGuiIO.DisplaySize.y;
			screenSpacePosition = ImVec2(position.x, position.y);

			// In front of camera
			return true;
		}


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
	//[ Private static data                                   ]
	//[-------------------------------------------------------]
	uint32_t DebugGuiHelper::mDrawTextCounter = 0;


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	void DebugGuiHelper::drawText(const char* text, float x, float y, bool drawBackground)
	{
		if (!drawBackground)
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 0.0f));
		}
		// TODO(co) Get rid of "std::to_string()" later on in case it's a problem (memory allocations inside)
		// TODO(sw) The GNUSTL on Android (currently needed to be used in conjunction with Qt) doesn't have "std::to_string"
		ImGui::Begin(("RendererRuntime::DebugGuiManager::drawText_" + ::detail::own_to_string(mDrawTextCounter)).c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing);
			ImGui::Text("%s", text);	// Use "%s" as format instead of the text itself to avoid compiler warning "-Wformat-security"
			ImGui::SetWindowPos(ImVec2(x, y));
		ImGui::End();
		if (!drawBackground)
		{
			ImGui::PopStyleColor();
		}
		++mDrawTextCounter;
	}

	void DebugGuiHelper::drawGizmo(const CameraSceneItem& cameraSceneItem, GizmoSettings& gizmoSettings, Transform& transform)
	{
		// Setup ImGuizmo
		if (ImGui::RadioButton("Translate", gizmoSettings.currentGizmoOperation == GizmoOperation::TRANSLATE))
		{
			gizmoSettings.currentGizmoOperation = GizmoOperation::TRANSLATE;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", gizmoSettings.currentGizmoOperation == GizmoOperation::ROTATE))
		{
			gizmoSettings.currentGizmoOperation = GizmoOperation::ROTATE;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", gizmoSettings.currentGizmoOperation == GizmoOperation::SCALE))
		{
			gizmoSettings.currentGizmoOperation = GizmoOperation::SCALE;
		}
		{ // Show and edit rotation quaternion using Euler angles in degree
			glm::vec3 eulerAngles = glm::degrees(EulerAngles::matrixToEuler(glm::mat3_cast(transform.rotation)));
			ImGui::InputFloat3("Tr", glm::value_ptr(transform.position), 3);
			ImGui::InputFloat3("Rt", glm::value_ptr(eulerAngles), 3);
			ImGui::InputFloat3("Sc", glm::value_ptr(transform.scale), 3);
			transform.rotation = EulerAngles::eulerToQuaternion(glm::radians(eulerAngles));
		}
		if (gizmoSettings.currentGizmoOperation != GizmoOperation::SCALE)
		{
			if (ImGui::RadioButton("Local", gizmoSettings.currentGizmoMode == GizmoMode::LOCAL))
			{
				gizmoSettings.currentGizmoMode = GizmoMode::LOCAL;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("World", gizmoSettings.currentGizmoMode == GizmoMode::WORLD))
			{
				gizmoSettings.currentGizmoMode = GizmoMode::WORLD;
			}
		}
		ImGui::Checkbox("", &gizmoSettings.useSnap);
		ImGui::SameLine();
		switch (gizmoSettings.currentGizmoOperation)
		{
			case GizmoOperation::TRANSLATE:
				ImGui::InputFloat3("Snap", &gizmoSettings.snap[0]);
				break;

			case GizmoOperation::ROTATE:
				ImGui::InputFloat("Angle Snap", &gizmoSettings.snap[0]);
				break;

			case GizmoOperation::SCALE:
				ImGui::InputFloat("Scale Snap", &gizmoSettings.snap[0]);
				break;
		}
		{ // Let ImGuizmo do its thing
			glm::mat4 matrix;
			transform.getAsMatrix(matrix);
			ImGuizmo::OPERATION operation = static_cast<ImGuizmo::OPERATION>(gizmoSettings.currentGizmoOperation);
			ImGuizmo::MODE mode = (operation == ImGuizmo::SCALE) ? ImGuizmo::LOCAL : static_cast<ImGuizmo::MODE>(gizmoSettings.currentGizmoMode);
			ImGuizmo::Manipulate(glm::value_ptr(cameraSceneItem.getWorldSpaceToViewSpaceMatrix()), glm::value_ptr(cameraSceneItem.getViewSpaceToClipSpaceMatrix()), operation, mode, glm::value_ptr(matrix), nullptr, gizmoSettings.useSnap ? &gizmoSettings.snap[0] : nullptr);
			transform = Transform(matrix);
		}
	}

	void DebugGuiHelper::drawSkeleton(const CameraSceneItem& cameraSceneItem, const SkeletonMeshSceneItem& skeletonMeshSceneItem)
	{
		// Get skeleton resource instance
		const SkeletonResource* skeletonResource = skeletonMeshSceneItem.getSceneResource().getRendererRuntime().getSkeletonResourceManager().tryGetById(skeletonMeshSceneItem.getSkeletonResourceId());
		if (nullptr != skeletonResource)
		{
			// Get transform data
			glm::mat4 objectSpaceToWorldSpace;
			skeletonMeshSceneItem.getParentSceneNodeSafe().getGlobalTransform().getAsMatrix(objectSpaceToWorldSpace);
			const glm::mat4 objectSpaceToClipSpaceMatrix = cameraSceneItem.getViewSpaceToClipSpaceMatrix() * cameraSceneItem.getWorldSpaceToViewSpaceMatrix() * objectSpaceToWorldSpace;

			// Get skeleton data
			const uint8_t numberOfBones = skeletonResource->getNumberOfBones();
			const uint8_t* boneParentIndices = skeletonResource->getBoneParentIndices();
			const glm::mat4* globalBoneMatrices = skeletonResource->getGlobalBoneMatrices();

			// Draw skeleton hierarchy as lines
			ImGui::Begin("skeleton", nullptr, ImGui::GetIO().DisplaySize, 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
			static const ImColor whiteImColor(255, 255, 255);
			ImDrawList* imDrawList = ImGui::GetWindowDrawList();
			ImVec2 parentBonePosition;
			ImVec2 bonePosition;
			for (uint8_t boneIndex = 1; boneIndex < numberOfBones; ++boneIndex)
			{
				if (::detail::objectSpaceToScreenSpacePosition(globalBoneMatrices[boneParentIndices[boneIndex]][3], objectSpaceToClipSpaceMatrix, parentBonePosition) &&
					::detail::objectSpaceToScreenSpacePosition(globalBoneMatrices[boneIndex][3], objectSpaceToClipSpaceMatrix, bonePosition))
				{
					imDrawList->AddLine(parentBonePosition, bonePosition, whiteImColor, 6.0f);
				}
			}
			ImGui::End();
		}
	}


	//[-------------------------------------------------------]
	//[ Private static methods                                ]
	//[-------------------------------------------------------]
	void DebugGuiHelper::beginFrame()
	{
		// Reset the draw text counter
		mDrawTextCounter = 0;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
