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
#include <Renderer/Buffer/VertexArrayTypes.h>
#include <Renderer/Buffer/IndexBufferTypes.h>
#include <Renderer/State/PipelineStateTypes.h>
#include <Renderer/State/SamplerStateTypes.h>

#include "VulkanRenderer/Vulkan.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class Context;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Vulkan mapping
	*/
	class Mapping
	{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		//[-------------------------------------------------------]
		//[ Renderer::FilterMode                                  ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::FilterMode" to Vulkan magnification filter mode
		*
		*  @param[in] context
		*    Renderer context to use
		*  @param[in] filterMode
		*    "Renderer::FilterMode" to map
		*
		*  @return
		*    Vulkan magnification filter mode
		*/
		static VkFilter getVulkanMagFilterMode(const Renderer::Context& context, Renderer::FilterMode filterMode);

		/**
		*  @brief
		*    "Renderer::FilterMode" to Vulkan minification filter mode
		*
		*  @param[in] context
		*    Renderer context to use
		*  @param[in] filterMode
		*    "Renderer::FilterMode" to map
		*
		*  @return
		*    Vulkan minification filter mode
		*/
		static VkFilter getVulkanMinFilterMode(const Renderer::Context& context, Renderer::FilterMode filterMode);

		/**
		*  @brief
		*    "Renderer::FilterMode" to Vulkan sampler mipmap mode
		*
		*  @param[in] context
		*    Renderer context to use
		*  @param[in] filterMode
		*    "Renderer::FilterMode" to map
		*
		*  @return
		*    Vulkan sampler mipmap mode
		*/
		static VkSamplerMipmapMode getVulkanMipmapMode(const Renderer::Context& context, Renderer::FilterMode filterMode);

		//[-------------------------------------------------------]
		//[ Renderer::TextureAddressMode                          ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::TextureAddressMode" to Vulkan texture address mode
		*
		*  @param[in] textureAddressMode
		*    "Renderer::TextureAddressMode" to map
		*
		*  @return
		*    Vulkan texture address mode
		*/
		static VkSamplerAddressMode getVulkanTextureAddressMode(Renderer::TextureAddressMode textureAddressMode);

		//[-------------------------------------------------------]
		//[ Renderer::Blend                                       ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::Blend" to Vulkan blend factor
		*
		*  @param[in] blend
		*    "Renderer::Blend" to map
		*
		*  @return
		*    Vulkan blend factor
		*/
		static VkBlendFactor getVulkanBlendFactor(Renderer::Blend blend);

		/**
		*  @brief
		*    "Renderer::BlendOp" to Vulkan blend operation
		*
		*  @param[in] blendOp
		*    "Renderer::BlendOp" to map
		*
		*  @return
		*    Vulkan blend operation
		*/
		static VkBlendOp getVulkanBlendOp(Renderer::BlendOp blendOp);

		//[-------------------------------------------------------]
		//[ Renderer::ComparisonFunc                              ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::ComparisonFunc" to Vulkan comparison function
		*
		*  @param[in] comparisonFunc
		*    "Renderer::ComparisonFunc" to map
		*
		*  @return
		*    Vulkan comparison function
		*/
		static VkCompareOp getVulkanComparisonFunc(Renderer::ComparisonFunc comparisonFunc);

		//[-------------------------------------------------------]
		//[ Renderer::VertexAttributeFormat and semantic          ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::VertexAttributeFormat" to Vulkan format
		*
		*  @param[in] vertexAttributeFormat
		*    "Renderer::VertexAttributeFormat" to map
		*
		*  @return
		*    Vulkan format
		*/
		static VkFormat getVulkanFormat(Renderer::VertexAttributeFormat vertexAttributeFormat);

		//[-------------------------------------------------------]
		//[ Renderer::IndexBufferFormat                           ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::IndexBufferFormat" to Vulkan type
		*
		*  @param[in] context
		*    Renderer context to use
		*  @param[in] indexBufferFormat
		*    "Renderer::IndexBufferFormat" to map
		*
		*  @return
		*    Vulkan index type
		*/
		static VkIndexType getVulkanType(const Renderer::Context& context, Renderer::IndexBufferFormat::Enum indexBufferFormat);

		//[-------------------------------------------------------]
		//[ Renderer::PrimitiveTopology                           ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::PrimitiveTopology" to Vulkan type
		*
		*  @param[in] primitiveTopology
		*    "Renderer::PrimitiveTopology" to map
		*
		*  @return
		*    Vulkan type
		*/
		static VkPrimitiveTopology getVulkanType(Renderer::PrimitiveTopology primitiveTopology);

		//[-------------------------------------------------------]
		//[ Renderer::TextureFormat                               ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    "Renderer::TextureFormat" to Vulkan format
		*
		*  @param[in] textureFormat
		*    "Renderer::TextureFormat" to map
		*
		*  @return
		*    Vulkan format
		*/
		static VkFormat getVulkanFormat(Renderer::TextureFormat::Enum textureFormat);

		/**
		*  @brief
		*    Number of multisamples to Vulkan sample count flag bits
		*
		*  @param[in] context
		*    Renderer context to use
		*  @param[in] numberOfMultisamples
		*    The number of multisamples per pixel (valid values: 1, 2, 4, 8)
		*
		*  @return
		*    Vulkan sample count flag bits
		*/
		static VkSampleCountFlagBits getVulkanSampleCountFlagBits(const Renderer::Context& context, uint8_t numberOfMultisamples);


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
