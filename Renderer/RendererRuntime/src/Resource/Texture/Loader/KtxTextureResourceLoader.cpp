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


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void KtxTextureResourceLoader::onDeserialization(IFile& file)
	{
		// TODO(co) Add optional top mipmap removal support (see "RendererRuntime::TextureResourceManager::NumberOfTopMipmapsToRemove")

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
		KtxHeader ktxHeader;
		file.read(&ktxHeader, sizeof(KtxHeader));
		file.skip(ktxHeader.bytesOfKeyValueData);
		mWidth = ktxHeader.pixelWidth;
		mHeight = ktxHeader.pixelHeight;

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

		// Get the size of the compressed image
		mNumberOfUsedImageDataBytes = 0;
		{
			uint32_t width = mWidth;
			uint32_t height = mHeight;
			for (uint32_t mipmap = 0; mipmap < ktxHeader.numberOfMipmapLevels; ++mipmap)
			{
				if (ktxHeader.glInternalFormat == GL_ETC1_RGB8_OES)
				{
					mNumberOfUsedImageDataBytes += std::max((width * height) >> 1, 8u);
				}
				else if (ktxHeader.glInternalFormat == GL_RGBA8)
				{
					mNumberOfUsedImageDataBytes += (width * height * 4);
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

			// Read the image data
			file.read(currentImageData, imageSize);

			// An mipmap level data might have padding bytes (up to 3) formular from https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
			const uint32_t paddingBytes = 3 - ((imageSize + 3) % 4);
			file.skip(paddingBytes);

			// Move on to the next mipmap
			currentImageData += imageSize;
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
		if (1 == mWidth || 1 == mHeight)
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
