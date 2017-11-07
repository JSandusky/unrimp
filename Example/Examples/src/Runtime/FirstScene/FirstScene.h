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
#include "Framework/ExampleBase.h"

#include <RendererRuntime/Core/Math/Transform.h>
#include <RendererRuntime/DebugGui/DebugGuiHelper.h>
#include <RendererRuntime/Resource/IResourceListener.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
class IController;
namespace RendererRuntime
{
	class ImGuiLog;
	class SceneNode;
	class CameraSceneItem;
	class SunlightSceneItem;
	class SkeletonMeshSceneItem;
	class CompositorWorkspaceInstance;
}


//[-------------------------------------------------------]
//[ Global definitions                                    ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	typedef uint32_t SceneResourceId;		///< POD scene resource identifier
	typedef uint32_t MaterialResourceId;	///< POD material resource identifier
}


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    First scene example
*
*  @remarks
*    Demonstrates:
*    - Compositor
*    - Scene
*    - Virtual reality (VR)
*/
class FirstScene : public ExampleBase, public RendererRuntime::IResourceListener
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Constructor
	*/
	FirstScene();

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~FirstScene() override;


//[-------------------------------------------------------]
//[ Public virtual ExampleBase methods                    ]
//[-------------------------------------------------------]
public:
	virtual void onInitialization() override;
	virtual void onDeinitialization() override;
	virtual void onKeyDown(uint32_t key) override;
	virtual void onKeyUp(uint32_t key) override;
	virtual void onMouseButtonDown(uint32_t button) override;
	virtual void onMouseButtonUp(uint32_t button) override;
	virtual void onMouseWheel(float delta) override;
	virtual void onMouseMove(int x, int y) override;
	virtual void onUpdate() override;
	virtual void onDraw() override;
	virtual bool doesCompleteOwnDrawing() const override;


//[-------------------------------------------------------]
//[ Protected virtual RendererRuntime::IResourceListener methods ]
//[-------------------------------------------------------]
protected:
	virtual void onLoadingStateChange(const RendererRuntime::IResource& resource) override;


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
private:
	explicit FirstScene(const FirstScene&) = delete;
	FirstScene& operator=(const FirstScene&) = delete;
	void createCompositorWorkspace();
	void createDebugGui(Renderer::IRenderTarget& mainRenderTarget);
	void trySetCustomMaterialResource();
	bool isControllerValid() const;


//[-------------------------------------------------------]
//[ Private definitions                                   ]
//[-------------------------------------------------------]
private:
	enum Compositor
	{
		DEBUG,
		FORWARD,
		DEFERRED,
		VR
	};
	enum Msaa
	{
		NONE,
		TWO,
		FOUR,
		EIGHT
	};
	enum TextureFiltering
	{
		POINT,
		BILINEAR,
		TRILINEAR,
		ANISOTROPIC_2,
		ANISOTROPIC_4,
		ANISOTROPIC_8,
		ANISOTROPIC_16
	};


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	RendererRuntime::ImGuiLog*					  mImGuiLog;
	RendererRuntime::CompositorWorkspaceInstance* mCompositorWorkspaceInstance;
	RendererRuntime::SceneResourceId			  mSceneResourceId;
	RendererRuntime::MaterialResourceId			  mMaterialResourceId;
	RendererRuntime::MaterialResourceId			  mCloneMaterialResourceId;
	bool										  mCustomMaterialResourceSet;
	IController*								  mController;
	// Crazy raw-pointers to point-of-interest scene stuff
	RendererRuntime::CameraSceneItem*		mCameraSceneItem;
	RendererRuntime::SunlightSceneItem*		mSunlightSceneItem;
	RendererRuntime::SkeletonMeshSceneItem*	mSkeletonMeshSceneItem;
	RendererRuntime::SceneNode*				mSceneNode;
	// States for runtime-editing
	RendererRuntime::DebugGuiHelper::GizmoSettings mGizmoSettings;
	// Global
	Compositor mInstancedCompositor;
	int		   mCurrentCompositor;
	bool	   mHighQualityLighting;
	int		   mCurrentMsaa;
	float	   mResolutionScale;
	int		   mCurrentTextureFiltering;
	int		   mNumberOfTopTextureMipmapsToRemove;
	// Environment
	float	   mWetness;
	// Post processing
	bool	   mPerformFxaa;
	bool	   mPerformChromaticAberration;
	bool	   mPerformOldCrtEffect;
	bool	   mPerformFilmGrain;
	bool	   mPerformSepiaColorCorrection;
	bool	   mPerformVignette;
	float	   mDepthOfFieldBlurrinessCutoff;
	// Selected material properties
	bool	   mUseEmissiveMap;
	float	   mAlbedoColor[3];
	// Selected scene item
	float	   mRotationSpeed;
	bool	   mShowSkeleton;
	// Scene hot-reloading memory
	bool					   mHasCameraTransformBackup;
	RendererRuntime::Transform mCameraTransformBackup;


};
