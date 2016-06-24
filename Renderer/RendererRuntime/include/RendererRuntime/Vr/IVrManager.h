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
#include "RendererRuntime/Core/Manager.h"

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4201)	// warning C4201: nonstandard extension used: nameless struct/union
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#include <glm/glm.hpp>
#pragma warning(pop)


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRenderTarget;
}
namespace RendererRuntime
{
	class ISceneResource;
	class CameraSceneItem;
	class CompositorInstance;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class IVrManager : private Manager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		enum class VrEye : int8_t
		{
			LEFT  =  0,
			RIGHT =  1
		};


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IVrManager methods    ]
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
		virtual bool isHmdPresent() const = 0;

		//[-------------------------------------------------------]
		//[ Lifecycle                                             ]
		//[-------------------------------------------------------]
		virtual void setSceneResource(ISceneResource* sceneResource) = 0;
		virtual bool startup() = 0;
		virtual bool isRunning() const = 0;
		virtual void shutdown() = 0;

		//[-------------------------------------------------------]
		//[ Transform (only valid if manager is running)          ]
		//[-------------------------------------------------------]
		virtual void updateHmdMatrixPose() = 0;
		virtual glm::mat4 getHmdViewSpaceToClipSpaceMatrix(VrEye vrEye, float nearZ, float farZ) const = 0;
		virtual glm::mat4 getHmdEyeSpaceToHeadSpaceMatrix(VrEye vrEye) const = 0;
		virtual const glm::mat4& getHmdPoseMatrix() const = 0;

		//[-------------------------------------------------------]
		//[ Render (only valid if manager is running)             ]
		//[-------------------------------------------------------]
		virtual void executeCompositorInstance(CompositorInstance& compositorInstance, Renderer::IRenderTarget& renderTarget, CameraSceneItem* cameraSceneItem) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline IVrManager();
		inline virtual ~IVrManager();
		IVrManager(const IVrManager&) = delete;
		IVrManager& operator=(const IVrManager&) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Vr/IVrManager.inl"