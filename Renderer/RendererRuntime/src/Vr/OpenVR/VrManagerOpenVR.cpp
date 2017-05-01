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
#include "RendererRuntime/Vr/OpenVR/VrManagerOpenVR.h"
#include "RendererRuntime/Vr/OpenVR/OpenVRRuntimeLinking.h"
#include "RendererRuntime/Vr/OpenVR/IVrManagerOpenVRListener.h"
#include "RendererRuntime/Vr/OpenVR/Loader/OpenVRMeshResourceLoader.h"
#include "RendererRuntime/Resource/Scene/SceneNode.h"
#include "RendererRuntime/Resource/Scene/SceneResource.h"
#include "RendererRuntime/Resource/Scene/SceneResourceManager.h"
#include "RendererRuntime/Resource/Scene/Item/MeshSceneItem.h"
#include "RendererRuntime/Resource/Scene/Item/CameraSceneItem.h"
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceInstance.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/IMaterialBlueprintResourceListener.h"
#include "RendererRuntime/Asset/AssetPackage.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/Core/Math/Transform.h"
#include "RendererRuntime/Core/Math/Math.h"
#include "RendererRuntime/IRendererRuntime.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4324)	// warning C4324: '<x>': structure was padded due to alignment specifier
	#include <glm/gtx/matrix_decompose.hpp>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Classes                                               ]
		//[-------------------------------------------------------]
		class VrManagerOpenVRListener : public RendererRuntime::IVrManagerOpenVRListener
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			VrManagerOpenVRListener()
			{
				// Nothing here
			}

			virtual ~VrManagerOpenVRListener()
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Protected methods                                     ]
		//[-------------------------------------------------------]
		protected:
			VrManagerOpenVRListener(const VrManagerOpenVRListener&) = delete;
			VrManagerOpenVRListener& operator=(const VrManagerOpenVRListener&) = delete;


		};


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		static const RendererRuntime::AssetPackageId ASSET_PACKAGE_ID("Unrimp/OpenVR");


		//[-------------------------------------------------------]
		//[ Global variables                                      ]
		//[-------------------------------------------------------]
		static VrManagerOpenVRListener defaultVrManagerOpenVRListener;


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		std::string getTrackedDeviceString(vr::IVRSystem& vrSystem, vr::TrackedDeviceIndex_t trackedDeviceIndex, vr::TrackedDeviceProperty trackedDeviceProperty, vr::TrackedPropertyError* trackedPropertyError = nullptr)
		{
			uint32_t requiredBufferLength = vrSystem.GetStringTrackedDeviceProperty(trackedDeviceIndex, trackedDeviceProperty, nullptr, 0, trackedPropertyError);
			if (0 == requiredBufferLength)
			{
				return "";
			}

			char* temp = new char[requiredBufferLength];
			vrSystem.GetStringTrackedDeviceProperty(trackedDeviceIndex, trackedDeviceProperty, temp, requiredBufferLength, trackedPropertyError);
			std::string result = temp;
			delete [] temp;
			return result;
		}

		std::string getRenderModelName(uint32_t renderModelIndex)
		{
			vr::IVRRenderModels* vrRenderModels = vr::VRRenderModels();

			uint32_t requiredBufferLength = vrRenderModels->GetRenderModelName(renderModelIndex, nullptr, 0);
			if (0 == requiredBufferLength)
			{
				return "";
			}

			char* temp = new char[requiredBufferLength];
			vrRenderModels->GetRenderModelName(renderModelIndex, temp, requiredBufferLength);
			std::string result = temp;
			delete [] temp;
			return result;
		}

		std::string getRenderModelComponentName(const std::string& renderModelName, uint32_t componentIndex)
		{
			vr::IVRRenderModels* vrRenderModels = vr::VRRenderModels();

			uint32_t requiredBufferLength = vrRenderModels->GetComponentName(renderModelName.c_str(), componentIndex, nullptr, 0);
			if (0 == requiredBufferLength)
			{
				return "";
			}

			char* temp = new char[requiredBufferLength];
			vrRenderModels->GetComponentName(renderModelName.c_str(), componentIndex, temp, requiredBufferLength);
			std::string result = temp;
			delete [] temp;
			return result;
		}

		std::string getRenderModelComponentRenderModelName(const std::string& renderModelName, const std::string& componentName)
		{
			vr::IVRRenderModels* vrRenderModels = vr::VRRenderModels();

			uint32_t requiredBufferLength = vrRenderModels->GetComponentRenderModelName(renderModelName.c_str(), componentName.c_str(), nullptr, 0);
			if (0 == requiredBufferLength)
			{
				return "";
			}

			char* temp = new char[requiredBufferLength];
			vrRenderModels->GetComponentRenderModelName(renderModelName.c_str(), componentName.c_str(), temp, requiredBufferLength);
			std::string result = temp;
			delete [] temp;
			return result;
		}

		glm::mat4 convertOpenVrMatrixToGlmMat34(const vr::HmdMatrix34_t& vrHmdMatrix34)
		{
			// Transform the OpenGL style transform matrix into a Direct3D style transform matrix as described at http://cv4mar.blogspot.de/2009/03/transformation-matrices-between-opengl.html
			// -> Direct3D: Left-handed coordinate system
			// -> OpenGL: Right-handed coordinate system
			return glm::mat4(
				 vrHmdMatrix34.m[0][0],  vrHmdMatrix34.m[1][0], -vrHmdMatrix34.m[2][0], 0.0f,
				 vrHmdMatrix34.m[0][1],  vrHmdMatrix34.m[1][1], -vrHmdMatrix34.m[2][1], 0.0f,
				-vrHmdMatrix34.m[0][2], -vrHmdMatrix34.m[1][2],  vrHmdMatrix34.m[2][2], 0.0f,
				 vrHmdMatrix34.m[0][3],  vrHmdMatrix34.m[1][3], -vrHmdMatrix34.m[2][3], 1.0f
				);
		}

		void createMeshSceneItem(RendererRuntime::SceneResource& sceneResource, RendererRuntime::SceneNode& sceneNode, const std::string& renderModelName)
		{
			// Check whether or not we need to generate the runtime mesh asset right now
			RendererRuntime::MeshResourceId meshResourceId = RendererRuntime::getUninitialized<RendererRuntime::MeshResourceId>();
			sceneResource.getRendererRuntime().getMeshResourceManager().loadMeshResourceByAssetId(RendererRuntime::AssetId(renderModelName.c_str()), meshResourceId, nullptr, false, RendererRuntime::OpenVRMeshResourceLoader::TYPE_ID);

			// Create mesh scene item
			RendererRuntime::MeshSceneItem* meshSceneItem = sceneResource.createSceneItem<RendererRuntime::MeshSceneItem>(sceneNode);
			if (nullptr != meshSceneItem)
			{
				meshSceneItem->setMeshResourceId(meshResourceId);
			}
		}

		void setSceneNodesVisible(RendererRuntime::SceneNode* sceneNodes[vr::k_unMaxTrackedDeviceCount], bool visible)
		{
			for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
			{
				RendererRuntime::SceneNode* sceneNode = sceneNodes[i];
				if (nullptr != sceneNode)
				{
					sceneNode->setVisible(visible);
				}
			}
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
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const VrManagerTypeId VrManagerOpenVR::TYPE_ID("VrManagerOpenVR");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void VrManagerOpenVR::setVrManagerOpenVRListener(IVrManagerOpenVRListener* vrManagerOpenVRListener)
	{
		// There must always be a valid VR manager OpenVR listener instance
		mVrManagerOpenVRListener = (nullptr != vrManagerOpenVRListener) ? vrManagerOpenVRListener : &::detail::defaultVrManagerOpenVRListener;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IVrManager methods    ]
	//[-------------------------------------------------------]
	VrManagerTypeId VrManagerOpenVR::getVrManagerTypeId() const
	{
		return TYPE_ID;
	}

	bool VrManagerOpenVR::isHmdPresent() const
	{
		return (mOpenVRRuntimeLinking->isOpenVRAvaiable() && vr::VR_IsRuntimeInstalled() && vr::VR_IsHmdPresent());
	}

	void VrManagerOpenVR::setSceneResourceId(SceneResourceId sceneResourceId)
	{
		// TODO(co) Decent implementation so it's no problem to change the scene resource at any time
		mSceneResourceId = sceneResourceId;
	}

	bool VrManagerOpenVR::startup(AssetId vrDeviceMaterialAssetId)
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

			// Get the OpenVR render models interface
			mVrRenderModels = static_cast<vr::IVRRenderModels*>(vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &vrInitError));
			if (nullptr == mVrRenderModels)
			{
				// De-initialize the OpenVR system
				vr::VR_Shutdown();
				mVrSystem = nullptr;

				// Error!
				RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF("Error: Unable to retrieve the OpenVR render models interface: %s", vr::VR_GetVRInitErrorAsEnglishDescription(vrInitError));
				return false;
			}

			// Try to load the device material resource material
			mVrDeviceMaterialResourceLoaded = false;
			mVrDeviceMaterialResourceId = getUninitialized<MaterialResourceId>();
			if (isInitialized(vrDeviceMaterialAssetId))
			{
				mRendererRuntime.getMaterialResourceManager().loadMaterialResourceByAssetId(vrDeviceMaterialAssetId, mVrDeviceMaterialResourceId, this);
			}

			{ // Create renderer resources
				// Create the texture instance
				uint32_t width = 0;
				uint32_t height = 0;
				mVrSystem->GetRecommendedRenderTargetSize(&width, &height);
				Renderer::ITexture* colorTexture2D = mColorTexture2D = mRendererRuntime.getTextureManager().createTexture2D(width, height, Renderer::TextureFormat::R8G8B8A8, nullptr, Renderer::TextureFlag::RENDER_TARGET);
				RENDERER_SET_RESOURCE_DEBUG_NAME(colorTexture2D, "OpenVR color render target texture")
				Renderer::ITexture* depthStencilTexture2D = mRendererRuntime.getTextureManager().createTexture2D(width, height, Renderer::TextureFormat::D32_FLOAT, nullptr, Renderer::TextureFlag::RENDER_TARGET);
				RENDERER_SET_RESOURCE_DEBUG_NAME(depthStencilTexture2D, "OpenVR depth stencil render target texture")

				// Create the framebuffer object (FBO) instance
				mFramebuffer = renderer.createFramebuffer(1, &colorTexture2D, depthStencilTexture2D);
				RENDERER_SET_RESOURCE_DEBUG_NAME(mFramebuffer, "OpenVR framebuffer")
			}

			{ // Add dynamic OpenVR asset package
			  // -> OpenVR render model names can get awful long due to absolute path information, so, we need to store them inside a separate list and tell the asset just about the render model name index
				AssetPackage& assetPackage = mRendererRuntime.getAssetManager().addAssetPackage(::detail::ASSET_PACKAGE_ID);
				const uint32_t renderModelCount = mVrRenderModels->GetRenderModelCount();
				for (uint32_t renderModelIndex = 0; renderModelIndex < renderModelCount; ++renderModelIndex)
				{
					std::string renderModelName = ::detail::getRenderModelName(renderModelIndex);
					const uint32_t componentCount = mVrRenderModels->GetComponentCount(renderModelName.c_str());
					if (componentCount > 0)
					{
						for (uint32_t componentIndex = 0; componentIndex < componentCount; ++componentIndex)
						{
							const std::string componentName = ::detail::getRenderModelComponentName(renderModelName, componentIndex);
							const std::string componentRenderModelName = ::detail::getRenderModelComponentRenderModelName(renderModelName, componentName);
							if (!componentRenderModelName.empty())
							{
								assetPackage.addAsset(AssetId(componentRenderModelName.c_str()), std::to_string(mRenderModelNames.size()).c_str());
								mRenderModelNames.push_back(componentRenderModelName);
							}
						}
					}
					else
					{
						assetPackage.addAsset(AssetId(renderModelName.c_str()), std::to_string(mRenderModelNames.size()).c_str());
						mRenderModelNames.push_back(renderModelName);
					}
				}
				assert(assetPackage.getSortedAssetVector().size() == mRenderModelNames.size());
			}

			// TODO(co) Optionally mirror the result on the given render target
			vr::VRCompositor()->ShowMirrorWindow();
		}

		// Done
		return true;
	}

	void VrManagerOpenVR::shutdown()
	{
		if (nullptr != mVrSystem)
		{
			// Remove dynamic OpenVR asset package
			mRendererRuntime.getAssetManager().removeAssetPackage(::detail::ASSET_PACKAGE_ID);
			mRenderModelNames.clear();

			// Release renderer resources
			mFramebuffer = nullptr;
			mColorTexture2D = nullptr;

			// De-initialize the OpenVR system
			vr::VR_Shutdown();
			mVrSystem = nullptr;
		}
	}

	void VrManagerOpenVR::updateHmdMatrixPose(CameraSceneItem* cameraSceneItem)
	{
		assert(nullptr != mVrSystem);

		// Process OpenVR events
		if (mVrDeviceMaterialResourceLoaded)
		{
			vr::VREvent_t vrVrEvent;
			while (mVrSystem->PollNextEvent(&vrVrEvent, sizeof(vr::VREvent_t)))
			{
				switch (vrVrEvent.eventType)
				{
					case vr::VREvent_TrackedDeviceActivated:
						setupRenderModelForTrackedDevice(vrVrEvent.trackedDeviceIndex);
						break;

					// Sent to the scene application to request hiding render models temporarily
					case vr::VREvent_HideRenderModels:
						mShowRenderModels = false;
						::detail::setSceneNodesVisible(mSceneNodes, mShowRenderModels);
						break;

					// Sent to the scene application to request restoring render model visibility
					case vr::VREvent_ShowRenderModels:
						mShowRenderModels = true;
						::detail::setSceneNodesVisible(mSceneNodes, mShowRenderModels);
						break;
				}

				// Tell the world
				mVrManagerOpenVRListener->onVrEvent(vrVrEvent);
			}
		}

		// Request poses from OpenVR
		vr::VRCompositor()->WaitGetPoses(mVrTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

		// Everything must be relative to the camera world space position
		glm::vec3 cameraPosition;
		if (nullptr != cameraSceneItem && nullptr != cameraSceneItem->getParentSceneNode())
		{
			cameraPosition = cameraSceneItem->getParentSceneNode()->getGlobalTransform().position;
		}

		// Don't draw controllers if somebody else has input focus
		const bool showControllers = (mShowRenderModels && !mVrSystem->IsInputFocusCapturedByAnotherProcess());

		// Gather all valid poses
		mNumberOfValidDevicePoses = 0;
		for (uint32_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; ++deviceIndex)
		{
			if (mVrTrackedDevicePose[deviceIndex].bPoseIsValid)
			{
				++mNumberOfValidDevicePoses;
				const glm::mat4& devicePoseMatrix = mDevicePoseMatrix[deviceIndex] = ::detail::convertOpenVrMatrixToGlmMat34(mVrTrackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking);
				SceneNode* sceneNode = mSceneNodes[deviceIndex];
				if (nullptr != sceneNode)
				{
					glm::vec3 scale;
					glm::quat rotation;
					glm::vec3 translation;
					glm::vec3 skew;
					glm::vec4 perspective;
					glm::decompose(devicePoseMatrix, scale, rotation, translation, skew, perspective);
					rotation = glm::conjugate(rotation);

					// Everything must be relative to the camera world space position
					translation -= cameraPosition;

					// Tell the scene node about the new position and rotation, scale doesn't change
					sceneNode->setPositionRotation(translation, rotation);

					// Show/hide scene node
					sceneNode->setVisible(showControllers);
				}
			}
		}

		// Update render model components so we can see e.g. controller trigger animations
		for (uint32_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; ++deviceIndex)
		{
			const TrackedDeviceInformation& trackedDeviceInformation = mTrackedDeviceInformation[deviceIndex];
			if (!trackedDeviceInformation.renderModelName.empty() && !trackedDeviceInformation.components.empty())
			{
				vr::VRControllerState_t vrControllerState;
				if (mVrSystem->GetControllerState(deviceIndex, &vrControllerState))
				{
					vr::IVRRenderModels* vrRenderModels = vr::VRRenderModels();
					for (const Component& component : trackedDeviceInformation.components)
					{
						assert(!component.name.empty());
						SceneNode* sceneNode = component.sceneNode;
						assert(nullptr != sceneNode);
						vr::RenderModel_ControllerMode_State_t renderModelControllerModeState;
						renderModelControllerModeState.bScrollWheelVisible = false;
						vr::RenderModel_ComponentState_t renderModelComponentState;
						if (vrRenderModels->GetComponentState(trackedDeviceInformation.renderModelName.c_str(), component.name.c_str(), &vrControllerState, &renderModelControllerModeState, &renderModelComponentState))
						{
							sceneNode->setTransform(Transform(::detail::convertOpenVrMatrixToGlmMat34(renderModelComponentState.mTrackingToComponentRenderModel)));
							sceneNode->setVisible((renderModelComponentState.uProperties & vr::VRComponentProperty_IsVisible) != 0);
						}
					}
				}
			}
		}

		// Backup HMD pose
		if (mVrTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
		{
			mHmdPoseMatrix = mDevicePoseMatrix[vr::k_unTrackedDeviceIndex_Hmd];
		}

		// Update camera scene node transform and hide the HMD scene node in case it's currently used as the camera scene node (we don't want to see the HMD mesh from the inside)
		SceneNode* hmdSceneNode = mSceneNodes[vr::k_unTrackedDeviceIndex_Hmd];
		if (nullptr != hmdSceneNode)
		{
			bool hmdSceneNodeVisible = true;
			if (nullptr != cameraSceneItem)
			{
				SceneNode* sceneNode = cameraSceneItem->getParentSceneNode();
				if (nullptr != sceneNode)
				{
				//	sceneNode->setTransform(hmdSceneNode->getGlobalTransform());	// TODO(co)
					hmdSceneNodeVisible = false;
				}
			}
			hmdSceneNode->setVisible(hmdSceneNodeVisible);
		}
	}

	glm::mat4 VrManagerOpenVR::getHmdViewSpaceToClipSpaceMatrix(VrEye vrEye, float nearZ, float farZ) const
	{
		// TODO(co) OpenVR (commit e1507a27547d22a680153862865d40b90fad8c75 - Jun 10, 2016): "vr::IVRSystem::GetProjectionMatrix()" apparently ignores the last "vr::API_OpenGL"/"vr::API_DirectX"-parameter
		// -> This issue is also described at https://github.com/ValveSoftware/openvr/issues/70 and https://github.com/ValveSoftware/openvr/issues/239 and https://github.com/ValveSoftware/openvr/issues/203
		// -> Transform the OpenGL style projection matrix into a Direct3D style projection matrix as described at http://cv4mar.blogspot.de/2009/03/transformation-matrices-between-opengl.html
		// -> Direct3D: Left-handed coordinate system with clip space depth value range 0..1
		// -> OpenGL: Right-handed coordinate system with clip space depth value range -1..1
		assert(nullptr != mVrSystem);
		const vr::HmdMatrix44_t vrHmdMatrix34 = mVrSystem->GetProjectionMatrix(static_cast<vr::Hmd_Eye>(vrEye), nearZ, farZ, vr::API_DirectX);
		return glm::mat4(
			 vrHmdMatrix34.m[0][0],  vrHmdMatrix34.m[1][0],  vrHmdMatrix34.m[2][0],  vrHmdMatrix34.m[3][0],
			 vrHmdMatrix34.m[0][1],  vrHmdMatrix34.m[1][1],  vrHmdMatrix34.m[2][1],  vrHmdMatrix34.m[3][1],
			-vrHmdMatrix34.m[0][2], -vrHmdMatrix34.m[1][2], -vrHmdMatrix34.m[2][2], -vrHmdMatrix34.m[3][2],
			 vrHmdMatrix34.m[0][3],  vrHmdMatrix34.m[1][3],  vrHmdMatrix34.m[2][3],  vrHmdMatrix34.m[3][3]
			);
	}

	glm::mat4 VrManagerOpenVR::getHmdEyeSpaceToHeadSpaceMatrix(VrEye vrEye) const
	{
		assert(nullptr != mVrSystem);
		return ::detail::convertOpenVrMatrixToGlmMat34(mVrSystem->GetEyeToHeadTransform(static_cast<vr::Hmd_Eye>(vrEye)));
	}


	//[-------------------------------------------------------]
	//[ Private virtual RendererRuntime::IVrManager methods   ]
	//[-------------------------------------------------------]
	void VrManagerOpenVR::executeCompositorWorkspaceInstance(CompositorWorkspaceInstance& compositorWorkspaceInstance, Renderer::IRenderTarget&, CameraSceneItem* cameraSceneItem, const LightSceneItem* lightSceneItem)
	{
		assert(nullptr != mVrSystem);

		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = mRendererRuntime.getMaterialBlueprintResourceManager().getMaterialBlueprintResourceListener();
		for (int8_t eyeIndex = 0; eyeIndex < 2; ++eyeIndex)
		{
			// Execute the compositor workspace instance
			materialBlueprintResourceListener.setCurrentRenderedVrEye(static_cast<IMaterialBlueprintResourceListener::VrEye>(eyeIndex));
			compositorWorkspaceInstance.execute(*mFramebuffer, cameraSceneItem, lightSceneItem);

			// Submit the rendered texture to the OpenVR compositor
			const vr::Texture_t vrTexture = { mColorTexture2D->getInternalResourceHandle(), mVrGraphicsAPIConvention, vr::ColorSpace_Auto };
			vr::VRCompositor()->Submit(static_cast<vr::Hmd_Eye>(eyeIndex), &vrTexture);
		}
		materialBlueprintResourceListener.setCurrentRenderedVrEye(IMaterialBlueprintResourceListener::VrEye::UNKNOWN);

		// Tell the compositor to begin work immediately instead of waiting for the next "vr::IVRCompositor::WaitGetPoses()" call
		vr::VRCompositor()->PostPresentHandoff();
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::IResourceListener methods ]
	//[-------------------------------------------------------]
	void VrManagerOpenVR::onLoadingStateChange(const IResource& resource)
	{
		if (mVrDeviceMaterialResourceId == resource.getId() && resource.getLoadingState() == IResource::LoadingState::LOADED)
		{
			mVrDeviceMaterialResourceLoaded = true;

			// Setup all render models for tracked devices
			for (uint32_t trackedDeviceIndex = vr::k_unTrackedDeviceIndex_Hmd; trackedDeviceIndex < vr::k_unMaxTrackedDeviceCount; ++trackedDeviceIndex)
			{
				if (mVrSystem->IsTrackedDeviceConnected(trackedDeviceIndex))
				{
					setupRenderModelForTrackedDevice(trackedDeviceIndex);
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	VrManagerOpenVR::VrManagerOpenVR(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mVrManagerOpenVRListener(&::detail::defaultVrManagerOpenVRListener),
		mVrDeviceMaterialResourceLoaded(false),
		mVrDeviceMaterialResourceId(getUninitialized<MaterialResourceId>()),
		mSceneResourceId(getUninitialized<SceneResourceId>()),
		mOpenVRRuntimeLinking(new OpenVRRuntimeLinking()),
		mVrGraphicsAPIConvention(vr::API_OpenGL),
		mVrSystem(nullptr),
		mVrRenderModels(nullptr),
		mShowRenderModels(true),
		mNumberOfValidDevicePoses(0)
	{
		memset(mSceneNodes, 0, sizeof(SceneNode*) * vr::k_unMaxTrackedDeviceCount);
	}

	VrManagerOpenVR::~VrManagerOpenVR()
	{
		shutdown();
		delete mOpenVRRuntimeLinking;
	}

	void VrManagerOpenVR::setupRenderModelForTrackedDevice(vr::TrackedDeviceIndex_t trackedDeviceIndex)
	{
		assert(trackedDeviceIndex < vr::k_unMaxTrackedDeviceCount);

		// Create and setup scene node with mesh item, this is what's controlled during runtime
		SceneResource* sceneResource = mRendererRuntime.getSceneResourceManager().tryGetById(mSceneResourceId);
		if (nullptr != sceneResource)
		{
			// Create scene node
			SceneNode* sceneNode = mSceneNodes[trackedDeviceIndex] = sceneResource->createSceneNode(Transform::IDENTITY);
			if (nullptr != sceneNode)
			{
				TrackedDeviceInformation& trackedDeviceInformation = mTrackedDeviceInformation[trackedDeviceIndex];
				trackedDeviceInformation.components.clear();

				// Get the render model name
				trackedDeviceInformation.renderModelName = ::detail::getTrackedDeviceString(*mVrSystem, trackedDeviceIndex, vr::Prop_RenderModelName_String);
				const std::string& renderModelName = trackedDeviceInformation.renderModelName;

				// In case the render model has components, don't use the render model directly, use its components instead so we can animate e.g. the controller trigger
				vr::IVRRenderModels* vrRenderModels = vr::VRRenderModels();
				const uint32_t componentCount = vrRenderModels->GetComponentCount(renderModelName.c_str());
				vr::VRControllerState_t vrControllerState;
				if (componentCount > 0 && mVrSystem->GetControllerState(trackedDeviceIndex, &vrControllerState))
				{
					for (uint32_t componentIndex = 0; componentIndex < componentCount; ++componentIndex)
					{
						const std::string componentName = ::detail::getRenderModelComponentName(renderModelName, componentIndex);
						const std::string componentRenderModelName = ::detail::getRenderModelComponentRenderModelName(renderModelName, componentName);
						if (!componentRenderModelName.empty())
						{
							// Get component state
							vr::RenderModel_ControllerMode_State_t renderModelControllerModeState;
							renderModelControllerModeState.bScrollWheelVisible = false;
							vr::RenderModel_ComponentState_t renderModelComponentState;
							if (vrRenderModels->GetComponentState(renderModelName.c_str(), componentName.c_str(), &vrControllerState, &renderModelControllerModeState, &renderModelComponentState))
							{
								// Create the scene node of the component
								SceneNode* componentSceneNode = sceneResource->createSceneNode(Transform(::detail::convertOpenVrMatrixToGlmMat34(renderModelComponentState.mTrackingToComponentRenderModel)));
								if (nullptr != componentSceneNode)
								{
									sceneNode->attachSceneNode(*componentSceneNode);
									::detail::createMeshSceneItem(*sceneResource, *componentSceneNode, componentRenderModelName);
									componentSceneNode->setVisible((renderModelComponentState.uProperties & vr::VRComponentProperty_IsVisible) != 0);
									trackedDeviceInformation.components.emplace_back(componentName, componentSceneNode);
								}
							}
						}
					}
				}
				else
				{
					::detail::createMeshSceneItem(*sceneResource, *sceneNode, renderModelName);
				}

				// Tell the world
				mVrManagerOpenVRListener->onSceneNodeCreated(trackedDeviceIndex, *sceneResource, *sceneNode);
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
