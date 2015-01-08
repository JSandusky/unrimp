/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "OpenGLRenderer/Mapping.h"
#include "OpenGLRenderer/Extensions.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	int Mapping::getOpenGLMagFilterMode(Renderer::FilterMode::Enum filterMode)
	{
		switch (filterMode)
		{
			case Renderer::FilterMode::MIN_MAG_MIP_POINT:
				return GL_NEAREST;

			case Renderer::FilterMode::MIN_MAG_POINT_MIP_LINEAR:
				return GL_NEAREST;

			case Renderer::FilterMode::MIN_POINT_MAG_LINEAR_MIP_POINT:
				return GL_LINEAR;

			case Renderer::FilterMode::MIN_POINT_MAG_MIP_LINEAR:
				return GL_LINEAR;

			case Renderer::FilterMode::MIN_LINEAR_MAG_MIP_POINT:
				return GL_NEAREST;

			case Renderer::FilterMode::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return GL_NEAREST;

			case Renderer::FilterMode::MIN_MAG_LINEAR_MIP_POINT:
				return GL_LINEAR;

			case Renderer::FilterMode::MIN_MAG_MIP_LINEAR:
				return GL_LINEAR;

			case Renderer::FilterMode::ANISOTROPIC:
				return GL_LINEAR;	// There's no special setting in OpenGL

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_POINT:
				return GL_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
				return GL_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
				return GL_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
				return GL_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
				return GL_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return GL_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
				return GL_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_LINEAR:
				return GL_LINEAR;

			case Renderer::FilterMode::COMPARISON_ANISOTROPIC:
				return GL_LINEAR;	// There's no special setting in OpenGL

			default:
				return GL_NEAREST;	// We should never be in here
		}
	}

	int Mapping::getOpenGLMinFilterMode(Renderer::FilterMode::Enum filterMode)
	{
		switch (filterMode)
		{
			case Renderer::FilterMode::MIN_MAG_MIP_POINT:
				return GL_NEAREST_MIPMAP_NEAREST;

			case Renderer::FilterMode::MIN_MAG_POINT_MIP_LINEAR:
				return GL_NEAREST_MIPMAP_LINEAR;

			case Renderer::FilterMode::MIN_POINT_MAG_LINEAR_MIP_POINT:
				return GL_NEAREST_MIPMAP_NEAREST;

			case Renderer::FilterMode::MIN_POINT_MAG_MIP_LINEAR:
				return GL_NEAREST_MIPMAP_LINEAR;

			case Renderer::FilterMode::MIN_LINEAR_MAG_MIP_POINT:
				return GL_LINEAR_MIPMAP_NEAREST;

			case Renderer::FilterMode::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return GL_LINEAR_MIPMAP_LINEAR;

			case Renderer::FilterMode::MIN_MAG_LINEAR_MIP_POINT:
				return GL_LINEAR_MIPMAP_NEAREST;

			case Renderer::FilterMode::MIN_MAG_MIP_LINEAR:
				return GL_LINEAR_MIPMAP_LINEAR;

			case Renderer::FilterMode::ANISOTROPIC:
				return GL_LINEAR_MIPMAP_LINEAR;	// There's no special setting in OpenGL

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_POINT:
				return GL_NEAREST_MIPMAP_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
				return GL_NEAREST_MIPMAP_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
				return GL_NEAREST_MIPMAP_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
				return GL_NEAREST_MIPMAP_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
				return GL_LINEAR_MIPMAP_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return GL_LINEAR_MIPMAP_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
				return GL_LINEAR_MIPMAP_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_LINEAR:
				return GL_LINEAR_MIPMAP_LINEAR;

			case Renderer::FilterMode::COMPARISON_ANISOTROPIC:
				return GL_LINEAR_MIPMAP_LINEAR;	// There's no special setting in OpenGL

			default:
				return GL_NEAREST;	// We should never be in here
		}
	}

	int Mapping::getOpenGLCompareMode(Renderer::FilterMode::Enum filterMode)
	{
		switch (filterMode)
		{
			case Renderer::FilterMode::MIN_MAG_MIP_POINT:
			case Renderer::FilterMode::MIN_MAG_POINT_MIP_LINEAR:
			case Renderer::FilterMode::MIN_POINT_MAG_LINEAR_MIP_POINT:
			case Renderer::FilterMode::MIN_POINT_MAG_MIP_LINEAR:
			case Renderer::FilterMode::MIN_LINEAR_MAG_MIP_POINT:
			case Renderer::FilterMode::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			case Renderer::FilterMode::MIN_MAG_LINEAR_MIP_POINT:
			case Renderer::FilterMode::MIN_MAG_MIP_LINEAR:
			case Renderer::FilterMode::ANISOTROPIC:
				return GL_NONE;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_POINT:
			case Renderer::FilterMode::COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			case Renderer::FilterMode::COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_LINEAR:
			case Renderer::FilterMode::COMPARISON_ANISOTROPIC:
				return GL_COMPARE_REF_TO_TEXTURE;

			default:
				return GL_NEAREST;	// We should never be in here
		}
	}

	int Mapping::getOpenGLTextureAddressMode(Renderer::TextureAddressMode::Enum textureAddressMode)
	{
		static const GLint MAPPING[] =
		{
			GL_REPEAT,			// Renderer::TextureAddressMode::WRAP
			GL_MIRRORED_REPEAT,	// Renderer::TextureAddressMode::MIRROR
			GL_CLAMP_TO_EDGE,	// Renderer::TextureAddressMode::CLAMP
			GL_CLAMP_TO_BORDER,	// Renderer::TextureAddressMode::BORDER
			GL_MIRRORED_REPEAT	// Renderer::TextureAddressMode::MIRROR_ONCE	// TODO(co) OpenGL equivalent? GL_ATI_texture_mirror_once ?
		};
		return MAPPING[textureAddressMode - 1];	// Lookout! The "Renderer::TextureAddressMode::Enum"-values start with 1, not 0
	}

	int Mapping::getOpenGLComparisonFunc(Renderer::ComparisonFunc::Enum comparisonFunc)
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
		return MAPPING[comparisonFunc - 1];	// Lookout! The "Renderer::ComparisonFunc::Enum"-values start with 1, not 0
	}

	int Mapping::getOpenGLSize(Renderer::VertexArrayFormat::Enum vertexArrayFormat)
	{
		static const GLint MAPPING[] =
		{
			1,	// Renderer::VertexArrayFormat::FLOAT_1
			2,	// Renderer::VertexArrayFormat::FLOAT_2
			3,	// Renderer::VertexArrayFormat::FLOAT_3
			4,	// Renderer::VertexArrayFormat::FLOAT_4
			4,	// Renderer::VertexArrayFormat::UNSIGNED_BYTE_4
			2,	// Renderer::VertexArrayFormat::SHORT_2
			4	// Renderer::VertexArrayFormat::SHORT_4
		};
		return MAPPING[vertexArrayFormat];
	}

	uint32_t Mapping::getOpenGLType(Renderer::VertexArrayFormat::Enum vertexArrayFormat)
	{
		static const GLenum MAPPING[] =
		{
			GL_FLOAT,			// Renderer::VertexArrayFormat::FLOAT_1
			GL_FLOAT,			// Renderer::VertexArrayFormat::FLOAT_2
			GL_FLOAT,			// Renderer::VertexArrayFormat::FLOAT_3
			GL_FLOAT,			// Renderer::VertexArrayFormat::FLOAT_4
			GL_UNSIGNED_BYTE,	// Renderer::VertexArrayFormat::UNSIGNED_BYTE_4
			GL_SHORT,			// Renderer::VertexArrayFormat::SHORT_2
			GL_SHORT			// Renderer::VertexArrayFormat::SHORT_4
		};
		return MAPPING[vertexArrayFormat];
	}

	uint32_t Mapping::getOpenGLType(Renderer::IndexBufferFormat::Enum indexBufferFormat)
	{
		static const GLenum MAPPING[] =
		{
			GL_UNSIGNED_BYTE,	// Renderer::IndexBufferFormat::UNSIGNED_CHAR  - One byte per element, uint8_t (may not be supported by each API)
			GL_UNSIGNED_SHORT,	// Renderer::IndexBufferFormat::UNSIGNED_SHORT - Two bytes per element, uint16_t
			GL_UNSIGNED_INT		// Renderer::IndexBufferFormat::UNSIGNED_INT   - Four bytes per element, uint32_t (may not be supported by each API)
		};
		return MAPPING[indexBufferFormat];
	}

	uint32_t Mapping::getOpenGLInternalFormat(Renderer::TextureFormat::Enum indexBufferFormat)
	{
		static const GLuint MAPPING[] =
		{
			GL_R8,										// Renderer::TextureFormat::A8            - 8-bit pixel format, all bits "red"
			GL_RGB8,									// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			GL_RGBA8,									// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			GL_RGBA16F_ARB,								// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			GL_RGBA32F_ARB,								// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha),
			GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,			// Renderer::TextureFormat::BC1           - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block)
			GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,			// Renderer::TextureFormat::BC2           - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,			// Renderer::TextureFormat::BC3           - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			GL_COMPRESSED_LUMINANCE_LATC1_EXT,			// Renderer::TextureFormat::BC4           - 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block)
			GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT,	// Renderer::TextureFormat::BC5           - 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block)
			0											// Renderer::TextureFormat::ETC1          - 3 component texture compression meant for mobile devices - not supported in OpenGL
		};
		return MAPPING[indexBufferFormat];
	}

	uint32_t Mapping::getOpenGLFormat(Renderer::TextureFormat::Enum textureFormat)
	{
		static const GLuint MAPPING[] =
		{
			GL_RED,		// Renderer::TextureFormat::A8            - 8-bit pixel format, all bits "red"
			GL_RGB,		// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			GL_RGBA,	// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			GL_RGBA,	// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			GL_RGBA		// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha)
		};
		return MAPPING[textureFormat];
	}

	uint32_t Mapping::getOpenGLType(Renderer::TextureFormat::Enum textureFormat)
	{
		static const GLenum MAPPING[] =
		{
			GL_UNSIGNED_BYTE,	// Renderer::TextureFormat::A8            - 8-bit pixel format, all bits alpha
			GL_UNSIGNED_BYTE,	// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			GL_UNSIGNED_BYTE,	// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			GL_HALF_FLOAT_ARB,	// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			GL_FLOAT			// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha)
		};
		return MAPPING[textureFormat];
	}

	uint32_t Mapping::getOpenGLType(Renderer::PrimitiveTopology::Enum primitive)
	{
		static const GLenum MAPPING[] =
		{
			GL_POINTS,			// Renderer::PrimitiveTopology::POINT_LIST
			GL_LINES,			// Renderer::PrimitiveTopology::LINE_LIST
			GL_LINE_STRIP,		// Renderer::PrimitiveTopology::LINE_STRIP
			GL_TRIANGLES,		// Renderer::PrimitiveTopology::TRIANGLE_LIST
			GL_TRIANGLE_STRIP	// Renderer::PrimitiveTopology::TRIANGLE_STRIP
		};
		return MAPPING[primitive - 1];	// Lookout! The "Renderer::PrimitiveTopology::Enum"-values start with 1, not 0
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
