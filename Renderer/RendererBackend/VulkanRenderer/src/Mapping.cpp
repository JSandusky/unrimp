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


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
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

	VkIndexType Mapping::getVulkanType(Renderer::IndexBufferFormat::Enum indexBufferFormat)
	{
		assert(Renderer::IndexBufferFormat::UNSIGNED_CHAR != indexBufferFormat && "One byte per element index buffer format isn't supported by Vulkan");
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


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
