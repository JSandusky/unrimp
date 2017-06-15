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
#include "OpenGLES3Renderer/Mapping.h"

#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	int Mapping::getOpenGLES3MagFilterMode(Renderer::FilterMode filterMode)
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
				return GL_LINEAR;	// There's no special setting in OpenGL ES 3

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
				return GL_LINEAR;	// There's no special setting in OpenGL ES 3

			case Renderer::FilterMode::UNKNOWN:
				assert(false && "Filter mode must not be unknown");
				return GL_NEAREST;

			default:
				return GL_NEAREST;	// We should never be in here
		}
	}

	int Mapping::getOpenGLES3MinFilterMode(Renderer::FilterMode filterMode, bool hasMipmaps)
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
				return hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;	// There's no special setting in OpenGL ES 3

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
				return hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;	// There's no special setting in OpenGL ES 3

			case Renderer::FilterMode::UNKNOWN:
				assert(false && "Filter mode must not be unknown");
				return GL_NEAREST;

			default:
				return GL_NEAREST;	// We should never be in here
		}
	}

	int Mapping::getOpenGLES3CompareMode(Renderer::FilterMode)
	{
		// "GL_COMPARE_REF_TO_TEXTURE" is not supported by OpenGL ES 3
		return GL_NONE;
	}

	int Mapping::getOpenGLES3TextureAddressMode(Renderer::TextureAddressMode textureAddressMode)
	{
		static const GLint MAPPING[] =
		{
			GL_REPEAT,			// Renderer::TextureAddressMode::WRAP
			GL_MIRRORED_REPEAT,	// Renderer::TextureAddressMode::MIRROR
			GL_CLAMP_TO_EDGE,	// Renderer::TextureAddressMode::CLAMP
			GL_CLAMP_TO_EDGE,	// Renderer::TextureAddressMode::BORDER - Not supported by OpenGL ES 3
			GL_MIRRORED_REPEAT	// Renderer::TextureAddressMode::MIRROR_ONCE	// TODO(co) OpenGL ES 3 equivalent?
		};
		return MAPPING[static_cast<int>(textureAddressMode) - 1];	// Lookout! The "Renderer::TextureAddressMode"-values start with 1, not 0
	}

	int Mapping::getOpenGLES3ComparisonFunc(Renderer::ComparisonFunc comparisonFunc)
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
		return MAPPING[static_cast<int>(comparisonFunc) - 1];	// Lookout! The "Renderer::ComparisonFunc"-values start with 1, not 0
	}

	int Mapping::getOpenGLES3Size(Renderer::VertexAttributeFormat vertexAttributeFormat)
	{
		static const GLint MAPPING[] =
		{
			1,	// Renderer::VertexAttributeFormat::FLOAT_1
			2,	// Renderer::VertexAttributeFormat::FLOAT_2
			3,	// Renderer::VertexAttributeFormat::FLOAT_3
			4,	// Renderer::VertexAttributeFormat::FLOAT_4
			4,	// Renderer::VertexAttributeFormat::R8G8B8A8_UNORM
			4,	// Renderer::VertexAttributeFormat::R8G8B8A8_UINT
			2,	// Renderer::VertexAttributeFormat::SHORT_2
			4,	// Renderer::VertexAttributeFormat::SHORT_4
			1	// Renderer::VertexAttributeFormat::UINT_1
		};
		return MAPPING[static_cast<int>(vertexAttributeFormat)];
	}

	uint32_t Mapping::getOpenGLES3Type(Renderer::VertexAttributeFormat vertexAttributeFormat)
	{
		static const GLenum MAPPING[] =
		{
			GL_FLOAT,			// Renderer::VertexAttributeFormat::FLOAT_1
			GL_FLOAT,			// Renderer::VertexAttributeFormat::FLOAT_2
			GL_FLOAT,			// Renderer::VertexAttributeFormat::FLOAT_3
			GL_FLOAT,			// Renderer::VertexAttributeFormat::FLOAT_4
			GL_UNSIGNED_BYTE,	// Renderer::VertexAttributeFormat::R8G8B8A8_UNORM
			GL_UNSIGNED_BYTE,	// Renderer::VertexAttributeFormat::R8G8B8A8_UINT
			GL_SHORT,			// Renderer::VertexAttributeFormat::SHORT_2
			GL_SHORT,			// Renderer::VertexAttributeFormat::SHORT_4
			GL_UNSIGNED_INT		// Renderer::VertexAttributeFormat::UINT_1
		};
		return MAPPING[static_cast<int>(vertexAttributeFormat)];
	}

	uint32_t Mapping::isOpenGLES3VertexAttributeFormatNormalized(Renderer::VertexAttributeFormat vertexAttributeFormat)
	{
		static const GLenum MAPPING[] =
		{
			0,	// Renderer::VertexAttributeFormat::FLOAT_1
			0,	// Renderer::VertexAttributeFormat::FLOAT_2
			0,	// Renderer::VertexAttributeFormat::FLOAT_3
			0,	// Renderer::VertexAttributeFormat::FLOAT_4
			1,	// Renderer::VertexAttributeFormat::R8G8B8A8_UNORM
			0,	// Renderer::VertexAttributeFormat::R8G8B8A8_UINT
			0,	// Renderer::VertexAttributeFormat::SHORT_2
			0,	// Renderer::VertexAttributeFormat::SHORT_4
			0	// Renderer::VertexAttributeFormat::UINT_1
		};
		return MAPPING[static_cast<int>(vertexAttributeFormat)];
	}

	uint32_t Mapping::isOpenGLES3VertexAttributeFormatInteger(Renderer::VertexAttributeFormat vertexAttributeFormat)
	{
		static const GLenum MAPPING[] =
		{
			0,	// Renderer::VertexAttributeFormat::FLOAT_1
			0,	// Renderer::VertexAttributeFormat::FLOAT_2
			0,	// Renderer::VertexAttributeFormat::FLOAT_3
			0,	// Renderer::VertexAttributeFormat::FLOAT_4
			0,	// Renderer::VertexAttributeFormat::R8G8B8A8_UNORM
			1,	// Renderer::VertexAttributeFormat::R8G8B8A8_UINT
			1,	// Renderer::VertexAttributeFormat::SHORT_2
			1,	// Renderer::VertexAttributeFormat::SHORT_4
			1	// Renderer::VertexAttributeFormat::UINT_1
		};
		return MAPPING[static_cast<int>(vertexAttributeFormat)];
	}

	uint32_t Mapping::getOpenGLES3Type(Renderer::BufferUsage bufferUsage)
	{
		// OpenGL ES 3 only supports: "STREAM_DRAW", "STATIC_DRAW" and "DYNAMIC_DRAW"

		// These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 3 constants, do not change them
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

	uint32_t Mapping::getOpenGLES3Type(Renderer::IndexBufferFormat::Enum indexBufferFormat)
	{
		static const GLenum MAPPING[] =
		{
			GL_UNSIGNED_BYTE,	// Renderer::IndexBufferFormat::UNSIGNED_CHAR  - One byte per element, uint8_t (may not be supported by each API)
			GL_UNSIGNED_SHORT,	// Renderer::IndexBufferFormat::UNSIGNED_SHORT - Two bytes per element, uint16_t
			GL_UNSIGNED_INT		// Renderer::IndexBufferFormat::UNSIGNED_INT   - Four bytes per element, uint32_t (may not be supported by each API)
		};
		return MAPPING[indexBufferFormat];
	}

	uint32_t Mapping::getOpenGLES3InternalFormat(Renderer::TextureFormat::Enum textureFormat)
	{
		static const uint32_t MAPPING[] =
		{
			GL_R8,								// Renderer::TextureFormat::R8            - 8-bit pixel format, all bits red
			GL_RGB,								// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			GL_RGBA,							// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			GL_RGBA,							// Renderer::TextureFormat::R8G8B8A8_SRGB - 32-bit pixel format, 8 bits for red, green, blue and alpha; sRGB = RGB hardware gamma correction, the alpha channel always remains linear	- TODO(co) OpenGL ES 3 sRGB format
			GL_R11F_G11F_B10F,					// Renderer::TextureFormat::R11G11B10F    - 32-bit float format using 11 bits the red and green channel, 10 bits the blue channel; red and green channels have a 6 bits mantissa and a 5 bits exponent and blue has a 5 bits mantissa and 5 bits exponent - available in OpenGL ES 3
			GL_RGBA16F,							// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			GL_RGBA32F,							// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha)
			GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,	// Renderer::TextureFormat::BC1           - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block) - "GL_EXT_texture_compression_dxt1" OpenGL ES extension
			GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,	// Renderer::TextureFormat::BC1_SRGB      - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block) - "GL_EXT_texture_compression_dxt1" OpenGL ES extension; sRGB = RGB hardware gamma correction, the alpha channel always remains linear	- TODO(co) OpenGL ES 3 sRGB format
			GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,	// Renderer::TextureFormat::BC2           - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - "GL_EXT_texture_compression_s3tc" OpenGL ES extension
			GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,	// Renderer::TextureFormat::BC2_SRGB      - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - "GL_EXT_texture_compression_s3tc" OpenGL ES extension; sRGB = RGB hardware gamma correction, the alpha channel always remains linear	- TODO(co) OpenGL ES 3 sRGB format
			GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,	// Renderer::TextureFormat::BC3           - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - "GL_EXT_texture_compression_s3tc" OpenGL ES extension
			GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,	// Renderer::TextureFormat::BC3_SRGB      - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - "GL_EXT_texture_compression_s3tc" OpenGL ES extension; sRGB = RGB hardware gamma correction, the alpha channel always remains linear	- TODO(co) OpenGL ES 3 sRGB format
			GL_3DC_X_AMD,						// Renderer::TextureFormat::BC4           - 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block) - "GL_AMD_compressed_3DC_texture" OpenGL ES extension
			GL_3DC_XY_AMD,						// Renderer::TextureFormat::BC5           - 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block) - "GL_AMD_compressed_3DC_texture" OpenGL ES extension
			GL_ETC1_RGB8_OES,					// Renderer::TextureFormat::ETC1          - 3 component texture compression meant for mobile devices
			GL_R32UI,							// Renderer::TextureFormat::R32_UINT      - 32-bit unsigned integer format
			GL_R32F,							// Renderer::TextureFormat::R32_FLOAT     - 32-bit float format
			GL_DEPTH_COMPONENT32F,				// Renderer::TextureFormat::D32_FLOAT     - 32-bit float depth format
			0,									// Renderer::TextureFormat::R16G16_SNORM  - A two-component, 32-bit signed-normalized-integer format that supports 16 bits for the red channel and 16 bits for the green channel	TODO(co) "GL_RG16_SNORM" OpenGL ES 3 needs https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_texture_norm16.txt
			GL_RG16F,							// Renderer::TextureFormat::R16G16_FLOAT  - A two-component, 32-bit floating-point format that supports 16 bits for the red channel and 16 bits for the green channel
			0									// Renderer::TextureFormat::UNKNOWN       - Unknown
		};
		return MAPPING[textureFormat];
	}

	uint32_t Mapping::getOpenGLES3Format(Renderer::TextureFormat::Enum textureFormat)
	{
		static const uint32_t MAPPING[] =
		{
			GL_RED,				// Renderer::TextureFormat::R8            - 8-bit pixel format, all bits red
			GL_RGB,				// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			GL_RGBA,			// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			GL_RGBA,			// Renderer::TextureFormat::R8G8B8A8_SRGB - 32-bit pixel format, 8 bits for red, green, blue and alpha; sRGB = RGB hardware gamma correction, the alpha channel always remains linear
			GL_RGB,				// Renderer::TextureFormat::R11G11B10F    - 32-bit float format using 11 bits the red and green channel, 10 bits the blue channel; red and green channels have a 6 bits mantissa and a 5 bits exponent and blue has a 5 bits mantissa and 5 bits exponent - available in OpenGL ES 3
			GL_RGBA,			// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha) - Not supported by OpenGL ES 3
			GL_RGBA,			// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha) - Not supported by OpenGL ES 3
			0,					// Renderer::TextureFormat::BC1           - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block) - Compressed format, so not supported in here
			0,					// Renderer::TextureFormat::BC1_SRGB      - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear - Compressed format, so not supported in here
			0,					// Renderer::TextureFormat::BC2           - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - Compressed format, so not supported in here
			0,					// Renderer::TextureFormat::BC2_SRGB      - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear - Compressed format, so not supported in here
			0,					// Renderer::TextureFormat::BC3           - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - Compressed format, so not supported in here
			0,					// Renderer::TextureFormat::BC3_SRGB      - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear - Compressed format, so not supported in here
			0,					// Renderer::TextureFormat::BC4           - 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block) - Compressed format, so not supported in here
			0,					// Renderer::TextureFormat::BC5           - 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block) - Compressed format, so not supported in here
			0,					// Renderer::TextureFormat::ETC1          - 3 component texture compression meant for mobile devices - Compressed format, so not supported in here
			GL_RED_INTEGER,		// Renderer::TextureFormat::R32_UINT      - 32-bit unsigned integer format
			GL_RED,				// Renderer::TextureFormat::R32_FLOAT     - 32-bit float format
			GL_DEPTH_COMPONENT,	// Renderer::TextureFormat::D32_FLOAT     - 32-bit float depth format
			GL_RG,				// Renderer::TextureFormat::R16G16_SNORM  - A two-component, 32-bit signed-normalized-integer format that supports 16 bits for the red channel and 16 bits for the green channel
			GL_RG,				// Renderer::TextureFormat::R16G16_FLOAT  - A two-component, 32-bit floating-point format that supports 16 bits for the red channel and 16 bits for the green channel
			0					// Renderer::TextureFormat::UNKNOWN       - Unknown
		};
		return MAPPING[textureFormat];
	}

	uint32_t Mapping::getOpenGLES3Type(Renderer::TextureFormat::Enum textureFormat)
	{
		static const GLenum MAPPING[] =
		{
			GL_UNSIGNED_BYTE,					// Renderer::TextureFormat::R8            - 8-bit pixel format, all bits red
			GL_UNSIGNED_BYTE,					// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			GL_UNSIGNED_BYTE,					// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			GL_UNSIGNED_BYTE,					// Renderer::TextureFormat::R8G8B8A8_SRGB - 32-bit pixel format, 8 bits for red, green, blue and alpha; sRGB = RGB hardware gamma correction, the alpha channel always remains linear
			GL_UNSIGNED_INT_10F_11F_11F_REV,	// Renderer::TextureFormat::R11G11B10F    - 32-bit float format using 11 bits the red and green channel, 10 bits the blue channel; red and green channels have a 6 bits mantissa and a 5 bits exponent and blue has a 5 bits mantissa and 5 bits exponent - available in OpenGL ES 3
			GL_FLOAT,							// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha) - Not supported by OpenGL ES 3
			GL_FLOAT,							// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha) - Not supported by OpenGL ES 3
			0,									// Renderer::TextureFormat::BC1           - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block) - Compressed format, so not supported in here
			0,									// Renderer::TextureFormat::BC1_SRGB      - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear - Compressed format, so not supported in here
			0,									// Renderer::TextureFormat::BC2           - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - Compressed format, so not supported in here
			0,									// Renderer::TextureFormat::BC2_SRGB      - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block); sRGB = hardware gamma correction - Compressed format, so not supported in here
			0,									// Renderer::TextureFormat::BC3           - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block) - Compressed format, so not supported in here
			0,									// Renderer::TextureFormat::BC3_SRGB      - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear - Compressed format, so not supported in here
			0,									// Renderer::TextureFormat::BC4           - 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block) - Compressed format, so not supported in here
			0,									// Renderer::TextureFormat::BC5           - 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block) - Compressed format, so not supported in here
			0,									// Renderer::TextureFormat::ETC1          - 3 component texture compression meant for mobile devices - Compressed format, so not supported in here
			GL_UNSIGNED_INT,					// Renderer::TextureFormat::R32_UINT      - 32-bit unsigned integer format
			GL_FLOAT,							// Renderer::TextureFormat::R32_FLOAT     - 32-bit float format
			GL_FLOAT,							// Renderer::TextureFormat::D32_FLOAT     - 32-bit float depth format
			GL_BYTE,							// Renderer::TextureFormat::R16G16_SNORM  - A two-component, 32-bit signed-normalized-integer format that supports 16 bits for the red channel and 16 bits for the green channel
			GL_FLOAT,							// Renderer::TextureFormat::R16G16_FLOAT  - A two-component, 32-bit floating-point format that supports 16 bits for the red channel and 16 bits for the green channel
			0									// Renderer::TextureFormat::UNKNOWN       - Unknown
		};
		return MAPPING[textureFormat];
	}

	uint32_t Mapping::getOpenGLES3Type(Renderer::PrimitiveTopology prmitive)
	{
		static const GLenum MAPPING[] =
		{
			GL_POINTS,			// Renderer::PrimitiveTopology::POINT_LIST
			GL_LINES,			// Renderer::PrimitiveTopology::LINE_LIST
			GL_LINE_STRIP,		// Renderer::PrimitiveTopology::LINE_STRIP
			GL_TRIANGLES,		// Renderer::PrimitiveTopology::TRIANGLE_LIST
			GL_TRIANGLE_STRIP	// Renderer::PrimitiveTopology::TRIANGLE_STRIP
		};
		return MAPPING[static_cast<int>(prmitive) - 1];	// Lookout! The "Renderer::PrimitiveTopology"-values start with 1, not 0
	}

	uint32_t Mapping::getOpenGLES3MapRangeType(Renderer::MapType mapType)
	{
		// OpenGL ES 3 defines access bits for "glMapBufferRange()"
		static const GLbitfield MAPPING[] =
		{
			GL_MAP_READ_BIT,					// Renderer::MapType::READ
			GL_MAP_WRITE_BIT,					// Renderer::MapType::WRITE
			GL_MAP_READ_BIT | GL_MAP_WRITE_BIT,	// Renderer::MapType::READ_WRITE
			GL_MAP_WRITE_BIT,					// Renderer::MapType::WRITE_DISCARD
			GL_MAP_WRITE_BIT					// Renderer::MapType::WRITE_NO_OVERWRITE
		};
		return MAPPING[static_cast<int>(mapType) - 1];	// Lookout! The "Renderer::MapType"-values start with 1, not 0
	}

	uint32_t Mapping::getOpenGLES3BlendType(Renderer::Blend blend)
	{
		if (blend <= Renderer::Blend::SRC_ALPHA_SAT)
		{
			static const GLenum MAPPING[] =
			{
				GL_ZERO,				// Renderer::Blend::ZERO
				GL_ONE,					// Renderer::Blend::ONE
				GL_SRC_COLOR,			// Renderer::Blend::SRC_COLOR
				GL_ONE_MINUS_SRC_COLOR,	// Renderer::Blend::INV_SRC_COLOR
				GL_SRC_ALPHA,			// Renderer::Blend::SRC_ALPHA
				GL_ONE_MINUS_SRC_ALPHA,	// Renderer::Blend::INV_SRC_ALPHA
				GL_DST_ALPHA,			// Renderer::Blend::DEST_ALPHA
				GL_ONE_MINUS_DST_ALPHA,	// Renderer::Blend::INV_DEST_ALPHA
				GL_DST_COLOR,			// Renderer::Blend::DEST_COLOR
				GL_ONE_MINUS_DST_COLOR,	// Renderer::Blend::INV_DEST_COLOR
				GL_SRC_ALPHA_SATURATE	// Renderer::Blend::SRC_ALPHA_SAT
			};
			return MAPPING[static_cast<int>(blend) - static_cast<int>(Renderer::Blend::ZERO)];
		}
		else
		{
			static const GLenum MAPPING[] =
			{
				GL_SRC_COLOR,			// Renderer::Blend::BLEND_FACTOR		TODO(co) Mapping "Renderer::Blend::BLEND_FACTOR" to OpenGL ES 3 possible?
				GL_ONE_MINUS_SRC_COLOR,	// Renderer::Blend::INV_BLEND_FACTOR	TODO(co) Mapping "Renderer::Blend::INV_BLEND_FACTOR" to OpenGL ES 3 possible?
				GL_SRC_COLOR,			// Renderer::Blend::SRC_1_COLOR			TODO(co) Mapping "Renderer::Blend::SRC_1_COLOR" to OpenGL ES 3 possible?
				GL_ONE_MINUS_SRC_COLOR,	// Renderer::Blend::INV_SRC_1_COLOR		TODO(co) Mapping "Renderer::Blend::INV_SRC_1_COLOR" to OpenGL ES 3 possible?
				GL_SRC_COLOR,			// Renderer::Blend::SRC_1_ALPHA			TODO(co) Mapping "Renderer::Blend::SRC_1_ALPHA" to OpenGL ES 3 possible?
				GL_ONE_MINUS_SRC_COLOR,	// Renderer::Blend::INV_SRC_1_ALPHA		TODO(co) Mapping "Renderer::Blend::INV_SRC_1_ALPHA" to OpenGL ES 3 possible?
			};
			return MAPPING[static_cast<int>(blend) - static_cast<int>(Renderer::Blend::BLEND_FACTOR)];
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
