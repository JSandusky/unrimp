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
#include "RendererRuntime/Core/Math/Transform.h"
#include "RendererRuntime/Vr/IVrManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <Renderer/Public/Renderer.h>

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#pragma warning(disable: 4324)	// warning C4324: '<x>': structure was padded due to alignment specifier
	#include <glm/gtc/type_ptr.hpp>
	#include <glm/gtc/matrix_transform.hpp>
	#include <glm/gtx/quaternion.hpp>
#pragma warning(pop)


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
			DEFINE_CONSTANT(WORLD_SPACE_TO_VIEW_SPACE_MATRIX)
			DEFINE_CONSTANT(WORLD_SPACE_TO_CLIP_SPACE_MATRIX)
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
	void MaterialBlueprintResourceListener::beginFillPass(IRendererRuntime& rendererRuntime, const Transform& worldSpaceToViewSpaceTransform, PassBufferManager::PassData& passData)
	{
		// Remember the pass data memory address of the current scope
		mPassData = &passData;

		// Get the aspect ratio
		float aspectRatio = 4.0f / 3.0f;
		{
			// Get the render target with and height
			// TODO(co) Don't use the main swap chain in here, use e.g. the scene camera item
			uint32_t width  = 1;
			uint32_t height = 1;
			Renderer::IRenderTarget *renderTarget = rendererRuntime.getRenderer().getMainSwapChain();
			if (nullptr != renderTarget)
			{
				renderTarget->getWidthAndHeight(width, height);

				// Get the aspect ratio
				aspectRatio = static_cast<float>(width) / height;
			}
		}

		// TODO(co) Use dynamic values
		float fovY = 45.0f;
		float nearZ = 0.1f;
		float farZ = 100.0f;

		// Calculate required matrices basing whether or not the VR-manager is currently running
		glm::mat4 viewSpaceToClipSpaceMatrix;
		glm::mat4 viewTranslateMatrix;
		{
			const IVrManager& vrManager = rendererRuntime.getVrManager();
			if (vrManager.isRunning() && VrEye::UNKNOWN != getCurrentRenderedVrEye())
			{
				const IVrManager::VrEye vrEye = static_cast<IVrManager::VrEye>(getCurrentRenderedVrEye());
				viewSpaceToClipSpaceMatrix = vrManager.getHmdViewSpaceToClipSpaceMatrix(vrEye, nearZ, farZ);
				viewTranslateMatrix = vrManager.getHmdEyeSpaceToHeadSpaceMatrix(vrEye) * vrManager.getHmdPoseMatrix();
			}
			else
			{
				viewSpaceToClipSpaceMatrix = glm::perspective(fovY, aspectRatio, nearZ, farZ);
			}
		}

		// Calculate the final matrices
		mPassData->worldSpaceToViewSpaceMatrix = glm::translate(glm::mat4(1.0f), worldSpaceToViewSpaceTransform.position) * glm::toMat4(worldSpaceToViewSpaceTransform.rotation);
		mPassData->worldSpaceToClipSpaceMatrix = viewSpaceToClipSpaceMatrix * viewTranslateMatrix * mPassData->worldSpaceToViewSpaceMatrix;
	}

	bool MaterialBlueprintResourceListener::fillPassValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes)
	{
		bool valueFilled = true;

		// Resolve the reference value
		// TODO(co) Add more of those standard property values
		if (::detail::WORLD_SPACE_TO_VIEW_SPACE_MATRIX == referenceValue)
		{
			memcpy(buffer, glm::value_ptr(mPassData->worldSpaceToViewSpaceMatrix), numberOfBytes);
		}
		else if (::detail::WORLD_SPACE_TO_CLIP_SPACE_MATRIX == referenceValue)
		{
			memcpy(buffer, glm::value_ptr(mPassData->worldSpaceToClipSpaceMatrix), numberOfBytes);
		}

		// TODO(co) This is just a test, remove later on
		else if (StringId("VIEW_SPACE_SUN_LIGHT_DIRECTION") == referenceValue)
		{
			glm::vec3 viewSpaceLightDirection(0.5f, 0.5f, 1.0f);
			viewSpaceLightDirection = glm::normalize(viewSpaceLightDirection);
			memcpy(buffer, glm::value_ptr(viewSpaceLightDirection), numberOfBytes);
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
