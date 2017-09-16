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
#include "OpenGLRenderer/Linux/OpenGLContextLinux.h"
#include "OpenGLRenderer/RenderTarget/RenderPass.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"

#include <Renderer/ILog.h>
#include <Renderer/Context.h>

#include <GL/glext.h>

// Need to redefine "None"-macro (which got undefined in "Extensions.h" due name clashes used in enums)
#ifndef None
	#define None 0L	///< Universal null resource or null atom
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	OpenGLContextLinux::OpenGLContextLinux(OpenGLRenderer& openGLRenderer, const RenderPass& renderPass, handle nativeWindowHandle, bool useExternalContext, const OpenGLContextLinux* shareContextLinux) :
		OpenGLContextLinux(openGLRenderer, nullptr, renderPass, nativeWindowHandle, useExternalContext, shareContextLinux)
	{
		// Nothing here
	}

	OpenGLContextLinux::~OpenGLContextLinux()
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
			if (NULL_HANDLE != mWindowRenderContext && mOwnsRenderContext)
			{
				glXDestroyContext(mDisplay, mWindowRenderContext);
			}

			if (mOwnsX11Display)
			{
				XCloseDisplay(mDisplay);
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual OpenGLRenderer::IOpenGLContext methods ]
	//[-------------------------------------------------------]
	void OpenGLContextLinux::makeCurrent() const
	{
		// Only do something when have created our renderer context and don't use a external renderer context
		if (!mUseExternalContext)
		{
			glXMakeCurrent(getDisplay(), mNativeWindowHandle, getRenderContext());
		}
	}


	// TODO(co) Cleanup
	static bool ctxErrorOccurred = false;
	static int ctxErrorHandler(Display*, XErrorEvent*)
	{
		ctxErrorOccurred = true;
		return 0;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	OpenGLContextLinux::OpenGLContextLinux(OpenGLRenderer& openGLRenderer, OpenGLRuntimeLinking* openGLRuntimeLinking, const RenderPass& renderPass, handle nativeWindowHandle, bool useExternalContext, const OpenGLContextLinux* shareContextLinux) :
		IOpenGLContext(openGLRuntimeLinking),
		mOpenGLRenderer(openGLRenderer),
		mNativeWindowHandle(nativeWindowHandle),
		mDisplay(nullptr),
		mOwnsX11Display(true),
		mWindowRenderContext(NULL_HANDLE),
		mUseExternalContext(useExternalContext),
		mOwnsRenderContext(true)
	{
		if (mUseExternalContext)
		{
			// We use an external context -> load the OpenGL 3 entry points
			loadOpenGL3EntryPoints();
		}
		else
		{
			const Renderer::Context& context = openGLRenderer.getContext();
			assert(context.getType() == Renderer::Context::ContextType::X11);

			// If the given renderer context is an X11 context use the display connection object provided by the context
			if (context.getType() == Renderer::Context::ContextType::X11)
			{
				mDisplay = static_cast<const Renderer::X11Context&>(context).getDisplay();
				mOwnsX11Display = mDisplay == nullptr;
			}

			if (mOwnsX11Display)
			{
				mDisplay = XOpenDisplay(0);
			}
		}
		if (nullptr != mDisplay)
		{
			// Lookout! OpenGL context sharing chaos: https://www.opengl.org/wiki/OpenGL_Context
			// "State" objects are not shared between contexts, including but not limited to:
			// - Vertex Array Objects (VAOs)
			// - Framebuffer Objects (FBOs)
			// -> Keep away from the share context parameter of "glxCreateContextAttribsARB()" and just share the OpenGL render context instead
			if (nullptr != shareContextLinux)
			{
				mWindowRenderContext = shareContextLinux->getRenderContext();
				mOwnsRenderContext = false;
			}
			else
			{
				// TODO(sw) We don't need a dummy context to load gl/glx entry points see "Misconception #2" from https://dri.freedesktop.org/wiki/glXGetProcAddressNeverReturnsNULL/
				// Load the >= OpenGL 3.0 entry points
				if (loadOpenGL3EntryPoints())
				{
					// Create the render context of the OpenGL window
					mWindowRenderContext = createOpenGLContext(renderPass);

					// If there's an OpenGL context, do some final initialization steps
					if (NULL_HANDLE != mWindowRenderContext)
					{
						// Make the OpenGL context to the current one, native window handle can be zero -> thus only offscreen rendering is supported/wanted
						const int result = glXMakeCurrent(mDisplay, mNativeWindowHandle, mWindowRenderContext);
						RENDERER_LOG(mOpenGLRenderer.getContext(), DEBUG, "Make new OpenGL context current: %d", result)
						{
							int major = 0;
							glGetIntegerv(GL_MAJOR_VERSION, &major);

							int minor = 0;
							glGetIntegerv(GL_MINOR_VERSION, &minor);

							GLint profile = 0;
							glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
							const bool isCoreProfile = (profile & GL_CONTEXT_CORE_PROFILE_BIT);

							RENDERER_LOG(mOpenGLRenderer.getContext(), DEBUG, "OpenGL context version: %d.%d %s", major, minor, (isCoreProfile ? "core" : "noncore"))
							int numberOfExtensions = 0;
							glGetIntegerv(GL_NUM_EXTENSIONS, &numberOfExtensions);
							RENDERER_LOG(mOpenGLRenderer.getContext(), DEBUG, "Number of supported OpenGL extensions: %d", numberOfExtensions)
							for (GLuint extensionIndex = 0; extensionIndex < static_cast<GLuint>(numberOfExtensions); ++extensionIndex)
							{
								RENDERER_LOG(mOpenGLRenderer.getContext(), DEBUG, "%s", glGetStringi(GL_EXTENSIONS, extensionIndex))
							}
						}
					}
				}
				else
				{
					// Error, failed to load >= OpenGL 3 entry points!
				}
			}
		}
		else
		{
			// Error, failed to get display!
		}
	}

	GLXContext OpenGLContextLinux::createOpenGLContext(const RenderPass& renderPass)
	{
		#define GLX_CONTEXT_MAJOR_VERSION_ARB	0x2091
		#define GLX_CONTEXT_MINOR_VERSION_ARB	0x2092

		// Get the available GLX extensions as string
		const char* extensions = glXQueryExtensionsString(mDisplay, XDefaultScreen(mDisplay));

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

				// Create the OpenGL context
				// -> OpenGL 4.1 (the best OpenGL version Mac OS X 10.11 supports, so lowest version we have to support)
				int ATTRIBUTES[] =
				{
					// We want an OpenGL context
					GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
					GLX_CONTEXT_MINOR_VERSION_ARB, 1,
					// -> "GLX_CONTEXT_DEBUG_BIT_ARB" comes from the "GL_ARB_debug_output"-extension
					GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
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

				// TODO(co) Use more detailed color and depth/stencil information from render pass
				const int depthBufferBits = 24;//(renderPass.getDepthStencilAttachmentTextureFormat() == Renderer::TextureFormat::Enum::UNKNOWN) ? 24 : 24;
				int numberOfElements = 0;
				int visualAttributes[] =
				{
					GLX_RENDER_TYPE,	GLX_RGBA_BIT,
					GLX_DOUBLEBUFFER,	true,
					GLX_RED_SIZE,		8,
					GLX_GREEN_SIZE,		8,
					GLX_BLUE_SIZE,		8,
					GLX_ALPHA_SIZE,		8,
					GLX_DEPTH_SIZE,		depthBufferBits,
					GLX_STENCIL_SIZE,	8,
					None
				};
				GLXFBConfig* fbc = glXChooseFBConfig(mDisplay, DefaultScreen(mDisplay), visualAttributes, &numberOfElements);
				RENDERER_LOG(mOpenGLRenderer.getContext(), DEBUG, "Got %d of OpenGL GLXFBConfig", numberOfElements)
				GLXContext glxContext = glXCreateContextAttribsARB(mDisplay, *fbc, 0, true, ATTRIBUTES);

				XSync(mDisplay, False);

				// TODO(sw) make this fallback optional (via an option)
				if (ctxErrorOccurred)
				{
					RENDERER_LOG(mOpenGLRenderer.getContext(), DEBUG, "Could not create OpenGL 3+ context try creating pre 3+ context")
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
					RENDERER_LOG(mOpenGLRenderer.getContext(), DEBUG, "OpenGL context with glXCreateContextAttribsARB created")
					return glxContext;
				}
				else
				{
					// Error, context creation failed!
					RENDERER_LOG(mOpenGLRenderer.getContext(), CRITICAL, "Could not create OpenGL context with glXCreateContextAttribsARB")
					return NULL_HANDLE;
				}
			}
			else
			{
				// Error, failed to obtain the "GLX_ARB_create_context" function pointer (wow, something went terrible wrong!)
				RENDERER_LOG(mOpenGLRenderer.getContext(), CRITICAL, "Could not find OpenGL glXCreateContextAttribsARB")
				return NULL_HANDLE;
			}
		}
		else
		{
			// Error, the OpenGL extension "GLX_ARB_create_context" is not supported... as a result we can't create an OpenGL context!
			RENDERER_LOG(mOpenGLRenderer.getContext(), CRITICAL, "OpenGL GLX_ARB_create_context not supported")
			return NULL_HANDLE;
		}

		#undef GLX_CONTEXT_MAJOR_VERSION_ARB
		#undef GLX_CONTEXT_MINOR_VERSION_ARB
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
