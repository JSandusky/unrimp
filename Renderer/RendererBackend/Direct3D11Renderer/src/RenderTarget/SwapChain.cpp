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
#include "Direct3D11Renderer/RenderTarget/SwapChain.h"
#include "Direct3D11Renderer/RenderTarget/RenderPass.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/D3D11.h"
#include "Direct3D11Renderer/Mapping.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"

#include <Renderer/ILog.h>

#include <VersionHelpers.h>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		typedef LONG NTSTATUS, *PNTSTATUS;
		typedef NTSTATUS (WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		// From https://stackoverflow.com/a/36545162
		RTL_OSVERSIONINFOW getRealOSVersion()
		{
			const HMODULE hModule = ::GetModuleHandleW(L"ntdll.dll");
			if (hModule)
			{
				PRAGMA_WARNING_PUSH
					PRAGMA_WARNING_DISABLE_MSVC(4191)	// warning C4191: 'reinterpret_cast': unsafe conversion from 'FARPROC' to '`anonymous-namespace'::detail::RtlGetVersionPtr'
					const RtlGetVersionPtr functionPointer = reinterpret_cast<RtlGetVersionPtr>(::GetProcAddress(hModule, "RtlGetVersion"));
				PRAGMA_WARNING_POP
				if (nullptr != functionPointer)
				{
					RTL_OSVERSIONINFOW rovi = { 0 };
					rovi.dwOSVersionInfoSize = sizeof(rovi);
					if (0x00000000 == functionPointer(&rovi))
					{
						return rovi;
					}
				}
			}
			RTL_OSVERSIONINFOW rovi = { 0 };
			return rovi;
		}

		// "IsWindows10OrGreater()" isn't practically usable
		// - See "Windows Dev Center" -> "Version Helper functions" -> "IsWindows10OrGreater" at https://msdn.microsoft.com/en-us/library/windows/desktop/dn424972(v=vs.85).aspx
		//   "For Windows 10, IsWindows10OrGreater returns false unless the application contains a manifest that includes a compatibility section that contains the GUID that designates Windows 10."
		bool isWindows10OrGreater()
		{
			return (getRealOSVersion().dwMajorVersion >= 10);
		}

		void handleDeviceLost(const Direct3D11Renderer::Direct3D11Renderer& direct3D11Renderer, HRESULT result)
		{
			// If the device was removed either by a disconnection or a driver upgrade, we must recreate all device resources
			if (DXGI_ERROR_DEVICE_REMOVED == result || DXGI_ERROR_DEVICE_RESET == result)
			{
				if (DXGI_ERROR_DEVICE_REMOVED == result)
				{
					result = direct3D11Renderer.getD3D11Device()->GetDeviceRemovedReason();
				}
				RENDERER_LOG(direct3D11Renderer.getContext(), CRITICAL, "Direct3D 11 device lost on present: Reason code 0x%08X", static_cast<unsigned int>(result))

				// TODO(co) Add device lost handling if needed. Probably more complex to recreate all device resources.
			}
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SwapChain::SwapChain(Renderer::IRenderPass& renderPass, Renderer::WindowHandle windowHandle) :
		ISwapChain(renderPass),
		mD3D11DeviceContext1(nullptr),
		mDxgiSwapChain(nullptr),
		mD3D11RenderTargetView(nullptr),
		mD3D11DepthStencilView(nullptr),
		mSynchronizationInterval(0),
		mAllowTearing(false)
	{
		const RenderPass& d3d11RenderPass = static_cast<RenderPass&>(renderPass);
		const Direct3D11Renderer& direct3D11Renderer = static_cast<Direct3D11Renderer&>(d3d11RenderPass.getRenderer());
		direct3D11Renderer.getD3D11DeviceContext()->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&mD3D11DeviceContext1));

		// Sanity check
		assert(1 == d3d11RenderPass.getNumberOfColorAttachments());

		// Get the Direct3D 11 device instance
		ID3D11Device* d3d11Device = direct3D11Renderer.getD3D11Device();

		// Get the native window handle
		const HWND hWnd = reinterpret_cast<HWND>(windowHandle.nativeWindowHandle);

		// Get a DXGI factory instance
		const bool isWindows10OrGreater = ::detail::isWindows10OrGreater();
		IDXGIFactory1* dxgiFactory1 = nullptr;
		IDXGIFactory2* dxgiFactory2 = nullptr;
		{
			IDXGIDevice* dxgiDevice = nullptr;
			IDXGIAdapter* dxgiAdapter = nullptr;
			d3d11Device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
			dxgiDevice->GetAdapter(&dxgiAdapter);
			dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory1));
			dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory2));

			// Determines whether tearing support is available for fullscreen borderless windows
			// -> To unlock frame rates of UWP applications on the Windows Store and providing support for both AMD Freesync and NVIDIA's G-SYNC we must explicitly allow tearing
			// -> See "Windows Dev Center" -> "Variable refresh rate displays": https://msdn.microsoft.com/en-us/library/windows/desktop/mt742104(v=vs.85).aspx
			if (isWindows10OrGreater)
			{
				IDXGIFactory5* dxgiFactory5 = nullptr;
				dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory5));
				if (nullptr != dxgiFactory5)
				{
					BOOL allowTearing = FALSE;
					if (SUCCEEDED(dxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))))
					{
						mAllowTearing = true;
					}
					dxgiFactory5->Release();
				}
			}

			// Release references
			dxgiAdapter->Release();
			dxgiDevice->Release();
		}

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

		{ // Create the swap chain
			UINT bufferCount = 1;
			DXGI_SWAP_EFFECT swapEffect = DXGI_SWAP_EFFECT_DISCARD;
			const bool isWindows8OrGreater = ::IsWindows8OrGreater();
			if (isWindows10OrGreater)
			{
				assert((d3d11RenderPass.getNumberOfMultisamples() == 1) && "Direct3D 11 doesn't support multisampling if the flip model vertical synchronization is used");
				bufferCount = 2;
				swapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			}
			else if (isWindows8OrGreater)
			{
				assert((d3d11RenderPass.getNumberOfMultisamples() == 1) && "Direct3D 11 doesn't support multisampling if the flip model vertical synchronization is used");
				bufferCount = 2;
				swapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			}

			// Quote from https://msdn.microsoft.com/de-de/library/windows/desktop/hh404557(v=vs.85).aspx : "Platform Update for Windows 7:  DXGI_SCALING_NONE is not supported on Windows 7 or Windows Server 2008 R2"
			if (direct3D11Renderer.getD3DFeatureLevel() == D3D_FEATURE_LEVEL_11_1 && nullptr != dxgiFactory2 && isWindows8OrGreater)
			{
				// Fill DXGI swap chain description
				DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc1 = {};
				dxgiSwapChainDesc1.Width			  = static_cast<UINT>(width);
				dxgiSwapChainDesc1.Height			  = static_cast<UINT>(height);
				dxgiSwapChainDesc1.Format			  = static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(d3d11RenderPass.getColorAttachmentTextureFormat(0)));
				dxgiSwapChainDesc1.SampleDesc.Count	  = 1;
				dxgiSwapChainDesc1.SampleDesc.Quality = 0;
				dxgiSwapChainDesc1.BufferUsage		  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				dxgiSwapChainDesc1.BufferCount		  = bufferCount;
				dxgiSwapChainDesc1.SwapEffect		  = swapEffect;
				dxgiSwapChainDesc1.Flags			  = mAllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

				// Fill DXGI swap chain fullscreen description
				DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullscreenDesc = {};
				dxgiSwapChainFullscreenDesc.RefreshRate.Numerator	= 60;
				dxgiSwapChainFullscreenDesc.RefreshRate.Denominator	= 1;
				dxgiSwapChainFullscreenDesc.Windowed				= TRUE;

				// Create swap chain
				IDXGISwapChain1* dxgiSwapChain = nullptr;
				dxgiFactory2->CreateSwapChainForHwnd(d3d11Device, hWnd, &dxgiSwapChainDesc1, &dxgiSwapChainFullscreenDesc, nullptr, &dxgiSwapChain);
				mDxgiSwapChain = reinterpret_cast<IDXGISwapChain*>(dxgiSwapChain);
			}
			else
			{
				// Fill DXGI swap chain description
				DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc = {};
				dxgiSwapChainDesc.BufferCount						 = bufferCount;
				dxgiSwapChainDesc.BufferDesc.Width					 = static_cast<UINT>(width);
				dxgiSwapChainDesc.BufferDesc.Height					 = static_cast<UINT>(height);
				dxgiSwapChainDesc.BufferDesc.Format					 = static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(d3d11RenderPass.getColorAttachmentTextureFormat(0)));
				dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator	 = 60;
				dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
				dxgiSwapChainDesc.BufferUsage						 = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				dxgiSwapChainDesc.OutputWindow						 = hWnd;
				dxgiSwapChainDesc.SampleDesc.Count					 = 1;
				dxgiSwapChainDesc.SampleDesc.Quality				 = 0;
				dxgiSwapChainDesc.Windowed							 = TRUE;
				dxgiSwapChainDesc.SwapEffect						 = swapEffect;
				dxgiSwapChainDesc.Flags								 = mAllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

				// Create swap chain
				dxgiFactory1->CreateSwapChain(d3d11Device, &dxgiSwapChainDesc, &mDxgiSwapChain);
			}
		}

		// Disable alt-return for automatic fullscreen state change
		// -> We handle this manually to have more control over it
		dxgiFactory1->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

		// Release our DXGI factory
		dxgiFactory1->Release();
		dxgiFactory2->Release();

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
		}
		if (nullptr != mD3D11RenderTargetView)
		{
			mD3D11RenderTargetView->Release();
		}
		if (nullptr != mDxgiSwapChain)
		{
			mDxgiSwapChain->Release();
		}
		if (nullptr != mD3D11DeviceContext1)
		{
			mD3D11DeviceContext1->Release();
		}

		// After releasing references to these resources, we need to call "Flush()" to ensure that Direct3D also releases any references it might still have to the same resources - such as pipeline bindings
		static_cast<Direct3D11Renderer&>(getRenderer()).getD3D11DeviceContext()->Flush();
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void SwapChain::setDebugName(const char* name)
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			// Assign a debug name to the DXGI swap chain
			if (nullptr != mDxgiSwapChain)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mDxgiSwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mDxgiSwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}

			// Assign a debug name to the Direct3D 11 render target view
			if (nullptr != mD3D11RenderTargetView)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D11RenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11RenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}

			// Assign a debug name to the Direct3D 11 depth stencil view
			if (nullptr != mD3D11DepthStencilView)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D11DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void SwapChain::getWidthAndHeight(uint32_t& width, uint32_t& height) const
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
			// Get the Direct3D 11 swap chain description
			DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
			mDxgiSwapChain->GetDesc(&dxgiSwapChainDesc);

			// Return the native window handle
			return reinterpret_cast<handle>(dxgiSwapChainDesc.OutputWindow);
		}

		// Error!
		return NULL_HANDLE;
	}

	void SwapChain::setVerticalSynchronizationInterval(uint32_t synchronizationInterval)
	{
		mSynchronizationInterval = synchronizationInterval;
	}

	void SwapChain::present()
	{
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain)
		{
			// TODO(co) "!getFullscreenState()": Add support for borderless window to get rid of this
			const Direct3D11Renderer& direct3D11Renderer = static_cast<Direct3D11Renderer&>(getRenderPass().getRenderer());
			const UINT flags = ((mAllowTearing && 0 == mSynchronizationInterval && !getFullscreenState()) ? DXGI_PRESENT_ALLOW_TEARING : 0);
			::detail::handleDeviceLost(direct3D11Renderer, mDxgiSwapChain->Present(mSynchronizationInterval, flags));

			// Discard the contents of the render target
			// -> This is a valid operation only when the existing contents will be entirely overwritten. If dirty or scroll rectangles are used, this call should be removed.
			if (nullptr != mD3D11DeviceContext1)
			{
				mD3D11DeviceContext1->DiscardView(mD3D11RenderTargetView);
				if (nullptr != mD3D11DepthStencilView)
				{
					// Discard the contents of the depth stencil
					mD3D11DeviceContext1->DiscardView(mD3D11DepthStencilView);
				}
			}
		}
	}

	void SwapChain::resizeBuffers()
	{
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain)
		{
			Direct3D11Renderer& direct3D11Renderer = static_cast<Direct3D11Renderer&>(getRenderer());

			// Get the currently set render target
			Renderer::IRenderTarget* renderTargetBackup = direct3D11Renderer.omGetRenderTarget();

			// In case this swap chain is the current render target, we have to unset it before continuing
			if (this == renderTargetBackup)
			{
				direct3D11Renderer.omSetRenderTarget(nullptr);
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
			const HRESULT result = mDxgiSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, mAllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u);
			if (SUCCEEDED(result))
			{
				// Create the Direct3D 11 views
				createDirect3D11Views();

				// If required, restore the previously set render target
				if (nullptr != renderTargetBackup)
				{
					direct3D11Renderer.omSetRenderTarget(renderTargetBackup);
				}
			}
			else
			{
				::detail::handleDeviceLost(direct3D11Renderer, result);
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
		return (FALSE != fullscreen);
	}

	void SwapChain::setFullscreenState(bool fullscreen)
	{
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain)
		{
			mDxgiSwapChain->SetFullscreenState(fullscreen, nullptr);
		}
	}

	void SwapChain::setRenderWindow(Renderer::IRenderWindow*)
	{
		// TODO(sw) implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void SwapChain::getSafeWidthAndHeight(uint32_t& width, uint32_t& height) const
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
		ID3D11Texture2D* d3d11Texture2DBackBuffer = nullptr;
		HRESULT hResult = mDxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&d3d11Texture2DBackBuffer));
		if (SUCCEEDED(hResult))
		{
			// Get the Direct3D 11 device instance
			ID3D11Device* d3d11Device = static_cast<Direct3D11Renderer&>(getRenderer()).getD3D11Device();

			// Create a render target view
			hResult = d3d11Device->CreateRenderTargetView(d3d11Texture2DBackBuffer, nullptr, &mD3D11RenderTargetView);
			d3d11Texture2DBackBuffer->Release();
			if (SUCCEEDED(hResult))
			{
				// Create depth stencil texture
				const Renderer::TextureFormat::Enum depthStencilAttachmentTextureFormat = static_cast<RenderPass&>(getRenderPass()).getDepthStencilAttachmentTextureFormat();
				if (Renderer::TextureFormat::Enum::UNKNOWN != depthStencilAttachmentTextureFormat)
				{
					// Get the swap chain width and height, ensures they are never ever zero
					UINT width  = 1;
					UINT height = 1;
					getSafeWidthAndHeight(width, height);

					// Create depth stencil texture
					ID3D11Texture2D* d3d11Texture2DDepthStencil = nullptr;
					D3D11_TEXTURE2D_DESC d3d11Texture2DDesc = {};
					d3d11Texture2DDesc.Width			  = width;
					d3d11Texture2DDesc.Height			  = height;
					d3d11Texture2DDesc.MipLevels		  = 1;
					d3d11Texture2DDesc.ArraySize		  = 1;
					d3d11Texture2DDesc.Format			  = static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(depthStencilAttachmentTextureFormat));
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
						D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DepthStencilViewDesc = {};
						d3d11DepthStencilViewDesc.Format			 = d3d11Texture2DDesc.Format;
						d3d11DepthStencilViewDesc.ViewDimension		 = D3D11_DSV_DIMENSION_TEXTURE2D;
						d3d11DepthStencilViewDesc.Texture2D.MipSlice = 0;
						d3d11Device->CreateDepthStencilView(d3d11Texture2DDepthStencil, &d3d11DepthStencilViewDesc, &mD3D11DepthStencilView);
						d3d11Texture2DDepthStencil->Release();
					}
				}
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
