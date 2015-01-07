/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "RendererRuntime/Resource/Texture/Loader/KtxTextureResourceLoader.h"
#include "RendererRuntime/Resource/Texture/TextureResource.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <fstream>
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
	ResourceLoaderTypeId KtxTextureResourceLoader::getResourceLoaderTypeId() const
	{
		return TYPE_ID;
	}

	void KtxTextureResourceLoader::onDeserialization()
	{
		// TODO(co) Error handling
		try
		{
			std::ifstream ifstream(mAsset.assetFilename, std::ios::binary);

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
			ifstream.read(reinterpret_cast<char*>(&ktxHeader), sizeof(KtxHeader));
			ifstream.ignore(ktxHeader.bytesOfKeyValueData);
			mWidth = ktxHeader.pixelWidth;
			mHeight = ktxHeader.pixelHeight;
			mTextureFormat = Renderer::TextureFormat::ETC1;	// TODO(co) Make this dynamic

			// Get the size of the compressed image
			mNumberOfUsedImageDataBytes = 0;
			{
				uint32_t width = mWidth;
				uint32_t height = mHeight;
				for (uint32_t mipmap = 0; mipmap < ktxHeader.numberOfMipmapLevels; ++mipmap)
				{
					mNumberOfUsedImageDataBytes += std::max((width * height) >> 1, 8u);
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
				ifstream.read(reinterpret_cast<char*>(&imageSize), sizeof(uint32_t));
				ifstream.read(reinterpret_cast<char*>(currentImageData), imageSize);

				// Move on to the next mipmap
				currentImageData += imageSize;
				width = std::max(width >> 1, 1u);	// /= 2
				height = std::max(height >> 1, 1u);	// /= 2
			}
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load texture asset %d: %s", mAsset.assetId, e.what());
		}
	}

	void KtxTextureResourceLoader::onProcessing()
	{
		// Nothing here
	}

	void KtxTextureResourceLoader::onRendererBackendDispatch()
	{
		// Create the renderer texture instance
		mTextureResource->mTexture = mRendererRuntime.getRenderer().createTexture2D(mWidth, mHeight, static_cast<Renderer::TextureFormat::Enum>(mTextureFormat), mImageData, Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
