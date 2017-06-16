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
#include "Direct3D9Renderer/RenderTarget/SwapChain.h"
#include "Direct3D9Renderer/d3d9.h"
#include "Direct3D9Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D9Renderer/Direct3D9Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SwapChain::SwapChain(Direct3D9Renderer &direct3D9Renderer, handle nativeWindowHandle) :
		ISwapChain(direct3D9Renderer),
		mDirect3DSwapChain9(nullptr),
		mDirect3DSurface9RenderTarget(nullptr),
		mDirect3DSurface9DepthStencil(nullptr)
	{
		// Get the Direct3D 9 device instance
		IDirect3DDevice9 *direct3DDevice9 = direct3D9Renderer.getDirect3DDevice9();

		// Get the native window handle
		const HWND hWnd = reinterpret_cast<HWND>(nativeWindowHandle);

		// Get the width and height of the given native window and ensure they are never ever zero
		// -> See "getSafeWidthAndHeight()"-method comments for details
		long width  = 1;
		long height = 1;
		{
			// Get the client rectangle of the given native window
			RECT rect;
			::GetClientRect(hWnd, &rect);

			// Get the width and height...
			width  = rect.right  - rect.left;
			height = rect.bottom - rect.top;

			// ... and ensure that none of them is ever zero
			if (width < 1)
			{
				width = 1;
			}
			if (height < 1)
			{
				height = 1;
			}
		}

		// Set up the structure used to create the Direct3D 9 swap chain instance
		// -> It appears that receiving and manually accessing the automatic depth stencil surface instance is not possible, so, we don't use the automatic depth stencil thing
		D3DPRESENT_PARAMETERS d3dPresentParameters = {};
		d3dPresentParameters.BackBufferWidth		= static_cast<UINT>(width);
		d3dPresentParameters.BackBufferHeight		= static_cast<UINT>(height);
		d3dPresentParameters.BackBufferCount		= 1;
		d3dPresentParameters.SwapEffect				= D3DSWAPEFFECT_DISCARD;
		d3dPresentParameters.hDeviceWindow			= hWnd;
		d3dPresentParameters.Windowed				= TRUE;
		d3dPresentParameters.EnableAutoDepthStencil = FALSE;

		// Create the Direct3D 9 swap chain
		// -> Direct3D 9 now also automatically fills the given present parameters instance with the chosen settings
		if (SUCCEEDED(direct3DDevice9->CreateAdditionalSwapChain(&d3dPresentParameters, &mDirect3DSwapChain9)))
		{
			// Get the Direct3D 9 render target surface instance
			mDirect3DSwapChain9->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &mDirect3DSurface9RenderTarget);

			// Create the Direct3D 9 depth stencil surface
			direct3DDevice9->CreateDepthStencilSurface(d3dPresentParameters.BackBufferWidth, d3dPresentParameters.BackBufferHeight,
				D3DFMT_D24S8, d3dPresentParameters.MultiSampleType, d3dPresentParameters.MultiSampleQuality, FALSE,
				&mDirect3DSurface9DepthStencil, nullptr);
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			setDebugName("Swap chain");
		#endif
	}

	SwapChain::~SwapChain()
	{
		// Release the used resources
		if (nullptr != mDirect3DSurface9DepthStencil)
		{
			mDirect3DSurface9DepthStencil->Release();
			mDirect3DSurface9DepthStencil = nullptr;
		}
		if (nullptr != mDirect3DSurface9RenderTarget)
		{
			mDirect3DSurface9RenderTarget->Release();
			mDirect3DSurface9RenderTarget = nullptr;
		}
		if (nullptr != mDirect3DSwapChain9)
		{
			mDirect3DSwapChain9->Release();
			mDirect3DSwapChain9 = nullptr;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void SwapChain::setDebugName(const char *name)
	{
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			// "IDirect3DSwapChain9" is not derived from "IDirect3DResource9", meaning we can't use the "IDirect3DResource9::SetPrivateData()"-method

			// Assign a debug name to the Direct3D 9 render target surface
			if (nullptr != mDirect3DSurface9RenderTarget)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mDirect3DSurface9RenderTarget->SetPrivateData(WKPDID_D3DDebugObjectName, nullptr, 0, 0);
				mDirect3DSurface9RenderTarget->SetPrivateData(WKPDID_D3DDebugObjectName, name, static_cast<UINT>(strlen(name)), 0);
			}

			// Assign a debug name to the Direct3D 9 depth stencil surface
			if (nullptr != mDirect3DSurface9DepthStencil)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mDirect3DSurface9DepthStencil->SetPrivateData(WKPDID_D3DDebugObjectName, nullptr, 0, 0);
				mDirect3DSurface9DepthStencil->SetPrivateData(WKPDID_D3DDebugObjectName, name, static_cast<UINT>(strlen(name)), 0);
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void SwapChain::getWidthAndHeight(uint32_t &width, uint32_t &height) const
	{
		// Is there a valid swap chain?
		if (nullptr != mDirect3DSwapChain9)
		{
			// Get the Direct3D 9 present parameters
			D3DPRESENT_PARAMETERS d3dPresentParameters;
			mDirect3DSwapChain9->GetPresentParameters(&d3dPresentParameters);

			// Get the width and height
			long swapChainWidth  = 1;
			long swapChainHeight = 1;
			{
				// Get the client rectangle of the native output window
				// -> Don't use the width and height stored in "DXGI_SWAP_CHAIN_DESC" -> "DXGI_MODE_DESC"
				//    because it might have been modified in order to avoid zero values
				RECT rect;
				::GetClientRect(d3dPresentParameters.hDeviceWindow, &rect);

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
		{
			// Set known default return values
			width  = 1;
			height = 1;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ISwapChain methods           ]
	//[-------------------------------------------------------]
	handle SwapChain::getNativeWindowHandle() const
	{
		// Is there a valid swap chain?
		if (nullptr != mDirect3DSwapChain9)
		{
			// Get the Direct3D 9 present parameters
			D3DPRESENT_PARAMETERS d3dPresentParameters;
			mDirect3DSwapChain9->GetPresentParameters(&d3dPresentParameters);

			// Return the native window handle
			return reinterpret_cast<handle>(d3dPresentParameters.hDeviceWindow);
		}

		// Error!
		return NULL_HANDLE;
	}

	void SwapChain::present()
	{
		// Is there a valid swap chain?
		if (nullptr != mDirect3DSwapChain9)
		{
			mDirect3DSwapChain9->Present(nullptr, nullptr, nullptr, nullptr, 0);
		}
	}

	void SwapChain::resizeBuffers()
	{
		// Is there a valid swap chain?
		if (nullptr != mDirect3DSwapChain9)
		{
			// Get the Direct3D 9 device instance
			IDirect3DDevice9 *direct3DDevice9 = nullptr;
			mDirect3DSwapChain9->GetDevice(&direct3DDevice9);

			// Get the Direct3D 9 present parameters to query the native window handle
			D3DPRESENT_PARAMETERS d3dPresentParameters;
			mDirect3DSwapChain9->GetPresentParameters(&d3dPresentParameters);
			const HWND nativeWindowHandle = d3dPresentParameters.hDeviceWindow;

			// Get the swap chain width and height, ensures they are never ever zero
			UINT width  = 1;
			UINT height = 1;
			getSafeWidthAndHeight(width, height);

			// Get the currently set render target
			Renderer::IRenderTarget *renderTargetBackup = static_cast<Direct3D9Renderer&>(getRenderer()).omGetRenderTarget();

			// In case this swap chain is the current render target, we have to unset it before continuing
			if (this == renderTargetBackup)
			{
				static_cast<Direct3D9Renderer&>(getRenderer()).omSetRenderTarget(nullptr);
			}
			else
			{
				renderTargetBackup = nullptr;
			}

			// Release the surfaces
			if (nullptr != mDirect3DSurface9DepthStencil)
			{
				mDirect3DSurface9DepthStencil->Release();
				mDirect3DSurface9DepthStencil = nullptr;
			}
			if (nullptr != mDirect3DSurface9RenderTarget)
			{
				mDirect3DSurface9RenderTarget->Release();
				mDirect3DSurface9RenderTarget = nullptr;
			}
			if (nullptr != mDirect3DSwapChain9)
			{
				mDirect3DSwapChain9->Release();
				mDirect3DSwapChain9 = nullptr;
			}

			// Set up the structure used to create the Direct3D 9 swap chain instance
			// -> It appears that receiving and manually accessing the automatic depth stencil surface instance is not possible, so, we don't use the automatic depth stencil thing
			::ZeroMemory(&d3dPresentParameters, sizeof(D3DPRESENT_PARAMETERS));
			d3dPresentParameters.BackBufferWidth		= width;
			d3dPresentParameters.BackBufferHeight		= height;
			d3dPresentParameters.BackBufferCount		= 1;
			d3dPresentParameters.SwapEffect				= D3DSWAPEFFECT_DISCARD;
			d3dPresentParameters.hDeviceWindow			= nativeWindowHandle;
			d3dPresentParameters.Windowed				= TRUE;
			d3dPresentParameters.EnableAutoDepthStencil = TRUE;
			d3dPresentParameters.AutoDepthStencilFormat = D3DFMT_D24X8;

			// Create the Direct3D 9 swap chain
			// -> Direct3D 9 now also automatically fills the given present parameters instance with the chosen settings
			if (SUCCEEDED(direct3DDevice9->CreateAdditionalSwapChain(&d3dPresentParameters, &mDirect3DSwapChain9)))
			{
				// Get the Direct3D 9 render target surface instance
				mDirect3DSwapChain9->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &mDirect3DSurface9RenderTarget);

				// Create the Direct3D 9 depth stencil surface
				direct3DDevice9->CreateDepthStencilSurface(d3dPresentParameters.BackBufferWidth, d3dPresentParameters.BackBufferHeight,
					D3DFMT_D24S8, d3dPresentParameters.MultiSampleType, d3dPresentParameters.MultiSampleQuality, FALSE,
					&mDirect3DSurface9DepthStencil, nullptr);
			}
		}
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

	void SwapChain::setRenderWindow(Renderer::IRenderWindow*)
	{
		// TODO(sw) implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void SwapChain::getSafeWidthAndHeight(uint32_t &width, uint32_t &height) const
	{
		// Get the Direct3D 9 present parameters
		D3DPRESENT_PARAMETERS d3dPresentParameters;
		mDirect3DSwapChain9->GetPresentParameters(&d3dPresentParameters);

		// Get the client rectangle of the native output window
		RECT rect;
		::GetClientRect(d3dPresentParameters.hDeviceWindow, &rect);

		// Get the width and height...
		long swapChainWidth  = rect.right  - rect.left;
		long swapChainHeight = rect.bottom - rect.top;

		// ... and ensure that none of them is ever zero
		if (swapChainWidth < 1)
		{
			swapChainWidth = 1;
		}
		if (swapChainHeight < 1)
		{
			swapChainHeight = 1;
		}

		// Write out the width and height
		width  = static_cast<UINT>(swapChainWidth);
		height = static_cast<UINT>(swapChainHeight);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
