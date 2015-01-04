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
#include "RendererRuntime/Resource/Texture/CrnTextureResourceSerializer.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <Renderer/Public/Renderer.h>

// #define CRND_HEADER_FILE_ONLY
//#define CRND_INCLUDE_CRND_H
#include <crunch/crn_decomp.h>
#include <crunch/dds_defs.h>

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4548)	// warning C4548: expression before comma has no effect; expected expression with side-effect
	#include <fstream>
#pragma warning(pop)
#include <algorithm>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	// TODO(co) Work-in-progress
	Renderer::ITexture* CrnTextureResourceSerializer::loadCrnTexture(std::istream& istream)
	{
		// TODO(co) We could get rid of the dynamic allocation when using a reused memory for the file and the destination buffer (had no measurable impact in a simple use-case, only do it when it can be shown that it has a positive effect)

		// Load the source image file into memory
		crn_uint32 src_file_size = 0;
		crn_uint8 *pSrc_file_data = nullptr;
		{
			// Get file size and file data
			istream.seekg(0, std::istream::end);
			src_file_size = static_cast<crn_uint32>(istream.tellg());
			istream.seekg(0, std::istream::beg);
			pSrc_file_data = new crn_uint8[static_cast<size_t>(src_file_size)];
			istream.read((char*)pSrc_file_data, src_file_size);
		}

		// Decompress/transcode CRN to DDS. 
		// DDS files are organized in face-major order, like this:
		//  Face0: Mip0, Mip1, Mip2, etc.
		//  Face1: Mip0, Mip1, Mip2, etc.
		//  etc.
		// While CRN files are organized in mip-major order, like this:
		//  Mip0: Face0, Face1, Face2, Face3, Face4, Face5
		//  Mip1: Face0, Face1, Face2, Face3, Face4, Face5
		//  etc.
		crnd::crn_texture_info tex_info;
		if (!crnd::crnd_get_texture_info(pSrc_file_data, src_file_size, &tex_info))
		{
			delete [] pSrc_file_data;
			//return error("crnd_get_texture_info() failed!\n");
			return nullptr;
		}

		// Get the renderer texture format
		Renderer::TextureFormat::Enum textureFormat = Renderer::TextureFormat::BC1;
		switch (tex_info.m_format)
		{
			// DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block)
			case cCRNFmtDXT1:
				textureFormat = Renderer::TextureFormat::BC1;
				break;

			// DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			case cCRNFmtDXT3:
				textureFormat = Renderer::TextureFormat::BC2;
				break;

			// DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			case cCRNFmtDXT5:
			case cCRNFmtDXT5_CCxY:
			case cCRNFmtDXT5_xGxR:
			case cCRNFmtDXT5_xGBR:
			case cCRNFmtDXT5_AGBR:
				textureFormat = Renderer::TextureFormat::BC3;
				break;

			// 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block)
			case cCRNFmtDXN_XY:
			case cCRNFmtDXN_YX:
				textureFormat = Renderer::TextureFormat::BC5;
				break;

			case cCRNFmtETC1:
				break;

			default:
				// Error!
				// TODO(co)
				return nullptr;
		}

		crnd::crnd_unpack_context pContext = crnd::crnd_unpack_begin(pSrc_file_data, src_file_size);
		if (!pContext)
		{
			delete [] pSrc_file_data;
		//	return error("crnd_unpack_begin() failed!\n");
			return nullptr;
		}

		// Allocate resulting image data
		const crn_uint32 bytesPerDxtBlock = crnd::crnd_get_bytes_per_dxt_block(tex_info.m_format);
		uint8_t* imageData = nullptr;
		{
			size_t compressedSize = 0;
			for (crn_uint32 level_index = 0; level_index < tex_info.m_levels; level_index++)
			{
				const crn_uint32 width = std::max(1U, tex_info.m_width >> level_index);
				const crn_uint32 height = std::max(1U, tex_info.m_height >> level_index);
				const crn_uint32 blocks_x = std::max(1U, (width + 3) >> 2);
				const crn_uint32 blocks_y = std::max(1U, (height + 3) >> 2);
				const crn_uint32 row_pitch = blocks_x * bytesPerDxtBlock;
				const crn_uint32 total_face_size = row_pitch * blocks_y;
				compressedSize += total_face_size;
			}
			compressedSize *= tex_info.m_faces;
			imageData = new uint8_t[compressedSize];
		}

		{ // Now transcode all face and mipmap levels into memory, one mip level at a time.
			uint8_t* currentImageData = imageData;
			for (crn_uint32 level_index = 0; level_index < tex_info.m_levels; level_index++)
			{
				// Compute the face's width, height, number of DXT blocks per row/col, etc.
				const crn_uint32 width = std::max(1U, tex_info.m_width >> level_index);
				const crn_uint32 height = std::max(1U, tex_info.m_height >> level_index);
				const crn_uint32 blocks_x = std::max(1U, (width + 3) >> 2);
				const crn_uint32 blocks_y = std::max(1U, (height + 3) >> 2);
				const crn_uint32 row_pitch = blocks_x * bytesPerDxtBlock;
				const crn_uint32 total_face_size = row_pitch * blocks_y;

				// Prepare the face pointer array needed by crnd_unpack_level().
				// TODO(co) We could get rid of this pointer list by changing the "crunch"-library implementation
				void *pDecomp_images[cCRNMaxFaces];
				for (crn_uint32 face_index = 0; face_index < tex_info.m_faces; ++face_index, currentImageData += total_face_size)
				{
					pDecomp_images[face_index] = currentImageData;
				}

				// Now transcode the level to raw DXTn
				if (!crnd::crnd_unpack_level(pContext, pDecomp_images, total_face_size, row_pitch, level_index))
				{
					// Free allocated memory
					delete [] imageData;
					crnd::crnd_unpack_end(pContext);
					delete [] pSrc_file_data;

					// return error("Failed transcoding texture!");
					return nullptr;
				}
			}
		}

		// Create the renderer texture instance
		Renderer::ITexture2D* texture2D = mRendererRuntime.getRenderer().createTexture2D(tex_info.m_width, tex_info.m_height, textureFormat, imageData, Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);   

		// Free allocated memory
		delete [] imageData;
		crnd::crnd_unpack_end(pContext);
		delete [] pSrc_file_data;

		// Done
		return texture2D;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
