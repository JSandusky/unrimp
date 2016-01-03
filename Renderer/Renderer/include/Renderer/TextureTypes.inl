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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline bool TextureFormat::isCompressed(Enum textureFormat)
	{
		static bool MAPPING[] =
		{
			false,	// Renderer::TextureFormat::A8            - 8-bit pixel format, all bits alpha
			false,	// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			false,	// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			false,	// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			false,	// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha)
			true,	// Renderer::TextureFormat::BC1           - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block) - when being uncompressed
			true,	// Renderer::TextureFormat::BC2           - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - when being uncompressed
			true,	// Renderer::TextureFormat::BC3           - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - when being uncompressed
			true,	// Renderer::TextureFormat::BC4           - 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block) - when being uncompressed
			true,	// Renderer::TextureFormat::BC5           - 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block) - when being uncompressed
			true,	// Renderer::TextureFormat::ETC1          - 3 component texture compression meant for mobile devices
			false,	// Renderer::TextureFormat::D32_FLOAT     - 32-bit float depth format
			false	// Renderer::TextureFormat::UNKNOWN       - Unknown
		};
		return MAPPING[textureFormat];
	}

	inline uint32_t TextureFormat::getNumberOfBytesPerElement(Enum textureFormat)
	{
		static const uint32_t MAPPING[] =
		{
			sizeof(uint8_t),		// Renderer::TextureFormat::A8            - 8-bit pixel format, all bits alpha
			sizeof(uint8_t) * 3,	// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			sizeof(uint8_t) * 4,	// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			sizeof(float) * 2,		// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			sizeof(float) * 4,		// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha)
			sizeof(uint8_t) * 3,	// Renderer::TextureFormat::BC1           - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block) - when being uncompressed
			sizeof(uint8_t) * 4,	// Renderer::TextureFormat::BC2           - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - when being uncompressed
			sizeof(uint8_t) * 4,	// Renderer::TextureFormat::BC3           - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - when being uncompressed
			sizeof(uint8_t) * 1,	// Renderer::TextureFormat::BC4           - 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block) - when being uncompressed
			sizeof(uint8_t) * 2,	// Renderer::TextureFormat::BC5           - 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block) - when being uncompressed
			sizeof(uint8_t) * 3,	// Renderer::TextureFormat::ETC1          - 3 component texture compression meant for mobile devices - when being uncompressed
			sizeof(float),			// Renderer::TextureFormat::D32_FLOAT     - 32-bit float depth format
			0						// Renderer::TextureFormat::UNKNOWN       - Unknown
		};
		return MAPPING[textureFormat];
	}

	inline uint32_t TextureFormat::getNumberOfBytesPerRow(Enum textureFormat, uint32_t width)
	{
		switch (textureFormat)
		{
			// 8-bit pixel format, all bits alpha
			case A8:
				return width;

			// 24-bit pixel format, 8 bits for red, green and blue
			case R8G8B8:
				return 3 * width;

			// 32-bit pixel format, 8 bits for red, green, blue and alpha
			case R8G8B8A8:
				return 4 * width;

			// 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			case R16G16B16A16F:
				return 8 * width;

			// 128-bit float format using 32 bits for the each channel (red, green, blue, alpha)
			case R32G32B32A32F:
				return 16 * width;

			// DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block)
			case BC1:
				return ((width + 3) >> 2) * 8;

			// DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			case BC2:
				return ((width + 3) >> 2) * 16;

			// DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			case BC3:
				return ((width + 3) >> 2) * 16;

			// 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block)
			case BC4:
				return ((width + 3) >> 2) * 8;

			// 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block)
			case BC5:
				return ((width + 3) >> 2) * 16;

			// 3 component texture compression meant for mobile devices
			case ETC1:
				return (width >> 1);

			// 32-bit float depth format
			case D32_FLOAT:
				return sizeof(float) * width;

			// Unknown
			case UNKNOWN:
				return 0;

			default:
				return 0;
		}
	}

	inline uint32_t TextureFormat::getNumberOfBytesPerSlice(Enum textureFormat, uint32_t width, uint32_t height)
	{
		switch (textureFormat)
		{
			// 8-bit pixel format, all bits alpha
			case A8:
				return width * height;

			// 24-bit pixel format, 8 bits for red, green and blue
			case R8G8B8:
				return 3 * width * height;

			// 32-bit pixel format, 8 bits for red, green, blue and alpha
			case R8G8B8A8:
				return 4 * width * height;

			// 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			case R16G16B16A16F:
				return 8 * width * height;

			// 128-bit float format using 32 bits for the each channel (red, green, blue, alpha)
			case R32G32B32A32F:
				return 16 * width * height;

			// DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block)
			case BC1:
				return ((width + 3) >> 2) * ((height + 3) >> 2) * 8;

			// DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			case BC2:
				return ((width + 3) >> 2) * ((height + 3) >> 2) * 16;

			// DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			case BC3:
				return ((width + 3) >> 2) * ((height + 3) >> 2) * 16;

			// 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block)
			case BC4:
				return ((width + 3) >> 2) * ((height + 3) >> 2) * 8;

			// 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block)
			case BC5:
				return ((width + 3) >> 2) * ((height + 3) >> 2) * 16;

			// 3 component texture compression meant for mobile devices
			case ETC1:
			{
				// We could use "std::max()", but then we would need to include <algorithm> in here (not worth it, stay lightweight in here)
				const uint32_t numberOfBytesPerSlice = (width * height) >> 1;
				return (numberOfBytesPerSlice > 8) ? numberOfBytesPerSlice : 8;
			}

			// 32-bit float depth format
			case D32_FLOAT:
				return sizeof(float) * width * height;

			// Unknown
			case UNKNOWN:
				return 0;

			default:
				return 0;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
