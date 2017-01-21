/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
#include <Renderer/RenderTarget/IFramebuffer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct IDirect3DSurface9;
namespace Renderer
{
	class ITexture;
}
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
	*    Direct3D 9 framebuffer class
	*/
	class Framebuffer : public Renderer::IFramebuffer
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
		*  @param[in] numberOfColorTextures
		*    Number of color render target textures
		*  @param[in] colorTextures
		*    The color render target textures, can be a null pointer or can contain null pointers, if not a null pointer there must be at
		*    least "numberOfColorTextures" textures in the provided C-array of pointers
		*  @param[in] depthStencilTexture
		*    The depth stencil render target texture, can be a null pointer
		*
		*  @note
		*    - The framebuffer keeps a reference to the provided texture instances
		*/
		Framebuffer(Direct3D9Renderer &direct3D9Renderer, uint32_t numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Framebuffer();

		/**
		*  @brief
		*    Return the number of Direct3D 9 render target surfaces
		*
		*  @return
		*    The number of Direct3D 9 render target surfaces
		*/
		inline uint32_t getNumberOfDirect3DSurface9Colors() const;

		/**
		*  @brief
		*    Return the Direct3D 9 render target surfaces
		*
		*  @return
		*    The Direct3D 9 render target surfaces, can be a null pointer, do not release the returned instances unless you added an own reference to it
		*/
		inline IDirect3DSurface9 **getDirect3DSurface9Colors() const;

		/**
		*  @brief
		*    Return the Direct3D 9 depth stencil surface
		*
		*  @return
		*    The Direct3D 9 depth stencil surface, can be a null pointer, do not release the returned instance unless you added an own reference to it
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
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Generic part
		uint32_t			 mNumberOfColorTextures;	///< Number of color render target textures
		Renderer::ITexture **mColorTextures;			///< The color render target textures (we keep a reference to it), can be a null pointer or can contain null pointers, if not a null pointer there must be at least "mNumberOfColorTextures" textures in the provided C-array of pointers
		Renderer::ITexture  *mDepthStencilTexture;		///< The depth stencil render target texture (we keep a reference to it), can be a null pointer
		uint32_t			 mWidth;					///< The framebuffer width
		uint32_t			 mHeight;					///< The framebuffer height
		// Direct3D 9 part
		IDirect3DSurface9 **mDirect3D9ColorSurfaces;		///< The Direct3D 9 color render target surfaces (we keep a reference to it), can be a null pointer or can contain null pointers, if not a null pointer there must be at least "mNumberOfColorTextures" textures in the provided C-array of pointers
		IDirect3DSurface9  *mDirect3D9DepthStencilSurface;	///< The Direct3D 9 depth stencil render target surface (we keep a reference to it), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D9Renderer/RenderTarget/Framebuffer.inl"
