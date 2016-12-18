/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "RendererRuntime/Resource/Texture/Loader/CrnTextureResourceLoader.h"
#include "RendererRuntime/Resource/Texture/TextureResource.h"
#include "RendererRuntime/IRendererRuntime.h"

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4061)	// warning C4061: enumerator 'cCRNFmtTotal' in switch of enum 'crn_format' is not explicitly handled by a case label
	#pragma warning(disable: 4365)	// warning C4365: '<x>': conversion from '<y>' to '<z>', signed/unsigned mismatch
	#pragma warning(disable: 4458)	// warning C4458: declaration of '<x>' hides class member
	#pragma warning(disable: 4548)	// warning C4548: expression before comma has no effect; expected expression with side-effect
	#pragma warning(disable: 4555)	// warning C4555: expression has no effect; expected expression with side-effect
	#pragma warning(disable: 4668)	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '<y>' for '<z>'
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value" // warning: expression result unused [-Wunused-value]
	#include <crunch/crn_decomp.h>
	#include <crunch/dds_defs.h>
#pragma clang diagnostic pop
#pragma warning(pop)

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
	const ResourceLoaderTypeId CrnTextureResourceLoader::TYPE_ID("crn");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void CrnTextureResourceLoader::onDeserialization()
	{
		// TODO(co) Error handling
		try
		{
			std::ifstream inputFileStream(mAsset.assetFilename, std::ios::binary);

			// Load the source image file into memory: Get file size and file data
			inputFileStream.seekg(0, std::istream::end);
			mNumberOfUsedFileDataBytes = static_cast<uint32_t>(inputFileStream.tellg());
			inputFileStream.seekg(0, std::istream::beg);
			if (mNumberOfFileDataBytes < mNumberOfUsedFileDataBytes)
			{
				mNumberOfFileDataBytes = mNumberOfUsedFileDataBytes;
				delete [] mFileData;
				mFileData = new uint8_t[mNumberOfFileDataBytes];
			}
			inputFileStream.read(reinterpret_cast<char*>(mFileData), mNumberOfFileDataBytes);
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load texture asset %d: %s", mAsset.assetId, e.what());
		}
	}

	void CrnTextureResourceLoader::onProcessing()
	{
		// TODO(co) Error handling

		// Decompress/transcode CRN to DDS
		// - DDS files are organized in face-major order, like this:
		//     Face0: Mip0, Mip1, Mip2, etc.
		//     Face1: Mip0, Mip1, Mip2, etc.
		//     etc.
		// - While CRN files are organized in mip-major order, like this:
		//     Mip0: Face0, Face1, Face2, Face3, Face4, Face5
		//     Mip1: Face0, Face1, Face2, Face3, Face4, Face5
		//     etc.
		crnd::crn_texture_info crnTextureInfo;
		if (!crnd::crnd_get_texture_info(mFileData, mNumberOfFileDataBytes, &crnTextureInfo))
		{
			//return error("crnd_get_texture_info() failed!\n");
			return;
		}
		mWidth  = crnTextureInfo.m_width;
		mHeight = crnTextureInfo.m_height;

		// Get the renderer texture format
		switch (crnTextureInfo.m_format)
		{
			// DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block)
			case cCRNFmtDXT1:
				mTextureFormat = Renderer::TextureFormat::BC1;
				break;

			// DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			case cCRNFmtDXT3:
				mTextureFormat = Renderer::TextureFormat::BC2;
				break;

			// DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			case cCRNFmtDXT5:
			case cCRNFmtDXT5_CCxY:
			case cCRNFmtDXT5_xGxR:
			case cCRNFmtDXT5_xGBR:
			case cCRNFmtDXT5_AGBR:
				mTextureFormat = Renderer::TextureFormat::BC3;
				break;

			// 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block)
			case cCRNFmtDXN_XY:
			case cCRNFmtDXN_YX:
				mTextureFormat = Renderer::TextureFormat::BC5;
				break;

			case cCRNFmtETC1:	// Not supported by CRN
			case cCRNFmtTotal:
			case cCRNFmtForceDWORD:
			case cCRNFmtDXT5A:
			default:
				// Error!
				// TODO(co)
				return;
		}

		crnd::crnd_unpack_context crndUnpackContext = crnd::crnd_unpack_begin(mFileData, mNumberOfFileDataBytes);
		if (nullptr == crndUnpackContext)
		{
		//	return error("crnd_unpack_begin() failed!\n");
			return;
		}

		// Allocate resulting image data
		const crn_uint32 numberOfBytesPerDxtBlock = crnd::crnd_get_bytes_per_dxt_block(crnTextureInfo.m_format);
		{
			mNumberOfUsedImageDataBytes = 0;
			for (crn_uint32 levelIndex = 0; levelIndex < crnTextureInfo.m_levels; ++levelIndex)
			{
				const crn_uint32 width = std::max(1U, mWidth >> levelIndex);
				const crn_uint32 height = std::max(1U, mHeight >> levelIndex);
				const crn_uint32 blocksX = std::max(1U, (width + 3) >> 2);
				const crn_uint32 blocksY = std::max(1U, (height + 3) >> 2);
				const crn_uint32 rowPitch = blocksX * numberOfBytesPerDxtBlock;
				const crn_uint32 totalFaceSize = rowPitch * blocksY;
				mNumberOfUsedImageDataBytes += totalFaceSize;
			}
			mNumberOfUsedImageDataBytes *= crnTextureInfo.m_faces;

			if (mNumberOfImageDataBytes < mNumberOfUsedImageDataBytes)
			{
				mNumberOfImageDataBytes = mNumberOfUsedImageDataBytes;
				delete [] mImageData;
				mImageData = new uint8_t[mNumberOfImageDataBytes];
			}
		}

		{ // Now transcode all face and mipmap levels into memory, one mip level at a time
			uint8_t* currentImageData = mImageData;
			for (crn_uint32 levelIndex = 0; levelIndex < crnTextureInfo.m_levels; ++levelIndex)
			{
				// Compute the face's width, height, number of DXT blocks per row/col, etc.
				const crn_uint32 width = std::max(1U, mWidth >> levelIndex);
				const crn_uint32 height = std::max(1U, mHeight >> levelIndex);
				const crn_uint32 blocksX = std::max(1U, (width + 3) >> 2);
				const crn_uint32 blocksY = std::max(1U, (height + 3) >> 2);
				const crn_uint32 rowPitch = blocksX * numberOfBytesPerDxtBlock;
				const crn_uint32 totalFaceSize = rowPitch * blocksY;

				// Prepare the face pointer array needed by "crnd_unpack_level()"
				// TODO(co) We could get rid of this pointer list by changing the "crunch"-library implementation
				void* decompressedImages[cCRNMaxFaces];
				for (crn_uint32 faceIndex = 0; faceIndex < crnTextureInfo.m_faces; ++faceIndex, currentImageData += totalFaceSize)
				{
					decompressedImages[faceIndex] = currentImageData;
				}

				// Now transcode the level to raw DXTn
				if (!crnd::crnd_unpack_level(crndUnpackContext, decompressedImages, totalFaceSize, rowPitch, levelIndex))
				{
					// Free allocated memory
					crnd::crnd_unpack_end(crndUnpackContext);

					// return error("Failed transcoding texture!");
					return;
				}
			}
		}

		// Free allocated memory
		crnd::crnd_unpack_end(crndUnpackContext);
	}

	void CrnTextureResourceLoader::onRendererBackendDispatch()
	{
		// Create the renderer texture instance
		mTextureResource->mTexture = mRendererRuntime.getTextureManager().createTexture2D(mWidth, mHeight, static_cast<Renderer::TextureFormat::Enum>(mTextureFormat), mImageData, Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
