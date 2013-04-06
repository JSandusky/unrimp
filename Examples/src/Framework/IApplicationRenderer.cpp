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
#include "Framework/IApplicationRenderer.h"
#ifdef SHARED_LIBRARIES
	// Dynamically linked libraries
	#ifdef WIN32
		#include "Framework/WindowsHeader.h"
	#elif defined LINUX
		#include "Framework/LinuxHeader.h"
		#include <dlfcn.h>
		#include <iostream>
	#else
		#error "Unsupported platform"
	#endif
	
	#include <stdio.h>
#endif

#include <string.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
IApplicationRenderer::~IApplicationRenderer()
{
	// Nothing to do in here
	// mRenderer is destroyed within onDeinitialization()
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void IApplicationRenderer::onInitialization()
{
	// Create the renderer instance (at this point our renderer pointer must be a null pointer, or something went terrible wrong!)
	mRenderer = createRendererInstance(mRendererName);

	// Is there a renderer instance and a native window handle?
	if (nullptr != mRenderer && NULL_HANDLE != getNativeWindowHandle())
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(mRenderer)

		// Get the window size
		int width  = 0;
		int height = 0;
		getWindowSize(width, height);

		// Set the viewport
		const Renderer::Viewport viewport =
		{
			0.0f,						// topLeftX (float)
			0.0f,						// topLeftY (float)
			static_cast<float>(width),	// width (float)
			static_cast<float>(height),	// height (float)
			0.0f,						// minDepth (float)
			1.0f						// maxDepth (float)
		};
		mRenderer->rsSetViewports(1, &viewport);

		// End debug event
		RENDERER_END_DEBUG_EVENT(mRenderer)
	}
}

void IApplicationRenderer::onDeinitialization()
{
	// Delete the renderer instance
	mRenderer = nullptr;

	// Destroy the shared library instance
	#ifdef SHARED_LIBRARIES
		#ifdef WIN32
			if (nullptr != mRendererSharedLibrary)
			{
				::FreeLibrary(static_cast<HMODULE>(mRendererSharedLibrary));
				mRendererSharedLibrary = nullptr;
			}
		#elif defined LINUX
			if (nullptr != mRendererSharedLibrary)
			{
				dlclose(mRendererSharedLibrary);
				mRendererSharedLibrary = nullptr;
			}
		#else
			#error "Unsupported platform"
		#endif
	#endif
}

void IApplicationRenderer::onResize()
{
	// Is there a renderer instance?
	if (nullptr != mRenderer)
	{
		// Get the main swap chain
		Renderer::ISwapChain *swapChain = mRenderer->getMainSwapChain();
		if (nullptr != swapChain)
		{
			// Inform the swap chain that the size of the native window was changed
			// -> Required for Direct3D 9, Direct3D 10, Direct3D 11
			// -> Not required for OpenGL and OpenGL ES 2
			swapChain->resizeBuffers();

			{ // Set the viewport
				// Get the window size
				int width  = 0;
				int height = 0;
				getWindowSize(width, height);

				// Set the viewport
				const Renderer::Viewport viewport =
				{
					0.0f,						// topLeftX (float)
					0.0f,						// topLeftY (float)
					static_cast<float>(width),	// width (float)
					static_cast<float>(height),	// height (float)
					0.0f,						// minDepth (float)
					1.0f						// maxDepth (float)
				};
				mRenderer->rsSetViewports(1, &viewport);
			}
		}
	}
}

void IApplicationRenderer::onToggleFullscreenState()
{
	// Is there a renderer instance?
	if (nullptr != mRenderer)
	{
		// Get the main swap chain
		Renderer::ISwapChain *swapChain = mRenderer->getMainSwapChain();
		if (nullptr != swapChain)
		{
			// Toggle the fullscreen state
			swapChain->setFullscreenState(!swapChain->getFullscreenState());
		}
	}
}

