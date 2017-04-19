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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Vr/IVrManager.h"
#include "RendererRuntime/Resource/IResourceListener.h"

#include <Renderer/Public/Renderer.h>

#include <openvr/openvr.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class ISceneNode;
	class IRendererRuntime;
	class OpenVRRuntimeLinking;
	class IVrManagerOpenVRListener;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t MaterialResourceId;	///< POD material resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class VrManagerOpenVR : private IVrManager, public IResourceListener
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT static const VrManagerTypeId TYPE_ID;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline IVrManagerOpenVRListener& getVrManagerOpenVRListener() const;
		RENDERERRUNTIME_API_EXPORT void setVrManagerOpenVRListener(IVrManagerOpenVRListener* vrManagerOpenVRListener);	// Does not take over the control of the memory
		inline vr::IVRSystem* getVrSystem() const;
		inline const vr::TrackedDevicePose_t& getVrTrackedDevicePose(vr::TrackedDeviceIndex_t trackedDeviceIndex) const;
		inline const glm::mat4& getDevicePoseMatrix(vr::TrackedDeviceIndex_t trackedDeviceIndex) const;


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IVrManager methods    ]
	//[-------------------------------------------------------]
	public:
		virtual VrManagerTypeId getVrManagerTypeId() const override;
		virtual bool isHmdPresent() const override;
		virtual void setSceneResource(ISceneResource* sceneResource) override;
		virtual bool startup(AssetId vrDeviceMaterialAssetId) override;
		inline virtual bool isRunning() const override;
		virtual void shutdown() override;
		virtual void updateHmdMatrixPose(CameraSceneItem* cameraSceneItem) override;
		virtual glm::mat4 getHmdViewSpaceToClipSpaceMatrix(VrEye vrEye, float nearZ, float farZ) const override;
		virtual glm::mat4 getHmdEyeSpaceToHeadSpaceMatrix(VrEye vrEye) const override;
		inline virtual const glm::mat4& getHmdPoseMatrix() const override;


	//[-------------------------------------------------------]
	//[ Private virtual RendererRuntime::IVrManager methods   ]
	//[-------------------------------------------------------]
	private:
		virtual void executeCompositorWorkspaceInstance(CompositorWorkspaceInstance& compositorWorkspaceInstance, Renderer::IRenderTarget& renderTarget, CameraSceneItem* cameraSceneItem, const LightSceneItem* lightSceneItem) override;


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::IResourceListener methods ]
	//[-------------------------------------------------------]
	protected:
		virtual void onLoadingStateChange(const IResource& resource) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit VrManagerOpenVR(IRendererRuntime& rendererRuntime);
		virtual ~VrManagerOpenVR();
		VrManagerOpenVR(const VrManagerOpenVR&) = delete;
		VrManagerOpenVR& operator=(const VrManagerOpenVR&) = delete;
		void setupRenderModelForTrackedDevice(vr::TrackedDeviceIndex_t unTrackedDeviceIndex);


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		struct Component
		{
			std::string name;
			ISceneNode* sceneNode = nullptr;	// TODO(co) No crazy raw-pointers
			Component(const std::string& _name, ISceneNode* _sceneNode) :
				name(_name),
				sceneNode(_sceneNode)
			{};
		};
		typedef std::vector<Component> Components;
		struct TrackedDeviceInformation
		{
			std::string renderModelName;
			Components  components;
		};


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&		   mRendererRuntime;			///< Renderer runtime instance, do not destroy the instance
		IVrManagerOpenVRListener*  mVrManagerOpenVRListener;	///< OpenVR manager listener, always valid, do not destroy the instance
		bool					   mVrDeviceMaterialResourceLoaded;
		MaterialResourceId		   mVrDeviceMaterialResourceId;
		ISceneResource*			   mSceneResource;			// TODO(co) No crazy raw-pointers
		ISceneNode*				   mSceneNodes[vr::k_unMaxTrackedDeviceCount];	// TODO(co) No crazy raw-pointers
		TrackedDeviceInformation   mTrackedDeviceInformation[vr::k_unMaxTrackedDeviceCount];
		OpenVRRuntimeLinking*	   mOpenVRRuntimeLinking;
		vr::EGraphicsAPIConvention mVrGraphicsAPIConvention;
		vr::IVRSystem*			   mVrSystem;
		vr::IVRRenderModels*	   mVrRenderModels;
		bool					   mShowRenderModels;
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
#include "RendererRuntime/Vr/OpenVR/VrManagerOpenVR.inl"
