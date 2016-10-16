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
#include "OpenGLRenderer/RenderTarget/SwapChain.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#ifdef WIN32
	#include "OpenGLRenderer/Windows/ContextWindows.h"
#elif defined LINUX
	#include "OpenGLRenderer/Linux/ContextLinux.h"
	#include "OpenGLRenderer/OpenGLRuntimeLinking.h" // For "glxSwapBuffers()"
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SwapChain::SwapChain(OpenGLRenderer &openGLRenderer, handle nativeWindowHandle) :
		ISwapChain(openGLRenderer),
		mNativeWindowHandle(nativeWindowHandle),
		#ifdef WIN32
			mContext(new ContextWindows(nativeWindowHandle, static_cast<const ContextWindows*>(&openGLRenderer.getContext()))),
		#elif defined LINUX
			mContext(new ContextLinux(nativeWindowHandle, false)),	// TODO(co) Add share context
		#else
			#error "Unsupported platform"
		#endif
		mOwnsContext(true)
	{
		// Nothing here
	}

	SwapChain::SwapChain(OpenGLRenderer &openGLRenderer, handle nativeWindowHandle, IContext& context) :
		ISwapChain(openGLRenderer),
		mNativeWindowHandle(nativeWindowHandle),
		mContext(&context),
		mOwnsContext(false)
	{
		// Nothing here
	}

	SwapChain::~SwapChain()
	{
		if (mOwnsContext)
		{
			delete mContext;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void SwapChain::getWidthAndHeight(uint32_t &width, uint32_t &height) const
	{
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
				OpenGLRenderer &openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
				Display *display = static_cast<const ContextLinux&>(openGLRenderer.getContext()).getDisplay();

				::Window rootWindow = 0;
				int positionX = 0, positionY = 0;
				unsigned int unsignedWidth = 0, unsignedHeight = 0, border = 0, depth = 0;
				XGetGeometry(display, mNativeWindowHandle, &rootWindow, &positionX, &positionY, &unsignedWidth, &unsignedHeight, &border, &depth);
				width = unsignedWidth;
				height = unsignedHeight;
			}
			else
		#else
			#error "Unsupported platform"
		#endif
		{
			// Set known default return values
			width  = 0;
			height = 0;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ISwapChain methods           ]
	//[-------------------------------------------------------]
	handle SwapChain::getNativeWindowHandle() const
	{
		return mNativeWindowHandle;
	}

	void SwapChain::present()
	{
		#ifdef WIN32
			HDC hDC = ::GetDC(reinterpret_cast<HWND>(mNativeWindowHandle));
			::SwapBuffers(hDC);
			::ReleaseDC(reinterpret_cast<HWND>(mNativeWindowHandle), hDC);
		#elif defined LINUX
			OpenGLRenderer &openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
			glXSwapBuffers(static_cast<const ContextLinux&>(openGLRenderer.getContext()).getDisplay(), mNativeWindowHandle);
		#else
			#error "Unsupported platform"
		#endif
	}

	void SwapChain::resizeBuffers()
	{
		// Nothing to do in here
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
