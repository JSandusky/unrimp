/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
			4	// Renderer::VertexArrayFormat::FLOAT_4
		};
		return MAPPING[vertexArrayFormat];
	}

	unsigned int Mapping::getOpenGLType(Renderer::VertexArrayFormat::Enum vertexArrayFormat)
	{
		static const GLenum MAPPING[] =
		{
			GL_FLOAT,	// Renderer::VertexArrayFormat::FLOAT_1
			GL_FLOAT,	// Renderer::VertexArrayFormat::FLOAT_2
			GL_FLOAT,	// Renderer::VertexArrayFormat::FLOAT_3
			GL_FLOAT	// Renderer::VertexArrayFormat::FLOAT_4
		};
		return MAPPING[vertexArrayFormat];
	}

	unsigned int Mapping::getOpenGLType(Renderer::IndexBufferFormat::Enum indexBufferFormat)
	{
		static const GLenum MAPPING[] =
		{
			GL_UNSIGNED_BYTE,	// Renderer::IndexBufferFormat::UNSIGNED_CHAR  - One byte per element, unsigned char (may not be supported by each API)
			GL_UNSIGNED_SHORT,	// Renderer::IndexBufferFormat::UNSIGNED_SHORT - Two bytes per element, unsigned short
			GL_UNSIGNED_INT		// Renderer::IndexBufferFormat::UNSIGNED_INT   - Four bytes per element, unsigned int (may not be supported by each API)
		};
		return MAPPING[indexBufferFormat];
	}

	unsigned int Mapping::getOpenGLInternalFormat(Renderer::TextureFormat::Enum indexBufferFormat)
	{
		static const GLuint MAPPING[] =
		{
			GL_ALPHA,		// Renderer::TextureFormat::A8            - 8-bit pixel format, all bits alpha
			GL_RGB8,		// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			GL_RGBA8,		// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			GL_RGBA16F_ARB,	// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			GL_RGBA32F_ARB	// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha)
		};
		return MAPPING[indexBufferFormat];
	}

	unsigned int Mapping::getOpenGLFormat(Renderer::TextureFormat::Enum textureFormat)
	{
		static const GLuint MAPPING[] =
		{
			GL_ALPHA,	// Renderer::TextureFormat::A8            - 8-bit pixel format, all bits alpha
			GL_RGB,		// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			GL_RGBA,	// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			GL_RGBA,	// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			GL_RGBA		// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha)
		};
		return MAPPING[textureFormat];
	}

	unsigned int Mapping::getOpenGLType(Renderer::TextureFormat::Enum textureFormat)
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

	unsigned int Mapping::getOpenGLType(Renderer::PrimitiveTopology::Enum primitive)
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
