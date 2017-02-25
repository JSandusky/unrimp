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
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/MaterialBlueprintResourceListener.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/LightBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialTechnique.h"
#include "RendererRuntime/Resource/Scene/Item/CameraSceneItem.h"
#include "RendererRuntime/Resource/Scene/Item/LightSceneItem.h"
#include "RendererRuntime/Resource/Scene/Node/ISceneNode.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ShadowMap/CompositorInstancePassShadowMap.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorContextData.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
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

#include <random>


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
			DEFINE_CONSTANT(VIEW_SPACE_TO_WORLD_SPACE_QUATERNION)
			DEFINE_CONSTANT(WORLD_SPACE_TO_CLIP_SPACE_MATRIX)
			DEFINE_CONSTANT(VIEW_SPACE_TO_CLIP_SPACE_MATRIX)
			DEFINE_CONSTANT(VIEW_SPACE_TO_TEXTURE_SPACE_MATRIX)
			DEFINE_CONSTANT(CLIP_SPACE_TO_VIEW_SPACE_MATRIX)
			DEFINE_CONSTANT(CLIP_SPACE_TO_WORLD_SPACE_MATRIX)
			DEFINE_CONSTANT(CAMERA_WORLD_SPACE_POSITION)
			DEFINE_CONSTANT(PROJECTION_PARAMETERS)
			DEFINE_CONSTANT(IMGUI_OBJECT_SPACE_TO_CLIP_SPACE_MATRIX)
			DEFINE_CONSTANT(VIEW_SPACE_SUN_LIGHT_DIRECTION)
			DEFINE_CONSTANT(VIEWPORT_SIZE)
			DEFINE_CONSTANT(INVERSE_VIEWPORT_SIZE)
			DEFINE_CONSTANT(NUMBER_OF_LIGHTS)
			DEFINE_CONSTANT(SHADOW_MATRIX)
			DEFINE_CONSTANT(SHADOW_CASCADE_SPLITS)
			DEFINE_CONSTANT(SHADOW_CASCADE_OFFSETS)
			DEFINE_CONSTANT(SHADOW_CASCADE_SCALES)
			DEFINE_CONSTANT(LENS_STAR_MATRIX)

			// Instance
			DEFINE_CONSTANT(INSTANCE_INDICES)
			DEFINE_CONSTANT(WORLD_POSITION_MATERIAL_INDEX)
		#undef DEFINE_CONSTANT


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Create 3D identity color correction lookup table (LUT)
		*
		*  @note
		*    - Basing on "GPU Gems 2" - "Chapter 24. Using Lookup Tables to Accelerate Color Transformations" by Jeremy Selan, Sony Pictures Imageworks - http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter24.html
		*    - A way for artists to create color correction lookup tables is described at https://docs.unrealengine.com/latest/INT/Engine/Rendering/PostProcessEffects/ColorGrading/
		*    - Color correction lookup table size is 16
		*    - Resulting texture asset ID is "Unrimp/Texture/DynamicByCode/IdentityColorCorrectionLookupTable3D"
		*/
		RendererRuntime::TextureResourceId createIdentityColorCorrectionLookupTable3D(const RendererRuntime::IRendererRuntime& rendererRuntime)
		{
			static const uint8_t SIZE = 16;
			static const uint8_t NUMBER_OF_COMPONENTS = 4;
			static const uint32_t NUMBER_OF_BYTES = SIZE * SIZE * SIZE * NUMBER_OF_COMPONENTS;
			uint8_t data[NUMBER_OF_BYTES];

			{ // Create the identity color correction lookup table 3D data
				uint8_t* currentData = data;
				for (uint8_t z = 0; z < SIZE; ++z)
				{
					for (uint8_t y = 0; y < SIZE; ++y)
					{
						for (uint8_t x = 0; x < SIZE; ++x)
						{
							currentData[0] = static_cast<uint8_t>((static_cast<float>(x) / static_cast<float>(SIZE)) * 255.0f);
							currentData[1] = static_cast<uint8_t>((static_cast<float>(y) / static_cast<float>(SIZE)) * 255.0f);
							currentData[2] = static_cast<uint8_t>((static_cast<float>(z) / static_cast<float>(SIZE)) * 255.0f);
							// currentData[3] = 0;	// Unused
							currentData += NUMBER_OF_COMPONENTS;
						}
					}
				}
			}

			// Create the renderer texture resource
			Renderer::ITexturePtr texturePtr(rendererRuntime.getTextureManager().createTexture3D(SIZE, SIZE, SIZE, Renderer::TextureFormat::R8G8B8A8, data));
			RENDERER_SET_RESOURCE_DEBUG_NAME(texturePtr, "3D identity color correction lookup table (LUT) texture")

			// Create dynamic texture asset
			return rendererRuntime.getTextureResourceManager().createTextureResourceByAssetId("Unrimp/Texture/DynamicByCode/IdentityColorCorrectionLookupTable3D", *texturePtr);
		}

		/**
		*  @brief
		*    Create 1D screen space ambient occlusion sample kernel texture
		*
		*  @remarks
		*    The sample kernel requirements are that:
		*    - Sample positions fall within the unit hemisphere
		*    - Sample positions are more densely clustered towards the origin. This effectively attenuates the occlusion contribution
		*      according to distance from the kernel center - samples closer to a point occlude it more than samples further away
		*
		*  @note
		*    - Basing on "SSAO Tutorial" from John Chapman - http://john-chapman-graphics.blogspot.de/2013/01/ssao-tutorial.html
		*    - Kernel size is 16, since the samples are randomly distributed this doesn't mean that a shader has to use all samples
		*    - Resulting texture asset ID is "Unrimp/Texture/DynamicByCode/ScreenSpaceAmbientOcclusionSampleKernel"
		*/
		RendererRuntime::TextureResourceId createScreenSpaceAmbientOcclusionSampleKernelTexture(const RendererRuntime::IRendererRuntime& rendererRuntime)
		{
			static const uint32_t KERNEL_SIZE = 16;
			glm::vec4 kernel[KERNEL_SIZE];

			{ // Create the kernel
				std::mt19937 randomGenerator;
				std::uniform_real_distribution<float> randomDistributionHalf(0.0f, 1.0f);
				std::uniform_real_distribution<float> randomDistributionFull(-1.0f, 1.0f);
				for (uint32_t i = 0; i < KERNEL_SIZE; ++i)
				{
					// Create a sample point on the surface of a hemisphere oriented along the z axis
					kernel[i] = glm::vec4(randomDistributionFull(randomGenerator), randomDistributionFull(randomGenerator), randomDistributionHalf(randomGenerator), 0.0f);
					kernel[i] = glm::normalize(kernel[i]);

					// Distribute the sample position within the hemisphere
					kernel[i] *= randomDistributionHalf(randomGenerator);

					// Apply accelerating interpolation function to generate more points closer to the origin
					float scale = float(i) / float(KERNEL_SIZE);
					scale = glm::mix(0.1f, 1.0f, scale * scale);	// Linear interpolation (= "lerp" = "mix")
					kernel[i] *= scale;
				}
			}

			// Create the renderer texture resource
			Renderer::ITexturePtr texturePtr(rendererRuntime.getTextureManager().createTexture1D(KERNEL_SIZE, Renderer::TextureFormat::R32G32B32A32F, kernel));
			RENDERER_SET_RESOURCE_DEBUG_NAME(texturePtr, "1D screen space ambient occlusion sample kernel texture")

			// Create dynamic texture asset
			return rendererRuntime.getTextureResourceManager().createTextureResourceByAssetId("Unrimp/Texture/DynamicByCode/ScreenSpaceAmbientOcclusionSampleKernel", *texturePtr);
		}

		/**
		*  @brief
		*    Create 2D screen space ambient occlusion 4x4 noise texture
		*
		*  @remarks
		*    When used for screen space ambient occlusion, the noise which is tiled over the screen is used to rotate the sample kernel. This will effectively increase the
		*    sample count and minimize "banding" artifacts. The tiling of the texture causes the orientation of the kernel to be repeated and introduces regularity into the
		*    result. By keeping the texture size small we can make this regularity occur at a high frequency, which can then be removed with a blur step that preserves the
		*    low-frequency detail of the image. Using a 4x4 texture and blur kernel produces excellent results at minimal cost. This is the same approach as used in Crysis.
		*
		*  @note
		*    - Basing on "SSAO Tutorial" from John Chapman - http://john-chapman-graphics.blogspot.de/2013/01/ssao-tutorial.html
		*    - Noise texture size is 4x4
		*    - Resulting texture asset ID is "Unrimp/Texture/DynamicByCode/ScreenSpaceAmbientOcclusionNoise4x4"
		*/
		RendererRuntime::TextureResourceId createScreenSpaceAmbientOcclusionNoiseTexture4x4(const RendererRuntime::IRendererRuntime& rendererRuntime)
		{
			static const uint32_t NOISE_SIZE = 4;
			static const uint32_t SQUARED_NOISE_SIZE = NOISE_SIZE * NOISE_SIZE;
			glm::vec4 noise[SQUARED_NOISE_SIZE];

			{ // Create the noise
				std::mt19937 randomGenerator;
				std::uniform_real_distribution<float> randomDistribution(-1.0f, 1.0f);
				for (uint32_t i = 0; i < SQUARED_NOISE_SIZE; ++i)
				{
					noise[i] = glm::vec4(randomDistribution(randomGenerator), randomDistribution(randomGenerator), 0.0f, 0.0f);
					noise[i] = glm::normalize(noise[i]);
				}
			}

			// Create the renderer texture resource
			Renderer::ITexturePtr texturePtr(rendererRuntime.getTextureManager().createTexture2D(NOISE_SIZE, NOISE_SIZE, Renderer::TextureFormat::R32G32B32A32F, noise));
			RENDERER_SET_RESOURCE_DEBUG_NAME(texturePtr, "2D screen space ambient occlusion 4x4 noise texture")

			// Create dynamic texture asset
			return rendererRuntime.getTextureResourceManager().createTextureResourceByAssetId("Unrimp/Texture/DynamicByCode/ScreenSpaceAmbientOcclusionNoise4x4", *texturePtr);
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
	//[ Protected virtual RendererRuntime::IMaterialBlueprintResourceListener methods ]
	//[-------------------------------------------------------]
	void MaterialBlueprintResourceListener::onStartup(const IRendererRuntime& rendererRuntime)
	{
		mIdentityColorCorrectionLookupTable3D = ::detail::createIdentityColorCorrectionLookupTable3D(rendererRuntime);
		mScreenSpaceAmbientOcclusionSampleKernelTextureResourceId = ::detail::createScreenSpaceAmbientOcclusionSampleKernelTexture(rendererRuntime);
		mScreenSpaceAmbientOcclusionNoiseTexture4x4ResourceId = ::detail::createScreenSpaceAmbientOcclusionNoiseTexture4x4(rendererRuntime);
	}

	void MaterialBlueprintResourceListener::onShutdown(const IRendererRuntime& rendererRuntime)
	{
		TextureResourceManager& textureResourceManager = rendererRuntime.getTextureResourceManager();
		textureResourceManager.destroyTextureResource(mIdentityColorCorrectionLookupTable3D);
		textureResourceManager.destroyTextureResource(mScreenSpaceAmbientOcclusionSampleKernelTextureResourceId);
		textureResourceManager.destroyTextureResource(mScreenSpaceAmbientOcclusionNoiseTexture4x4ResourceId);
	}

	void MaterialBlueprintResourceListener::beginFillPass(IRendererRuntime& rendererRuntime, const Renderer::IRenderTarget& renderTarget, const CompositorContextData& compositorContextData, PassBufferManager::PassData& passData)
	{
		// Remember the pass data memory address of the current scope
		mRendererRuntime = &rendererRuntime;
		mPassData = &passData;
		mCompositorContextData = &compositorContextData;

		// Get the render target with and height
		renderTarget.getWidthAndHeight(mRenderTargetWidth, mRenderTargetHeight);

		// Get camera settings
		const CameraSceneItem* cameraSceneItem = compositorContextData.getCameraSceneItem();
		mNearZ = (nullptr != cameraSceneItem) ? cameraSceneItem->getNearZ() : CameraSceneItem::DEFAULT_NEAR_Z;
		mFarZ  = (nullptr != cameraSceneItem) ? cameraSceneItem->getFarZ()  : CameraSceneItem::DEFAULT_FAR_Z;

		// Calculate required matrices basing whether or not the VR-manager is currently running
		glm::mat4 viewTranslateMatrix;
		glm::mat4 viewSpaceToClipSpaceMatrix;
		const IVrManager& vrManager = rendererRuntime.getVrManager();
		if (vrManager.isRunning() && VrEye::UNKNOWN != getCurrentRenderedVrEye() && !cameraSceneItem->hasCustomWorldSpaceToViewSpaceMatrix() && !cameraSceneItem->hasCustomViewSpaceToClipSpaceMatrix())
		{
			// Virtual reality rendering
			const IVrManager::VrEye vrEye = static_cast<IVrManager::VrEye>(getCurrentRenderedVrEye());
			viewSpaceToClipSpaceMatrix = vrManager.getHmdViewSpaceToClipSpaceMatrix(vrEye, mNearZ, mFarZ);
			viewTranslateMatrix = vrManager.getHmdEyeSpaceToHeadSpaceMatrix(vrEye) * vrManager.getHmdPoseMatrix();

			// Calculate the final matrices
			const Transform& worldSpaceToViewSpaceTransform = (nullptr != cameraSceneItem) ? cameraSceneItem->getWorldSpaceToViewSpaceTransform() : Transform::IDENTITY;
			mPassData->worldSpaceToViewSpaceMatrix = glm::translate(glm::mat4(1.0f), worldSpaceToViewSpaceTransform.position) * glm::toMat4(worldSpaceToViewSpaceTransform.rotation);
		}
		else if (nullptr != cameraSceneItem)
		{
			// Standard rendering using a camera scene item

			// Get world space to view space matrix (Aka "view matrix")
			mPassData->worldSpaceToViewSpaceMatrix = cameraSceneItem->getWorldSpaceToViewSpaceMatrix();

			// Get view space to clip space matrix (aka "projection matrix")
			viewSpaceToClipSpaceMatrix = cameraSceneItem->getViewSpaceToClipSpaceMatrix(static_cast<float>(mRenderTargetWidth) / mRenderTargetHeight);
		}
		else
		{
			// Standard rendering

			// Get world space to view space matrix (Aka "view matrix")
			mPassData->worldSpaceToViewSpaceMatrix = glm::lookAt(Transform::IDENTITY.position, Transform::IDENTITY.position + Transform::IDENTITY.rotation * Math::FORWARD_VECTOR, Math::UP_VECTOR);

			// Get view space to clip space matrix (aka "projection matrix")
			viewSpaceToClipSpaceMatrix = glm::perspective(CameraSceneItem::DEFAULT_FOV_Y, static_cast<float>(mRenderTargetWidth) / mRenderTargetHeight, CameraSceneItem::DEFAULT_NEAR_Z, CameraSceneItem::DEFAULT_FAR_Z);
		}
		mPassData->worldSpaceToViewSpaceQuaternion = glm::quat(mPassData->worldSpaceToViewSpaceMatrix);
		mPassData->worldSpaceToClipSpaceMatrix = viewSpaceToClipSpaceMatrix * viewTranslateMatrix * mPassData->worldSpaceToViewSpaceMatrix;
		mPassData->viewSpaceToClipSpaceMatrix = viewSpaceToClipSpaceMatrix;
	}

	bool MaterialBlueprintResourceListener::fillPassValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes)
	{
		bool valueFilled = true;

		// Resolve the reference value
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
		else if (::detail::VIEW_SPACE_TO_WORLD_SPACE_QUATERNION == referenceValue)
		{
			assert(sizeof(float) * 4 == numberOfBytes);
			memcpy(buffer, glm::value_ptr(glm::inverse(mPassData->worldSpaceToViewSpaceQuaternion)), numberOfBytes);
		}
		else if (::detail::WORLD_SPACE_TO_CLIP_SPACE_MATRIX == referenceValue)
		{
			assert(sizeof(float) * 4 * 4 == numberOfBytes);
			memcpy(buffer, glm::value_ptr(mPassData->worldSpaceToClipSpaceMatrix), numberOfBytes);
		}
		else if (::detail::VIEW_SPACE_TO_CLIP_SPACE_MATRIX == referenceValue)
		{
			assert(sizeof(float) * 4 * 4 == numberOfBytes);
			memcpy(buffer, glm::value_ptr(mPassData->viewSpaceToClipSpaceMatrix), numberOfBytes);
		}
		else if (::detail::VIEW_SPACE_TO_TEXTURE_SPACE_MATRIX == referenceValue)
		{
			assert(sizeof(float) * 4 * 4 == numberOfBytes);
			memcpy(buffer, glm::value_ptr(Math::getTextureScaleBiasMatrix(mRendererRuntime->getRenderer()) * mPassData->viewSpaceToClipSpaceMatrix), numberOfBytes);
		}
		else if (::detail::CLIP_SPACE_TO_VIEW_SPACE_MATRIX == referenceValue)
		{
			assert(sizeof(float) * 4 * 4 == numberOfBytes);
			memcpy(buffer, glm::value_ptr(glm::inverse(mPassData->viewSpaceToClipSpaceMatrix)), numberOfBytes);
		}
		else if (::detail::CLIP_SPACE_TO_WORLD_SPACE_MATRIX == referenceValue)
		{
			assert(sizeof(float) * 4 * 4 == numberOfBytes);
			memcpy(buffer, glm::value_ptr(glm::inverse(mPassData->worldSpaceToClipSpaceMatrix)), numberOfBytes);
		}
		else if (::detail::CAMERA_WORLD_SPACE_POSITION == referenceValue)
		{
			assert(sizeof(float) * 3 == numberOfBytes);
			const CameraSceneItem* cameraSceneItem = mCompositorContextData->getCameraSceneItem();
			memcpy(buffer, glm::value_ptr((nullptr != cameraSceneItem) ? cameraSceneItem->getParentSceneNodeSafe().getTransform().position : Math::ZERO_VECTOR), numberOfBytes);
		}
		else if (::detail::PROJECTION_PARAMETERS == referenceValue)
		{
			assert(sizeof(float) * 2 == numberOfBytes);
			const float projectionParameters[2] = { mFarZ / (mFarZ - mNearZ), (-mFarZ * mNearZ) / (mFarZ - mNearZ) };
			memcpy(buffer, &projectionParameters[0], numberOfBytes);
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
			glm::vec3 worldSpaceSunLightDirection;
			const LightSceneItem* lightSceneItem = mCompositorContextData->getLightSceneItem();
			if (nullptr != lightSceneItem && nullptr != lightSceneItem->getParentSceneNode())
			{
				worldSpaceSunLightDirection = lightSceneItem->getParentSceneNode()->getTransform().rotation * Math::FORWARD_VECTOR;
			}
			else
			{
				// Error!
				assert(false);
				worldSpaceSunLightDirection = Math::FORWARD_VECTOR;
			}
			const glm::vec3 viewSpaceSunLightDirection = glm::normalize(mPassData->worldSpaceToViewSpaceQuaternion * worldSpaceSunLightDirection);	// Normalize shouldn't be necessary, but last chance here to correct rounding errors before the shader is using the normalized direction vector
			memcpy(buffer, glm::value_ptr(viewSpaceSunLightDirection), numberOfBytes);
		}
		else if (::detail::VIEWPORT_SIZE == referenceValue)
		{
			assert(sizeof(float) * 2 == numberOfBytes);
			float* floatBuffer = reinterpret_cast<float*>(buffer);

			// 0 = Viewport width
			// 1 = Viewport height
			floatBuffer[0] = static_cast<float>(mRenderTargetWidth);
			floatBuffer[1] = static_cast<float>(mRenderTargetHeight);
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
		else if (::detail::NUMBER_OF_LIGHTS == referenceValue)
		{
			assert(sizeof(int) == numberOfBytes);
			*reinterpret_cast<int*>(buffer) = static_cast<int>(mRendererRuntime->getMaterialBlueprintResourceManager().getLightBufferManager().getNumberOfLights());
		}
		else if (::detail::SHADOW_MATRIX == referenceValue)
		{
			assert(sizeof(float) * 4 * 4 == numberOfBytes);
			const CompositorInstancePassShadowMap* compositorInstancePassShadowMap = mCompositorContextData->getCompositorInstancePassShadowMap();
			if (nullptr != compositorInstancePassShadowMap)
			{
				memcpy(buffer, glm::value_ptr(compositorInstancePassShadowMap->getPassData().shadowMatrix), numberOfBytes);
			}
			else
			{
				// Error!
				assert(false);
				memset(buffer, 0, numberOfBytes);
			}
		}
		else if (::detail::SHADOW_CASCADE_SPLITS == referenceValue)
		{
			assert(sizeof(float) * 4 == numberOfBytes);
			const CompositorInstancePassShadowMap* compositorInstancePassShadowMap = mCompositorContextData->getCompositorInstancePassShadowMap();
			if (nullptr != compositorInstancePassShadowMap)
			{
				memcpy(buffer, compositorInstancePassShadowMap->getPassData().cascadeSplits, numberOfBytes);
			}
			else
			{
				// Error!
				assert(false);
				memset(buffer, 0, numberOfBytes);
			}
		}
		else if (::detail::SHADOW_CASCADE_OFFSETS == referenceValue)
		{
			assert(sizeof(float) * 4 * 4 == numberOfBytes);
			const CompositorInstancePassShadowMap* compositorInstancePassShadowMap = mCompositorContextData->getCompositorInstancePassShadowMap();
			if (nullptr != compositorInstancePassShadowMap)
			{
				memcpy(buffer, compositorInstancePassShadowMap->getPassData().cascadeOffsets, numberOfBytes);
			}
			else
			{
				// Error!
				assert(false);
				memset(buffer, 0, numberOfBytes);
			}
		}
		else if (::detail::SHADOW_CASCADE_SCALES == referenceValue)
		{
			assert(sizeof(float) * 4 * 4 == numberOfBytes);
			const CompositorInstancePassShadowMap* compositorInstancePassShadowMap = mCompositorContextData->getCompositorInstancePassShadowMap();
			if (nullptr != compositorInstancePassShadowMap)
			{
				memcpy(buffer, compositorInstancePassShadowMap->getPassData().cascadeScales, numberOfBytes);
			}
			else
			{
				// Error!
				assert(false);
				memset(buffer, 0, numberOfBytes);
			}
		}
		else if (::detail::LENS_STAR_MATRIX == referenceValue)
		{
			assert(sizeof(float) * 4 * 4 == numberOfBytes);

			// The following is basing on 'Pseudo Lens Flare' from John Chapman - http://john-chapman-graphics.blogspot.de/2013/02/pseudo-lens-flare.html

			// Get the camera rotation; it just needs to change continuously as the camera rotates
			glm::vec3 cameraX = mPassData->worldSpaceToViewSpaceMatrix[0];	// Camera x (left) vector
			glm::vec3 cameraZ = mPassData->worldSpaceToViewSpaceMatrix[1];	// Camera z (forward) vector
			const float cameraRotation = glm::dot(cameraX, glm::vec3(0.0f, 0.0f, 1.0f)) + glm::dot(cameraZ, glm::vec3(0.0f, 1.0f, 0.0f));

			// Calculate the lens star matrix
			const glm::mat3 scaleBias1(
				2.0f, 0.0f, -1.0f,
				0.0f, 2.0f, -1.0f,
				0.0f, 0.0f,  1.0f
			);
			const glm::mat3 rotation(
				glm::cos(cameraRotation), -glm::sin(cameraRotation),  0.0f,
				glm::sin(cameraRotation),  glm::cos(cameraRotation),  0.0f,
				0.0f,                      0.0f,                      1.0f
			);
			const glm::mat3 scaleBias2(
				0.5f, 0.0f, 0.5f,
				0.0f, 0.5f, 0.5f,
				0.0f, 0.0f, 1.0f
			);
			const glm::mat4 uLensStarMatrix = scaleBias2 * rotation * scaleBias1;

			// Copy the matrix over
			memcpy(buffer, glm::value_ptr(uLensStarMatrix), numberOfBytes);
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