void IApplicationRenderer::onDrawRequest()
{
	// Is there a renderer instance?
	if (nullptr != mRenderer)
	{
		// Get the main swap chain and ensure there's one
		Renderer::ISwapChainPtr swapChain(mRenderer->getMainSwapChain());
		if (nullptr != swapChain)
		{
			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(mRenderer)

			// Make the main swap chain to the current render target
			mRenderer->omSetRenderTarget(swapChain);

			// Call the draw method
			onDraw();

			// Present the content of the current back buffer
			swapChain->present();

			// End debug event
			RENDERER_END_DEBUG_EVENT(mRenderer)
		}
	}
}


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
IApplicationRenderer::IApplicationRenderer(const char *rendererName) :
	IApplication(rendererName),
	mRendererSharedLibrary(nullptr)
{
	// Copy the given renderer name
	if (nullptr != rendererName)
	{
		strncpy(mRendererName, rendererName, 64);
	}
	else
	{
		mRendererName[0] = '\0';
	}
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
Renderer::IRenderer *IApplicationRenderer::createRendererInstance(const char *rendererName)
{
	Renderer::IRenderer *renderer = nullptr;

	// Is the given pointer valid?
	if (nullptr != rendererName)
	{
		// In order to keep it simple in this test project the supported renderer backends are
		// fixed typed in. For a real system a dynamic plugin system would be a good idea.
		#ifdef SHARED_LIBRARIES
			// Dynamically linked libraries
			#ifdef WIN32
				// Load in the dll
				char rendererFilename[128];
				#ifdef _DEBUG
					sprintf(rendererFilename, "%sRendererD.dll", rendererName);
				#else
					sprintf(rendererFilename, "%sRenderer.dll", rendererName);
				#endif
				mRendererSharedLibrary = ::LoadLibraryExA(rendererFilename, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
				if (nullptr != mRendererSharedLibrary)
				{
					// Get the "CreateRendererInstance()" function pointer
					char functionName[128];
					sprintf(functionName, "create%sRendererInstance", rendererName);
					void *symbol = ::GetProcAddress(static_cast<HMODULE>(mRendererSharedLibrary), functionName);
					if (nullptr != symbol)
					{
						// "createRendererInstance()" signature
						typedef Renderer::IRenderer *(__cdecl *createRendererInstance)(Renderer::handle);

						// Create the renderer instance
						renderer = (static_cast<createRendererInstance>(symbol))(getNativeWindowHandle());
					}
					else
					{
						// Error!
						OUTPUT_DEBUG_PRINTF("Failed to locate the entry point \"%s\" within the renderer shared library \"%s\"", functionName, rendererFilename)
					}
				}
				else
				{
					OUTPUT_DEBUG_PRINTF("Failed to load in the shared library \"%s\"\n", rendererFilename)
				}
			#elif defined LINUX
				// Load in the dll
				char rendererFilename[128];
				#ifdef _DEBUG
					sprintf(rendererFilename, "%sRendererD.so", rendererName);
				#else
					sprintf(rendererFilename, "lib%sRenderer.so", rendererName);
				#endif
				mRendererSharedLibrary = ::dlopen(rendererFilename, RTLD_NOW);
				if (nullptr != mRendererSharedLibrary)
				{
					// Get the "CreateRendererInstance()" function pointer
					char functionName[128];
					sprintf(functionName, "create%sRendererInstance", rendererName);
					void *symbol = ::dlsym(mRendererSharedLibrary, functionName);
					if (nullptr != symbol)
					{
						// "createRendererInstance()" signature
						typedef Renderer::IRenderer *(*createRendererInstance)(Renderer::handle);

						// Create the renderer instance
						
						renderer = (reinterpret_cast<createRendererInstance>(symbol))(getNativeWindowHandle());
					}
					else
					{
						// Error!
						std::cerr<<"Failed to locate the entry point \""<<functionName<<"\" within the renderer shared library \""<<rendererFilename<<"\"\n";
						OUTPUT_DEBUG_PRINTF("Failed to locate the entry point \"%s\" within the renderer shared library \"%s\"", functionName, rendererFilename)
					}
				}
				else
				{
					std::cerr<<"Failed to load in the shared library \""<<rendererFilename<<"\"\nReason:"<<dlerror()<<"\n";
					OUTPUT_DEBUG_PRINTF("Failed to load in the shared library \"%s\"\n", rendererFilename)
				}
			#else
				#error "Unsupported platform"
			#endif
		#else
			// Statically linked libraries

			// Null
			#ifndef RENDERER_NO_NULL
				if (0 == strcmp(rendererName, "Null"))
				{
					// "createNullRendererInstance()" signature
					extern Renderer::IRenderer *createNullRendererInstance(Renderer::handle);

					// Create the renderer instance
					renderer = createNullRendererInstance(getNativeWindowHandle());
				}
				else
			#endif

			// OpenGL
			#ifndef RENDERER_NO_OPENGL
				if (0 == strcmp(rendererName, "OpenGL"))
				{
					// "createOpenGLRendererInstance()" signature
					extern Renderer::IRenderer *createOpenGLRendererInstance(Renderer::handle);

					// Create the renderer instance
					renderer = createOpenGLRendererInstance(getNativeWindowHandle());
				}
				else
			#endif

			// OpenGLES2
			#ifndef RENDERER_NO_OPENGLES2
				if (0 == strcmp(rendererName, "OpenGLES2"))
				{
					// "createOpenGLES2RendererInstance()" signature
					extern Renderer::IRenderer *createOpenGLES2RendererInstance(Renderer::handle);

					// Create the renderer instance
					renderer = createOpenGLES2RendererInstance(getNativeWindowHandle());
				}
				else
			#endif

			// Direct3D 9
			#ifndef RENDERER_NO_DIRECT3D9
				if (0 == strcmp(rendererName, "Direct3D9"))
				{
					// "createDirect3D9RendererInstance()" signature
					extern Renderer::IRenderer *createDirect3D9RendererInstance(Renderer::handle);

					// Create the renderer instance
					renderer = createDirect3D9RendererInstance(getNativeWindowHandle());
				}
				else
			#endif

			// Direct3D 10
			#ifndef RENDERER_NO_DIRECT3D10
				if (0 == strcmp(rendererName, "Direct3D10"))
				{
					// "createDirect3D10RendererInstance()" signature
					extern Renderer::IRenderer *createDirect3D10RendererInstance(Renderer::handle);

					// Create the renderer instance
					renderer = createDirect3D10RendererInstance(getNativeWindowHandle());
				}
				else
			#endif

			// Direct3D 11
			#ifndef RENDERER_NO_DIRECT3D11
				if (0 == strcmp(rendererName, "Direct3D11"))
				{
					// "createDirect3D11RendererInstance()" signature
					extern Renderer::IRenderer *createDirect3D11RendererInstance(Renderer::handle);

					// Create the renderer instance
					renderer = createDirect3D11RendererInstance(getNativeWindowHandle());
				}
			#endif

				// Nothing to see, but keep it
				{
				}
		#endif
	}

	// Is the renderer instance is properly initialized?
	if (nullptr != renderer && !renderer->isInitialized())
	{
		// We are not interested in not properly initialized renderer instances, so get rid of the broken thing
		delete renderer;
		renderer = nullptr;
	}

	#ifdef RENDERER_NO_DEBUG
		// By using
		//   "Renderer::IRenderer::isDebugEnabled()"
		// in here its possible to check whether or not your application is currently running
		// within a known debug/profile tool like e.g. Direct3D PIX (also works directly within VisualStudio
		// 2012 out-of-the-box). In case you want at least try to protect your asset, you might want to stop
		// the execution of your application when a debug/profile tool is used which can e.g. record your data.
		// Please be aware that this will only make it a little bit harder to debug and e.g. while doing so
		// reading out your asset data. Public articles like
		// "PIX: How to circumvent D3DPERF_SetOptions" at
		//   http://www.gamedev.net/blog/1323/entry-2250952-pix-how-to-circumvent-d3dperf-setoptions/
		// describe how to "hack around" this security measurement, so, don't rely on it. Those debug
		// methods work fine when using a Direct3D renderer implementation. OpenGL on the other hand
		// has no Direct3D PIX like functions or extensions, use for instance "gDEBugger" (http://www.gremedy.com/)
		// instead.
		if (nullptr != renderer && renderer->isDebugEnabled())
		{
			// We don't allow debugging in case debugging is disabled
			OUTPUT_DEBUG_STRING("Debugging with debug/profile tools like e.g. Direct3D PIX is disabled within this application")
			delete renderer;
			renderer = nullptr;
		}
	#endif

	// Done
	return renderer;
}
