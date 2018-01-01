/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include <Renderer/Texture/TextureTypes.h>
#include <Renderer/RenderTarget/IRenderPass.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 10 render pass interface
	*/
	class RenderPass : public Renderer::IRenderPass
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Owner renderer instance
		*  @param[in] numberOfColorAttachments
		*    Number of color render target textures, must be <="Renderer::Capabilities::maximumNumberOfSimultaneousRenderTargets"
		*  @param[in] colorAttachmentTextureFormats
		*    The color render target texture formats, can be a null pointer or can contain null pointers, if not a null pointer there must be at
		*    least "numberOfColorAttachments" textures in the provided C-array of pointers
		*  @param[in] depthStencilAttachmentTextureFormat
		*    The optional depth stencil render target texture format, can be a "Renderer::TextureFormat::UNKNOWN" if there should be no depth buffer
		*  @param[in] numberOfMultisamples
		*    The number of multisamples per pixel (valid values: 1, 2, 4, 8)
		*/
		RenderPass(Renderer::IRenderer& renderer, uint32_t numberOfColorAttachments, const Renderer::TextureFormat::Enum* colorAttachmentTextureFormats, Renderer::TextureFormat::Enum depthStencilAttachmentTextureFormat, uint8_t numberOfMultisamples);

		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~RenderPass() override;

		/**
		*  @brief
		*    Return the number of color render target textures
		*
		*  @return
		*    The number of color render target textures
		*/
		inline uint32_t getNumberOfColorAttachments() const;

		/**
		*  @brief
		*    Return the number of render target textures (color and depth stencil)
		*
		*  @return
		*    The number of render target textures (color and depth stencil)
		*/
		inline uint32_t getNumberOfAttachments() const;

		/**
		*  @brief
		*    Return the color attachment texture format
		*
		*  @param[in] colorAttachmentIndex
		*    Color attachment index
		*
		*  @return
		*    The color attachment texture format
		*/
		inline Renderer::TextureFormat::Enum getColorAttachmentTextureFormat(uint32_t colorAttachmentIndex) const;

		/**
		*  @brief
		*    Return the depth stencil attachment texture format
		*
		*  @return
		*    The depth stencil attachment texture format
		*/
		inline Renderer::TextureFormat::Enum getDepthStencilAttachmentTextureFormat() const;

		/**
		*  @brief
		*    Return the number of multisamples
		*
		*  @return
		*    The number of multisamples
		*/
		inline uint8_t getNumberOfMultisamples() const;


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	protected:
		virtual void selfDestruct() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit RenderPass(const RenderPass& source) = delete;
		RenderPass& operator =(const RenderPass& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		uint32_t					  mNumberOfColorAttachments;
		Renderer::TextureFormat::Enum mColorAttachmentTextureFormats[8];
		Renderer::TextureFormat::Enum mDepthStencilAttachmentTextureFormat;
		uint8_t						  mNumberOfMultisamples;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D10Renderer/RenderTarget/RenderPass.inl"
