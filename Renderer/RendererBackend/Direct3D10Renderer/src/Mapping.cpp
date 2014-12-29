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
#include "Direct3D10Renderer/Mapping.h"
#include "Direct3D10Renderer/D3D10.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	uint32_t Mapping::getDirect3D10Format(Renderer::VertexArrayFormat::Enum vertexArrayFormat)
	{
		// DXGI_FORMAT
		static const uint32_t MAPPING[] =
		{
			DXGI_FORMAT_R32_FLOAT,			// Renderer::VertexArrayFormat::FLOAT_1
			DXGI_FORMAT_R32G32_FLOAT,		// Renderer::VertexArrayFormat::FLOAT_2
			DXGI_FORMAT_R32G32B32_FLOAT,	// Renderer::VertexArrayFormat::FLOAT_3
			DXGI_FORMAT_R32G32B32A32_FLOAT,	// Renderer::VertexArrayFormat::FLOAT_4
			DXGI_FORMAT_R8G8B8A8_UINT,		// Renderer::VertexArrayFormat::UNSIGNED_BYTE_4
			DXGI_FORMAT_R16G16_SINT,		// Renderer::VertexArrayFormat::SHORT_2
			DXGI_FORMAT_R16G16B16A16_SINT	// Renderer::VertexArrayFormat::SHORT_4
		};
		return MAPPING[vertexArrayFormat];
	}

	uint32_t Mapping::getDirect3D10UsageAndCPUAccessFlags(Renderer::BufferUsage::Enum bufferUsage, uint32_t &cpuAccessFlags)
	{
		// Direct3D 10 only supports a subset of the OpenGL usage indications
		// -> See "D3D10_USAGE enumeration "-documentation at http://msdn.microsoft.com/en-us/library/windows/desktop/bb172499%28v=vs.85%29.aspx
		switch (bufferUsage)
		{
			case Renderer::BufferUsage::STREAM_DRAW:
			case Renderer::BufferUsage::STREAM_COPY:
			case Renderer::BufferUsage::STATIC_DRAW:
			case Renderer::BufferUsage::STATIC_COPY:
				cpuAccessFlags = 0;
				return D3D10_USAGE_IMMUTABLE;

			case Renderer::BufferUsage::STREAM_READ:
			case Renderer::BufferUsage::STATIC_READ:
				cpuAccessFlags = D3D10_CPU_ACCESS_READ;
				return D3D10_USAGE_STAGING;

			case Renderer::BufferUsage::DYNAMIC_DRAW:
			case Renderer::BufferUsage::DYNAMIC_COPY:
				cpuAccessFlags = D3D10_CPU_ACCESS_WRITE;
				return D3D10_USAGE_DYNAMIC;

			default:
			case Renderer::BufferUsage::DYNAMIC_READ:
				cpuAccessFlags = 0;
				return D3D10_USAGE_DEFAULT;
		}
	}

	uint32_t Mapping::getDirect3D10Format(Renderer::IndexBufferFormat::Enum indexBufferFormat)
	{
		// DXGI_FORMAT
		static const uint32_t MAPPING[] =
		{
			DXGI_FORMAT_R32_UINT,	// Renderer::IndexBufferFormat::UNSIGNED_CHAR  - One byte per element, uint8_t (may not be supported by each API) - Not supported by Direct3D 10
			DXGI_FORMAT_R16_UINT,	// Renderer::IndexBufferFormat::UNSIGNED_SHORT - Two bytes per element, uint16_t
			DXGI_FORMAT_R32_UINT	// Renderer::IndexBufferFormat::UNSIGNED_INT   - Four bytes per element, uint32_t (may not be supported by each API)
		};
		return MAPPING[indexBufferFormat];
	}

	uint32_t Mapping::getDirect3D10Format(Renderer::TextureFormat::Enum textureFormat)
	{
		// DXGI_FORMAT
		static const uint32_t MAPPING[] =
		{
			DXGI_FORMAT_A8_UNORM,			// Renderer::TextureFormat::A8            - 8-bit pixel format, all bits alpha
			DXGI_FORMAT_B8G8R8X8_UNORM,		// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			DXGI_FORMAT_R8G8B8A8_UNORM,		// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			DXGI_FORMAT_R16G16B16A16_FLOAT,	// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			DXGI_FORMAT_R32G32B32A32_FLOAT,	// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha)
			DXGI_FORMAT_BC1_UNORM,			// Renderer::TextureFormat::BC1           - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block)
			DXGI_FORMAT_BC2_UNORM,			// Renderer::TextureFormat::BC2           - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			DXGI_FORMAT_BC3_UNORM,			// Renderer::TextureFormat::BC3           - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			DXGI_FORMAT_BC4_UNORM,			// Renderer::TextureFormat::BC4           - 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block)
			DXGI_FORMAT_BC5_UNORM			// Renderer::TextureFormat::BC5           - 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block)
		};
		return MAPPING[textureFormat];
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
