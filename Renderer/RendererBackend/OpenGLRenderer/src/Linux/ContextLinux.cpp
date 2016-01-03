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
#include "OpenGLRenderer/Linux/ContextLinux.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"

#include <iostream>	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ContextLinux::ContextLinux(handle nativeWindowHandle, bool useExternalContext) :
		mNativeWindowHandle(nativeWindowHandle),
		mDummyWindow(NULL_HANDLE),
		mDisplay(nullptr),
		m_pDummyVisualInfo(nullptr),
		mWindowRenderContext(NULL_HANDLE),
		mUseExternalContext(useExternalContext)
	{
		// Get X server display connection
		if (!mUseExternalContext)
		{
			mDisplay = XOpenDisplay(nullptr);
		}
		if (nullptr != mDisplay)
		{
			// Get an appropriate visual
			int attributeList[] =
			{
				GLX_RGBA,
				GLX_DOUBLEBUFFER,
				GLX_RED_SIZE,	4,
				GLX_GREEN_SIZE,	4,
				GLX_BLUE_SIZE,	4,
				GLX_DEPTH_SIZE,	16,
				0	// = "None"
			};

			m_pDummyVisualInfo = glXChooseVisual(mDisplay, DefaultScreen(mDisplay), attributeList);
			if (nullptr != m_pDummyVisualInfo)
			{
				if (NULL_HANDLE == mNativeWindowHandle)
				{
					// Create a color map
					XSetWindowAttributes setWindowAttributes;
					setWindowAttributes.colormap = XCreateColormap(mDisplay, RootWindow(mDisplay, m_pDummyVisualInfo->screen), m_pDummyVisualInfo->visual, AllocNone);

					// Create a window
					setWindowAttributes.border_pixel = 0;
					setWindowAttributes.event_mask = 0;
					mNativeWindowHandle = mDummyWindow = XCreateWindow(mDisplay, RootWindow(mDisplay, m_pDummyVisualInfo->screen), 0, 0, 300,
																		300, 0, m_pDummyVisualInfo->depth, InputOutput, m_pDummyVisualInfo->visual,
																		CWBorderPixel | CWColormap | CWEventMask, &setWindowAttributes);
					std::cout<<"Create dummy window\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead
				}

				// Create a GLX context
				GLXContext legacyRenderContext = glXCreateContext(mDisplay, m_pDummyVisualInfo, 0, GL_TRUE);
				if (nullptr != legacyRenderContext)
				{
					// Make the internal dummy to the current render target
					const int result = glXMakeCurrent(mDisplay, mNativeWindowHandle, legacyRenderContext);
					std::cout<<"Make legacy context current: "<<result<<"\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead

					// Create the render context of the OpenGL window
					mWindowRenderContext = createOpenGLContext();

					// Destroy the legacy OpenGL render context
					glXMakeCurrent(mDisplay, None, nullptr);
					glXDestroyContext(mDisplay, legacyRenderContext);

					// If there's an OpenGL context, do some final initialization steps
					if (NULL_HANDLE != mWindowRenderContext)
					{
						// Make the OpenGL context to the current one
						const int result = glXMakeCurrent(mDisplay, mNativeWindowHandle, mWindowRenderContext);
						std::cout<<"Make new context current: "<<result<<"\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead
						std::cout<<"Supported extensions: "<<glGetString(GL_EXTENSIONS)<<"\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead
					}
				}
				else
				{
					// Error, failed to create a GLX context!
				}
			}
			else
			{
				// Error, failed to get an appropriate visual!
			}
		}
		else
		{
			// Error, failed to get display!
		}
	}

	ContextLinux::~ContextLinux()
	{
		// Release the device context of the OpenGL window
		if (nullptr != mDisplay)
		{
			// Is the device context of the OpenGL window is the currently active OpenGL device context?
			if (glXGetCurrentContext() == mWindowRenderContext)
			{
				glXMakeCurrent(mDisplay, None, nullptr);
			}

			// Destroy the render context of the OpenGL window
			if (NULL_HANDLE != mWindowRenderContext)
			{
				glXDestroyContext(mDisplay, mWindowRenderContext);
			}
		}

		// Destroy the OpenGL dummy window, in case there's one
		if (NULL_HANDLE != mDummyWindow)
		{
			// Destroy the OpenGL dummy window
			::XDestroyWindow(mDisplay, mDummyWindow);
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual OpenGLRenderer::IContext methods       ]
	//[-------------------------------------------------------]
	void ContextLinux::makeCurrent() const
	{
		glXMakeCurrent(getDisplay(), mNativeWindowHandle, getRenderContext());
	}


	// TODO(co) Cleanup
	static bool ctxErrorOccurred = false;
	static int ctxErrorHandler(Display *, XErrorEvent *)
	{
		ctxErrorOccurred = true;
		return 0;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	GLXContext ContextLinux::createOpenGLContext()
	{
		#define GLX_CONTEXT_MAJOR_VERSION_ARB	0x2091
		#define GLX_CONTEXT_MINOR_VERSION_ARB	0x2092

		// Get the available GLX extensions as string
		const char *extensions = glXQueryExtensionsString(mDisplay, XDefaultScreen(mDisplay));

		// Check whether or not "GLX_ARB_create_context" is a substring of the GLX extension string meaning that this OpenGL extension is supported
		if (nullptr != strstr(extensions, "GLX_ARB_create_context"))
		{
			// Get the OpenGL extension "glXCreateContextAttribsARB" function pointer
			typedef GLXContext (*GLXCREATECONTEXTATTRIBSARBPROC)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
			GLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = reinterpret_cast<GLXCREATECONTEXTATTRIBSARBPROC>(glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB"));
			if (nullptr != glXCreateContextAttribsARB)
			{
				// TODO(co) Cleanup
				ctxErrorOccurred = false;
				int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);

				// OpenGL 3.1 - required for "gl_InstanceID" within shaders
				// Create the OpenGL context
				int ATTRIBUTES[] =
				{
					// We want an OpenGL context
					GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
					GLX_CONTEXT_MINOR_VERSION_ARB, 1,
					// -> "GLX_CONTEXT_DEBUG_BIT_ARB" comes from the "GL_ARB_debug_output"-extension
					// TODO(co) Make it possible to activate "GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB" from the outside
					#ifdef RENDERER_OUTPUT_DEBUG
					//	GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
					//	GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,	// Error messages like "Implicit version number 110 not supported by GL3 forward compatible context" might occur
					#else
					//	GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,	// Error messages like "Implicit version number 110 not supported by GL3 forward compatible context" might occur
					#endif
					// Done
					0
				};

				int numberOfElements = 0;
				int visualAttributes[] =
				{
					GLX_RENDER_TYPE, GLX_RGBA_BIT,
					GLX_DOUBLEBUFFER, true,
					GLX_RED_SIZE, 1,
					GLX_GREEN_SIZE, 1,
					GLX_BLUE_SIZE, 1,
					None
				};
				GLXFBConfig *fbc = glXChooseFBConfig(mDisplay, DefaultScreen(mDisplay), visualAttributes, &numberOfElements);
				std::cout<<"Renderer: Got "<<numberOfElements<<" of FBCOnfig\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead
				GLXContext glxContext = glXCreateContextAttribsARB(mDisplay, *fbc, 0, true, ATTRIBUTES);

				XSync(mDisplay, False);

				// TODO(sw) make this fallback optional (via an option)
				if (ctxErrorOccurred)
				{
					std::cerr<<"could not create opengl 3+ context try creating pre 3+ context\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead
					ctxErrorOccurred = false;

					// GLX_CONTEXT_MAJOR_VERSION_ARB = 1
					ATTRIBUTES[1] = 1;
					// GLX_CONTEXT_MINOR_VERSION_ARB = 0
					ATTRIBUTES[3] = 0;
					glxContext = glXCreateContextAttribsARB(mDisplay, *fbc, 0, true, ATTRIBUTES);

					// Synchronize to ensure any errors generated are processed
					XSync(mDisplay, False);

					// Restore the original error handler
					XSetErrorHandler(oldHandler);
				}

				if (nullptr != glxContext)
				{
					// Done
					std::cout<<"Renderer: OGL Context with glXCreateContextAttribsARB created\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead, do we really need messages on success?
					return glxContext;
				}
				else
				{
					// Error, context creation failed!
					std::cerr<<"Renderer: Could not create OGL Context with glXCreateContextAttribsARB\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead
					return NULL_HANDLE;
				}
			}
			else
			{
				// Error, failed to obtain the "GLX_ARB_create_context" function pointer (wow, something went terrible wrong!)
				std::cerr<<"Renderer: could not found glXCreateContextAttribsARB\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead
				return NULL_HANDLE;
			}
		}
		else
		{
			// Error, the OpenGL extension "GLX_ARB_create_context" is not supported... as a result we can't create an OpenGL context!
			std::cerr<<"Renderer: GLX_ARB_create_context not supported\n";	// TODO(co) Use "RENDERER_OUTPUT_DEBUG_PRINTF" instead
			return NULL_HANDLE;
		}

		#undef GLX_CONTEXT_MAJOR_VERSION_ARB
		#undef GLX_CONTEXT_MINOR_VERSION_ARB
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
