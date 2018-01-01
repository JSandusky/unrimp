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
#include <Renderer/Texture/ITexture1D.h>
#include <Renderer/Texture/TextureTypes.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct ID3D10Texture1D;
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
	*    Direct3D 10 1D texture class
	*/
	class Texture1D : public Renderer::ITexture1D
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
		*  @param[in] textureFormat
		*    Texture format
		*  @param[in] data
		*    Texture data, can be a null pointer
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*  @param[in] textureUsage
		*    Indication of the texture usage (only relevant for Direct3D, OpenGL has no texture usage indication)
		*/
		Texture1D(Direct3D10Renderer& direct3D10Renderer, uint32_t width, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, Renderer::TextureUsage textureUsage);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Texture1D() override;

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
		*    Return the Direct3D texture 1D resource instance
		*
		*  @return
		*    The Direct3D texture 1D resource instance, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D10Texture1D* getD3D10Texture1D() const;

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
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	protected:
		virtual void selfDestruct() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit Texture1D(const Texture1D& source) = delete;
		Texture1D& operator =(const Texture1D& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Renderer::TextureFormat::Enum mTextureFormat;
		bool						  mGenerateMipmaps;
		ID3D10Texture1D*			  mD3D10Texture1D;					///< Direct3D 10 texture 1D resource, can be a null pointer
		ID3D10ShaderResourceView*	  mD3D10ShaderResourceViewTexture;	///< Direct3D 10 shader resource view, can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D10Renderer/Texture/Texture1D.inl"
