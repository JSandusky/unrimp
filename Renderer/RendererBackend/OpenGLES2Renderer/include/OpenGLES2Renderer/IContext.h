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
#include <Renderer/PlatformTypes.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#ifdef WIN32
	// Disable warnings in external headers, we can't fix them
	__pragma(warning(push))
		__pragma(warning(disable: 4668))	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
		#include <EGL/egl.h>
	__pragma(warning(pop))
#else
	#if (defined(LINUX) && !defined(ANDROID))
		#include <X11/Xutil.h>	// For "Display"
	#endif
	#include <EGL/egl.h>
#endif

// Get rid of some nasty OS macros
#undef max
#undef None	// Linux: Undefine "None", this name is used inside enums defined by Unrimp (which gets defined inside Xlib.h pulled in by glx.h)


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{
	class IExtensions;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract OpenGL ES context base interface
	*/
	class IContext
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		virtual ~IContext();

		/**
		*  @brief
		*    Return the handle of a native OS window which is valid as long as the renderer instance exists
		*
		*  @return
		*    The handle of a native OS window which is valid as long as the renderer instance exists, "NULL_HANDLE" if there's no such window
		*/
		inline handle getNativeWindowHandle() const;

		/**
		*  @brief
		*    Return the used EGL display
		*
		*  @return
		*    The used EGL display, "EGL_NO_DISPLAY" on error
		*/
		inline EGLDisplay getEGLDisplay() const;

		/**
		*  @brief
		*    Return the used EGL configuration
		*
		*  @return
		*    The used EGL configuration, null pointer on error
		*/
		inline EGLConfig getEGLConfig() const;

		/**
		*  @brief
		*    Return the used EGL context
		*
		*  @return
		*    The used EGL context, "EGL_NO_CONTEXT" on error
		*/
		inline EGLContext getEGLContext() const;

		/**
		*  @brief
		*    Return the used EGL dummy surface
		*
		*  @return
		*    The used EGL dummy surface, "EGL_NO_SURFACE" on error
		*/
		inline EGLSurface getEGLDummySurface() const;

		/**
		*  @brief
		*    Makes a given EGL surface to the currently used one
		*
		*  @param[in] eglSurface
		*    EGL surface to make to the current one, can be a null pointer, in this case an internal dummy surface is set
		*
		*  @return
		*    "EGL_TRUE" if all went fine, else "EGL_FALSE"
		*/
		EGLBoolean makeCurrent(EGLSurface eglSurface);

		//[-------------------------------------------------------]
		//[ Platform specific                                     ]
		//[-------------------------------------------------------]
		#if (defined(LINUX) && !defined(ANDROID))
			inline ::Display* getX11Display() const;
		#endif


	//[-------------------------------------------------------]
	//[ Public virtual OpenGLES2Renderer::IContext methods    ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Initialize the context
		*
		*  @param[in] multisampleAntialiasingSamples
		*    Multisample antialiasing samples per pixel, <=1 means no antialiasing
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		virtual bool initialize(uint32_t multisampleAntialiasingSamples);

		/**
		*  @brief
		*    Return the available extensions
		*
		*  @return
		*    The available extensions
		*/
		virtual const IExtensions &getExtensions() const = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] nativeWindowHandle
		*    Handle of a native OS window which is valid as long as the renderer instance exists, "NULL_HANDLE" if there's no such window
		*/
		explicit IContext(handle nativeWindowHandle);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		explicit IContext(const IContext &source);

		/**
		*  @brief
		*    De-initialize the context
		*/
		void deinitialize();

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		IContext &operator =(const IContext &source);


	//[-------------------------------------------------------]
	//[ Protected virtual OpenGLES2Renderer::IContext methods ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Chooses a EGL config
		*
		*  @param[in] multisampleAntialiasingSamples
		*    Multisample antialiasing samples per pixel
		*
		*  @return
		*    The chosen EGL config, a null pointer on error
		*
		*  @note
		*    - Automatically tries to find fallback configurations
		*/
		virtual EGLConfig chooseConfig(uint32_t multisampleAntialiasingSamples) const;


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		handle				mNativeWindowHandle;	///< Handle of a native OS window which is valid as long as the renderer instance exists, "NULL_HANDLE" if there's no such window
		// X11
		#if (defined(LINUX) && !defined(ANDROID))
			::Display	   *mX11Display;
		#endif
		// EGL
		EGLDisplay			mDisplay;
		// EGL
		EGLConfig			mConfig;
		EGLContext			mContext;
		EGLNativeWindowType	mDummyNativeWindow;	///< Native dummy window handle, can be identical to "mNativeWindowHandle" if it's in fact no dummy at all, can be "NULL_HANDLE"
		EGLSurface			mDummySurface;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer


//[-------------------------------------------------------]
//[ Implementation includes                               ]
//[-------------------------------------------------------]
#include "OpenGLES2Renderer/IContext.inl"
#include "OpenGLES2Renderer/ContextRuntimeLinking.h"	// Required in here because we redefine the OpenGL ES 2 functions for dynamic runtime linking
