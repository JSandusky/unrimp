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
#include "RendererRuntime/Vr/VrManager.h"
#include "RendererRuntime/Vr/Detail/OpenVRRuntimeLinking.h"
#include "RendererRuntime/Resource/Compositor/CompositorInstance.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/IMaterialBlueprintResourceListener.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <cassert>


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
		glm::mat4 convertOpenVrMatrixToGlmMat4(const vr::HmdMatrix44_t& vrHmdMatrix34)
		{
			return glm::mat4(
				vrHmdMatrix34.m[0][0], vrHmdMatrix34.m[1][0], vrHmdMatrix34.m[2][0], vrHmdMatrix34.m[3][0],
				vrHmdMatrix34.m[0][1], vrHmdMatrix34.m[1][1], vrHmdMatrix34.m[2][1], vrHmdMatrix34.m[3][1],
				vrHmdMatrix34.m[0][2], vrHmdMatrix34.m[1][2], vrHmdMatrix34.m[2][2], vrHmdMatrix34.m[3][2],
				vrHmdMatrix34.m[0][3], vrHmdMatrix34.m[1][3], vrHmdMatrix34.m[2][3], vrHmdMatrix34.m[3][3]
				);
		}

		glm::mat4 convertOpenVrMatrixToGlmMat4(const vr::HmdMatrix34_t& vrHmdMatrix34)
		{
			return glm::mat4(
				vrHmdMatrix34.m[0][0], vrHmdMatrix34.m[1][0], vrHmdMatrix34.m[2][0], 0.0f,
				vrHmdMatrix34.m[0][1], vrHmdMatrix34.m[1][1], vrHmdMatrix34.m[2][1], 0.0f,
				vrHmdMatrix34.m[0][2], vrHmdMatrix34.m[1][2], vrHmdMatrix34.m[2][2], 0.0f,
				vrHmdMatrix34.m[0][3], vrHmdMatrix34.m[1][3], vrHmdMatrix34.m[2][3], 1.0f
				);
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
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	bool VrManager::isHmdPresent() const
	{
		return (mOpenVRRuntimeLinking->isOpenVRAvaiable() && vr::VR_IsRuntimeInstalled() && vr::VR_IsHmdPresent());
	}

	bool VrManager::startup()
	{
		assert(nullptr == mVrSystem);
		if (nullptr == mVrSystem)
		{
			// OpenVR "IVRSystem_012" (Jun 10, 2016) only works with OpenGL and DirectX 11
			Renderer::IRenderer& renderer = mRendererRuntime.getRenderer();
			const bool isOpenGLRenderer = (0 == strcmp(renderer.getName(), "OpenGL"));
			const bool isDirect3D11Renderer = (0 == strcmp(renderer.getName(), "Direct3D11"));
			if (!isOpenGLRenderer && !isDirect3D11Renderer)
			{
				// Error!
				RENDERERRUNTIME_OUTPUT_DEBUG_STRING("Error: OpenVR \"IVRSystem_012\" (Jun 10, 2016) only works with OpenGL and DirectX 11");
				return false;
			}
			mVrGraphicsAPIConvention = isOpenGLRenderer ? vr::API_OpenGL : vr::API_DirectX;

			// Initialize the OpenVR system
			vr::EVRInitError vrInitError = vr::VRInitError_None;
			mVrSystem = vr::VR_Init(&vrInitError, vr::VRApplication_Scene);
			if (vr::VRInitError_None != vrInitError)
			{
				// Error!
				RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF("Error: Unable to initialize OpenVR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(vrInitError));
				return false;
			}

			{ // Create renderer resources
				// Create the texture instance
				uint32_t width = 0;
				uint32_t height = 0;
				mVrSystem->GetRecommendedRenderTargetSize(&width, &height);
				Renderer::ITexture* colorTexture2D = mColorTexture2D = renderer.createTexture2D(width, height, Renderer::TextureFormat::R8G8B8A8, nullptr, Renderer::TextureFlag::RENDER_TARGET);
				Renderer::ITexture* depthStencilTexture2D = renderer.createTexture2D(width, height, Renderer::TextureFormat::D32_FLOAT, nullptr, Renderer::TextureFlag::RENDER_TARGET);

				// Create the framebuffer object (FBO) instance
				mFramebuffer = renderer.createFramebuffer(1, &colorTexture2D, depthStencilTexture2D);
			}
		}

		// Done
		return true;
	}

	void VrManager::shutdown()
	{
		if (nullptr != mVrSystem)
		{
			{ // Release renderer resources
				mFramebuffer = nullptr;
				mColorTexture2D = nullptr;
			}

			// De-initialize the OpenVR system
			vr::VR_Shutdown();
			mVrSystem = nullptr;
		}
	}

	void VrManager::updateHmdMatrixPose()
	{
		assert(nullptr != mVrSystem);

		{ // Process OpenVR events
			vr::VREvent_t vrVREvent;
			while (mVrSystem->PollNextEvent(&vrVREvent, sizeof(vr::VREvent_t)))
			{
				// For now, nothing to do in here, just be polite and eat up the received events
			}
		}

		// Request poses from OpenVR
		vr::VRCompositor()->WaitGetPoses(mVrTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

		// Gather all valid poses
		mNumberOfValidDevicePoses = 0;
		for (int32_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; ++deviceIndex)
		{
			if (mVrTrackedDevicePose[deviceIndex].bPoseIsValid)
			{
				++mNumberOfValidDevicePoses;
				mDevicePoseMatrix[deviceIndex] = ::detail::convertOpenVrMatrixToGlmMat4(mVrTrackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking);
			}
		}

		// Backup HMD pose
		if (mVrTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
		{
			mHmdPoseMatrix = glm::inverse(mDevicePoseMatrix[vr::k_unTrackedDeviceIndex_Hmd]);
		}
	}

	glm::mat4 VrManager::getHmdViewSpaceToClipSpaceMatrix(vr::Hmd_Eye vrHmdEye, float nearZ, float farZ) const
	{
		assert(nullptr != mVrSystem);
		return ::detail::convertOpenVrMatrixToGlmMat4(mVrSystem->GetProjectionMatrix(vrHmdEye, nearZ, farZ, vr::API_DirectX));
	}

	glm::mat4 VrManager::getHmdEyeSpaceToHeadSpaceMatrix(vr::Hmd_Eye vrHmdEye) const
	{
		assert(nullptr != mVrSystem);
		return glm::inverse(::detail::convertOpenVrMatrixToGlmMat4(mVrSystem->GetEyeToHeadTransform(vrHmdEye)));
	}

	void VrManager::executeCompositorInstance(CompositorInstance& compositorInstance, Renderer::IRenderTarget&, CameraSceneItem* cameraSceneItem)
	{
		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = mRendererRuntime.getMaterialBlueprintResourceManager().getMaterialBlueprintResourceListener();
		for (int8_t eyeIndex = 0; eyeIndex < 2; ++eyeIndex)
		{
			// Execute the compositor instance
			materialBlueprintResourceListener.setCurrentRenderedVrEye(static_cast<IMaterialBlueprintResourceListener::VrEye>(eyeIndex));
			compositorInstance.execute(*mFramebuffer, cameraSceneItem);

			// Submit the rendered texture to the OpenVR compositor
			const vr::Texture_t vrTexture = { mColorTexture2D->getInternalResourceHandle(), mVrGraphicsAPIConvention, vr::ColorSpace_Auto };
			vr::VRCompositor()->Submit(static_cast<vr::Hmd_Eye>(eyeIndex), &vrTexture);
		}
		materialBlueprintResourceListener.setCurrentRenderedVrEye(IMaterialBlueprintResourceListener::VrEye::UNKNOWN);

		// TODO(co) Optionally mirror the result on the given render target
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	VrManager::VrManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mOpenVRRuntimeLinking(new OpenVRRuntimeLinking()),
		mVrGraphicsAPIConvention(vr::API_OpenGL),
		mVrSystem(nullptr),
		mNumberOfValidDevicePoses(0)
	{
		// Nothing here
	}

	VrManager::~VrManager()
	{
		shutdown();
		delete mOpenVRRuntimeLinking;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
