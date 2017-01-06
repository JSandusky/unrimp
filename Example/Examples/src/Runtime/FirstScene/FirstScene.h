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
#include "Framework/Stopwatch.h"
#include "Framework/ExampleBase.h"

#include <RendererRuntime/Resource/IResourceListener.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
class FreeCameraController;
namespace RendererRuntime
{
	class ISceneNode;
	class ISceneResource;
	class LightSceneItem;
	class CameraSceneItem;
	class CompositorWorkspaceInstance;
}


//[-------------------------------------------------------]
//[ Global definitions                                    ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
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
	*
	*  @param[in] rendererName
	*    Case sensitive ASCII name of the renderer to instance, if null pointer or unknown renderer no renderer will be used.
	*    Example renderer names: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11", "Direct3D12", "Vulkan"
	*/
	explicit FirstScene(const char *rendererName);

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~FirstScene();


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
	virtual void onMouseMove(int x, int y) override;
	virtual void onUpdate() override;
	virtual void onDraw() override;

	// This example wants complete control of the drawing
	inline virtual bool doesCompleteOwnDrawing() const override { return true; }


//[-------------------------------------------------------]
//[ Protected virtual RendererRuntime::IResourceListener methods ]
//[-------------------------------------------------------]
protected:
	virtual void onLoadingStateChange(const RendererRuntime::IResource& resource) override;


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
private:
	FirstScene(const FirstScene&) = delete;
	FirstScene& operator=(const FirstScene&) = delete;
	void createCompositorWorkspace();
	void createDebugGui(Renderer::IRenderTarget& mainRenderTarget);
	void trySetCustomMaterialResource();


//[-------------------------------------------------------]
//[ Private definitions                                   ]
//[-------------------------------------------------------]
private:
	enum Compositor
	{
		DEBUG,
		FORWARD,
		DEFERRED,
		NumberOfCompositors
	};
	enum Msaa
	{
		NONE,
		TWO,
		FOUR,
		EIGHT
	};


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	RendererRuntime::CompositorWorkspaceInstance* mCompositorWorkspaceInstance;
	RendererRuntime::ISceneResource*			  mSceneResource;
	RendererRuntime::MaterialResourceId			  mMaterialResourceId;
	RendererRuntime::MaterialResourceId			  mCloneMaterialResourceId;
	bool										  mCustomMaterialResourceSet;
	FreeCameraController*						  mFreeCameraController;
	// Crazy raw-pointers to point-of-interest scene stuff
	RendererRuntime::CameraSceneItem* mCameraSceneItem;
	RendererRuntime::LightSceneItem*  mLightSceneItem;
	RendererRuntime::ISceneNode*	  mSceneNode;
	// Timing
	Stopwatch mStopwatch;	///< Stopwatch instance
	float	  mGlobalTimer;	///< Global timer
	// States for runtime-fun
	Compositor mInstancedCompositor;
	int		   mCurrentCompositor;
	int		   mCurrentMsaa;
	float	   mResolutionScale;
	bool	   mPerformFxaa;
	float	   mRotationSpeed;
	float	   mSunLightColor[3];
	float	   mWetness;
	bool	   mPerformLighting;
	bool	   mUseDiffuseMap;
	bool	   mUseEmissiveMap;
	bool	   mUseNormalMap;
	bool	   mUseSpecularMap;
	float	   mDiffuseColor[3];


};
