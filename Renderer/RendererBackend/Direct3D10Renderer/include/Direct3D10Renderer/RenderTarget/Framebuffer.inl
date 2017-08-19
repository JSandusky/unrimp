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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline uint32_t Framebuffer::getNumberOfColorTextures() const
	{
		return mNumberOfColorTextures;
	}

	inline Renderer::ITexture** Framebuffer::getColorTextures() const
	{
		return mColorTextures;
	}

	inline Renderer::ITexture* Framebuffer::getDepthStencilTexture() const
	{
		return mDepthStencilTexture;
	}

	inline ID3D10RenderTargetView** Framebuffer::getD3D10RenderTargetViews() const
	{
		return mD3D10RenderTargetViews;
	}

	inline ID3D10DepthStencilView* Framebuffer::getD3D10DepthStencilView() const
	{
		return mD3D10DepthStencilView;
	}

	inline bool Framebuffer::getGenerateMipmaps() const
	{
		return mGenerateMipmaps;
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	inline const Renderer::IRenderPass& Framebuffer::getRenderPass() const
	{
		return mRenderPass;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
