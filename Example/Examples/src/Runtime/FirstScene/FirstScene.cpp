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
#include "PrecompiledHeader.h"
#include "Runtime/FirstScene/FirstScene.h"
#include "Runtime/FirstScene/FreeCameraController.h"
#ifdef WIN32	// TODO(sw) openvr doesn't support non windows systems yet
	#include "Runtime/FirstScene/VrController.h"
#endif

#include <RendererRuntime/Vr/IVrManager.h>
#include <RendererRuntime/Core/Math/Transform.h>
#include <RendererRuntime/Core/Math/EulerAngles.h>
#include <RendererRuntime/Core/Time/TimeManager.h>
#include <RendererRuntime/DebugGui/DebugGuiManager.h>
#include <RendererRuntime/Resource/Scene/ISceneResource.h>
#include <RendererRuntime/Resource/Scene/SceneResourceManager.h>
#include <RendererRuntime/Resource/Scene/Node/ISceneNode.h>
#include <RendererRuntime/Resource/Scene/Item/LightSceneItem.h>
#include <RendererRuntime/Resource/Scene/Item/CameraSceneItem.h>
#include <RendererRuntime/Resource/Scene/Item/SkeletonMeshSceneItem.h>
#include <RendererRuntime/Resource/Mesh/MeshResourceManager.h>
#include <RendererRuntime/Resource/CompositorNode/Pass/ICompositorInstancePass.h>
#include <RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceInstance.h>
#include <RendererRuntime/Resource/CompositorNode/Pass/DebugGui/CompositorResourcePassDebugGui.h>
#include <RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h>
#include <RendererRuntime/Resource/Material/MaterialResourceManager.h>
#include <RendererRuntime/Resource/Texture/TextureResourceManager.h>

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
		// Referenced assets
		static const RendererRuntime::AssetId CompositorWorkspaceAssetId[3] = { "Example/CompositorWorkspace/Default/Debug", "Example/CompositorWorkspace/Default/Forward", "Example/CompositorWorkspace/Default/Deferred" };
		static const RendererRuntime::AssetId SceneAssetId("Example/Scene/Default/FirstScene");
		static const RendererRuntime::AssetId VrDeviceMaterialAssetId("Example/Material/Default/VrDevice");
		static const RendererRuntime::AssetId ImrodMaterialAssetId("Example/Material/Character/Imrod");
		static const RendererRuntime::AssetId HdrToLdrMaterialAssetId("Example/MaterialBlueprint/Compositor/HdrToLdr");
		static const RendererRuntime::AssetId IdentityColorCorrectionLookupTableTextureAssetId("Unrimp/Texture/DynamicByCode/IdentityColorCorrectionLookupTable3D");
		static const RendererRuntime::AssetId SepiaColorCorrectionLookupTableTextureAssetId("Example/Texture/Compositor/SepiaColorCorrectionLookupTable16x1");
		static const RendererRuntime::AssetId FinalMaterialAssetId("Example/MaterialBlueprint/Compositor/Final");
		static const RendererRuntime::AssetId ScreenSpaceAmbientOcclusionGenerationCompositorMaterialAssetId("Example/MaterialBlueprint/Compositor/ScreenSpaceAmbientOcclusionGeneration");
		static const RendererRuntime::AssetId DeferredAmbientCompositorMaterialAssetId("Example/MaterialBlueprint/Deferred/AmbientCompositor");
		static const RendererRuntime::AssetId AtmosphereCompositorMaterialAssetId("Example/MaterialBlueprint/Compositor/Atmosphere");
		static const RendererRuntime::AssetId DepthOfFieldCompositorMaterialAssetId("Example/MaterialBlueprint/Compositor/DepthOfField");


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstScene::FirstScene() :
	mCompositorWorkspaceInstance(nullptr),
	mSceneResource(nullptr),
	mMaterialResourceId(RendererRuntime::getUninitialized<RendererRuntime::MaterialResourceId>()),
	mCloneMaterialResourceId(RendererRuntime::getUninitialized<RendererRuntime::MaterialResourceId>()),
	mCustomMaterialResourceSet(false),
	mController(nullptr),
	mCameraSceneItem(nullptr),
	mLightSceneItem(nullptr),
	mSkeletonMeshSceneItem(nullptr),
	mSceneNode(nullptr),
	mInstancedCompositor(Compositor::DEFERRED),
	mCurrentCompositor(mInstancedCompositor),
	mCurrentMsaa(Msaa::NONE),
	mResolutionScale(1.0f),
	mNumberOfTopTextureMipmapsToRemove(0),
	mPerformFxaa(false),
	mPerformSepiaColorCorrection(false),
	mRotationSpeed(0.0f),
	mShowSkeleton(false),
	mSunLightColor{1.0f, 1.0f, 1.0f},
	mWetness(0.0f),
	mPerformLighting(true),
	mUseDiffuseMap(true),
	mUseEmissiveMap(true),
	mUseNormalMap(true),
	mUseRoughnessMap(true),
	mDiffuseColor{1.0f, 1.0f, 1.0f}
{
	// Nothing here
}

