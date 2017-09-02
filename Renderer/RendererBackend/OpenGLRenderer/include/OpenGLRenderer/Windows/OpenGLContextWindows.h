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
#include "OpenGLRenderer/IOpenGLContext.h"

#include <Renderer/PlatformTypes.h>
#include <Renderer/WindowsHeader.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{
	class RenderPass;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Windows OpenGL context class
	*/
	class OpenGLContextWindows : public IOpenGLContext
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class OpenGLRenderer;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderPass
		*    Render pass
		*  @param[in] nativeWindowHandle
		*    Optional native main window handle, can be a null handle
		*  @param[in] shareContextWindows
		*    Optional share context, can be a null pointer
		*/
		OpenGLContextWindows(const RenderPass& renderPass, handle nativeWindowHandle, const OpenGLContextWindows* shareContextWindows = nullptr);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~OpenGLContextWindows() override;

		/**
		*  @brief
		*    Return the primary device context
		*
		*  @return
		*    The primary device context, null pointer on error
		*/
		inline HDC getDeviceContext() const;

		/**
		*  @brief
		*    Return the primary render context
		*
		*  @return
		*    The primary render context, null pointer on error
		*/
		inline HGLRC getRenderContext() const;


	//[-------------------------------------------------------]
	//[ Public virtual OpenGLRenderer::IOpenGLContext methods ]
	//[-------------------------------------------------------]
	public:
		inline virtual bool isInitialized() const override;
		virtual void makeCurrent() const override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit OpenGLContextWindows(const OpenGLContextWindows& source) = delete;
		OpenGLContextWindows& operator =(const OpenGLContextWindows& source) = delete;

		/**
		*  @brief
		*    Constructor for primary context
		*
		*  @param[in] openGLRuntimeLinking
		*    OpenGL runtime linking instance, if null pointer this isn't a primary context
		*  @param[in] renderPass
		*    Render pass
		*  @param[in] nativeWindowHandle
		*    Optional native main window handle, can be a null handle
		*  @param[in] shareContextWindows
		*    Optional share context, can be a null pointer
		*/
		OpenGLContextWindows(OpenGLRuntimeLinking* openGLRuntimeLinking, const RenderPass& renderPass, handle nativeWindowHandle, const OpenGLContextWindows* shareContextWindows = nullptr);

		/**
		*  @brief
		*    Create a OpenGL context
		*
		*  @param[in] shareContextWindows
		*    Optional share context, can be a null pointer
		*
		*  @return
		*    The created OpenGL context, null pointer on error
		*/
		HGLRC createOpenGLContext(const OpenGLContextWindows* shareContextWindows);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		handle mNativeWindowHandle;		///< OpenGL window, can be a null pointer (HWND)
		handle mDummyWindow;			///< OpenGL dummy window, can be a null pointer (HWND)
		HDC	   mWindowDeviceContext;	///< The device context of the OpenGL dummy window, can be a null pointer
		HGLRC  mWindowRenderContext;	///< The render context of the OpenGL dummy window, can be a null pointer
		bool   mOwnsRenderContext;		///< Does this context owns the OpenGL render context?


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/Windows/OpenGLContextWindows.inl"
