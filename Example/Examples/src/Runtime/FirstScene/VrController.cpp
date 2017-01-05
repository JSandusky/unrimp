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
#include "PrecompiledHeader.h"
#include "Runtime/FirstScene/VrController.h"

#include <RendererRuntime/IRendererRuntime.h>
#include <RendererRuntime/Core/Math/Math.h>
#include <RendererRuntime/Core/Math/EulerAngles.h>
#include <RendererRuntime/Resource/Scene/ISceneResource.h>
#include <RendererRuntime/Resource/Scene/Item/MeshSceneItem.h>
#include <RendererRuntime/Resource/Scene/Item/LightSceneItem.h>
#include <RendererRuntime/Resource/Scene/Item/CameraSceneItem.h>
#include <RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h>
#include <RendererRuntime/Resource/MaterialBlueprint/Listener/MaterialBlueprintResourceListener.h>
#include <RendererRuntime/Vr/OpenVR/VrManagerOpenVR.h>
#include <RendererRuntime/Vr/OpenVR/IVrManagerOpenVRListener.h>

#include <imgui/imgui.h>


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
			DEFINE_CONSTANT(IMGUI_OBJECT_SPACE_TO_CLIP_SPACE_MATRIX)
		#undef DEFINE_CONSTANT


		//[-------------------------------------------------------]
		//[ Classes                                               ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Virtual reality manager OpenVR listener
		*
		*  @todo
		*    - TODO(co) Support the dynamic adding and removal of VR controllers (index updates)
		*/
		class VrManagerOpenVRListener : public RendererRuntime::IVrManagerOpenVRListener
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			VrManagerOpenVRListener() :
				mVrManagerOpenVR(nullptr),
				mNumberOfVrControllers(0)
			{
				for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
				{
					mVrControllerTrackedDeviceIndices[i] = RendererRuntime::getUninitialized<vr::TrackedDeviceIndex_t>();
				}
			}

			virtual ~VrManagerOpenVRListener()
			{
				// Nothing here
			}

			void setVrManagerOpenVR(RendererRuntime::VrManagerOpenVR& vrManagerOpenVR)
			{
				mVrManagerOpenVR = &vrManagerOpenVR;
			}

			uint32_t getNumberOfVrControllers() const
			{
				return mNumberOfVrControllers;
			}

			vr::TrackedDeviceIndex_t getVrControllerTrackedDeviceIndices(uint32_t vrControllerIndex) const
			{
				assert(vrControllerIndex < vr::k_unMaxTrackedDeviceCount);
				return mVrControllerTrackedDeviceIndices[vrControllerIndex];
			}


		//[-------------------------------------------------------]
		//[ Private virtual RendererRuntime::IVrManagerOpenVRListener methods ]
		//[-------------------------------------------------------]
		private:
			virtual void onVrEvent(const vr::VREvent_t& vrVrEvent) override
			{
				switch (vrVrEvent.eventType)
				{
					// Handle quiting the application from Steam
					case vr::VREvent_DriverRequestedQuit:
					case vr::VREvent_Quit:
						// TODO(co)
						NOP;
						break;

					case vr::VREvent_ButtonPress:
						// TODO(co)
						NOP;
						break;
				}
			}

			virtual void onMeshSceneItemCreated(vr::TrackedDeviceIndex_t trackedDeviceIndex, RendererRuntime::MeshSceneItem& meshSceneItem) override
			{
				if (mVrManagerOpenVR->getVrSystem()->GetTrackedDeviceClass(trackedDeviceIndex) == vr::TrackedDeviceClass_Controller)
				{
					// Remember the VR controller tracked device index
					mVrControllerTrackedDeviceIndices[mNumberOfVrControllers] = trackedDeviceIndex;
					++mNumberOfVrControllers;

					// Attach a point light to controllers, this way they can be seen easier and it's possible to illuminate the scene by using the hands
					static_cast<RendererRuntime::LightSceneItem*>(meshSceneItem.getSceneResource().createSceneItem(RendererRuntime::LightSceneItem::TYPE_ID, meshSceneItem.getParentSceneNodeSafe()));
				}
			}


		//[-------------------------------------------------------]
		//[ Private methods                                       ]
		//[-------------------------------------------------------]
		private:
			VrManagerOpenVRListener(const VrManagerOpenVRListener&) = delete;
			VrManagerOpenVRListener& operator=(const VrManagerOpenVRListener&) = delete;


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			RendererRuntime::VrManagerOpenVR* mVrManagerOpenVR;
			uint32_t						  mNumberOfVrControllers;
			vr::TrackedDeviceIndex_t		  mVrControllerTrackedDeviceIndices[vr::k_unMaxTrackedDeviceCount];


		};

		class MaterialBlueprintResourceListener : public RendererRuntime::MaterialBlueprintResourceListener
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			MaterialBlueprintResourceListener() :
				mVrManagerOpenVR(nullptr),
				mVrManagerOpenVRListener(nullptr)
			{
				// Nothing here
			}

			virtual ~MaterialBlueprintResourceListener()
			{
				// Nothing here
			}

			void setVrManagerOpenVR(const RendererRuntime::VrManagerOpenVR& vrManagerOpenVR, const VrManagerOpenVRListener& vrManagerOpenVRListener)
			{
				mVrManagerOpenVR = &vrManagerOpenVR;
				mVrManagerOpenVRListener = &vrManagerOpenVRListener;
			}


		//[-------------------------------------------------------]
		//[ Private virtual RendererRuntime::IMaterialBlueprintResourceListener methods ]
		//[-------------------------------------------------------]
		private:
			virtual bool fillPassValue(uint32_t referenceValue, uint8_t* buffer, uint32_t numberOfBytes) override
			{
				if (::detail::IMGUI_OBJECT_SPACE_TO_CLIP_SPACE_MATRIX == referenceValue && mVrManagerOpenVRListener->getNumberOfVrControllers() >= 2)
				{
					// Place the GUI over the second HTC vive controller (yes, two controllers required)
					assert(sizeof(float) * 4 * 4 == numberOfBytes);
					const ImGuiIO& imGuiIo = ImGui::GetIO();
					const glm::quat rotationOffset = RendererRuntime::EulerAngles::eulerToQuaternion(glm::vec3(glm::degrees(0.0f), glm::degrees(180.0f), 0.0f));
					const glm::mat4 guiScaleMatrix = glm::scale(RendererRuntime::Math::IDENTITY_MATRIX, glm::vec3(1.0f / imGuiIo.DisplaySize.x, 1.0f / imGuiIo.DisplaySize.y, 1.0f));
					const glm::mat4& devicePoseMatrix = mVrManagerOpenVR->getDevicePoseMatrix(mVrManagerOpenVRListener->getVrControllerTrackedDeviceIndices(1));
					const glm::mat4 objectSpaceToClipSpaceMatrix = getPassData().worldSpaceToClipSpaceMatrix * devicePoseMatrix * glm::mat4_cast(rotationOffset) * guiScaleMatrix;
					memcpy(buffer, glm::value_ptr(objectSpaceToClipSpaceMatrix), numberOfBytes);

					// Value filled
					return true;
				}
				else
				{
					// Call the base implementation
					return RendererRuntime::MaterialBlueprintResourceListener::fillPassValue(referenceValue, buffer, numberOfBytes);
				}
			}


		//[-------------------------------------------------------]
		//[ Private methods                                       ]
		//[-------------------------------------------------------]
		private:
			MaterialBlueprintResourceListener(const MaterialBlueprintResourceListener&) = delete;
			MaterialBlueprintResourceListener& operator=(const MaterialBlueprintResourceListener&) = delete;


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			const RendererRuntime::VrManagerOpenVR*	mVrManagerOpenVR;
			const VrManagerOpenVRListener*			mVrManagerOpenVRListener;


		};


		//[-------------------------------------------------------]
		//[ Global variables                                      ]
		//[-------------------------------------------------------]
		static VrManagerOpenVRListener defaultVrManagerOpenVRListener;
		static MaterialBlueprintResourceListener materialBlueprintResourceListener;


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
VrController::VrController(RendererRuntime::CameraSceneItem& cameraSceneItem) :
	IController(cameraSceneItem),
	mRendererRuntime(cameraSceneItem.getSceneResource().getRendererRuntime())
{
	// Register our listeners
	if (mRendererRuntime.getVrManager().getVrManagerTypeId() == RendererRuntime::VrManagerOpenVR::TYPE_ID)
	{
		RendererRuntime::VrManagerOpenVR& vrManagerOpenVR = static_cast<RendererRuntime::VrManagerOpenVR&>(mRendererRuntime.getVrManager());
		::detail::defaultVrManagerOpenVRListener.setVrManagerOpenVR(vrManagerOpenVR);
		vrManagerOpenVR.setVrManagerOpenVRListener(&::detail::defaultVrManagerOpenVRListener);
		::detail::materialBlueprintResourceListener.setVrManagerOpenVR(vrManagerOpenVR, ::detail::defaultVrManagerOpenVRListener);
		mRendererRuntime.getMaterialBlueprintResourceManager().setMaterialBlueprintResourceListener(&::detail::materialBlueprintResourceListener);
	}
}

VrController::~VrController()
{
	// Unregister our listeners
	if (mRendererRuntime.getVrManager().getVrManagerTypeId() == RendererRuntime::VrManagerOpenVR::TYPE_ID)
	{
		static_cast<RendererRuntime::VrManagerOpenVR&>(mRendererRuntime.getVrManager()).setVrManagerOpenVRListener(nullptr);
		mRendererRuntime.getMaterialBlueprintResourceManager().setMaterialBlueprintResourceListener(nullptr);
	}
}


//[-------------------------------------------------------]
//[ Public virtual IController methods                    ]
//[-------------------------------------------------------]
void VrController::onUpdate(float pastMilliseconds)
{
	// TODO(co)

	// Call the base implementation
	IController::onUpdate(pastMilliseconds);
}
