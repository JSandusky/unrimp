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
#include "Framework/IApplicationFrontend.h"

// TODO(sw) To make the example agnostic from unrimp internal headers is should reference the header in <unrimp source dir>/include
#include <Renderer/Public/Renderer.h>
#ifndef RENDERER_NO_RUNTIME
	#include <RendererRuntime/Public/RendererRuntime.h>
#endif


//[-------------------------------------------------------]
//[ Definitions                                           ]
//[-------------------------------------------------------]
#ifdef RENDERER_ONLY_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D10
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_OPENGL
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D10
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_OPENGLES2
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D10
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_DIRECT3D9
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D10
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_DIRECT3D10
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_DIRECT3D11
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D10
#endif


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class RendererInstance;
}


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Example base class
*/
class ExampleBase
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Destructor
	*/
	virtual ~ExampleBase();

	/**
	*  @brief
	*    Initializes the example. Does nothing when already initialized
	*/
	void initialize();

	/**
	*  @brief
	*    Deinitialize the example. Does nothing when already deinitialized
	*/
	void deinitialize();

	/**
	*  @brief
	*    Let the example draw one frame
	*/
	void draw();

	/**
	*  @brief
	*   Set the application frontend to be used by the example
	*/
	void setApplicationFrontend(IApplicationFrontend* applicationFrontend);

	/**
	*  @brief
	*    Return the renderer instance
	*
	*  @return
	*    The renderer instance, can be a null pointer, do not release the returned instance unless you added an own reference to it
	*/
	inline Renderer::IRenderer *getRenderer() const;

	/**
	*  @brief
	*    Return the main renderer target
	*
	*  @return
	*    The main renderer target instance, can be a null pointer, do not release the returned instance unless you added an own reference to it
	*/
	inline Renderer::IRenderTarget *getMainRenderTarget() const;

	/**
	*  @brief
	*    Return the renderer runtime instance
	*
	*  @return
	*    The renderer runtime instance, can be a null pointer
	*/
	inline RendererRuntime::IRendererRuntime *getRendererRuntime() const;

	/**
	*  @brief
	*    Return the renderer toolkit instance
	*
	*  @return
	*    The renderer toolkit instance, can be a null pointer
	*
	*  @remarks
	*    During runtime, the renderer toolkit can optionally be used to enable asset hot-reloading. Meaning,
	*    as soon as an source asset gets changed, the asset is recompiled in a background thread and the compiled
	*    runtime-ready asset is reloaded. One can see the change in realtime without the need to restart the application.
	*
	*    This feature links during runtime the renderer toolkit as soon as this method is accessed the first time. If
	*    the renderer toolkit shared library is not there, this method will return a null pointer. This is a developer-feature
	*    and as such, it's not available in static builds which are meant for the end-user who e.g. just want to "play the game".
	*/
	inline RendererToolkit::IRendererToolkit *getRendererToolkit();


//[-------------------------------------------------------]
//[ Public virtual ExampleBase methods                    ]
//[-------------------------------------------------------]
public:
	virtual void onInitialization();
	virtual void onDeinitialization();
	virtual void onKeyDown(uint32_t key);
	virtual void onKeyUp(uint32_t key);
	virtual void onMouseButtonDown(uint32_t button);
	virtual void onMouseButtonUp(uint32_t button);
	virtual void onMouseMove(int x, int y);
	virtual void onUpdate();
	virtual void onDraw();

	/**
	*  @brief
	*    Return if the examples does the drawing completely on its own.
	*    Thus no draw handling in frontend (aka draw request handling in IApplicationRenderer)
	*
	*  @return
	*    True if the example does its complete draw handling, otherwise false
	*/
	inline virtual bool doesCompleteOwnDrawing() const;


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
protected:
	/**
	*  @brief
	*    Constructor
	*/
	explicit ExampleBase();


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	bool mInitialized;
	IApplicationFrontend*		mApplicationFrontend;	///< Renderer instance, can be a null pointer, do not destroy the instance


};


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "ExampleBase.inl"
