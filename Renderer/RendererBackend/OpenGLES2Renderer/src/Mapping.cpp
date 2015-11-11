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
#include "OpenGLES2Renderer/Mapping.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	int Mapping::getOpenGLES2MagFilterMode(Renderer::FilterMode::Enum filterMode)
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
				return GL_LINEAR;	// There's no special setting in OpenGL ES 2

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
				return GL_LINEAR;	// There's no special setting in OpenGL ES 2

			default:
				return GL_NEAREST;	// We should never be in here
		}
	}

	int Mapping::getOpenGLES2MinFilterMode(Renderer::FilterMode::Enum filterMode, bool hasMipmaps)
	{
		switch (filterMode)
		{
			case Renderer::FilterMode::MIN_MAG_MIP_POINT:
				return hasMipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;

			case Renderer::FilterMode::MIN_MAG_POINT_MIP_LINEAR:
				return hasMipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST;

			case Renderer::FilterMode::MIN_POINT_MAG_LINEAR_MIP_POINT:
				return hasMipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;

			case Renderer::FilterMode::MIN_POINT_MAG_MIP_LINEAR:
				return hasMipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST;

			case Renderer::FilterMode::MIN_LINEAR_MAG_MIP_POINT:
				return hasMipmaps ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;

			case Renderer::FilterMode::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

			case Renderer::FilterMode::MIN_MAG_LINEAR_MIP_POINT:
				return hasMipmaps ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;

			case Renderer::FilterMode::MIN_MAG_MIP_LINEAR:
				return hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

			case Renderer::FilterMode::ANISOTROPIC:
				return hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;	// There's no special setting in OpenGL ES 2

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_POINT:
				return hasMipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
				return hasMipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
				return hasMipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
				return hasMipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
				return hasMipmaps ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
				return hasMipmaps ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_LINEAR:
				return hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

			case Renderer::FilterMode::COMPARISON_ANISOTROPIC:
				return hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;	// There's no special setting in OpenGL ES 2

			default:
				return GL_NEAREST;	// We should never be in here
		}
	}

	int Mapping::getOpenGLES2CompareMode(Renderer::FilterMode::Enum)
	{
		// "GL_COMPARE_REF_TO_TEXTURE" is not supported by OpenGL ES 2
		return GL_NONE;
	}

	int Mapping::getOpenGLES2TextureAddressMode(Renderer::TextureAddressMode::Enum textureAddressMode)
	{
		static const GLint MAPPING[] =
		{
			GL_REPEAT,			// Renderer::TextureAddressMode::WRAP
			GL_MIRRORED_REPEAT,	// Renderer::TextureAddressMode::MIRROR
			GL_CLAMP_TO_EDGE,	// Renderer::TextureAddressMode::CLAMP
			GL_CLAMP_TO_EDGE,	// Renderer::TextureAddressMode::BORDER - Not supported by OpenGL ES 2
			GL_MIRRORED_REPEAT	// Renderer::TextureAddressMode::MIRROR_ONCE	// TODO(co) OpenGL ES 2 equivalent?
		};
		return MAPPING[textureAddressMode - 1];	// Lookout! The "Renderer::TextureAddressMode::Enum"-values start with 1, not 0
	}

	int Mapping::getOpenGLES2ComparisonFunc(Renderer::ComparisonFunc::Enum comparisonFunc)
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

	int Mapping::getOpenGLES2Size(Renderer::VertexAttributeFormat::Enum vertexAttributeFormat)
	{
		static const GLint MAPPING[] =
		{
			1,	// Renderer::VertexAttributeFormat::FLOAT_1
			2,	// Renderer::VertexAttributeFormat::FLOAT_2
			3,	// Renderer::VertexAttributeFormat::FLOAT_3
			4,	// Renderer::VertexAttributeFormat::FLOAT_4
			4,	// Renderer::VertexAttributeFormat::UNSIGNED_BYTE_4
			2,	// Renderer::VertexAttributeFormat::SHORT_2
			4	// Renderer::VertexAttributeFormat::SHORT_4
		};
		return MAPPING[vertexAttributeFormat];
	}

	uint32_t Mapping::getOpenGLES2Type(Renderer::VertexAttributeFormat::Enum vertexAttributeFormat)
	{
		static const GLenum MAPPING[] =
		{
			GL_FLOAT,			// Renderer::VertexAttributeFormat::FLOAT_1
			GL_FLOAT,			// Renderer::VertexAttributeFormat::FLOAT_2
			GL_FLOAT,			// Renderer::VertexAttributeFormat::FLOAT_3
			GL_FLOAT,			// Renderer::VertexAttributeFormat::FLOAT_4
			GL_UNSIGNED_BYTE,	// Renderer::VertexAttributeFormat::UNSIGNED_BYTE_4
			GL_SHORT,			// Renderer::VertexAttributeFormat::SHORT_2
			GL_SHORT			// Renderer::VertexAttributeFormat::SHORT_4
		};
		return MAPPING[vertexAttributeFormat];
	}

	uint32_t Mapping::getOpenGLES2Type(Renderer::BufferUsage::Enum bufferUsage)
	{
		// OpenGL ES 2 only supports: "STREAM_DRAW", "STATIC_DRAW" and "DYNAMIC_DRAW"

		// These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 2 constants, do not change them
		// -> This also means that we have to use a switch statement for the mapping
		switch (bufferUsage)
		{
			case Renderer::BufferUsage::STREAM_DRAW:
			case Renderer::BufferUsage::STREAM_READ:
			case Renderer::BufferUsage::STREAM_COPY:
				return GL_STREAM_DRAW;

			case Renderer::BufferUsage::STATIC_DRAW:
			case Renderer::BufferUsage::STATIC_READ:
			case Renderer::BufferUsage::STATIC_COPY:
				return GL_STATIC_DRAW;

			case Renderer::BufferUsage::DYNAMIC_DRAW:
			case Renderer::BufferUsage::DYNAMIC_READ:
			case Renderer::BufferUsage::DYNAMIC_COPY:
			default:
				return GL_DYNAMIC_DRAW;
		}
	}

	uint32_t Mapping::getOpenGLES2Type(Renderer::IndexBufferFormat::Enum indexBufferFormat)
	{
		static const GLenum MAPPING[] =
		{
			GL_UNSIGNED_BYTE,	// Renderer::IndexBufferFormat::UNSIGNED_CHAR  - One byte per element, uint8_t (may not be supported by each API)
			GL_UNSIGNED_SHORT,	// Renderer::IndexBufferFormat::UNSIGNED_SHORT - Two bytes per element, uint16_t
			GL_UNSIGNED_INT		// Renderer::IndexBufferFormat::UNSIGNED_INT   - Four bytes per element, uint32_t (may not be supported by each API)
		};
		return MAPPING[indexBufferFormat];
	}

	uint32_t Mapping::getOpenGLES2InternalFormat(Renderer::TextureFormat::Enum textureFormat)
	{
		static const uint32_t MAPPING[] =
		{
			GL_ALPHA,							// Renderer::TextureFormat::A8            - 8-bit pixel format, all bits alpha
			GL_RGB,								// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			GL_RGBA,							// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			GL_RGBA,							// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha) - Not supported by OpenGL ES 2
			GL_RGBA,							// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha) - Not supported by OpenGL ES 2
			GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,	// Renderer::TextureFormat::BC1           - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block) - "GL_EXT_texture_compression_dxt1" OpenGL ES extension
			GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,	// Renderer::TextureFormat::BC2           - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - "GL_EXT_texture_compression_s3tc" OpenGL ES extension
			GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,	// Renderer::TextureFormat::BC3           - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - "GL_EXT_texture_compression_s3tc" OpenGL ES extension
			GL_3DC_X_AMD,						// Renderer::TextureFormat::BC4           - 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block) - "GL_AMD_compressed_3DC_texture" OpenGL ES extension
			GL_3DC_XY_AMD,						// Renderer::TextureFormat::BC5           - 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block) - "GL_AMD_compressed_3DC_texture" OpenGL ES extension
			GL_ETC1_RGB8_OES,					// Renderer::TextureFormat::ETC1          - 3 component texture compression meant for mobile devices
			GL_FLOAT							// Renderer::TextureFormat::D32_FLOAT     - 32-bit float depth format	TODO(co) Check this
		};
		return MAPPING[textureFormat];
	}

	uint32_t Mapping::getOpenGLES2Format(Renderer::TextureFormat::Enum textureFormat)
	{
		static const uint32_t MAPPING[] =
		{
			GL_ALPHA,	// Renderer::TextureFormat::A8            - 8-bit pixel format, all bits alpha
			GL_RGB,		// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			GL_RGBA,	// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			GL_RGBA,	// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha) - Not supported by OpenGL ES 2
			GL_RGBA,	// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha) - Not supported by OpenGL ES 2
			0,			// Renderer::TextureFormat::BC1           - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block) - Compressed format, so not supported in here
			0,			// Renderer::TextureFormat::BC2           - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - Compressed format, so not supported in here
			0,			// Renderer::TextureFormat::BC3           - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - Compressed format, so not supported in here
			0,			// Renderer::TextureFormat::BC4           - 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block) - Compressed format, so not supported in here
			0,			// Renderer::TextureFormat::BC5           - 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block) - Compressed format, so not supported in here
			0,			// Renderer::TextureFormat::ETC1          - 3 component texture compression meant for mobile devices - Compressed format, so not supported in here
			0			// Renderer::TextureFormat::D32_FLOAT     - 32-bit float depth format	TODO(co) Check this
		};
		return MAPPING[textureFormat];
	}

	uint32_t Mapping::getOpenGLES2Type(Renderer::TextureFormat::Enum textureFormat)
	{
		static const GLenum MAPPING[] =
		{
			GL_UNSIGNED_BYTE,	// Renderer::TextureFormat::A8            - 8-bit pixel format, all bits alpha
			GL_UNSIGNED_BYTE,	// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			GL_UNSIGNED_BYTE,	// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			GL_FLOAT,			// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha) - Not supported by OpenGL ES 2
			GL_FLOAT,			// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha) - Not supported by OpenGL ES 2
			0,					// Renderer::TextureFormat::BC1           - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block) - Compressed format, so not supported in here
			0,					// Renderer::TextureFormat::BC2           - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - Compressed format, so not supported in here
			0,					// Renderer::TextureFormat::BC3           - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - Compressed format, so not supported in here
			0,					// Renderer::TextureFormat::BC4           - 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block) - Compressed format, so not supported in here
			0,					// Renderer::TextureFormat::BC5           - 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block) - Compressed format, so not supported in here
			0,					// Renderer::TextureFormat::ETC1          - 3 component texture compression meant for mobile devices - Compressed format, so not supported in here
			0					// Renderer::TextureFormat::D32_FLOAT     - 32-bit float depth format	TODO(co) Check this
		};
		return MAPPING[textureFormat];
	}

	uint32_t Mapping::getOpenGLES2Type(Renderer::PrimitiveTopology::Enum prmitive)
	{
		static const GLenum MAPPING[] =
		{
			GL_POINTS,			// Renderer::PrimitiveTopology::POINT_LIST
			GL_LINES,			// Renderer::PrimitiveTopology::LINE_LIST
			GL_LINE_STRIP,		// Renderer::PrimitiveTopology::LINE_STRIP
			GL_TRIANGLES,		// Renderer::PrimitiveTopology::TRIANGLE_LIST
			GL_TRIANGLE_STRIP	// Renderer::PrimitiveTopology::TRIANGLE_STRIP
		};
		return MAPPING[prmitive - 1];	// Lookout! The "Renderer::PrimitiveTopology::Enum"-values start with 1, not 0
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
