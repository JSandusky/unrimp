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
#include "OpenGLRenderer/SwapChain.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#ifdef WIN32
	#include "OpenGLRenderer/Windows/ContextWindows.h"
#elif defined LINUX
	#include "OpenGLRenderer/Linux/ContextLinux.h"
	#include "OpenGLRenderer/OpenGLRuntimeLinking.h" // for glxSwapBuffers
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
		mNativeWindowHandle(nativeWindowHandle)
	{
		// TODO(co) Implement me
	}

	SwapChain::~SwapChain()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void SwapChain::getWidthAndHeight(unsigned int &width, unsigned int &height) const
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
				Display* display = static_cast<const ContextLinux&>(openGLRenderer.getContext()).getDisplay();
				
				::Window nRootWindow = 0;
				int nPositionX = 0, nPositionY = 0;
				unsigned int nWidth = 0, nHeight = 0, nBorder = 0, nDepth = 0;
				XGetGeometry(display, mNativeWindowHandle, &nRootWindow, &nPositionX, &nPositionY, &nWidth, &nHeight, &nBorder, &nDepth);
				width = nWidth;
				height = nHeight;
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
		// TODO(co) Correct implementation
		OpenGLRenderer &openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		#ifdef WIN32
			SwapBuffers(static_cast<const ContextWindows&>(openGLRenderer.getContext()).getDeviceContext());
		#elif defined LINUX
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
