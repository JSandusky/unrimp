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
#include <Renderer/Texture/ITextureCube.h>
#include <Renderer/Texture/TextureTypes.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct ID3D10Texture2D;
struct ID3D10ShaderResourceView;
namespace Direct3D10Renderer
{
	class Direct3D10Renderer;
}


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
	*    Direct3D 10 cube texture class
	*/
	class TextureCube : public Renderer::ITextureCube
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D10Renderer
		*    Owner Direct3D 10 renderer instance
		*  @param[in] width
		*    Texture width, must be >0
		*  @param[in] height
		*    Texture height, must be >0
		*  @param[in] textureFormat
		*    Texture format
		*  @param[in] data
		*    Texture data, can be a null pointer
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*  @param[in] textureUsage
		*    Indication of the texture usage (only relevant for Direct3D, OpenGL has no texture usage indication)
		*/
		TextureCube(Direct3D10Renderer& direct3D10Renderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, Renderer::TextureUsage textureUsage);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~TextureCube();

		/**
		*  @brief
		*    Return the texture format
		*
		*  @return
		*    The texture format
		*
		*  @remarks
		*    For textures used as depth stencil render target, Direct3D 10 format handling becomes a little bit more complex due to
		*    the offered flexibility. For example the abstract texture format "Renderer::TextureFormat::D32_FLOAT" translates into
		*    - Direct3D 10 texture format is "DXGI_FORMAT_R32_TYPELESS"
		*    - Direct3D 10 shader resource view format is "DXGI_FORMAT_R32_FLOAT"
		*    - Direct3D 10 depth stencil view format is "DXGI_FORMAT_D32_FLOAT"
		*/
		inline Renderer::TextureFormat::Enum getTextureFormat() const;

		/**
		*  @brief
		*    Return whether or not mipmaps should be generated automatically
		*
		*  @return
		*    "true" if mipmaps should be generated automatically, else "false"
		*/
		inline bool getGenerateMipmaps() const;

		/**
		*  @brief
		*    Return the Direct3D texture cube resource instance
		*
		*  @return
		*    The Direct3D texture cube resource instance, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D10Texture2D* getD3D10TextureCube() const;

		/**
		*  @brief
		*    Return the Direct3D shader resource view instance
		*
		*  @return
		*    The Direct3D shader resource view instance, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*
		*  @note
		*    - It's not recommended to manipulate the returned Direct3D 10 resource
		*      view by e.g. assigning another Direct3D 10 resource to it
		*/
		inline ID3D10ShaderResourceView* getD3D10ShaderResourceView() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char* name) override;
		virtual void* getInternalResourceHandle() const override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit TextureCube(const TextureCube& source) = delete;
		TextureCube& operator =(const TextureCube& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Renderer::TextureFormat::Enum mTextureFormat;
		uint8_t						  mNumberOfMultisamples;
		bool						  mGenerateMipmaps;
		ID3D10Texture2D*			  mD3D10TextureCube;				///< Direct3D 10 texture cube resource, can be a null pointer
		ID3D10ShaderResourceView*	  mD3D10ShaderResourceViewTexture;	///< Direct3D 10 shader resource view, can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D10Renderer/Texture/TextureCube.inl"
