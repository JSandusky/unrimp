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
#include "RendererRuntime/Resource/Texture/EtcTextureResourceSerializer.h"
#include "RendererRuntime/IRendererRuntime.h"

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
	Renderer::ITexture* EtcTextureResourceSerializer::loadEtcTexture(std::istream& istream)
	{
		// TODO(co) Error handling
		// TODO(co) We could get rid of the dynamic allocation when using a reused memory for the file and the destination buffer (had no measurable impact in a simple use-case, only do it when it can be shown that it has a positive effect)

		// Read in the image header
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
		KtxHeader ktxHeader;
		istream.read(reinterpret_cast<char*>(&ktxHeader), sizeof(KtxHeader));
		istream.ignore(ktxHeader.bytesOfKeyValueData);

		// Get the size of the compressed image
		size_t compressedNumberOfBytes = 0;
		{
			uint32_t width = ktxHeader.pixelWidth;
			uint32_t height = ktxHeader.pixelHeight;
			for (uint32_t mipmap = 0; mipmap < ktxHeader.numberOfMipmapLevels; ++mipmap)
			{
				compressedNumberOfBytes += std::max((width * height) >> 1, 8u);
				width = std::max(width >> 1, 1u);	// /= 2
				height = std::max(height >> 1, 1u);	// /= 2
			}
		}

		// Load in the image data
		uint8_t* imageData = new uint8_t[compressedNumberOfBytes];
		uint8_t* currentImageData = imageData;
		uint32_t width = ktxHeader.pixelWidth;
		uint32_t height = ktxHeader.pixelHeight;
		for (uint32_t mipmap = 0; mipmap < ktxHeader.numberOfMipmapLevels; ++mipmap)
		{
			uint32_t imageSize = 0;
			istream.read(reinterpret_cast<char*>(&imageSize), sizeof(uint32_t));
			istream.read(reinterpret_cast<char*>(currentImageData), imageSize);

			// Move on to the next mipmap
			currentImageData += imageSize;
			width = std::max(width >> 1, 1u);	// /= 2
			height = std::max(height >> 1, 1u);	// /= 2
		}

		// Create the renderer texture instance
		Renderer::ITexture2D* texture2D = mRendererRuntime.getRenderer().createTexture2D(ktxHeader.pixelWidth, ktxHeader.pixelHeight, Renderer::TextureFormat::ETC1, imageData, Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);

		// Free allocated memory
		delete [] imageData;

		// Done
		return texture2D;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
