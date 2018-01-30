/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include <Renderer/Buffer/IIndirectBuffer.h>
#include <Renderer/Buffer/BufferTypes.h>

#include "VulkanRenderer/Vulkan.h"
#include "VulkanRenderer/Helper.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{
	class VulkanRenderer;
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
	*    Vulkan indirect buffer object interface
	*/
	class IndirectBuffer final : public Renderer::IIndirectBuffer
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] vulkanRenderer
		*    Owner Vulkan renderer instance
		*  @param[in] numberOfBytes
		*    Number of bytes within the indirect buffer, must be valid
		*  @param[in] data
		*    Indirect buffer data, can be a null pointer (empty buffer)
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*/
		IndirectBuffer(VulkanRenderer& vulkanRenderer, uint32_t numberOfBytes, const void* data = nullptr, Renderer::BufferUsage bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~IndirectBuffer() override;

		/**
		*  @brief
		*    Return the Vulkan indirect buffer
		*
		*  @return
		*    The Vulkan indirect buffer
		*/
		inline VkBuffer getVkBuffer() const;

		/**
		*  @brief
		*    Return the Vulkan device memory
		*
		*  @return
		*    The Vulkan device memory
		*/
		inline VkDeviceMemory getVkDeviceMemory() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IIndirectBuffer methods      ]
	//[-------------------------------------------------------]
	public:
		inline virtual const uint8_t* getEmulationData() const override;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		DECLARE_SET_DEBUG_NAME	// virtual void setDebugName(const char* name) override;


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	protected:
		virtual void selfDestruct() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit IndirectBuffer(const IndirectBuffer& source) = delete;
		IndirectBuffer& operator =(const IndirectBuffer& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		VkBuffer	   mVkBuffer;		///< Vulkan indirect buffer
		VkDeviceMemory mVkDeviceMemory;	///< Vulkan indirect memory


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "VulkanRenderer/Buffer/IndirectBuffer.inl"
