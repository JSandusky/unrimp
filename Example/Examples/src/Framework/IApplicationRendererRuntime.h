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
#include "Framework/IApplicationRenderer.h"

#include <RendererRuntime/Public/RendererRuntime.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class RendererRuntimeInstance;
}
namespace RendererToolkit
{
	class IRendererToolkit;
	#ifdef SHARED_LIBRARIES
		class IProject;
		class RendererToolkitInstance;
	#endif
}


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Renderer runtime application interface
*/
class IApplicationRendererRuntime : public IApplicationRenderer
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
	*  @param[in] exampleBase
	*    Pointer to an example which should be used
	*/
	explicit IApplicationRendererRuntime(const char *rendererName, ExampleBase* exampleBase);

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~IApplicationRendererRuntime();


//[-------------------------------------------------------]
//[ Public virtual IApplicationFrontend methods           ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Return the renderer runtime instance
	*
	*  @return
	*    The renderer runtime instance, can be a null pointer
	*/
	virtual RendererRuntime::IRendererRuntime *getRendererRuntime() const override;

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
	virtual RendererToolkit::IRendererToolkit *getRendererToolkit() override;


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
public:
	virtual void onInitialization() override;
	virtual void onDeinitialization() override;
	virtual void onUpdate() override;


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
protected:
	/**
	*  @brief
	*    Constructor
	*
	*  @param[in] rendererName
	*    Case sensitive ASCII name of the renderer to instance, if null pointer or unknown renderer no renderer will be used.
	*    Example renderer names: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11", "Direct3D12", "Vulkan"
	*/
	explicit IApplicationRendererRuntime(const char *rendererName);


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	RendererRuntime::RendererRuntimeInstance* mRendererRuntimeInstance;	///< Renderer runtime instance, can be a null pointer
	#ifdef SHARED_LIBRARIES
		RendererToolkit::RendererToolkitInstance* mRendererToolkitInstance;	///< Renderer toolkit instance, can be a null pointer
		RendererToolkit::IProject*				  mProject;	// TODO(co) First asset hot-reloading test
	#endif


};
