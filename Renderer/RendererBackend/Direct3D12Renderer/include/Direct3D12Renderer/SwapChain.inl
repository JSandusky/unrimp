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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline IDXGISwapChain3 *SwapChain::getDxgiSwapChain3() const
	{
		return mDxgiSwapChain3;
	}

	inline ID3D12DescriptorHeap *SwapChain::getD3D12DescriptorHeapRenderTargetView() const
	{
		return mD3D12DescriptorHeapRenderTargetView;
	}

	inline ID3D12DescriptorHeap *SwapChain::getD3D12DescriptorHeapDepthStencilView() const
	{
		return mD3D12DescriptorHeapDepthStencilView;
	}

	inline UINT SwapChain::getRenderTargetViewDescriptorSize() const
	{
		return mRenderTargetViewDescriptorSize;
	}

	inline UINT SwapChain::getBackD3D12ResourceRenderTargetFrameIndex() const
	{
		return mFrameIndex;
	}

	inline ID3D12Resource *SwapChain::getBackD3D12ResourceRenderTarget() const
	{
		return mD3D12ResourceRenderTargets[mFrameIndex];
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
