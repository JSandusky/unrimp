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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Export.h"
#include "RendererRuntime/Core/Manager.h"

#include <Renderer/Public/Renderer.h>

#include <openvr/openvr.h>

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4201)	// warning C4201: nonstandard extension used: nameless struct/union
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#include <glm/glm.hpp>
#pragma warning(pop)


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class CameraSceneItem;
	class IRendererRuntime;
	class CompositorInstance;
	class OpenVRRuntimeLinking;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class VrManager : private Manager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Check whether or not a head-mounted display (HMD) is present
		*
		*  @return
		*    "true" if a HMD is present, else "false" (OpenVR shared library not there? OpenVR runtime not installed? HMD not connected?)
		*
		*  @note
		*    - The VR manager is using OpenVR with runtime linking, so you need to ensure the OpenVR shared library
		*      can be loaded ("openvr_api.dll" under MS Windows, "libopenvr_api.so" under Linux)
		*    - Method can also be used when the VR manager is not running
		*/
		RENDERERRUNTIME_API_EXPORT bool isHmdPresent() const;

		//[-------------------------------------------------------]
		//[ Lifecycle                                             ]
		//[-------------------------------------------------------]
		RENDERERRUNTIME_API_EXPORT bool startup();
		inline bool isRunning() const;
		RENDERERRUNTIME_API_EXPORT void shutdown();

		//[-------------------------------------------------------]
		//[ Transform (only valid if manager is running)          ]
		//[-------------------------------------------------------]
		RENDERERRUNTIME_API_EXPORT void updateHmdMatrixPose();
		RENDERERRUNTIME_API_EXPORT glm::mat4 getHmdViewSpaceToClipSpaceMatrix(vr::Hmd_Eye vrHmdEye, float nearZ, float farZ) const;
		RENDERERRUNTIME_API_EXPORT glm::mat4 getHmdEyeSpaceToHeadSpaceMatrix(vr::Hmd_Eye vrHmdEye) const;
		inline const glm::mat4& getHmdPoseMatrix() const;

		//[-------------------------------------------------------]
		//[ Render                                                ]
		//[-------------------------------------------------------]
		RENDERERRUNTIME_API_EXPORT void executeCompositorInstance(CompositorInstance& compositorInstance, Renderer::IRenderTarget& renderTarget, CameraSceneItem* cameraSceneItem);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	protected:
		explicit VrManager(IRendererRuntime& rendererRuntime);
		virtual ~VrManager();
		VrManager(const VrManager&) = delete;
		VrManager& operator=(const VrManager&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&		   mRendererRuntime;			///< Renderer runtime instance, do not destroy the instance
		OpenVRRuntimeLinking*	   mOpenVRRuntimeLinking;
		vr::EGraphicsAPIConvention mVrGraphicsAPIConvention;
		vr::IVRSystem*			   mVrSystem;
		// Transform
		vr::TrackedDevicePose_t	mVrTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
		glm::mat4				mDevicePoseMatrix[vr::k_unMaxTrackedDeviceCount];
		uint32_t				mNumberOfValidDevicePoses;
		glm::mat4				mHmdPoseMatrix;
		// Renderer resources
		Renderer::ITexture2DPtr	  mColorTexture2D;	///< Color 2D texture, can be a null pointer
		Renderer::IFramebufferPtr mFramebuffer;		///< Framebuffer object (FBO), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Vr/VrManager.inl"
