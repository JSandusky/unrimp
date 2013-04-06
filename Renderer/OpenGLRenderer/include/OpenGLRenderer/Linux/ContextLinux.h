/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
#ifndef __OPENGLRENDERER_CONTEXTLINUX_H__
#define __OPENGLRENDERER_CONTEXTLINUX_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/IContext.h"

#include <Renderer/PlatformTypes.h>
#include <Renderer/LinuxHeader.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{
	class OpenGLRuntimeLinking;
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
	*    Linux OpenGL context class
	*/
	class ContextLinux : public IContext
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] nativeWindowHandle
		*    Optional native main window handle, can be a null handle
		*/
		explicit ContextLinux(handle nativeWindowHandle);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ContextLinux();

		/**
		*  @brief
		*    Return the primary device context
		*
		*  @return
		*    The primary device context, null pointer on error
		*/
		inline Display *getDisplay() const;

		/**
		*  @brief
		*    Return the primary render context
		*
		*  @return
		*    The primary render context, null pointer on error
		*/
		inline GLXContext getRenderContext() const;


	//[-------------------------------------------------------]
	//[ Public virtual OpenGLRenderer::IContext methods       ]
	//[-------------------------------------------------------]
	public:
		inline virtual bool isInitialized() const override;


	//[-------------------------------------------------------]
	//[ Private static methods                                ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Debug message callback function called by the "GL_ARB_debug_output"-extension
		*
		*  @param[in] source
		*    Source of the debug message
		*  @param[in] type
		*    Type of the debug message
		*  @param[in] id
		*    ID of the debug message
		*  @param[in] severity
		*    Severity of the debug message
		*  @param[in] length
		*    Length of the debug message
		*  @param[in] message
		*    The debug message
		*  @param[in] userParam
		*    Additional user parameter of the debug message
		*/
		static void debugMessageCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char *message, void *userParam);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Create a OpenGL context
		*
		*  @return
		*    The created OpenGL context, null pointer on error
		*/
		GLXContext createOpenGLContext();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		OpenGLRuntimeLinking *mOpenGLRuntimeLinking;	///< OpenGL runtime linking instance, always valid
		handle				  mNativeWindowHandle;		///< OpenGL window, can be a null pointer (Window)
		handle				  mDummyWindow;				///< OpenGL dummy window, can be a null pointer (Window)
		Display				 *mDisplay;					///< The device context of the OpenGL dummy window, can be a null pointer
		XVisualInfo 		 *m_pDummyVisualInfo;
		GLXContext			  mWindowRenderContext;		///< The render context of the OpenGL dummy window, can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/Linux/ContextLinux.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_CONTEXTLINUX_H__
