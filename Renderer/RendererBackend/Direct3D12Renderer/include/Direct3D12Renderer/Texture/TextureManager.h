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
#include <Renderer/Texture/ITextureManager.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{
	class Direct3D12Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 12 texture manager interface
	*/
	class TextureManager : public Renderer::ITextureManager
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D12Renderer
		*    Owner Direct3D 12 renderer instance
		*/
		explicit TextureManager(Direct3D12Renderer& direct3D12Renderer);

		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~TextureManager();


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureManager methods      ]
	//[-------------------------------------------------------]
	public:
		virtual Renderer::ITexture1D *createTexture1D(uint32_t width, Renderer::TextureFormat::Enum textureFormat, const void *data = nullptr, uint32_t flags = 0, Renderer::TextureUsage textureUsage = Renderer::TextureUsage::DEFAULT) override;
		virtual Renderer::ITexture2D *createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void *data = nullptr, uint32_t flags = 0, Renderer::TextureUsage textureUsage = Renderer::TextureUsage::DEFAULT, uint8_t numberOfMultisamples = 1, const Renderer::OptimizedTextureClearValue* optimizedTextureClearValue = nullptr) override;
		virtual Renderer::ITexture2DArray *createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, const void *data = nullptr, uint32_t flags = 0, Renderer::TextureUsage textureUsage = Renderer::TextureUsage::DEFAULT) override;
		virtual Renderer::ITexture3D *createTexture3D(uint32_t width, uint32_t height, uint32_t depth, Renderer::TextureFormat::Enum textureFormat, const void *data = nullptr, uint32_t flags = 0, Renderer::TextureUsage textureUsage = Renderer::TextureUsage::DEFAULT) override;
		virtual Renderer::ITextureCube *createTextureCube(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void *data = nullptr, uint32_t flags = 0, Renderer::TextureUsage textureUsage = Renderer::TextureUsage::DEFAULT) override;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D12Renderer/Texture/TextureManager.inl"
