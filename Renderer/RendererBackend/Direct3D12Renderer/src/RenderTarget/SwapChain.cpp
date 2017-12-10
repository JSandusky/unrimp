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
#include "Direct3D12Renderer/RenderTarget/SwapChain.h"
#include "Direct3D12Renderer/RenderTarget/RenderPass.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/D3D12X.h"
#include "Direct3D12Renderer/Mapping.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		void handleDeviceLost(const Direct3D12Renderer::Direct3D12Renderer& direct3D12Renderer, HRESULT result)
		{
			// If the device was removed either by a disconnection or a driver upgrade, we must recreate all device resources
			if (DXGI_ERROR_DEVICE_REMOVED == result || DXGI_ERROR_DEVICE_RESET == result)
			{
				if (DXGI_ERROR_DEVICE_REMOVED == result)
				{
					result = direct3D12Renderer.getD3D12Device()->GetDeviceRemovedReason();
				}
				RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "Direct3D 12 device lost on present: Reason code 0x%08X", static_cast<unsigned int>(result))

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
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SwapChain::SwapChain(Renderer::IRenderPass& renderPass, Renderer::WindowHandle windowHandle) :
		ISwapChain(renderPass),
		mDxgiSwapChain3(nullptr),
		mD3D12DescriptorHeapRenderTargetView(nullptr),
		mD3D12DescriptorHeapDepthStencilView(nullptr),
		mRenderTargetViewDescriptorSize(0),
		mD3D12ResourceRenderTargets{},
		mD3D12ResourceDepthStencil(nullptr),
		mSynchronizationInterval(0),
		mFrameIndex(0),
		mFenceEvent(nullptr),
		mD3D12Fence(nullptr),
		mFenceValue(0)
	{
		Direct3D12Renderer& direct3D12Renderer = static_cast<Direct3D12Renderer&>(renderPass.getRenderer());
		const RenderPass& d3d12RenderPass = static_cast<RenderPass&>(renderPass);

		// Sanity check
		RENDERER_ASSERT(direct3D12Renderer.getContext(), 1 == d3d12RenderPass.getNumberOfColorAttachments(), "There must be exactly one Direct3D 12 render pass color attachment")

		// Get the native window handle
		const HWND hWnd = reinterpret_cast<HWND>(windowHandle.nativeWindowHandle);

		// Get our IDXGI factory instance
		IDXGIFactory4& dxgiFactory4 = direct3D12Renderer.getDxgiFactory4Safe();

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

		// TODO(co) Add tearing support, see Direct3D 11 backend
		// Determines whether tearing support is available for fullscreen borderless windows
		// -> To unlock frame rates of UWP applications on the Windows Store and providing support for both AMD Freesync and NVIDIA's G-SYNC we must explicitly allow tearing
		// -> See "Windows Dev Center" -> "Variable refresh rate displays": https://msdn.microsoft.com/en-us/library/windows/desktop/mt742104(v=vs.85).aspx

		// Create the swap chain
		DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc = {};
		dxgiSwapChainDesc.BufferCount							= NUMBER_OF_FRAMES;
		dxgiSwapChainDesc.BufferDesc.Width						= static_cast<UINT>(width);
		dxgiSwapChainDesc.BufferDesc.Height						= static_cast<UINT>(height);
		dxgiSwapChainDesc.BufferDesc.Format						= static_cast<DXGI_FORMAT>(Mapping::getDirect3D12Format(d3d12RenderPass.getColorAttachmentTextureFormat(0)));
		dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator		= 60;
		dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
		dxgiSwapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		dxgiSwapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_FLIP_DISCARD;
		dxgiSwapChainDesc.OutputWindow							= hWnd;
		dxgiSwapChainDesc.SampleDesc.Count						= 1;
		dxgiSwapChainDesc.Windowed								= TRUE;
		IDXGISwapChain* dxgiSwapChain = nullptr;
		dxgiFactory4.CreateSwapChain(direct3D12Renderer.getD3D12CommandQueue(), &dxgiSwapChainDesc, &dxgiSwapChain);
		if (FAILED(dxgiSwapChain->QueryInterface(IID_PPV_ARGS(&mDxgiSwapChain3))))
		{
			dxgiSwapChain->Release();
			RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "Failed to retrieve the Direct3D 12 DXGI swap chain 3")
		}

		// Disable alt-return for automatic fullscreen state change
		// -> We handle this manually to have more control over it
		dxgiFactory4.MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

		// Create the Direct3D 12 views
		if (nullptr != mDxgiSwapChain3)
		{
			createDirect3D12Views();
		}

		{ // Create synchronization objects
			// Get the Direct3D 12 device instance
			ID3D12Device* d3d12Device = nullptr;
			mDxgiSwapChain3->GetDevice(__uuidof(ID3D12Device), (void**)&d3d12Device);
			if (nullptr != d3d12Device)
			{
				if (SUCCEEDED(d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mD3D12Fence))))
				{
					mFenceValue = 1;

					// Create an event handle to use for frame synchronization
					mFenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
					if (nullptr == mFenceEvent)
					{
						RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "Failed to create an Direct3D 12 event handle to use for frame synchronization. Error code %d", ::GetLastError())
					}
				}
				else
				{
					RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "Failed to create Direct3D 12 fence instance")
				}
			}
			else
			{
				RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "Failed to retrieve the Direct3D 12 device instance from the swap chain")
			}
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
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
		destroyDirect3D12Views();
		if (nullptr != mDxgiSwapChain3)
		{
			mDxgiSwapChain3->Release();
		}

		// Destroy synchronization objects
		if (nullptr != mFenceEvent)
		{
			::CloseHandle(mFenceEvent);
		}
		if (nullptr != mD3D12Fence)
		{
			mD3D12Fence->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void SwapChain::setDebugName(const char* name)
	{
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			// Assign a debug name to the DXGI swap chain
			if (nullptr != mDxgiSwapChain3)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mDxgiSwapChain3->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mDxgiSwapChain3->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}

			// Assign a debug name to the Direct3D 12 frame resources
			for (int frame = 0; frame < NUMBER_OF_FRAMES; ++frame)
			{
				if (nullptr != mD3D12ResourceRenderTargets[frame])
				{
					// Set the debug name
					// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
					mD3D12ResourceRenderTargets[frame]->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
					mD3D12ResourceRenderTargets[frame]->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
				}
			}
			if (nullptr != mD3D12ResourceDepthStencil)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D12ResourceDepthStencil->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12ResourceDepthStencil->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}

			// Assign a debug name to the Direct3D 12 descriptor heaps
			if (nullptr != mD3D12DescriptorHeapRenderTargetView)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D12DescriptorHeapRenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12DescriptorHeapRenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
			if (nullptr != mD3D12DescriptorHeapDepthStencilView)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D12DescriptorHeapDepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12DescriptorHeapDepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void SwapChain::getWidthAndHeight(uint32_t& width, uint32_t& height) const
	{
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain3)
		{
			// Get the Direct3D 12 swap chain description
			DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
			mDxgiSwapChain3->GetDesc(&dxgiSwapChainDesc);

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
		if (nullptr != mDxgiSwapChain3)
		{
			// Get the Direct3D 12 swap chain description
			DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
			mDxgiSwapChain3->GetDesc(&dxgiSwapChainDesc);

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
		if (nullptr != mDxgiSwapChain3)
		{
			const Direct3D12Renderer& direct3D12Renderer = static_cast<Direct3D12Renderer&>(getRenderPass().getRenderer());
			::detail::handleDeviceLost(direct3D12Renderer, mDxgiSwapChain3->Present(mSynchronizationInterval, 0));

			// Wait for the GPU to be done with all resources
			waitForPreviousFrame();
		}
	}

	void SwapChain::resizeBuffers()
	{
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain3)
		{
			Direct3D12Renderer& direct3D12Renderer = static_cast<Direct3D12Renderer&>(getRenderer());

			// Get the currently set render target
			Renderer::IRenderTarget* renderTargetBackup = direct3D12Renderer.omGetRenderTarget();

			// In case this swap chain is the current render target, we have to unset it before continuing
			if (this == renderTargetBackup)
			{
				direct3D12Renderer.omSetRenderTarget(nullptr);
			}
			else
			{
				renderTargetBackup = nullptr;
			}

			// Release the views
			destroyDirect3D12Views();

			// Get the swap chain width and height, ensures they are never ever zero
			UINT width  = 1;
			UINT height = 1;
			getSafeWidthAndHeight(width, height);

			// Resize the Direct3D 12 swap chain
			// -> Preserve the existing buffer count and format
			const HRESULT result = mDxgiSwapChain3->ResizeBuffers(NUMBER_OF_FRAMES, width, height, static_cast<DXGI_FORMAT>(Mapping::getDirect3D12Format(static_cast<RenderPass&>(getRenderPass()).getColorAttachmentTextureFormat(0))), 0);
			if (SUCCEEDED(result))
			{
				// Create the Direct3D 12 views
				createDirect3D12Views();

				// If required, restore the previously set render target
				if (nullptr != renderTargetBackup)
				{
					direct3D12Renderer.omSetRenderTarget(renderTargetBackup);
				}
			}
			else
			{
				::detail::handleDeviceLost(direct3D12Renderer, result);
			}
		}
	}

	bool SwapChain::getFullscreenState() const
	{
		// Window mode by default
		BOOL fullscreen = FALSE;

		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain3)
		{
			mDxgiSwapChain3->GetFullscreenState(&fullscreen, nullptr);
		}

		// Done
		return (fullscreen != FALSE);
	}

	void SwapChain::setFullscreenState(bool fullscreen)
	{
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain3)
		{
			const HRESULT result = mDxgiSwapChain3->SetFullscreenState(fullscreen, nullptr);
			if (FAILED(result))
			{
				// TODO(co) Better error handling
				RENDERER_LOG(static_cast<Direct3D12Renderer&>(getRenderer()).getContext(), CRITICAL, "Failed to set Direct3D 12 fullscreen state")
			}
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
		// Get the Direct3D 12 swap chain description
		DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
		mDxgiSwapChain3->GetDesc(&dxgiSwapChainDesc);

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

	void SwapChain::createDirect3D12Views()
	{
		RENDERER_ASSERT(getRenderer().getContext(), nullptr != mDxgiSwapChain3, "Invalid Direct3D 12 DXGI swap chain 3")

		// TODO(co) Debug name gets lost when resizing a window, fix this

		// Get the Direct3D 12 device instance
		ID3D12Device* d3d12Device = nullptr;
		mDxgiSwapChain3->GetDevice(__uuidof(ID3D12Device), (void**)&d3d12Device);
		if (nullptr != d3d12Device)
		{
			{ // Describe and create a render target view (RTV) descriptor heap
				D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc = {};
				d3d12DescriptorHeapDesc.NumDescriptors	= NUMBER_OF_FRAMES;
				d3d12DescriptorHeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				d3d12DescriptorHeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				if (SUCCEEDED(d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(&mD3D12DescriptorHeapRenderTargetView))))
				{
					mRenderTargetViewDescriptorSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

					{ // Create frame resources
						CD3DX12_CPU_DESCRIPTOR_HANDLE d3d12XCpuDescriptorHandle(mD3D12DescriptorHeapRenderTargetView->GetCPUDescriptorHandleForHeapStart());

						// Create a RTV for each frame
						for (UINT frame = 0; frame < NUMBER_OF_FRAMES; ++frame)
						{
							if (SUCCEEDED(mDxgiSwapChain3->GetBuffer(frame, IID_PPV_ARGS(&mD3D12ResourceRenderTargets[frame]))))
							{
								d3d12Device->CreateRenderTargetView(mD3D12ResourceRenderTargets[frame], nullptr, d3d12XCpuDescriptorHandle);
								d3d12XCpuDescriptorHandle.Offset(1, mRenderTargetViewDescriptorSize);
							}
							else
							{
								RENDERER_LOG(static_cast<Direct3D12Renderer&>(getRenderer()).getContext(), CRITICAL, "Failed to retrieve frame buffer from Direct3D 12 DXGI swap chain")
							}
						}
					}

					mFrameIndex = mDxgiSwapChain3->GetCurrentBackBufferIndex();
				}
				else
				{
					RENDERER_LOG(static_cast<Direct3D12Renderer&>(getRenderer()).getContext(), CRITICAL, "Failed to describe and create a Direct3D 12 render target view (RTV) descriptor heap")
				}
			}

			// Describe and create a depth stencil view (DSV) descriptor heap
			const Renderer::TextureFormat::Enum depthStencilAttachmentTextureFormat = static_cast<RenderPass&>(getRenderPass()).getDepthStencilAttachmentTextureFormat();
			if (Renderer::TextureFormat::Enum::UNKNOWN != depthStencilAttachmentTextureFormat)
			{
				D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc = {};
				d3d12DescriptorHeapDesc.NumDescriptors	= 1;
				d3d12DescriptorHeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
				d3d12DescriptorHeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				if (SUCCEEDED(d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(&mD3D12DescriptorHeapDepthStencilView))))
				{
					D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
					depthStencilDesc.Format = static_cast<DXGI_FORMAT>(Mapping::getDirect3D12Format(depthStencilAttachmentTextureFormat));
					depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
					depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

					D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
					depthOptimizedClearValue.Format = depthStencilDesc.Format;
					depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
					depthOptimizedClearValue.DepthStencil.Stencil = 0;

					// Get the swap chain width and height, ensures they are never ever zero
					UINT width  = 1;
					UINT height = 1;
					getSafeWidthAndHeight(width, height);

					const CD3DX12_HEAP_PROPERTIES d3d12XHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
					const CD3DX12_RESOURCE_DESC d3d12XResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(depthStencilDesc.Format, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
					if (SUCCEEDED(d3d12Device->CreateCommittedResource(
						&d3d12XHeapProperties,
						D3D12_HEAP_FLAG_NONE,
						&d3d12XResourceDesc,
						D3D12_RESOURCE_STATE_DEPTH_WRITE,
						&depthOptimizedClearValue,
						IID_PPV_ARGS(&mD3D12ResourceDepthStencil)
						)))
					{
						d3d12Device->CreateDepthStencilView(mD3D12ResourceDepthStencil, &depthStencilDesc, mD3D12DescriptorHeapDepthStencilView->GetCPUDescriptorHandleForHeapStart());
					}
					else
					{
						RENDERER_LOG(static_cast<Direct3D12Renderer&>(getRenderer()).getContext(), CRITICAL, "Failed to create the Direct3D 12 depth stencil view (DSV) resource")
					}
				}
				else
				{
					RENDERER_LOG(static_cast<Direct3D12Renderer&>(getRenderer()).getContext(), CRITICAL, "Failed to describe and create a Direct3D 12 depth stencil view (DSV) descriptor heap")
				}
			}
		}
		else
		{
			RENDERER_LOG(static_cast<Direct3D12Renderer&>(getRenderer()).getContext(), CRITICAL, "Failed to retrieve the Direct3D 12 device instance from the swap chain")
		}
	}

	void SwapChain::destroyDirect3D12Views()
	{
		// Wait for the GPU to be done with all resources
		waitForPreviousFrame();

		// Release Direct3D 12 resources
		for (int frame = 0; frame < NUMBER_OF_FRAMES; ++frame)
		{
			if (nullptr != mD3D12ResourceRenderTargets[frame])
			{
				mD3D12ResourceRenderTargets[frame]->Release();
				mD3D12ResourceRenderTargets[frame] = nullptr;
			}
		}
		if (nullptr != mD3D12ResourceDepthStencil)
		{
			mD3D12ResourceDepthStencil->Release();
			mD3D12ResourceDepthStencil = nullptr;
		}

		// Release Direct3D 12 descriptor heap
		if (nullptr != mD3D12DescriptorHeapRenderTargetView)
		{
			mD3D12DescriptorHeapRenderTargetView->Release();
			mD3D12DescriptorHeapRenderTargetView = nullptr;
		}
		if (nullptr != mD3D12DescriptorHeapDepthStencilView)
		{
			mD3D12DescriptorHeapDepthStencilView->Release();
			mD3D12DescriptorHeapDepthStencilView = nullptr;
		}
	}

	void SwapChain::waitForPreviousFrame()
	{
		RENDERER_ASSERT(getRenderer().getContext(), nullptr != mDxgiSwapChain3, "Invalid Direct3D 12 DXGI swap chain 3")

		// TODO(co) This is the most simple but least effective approach and only meant for the Direct3D 12 renderer backend kickoff.

		// Signal and increment the fence value
		const UINT64 fence = mFenceValue;
		if (SUCCEEDED(static_cast<Direct3D12Renderer&>(getRenderer()).getD3D12CommandQueue()->Signal(mD3D12Fence, fence)))
		{
			++mFenceValue;

			// Wait until the previous frame is finished
			if (mD3D12Fence->GetCompletedValue() < fence)
			{
				if (SUCCEEDED(mD3D12Fence->SetEventOnCompletion(fence, mFenceEvent)))
				{
					::WaitForSingleObject(mFenceEvent, INFINITE);
				}
				else
				{
					RENDERER_LOG(static_cast<Direct3D12Renderer&>(getRenderer()).getContext(), CRITICAL, "Failed to set Direct3D 12 event on completion")
				}
			}

			mFrameIndex = mDxgiSwapChain3->GetCurrentBackBufferIndex();
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
