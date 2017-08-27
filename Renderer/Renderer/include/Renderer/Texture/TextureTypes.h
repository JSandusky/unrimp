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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <inttypes.h>	// For uint32_t, uint64_t etc.


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Definitions                                           ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Texture format
	*/
	struct TextureFormat
	{
		enum Enum
		{
			R8				  = 0,	///< 8-bit pixel format, all bits red
			R8G8B8			  = 1,	///< 24-bit pixel format, 8 bits for red, green and blue
			R8G8B8A8		  = 2,	///< 32-bit pixel format, 8 bits for red, green, blue and alpha
			R8G8B8A8_SRGB	  = 3,	///< 32-bit pixel format, 8 bits for red, green, blue and alpha; sRGB = RGB hardware gamma correction, the alpha channel always remains linear
			B8G8R8A8		  = 4,	///< 32-bit pixel format, 8 bits for red, green, blue and alpha
			R11G11B10F		  = 5,	///< 32-bit float format using 11 bits the red and green channel, 10 bits the blue channel; red and green channels have a 6 bits mantissa and a 5 bits exponent and blue has a 5 bits mantissa and 5 bits exponent
			R16G16B16A16F	  = 6,	///< 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			R32G32B32A32F	  = 7,	///< 128-bit float format using 32 bits for the each channel (red, green, blue, alpha)
			BC1				  = 8,	///< DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block)
			BC1_SRGB		  = 9,	///< DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear
			BC2				  = 10,	///< DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			BC2_SRGB		  = 11,	///< DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear
			BC3				  = 12,	///< DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			BC3_SRGB		  = 13,	///< DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear
			BC4				  = 14,	///< 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block)
			BC5				  = 15,	///< 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block)
			ETC1			  = 16,	///< 3 component texture compression meant for mobile devices
			R32_UINT		  = 17,	///< 32-bit unsigned integer format
			R32_FLOAT		  = 18,	///< 32-bit float format
			D32_FLOAT		  = 19,	///< 32-bit float depth format
			R16G16_SNORM	  = 20,	///< A two-component, 32-bit signed-normalized-integer format that supports 16 bits for the red channel and 16 bits for the green channel
			R16G16_FLOAT	  = 21,	///< A two-component, 32-bit floating-point format that supports 16 bits for the red channel and 16 bits for the green channel
			UNKNOWN			  = 22,	///< Unknown
			NUMBER_OF_FORMATS = 23	///< Number of texture formats
		};

		/**
		*  @brief
		*    Return whether or not the given "Renderer::TextureFormat" is a compressed format
		*
		*  @param[in] textureFormat
		*    "Renderer::TextureFormat" to check
		*
		*  @return
		*    "true" if the given "Renderer::TextureFormat" is a compressed format, else "false"
		*/
		inline static bool isCompressed(Enum textureFormat);

		/**
		*  @brief
		*    Return whether or not the given "Renderer::TextureFormat" is a depth format
		*
		*  @param[in] textureFormat
		*    "Renderer::TextureFormat" to check
		*
		*  @return
		*    "true" if the given "Renderer::TextureFormat" is a depth format, else "false"
		*/
		inline static bool isDepth(Enum textureFormat);

		/**
		*  @brief
		*    "Renderer::TextureFormat" to number of bytes per element
		*
		*  @param[in] textureFormat
		*    "Renderer::TextureFormat" to map
		*
		*  @return
		*    Number of bytes per element
		*/
		inline static uint32_t getNumberOfBytesPerElement(Enum textureFormat);

		/**
		*  @brief
		*    "Renderer::TextureFormat" to number of bytes per row
		*
		*  @param[in] textureFormat
		*    "Renderer::TextureFormat" to map
		*  @param[in] width
		*    Row width
		*
		*  @return
		*    Number of bytes per row
		*
		*  @note
		*    - Do not add this within the public "Renderer/Public/Renderer.h"-header, it's for the internal implementation only
		*/
		inline static uint32_t getNumberOfBytesPerRow(Enum textureFormat, uint32_t width);

		/**
		*  @brief
		*    "Renderer::TextureFormat" to number of bytes per slice
		*
		*  @param[in] textureFormat
		*    "Renderer::TextureFormat" to map
		*  @param[in] width
		*    Slice width
		*  @param[in] height
		*    Slice height
		*
		*  @return
		*    Number of bytes per slice
		*
		*  @note
		*    - Do not add this within the public "Renderer/Public/Renderer.h"-header, it's for the internal implementation only
		*/
		inline static uint32_t getNumberOfBytesPerSlice(Enum textureFormat, uint32_t width, uint32_t height);
	};

	/**
	*  @brief
	*    Texture flags
	*/
	struct TextureFlag
	{
		enum Enum
		{
			DATA_CONTAINS_MIPMAPS = 1 << 0,	///< The user provided data containing mipmaps from 0-n down to 1x1 linearly in memory
			GENERATE_MIPMAPS      = 1 << 1,	///< Automatically generate mipmaps (avoid this if you can, will be ignored in case the "DATA_CONTAINS_MIPMAPS"-flag is set)
			RENDER_TARGET         = 1 << 2	///< This texture can be used as render target
		};
	};

	/**
	*  @brief
	*    Texture usage indication
	*
	*  @note
	*    - Only relevant for Direct3D, OpenGL has no texture usage indication
	*    - Original Direct3D comments from http://msdn.microsoft.com/en-us/library/windows/desktop/ff476259%28v=vs.85%29.aspx are used in here
	*    - These constants directly map to Direct3D 10 & 11 constants, do not change them
	*/
	enum class TextureUsage
	{
		DEFAULT   = 0,	///< A resource that requires read and write access by the GPU. This is likely to be the most common usage choice.
		IMMUTABLE = 1,	///< A resource that can only be read by the GPU. It cannot be written by the GPU, and cannot be accessed at all by the CPU. This type of resource must be initialized when it is created, since it cannot be changed after creation.
		DYNAMIC   = 2,	///< A resource that is accessible by both the GPU (read only) and the CPU (write only). A dynamic resource is a good choice for a resource that will be updated by the CPU at least once per frame. To update a dynamic resource, use a map method.
		STAGING   = 3	///< A resource that supports data transfer (copy) from the GPU to the CPU.
	};

	/**
	*  @brief
	*    Optimized clear value
	*
	*  @see
	*    - "ID3D12Device::CreateCommittedResource method" documentation at https://msdn.microsoft.com/de-de/library/windows/desktop/dn899178%28v=vs.85%29.aspx
	*/
	struct OptimizedTextureClearValue
	{
		union
		{
			float color[4];
			struct
			{
				float depth;
				uint8_t stencil;
			} DepthStencil;
		};
	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/Texture/TextureTypes.inl"
