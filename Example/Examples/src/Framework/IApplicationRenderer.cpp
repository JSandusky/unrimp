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
#include "PrecompiledHeader.h"
#include "Framework/IApplicationRenderer.h"
#include "Framework/ExampleBase.h"

#include <Renderer/Public/StdLog.h>
#include <Renderer/Public/RendererInstance.h>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global variables                                      ]
		//[-------------------------------------------------------]
		Renderer::StdLog g_RendererLog;


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
IApplicationRenderer::IApplicationRenderer(const char* rendererName, ExampleBase* exampleBase) :
	IApplication(rendererName),
	mRendererContext(nullptr),
	mRendererInstance(nullptr),
	mRenderer(nullptr),
	mExampleBase(exampleBase)
{
	if (nullptr != mExampleBase)
	{
		mExampleBase->setApplicationFrontend(this);
	}
	
	// Copy the given renderer name
	if (nullptr != rendererName)
	{
		strncpy(mRendererName, rendererName, 32);

		// In case the source string is longer then 32 bytes (including null terminator) make sure that the string is null terminated
		mRendererName[31] = '\0';
	}
	else
	{
		mRendererName[0] = '\0';
	}
}

IApplicationRenderer::~IApplicationRenderer()
{
	// Nothing here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void IApplicationRenderer::onInitialization()
{
	createRenderer();
	initializeExample();
}

void IApplicationRenderer::onDeinitialization()
{
	deinitializeExample();

	// Delete the renderer instance
	mRenderer = nullptr;
	if (nullptr != mRendererInstance)
	{
		mRendererInstance->destroyRenderer();
	}

	// Call base implementation after renderer was destroyed, needed at least under linux see comments in private method RendererInstance::loadRendererApiSharedLibrary for more details
	IApplication::onDeinitialization();

	// Delete the renderer instance
	delete mRendererInstance;
	mRendererInstance = nullptr;
	delete mRendererContext;
	mRendererContext = nullptr;
}

void IApplicationRenderer::onUpdate()
{
	if (nullptr != mExampleBase)
	{
		mExampleBase->onUpdate();
	}
}

void IApplicationRenderer::onResize()
{
	// Is there a renderer instance?
	if (nullptr != mRenderer)
	{
		// Get the main swap chain
		Renderer::ISwapChain* swapChain = mRenderer->getMainSwapChain();
		if (nullptr != swapChain)
		{
			// Inform the swap chain that the size of the native window was changed
			// -> Required for Direct3D 9, Direct3D 10, Direct3D 11
			// -> Not required for OpenGL and OpenGL ES 2
			swapChain->resizeBuffers();
		}
	}
}

void IApplicationRenderer::onToggleFullscreenState()
{
	// Is there a renderer instance?
	if (nullptr != mRenderer)
	{
		// Get the main swap chain
		Renderer::ISwapChain* swapChain = mRenderer->getMainSwapChain();
		if (nullptr != swapChain)
		{
			// Toggle the fullscreen state
			swapChain->setFullscreenState(!swapChain->getFullscreenState());
		}
	}
}

void IApplicationRenderer::onKeyDown(uint32_t key)
{
	if (nullptr != mExampleBase)
	{
		mExampleBase->onKeyDown(key);
	}
}

void IApplicationRenderer::onKeyUp(uint32_t key)
{
	if (nullptr != mExampleBase)
	{
		mExampleBase->onKeyUp(key);
	}
}

void IApplicationRenderer::onMouseButtonDown(uint32_t button)
{
	if (nullptr != mExampleBase)
	{
		mExampleBase->onMouseButtonDown(button);
	}
}

void IApplicationRenderer::onMouseButtonUp(uint32_t button)
{
	if (nullptr != mExampleBase)
	{
		mExampleBase->onMouseButtonUp(button);
	}
}

void IApplicationRenderer::onMouseMove(int x, int y)
{
	if (nullptr != mExampleBase)
	{
		mExampleBase->onMouseMove(x, y);
	}
}

void IApplicationRenderer::onDrawRequest()
{
	if (nullptr != mExampleBase && mExampleBase->doesCompleteOwnDrawing())
	{
		// The example does the drawing completely on its own
		mExampleBase->draw();
	}

	// Is there a renderer instance?
	else if (nullptr != mRenderer)
	{
		// Get the main swap chain and ensure there's one
		Renderer::ISwapChain* swapChain = mRenderer->getMainSwapChain();
		if (nullptr != swapChain)
		{
			// Begin scene rendering
			// -> Required for Direct3D 9 and Direct3D 12
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			if (mRenderer->beginScene())
			{
				// Begin debug event
				COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(mCommandBuffer)

				// Make the main swap chain to the current render target
				Renderer::Command::SetRenderTarget::create(mCommandBuffer, swapChain);

				{ // Since Direct3D 12 is command list based, the viewport and scissor rectangle
				  // must be set in every draw call to work with all supported renderer APIs
					// Get the window size
					uint32_t width  = 1;
					uint32_t height = 1;
					swapChain->getWidthAndHeight(width, height);

					// Set the viewport and scissor rectangle
					Renderer::Command::SetViewportAndScissorRectangle::create(mCommandBuffer, 0, 0, width, height);
				}

				// Submit command buffer to the renderer backend
				mCommandBuffer.submitAndClear(*mRenderer);

				// Call the draw method
				if (nullptr != mExampleBase)
				{
					mExampleBase->draw();
				}

				// End debug event
				COMMAND_END_DEBUG_EVENT(mCommandBuffer)

				// Submit command buffer to the renderer backend
				mCommandBuffer.submitAndClear(*mRenderer);

				// End scene rendering
				// -> Required for Direct3D 9 and Direct3D 12
				// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
				mRenderer->endScene();
			}

			// Present the content of the current back buffer
			swapChain->present();
		}
	}
}


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
IApplicationRenderer::IApplicationRenderer(const char* rendererName) :
	IApplicationRenderer(rendererName, nullptr)
{
	// Nothing here
}

void IApplicationRenderer::createRenderer()
{
	if (nullptr == mRenderer)
	{
		// Create the renderer instance
		mRenderer = createRendererInstance(mRendererName);
	}
}

void IApplicationRenderer::initializeExample()
{
	if (nullptr != mExampleBase)
	{
		mExampleBase->initialize();
	}
}

void IApplicationRenderer::deinitializeExample()
{
	if (nullptr != mExampleBase)
	{
		mExampleBase->deinitialize();
	}
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
Renderer::IRenderer* IApplicationRenderer::createRendererInstance(const char* rendererName)
{
	// Is the given pointer valid?
	if (nullptr != rendererName)
	{
		bool loadRendererApiSharedLibrary = false;
#ifdef WIN32
		mRendererContext = new Renderer::Context(Renderer::Context::ContextType::WIN32, ::detail::g_RendererLog, getNativeWindowHandle());
#elif LINUX
		// Under linux the opengl library interacts with the library fro X11 so we need to load the library ourself instead letting it be loaded by the renderer instance
		// See http://dri.sourceforge.net/doc/DRIuserguide.html "11.5 libGL.so and dlopen()"
		loadRendererApiSharedLibrary = true;
		mRendererContext = new Renderer::X11Context(::detail::g_RendererLog, getX11Display(), getNativeWindowHandle());
#endif
		mRendererInstance = new Renderer::RendererInstance(rendererName, *mRendererContext, loadRendererApiSharedLibrary);
	}
	Renderer::IRenderer* renderer = (nullptr != mRendererInstance) ? mRendererInstance->getRenderer() : nullptr;

	// Is the renderer instance is properly initialized?
	if (nullptr != renderer && !renderer->isInitialized())
	{
		// We are not interested in not properly initialized renderer instances, so get rid of the broken thing
		renderer = nullptr;
		delete mRendererInstance;
		mRendererInstance = nullptr;
		delete mRendererContext;
		mRendererContext = nullptr;
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
			RENDERER_LOG(renderer->getContext(), CRITICAL, "Debugging with debug/profile tools like e.g. Direct3D PIX is disabled within this application")
			delete renderer;
			renderer = nullptr;
		}
	#endif

	// Done
	return renderer;
}
