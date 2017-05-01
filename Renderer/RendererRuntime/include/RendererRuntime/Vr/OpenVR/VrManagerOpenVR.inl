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
#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline IVrManagerOpenVRListener& VrManagerOpenVR::getVrManagerOpenVRListener() const
	{
		// We know this pointer must always be valid
		assert(nullptr != mVrManagerOpenVRListener);
		return *mVrManagerOpenVRListener;
	}

	inline vr::IVRSystem* VrManagerOpenVR::getVrSystem() const
	{
		return mVrSystem;
	}

	inline MaterialResourceId VrManagerOpenVR::getVrDeviceMaterialResourceId() const
	{
		return mVrDeviceMaterialResourceId;
	}

	inline const VrManagerOpenVR::RenderModelNames& VrManagerOpenVR::getRenderModelNames() const
	{
		return mRenderModelNames;
	}

	inline const vr::TrackedDevicePose_t& VrManagerOpenVR::getVrTrackedDevicePose(vr::TrackedDeviceIndex_t trackedDeviceIndex) const
	{
		assert(trackedDeviceIndex < vr::k_unMaxTrackedDeviceCount);
		return mVrTrackedDevicePose[trackedDeviceIndex];
	}

	inline const glm::mat4& VrManagerOpenVR::getDevicePoseMatrix(vr::TrackedDeviceIndex_t trackedDeviceIndex) const
	{
		assert(trackedDeviceIndex < vr::k_unMaxTrackedDeviceCount);
		return mDevicePoseMatrix[trackedDeviceIndex];
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IVrManager methods    ]
	//[-------------------------------------------------------]
	inline bool VrManagerOpenVR::isRunning() const
	{
		return (nullptr != mVrSystem);
	}

	inline const glm::mat4& VrManagerOpenVR::getHmdPoseMatrix() const
	{
		return mHmdPoseMatrix;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
