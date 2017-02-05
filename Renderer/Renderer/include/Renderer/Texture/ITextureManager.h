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
#include "Renderer/SmartRefCount.h"
#include "Renderer/Texture/TextureTypes.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRenderer;
	class ITexture1D;
	class ITexture2D;
	class ITexture3D;
	class ITextureCube;
	class ITexture2DArray;
}


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
	*    Abstract texture manager interface
	*
	*  @remarks
	*    The texture manager is responsible for managing fine granular instances of
	*    - 1D texture ("Renderer::ITexture1D")
	*    - 2D texture ("Renderer::ITexture2D")
	*    - 2D texture array ("Renderer::ITexture2DArray")
	*    - 3D texture ("Renderer::ITexture3D")
	*    - Cube texture ("Renderer::ITextureCube")
	*/
	class ITextureManager : public RefCount<ITextureManager>
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~ITextureManager();

		/**
		*  @brief
		*    Return the owner renderer instance
		*
		*  @return
		*    The owner renderer instance, do not release the returned instance unless you added an own reference to it
		*/
		inline IRenderer& getRenderer() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureManager methods      ]
	//[-------------------------------------------------------]
	public:
		//[-------------------------------------------------------]
		//[ Resource creation                                     ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Create a 1D texture instance
		*
		*  @param[in] width
		*    Texture width, must be >0 else a null pointer is returned
		*  @param[in] textureFormat
		*    Texture format
		*  @param[in] data
		*    Texture data, can be a null pointer, the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*  @param[in] textureUsage
		*    Indication of the texture usage, (only relevant for Direct3D, OpenGL has no texture usage indication)
		*
		*  @return
		*    The created 1D texture instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @remarks
		*    The texture data has to be in CRN-texture layout, which means organized in mip-major order, like this:
		*    - Mip0: Face0, Face1, Face2, Face3, Face4, Face5
		*    - Mip1: Face0, Face1, Face2, Face3, Face4, Face5
		*    (DDS-texture layout is using face-major order)
		*/
		virtual ITexture1D *createTexture1D(uint32_t width, TextureFormat::Enum textureFormat, const void *data = nullptr, uint32_t flags = 0, TextureUsage textureUsage = TextureUsage::DEFAULT) = 0;

		/**
		*  @brief
		*    Create a 2D texture instance
		*
		*  @param[in] width
		*    Texture width, must be >0 else a null pointer is returned
		*  @param[in] height
		*    Texture height, must be >0 else a null pointer is returned
		*  @param[in] textureFormat
		*    Texture data format
		*  @param[in] data
		*    Texture data, can be a null pointer, the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*  @param[in] textureUsage
		*    Indication of the texture usage (only relevant for Direct3D, OpenGL has no texture usage indication)
		*  @param[in] numberOfMultisamples
		*    The number of multisamples per pixel (valid values: 1, 2, 4, 8)
		*  @param[in] optimizedTextureClearValue
		*    Optional optimized texture clear value
		*
		*  @return
		*    The created 2D texture instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @remarks
		*    The texture data has to be in CRN-texture layout, which means organized in mip-major order, like this:
		*    - Mip0: Face0, Face1, Face2, Face3, Face4, Face5
		*    - Mip1: Face0, Face1, Face2, Face3, Face4, Face5
		*    (DDS-texture layout is using face-major order)
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumNumberOf2DTextureArraySlices" is not 0
		*/
		virtual ITexture2D *createTexture2D(uint32_t width, uint32_t height, TextureFormat::Enum textureFormat, const void *data = nullptr, uint32_t flags = 0, TextureUsage textureUsage = TextureUsage::DEFAULT, uint8_t numberOfMultisamples = 1, const OptimizedTextureClearValue* optimizedTextureClearValue = nullptr) = 0;

		/**
		*  @brief
		*    Create a 2D array texture instance
		*
		*  @param[in] width
		*    Texture width, must be >0 else a null pointer is returned
		*  @param[in] height
		*    Texture height, must be >0 else a null pointer is returned
		*  @param[in] numberOfSlices
		*    Number of slices, must be >0 else a null pointer is returned
		*  @param[in] textureFormat
		*    Texture format
		*  @param[in] data
		*    Texture data, can be a null pointer, the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*  @param[in] textureUsage
		*    Indication of the texture usage, (only relevant for Direct3D, OpenGL has no texture usage indication)
		*
		*  @return
		*    The created 2D array texture instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @remarks
		*    The texture array data consists of a sequence of texture slices. Each the texture slice data of a single texture slice has to
		*    be in CRN-texture layout, which means organized in mip-major order, like this:
		*    - Mip0: Face0, Face1, Face2, Face3, Face4, Face5
		*    - Mip1: Face0, Face1, Face2, Face3, Face4, Face5
		*    (DDS-texture layout is using face-major order)
		*/
		virtual ITexture2DArray *createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, TextureFormat::Enum textureFormat, const void *data = nullptr, uint32_t flags = 0, TextureUsage textureUsage = TextureUsage::DEFAULT) = 0;

		/**
		*  @brief
		*    Create a 3D texture instance
		*
		*  @param[in] width
		*    Texture width, must be >0 else a null pointer is returned
		*  @param[in] height
		*    Texture height, must be >0 else a null pointer is returned
		*  @param[in] depth
		*    Texture depth, must be >0 else a null pointer is returned
		*  @param[in] textureFormat
		*    Texture format
		*  @param[in] data
		*    Texture data, can be a null pointer, the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*  @param[in] textureUsage
		*    Indication of the texture usage, (only relevant for Direct3D, OpenGL has no texture usage indication)
		*
		*  @return
		*    The created 3D texture instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @remarks
		*    The texture data has to be in CRN-texture layout, which means organized in mip-major order, like this:
		*    - Mip0: Face0, Face1, Face2, Face3, Face4, Face5
		*    - Mip1: Face0, Face1, Face2, Face3, Face4, Face5
		*    (DDS-texture layout is using face-major order)
		*/
		virtual ITexture3D *createTexture3D(uint32_t width, uint32_t height, uint32_t depth, TextureFormat::Enum textureFormat, const void *data = nullptr, uint32_t flags = 0, TextureUsage textureUsage = TextureUsage::DEFAULT) = 0;

		/**
		*  @brief
		*    Create a cube texture instance
		*
		*  @param[in] width
		*    Texture width, must be >0 else a null pointer is returned
		*  @param[in] height
		*    Texture height, must be >0 else a null pointer is returned
		*  @param[in] textureFormat
		*    Texture format
		*  @param[in] data
		*    Texture data, can be a null pointer, the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*  @param[in] textureUsage
		*    Indication of the texture usage, (only relevant for Direct3D, OpenGL has no texture usage indication)
		*
		*  @return
		*    The created cube texture instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @remarks
		*    The texture data has to be in CRN-texture layout, which means organized in mip-major order, like this:
		*    - Mip0: Face0, Face1, Face2, Face3, Face4, Face5
		*    - Mip1: Face0, Face1, Face2, Face3, Face4, Face5
		*    (DDS-texture layout is using face-major order)
		*/
		virtual ITextureCube *createTextureCube(uint32_t width, uint32_t height, TextureFormat::Enum textureFormat, const void *data = nullptr, uint32_t flags = 0, TextureUsage textureUsage = TextureUsage::DEFAULT) = 0;


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
		inline explicit ITextureManager(IRenderer& renderer);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit ITextureManager(const ITextureManager& source);

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		inline ITextureManager& operator =(const ITextureManager& source);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRenderer& mRenderer;	///< The owner renderer instance


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<ITextureManager> ITextureManagerPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/Texture/ITextureManager.inl"
