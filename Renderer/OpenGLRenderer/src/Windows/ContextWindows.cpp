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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/Windows/ContextWindows.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	ContextWindows::ContextWindows(handle nativeWindowHandle) :
		mOpenGLRuntimeLinking(new OpenGLRuntimeLinking()),
		mNativeWindowHandle(nativeWindowHandle),
		mDummyWindow(NULL_HANDLE),
		mWindowDeviceContext(NULL_HANDLE),
		mWindowRenderContext(NULL_HANDLE)
	{
		// Is OpenGL available?
		if (mOpenGLRuntimeLinking->isOpenGLAvaiable())
		{
			// Create a OpenGL dummy window?
			// -> Under MS Windows, a OpenGL context is always coupled to a window... even if we're not going to render into a window at all...
			if (NULL_HANDLE == mNativeWindowHandle)
			{
				// Setup and register the window class for the OpenGL dummy window
				WNDCLASS windowDummyClass;
				windowDummyClass.hInstance		= ::GetModuleHandle(nullptr);
				windowDummyClass.lpszClassName	= TEXT("OpenGLDummyWindow");
				windowDummyClass.lpfnWndProc	= DefWindowProc;
				windowDummyClass.style			= 0;
				windowDummyClass.hIcon			= nullptr;
				windowDummyClass.hCursor		= nullptr;
				windowDummyClass.lpszMenuName	= nullptr;
				windowDummyClass.cbClsExtra		= 0;
				windowDummyClass.cbWndExtra		= 0;
				windowDummyClass.hbrBackground	= nullptr;
				::RegisterClass(&windowDummyClass);

				// Create the OpenGL dummy window
				mNativeWindowHandle = mDummyWindow = reinterpret_cast<handle>(::CreateWindow(TEXT("OpenGLDummyWindow"), TEXT("PFormat"), WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 8, 8, HWND_DESKTOP, nullptr, ::GetModuleHandle(nullptr), nullptr));
			}

			// Is there a valid window handle?
			if (NULL_HANDLE != mNativeWindowHandle)
			{
				// Get the device context of the OpenGL window
				mWindowDeviceContext = ::GetDC(reinterpret_cast<HWND>(mNativeWindowHandle));
				if (NULL_HANDLE != mWindowDeviceContext)
				{
					// Get the color depth of the deskop
					int bits = 32;
					{
						HDC deskTopDC = ::GetDC(nullptr);
						bits = ::GetDeviceCaps(deskTopDC, BITSPIXEL);
						::ReleaseDC(nullptr, deskTopDC);
					}

					// Get the first best pixel format
					const PIXELFORMATDESCRIPTOR pixelFormatDescriptor =
					{
						sizeof(PIXELFORMATDESCRIPTOR),	// Size of this pixel format descriptor
						1,								// Version number
						PFD_DRAW_TO_WINDOW |			// Format must support window
						PFD_SUPPORT_OPENGL |			// Format must support OpenGL
						PFD_DOUBLEBUFFER,				// Must support double buffering
						PFD_TYPE_RGBA,					// Request an RGBA format
						static_cast<UCHAR>(bits),		// Select our color depth
						0, 0, 0, 0, 0, 0,				// Color bits ignored
						0,								// No alpha buffer
						0,								// Shift bit ignored
						0,								// No accumulation buffer
						0, 0, 0, 0,						// Accumulation bits ignored
						static_cast<BYTE>(24),			// Z-buffer (depth buffer)
						0,								// No stencil buffer
						0,								// No auxiliary buffer
						PFD_MAIN_PLANE,					// Main drawing layer
						0,								// Reserved
						0, 0, 0							// Layer masks ignored
					};
					const int pixelFormat = ::ChoosePixelFormat(mWindowDeviceContext, &pixelFormatDescriptor);
					if (0 != pixelFormat)
					{
						// Set the pixel format
						::SetPixelFormat(mWindowDeviceContext, pixelFormat, &pixelFormatDescriptor);

						// Create a legacy OpenGL render context
						HGLRC legacyRenderContext = wglCreateContext(mWindowDeviceContext);
						if (NULL_HANDLE != legacyRenderContext)
						{
							// Make the legacy OpenGL render context to the current one
							wglMakeCurrent(mWindowDeviceContext, legacyRenderContext);

							// Create the render context of the OpenGL window
							mWindowRenderContext = createOpenGLContext();

							// Destroy the legacy OpenGL render context
							wglMakeCurrent(nullptr, nullptr);
							wglDeleteContext(legacyRenderContext);

							// If there's an OpenGL context, do some final initialization steps
							if (NULL_HANDLE != mWindowRenderContext)
							{
								// Make the OpenGL context to the current one
								wglMakeCurrent(mWindowDeviceContext, mWindowRenderContext);
							}
						}
						else
						{
							// Error, failed to create a legacy OpenGL render context!
						}
					}
					else
					{
						// Error, failed to choose a pixel format!
					}
				}
				else
				{
					// Error, failed to obtain the device context of the OpenGL window!
				}
			}
			else
			{
				// Error, failed to create the OpenGL window!
			}

			// Is there a valid render context?
			if (nullptr != mWindowRenderContext)
			{
				// Initialize the OpenGL extensions
				getExtensions().initialize();

				#ifdef RENDERER_OUTPUT_DEBUG
					// "GL_ARB_debug_output"-extension available?
					if (getExtensions().isGL_ARB_debug_output())
					{
						// Synchronous debug output, please
						// -> Makes it easier to find the place causing the issue
						glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

						// We don't need to configure the debug output by using "glDebugMessageControlARB()",
						// by default all messages are enabled and this is good this way

						// Set the debug message callback function
						glDebugMessageCallbackARB(&ContextWindows::debugMessageCallback, nullptr);
					}
				#endif
			}
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	ContextWindows::~ContextWindows()
	{
		// Release the device context of the OpenGL window
		if (NULL_HANDLE != mWindowDeviceContext)
		{
			// Is the device context of the OpenGL window is the currently active OpenGL device context?
			if (wglGetCurrentDC() == mWindowDeviceContext)
			{
				wglMakeCurrent(nullptr, nullptr);
			}

			// Destroy the render context of the OpenGL window
			if (NULL_HANDLE != mWindowRenderContext)
			{
				wglDeleteContext(mWindowRenderContext);
			}

			// Release the device context of the OpenGL window
			if (NULL_HANDLE != mNativeWindowHandle)
			{
				::ReleaseDC(reinterpret_cast<HWND>(mNativeWindowHandle), mWindowDeviceContext);
			}
		}

		// Destroy the OpenGL dummy window, in case there's one
		if (NULL_HANDLE != mDummyWindow)
		{
			// Destroy the OpenGL dummy window
			::DestroyWindow(reinterpret_cast<HWND>(mDummyWindow));

			// Unregister the window class for the OpenGL dummy window
			::UnregisterClass(TEXT("OpenGLDummyWindow"), ::GetModuleHandle(nullptr));
		}

		// Destroy the OpenGL runtime linking instance
		delete mOpenGLRuntimeLinking;
	}


	//[-------------------------------------------------------]
	//[ Private static methods                                ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Debug message callback function called by the "GL_ARB_debug_output"-extension
	*/
	void ContextWindows::debugMessageCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int, const char *message, void *)
	{
		// Source to string
		char debugSource[16];
		switch (source)
		{
			case GL_DEBUG_SOURCE_API_ARB:
				strncpy(debugSource, "OpenGL", 7);
				break;

			case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
				strncpy(debugSource, "Windows", 8);
				break;

			case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
				strncpy(debugSource, "Shader compiler", 16);
				break;

			case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
				strncpy(debugSource, "Third party", 12);
				break;

			case GL_DEBUG_SOURCE_APPLICATION_ARB:
				strncpy(debugSource, "Application", 12);
				break;

			case GL_DEBUG_SOURCE_OTHER_ARB:
				strncpy(debugSource, "Other", 6);
				break;

			default:
				strncpy(debugSource, "?", 1);
				break;
		}

		// Debug type to string
		char debugType[20];
		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR_ARB:
				strncpy(debugType, "Error", 6);
				break;

			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
				strncpy(debugType, "Deprecated behavior", 20);
				break;

			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
				strncpy(debugType, "Undefined behavior", 19);
				break;

			case GL_DEBUG_TYPE_PORTABILITY_ARB:
				strncpy(debugType, "Portability", 12);
				break;

			case GL_DEBUG_TYPE_PERFORMANCE_ARB:
				strncpy(debugType, "Performance", 12);
				break;

			case GL_DEBUG_TYPE_OTHER_ARB:
				strncpy(debugType, "Other", 6);
				break;

			default:
				strncpy(debugType, "?", 1);
				break;
		}

		// Debug severity to string
		char debugSeverity[7];
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH_ARB:
				strncpy(debugSeverity, "High", 5);
				break;

			case GL_DEBUG_SEVERITY_MEDIUM_ARB:
				strncpy(debugSeverity, "Medium", 7);
				break;

			case GL_DEBUG_SEVERITY_LOW_ARB:
				strncpy(debugSeverity, "Low", 3);
				break;

			default:
				strncpy(debugType, "?", 1);
				break;
		}

		// Output the debug message
		#ifdef _DEBUG
			RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL error: OpenGL debug message\tSource:\"%s\"\tType:\"%s\"\tID:\"%d\"\tSeverity:\"%s\"\tMessage:\"%s\"\n", debugSource, debugType, id, debugSeverity, message)
		#else
			// Avoid "warning C4100: '<x>' : unreferenced formal parameter"-warning
			id = id;
			message = message;
		#endif
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Create a OpenGL context
	*/
	HGLRC ContextWindows::createOpenGLContext()
	{
		// Disable the following warning, we can't do anything to resolve this warning
		__pragma(warning(push))
		__pragma(warning(disable: 4191))	// warning C4191: 'reinterpret_cast' : unsafe conversion from 'PROC' to '<x>'

		// Get the OpenGL extension wglGetExtensionsStringARB function pointer, we need it to check for further supported OpenGL extensions
		PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(wglGetProcAddress("wglGetExtensionsStringARB"));
		if (nullptr != wglGetExtensionsStringARB)
		{
			// Get the available WGL extensions as string
			const char *extensions = wglGetExtensionsStringARB(mWindowDeviceContext);

			// Check whether or not "WGL_ARB_create_context" is a substring of the WGL extension string meaning that this OpenGL extension is supported
			if (nullptr != strstr(extensions, "WGL_ARB_create_context"))
			{
				// Get the OpenGL extension "wglCreateContextAttribsARB" function pointer
				PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
				if (nullptr != wglCreateContextAttribsARB)
				{
					// OpenGL 3.1 - required for "gl_InstanceID" within shaders
					// Create the OpenGL context
					static const int ATTRIBUTES[] =
					{
						// We want an OpenGL context
						WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
						WGL_CONTEXT_MINOR_VERSION_ARB, 1,
						// -> "WGL_CONTEXT_DEBUG_BIT_ARB" comes from the "GL_ARB_debug_output"-extension
						// TODO(co) Make it possible to activate "WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB" from the outside
						#ifdef RENDERER_OUTPUT_DEBUG
							WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
						//	WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,	// Error messages like "Implicit version number 110 not supported by GL3 forward compatible context" might occur
						#else
						//	WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,	// Error messages like "Implicit version number 110 not supported by GL3 forward compatible context" might occur
						#endif
						// Done
						0
					};

					const HGLRC hglrc = wglCreateContextAttribsARB(mWindowDeviceContext, nullptr, ATTRIBUTES);
					if (nullptr != hglrc)
					{
						// Done
						return hglrc;
					}
					else
					{
						// Error, context creation failed!
						return NULL_HANDLE;
					}
				}
				else
				{
					// Error, failed to obtain the "wglCreateContextAttribsARB" function pointer (wow, something went terrible wrong!)
					return NULL_HANDLE;
				}
			}
			else
			{
				// Error, the OpenGL extension "WGL_ARB_create_context" is not supported... as a result we can't create an OpenGL context!
				return NULL_HANDLE;
			}
		}
		else
		{
			// Error, failed to obtain the "wglGetExtensionsStringARB" function pointer (wow, something went terrible wrong!)
			return NULL_HANDLE;
		}

		// Restore the previous warning configuration
		__pragma(warning(pop))
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
