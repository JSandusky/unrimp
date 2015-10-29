/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "Direct3D12Renderer/SwapChain.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/D3D12.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SwapChain::SwapChain(Direct3D12Renderer &direct3D12Renderer, handle) :
		ISwapChain(direct3D12Renderer)
		// TODO(co) Direct3D 12 update
	//	mDxgiSwapChain(nullptr),
	//	mD3D12RenderTargetView(nullptr),
	//	mD3D12DepthStencilView(nullptr)
	{
		// TODO(co) Direct3D 12 update
		/*
		// Get the Direct3D 12 device instance
		ID3D12Device *d3d12Device = direct3D12Renderer.getD3D12Device();

		// Get the native window handle
		const HWND hWnd = reinterpret_cast<HWND>(nativeWindowHandle);

		// Get a IDXGIFactory1 instance
		IDXGIDevice *dxgiDevice = nullptr;
		IDXGIAdapter *dxgiAdapter = nullptr;
		IDXGIFactory1 *dxgiFactory1 = nullptr;
		d3d12Device->QueryInterface(&dxgiDevice);
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
		dxgiFactory1->CreateSwapChain(d3d12Device, &dxgiSwapChainDesc, &mDxgiSwapChain);
		dxgiFactory1->Release();

		// Disable alt-return for automatic fullscreen state change
		// -> We handle this manually to have more control over it
		dxgiFactory1->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

		// Create the Direct3D 12 views
		if (nullptr != mDxgiSwapChain)
		{
			createDirect3D12Views();
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("Swap chain");
		#endif
		*/
	}

	SwapChain::~SwapChain()
	{
		// TODO(co) Direct3D 12 update
		/*
		// "DXGI Overview - Destroying a Swap Chain" at MSDN http://msdn.microsoft.com/en-us/library/bb205075.aspx states
		//   "You may not release a swap chain in full-screen mode because doing so may create thread contention (which will
		//    cause DXGI to raise a non-continuable exception). Before releasing a swap chain, first switch to windowed mode
		//    (using IDXGISwapChain::SetFullscreenState( FALSE, NULL )) and then call IUnknown::Release."
		if (getFullscreenState())
		{
			setFullscreenState(false);
		}

		// Release the used resources
		if (nullptr != mD3D12DepthStencilView)
		{
			mD3D12DepthStencilView->Release();
			mD3D12DepthStencilView = nullptr;
		}
		if (nullptr != mD3D12RenderTargetView)
		{
			mD3D12RenderTargetView->Release();
			mD3D12RenderTargetView = nullptr;
		}
		if (nullptr != mDxgiSwapChain)
		{
			mDxgiSwapChain->Release();
			mDxgiSwapChain = nullptr;
		}
		*/
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void SwapChain::setDebugName(const char *)
	{
		// TODO(co) Direct3D 12 update
		/*
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			// Assign a debug name to the DXGI swap chain
			if (nullptr != mDxgiSwapChain)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mDxgiSwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mDxgiSwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}

			// Assign a debug name to the Direct3D 12 render target view
			if (nullptr != mD3D12RenderTargetView)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D12RenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12RenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}

			// Assign a debug name to the Direct3D 12 depth stencil view
			if (nullptr != mD3D12DepthStencilView)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D12DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
		*/
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void SwapChain::getWidthAndHeight(uint32_t &, uint32_t &) const
	{
		// TODO(co) Direct3D 12 update
		/*
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain)
		{
			// Get the Direct3D 12 swap chain description
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
		*/
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ISwapChain methods           ]
	//[-------------------------------------------------------]
	handle SwapChain::getNativeWindowHandle() const
	{
		// TODO(co) Direct3D 12 update
		/*
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain)
		{
			// Get the Direct3D 12 swap chain description
			DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
			mDxgiSwapChain->GetDesc(&dxgiSwapChainDesc);

			// Return the native window handle
			return reinterpret_cast<handle>(dxgiSwapChainDesc.OutputWindow);
		}
		*/
		// Error!
		return NULL_HANDLE;
	}

	void SwapChain::present()
	{
		// TODO(co) Direct3D 12 update
		/*
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain)
		{
			mDxgiSwapChain->Present(0, 0);
		}
		*/
	}

	void SwapChain::resizeBuffers()
	{
		// TODO(co) Direct3D 12 update
		/*
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
			if (nullptr != mD3D12DepthStencilView)
			{
				mD3D12DepthStencilView->Release();
				mD3D12DepthStencilView = nullptr;
			}
			if (nullptr != mD3D12RenderTargetView)
			{
				mD3D12RenderTargetView->Release();
				mD3D12RenderTargetView = nullptr;
			}

			// Get the swap chain width and height, ensures they are never ever zero
			UINT width  = 1;
			UINT height = 1;
			getSafeWidthAndHeight(width, height);

			// Resize the Direct3D 12 swap chain
			// -> Preserve the existing buffer count and format
			if (SUCCEEDED(mDxgiSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0)))
			{
				// Create the Direct3D 12 views
				createDirect3D12Views();

				// If required, restore the previously set render target
				if (nullptr != renderTargetBackup)
				{
					getRenderer().omSetRenderTarget(renderTargetBackup);
				}
			}
		}
		*/
	}

	bool SwapChain::getFullscreenState() const
	{
		// TODO(co) Direct3D 12 update
		/*
		// Window mode by default
		BOOL fullscreen = FALSE;

		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain)
		{
			mDxgiSwapChain->GetFullscreenState(&fullscreen, nullptr);
		}

		// Done
		return (fullscreen != FALSE);
		*/
		return false;
	}

	void SwapChain::setFullscreenState(bool)
	{
		// TODO(co) Direct3D 12 update
		/*
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain)
		{
			mDxgiSwapChain->SetFullscreenState(fullscreen, nullptr);
		}
		*/
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void SwapChain::getSafeWidthAndHeight(uint32_t &, uint32_t &) const
	{
		// TODO(co) Direct3D 12 update
		/*
		// Get the Direct3D 12 swap chain description
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
		*/
	}

	void SwapChain::createDirect3D12Views()
	{
		// TODO(co) Direct3D 12 update
		/*
		// Create a render target view
		ID3D12Texture2D *d3d12Texture2DBackBuffer = nullptr;
		HRESULT hResult = mDxgiSwapChain->GetBuffer(0, __uuidof(ID3D12Texture2D), reinterpret_cast<LPVOID*>(&d3d12Texture2DBackBuffer));
		if (SUCCEEDED(hResult))
		{
			// Get the Direct3D 12 device instance
			ID3D12Device *d3d12Device = static_cast<Direct3D12Renderer&>(getRenderer()).getD3D12Device();

			// Create a render target view
			hResult = d3d12Device->CreateRenderTargetView(d3d12Texture2DBackBuffer, nullptr, &mD3D12RenderTargetView);
			d3d12Texture2DBackBuffer->Release();
			if (SUCCEEDED(hResult))
			{
				// Get the swap chain width and height, ensures they are never ever zero
				UINT width  = 1;
				UINT height = 1;
				getSafeWidthAndHeight(width, height);

				// Create depth stencil texture
				ID3D12Texture2D *d3d12Texture2DDepthStencil = nullptr;
				D3D12_TEXTURE2D_DESC d3d12Texture2DDesc;
				::ZeroMemory(&d3d12Texture2DDesc, sizeof(D3D12_TEXTURE2D_DESC));
				d3d12Texture2DDesc.Width			  = width;
				d3d12Texture2DDesc.Height			  = height;
				d3d12Texture2DDesc.MipLevels		  = 1;
				d3d12Texture2DDesc.ArraySize		  = 1;
				d3d12Texture2DDesc.Format			  = DXGI_FORMAT_D24_UNORM_S8_UINT;
				d3d12Texture2DDesc.SampleDesc.Count   = 1;
				d3d12Texture2DDesc.SampleDesc.Quality = 0;
				d3d12Texture2DDesc.Usage			  = D3D12_USAGE_DEFAULT;
				d3d12Texture2DDesc.BindFlags		  = D3D12_BIND_DEPTH_STENCIL;
				d3d12Texture2DDesc.CPUAccessFlags	  = 0;
				d3d12Texture2DDesc.MiscFlags		  = 0;
				hResult = d3d12Device->CreateTexture2D(&d3d12Texture2DDesc, nullptr, &d3d12Texture2DDepthStencil);
				if (SUCCEEDED(hResult))
				{
					// Create the depth stencil view
					D3D12_DEPTH_STENCIL_VIEW_DESC d3d12DepthStencilViewDesc;
					::ZeroMemory(&d3d12DepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
					d3d12DepthStencilViewDesc.Format			 = d3d12Texture2DDesc.Format;
					d3d12DepthStencilViewDesc.ViewDimension		 = D3D12_DSV_DIMENSION_TEXTURE2D;
					d3d12DepthStencilViewDesc.Texture2D.MipSlice = 0;
					d3d12Device->CreateDepthStencilView(d3d12Texture2DDepthStencil, &d3d12DepthStencilViewDesc, &mD3D12DepthStencilView);
					d3d12Texture2DDepthStencil->Release();
				}
			}
		}
		*/
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
