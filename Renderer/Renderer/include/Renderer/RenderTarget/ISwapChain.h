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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Renderer/PlatformTypes.h"
#include "Renderer/RenderTarget/IRenderTarget.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract swap chain interface
	*/
	class ISwapChain : public IRenderTarget
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~ISwapChain();


	//[-------------------------------------------------------]
	//[ Public virtual ISwapChain methods                     ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the native window handle
		*
		*  @return
		*    Native window handle the swap chain is using as output window, can be a null handle
		*/
		virtual handle getNativeWindowHandle() const = 0;

		/**
		*  @brief
		*    Present the content of the current back buffer
		*
		*  @note
		*    - Swap of front and back buffer
		*/
		virtual void present() = 0;

		/**
		*  @brief
		*    Call this method whenever the size of the native window was changed
		*/
		virtual void resizeBuffers() = 0;

		/**
		*  @brief
		*    Return the current fullscreen state
		*
		*  @return
		*    "true" if fullscreen, else "false"
		*/
		virtual bool getFullscreenState() const = 0;

		/**
		*  @brief
		*    Set the current fullscreen state
		*
		*  @param[in] fullscreen
		*    "true" if fullscreen, else "false"
		*/
		virtual void setFullscreenState(bool fullscreen) = 0;

		/**
		*  @brief
		*    Sets the width and height of the swap chain
		*
		*  @param[in] width
		*    Width of the swap chain
		*  @param[in] height
		*    Height of the swap chain
		*
		*  @note
		*    This method can be used to override the width and height which gets returned by the IRenderTarget::getWidthAndHeight method.
		*    This method can also be used to set specific size when the user don't want that the swap chain itself tempers with the given window handle (the handle might be invalid but non 0)
		*    e.g. the user uses a windowing library (e.g. SDL2) which abstracts the window handling on different windowing platforms(e.g. Win32 or Linux/Wayland) and the application should run on a windowing platform which isn't supported by the swapchain implementation itself
		*/
		virtual void setWidthAndHeight(uint32_t width, uint32_t height) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Owner renderer instance
		*/
		inline explicit ISwapChain(IRenderer& renderer);

		explicit ISwapChain(const ISwapChain& source) = delete;
		ISwapChain& operator =(const ISwapChain& source) = delete;


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<ISwapChain> ISwapChainPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/RenderTarget/ISwapChain.inl"
