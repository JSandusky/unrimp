/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "Direct3D11Renderer/SwapChain.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/D3D11.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SwapChain::SwapChain(Direct3D11Renderer &direct3D11Renderer, handle nativeWindowHandle) :
		ISwapChain(direct3D11Renderer),
		mDxgiSwapChain(nullptr),
		mD3D11RenderTargetView(nullptr),
		mD3D11DepthStencilView(nullptr)
	{
		// Get the Direct3D 11 device instance
		ID3D11Device *d3d11Device = direct3D11Renderer.getD3D11Device();

		// Get the native window handle
		const HWND hWnd = reinterpret_cast<HWND>(nativeWindowHandle);

		// Get a IDXGIFactory1 instance
		IDXGIDevice *dxgiDevice = nullptr;
		IDXGIAdapter *dxgiAdapter = nullptr;
		IDXGIFactory1 *dxgiFactory1 = nullptr;
		d3d11Device->QueryInterface(&dxgiDevice);
		dxgiDevice->GetAdapter(&dxgiAdapter);
		dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory1));
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
		DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
		::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
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
		dxgiFactory1->CreateSwapChain(d3d11Device, &dxgiSwapChainDesc, &mDxgiSwapChain);
		dxgiFactory1->Release();

		// Disable alt-return for automatic fullscreen state change
		// -> We handle this manually to have more control over it
		dxgiFactory1->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

		// Create the Direct3D 11 views
		if (nullptr != mDxgiSwapChain)
		{
			createDirect3D11Views();
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
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
		if (nullptr != mD3D11DepthStencilView)
		{
			mD3D11DepthStencilView->Release();
			mD3D11DepthStencilView = nullptr;
		}
		if (nullptr != mD3D11RenderTargetView)
		{
			mD3D11RenderTargetView->Release();
			mD3D11RenderTargetView = nullptr;
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
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			// Assign a debug name to the DXGI swap chain
			if (nullptr != mDxgiSwapChain)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mDxgiSwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mDxgiSwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}

			// Assign a debug name to the Direct3D 11 render target view
			if (nullptr != mD3D11RenderTargetView)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D11RenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11RenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}

			// Assign a debug name to the Direct3D 11 depth stencil view
			if (nullptr != mD3D11DepthStencilView)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D11DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
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
			// Get the Direct3D 11 swap chain description
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
			width  = 0;
			height = 0;
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
			// Get the Direct3D 11 swap chain description
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
			// Get the currently set render target
			Renderer::IRenderTarget *renderTargetBackup = getRenderer().omGetRenderTarget();

			// In case this swap chain is the current render target, we have to unset it before continuing
			if (this == renderTargetBackup)
			{
				getRenderer().omSetRenderTarget(nullptr);
			}
			else
			{
				renderTargetBackup = nullptr;
			}

			// Release the views
			if (nullptr != mD3D11DepthStencilView)
			{
				mD3D11DepthStencilView->Release();
				mD3D11DepthStencilView = nullptr;
			}
			if (nullptr != mD3D11RenderTargetView)
			{
				mD3D11RenderTargetView->Release();
				mD3D11RenderTargetView = nullptr;
			}

			// Get the swap chain width and height, ensures they are never ever zero
			UINT width  = 1;
			UINT height = 1;
			getSafeWidthAndHeight(width, height);

			// Resize the Direct3D 11 swap chain
			// -> Preserve the existing buffer count and format
			if (SUCCEEDED(mDxgiSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0)))
			{
				// Create the Direct3D 11 views
				createDirect3D11Views();

				// If required, restore the previously set render target
				if (nullptr != renderTargetBackup)
				{
					getRenderer().omSetRenderTarget(renderTargetBackup);
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
		// Get the Direct3D 11 swap chain description
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

	void SwapChain::createDirect3D11Views()
	{
		// Create a render target view
		ID3D11Texture2D *d3d11Texture2DBackBuffer = nullptr;
		HRESULT hResult = mDxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&d3d11Texture2DBackBuffer));
		if (SUCCEEDED(hResult))
		{
			// Get the Direct3D 11 device instance
			ID3D11Device *d3d11Device = static_cast<Direct3D11Renderer&>(getRenderer()).getD3D11Device();

			// Create a render target view
			hResult = d3d11Device->CreateRenderTargetView(d3d11Texture2DBackBuffer, nullptr, &mD3D11RenderTargetView);
			d3d11Texture2DBackBuffer->Release();
			if (SUCCEEDED(hResult))
			{
				// Get the swap chain width and height, ensures they are never ever zero
				UINT width  = 1;
				UINT height = 1;
				getSafeWidthAndHeight(width, height);

				// Create depth stencil texture
				ID3D11Texture2D *d3d11Texture2DDepthStencil = nullptr;
				D3D11_TEXTURE2D_DESC d3d11Texture2DDesc;
				::ZeroMemory(&d3d11Texture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
				d3d11Texture2DDesc.Width			  = width;
				d3d11Texture2DDesc.Height			  = height;
				d3d11Texture2DDesc.MipLevels		  = 1;
				d3d11Texture2DDesc.ArraySize		  = 1;
				d3d11Texture2DDesc.Format			  = DXGI_FORMAT_D24_UNORM_S8_UINT;
				d3d11Texture2DDesc.SampleDesc.Count   = 1;
				d3d11Texture2DDesc.SampleDesc.Quality = 0;
				d3d11Texture2DDesc.Usage			  = D3D11_USAGE_DEFAULT;
				d3d11Texture2DDesc.BindFlags		  = D3D11_BIND_DEPTH_STENCIL;
				d3d11Texture2DDesc.CPUAccessFlags	  = 0;
				d3d11Texture2DDesc.MiscFlags		  = 0;
				hResult = d3d11Device->CreateTexture2D(&d3d11Texture2DDesc, nullptr, &d3d11Texture2DDepthStencil);
				if (SUCCEEDED(hResult))
				{
					// Create the depth stencil view
					D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DepthStencilViewDesc;
					::ZeroMemory(&d3d11DepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
					d3d11DepthStencilViewDesc.Format			 = d3d11Texture2DDesc.Format;
					d3d11DepthStencilViewDesc.ViewDimension		 = D3D11_DSV_DIMENSION_TEXTURE2D;
					d3d11DepthStencilViewDesc.Texture2D.MipSlice = 0;
					d3d11Device->CreateDepthStencilView(d3d11Texture2DDepthStencil, &d3d11DepthStencilViewDesc, &mD3D11DepthStencilView);
					d3d11Texture2DDepthStencil->Release();
				}
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
