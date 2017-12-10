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
#include "VulkanRenderer/Mapping.h"

#include <Renderer/IAssert.h>
#include <Renderer/Context.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	VkFilter Mapping::getVulkanMagFilterMode(const Renderer::Context& context, Renderer::FilterMode filterMode)
	{
		switch (filterMode)
		{
			case Renderer::FilterMode::MIN_MAG_MIP_POINT:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::MIN_MAG_POINT_MIP_LINEAR:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::MIN_POINT_MAG_LINEAR_MIP_POINT:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::MIN_POINT_MAG_MIP_LINEAR:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::MIN_LINEAR_MAG_MIP_POINT:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::MIN_MAG_LINEAR_MIP_POINT:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::MIN_MAG_MIP_LINEAR:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::ANISOTROPIC:
				return VK_FILTER_LINEAR;	// There's no special setting in Vulkan

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_POINT:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_LINEAR:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::COMPARISON_ANISOTROPIC:
				return VK_FILTER_LINEAR;	// There's no special setting in Vulkan

			case Renderer::FilterMode::UNKNOWN:
				RENDERER_ASSERT(context, false, "Vulkan filter mode must not be unknown")
				return VK_FILTER_NEAREST;

			default:
				return VK_FILTER_NEAREST;	// We should never be in here
		}
	}

	VkFilter Mapping::getVulkanMinFilterMode(const Renderer::Context& context, Renderer::FilterMode filterMode)
	{
		switch (filterMode)
		{
			case Renderer::FilterMode::MIN_MAG_MIP_POINT:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::MIN_MAG_POINT_MIP_LINEAR:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::MIN_POINT_MAG_LINEAR_MIP_POINT:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::MIN_POINT_MAG_MIP_LINEAR:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::MIN_LINEAR_MAG_MIP_POINT:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::MIN_MAG_LINEAR_MIP_POINT:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::MIN_MAG_MIP_LINEAR:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::ANISOTROPIC:
				return VK_FILTER_LINEAR;	// There's no special setting in Vulkan

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_POINT:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
				return VK_FILTER_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_LINEAR:
				return VK_FILTER_LINEAR;

			case Renderer::FilterMode::COMPARISON_ANISOTROPIC:
				return VK_FILTER_LINEAR;	// There's no special setting in Vulkan

			case Renderer::FilterMode::UNKNOWN:
				RENDERER_ASSERT(context, false, "Vulkan filter mode must not be unknown")
				return VK_FILTER_NEAREST;

			default:
				return VK_FILTER_NEAREST;	// We should never be in here
		}
	}

	VkSamplerMipmapMode Mapping::getVulkanMipmapMode(const Renderer::Context& context, Renderer::FilterMode filterMode)
	{
		switch (filterMode)
		{
			case Renderer::FilterMode::MIN_MAG_MIP_POINT:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;

			case Renderer::FilterMode::MIN_MAG_POINT_MIP_LINEAR:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;

			case Renderer::FilterMode::MIN_POINT_MAG_LINEAR_MIP_POINT:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;

			case Renderer::FilterMode::MIN_POINT_MAG_MIP_LINEAR:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;

			case Renderer::FilterMode::MIN_LINEAR_MAG_MIP_POINT:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;

			case Renderer::FilterMode::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;

			case Renderer::FilterMode::MIN_MAG_LINEAR_MIP_POINT:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;

			case Renderer::FilterMode::MIN_MAG_MIP_LINEAR:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;

			case Renderer::FilterMode::ANISOTROPIC:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;	// There's no special setting in Vulkan

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_POINT:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;

			case Renderer::FilterMode::COMPARISON_MIN_MAG_MIP_LINEAR:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;

			case Renderer::FilterMode::COMPARISON_ANISOTROPIC:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;	// There's no special setting in Vulkan

			case Renderer::FilterMode::UNKNOWN:
				RENDERER_ASSERT(context, false, "Vulkan filter mode must not be unknown")
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;

			default:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;	// We should never be in here
		}
	}

	VkSamplerAddressMode Mapping::getVulkanTextureAddressMode(Renderer::TextureAddressMode textureAddressMode)
	{
		static const VkSamplerAddressMode MAPPING[] =
		{
			VK_SAMPLER_ADDRESS_MODE_REPEAT,					// Renderer::TextureAddressMode::WRAP
			VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,		// Renderer::TextureAddressMode::MIRROR
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,			// Renderer::TextureAddressMode::CLAMP
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,		// Renderer::TextureAddressMode::BORDER
			VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE	// Renderer::TextureAddressMode::MIRROR_ONCE
		};
		return MAPPING[static_cast<int>(textureAddressMode) - 1];	// Lookout! The "Renderer::TextureAddressMode"-values start with 1, not 0
	}

	VkBlendFactor Mapping::getVulkanBlendFactor(Renderer::Blend blend)
	{
		static const VkBlendFactor MAPPING[] =
		{
			VK_BLEND_FACTOR_ZERO,						// Renderer::Blend::ZERO			 = 1
			VK_BLEND_FACTOR_ONE,						// Renderer::Blend::ONE				 = 2
			VK_BLEND_FACTOR_SRC_COLOR,					// Renderer::Blend::SRC_COLOR		 = 3
			VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,		// Renderer::Blend::INV_SRC_COLOR	 = 4
			VK_BLEND_FACTOR_SRC_ALPHA,					// Renderer::Blend::SRC_ALPHA		 = 5
			VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,		// Renderer::Blend::INV_SRC_ALPHA	 = 6
			VK_BLEND_FACTOR_DST_ALPHA,					// Renderer::Blend::DEST_ALPHA		 = 7
			VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,		// Renderer::Blend::INV_DEST_ALPHA	 = 8
			VK_BLEND_FACTOR_DST_COLOR,					// Renderer::Blend::DEST_COLOR		 = 9
			VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,		// Renderer::Blend::INV_DEST_COLOR	 = 10
			VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,			// Renderer::Blend::SRC_ALPHA_SAT	 = 11
			VK_BLEND_FACTOR_MAX_ENUM,					// <undefined>						 = 12 !
			VK_BLEND_FACTOR_MAX_ENUM,					// <undefined>						 = 13 !
			VK_BLEND_FACTOR_CONSTANT_COLOR,				// Renderer::Blend::BLEND_FACTOR	 = 14
			VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,	// Renderer::Blend::INV_BLEND_FACTOR = 15
			VK_BLEND_FACTOR_SRC1_COLOR,					// Renderer::Blend::SRC_1_COLOR		 = 16
			VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,		// Renderer::Blend::INV_SRC_1_COLOR	 = 17
			VK_BLEND_FACTOR_SRC1_ALPHA,					// Renderer::Blend::SRC_1_ALPHA		 = 18
			VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA		// Renderer::Blend::INV_SRC_1_ALPHA	 = 19
		};
		return MAPPING[static_cast<int>(blend) - 1];	// Lookout! The "Renderer::Blend"-values start with 1, not 0, there are also holes
	}

	VkBlendOp Mapping::getVulkanBlendOp(Renderer::BlendOp blendOp)
	{
		static const VkBlendOp MAPPING[] =
		{
			VK_BLEND_OP_ADD,				// Renderer::BlendOp::ADD
			VK_BLEND_OP_SUBTRACT,			// Renderer::BlendOp::SUBTRACT
			VK_BLEND_OP_REVERSE_SUBTRACT,	// Renderer::BlendOp::REV_SUBTRACT
			VK_BLEND_OP_MIN,				// Renderer::BlendOp::MIN
			VK_BLEND_OP_MAX					// Renderer::BlendOp::MAX
		};
		return MAPPING[static_cast<int>(blendOp) - 1];	// Lookout! The "Renderer::Blend"-values start with 1, not 0
	}

	VkCompareOp Mapping::getVulkanComparisonFunc(Renderer::ComparisonFunc comparisonFunc)
	{
		static const VkCompareOp MAPPING[] =
		{
			VK_COMPARE_OP_NEVER,			// Renderer::ComparisonFunc::NEVER
			VK_COMPARE_OP_LESS,				// Renderer::ComparisonFunc::LESS
			VK_COMPARE_OP_EQUAL,			// Renderer::ComparisonFunc::EQUAL
			VK_COMPARE_OP_LESS_OR_EQUAL,	// Renderer::ComparisonFunc::LESS_EQUAL
			VK_COMPARE_OP_GREATER,			// Renderer::ComparisonFunc::GREATER
			VK_COMPARE_OP_NOT_EQUAL,		// Renderer::ComparisonFunc::NOT_EQUAL
			VK_COMPARE_OP_GREATER_OR_EQUAL,	// Renderer::ComparisonFunc::GREATER_EQUAL
			VK_COMPARE_OP_ALWAYS			// Renderer::ComparisonFunc::ALWAYS
		};
		return MAPPING[static_cast<int>(comparisonFunc) - 1];	// Lookout! The "Renderer::ComparisonFunc"-values start with 1, not 0
	}

	VkFormat Mapping::getVulkanFormat(Renderer::VertexAttributeFormat vertexAttributeFormat)
	{
		static const VkFormat MAPPING[] =
		{
			VK_FORMAT_R32_SFLOAT,			// Renderer::VertexAttributeFormat::FLOAT_1
			VK_FORMAT_R32G32_SFLOAT,		// Renderer::VertexAttributeFormat::FLOAT_2
			VK_FORMAT_R32G32B32_SFLOAT,		// Renderer::VertexAttributeFormat::FLOAT_3
			VK_FORMAT_R32G32B32A32_SFLOAT,	// Renderer::VertexAttributeFormat::FLOAT_4
			VK_FORMAT_R8G8B8A8_UNORM,		// Renderer::VertexAttributeFormat::R8G8B8A8_UNORM
			VK_FORMAT_R8G8B8A8_UINT,		// Renderer::VertexAttributeFormat::R8G8B8A8_UINT
			VK_FORMAT_R16G16_SINT,			// Renderer::VertexAttributeFormat::SHORT_2
			VK_FORMAT_R16G16B16A16_SINT,	// Renderer::VertexAttributeFormat::SHORT_4
			VK_FORMAT_R32_UINT				// Renderer::VertexAttributeFormat::UINT_1
		};
		return MAPPING[static_cast<int>(vertexAttributeFormat)];
	}

	VkIndexType Mapping::getVulkanType(const Renderer::Context& context, Renderer::IndexBufferFormat::Enum indexBufferFormat)
	{
		RENDERER_ASSERT(context, Renderer::IndexBufferFormat::UNSIGNED_CHAR != indexBufferFormat, "One byte per element index buffer format isn't supported by Vulkan")
		static const VkIndexType MAPPING[] =
		{
			VK_INDEX_TYPE_MAX_ENUM,	// Renderer::IndexBufferFormat::UNSIGNED_CHAR  - One byte per element, uint8_t (may not be supported by each API) - Not supported by Vulkan
			VK_INDEX_TYPE_UINT16,	// Renderer::IndexBufferFormat::UNSIGNED_SHORT - Two bytes per element, uint16_t
			VK_INDEX_TYPE_UINT32	// Renderer::IndexBufferFormat::UNSIGNED_INT   - Four bytes per element, uint32_t (may not be supported by each API)
		};
		return MAPPING[indexBufferFormat];
	}

	VkPrimitiveTopology Mapping::getVulkanType(Renderer::PrimitiveTopology primitiveTopology)
	{
		// Tessellation support: Up to 32 vertices per patch are supported "Renderer::PrimitiveTopology::PATCH_LIST_1" ... "Renderer::PrimitiveTopology::PATCH_LIST_32"
		if (primitiveTopology >= Renderer::PrimitiveTopology::PATCH_LIST_1)
		{
			// Use tessellation
			return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		}
		else
		{
			static const VkPrimitiveTopology MAPPING[] =
			{
				VK_PRIMITIVE_TOPOLOGY_POINT_LIST,		// Renderer::PrimitiveTopology::POINT_LIST
				VK_PRIMITIVE_TOPOLOGY_LINE_LIST,		// Renderer::PrimitiveTopology::LINE_LIST
				VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,		// Renderer::PrimitiveTopology::LINE_STRIP
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,	// Renderer::PrimitiveTopology::TRIANGLE_LIST
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP	// Renderer::PrimitiveTopology::TRIANGLE_STRIP
			};
			return MAPPING[static_cast<int>(primitiveTopology) - 1];	// Lookout! The "Renderer::PrimitiveTopology"-values start with 1, not 0
		}
	}

	VkFormat Mapping::getVulkanFormat(Renderer::TextureFormat::Enum textureFormat)
	{
		static const VkFormat MAPPING[] =
		{
			VK_FORMAT_R8_UNORM,					// Renderer::TextureFormat::R8            - 8-bit pixel format, all bits red
			VK_FORMAT_R8G8B8_UNORM,				// Renderer::TextureFormat::R8G8B8        - 24-bit pixel format, 8 bits for red, green and blue
			VK_FORMAT_R8G8B8A8_UNORM,			// Renderer::TextureFormat::R8G8B8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			VK_FORMAT_R8G8B8A8_SRGB,			// Renderer::TextureFormat::R8G8B8A8_SRGB - 32-bit pixel format, 8 bits for red, green, blue and alpha; sRGB = RGB hardware gamma correction, the alpha channel always remains linear
			VK_FORMAT_B8G8R8A8_UNORM,			// Renderer::TextureFormat::B8G8R8A8      - 32-bit pixel format, 8 bits for red, green, blue and alpha
			VK_FORMAT_B10G11R11_UFLOAT_PACK32,	// Renderer::TextureFormat::R11G11B10F    - 32-bit float format using 11 bits the red and green channel, 10 bits the blue channel; red and green channels have a 6 bits mantissa and a 5 bits exponent and blue has a 5 bits mantissa and 5 bits exponent
			VK_FORMAT_R16G16B16A16_SFLOAT,		// Renderer::TextureFormat::R16G16B16A16F - 64-bit float format using 16 bits for the each channel (red, green, blue, alpha)
			VK_FORMAT_R32G32B32A32_SFLOAT,		// Renderer::TextureFormat::R32G32B32A32F - 128-bit float format using 32 bits for the each channel (red, green, blue, alpha)
			VK_FORMAT_BC1_RGB_UNORM_BLOCK,		// Renderer::TextureFormat::BC1           - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block)
			VK_FORMAT_BC1_RGB_SRGB_BLOCK,		// Renderer::TextureFormat::BC1_SRGB      - DXT1 compression (known as BC1 in DirectX 10, RGB compression: 8:1, 8 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear
			VK_FORMAT_BC2_UNORM_BLOCK,			// Renderer::TextureFormat::BC2           - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			VK_FORMAT_BC2_SRGB_BLOCK,			// Renderer::TextureFormat::BC2_SRGB      - DXT3 compression (known as BC2 in DirectX 10, RGBA compression: 4:1, 16 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear
			VK_FORMAT_BC3_UNORM_BLOCK,			// Renderer::TextureFormat::BC3           - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block)
			VK_FORMAT_BC3_SRGB_BLOCK,			// Renderer::TextureFormat::BC3_SRGB      - DXT5 compression (known as BC3 in DirectX 10, RGBA compression: 4:1, 16 bytes per block); sRGB = RGB hardware gamma correction, the alpha channel always remains linear
			VK_FORMAT_BC4_UNORM_BLOCK,			// Renderer::TextureFormat::BC4           - 1 component texture compression (also known as 3DC+/ATI1N, known as BC4 in DirectX 10, 8 bytes per block)
			VK_FORMAT_BC5_UNORM_BLOCK,			// Renderer::TextureFormat::BC5           - 2 component texture compression (luminance & alpha compression 4:1 -> normal map compression, also known as 3DC/ATI2N, known as BC5 in DirectX 10, 16 bytes per block)
			VK_FORMAT_UNDEFINED,				// Renderer::TextureFormat::ETC1          - 3 component texture compression meant for mobile devices - not supported in Direct3D 11 - TODO(co) Check for Vulkan format
			VK_FORMAT_R32_UINT,					// Renderer::TextureFormat::R32_UINT      - 32-bit unsigned integer format
			VK_FORMAT_R32_SFLOAT,				// Renderer::TextureFormat::R32_FLOAT     - 32-bit float format
			VK_FORMAT_D32_SFLOAT,				// Renderer::TextureFormat::D32_FLOAT     - 32-bit float depth format
			VK_FORMAT_R16G16_UNORM,				// Renderer::TextureFormat::R16G16_SNORM  - A two-component, 32-bit signed-normalized-integer format that supports 16 bits for the red channel and 16 bits for the green channel
			VK_FORMAT_R16G16_SFLOAT,			// Renderer::TextureFormat::R16G16_FLOAT  - A two-component, 32-bit floating-point format that supports 16 bits for the red channel and 16 bits for the green channel
			VK_FORMAT_UNDEFINED					// Renderer::TextureFormat::UNKNOWN       - Unknown
		};
		return MAPPING[textureFormat];
	}

	VkSampleCountFlagBits Mapping::getVulkanSampleCountFlagBits(const Renderer::Context& context, uint8_t numberOfMultisamples)
	{
		RENDERER_ASSERT(context, numberOfMultisamples <= 8, "Invalid number of Vulkan multisamples")
		static const VkSampleCountFlagBits MAPPING[] =
		{
			VK_SAMPLE_COUNT_1_BIT,
			VK_SAMPLE_COUNT_2_BIT,
			VK_SAMPLE_COUNT_4_BIT,
			VK_SAMPLE_COUNT_8_BIT
		};
		return MAPPING[numberOfMultisamples - 1];	// Lookout! The "numberOfMultisamples"-values start with 1, not 0
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
