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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "VulkanRenderer/Buffer/UniformBuffer.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"
#include "VulkanRenderer/Helper.h"

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>
#include <Renderer/IAllocator.h>

#include <cstring>	// For "memcpy()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	UniformBuffer::UniformBuffer(VulkanRenderer& vulkanRenderer, uint32_t numberOfBytes, const void* data, Renderer::BufferUsage) :
		IUniformBuffer(vulkanRenderer),
		mVkBuffer(VK_NULL_HANDLE),
		mVkDeviceMemory(VK_NULL_HANDLE)
	{
		Helper::createAndAllocateVkBuffer(vulkanRenderer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, numberOfBytes, data, mVkBuffer, mVkDeviceMemory);
		SET_DEFAULT_DEBUG_NAME	// setDebugName("");
	}

	UniformBuffer::~UniformBuffer()
	{
		Helper::destroyAndFreeVkBuffer(static_cast<const VulkanRenderer&>(getRenderer()), mVkBuffer, mVkDeviceMemory);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	DEFINE_SET_DEBUG_NAME_VKBUFFER_VKDEVICEMEMORY(UniformBuffer, "UBO", 6)	// void UniformBuffer::setDebugName(const char* name)


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void UniformBuffer::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), UniformBuffer, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
