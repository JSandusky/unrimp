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
#include "VulkanRenderer/Buffer/IndexBuffer.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/Mapping.h"
#include "VulkanRenderer/Helper.h"

#include <Renderer/IAssert.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IndexBuffer::IndexBuffer(VulkanRenderer& vulkanRenderer, uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void* data, Renderer::BufferUsage) :
		IIndexBuffer(vulkanRenderer),
		mVkIndexType(Mapping::getVulkanType(vulkanRenderer.getContext(), indexBufferFormat)),
		mVkBuffer(VK_NULL_HANDLE),
		mVkDeviceMemory(VK_NULL_HANDLE)
	{
		Helper::createAndAllocateVkBuffer(vulkanRenderer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, numberOfBytes, data, mVkBuffer, mVkDeviceMemory);
		SET_DEFAULT_DEBUG_NAME	// setDebugName("");
	}

	IndexBuffer::~IndexBuffer()
	{
		Helper::destroyAndFreeVkBuffer(static_cast<const VulkanRenderer&>(getRenderer()), mVkBuffer, mVkDeviceMemory);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	DEFINE_SET_DEBUG_NAME_VKBUFFER_VKDEVICEMEMORY(IndexBuffer, "IBO", 6)	// void IndexBuffer::setDebugName(const char* name)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
