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
#include "Direct3D9Renderer/Mapping.h"
#include "Direct3D9Renderer/d3d9.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	uint32_t Mapping::getDirect3D9MagFilterMode(Renderer::FilterMode filterMode)
	{
		switch (filterMode)
		{
			case Renderer::FilterMode::MIN_MAG_MIP_POINT:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::MIN_MAG_POINT_MIP_LINEAR:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::MIN_POINT_MAG_LINEAR_MIP_POINT:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::MIN_POINT_MAG_MIP_LINEAR:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::MIN_LINEAR_MAG_MIP_POINT:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::MIN_MAG_LINEAR_MIP_POINT:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::MIN_MAG_MIP_LINEAR:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::ANISOTROPIC:
				return D3DTEXF_ANISOTROPIC;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_POINT:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_LINEAR:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::COMPARISON_ANISOTROPIC:
				return D3DTEXF_LINEAR;

			default:
				return D3DTEXF_POINT;	// We should never be in here
		}
	}

	uint32_t Mapping::getDirect3D9MinFilterMode(Renderer::FilterMode filterMode)
	{
		switch (filterMode)
		{
			case Renderer::FilterMode::MIN_MAG_MIP_POINT:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::MIN_MAG_POINT_MIP_LINEAR:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::MIN_POINT_MAG_LINEAR_MIP_POINT:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::MIN_POINT_MAG_MIP_LINEAR:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::MIN_LINEAR_MAG_MIP_POINT:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::MIN_MAG_LINEAR_MIP_POINT:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::MIN_MAG_MIP_LINEAR:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::ANISOTROPIC:
				return D3DTEXF_ANISOTROPIC;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_POINT:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_LINEAR:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::COMPARISON_ANISOTROPIC:
				return D3DTEXF_ANISOTROPIC;

			default:
				return D3DTEXF_POINT;	// We should never be in here
		}
	}

	uint32_t Mapping::getDirect3D9MipFilterMode(Renderer::FilterMode filterMode)
	{
		switch (filterMode)
		{
			case Renderer::FilterMode::MIN_MAG_MIP_POINT:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::MIN_MAG_POINT_MIP_LINEAR:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::MIN_POINT_MAG_LINEAR_MIP_POINT:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::MIN_POINT_MAG_MIP_LINEAR:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::MIN_LINEAR_MAG_MIP_POINT:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::MIN_MAG_LINEAR_MIP_POINT:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::MIN_MAG_MIP_LINEAR:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::ANISOTROPIC:
				return D3DTEXF_ANISOTROPIC;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_POINT:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return D3DTEXF_POINT;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_LINEAR:
				return D3DTEXF_LINEAR;

			case Renderer::FilterMode::COMPARISON_ANISOTROPIC:
				return D3DTEXF_ANISOTROPIC;

			default:
				return D3DTEXF_POINT;	// We should never be in here
		}
	}

	uint32_t Mapping::getDirect3D9TextureAddressMode(Renderer::TextureAddressMode textureAddressMode)
	{
		static const uint32_t MAPPING[] =
		{
			D3DTADDRESS_WRAP,		// Renderer::TextureAddressMode::WRAP
			D3DTADDRESS_MIRROR,		// Renderer::TextureAddressMode::MIRROR
			D3DTADDRESS_CLAMP,		// Renderer::TextureAddressMode::CLAMP
			D3DTADDRESS_BORDER,		// Renderer::TextureAddressMode::BORDER
			D3DTADDRESS_MIRRORONCE	// Renderer::TextureAddressMode::MIRROR_ONCE
		};
		return MAPPING[static_cast<int>(textureAddressMode) - 1];	// Lookout! The "Renderer::TextureAddressMode"-values start with 1, not 0
	}

	/*TODO(co)
	GLint Mapping::getDirect3D9ComparisonFunc(Renderer::ComparisonFunc comparisonFunc)
	{
		static const GLint MAPPING[] =
		{
			GL_NEVER,		// Renderer::ComparisonFunc::NEVER
			GL_LESS,		// Renderer::ComparisonFunc::LESS
			GL_EQUAL,		// Renderer::ComparisonFunc::EQUAL
			GL_LEQUAL,		// Renderer::ComparisonFunc::LESS_EQUAL
			GL_GREATER,		// Renderer::ComparisonFunc::GREATER
			GL_NOTEQUAL,	// Renderer::ComparisonFunc::NOT_EQUAL
			GL_GEQUAL,		// Renderer::ComparisonFunc::GREATER_EQUAL
			GL_ALWAYS		// Renderer::ComparisonFunc::ALWAYS
		};
		return MAPPING[comparisonFunc];
	}*/

	uint8_t Mapping::getDirect3D9Type(Renderer::VertexAttributeFormat vertexAttributeFormat)
	{
		// BYTE
		static const uint8_t MAPPING[] =
		{
			D3DDECLTYPE_FLOAT1,		// Renderer::VertexAttributeFormat::FLOAT_1
			D3DDECLTYPE_FLOAT2,		// Renderer::VertexAttributeFormat::FLOAT_2
			D3DDECLTYPE_FLOAT3,		// Renderer::VertexAttributeFormat::FLOAT_3
			D3DDECLTYPE_FLOAT4,		// Renderer::VertexAttributeFormat::FLOAT_4
			D3DDECLTYPE_UBYTE4N,	// Renderer::VertexAttributeFormat::R8G8B8A8_UNORM
			D3DDECLTYPE_UBYTE4,		// Renderer::VertexAttributeFormat::R8G8B8A8_UINT
			D3DDECLTYPE_SHORT2,		// Renderer::VertexAttributeFormat::SHORT_2
			D3DDECLTYPE_SHORT4		// Renderer::VertexAttributeFormat::SHORT_4
		};
		return MAPPING[static_cast<int>(vertexAttributeFormat)];
	}

	uint32_t Mapping::getDirect3D9Semantic(const char *semanticName)
	{
		BYTE direct3D9Semantic = D3DDECLUSAGE_POSITION;
		if (0 == stricmp("POSITION", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_POSITION;
		}
		else if (0 == stricmp("BLENDWEIGHT", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_BLENDWEIGHT;
		}
		else if (0 == stricmp("BLENDINDICES", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_BLENDINDICES;
		}
		else if (0 == stricmp("NORMAL", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_NORMAL;
		}
		else if (0 == stricmp("PSIZE", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_PSIZE;
		}
		else if (0 == stricmp("TEXCOORD", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_TEXCOORD;
		}
		else if (0 == stricmp("TANGENT", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_TANGENT;
		}
		else if (0 == stricmp("BINORMAL", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_BINORMAL;
		}
		else if (0 == stricmp("TESSFACTOR", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_TESSFACTOR;
		}
		else if (0 == stricmp("POSITIONT", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_POSITIONT;
		}
		else if (0 == stricmp("COLOR", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_COLOR;
		}
		else if (0 == stricmp("FOG", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_FOG;
		}
		else if (0 == stricmp("DEPTH", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_DEPTH;
		}
		else if (0 == stricmp("SAMPLE", semanticName))
		{
			direct3D9Semantic = D3DDECLUSAGE_SAMPLE;
		}
		return direct3D9Semantic;
	}

	uint32_t Mapping::getDirect3D9Usage(Renderer::BufferUsage bufferUsage)
	{
		// Direct3D 9 only supports a subset of the OpenGL usage indications
		// -> See "D3DUSAGE"-documentation at http://msdn.microsoft.com/en-us/library/windows/desktop/bb172625%28v=vs.85%29.aspx
		switch (bufferUsage)
		{
			case Renderer::BufferUsage::STREAM_DRAW:
			case Renderer::BufferUsage::STREAM_COPY:
			case Renderer::BufferUsage::STATIC_DRAW:
			case Renderer::BufferUsage::STATIC_COPY:
				return D3DUSAGE_WRITEONLY;

			case Renderer::BufferUsage::STREAM_READ:
			case Renderer::BufferUsage::STATIC_READ:
				return 0;

			case Renderer::BufferUsage::DYNAMIC_DRAW:
			case Renderer::BufferUsage::DYNAMIC_COPY:
				return (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);

			default:
			case Renderer::BufferUsage::DYNAMIC_READ:
				return D3DUSAGE_DYNAMIC;
		}
	}

	uint32_t Mapping::getDirect3D9Format(Renderer::IndexBufferFormat::Enum indexBufferFormat)
	{
		// D3DFORMAT
		static const uint32_t MAPPING[] =
		{
			D3DFMT_INDEX32,	// Renderer::IndexBufferFormat::UNSIGNED_CHAR  - One byte per element, uint8_t (may not be supported by each API) - Not supported by Direct3D 9
			D3DFMT_INDEX16,	// Renderer::IndexBufferFormat::UNSIGNED_SHORT - Two bytes per element, uint16_t
			D3DFMT_INDEX32	// Renderer::IndexBufferFormat::UNSIGNED_INT   - Four bytes per element, uint32_t (may not be supported by each API)
		};
		return MAPPING[indexBufferFormat];
	}

	uint32_t Mapping::getDirect3D9Format(Renderer::TextureFormat::Enum textureFormat)
	{
		// D3DFORMAT
		#define MCHAR4(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))
		static const uint32_t MAPPING[] =
		{
			D3DFMT_A8,					// Renderer::TextureFormat::A8            - 8-bit pixel format, all bits alpha
			D3DFMT_X8R8G8B8,			// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue - "D3DFMT_R8G8B8" is usually not supported
			D3DFMT_A8R8G8B8,			// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			D3DFMT_A8R8G8B8,			// Renderer::TextureFormat::R8G8B8A8_SRGB - 32-bit pixel format, 8 bits for red, green, blue and alpha; sRGB = RGB hardware gamma correction, the alpha channel always remains linear	- TODO(co) DirectX 9 sRGB format
			D3DFMT_A16B16G16R16F,		// Renderer::TextureFormat::R11G11B10F    - 32-bit float format using 11 bits the red and green channel, 10 bits the blue channel; red and green channels have a 6 bits mantissa and a 5 bits exponent and blue has a 5 bits mantissa and 5 bits exponent - "DXGI_FORMAT_R11G11B10_FLOAT" doesn't exist in Direct3D 9
			D3DFMT_A16B16G16R16F,		// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			D3DFMT_A32B32G32R32F,		// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha)
			D3DFMT_DXT1,				// Renderer::TextureFormat::BC1           - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block)
			D3DFMT_DXT1,				// Renderer::TextureFormat::BC1_SRGB      - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear	- TODO(co) DirectX 9 sRGB format
			D3DFMT_DXT3,				// Renderer::TextureFormat::BC2           - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			D3DFMT_DXT3,				// Renderer::TextureFormat::BC2_SRGB      - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear	- TODO(co) DirectX 9 sRGB format
			D3DFMT_DXT5,				// Renderer::TextureFormat::BC3           - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			D3DFMT_DXT5,				// Renderer::TextureFormat::BC3_SRGB      - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear	- TODO(co) DirectX 9 sRGB format
			MCHAR4('A', 'T', 'I', '1'),	// Renderer::TextureFormat::BC4           - 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block)
			MCHAR4('A', 'T', 'I', '2'),	// Renderer::TextureFormat::BC5           - 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block)
			D3DFMT_UNKNOWN,				// Renderer::TextureFormat::ETC1          - 3 component texture compression meant for mobile devices - not supported in Direct3D 9
			D3DFMT_R32F,				// Renderer::TextureFormat::R32_FLOAT     - 32-bit float format
			D3DFMT_D32F_LOCKABLE,		// Renderer::TextureFormat::D32_FLOAT     - 32-bit float depth format	TODO(co) Check this
			D3DFMT_UNKNOWN				// Renderer::TextureFormat::UNKNOWN       - Unknown
		};
		#undef MCHAR4
		return MAPPING[textureFormat];
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
