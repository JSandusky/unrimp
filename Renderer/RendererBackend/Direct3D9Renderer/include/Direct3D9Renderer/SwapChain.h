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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/ISwapChain.h>
#include <Renderer/PlatformTypes.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct IDirect3DSurface9;
struct IDirect3DSwapChain9;
namespace Direct3D9Renderer
{
	class Direct3D9Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 9 swap chain class
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
		*  @param[in] direct3D9Renderer
		*    Owner Direct3D 9 renderer instance
		*  @param[in] nativeWindowHandle
		*    Native window handle, in case of a null handle nothing happens
		*/
		SwapChain(Direct3D9Renderer &direct3D9Renderer, handle nativeWindowHandle);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~SwapChain();

		/**
		*  @brief
		*    Return the Direct3D 9 swap chain instance
		*
		*  @return
		*    The Direct3D 9 swap chain instance, null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline IDirect3DSwapChain9 *getDirect3DSwapChain9() const;

		/**
		*  @brief
		*    Return the Direct3D 9 render target surface instance
		*
		*  @return
		*    The Direct3D 9 render target surface instance, null pointer on error, do not release the returned instance unless you added an own reference to it
		*
		*  @note
		*    - It's highly recommended to not keep any references to the returned instance, else issues may occure when resizing the swap chain
		*/
		inline IDirect3DSurface9 *getDirect3DSurface9RenderTarget() const;

		/**
		*  @brief
		*    Return the Direct3D 9 depth stencil surface instance
		*
		*  @return
		*    The Direct3D 9 depth stencil surface instance, null pointer on error, do not release the returned instance unless you added an own reference to it
		*
		*  @note
		*    - It's highly recommended to not keep any references to the returned instance, else issues may occure when resizing the swap chain
		*/
		inline IDirect3DSurface9 *getDirect3DSurface9DepthStencil() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char *name) override;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	public:
		virtual void getWidthAndHeight(uint32_t &width, uint32_t &height) const override;


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
		*    For instance "IDirect3DDevice9::CreateAdditionalSwapChain()" can automatically choose the width and height to match the client
		*    rectangle of the native window, but as soon as the width or height is zero we will get the error message
		*      "Direct3D9: (ERROR) :Failed to create driver surface"
		*      "Direct3D9: (ERROR) :Failure initializing swap chain. CreateAdditionalSwapChain fails"
		*      "D3D9 Helper: IDirect3DDevice9::CreateAdditionalSwapChain failed: E_NOTIMPL"
		*   So, best to use this method which gets the width and height of the native output
		*   window manually and ensures it's never zero.
		*
		*  @note
		*    - "mDXGISwapChain" must be valid when calling this method
		*/
		void getSafeWidthAndHeight(uint32_t &width, uint32_t &height) const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IDirect3DSwapChain9	*mDirect3DSwapChain9;			///< The Direct3D 9 swap chain instance, null pointer on error
		IDirect3DSurface9	*mDirect3DSurface9RenderTarget;	///< The Direct3D 9 render target surface instance, null pointer on error
		IDirect3DSurface9	*mDirect3DSurface9DepthStencil;	///< The Direct3D 9 depth stencil surface instance, null pointer on error


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D9Renderer/SwapChain.inl"