FirstScene::~FirstScene()
{
	// The resources are released within "onDeinitialization()"
	// Nothing here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstScene::onInitialization()
{
	// Call the base implementation
	ExampleBase::onInitialization();

	// Get and check the renderer runtime instance
	RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
	if (nullptr != rendererRuntime)
	{
		// Create the compositor workspace instance
		createCompositorWorkspace();

		// Create the scene resource
		mSceneResource = rendererRuntime->getSceneResourceManager().loadSceneResourceByAssetId(::detail::SceneAssetId, this);

		// Load the material resource we're going to clone
		mMaterialResourceId = rendererRuntime->getMaterialResourceManager().loadMaterialResourceByAssetId(::detail::ImrodMaterialAssetId, this);

		{ // Startup the VR-manager
			RendererRuntime::IVrManager& vrManager = rendererRuntime->getVrManager();
			if (vrManager.isHmdPresent())
			{
				vrManager.setSceneResource(mSceneResource);
				vrManager.startup(::detail::VrDeviceMaterialAssetId);
			}
		}
	}
}

void FirstScene::onDeinitialization()
{
	// Release the used resources
	delete mCompositorWorkspaceInstance;
	mCompositorWorkspaceInstance = nullptr;
	delete mSceneResource;

	// Destroy controller instance
	if (nullptr != mController)
	{
		delete mController;
		mController = nullptr;
	}

	// Call the base implementation
	ExampleBase::onDeinitialization();
}

void FirstScene::onKeyDown(uint32_t key)
{
	if (nullptr != mController)
	{
		mController->onKeyDown(key);
	}
}

void FirstScene::onKeyUp(uint32_t key)
{
	if (nullptr != mController)
	{
		mController->onKeyUp(key);
	}
}

void FirstScene::onMouseButtonDown(uint32_t button)
{
	if (nullptr != mController)
	{
		mController->onMouseButtonDown(button);
	}
}

void FirstScene::onMouseButtonUp(uint32_t button)
{
	if (nullptr != mController)
	{
		mController->onMouseButtonUp(button);
	}
}

void FirstScene::onMouseMove(int x, int y)
{
	if (nullptr != mController)
	{
		mController->onMouseMove(x, y);
	}
}

void FirstScene::onUpdate()
{
	// Call the base implementation
	ExampleBase::onUpdate();

	RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
	if (nullptr != rendererRuntime)
	{
		{ // Tell the material blueprint resource manager about our global material properties
			RendererRuntime::MaterialProperties& globalMaterialProperties = rendererRuntime->getMaterialBlueprintResourceManager().getGlobalMaterialProperties();
			globalMaterialProperties.setPropertyById("AmbientColor", RendererRuntime::MaterialPropertyValue::fromFloat3(0.2f, 0.2f, 0.2f));
			globalMaterialProperties.setPropertyById("SunLightColor", RendererRuntime::MaterialPropertyValue::fromFloat3(mSunLightColor[0] * 2.0f, mSunLightColor[1] * 2.0f, mSunLightColor[2] * 2.0f));
			globalMaterialProperties.setPropertyById("Wetness", RendererRuntime::MaterialPropertyValue::fromFloat(mWetness));
		}

		// Update the scene node rotation
		if (nullptr != mSceneNode && mRotationSpeed > 0.0f)
		{
			glm::vec3 eulerAngles = RendererRuntime::EulerAngles::matrixToEuler(glm::mat3_cast(mSceneNode->getTransform().rotation));
			eulerAngles.x += rendererRuntime->getTimeManager().getPastSecondsSinceLastFrame() * mRotationSpeed;
			mSceneNode->setRotation(RendererRuntime::EulerAngles::eulerToQuaternion(eulerAngles));
		}

		// Update controller
		if (nullptr != mController)
		{
			mController->onUpdate(rendererRuntime->getTimeManager().getPastSecondsSinceLastFrame());
		}
	}

	// TODO(co) We need to get informed when the mesh scene item received the mesh resource loading finished signal
	trySetCustomMaterialResource();
}

void FirstScene::onDraw()
{
	Renderer::IRenderer* renderer = getRenderer();
	if (nullptr != renderer)
	{
		Renderer::IRenderTarget* mainRenderTarget = getMainRenderTarget();
		if (nullptr != mainRenderTarget)
		{
			// Execute the compositor workspace instance
			if (nullptr != mCompositorWorkspaceInstance)
			{
				createDebugGui(*mainRenderTarget);

				// Decide whether or not the VR-manager is used for rendering
				RendererRuntime::IVrManager& vrManager = mCompositorWorkspaceInstance->getRendererRuntime().getVrManager();
				if (vrManager.isRunning())
				{
					// Update the VR-manager just before rendering
					vrManager.updateHmdMatrixPose(mCameraSceneItem);

					// Execute the compositor workspace instance
					vrManager.executeCompositorWorkspaceInstance(*mCompositorWorkspaceInstance, *mainRenderTarget, mCameraSceneItem, mLightSceneItem);
				}
				else
				{
					// Execute the compositor workspace instance
					mCompositorWorkspaceInstance->execute(*mainRenderTarget, mCameraSceneItem, mLightSceneItem);
				}
			}
		}
	}
}

bool FirstScene::doesCompleteOwnDrawing() const
{
	// This example wants complete control of the drawing
	return true;
}


//[-------------------------------------------------------]
//[ Protected virtual RendererRuntime::IResourceListener methods ]
//[-------------------------------------------------------]
void FirstScene::onLoadingStateChange(const RendererRuntime::IResource& resource)
{
	const RendererRuntime::IResource::LoadingState loadingState = resource.getLoadingState();
	if (&resource == mSceneResource)
	{
		if (RendererRuntime::IResource::LoadingState::LOADED == loadingState)
		{
			// Sanity checks
			assert(nullptr == mSceneNode);
			assert(nullptr == mCameraSceneItem);
			assert(nullptr == mLightSceneItem);
			assert(nullptr == mSkeletonMeshSceneItem);

			// Loop through all scene nodes and grab the first found camera, directional light and mesh
			for (RendererRuntime::ISceneNode* sceneNode : mSceneResource->getSceneNodes())
			{
				// Loop through all scene items attached to the current scene node
				for (RendererRuntime::ISceneItem* sceneItem : sceneNode->getAttachedSceneItems())
				{
					if (sceneItem->getSceneItemTypeId() == RendererRuntime::MeshSceneItem::TYPE_ID)
					{
						// Grab the first found mesh scene item scene node
						if (nullptr == mSceneNode)
						{
							mSceneNode = sceneNode;
							trySetCustomMaterialResource();
						}
					}
					else if (sceneItem->getSceneItemTypeId() == RendererRuntime::CameraSceneItem::TYPE_ID)
					{
						// Grab the first found camera scene item
						if (nullptr == mCameraSceneItem)
						{
							mCameraSceneItem = static_cast<RendererRuntime::CameraSceneItem*>(sceneItem);
						}
					}
					else if (sceneItem->getSceneItemTypeId() == RendererRuntime::LightSceneItem::TYPE_ID)
					{
						// Grab the first found directional light scene item
						if (nullptr == mLightSceneItem)
						{
							RendererRuntime::LightSceneItem* lightSceneItem = static_cast<RendererRuntime::LightSceneItem*>(sceneItem);
							if (lightSceneItem->getLightType() == RendererRuntime::LightSceneItem::LightType::DIRECTIONAL)
							{
								mLightSceneItem = lightSceneItem;
							}
						}
					}
					else if (sceneItem->getSceneItemTypeId() == RendererRuntime::SkeletonMeshSceneItem::TYPE_ID)
					{
						// Grab the first found skeleton mesh scene item
						if (nullptr == mSkeletonMeshSceneItem)
						{
							mSkeletonMeshSceneItem = static_cast<RendererRuntime::SkeletonMeshSceneItem*>(sceneItem);
						}
					}
				}
			}

			if (nullptr != mCameraSceneItem && nullptr != mCameraSceneItem->getParentSceneNode())
			{
				#ifdef WIN32	// TODO(sw) openvr doesn't support non windows systems yet
					if (mCompositorWorkspaceInstance->getRendererRuntime().getVrManager().isRunning())
					{
						mController = new VrController(*mCameraSceneItem);
					}
					else
				#endif
				{
					mController = new FreeCameraController(*mCameraSceneItem);

					// Set initial camera position if virtual reality is disabled
					RendererRuntime::ISceneNode* sceneNode = mCameraSceneItem->getParentSceneNode();
					sceneNode->setPosition(glm::vec3(-3.12873816f, 0.6473912f, 2.20889306f));
					sceneNode->setRotation(glm::quat(0.412612021f, -0.0201802868f, 0.909596086f, 0.0444870926f));
				}
			}
		}
		else
		{
			mCameraSceneItem = nullptr;
			mLightSceneItem = nullptr;
			mSkeletonMeshSceneItem = nullptr;
			if (nullptr != mController)
			{
				delete mController;
				mController = nullptr;
			}
			mSceneNode = nullptr;
		}
	}
	else if (RendererRuntime::IResource::LoadingState::LOADED == loadingState && resource.getAssetId() == ::detail::ImrodMaterialAssetId)
	{
		// Create our material resource clone
		RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
		if (nullptr != rendererRuntime)
		{
			mCloneMaterialResourceId = rendererRuntime->getMaterialResourceManager().createMaterialResourceByCloning(resource.getId());
			trySetCustomMaterialResource();
		}
	}
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
void FirstScene::createCompositorWorkspace()
{
	RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
	if (nullptr != rendererRuntime)
	{
		// Create the compositor workspace instance
		if (nullptr != mCompositorWorkspaceInstance)
		{
			delete mCompositorWorkspaceInstance;
		}
		mCompositorWorkspaceInstance = new RendererRuntime::CompositorWorkspaceInstance(*rendererRuntime, ::detail::CompositorWorkspaceAssetId[mInstancedCompositor]);
	}
}

void FirstScene::createDebugGui(Renderer::IRenderTarget& mainRenderTarget)
{
	if (nullptr != mCompositorWorkspaceInstance && nullptr != mSceneResource)
	{
		// Get the render target the debug GUI is rendered into, use the provided main render target as fallback
		const RendererRuntime::ICompositorInstancePass* compositorInstancePass = mCompositorWorkspaceInstance->getFirstCompositorInstancePassByCompositorPassTypeId(RendererRuntime::CompositorResourcePassDebugGui::TYPE_ID);
		if (nullptr != compositorInstancePass)
		{
			// Setup GUI
			const RendererRuntime::IRendererRuntime& rendererRuntime = mSceneResource->getRendererRuntime();
			rendererRuntime.getDebugGuiManager().newFrame(nullptr != compositorInstancePass->getRenderTarget() ? *compositorInstancePass->getRenderTarget() : mainRenderTarget);
			ImGui::Begin("Options");
				// Compositing
				{
					const char* items[] = { "Debug", "Forward", "Deferred" };
					ImGui::Combo("Compositor", &mCurrentCompositor, items, static_cast<int>(glm::countof(items)));
				}
				{
					const Renderer::Capabilities& capabilities = rendererRuntime.getRenderer().getCapabilities();
					if (capabilities.maximumNumberOfMultisamples > 1)
					{
						const char* items[] = { "None", "2x", "4x", "8x" };
						ImGui::Combo("MSAA", &mCurrentMsaa, items, static_cast<int>(glm::countof(items)));
					}
				}
				ImGui::SliderFloat("Resolution Scale", &mResolutionScale, 0.05f, 4.0f, "%.3f");
				ImGui::SliderInt("Mipmaps to Remove", &mNumberOfTopTextureMipmapsToRemove, 0, 8);
				ImGui::Checkbox("Perform FXAA", &mPerformFxaa);
				ImGui::Checkbox("Perform Sepia Color Correction", &mPerformSepiaColorCorrection);

				// Scene
				ImGui::Separator();
				ImGui::SliderFloat("Rotation Speed", &mRotationSpeed, 0.0f, 2.0f, "%.3f");
				ImGui::Checkbox("Show Skeleton", &mShowSkeleton);

				// Global material properties
				ImGui::Separator();
				ImGui::ColorEdit3("Sun Light Color", mSunLightColor);
				ImGui::SliderFloat("Wetness", &mWetness, 0.0f, 2.0f, "%.3f");

				// Material properties
				ImGui::Separator();
				ImGui::Checkbox("Perform Lighting", &mPerformLighting);
				ImGui::Checkbox("Use Diffuse Map", &mUseDiffuseMap);
				ImGui::Checkbox("Use Emissive Map", &mUseEmissiveMap);
				ImGui::Checkbox("Use Normal Map", &mUseNormalMap);
				ImGui::Checkbox("Use Roughness Map", &mUseRoughnessMap);
				ImGui::ColorEdit3("Diffuse Color", mDiffuseColor);

				// Scene visualizations
				if (nullptr != mCameraSceneItem)
				{
					// Draw skeleton
					if (mShowSkeleton && nullptr != mSkeletonMeshSceneItem && nullptr != mSkeletonMeshSceneItem->getParentSceneNode())
					{
						RendererRuntime::DebugGuiHelper::drawSkeleton(*mCameraSceneItem, *mSkeletonMeshSceneItem);
					}

					// Scene node transform using gizmo
					if (nullptr != mSceneNode)
					{
						ImGui::Separator();
						RendererRuntime::Transform transform = mSceneNode->getTransform();
						RendererRuntime::DebugGuiHelper::drawGizmo(*mCameraSceneItem, mGizmoSettings, transform);
						mSceneNode->setTransform(transform);
					}
				}
			ImGui::End();

			// Recreate the compositor workspace instance, if required
			if (mInstancedCompositor != mCurrentCompositor)
			{
				mInstancedCompositor = static_cast<Compositor>(mCurrentCompositor);
				createCompositorWorkspace();
			}

			// Update texture resource manager
			rendererRuntime.getTextureResourceManager().setNumberOfTopMipmapsToRemove(static_cast<uint8_t>(mNumberOfTopTextureMipmapsToRemove));

			// Update compositor workspace
			{ // MSAA
				uint8_t numberOfMultisamples = 1;
				switch (mCurrentMsaa)
				{
					default:
					case Msaa::NONE:
						numberOfMultisamples = 1;
						break;

					case Msaa::TWO:
						numberOfMultisamples = 2;
						break;

					case Msaa::FOUR:
						numberOfMultisamples = 4;
						break;

					case Msaa::EIGHT:
						numberOfMultisamples = 8;
						break;
				}
				mCompositorWorkspaceInstance->setNumberOfMultisamples(numberOfMultisamples);

				// Tell compositor materials about the number of multisamples
				if (1 == numberOfMultisamples)
				{
					numberOfMultisamples = 0;
				}
				RendererRuntime::MaterialResource* materialResource = rendererRuntime.getMaterialResourceManager().getMaterialResourceByAssetId(::detail::ScreenSpaceAmbientOcclusionGenerationCompositorMaterialAssetId);
				if (nullptr != materialResource)
				{
					materialResource->setPropertyById("NumberOfMultisamples", RendererRuntime::MaterialPropertyValue::fromInteger(numberOfMultisamples));
				}
				materialResource = rendererRuntime.getMaterialResourceManager().getMaterialResourceByAssetId(::detail::DeferredAmbientCompositorMaterialAssetId);
				if (nullptr != materialResource)
				{
					materialResource->setPropertyById("NumberOfMultisamples", RendererRuntime::MaterialPropertyValue::fromInteger(numberOfMultisamples));
				}
				materialResource = rendererRuntime.getMaterialResourceManager().getMaterialResourceByAssetId(::detail::AtmosphereCompositorMaterialAssetId);
				if (nullptr != materialResource)
				{
					materialResource->setPropertyById("NumberOfMultisamples", RendererRuntime::MaterialPropertyValue::fromInteger(numberOfMultisamples));
				}
				materialResource = rendererRuntime.getMaterialResourceManager().getMaterialResourceByAssetId(::detail::DepthOfFieldCompositorMaterialAssetId);
				if (nullptr != materialResource)
				{
					materialResource->setPropertyById("NumberOfMultisamples", RendererRuntime::MaterialPropertyValue::fromInteger(numberOfMultisamples));
				}
			}
			mCompositorWorkspaceInstance->setResolutionScale(mResolutionScale);

			{ // Update the material resource instance
				const RendererRuntime::MaterialResourceManager& materialResourceManager = rendererRuntime.getMaterialResourceManager();
				const RendererRuntime::MaterialResources& materialResources = materialResourceManager.getMaterialResources();

				// HDR to LDR compositor material
				RendererRuntime::MaterialResource* materialResource = materialResourceManager.getMaterialResourceByAssetId(::detail::HdrToLdrMaterialAssetId);
				if (nullptr != materialResource)
				{
					materialResource->setPropertyById("ColorCorrectionLookupTableMap", RendererRuntime::MaterialPropertyValue::fromTextureAssetId(mPerformSepiaColorCorrection ? ::detail::SepiaColorCorrectionLookupTableTextureAssetId : ::detail::IdentityColorCorrectionLookupTableTextureAssetId));
				}

				// Final compositor material
				materialResource = materialResourceManager.getMaterialResourceByAssetId(::detail::FinalMaterialAssetId);
				if (nullptr != materialResource)
				{
					materialResource->setPropertyById("Fxaa", RendererRuntime::MaterialPropertyValue::fromBoolean(mPerformFxaa));
				}

				// Imrod material
				materialResource = materialResources.tryGetElementById(mMaterialResourceId);
				if (nullptr != materialResource)
				{
					materialResource->setPropertyById("Lighting", RendererRuntime::MaterialPropertyValue::fromBoolean(mPerformLighting));
				}

				// Imrod material clone
				materialResource = materialResources.tryGetElementById(mCloneMaterialResourceId);
				if (nullptr != materialResource)
				{
					materialResource->setPropertyById("UseDiffuseMap", RendererRuntime::MaterialPropertyValue::fromBoolean(mUseDiffuseMap));
					materialResource->setPropertyById("UseEmissiveMap", RendererRuntime::MaterialPropertyValue::fromBoolean(mUseEmissiveMap));
					materialResource->setPropertyById("UseNormalMap", RendererRuntime::MaterialPropertyValue::fromBoolean(mUseNormalMap));
					materialResource->setPropertyById("UseRoughnessMap", RendererRuntime::MaterialPropertyValue::fromBoolean(mUseRoughnessMap));
					materialResource->setPropertyById("DiffuseColor", RendererRuntime::MaterialPropertyValue::fromFloat3(mDiffuseColor));
				}
			}
		}
	}
}

void FirstScene::trySetCustomMaterialResource()
{
	if (!mCustomMaterialResourceSet && nullptr != mSceneNode && RendererRuntime::isInitialized(mCloneMaterialResourceId))
	{
		const RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
		if (nullptr != rendererRuntime)
		{
			for (RendererRuntime::ISceneItem* sceneItem : mSceneNode->getAttachedSceneItems())
			{
				if (sceneItem->getSceneItemTypeId() == RendererRuntime::MeshSceneItem::TYPE_ID)
				{
					// Tell the mesh scene item about our custom material resource
					RendererRuntime::MeshSceneItem* meshSceneItem = static_cast<RendererRuntime::MeshSceneItem*>(sceneItem);
					if (RendererRuntime::IResource::LoadingState::LOADED == rendererRuntime->getMeshResourceManager().getResourceByResourceId(meshSceneItem->getMeshResourceId()).getLoadingState())
					{
						meshSceneItem->setMaterialResourceIdOfAllSubMeshes(mCloneMaterialResourceId);
						mCustomMaterialResourceSet = true;
					}
				}
			}
		}
	}
}
