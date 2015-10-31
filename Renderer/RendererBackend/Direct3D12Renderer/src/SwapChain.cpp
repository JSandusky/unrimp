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
#include "Direct3D12Renderer/D3D12X.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SwapChain::SwapChain(Direct3D12Renderer &direct3D12Renderer, handle nativeWindowHandle) :
		ISwapChain(direct3D12Renderer),
		mDxgiSwapChain3(nullptr),
		mD3D12DescriptorHeap(nullptr),
		mRenderTargetViewDescriptorSize(0),
		mFrameIndex(0),
		mFenceEvent(nullptr),
		mD3D12Fence(nullptr),
		mFenceValue(0)
	{
		memset(mD3D12ResourceRenderTargets, 0, sizeof(ID3D12Resource*) * NUMBER_OF_FRAMES);

		// Get the native window handle
		const HWND hWnd = reinterpret_cast<HWND>(nativeWindowHandle);

		// Get our IDXGI factory instance
		IDXGIFactory4 &dxgiFactory4 = direct3D12Renderer.getDxgiFactory4Safe();

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
		dxgiSwapChainDesc.BufferCount		= NUMBER_OF_FRAMES;
		dxgiSwapChainDesc.BufferDesc.Width	= static_cast<UINT>(width);
		dxgiSwapChainDesc.BufferDesc.Height	= static_cast<UINT>(height);
		dxgiSwapChainDesc.BufferDesc.Format	= DXGI_FORMAT_R8G8B8A8_UNORM;
		dxgiSwapChainDesc.BufferUsage		= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		dxgiSwapChainDesc.SwapEffect		= DXGI_SWAP_EFFECT_FLIP_DISCARD;
		dxgiSwapChainDesc.OutputWindow		= hWnd;
		dxgiSwapChainDesc.SampleDesc.Count	= 1;
		dxgiSwapChainDesc.Windowed			= TRUE;
		IDXGISwapChain* dxgiSwapChain = nullptr;
		dxgiFactory4.CreateSwapChain(direct3D12Renderer.getD3D12CommandQueue(), &dxgiSwapChainDesc, &dxgiSwapChain);
		if (FAILED(dxgiSwapChain->QueryInterface(IID_PPV_ARGS(&mDxgiSwapChain3))))
		{
			dxgiSwapChain->Release();
			RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to retrieve DXGI swap chain 3")
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
						RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 12 error: Failed to create an event handle to use for frame synchronization. Error code %d", ::GetLastError())
					}
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create Direct3D 12 fence instance")
				}
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to retrieve the Direct3D 12 device instance from the swap chain")
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
			mDxgiSwapChain3 = nullptr;
		}

		// Destroy synchronization objects
		if (nullptr != mFenceEvent)
		{
			::CloseHandle(mFenceEvent);
			mFenceEvent = nullptr;
		}
		if (nullptr != mD3D12Fence)
		{
			mD3D12Fence->Release();
			mD3D12Fence = nullptr;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void SwapChain::setDebugName(const char *name)
	{
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			// Assign a debug name to the DXGI swap chain
			if (nullptr != mDxgiSwapChain3)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mDxgiSwapChain3->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mDxgiSwapChain3->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}

			// Assign a debug name to the Direct3D 12 frame resources
			for (int frame = 0; frame < NUMBER_OF_FRAMES; ++frame)
			{
				if (nullptr != mD3D12ResourceRenderTargets[frame])
				{
					// Set the debug name
					// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
					mD3D12ResourceRenderTargets[frame]->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
					mD3D12ResourceRenderTargets[frame]->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
				}
			}

			// Assign a debug name to the Direct3D 12 descriptor heap
			if (nullptr != mD3D12DescriptorHeap)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D12DescriptorHeap->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12DescriptorHeap->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	void SwapChain::getWidthAndHeight(uint32_t &width, uint32_t &height) const
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

	void SwapChain::present()
	{
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain3)
		{
			mDxgiSwapChain3->Present(0, 0);

			// Wait for the GPU to be done with all resources
			waitForPreviousFrame();
		}
	}

	void SwapChain::resizeBuffers()
	{
		// Is there a valid swap chain?
		if (nullptr != mDxgiSwapChain3)
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
			destroyDirect3D12Views();

			// Get the swap chain width and height, ensures they are never ever zero
			UINT width  = 1;
			UINT height = 1;
			getSafeWidthAndHeight(width, height);

			// Resize the Direct3D 12 swap chain
			// -> Preserve the existing buffer count and format
			if (SUCCEEDED(mDxgiSwapChain3->ResizeBuffers(NUMBER_OF_FRAMES, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0)))
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
			mDxgiSwapChain3->SetFullscreenState(fullscreen, nullptr);
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void SwapChain::getSafeWidthAndHeight(uint32_t &width, uint32_t &height) const
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
		assert(nullptr != mDxgiSwapChain3);

		// TODO(co) Debug name gets lost when resizing a window, fix this

		// Get the Direct3D 12 device instance
		ID3D12Device* d3d12Device = nullptr;
		mDxgiSwapChain3->GetDevice(__uuidof(ID3D12Device), (void**)&d3d12Device);
		if (nullptr != d3d12Device)
		{
			// Describe and create a render target view (RTV) descriptor heap
			D3D12_DESCRIPTOR_HEAP_DESC d3d12DescriptorHeapDesc;
			::ZeroMemory(&d3d12DescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
			d3d12DescriptorHeapDesc.NumDescriptors	= NUMBER_OF_FRAMES;
			d3d12DescriptorHeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			d3d12DescriptorHeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			if (SUCCEEDED(d3d12Device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, IID_PPV_ARGS(&mD3D12DescriptorHeap))))
			{
				mRenderTargetViewDescriptorSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

				{ // Create frame resources
					CD3DX12_CPU_DESCRIPTOR_HANDLE d3d12XCpuDescriptorHandle(mD3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

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
							RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to retrieve frame buffer from DXGI swap chain")
						}
					}
				}

				mFrameIndex = mDxgiSwapChain3->GetCurrentBackBufferIndex();
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to describe and create a render target view (RTV) descriptor heap")
			}
		}
		else
		{
			RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to retrieve the Direct3D 12 device instance from the swap chain")
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
		if (nullptr != mD3D12DescriptorHeap)
		{
			mD3D12DescriptorHeap->Release();
			mD3D12DescriptorHeap = nullptr;
		}
	}

	void SwapChain::waitForPreviousFrame()
	{
		assert(nullptr != mDxgiSwapChain3);

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
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to set event on completion")
				}
			}

			mFrameIndex = mDxgiSwapChain3->GetCurrentBackBufferIndex();
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
