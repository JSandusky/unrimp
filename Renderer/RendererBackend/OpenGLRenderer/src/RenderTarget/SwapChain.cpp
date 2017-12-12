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
#include "OpenGLRenderer/RenderTarget/SwapChain.h"
#include "OpenGLRenderer/RenderTarget/RenderPass.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/Extensions.h"
#ifdef WIN32
	#include "OpenGLRenderer/Windows/OpenGLContextWindows.h"
#elif defined LINUX
	#include "OpenGLRenderer/Linux/OpenGLContextLinux.h"
#endif

#include <Renderer/IAllocator.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	#include <tuple>	// For "std::ignore"
	#include <algorithm>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SwapChain::SwapChain(Renderer::IRenderPass& renderPass, Renderer::WindowHandle windowHandle, bool useExternalContext) :
		ISwapChain(renderPass),
		mNativeWindowHandle(windowHandle.nativeWindowHandle),
		#ifdef WIN32
			mOpenGLContext(RENDERER_NEW(renderPass.getRenderer().getContext(), OpenGLContextWindows)(static_cast<const RenderPass&>(renderPass), windowHandle.nativeWindowHandle, static_cast<const OpenGLContextWindows*>(&static_cast<OpenGLRenderer&>(renderPass.getRenderer()).getOpenGLContext()))),
		#elif defined LINUX
			mOpenGLContext(RENDERER_NEW(renderPass.getRenderer().getContext(), OpenGLContextLinux)(static_cast<OpenGLRenderer&>(renderPass.getRenderer()), static_cast<const RenderPass&>(renderPass), windowHandle.nativeWindowHandle, useExternalContext, static_cast<const OpenGLContextLinux*>(&static_cast<OpenGLRenderer&>(renderPass.getRenderer()).getOpenGLContext()))),
		#else
			#error "Unsupported platform"
		#endif
		mOwnsOpenGLContext(true),
		mRenderWindow(windowHandle.renderWindow),
		mVerticalSynchronizationInterval(0),
		mNewVerticalSynchronizationInterval(~0u)
	{
		#ifdef WIN32
			std::ignore = useExternalContext;
		#endif
	}

	SwapChain::~SwapChain()
	{
		if (mOwnsOpenGLContext)
		{
			RENDERER_DELETE(getRenderer().getContext(), IOpenGLContext, mOpenGLContext);
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void SwapChain::getWidthAndHeight(uint32_t& width, uint32_t& height) const
	{
		// Return stored width and height when both valid
		if (nullptr != mRenderWindow)
		{
			mRenderWindow->getWidthAndHeight(width, height);
			return;
		}
		#ifdef WIN32
			// Is there a valid native OS window?
			if (NULL_HANDLE != mNativeWindowHandle)
			{
				// Get the width and height
				long swapChainWidth  = 1;
				long swapChainHeight = 1;
				{
					// Get the client rectangle of the native output window
					// -> Don't use the width and height stored in "DXGI_SWAP_CHAIN_DESC" -> "DXGI_MODE_DESC"
					//    because it might have been modified in order to avoid zero values
					RECT rect;
					::GetClientRect(reinterpret_cast<HWND>(mNativeWindowHandle), &rect);

					// Get the width and height...
					swapChainWidth  = rect.right  - rect.left;
					swapChainHeight = rect.bottom - rect.top;

					// ... and ensure that none of them is ever zero
					if (swapChainWidth < 1)
					{
						swapChainWidth = 1;
					}
					if (swapChainHeight < 1)
					{
						swapChainHeight = 1;
					}
				}

				// Write out the width and height
				width  = static_cast<UINT>(swapChainWidth);
				height = static_cast<UINT>(swapChainHeight);
			}
			else
		#elif defined LINUX
			if (NULL_HANDLE != mNativeWindowHandle)
			{
				OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
				Display* display = static_cast<const OpenGLContextLinux&>(openGLRenderer.getOpenGLContext()).getDisplay();

				// Get the width and height...
				::Window rootWindow = 0;
				int positionX = 0, positionY = 0;
				unsigned int unsignedWidth = 0, unsignedHeight = 0, border = 0, depth = 0;
				if (nullptr != display)
				{
					XGetGeometry(display, mNativeWindowHandle, &rootWindow, &positionX, &positionY, &unsignedWidth, &unsignedHeight, &border, &depth);
				}

				// ... and ensure that none of them is ever zero
				if (unsignedWidth < 1)
				{
					unsignedWidth = 1;
				}
				if (unsignedHeight < 1)
				{
					unsignedHeight = 1;
				}

				// Done
				width = unsignedWidth;
				height = unsignedHeight;
			}
			else
		#else
			#error "Unsupported platform"
		#endif
		{
			// Set known default return values
			width  = 1;
			height = 1;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ISwapChain methods           ]
	//[-------------------------------------------------------]
	void SwapChain::setVerticalSynchronizationInterval(uint32_t synchronizationInterval)
	{
		mNewVerticalSynchronizationInterval = synchronizationInterval;
	}

	void SwapChain::present()
	{
		if (nullptr != mRenderWindow)
		{
			mRenderWindow->present();
			return;
		}
		#ifdef WIN32
		{
			// Set new vertical synchronization interval?
			// -> We do this in here to avoid having to use "wglMakeCurrent()"/"glXMakeCurrent()" to often at multiple places
			if (~0u != mNewVerticalSynchronizationInterval)
			{
				const Extensions& extensions = static_cast<OpenGLRenderer&>(getRenderer()).getExtensions();
				if (extensions.isWGL_EXT_swap_control())
				{
					int setVerticalSynchronizationInterval = static_cast<int>(mNewVerticalSynchronizationInterval);
					if (extensions.isWGL_EXT_swap_control_tear() && setVerticalSynchronizationInterval > 0)
					{
						// Use adaptive vertical synchronization
						setVerticalSynchronizationInterval = -setVerticalSynchronizationInterval;
					}
					wglSwapIntervalEXT(setVerticalSynchronizationInterval);
				}
				mVerticalSynchronizationInterval = mNewVerticalSynchronizationInterval;
				mNewVerticalSynchronizationInterval = ~0u;
			}

			// Swap buffers
			::SwapBuffers(static_cast<OpenGLContextWindows*>(mOpenGLContext)->getDeviceContext());
			if (mVerticalSynchronizationInterval > 0)
			{
				glFinish();
			}
		}
		#elif defined LINUX
			// TODO(co) Add support for vertical synchronization and adaptive vertical synchronization: "GLX_EXT_swap_control" and "GLX_EXT_swap_control_tear"
			if (NULL_HANDLE != mNativeWindowHandle)
			{
				OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
				glXSwapBuffers(static_cast<const OpenGLContextLinux&>(openGLRenderer.getOpenGLContext()).getDisplay(), mNativeWindowHandle);
			}
		#else
			#error "Unsupported platform"
		#endif
	}

	void SwapChain::resizeBuffers()
	{
		// Nothing here
	}

	bool SwapChain::getFullscreenState() const
	{
		// TODO(co) Implement me
		return false;
	}

	void SwapChain::setFullscreenState(bool)
	{
		// TODO(co) Implement me
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
