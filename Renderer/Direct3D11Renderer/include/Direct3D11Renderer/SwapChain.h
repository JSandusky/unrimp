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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __DIRECT3D11RENDERER_SWAPCHAIN_H__
#define __DIRECT3D11RENDERER_SWAPCHAIN_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/ISwapChain.h>
#include <Renderer/PlatformTypes.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
namespace Direct3D11Renderer
{
	class Direct3D11Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 11 swap chain class
	*/
	class SwapChain : public Renderer::ISwapChain
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D11Renderer
		*    Owner Direct3D 11 renderer instance
		*  @param[in] nativeWindowHandle
		*    Native window handle, in case of a null handle nothing happens
		*/
		SwapChain(Direct3D11Renderer &direct3D11Renderer, handle nativeWindowHandle);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~SwapChain();

		/**
		*  @brief
		*    Return the DXGI swap chain instance
		*
		*  @return
		*    The DXGI swap chain instance, null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline IDXGISwapChain *getDxgiSwapChain() const;

		/**
		*  @brief
		*    Return the Direct3D 11 render target view instance
		*
		*  @return
		*    The Direct3D 11 render target view instance, null pointer on error, do not release the returned instance unless you added an own reference to it
		*
		*  @note
		*    - It's highly recommended to not keep any references to the returned instance, else issues may occure when resizing the swap chain
		*/
		inline ID3D11RenderTargetView *getD3D11RenderTargetView() const;

		/**
		*  @brief
		*    Return the Direct3D 11 depth stencil view instance
		*
		*  @return
		*    The Direct3D 11 depth stencil view instance, null pointer on error, do not release the returned instance unless you added an own reference to it
		*
		*  @note
		*    - It's highly recommended to not keep any references to the returned instance, else issues may occure when resizing the swap chain
		*/
		inline ID3D11DepthStencilView *getD3D11DepthStencilView() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char *name) override;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	public:
		virtual void getWidthAndHeight(unsigned int &width, unsigned int &height) const override;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ISwapChain methods           ]
	//[-------------------------------------------------------]
	public:
		virtual handle getNativeWindowHandle() const override;
		virtual void present() override;
		virtual void resizeBuffers() override;
		virtual bool getFullscreenState() const override;
		virtual void setFullscreenState(bool fullscreen) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Return the swap chain width and height
		*
		*  @param[out] width
		*    Receives the swap chain width
		*  @param[out] height
		*    Receives the swap chain height
		*
		*  @remarks
		*    For instance "IDXGISwapChain::ResizeBuffers()" can automatically choose the width and height to match the client
		*    rectangle of the native window, but as soon as the width or height is zero we will get the error message
		*     "DXGI Error: The buffer height inferred from the output window is zero. Taking 8 as a reasonable default instead"
		*     "D3D11: ERROR: ID3D11Device::CreateTexture2D: The Dimensions are invalid. For feature level D3D_FEATURE_LEVEL_11_0,
		*      the Width (value = 116) must be between 1 and 16384, inclusively. The Height (value = 0) must be between 1 and 16384,
		*      inclusively. And, the ArraySize (value = 1) must be between 1 and 2048, inclusively. [ STATE_CREATION ERROR #101: CREATETEXTURE2D_INVALIDDIMENSIONS ]"
		*   including an evil memory leak. So, best to use this method which gets the width and height of the native output
		*   window manually and ensures it's never zero.
		*
		*  @note
		*    - "mDxgiSwapChain" must be valid when calling this method
		*/
		void getSafeWidthAndHeight(unsigned int &width, unsigned int &height) const;

		/**
		*  @brief
		*    Create the Direct3D 11 views
		*/
		void createDirect3D11Views();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IDXGISwapChain		   *mDxgiSwapChain;			///< The DXGI swap chain instance, null pointer on error
		ID3D11RenderTargetView *mD3D11RenderTargetView;	///< The Direct3D 11 render target view instance, null pointer on error
		ID3D11DepthStencilView *mD3D11DepthStencilView;	///< The Direct3D 11 depth stencil view instance, null pointer on error


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/SwapChain.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D11RENDERER_SWAPCHAIN_H__
