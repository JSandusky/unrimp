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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/Texture/Loader/KtxTextureResourceLoader.h"
#include "RendererRuntime/Resource/Texture/TextureResource.h"
#include "RendererRuntime/Core/File/IFile.h"
#include "RendererRuntime/IRendererRuntime.h"

// TODO(sw) Replace with own defines when we know all format types we want to support loading?
#include <GL/gl.h>
#include <GLES3/gl3.h>  // Needed for the internal format type definitions
#include <GLES3/gl2ext.h> // Needed for the GL_ETC1_RGB8_OES internal format type definition

#include <algorithm>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId KtxTextureResourceLoader::TYPE_ID("ktx");
	
	// Read in the image header
	#pragma pack(push)
	#pragma pack(1)
		struct KtxHeader
		{
			uint8_t  identifier[12];
			uint32_t endianness;
			uint32_t glType;
			uint32_t glTypeSize;
			uint32_t glFormat;
			uint32_t glInternalFormat;
			uint32_t glBaseInternalFormat;
			uint32_t pixelWidth;
			uint32_t pixelHeight;
			uint32_t pixelDepth;
			uint32_t numberOfArrayElements;
			uint32_t numberOfFaces;
			uint32_t numberOfMipmapLevels;
			uint32_t bytesOfKeyValueData;
		};
	#pragma pack(pop)
		
	
	// This codes are base of codes from https://github.com/KhronosGroup/KTX/tree/master/lib
	// TODO(sw) cleanup and simplify it when possible
	struct KTX_texinfo {
		/* Data filled in by _ktxCheckHeader() */
		uint32_t textureDimensions;
		uint32_t glTarget;
		uint32_t compressed;
		uint32_t generateMipmaps;
	};

	#define KTX_IDENTIFIER_REF  { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A }
	#define KTX_ENDIAN_REF      (0x04030201)
	#define KTX_ENDIAN_REF_REV  (0x01020304)
	#define KTX_HEADER_SIZE	(64)

	static void _ktxSwapEndian16(uint16_t* pData16, int count)
	{
		int i;
		for (i = 0; i < count; ++i)
		{
			uint16_t x = *pData16;
			*pData16++ = (x << 8) | (x >> 8);
		}
	}

	/*
	* SwapEndian32: Swaps endianness in an array of 32-bit values
	*/
	static void _ktxSwapEndian32(uint32_t* pData32, int count)
	{
		int i;
		for (i = 0; i < count; ++i)
		{
			uint32_t x = *pData32;
			*pData32++ = (x << 24) | ((x & 0xFF00) << 8) | ((x & 0xFF0000) >> 8) | (x >> 24);
		}
	}

	static bool checkHeader(KtxHeader& header, KTX_texinfo& texinfo)
	{
		uint8_t identifier_reference[12] = KTX_IDENTIFIER_REF;
		khronos_uint32_t max_dim;

		/* Compare identifier, is this a KTX file? */
		if (memcmp(header.identifier, identifier_reference, 12) != 0)
		{
			// KTX_UNKNOWN_FILE_FORMAT;
			return false;
		}

		if (header.endianness == KTX_ENDIAN_REF_REV)
		{
			/* Convert endianness of header fields. */
			_ktxSwapEndian32(&header.glType, 12);

			if (header.glTypeSize != 1 &&
				header.glTypeSize != 2 &&
				header.glTypeSize != 4)
			{
				/* Only 8-, 16-, and 32-bit types supported so far. */
				//KTX_INVALID_VALUE
				return false;
			}
		}
		else if (header.endianness != KTX_ENDIAN_REF)
		{
			// KTX_INVALID_VALUE
			return false;
		}

		/* Check glType and glFormat */
		texinfo.compressed = 0;
		if (header.glType == 0 || header.glFormat == 0)
		{
			if (header.glType + header.glFormat != 0)
			{
				/* either both or none of glType, glFormat must be zero */
				// KTX_INVALID_VALUE;
				return false;
			}
			texinfo.compressed = 1;
		}

		/* Check texture dimensions. KTX files can store 8 types of textures:
		1D, 2D, 3D, cube, and array variants of these. There is currently
		no GL extension for 3D array textures. */
		if ((header.pixelWidth == 0) ||
			(header.pixelDepth > 0 && header.pixelHeight == 0))
		{
			/* texture must have width */
			/* texture must have height if it has depth */
			// KTX_INVALID_VALUE;
			return false; 
		}

		texinfo.textureDimensions = 1;
		texinfo.glTarget = GL_TEXTURE_1D;
		texinfo.generateMipmaps = 0;
		if (header.pixelHeight > 0)
		{
			texinfo.textureDimensions = 2;
			texinfo.glTarget = GL_TEXTURE_2D;
		}
		if (header.pixelDepth > 0)
		{
			texinfo.textureDimensions = 3;
			texinfo.glTarget = GL_TEXTURE_3D;
		}

		if (header.numberOfFaces == 6)
		{
			if (texinfo.textureDimensions == 2)
			{
				texinfo.glTarget = GL_TEXTURE_CUBE_MAP;
			}
			else
			{
				// KTX_INVALID_VALUE;
				return false; 
			}
		}
		else if (header.numberOfFaces != 1)
		{
			/* numberOfFaces must be either 1 or 6 */
			// KTX_INVALID_VALUE;
			return false; 
		}
		
		/* Check number of mipmap levels */
		if (header.numberOfMipmapLevels == 0)
		{
			texinfo.generateMipmaps = 1;
			header.numberOfMipmapLevels = 1;
		}
		/* This test works for arrays too because height or depth will be 0. */
		max_dim = std::max(std::max(header.pixelWidth, header.pixelHeight), header.pixelDepth);
		if (max_dim < ((uint32_t)1 << (header.numberOfMipmapLevels - 1)))
		{
			/* Can't have more mip levels than 1 + log2(max(width, height, depth)) */
			// KTX_INVALID_VALUE;
			return false; 
		}

		return true;

	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void KtxTextureResourceLoader::onDeserialization(IFile& file)
	{
		// TODO(co) Add optional top mipmap removal support (see "RendererRuntime::TextureResourceManager::NumberOfTopMipmapsToRemove")

		KtxHeader ktxHeader;
		file.read(&ktxHeader, sizeof(KtxHeader));

		KTX_texinfo texInfo;
		if (!checkHeader(ktxHeader, texInfo))
		{
			assert(false && "ktx header invalid");
		}

		file.skip(ktxHeader.bytesOfKeyValueData);
		mWidth = ktxHeader.pixelWidth;
		mHeight = ktxHeader.pixelHeight;

		// Check if the file contains data for one texture or for 6 textures
		if (ktxHeader.numberOfFaces != 1 && ktxHeader.numberOfFaces != 6 )
		{
			assert(false && "Don't support more then one faces or exactly 6 faces");
		}

		if (ktxHeader.glFormat == 0)
		{
			// When glFormat == 0 -> compressed version
			// For now we only support ETC1 compression
			if (ktxHeader.glInternalFormat != GL_ETC1_RGB8_OES)
			{
				assert(false && "unsupported compressed glInternalFormat");
			}
			mTextureFormat = Renderer::TextureFormat::ETC1;
		}
		else
		{
			// glInternalFormat defines the format of the texture
			// For now we only support R8G8B8A8
			if (ktxHeader.glInternalFormat == GL_RGBA8)
			{
				mTextureFormat = Renderer::TextureFormat::R8G8B8A8;
			}
			else
			{
				assert(false && "unsupported uncompressed glInternalFormat");
			}
		}

		// Does the data contain mipmaps?
		mDataContainsMipmaps = (ktxHeader.numberOfMipmapLevels > 1);
		mCubeMap = (ktxHeader.numberOfFaces > 1);

		// Get the size of the compressed image
		mNumberOfUsedImageDataBytes = 0;
		{
			uint32_t width = mWidth;
			uint32_t height = mHeight;
			for (uint32_t mipmap = 0; mipmap < ktxHeader.numberOfMipmapLevels; ++mipmap)
			{
				for (uint32_t face = 0; face < ktxHeader.numberOfFaces; ++face)
				{
					if (ktxHeader.glInternalFormat == GL_ETC1_RGB8_OES)
					{
						mNumberOfUsedImageDataBytes += std::max((width * height) >> 1, 8u);
					}
					else if (ktxHeader.glInternalFormat == GL_RGBA8)
					{
						mNumberOfUsedImageDataBytes += (width * height * 4);
					}
				}
				width = std::max(width >> 1, 1u);	// /= 2
				height = std::max(height >> 1, 1u);	// /= 2
			}
		}
		if (mNumberOfImageDataBytes < mNumberOfUsedImageDataBytes)
		{
			mNumberOfImageDataBytes = mNumberOfUsedImageDataBytes;
			delete [] mImageData;
			mImageData = new uint8_t[mNumberOfImageDataBytes];
		}

		// Load in the image data
		uint8_t* currentImageData = mImageData;
		uint32_t width = mWidth;
		uint32_t height = mHeight;
		for (uint32_t mipmap = 0; mipmap < ktxHeader.numberOfMipmapLevels; ++mipmap)
		{
			uint32_t imageSize = 0;
			file.read(&imageSize, sizeof(uint32_t));

			// Perform endianness conversion on image size data
			if (ktxHeader.endianness == KTX_ENDIAN_REF_REV)
			{
				_ktxSwapEndian32(&imageSize, 1);
			}

			for (uint32_t face = 0; face < ktxHeader.numberOfFaces; ++face)
			{
				// Read the image data per face
				file.read(currentImageData, imageSize);

				// Perform endianness conversion on texture data
				if (ktxHeader.endianness == KTX_ENDIAN_REF_REV && ktxHeader.glTypeSize == 2)
				{
					_ktxSwapEndian16((uint16_t*)currentImageData, imageSize / 2);
				}
				else if (ktxHeader.endianness == KTX_ENDIAN_REF_REV && ktxHeader.glTypeSize == 4)
				{
					_ktxSwapEndian32((uint32_t*)currentImageData, imageSize / 4);
				}

				// Move on to the next face of the current mipmap
				currentImageData += imageSize;
			}

			// An mipmap level data might have padding bytes (up to 3) formular from https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
			const uint32_t paddingBytes = 3 - ((imageSize + 3) % 4);
			file.skip(paddingBytes);

			// Move on to the next mipmap
			width = std::max(width >> 1, 1u);	// /= 2
			height = std::max(height >> 1, 1u);	// /= 2
		}

		// Can we create the renderer resource asynchronous as well?
		if (mRendererRuntime.getRenderer().getCapabilities().nativeMultiThreading)
		{
			mTexture = createRendererTexture();
		}
	}

	bool KtxTextureResourceLoader::onDispatch()
	{
		// Create the renderer texture instance
		mTextureResource->setTexture(*(mRendererRuntime.getRenderer().getCapabilities().nativeMultiThreading ? mTexture : createRendererTexture()));

		// Fully loaded
		return true;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	Renderer::ITexture* KtxTextureResourceLoader::createRendererTexture()
	{
		Renderer::ITexture* texture = nullptr;
		if (mCubeMap)
		{
			// Cube texture
			texture = mRendererRuntime.getTextureManager().createTextureCube(mWidth, mHeight, static_cast<Renderer::TextureFormat::Enum>(mTextureFormat), mImageData, mDataContainsMipmaps ? Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS : 0u);
		}
		else if (1 == mWidth || 1 == mHeight)
		{
			// 1D texture
			texture = mRendererRuntime.getTextureManager().createTexture1D((1 == mWidth) ? mHeight : mWidth, static_cast<Renderer::TextureFormat::Enum>(mTextureFormat), mImageData, mDataContainsMipmaps ? Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS : 0u);
		}
		else
		{
			// 2D texture
			texture = mRendererRuntime.getTextureManager().createTexture2D(mWidth, mHeight, static_cast<Renderer::TextureFormat::Enum>(mTextureFormat), mImageData, mDataContainsMipmaps ? Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS : 0u);
		}
		RENDERER_SET_RESOURCE_DEBUG_NAME(texture, getAsset().assetFilename)
		return texture;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
