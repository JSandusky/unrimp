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
#include "Direct3D10Renderer/RenderTarget/SwapChain.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/D3D10.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SwapChain::SwapChain(Direct3D10Renderer &direct3D10Renderer, handle nativeWindowHandle) :
		ISwapChain(direct3D10Renderer),
		mDxgiSwapChain(nullptr),
		mD3D10RenderTargetView(nullptr),
		mD3D10DepthStencilView(nullptr)
	{
		// Get the Direct3D 10 device instance
		ID3D10Device *d3d10Device = direct3D10Renderer.getD3D10Device();

		// Get the native window handle
		const HWND hWnd = reinterpret_cast<HWND>(nativeWindowHandle);

		// Get a DXGI factory instance
		IDXGIDevice *dxgiDevice = nullptr;
		IDXGIAdapter *dxgiAdapter = nullptr;
		IDXGIFactory *dxgiFactory = nullptr;
		d3d10Device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
		dxgiDevice->GetAdapter(&dxgiAdapter);
		dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
		dxgiAdapter->Release();
		dxgiDevice->Release();

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

		// Create the swap chain
		DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc = {};
		dxgiSwapChainDesc.BufferCount						 = 1;
		dxgiSwapChainDesc.BufferDesc.Width					 = static_cast<UINT>(width);
		dxgiSwapChainDesc.BufferDesc.Height					 = static_cast<UINT>(height);
		dxgiSwapChainDesc.BufferDesc.Format					 = DXGI_FORMAT_R8G8B8A8_UNORM;
		dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator	 = 60;
		dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		dxgiSwapChainDesc.BufferUsage						 = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		dxgiSwapChainDesc.OutputWindow						 = hWnd;
		dxgiSwapChainDesc.SampleDesc.Count					 = 1;
		dxgiSwapChainDesc.SampleDesc.Quality				 = 0;
		dxgiSwapChainDesc.Windowed							 = TRUE;
		dxgiFactory->CreateSwapChain(d3d10Device, &dxgiSwapChainDesc, &mDxgiSwapChain);

		// Disable alt-return for automatic fullscreen state change
		// -> We handle this manually to have more control over it
		dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

		// Release our DXGI factory
		dxgiFactory->Release();

		// Create the Direct3D 10 views
		if (nullptr != mDxgiSwapChain)
		{
			createDirect3D10Views();
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			setDebugName("Swap chain");
		#endif
	}

	SwapChain::~SwapChain()
	{
		// "DXGI Overview - Destroying a Swap Chain" at MSDN http://msdn.microsoft.com/en-us/library/bb205075.aspx states
		//   "You may not release a swap chain in full-screen mode because doing so may create thread contention (which will
		//    cause DXGI to raise a non-continuable exception). Before releasing a swap chain, first switch to windowed mode
		//    (using IDXGISwapChain::SetFullscreenState( FALSE, NULL )) and then call IUnknown::Release."
		if (getFullscreenState())
		{
			setFullscreenState(false);
		}

		// Release the used resources
		if (nullptr != mD3D10DepthStencilView)
		{
			mD3D10DepthStencilView->Release();
			mD3D10DepthStencilView = nullptr;
		}
		if (nullptr != mD3D10RenderTargetView)
		{
			mD3D10RenderTargetView->Release();
			mD3D10RenderTargetView = nullptr;
		}
		if (nullptr != mDxgiSwapChain)
		{
			mDxgiSwapChain->Release();
			mDxgiSwapChain = nullptr;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void SwapChain::setDebugName(const char *name)
	{
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			// Assign a debug name to the DXGI swap chain
			if (nullptr != mDxgiSwapChain)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mDxgiSwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mDxgiSwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}

			// Assign a debug name to the Direct3D 10 render target view
			if (nullptr != mD3D10RenderTargetView)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D10RenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10RenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}

			// Assign a debug name to the Direct3D 10 depth stencil view
			if (nullptr != mD3D10DepthStencilView)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D10DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void SwapChain::getWidthAndHeight(uint32_t &width, uint32_t &height) const
	{
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain)
		{
			// Get the Direct3D 10 swap chain description
			DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
			mDxgiSwapChain->GetDesc(&dxgiSwapChainDesc);

			// Get the width and height
			long swapChainWidth  = 1;
			long swapChainHeight = 1;
			{
				// Get the client rectangle of the native output window
				// -> Don't use the width and height stored in "DXGI_SWAP_CHAIN_DESC" -> "DXGI_MODE_DESC"
				//    because it might have been modified in order to avoid zero values
				RECT rect;
				::GetClientRect(dxgiSwapChainDesc.OutputWindow, &rect);

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
		if (nullptr != mDxgiSwapChain)
		{
			// Get the Direct3D 10 swap chain description
			DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
			mDxgiSwapChain->GetDesc(&dxgiSwapChainDesc);

			// Return the native window handle
			return reinterpret_cast<handle>(dxgiSwapChainDesc.OutputWindow);
		}

		// Error!
		return NULL_HANDLE;
	}

	void SwapChain::present()
	{
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain)
		{
			mDxgiSwapChain->Present(0, 0);
		}
	}

	void SwapChain::resizeBuffers()
	{
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain)
		{
			Direct3D10Renderer& direct3D10Renderer = static_cast<Direct3D10Renderer&>(getRenderer());

			// Get the currently set render target
			Renderer::IRenderTarget *renderTargetBackup = direct3D10Renderer.omGetRenderTarget();

			// In case this swap chain is the current render target, we have to unset it before continuing
			if (this == renderTargetBackup)
			{
				direct3D10Renderer.omSetRenderTarget(nullptr);
			}
			else
			{
				renderTargetBackup = nullptr;
			}

			// Release the views
			if (nullptr != mD3D10DepthStencilView)
			{
				mD3D10DepthStencilView->Release();
				mD3D10DepthStencilView = nullptr;
			}
			if (nullptr != mD3D10RenderTargetView)
			{
				mD3D10RenderTargetView->Release();
				mD3D10RenderTargetView = nullptr;
			}

			// Get the swap chain width and height, ensures they are never ever zero
			UINT width  = 1;
			UINT height = 1;
			getSafeWidthAndHeight(width, height);

			// Resize the Direct3D 10 swap chain
			// -> Preserve the existing buffer count and format
			// -> Automatically choose the width and height to match the client rectangle of the native window
			if (SUCCEEDED(mDxgiSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0)))
			{
				// Create the Direct3D 10 views
				createDirect3D10Views();

				// If required, restore the previously set render target
				if (nullptr != renderTargetBackup)
				{
					direct3D10Renderer.omSetRenderTarget(renderTargetBackup);
				}
			}
		}
	}

	bool SwapChain::getFullscreenState() const
	{
		// Window mode by default
		BOOL fullscreen = FALSE;

		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain)
		{
			mDxgiSwapChain->GetFullscreenState(&fullscreen, nullptr);
		}

		// Done
		return (fullscreen != FALSE);
	}

	void SwapChain::setFullscreenState(bool fullscreen)
	{
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain)
		{
			mDxgiSwapChain->SetFullscreenState(fullscreen, nullptr);
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void SwapChain::getSafeWidthAndHeight(uint32_t &width, uint32_t &height) const
	{
		// Get the Direct3D 10 swap chain description
		DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
		mDxgiSwapChain->GetDesc(&dxgiSwapChainDesc);

		// Get the client rectangle of the native output window
		RECT rect;
		::GetClientRect(dxgiSwapChainDesc.OutputWindow, &rect);

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

	void SwapChain::createDirect3D10Views()
	{
		// Create a render target view
		ID3D10Texture2D *d3d10Texture2DBackBuffer = nullptr;
		HRESULT hResult = mDxgiSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<LPVOID*>(&d3d10Texture2DBackBuffer));
		if (SUCCEEDED(hResult))
		{
			// Get the Direct3D 10 device instance
			ID3D10Device *d3d10Device = static_cast<Direct3D10Renderer&>(getRenderer()).getD3D10Device();

			// Create a render target view
			hResult = d3d10Device->CreateRenderTargetView(d3d10Texture2DBackBuffer, nullptr, &mD3D10RenderTargetView);
			d3d10Texture2DBackBuffer->Release();
			if (SUCCEEDED(hResult))
			{
				// Get the swap chain width and height, ensures they are never ever zero
				UINT width  = 1;
				UINT height = 1;
				getSafeWidthAndHeight(width, height);

				// Create depth stencil texture
				ID3D10Texture2D *d3d10Texture2DDepthStencil = nullptr;
				D3D10_TEXTURE2D_DESC d3d10Texture2DDesc = {};
				d3d10Texture2DDesc.Width			  = width;
				d3d10Texture2DDesc.Height			  = height;
				d3d10Texture2DDesc.MipLevels		  = 1;
				d3d10Texture2DDesc.ArraySize		  = 1;
				d3d10Texture2DDesc.Format			  = DXGI_FORMAT_D24_UNORM_S8_UINT;
				d3d10Texture2DDesc.SampleDesc.Count   = 1;
				d3d10Texture2DDesc.SampleDesc.Quality = 0;
				d3d10Texture2DDesc.Usage			  = D3D10_USAGE_DEFAULT;
				d3d10Texture2DDesc.BindFlags		  = D3D10_BIND_DEPTH_STENCIL;
				d3d10Texture2DDesc.CPUAccessFlags	  = 0;
				d3d10Texture2DDesc.MiscFlags		  = 0;
				hResult = d3d10Device->CreateTexture2D(&d3d10Texture2DDesc, nullptr, &d3d10Texture2DDepthStencil);
				if (SUCCEEDED(hResult))
				{
					// Create the depth stencil view
					D3D10_DEPTH_STENCIL_VIEW_DESC d3d10DepthStencilViewDesc = {};
					d3d10DepthStencilViewDesc.Format			 = d3d10Texture2DDesc.Format;
					d3d10DepthStencilViewDesc.ViewDimension		 = D3D10_DSV_DIMENSION_TEXTURE2D;
					d3d10DepthStencilViewDesc.Texture2D.MipSlice = 0;
					d3d10Device->CreateDepthStencilView(d3d10Texture2DDepthStencil, &d3d10DepthStencilViewDesc, &mD3D10DepthStencilView);
					d3d10Texture2DDepthStencil->Release();
				}
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
