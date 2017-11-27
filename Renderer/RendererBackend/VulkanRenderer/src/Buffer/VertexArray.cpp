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
#include "VulkanRenderer/Buffer/VertexArray.h"
#include "VulkanRenderer/Buffer/IndexBuffer.h"
#include "VulkanRenderer/Buffer/VertexBuffer.h"
#include "VulkanRenderer/VulkanRuntimeLinking.h"

#include <Renderer/Buffer/VertexArrayTypes.h>

#include <cstring>	// For "memcpy()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArray::VertexArray(VulkanRenderer& vulkanRenderer, const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, IndexBuffer* indexBuffer) :
		IVertexArray(reinterpret_cast<Renderer::IRenderer&>(vulkanRenderer)),
		mIndexBuffer(indexBuffer),
		mNumberOfSlots(numberOfVertexBuffers),
		mVertexVkBuffers(nullptr),
		mStrides(nullptr),
		mOffsets(nullptr),
		mVertexBuffers(nullptr)
	{
		// Add a reference to the given index buffer
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->addReference();
		}

		// Add a reference to the used vertex buffers
		if (mNumberOfSlots > 0)
		{
			mVertexVkBuffers = new VkBuffer[mNumberOfSlots];
			mStrides = new uint32_t[mNumberOfSlots];
			mOffsets = new VkDeviceSize[mNumberOfSlots]{};	// Vertex buffer offset is not supported by OpenGL, so our renderer API doesn't support it either, set everything to zero
			mVertexBuffers = new VertexBuffer*[mNumberOfSlots];

			{ // Loop through all vertex buffers
				VkBuffer* currentVertexVkBuffer = mVertexVkBuffers;
				VertexBuffer** currentVertexBuffer = mVertexBuffers;
				const Renderer::VertexArrayVertexBuffer* vertexBufferEnd = vertexBuffers + mNumberOfSlots;
				for (const Renderer::VertexArrayVertexBuffer* vertexBuffer = vertexBuffers; vertexBuffer < vertexBufferEnd; ++vertexBuffer, ++currentVertexVkBuffer, ++currentVertexBuffer)
				{
					// TODO(co) Add security check: Is the given resource one of the currently used renderer?
					*currentVertexBuffer = static_cast<VertexBuffer*>(vertexBuffer->vertexBuffer);
					*currentVertexVkBuffer = (*currentVertexBuffer)->getVkBuffer();
					(*currentVertexBuffer)->addReference();
				}
			}

			{ // Gather slot related data
				const Renderer::VertexAttribute* attribute = vertexAttributes.attributes;
				const Renderer::VertexAttribute* attributesEnd = attribute + vertexAttributes.numberOfAttributes;
				for (; attribute < attributesEnd;  ++attribute)
				{
					mStrides[attribute->inputSlot] = attribute->strideInBytes;
				}
			}
		}
	}

	VertexArray::~VertexArray()
	{
		// Release the index buffer reference
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->releaseReference();
		}

		// Cleanup Vulkan input slot data
		if (mNumberOfSlots > 0)
		{
			delete [] mVertexVkBuffers;
			delete [] mStrides;
			delete [] mOffsets;
		}

		// Release the reference to the used vertex buffers
		if (nullptr != mVertexBuffers)
		{
			// Release references
			VertexBuffer** vertexBuffersEnd = mVertexBuffers + mNumberOfSlots;
			for (VertexBuffer** vertexBuffer = mVertexBuffers; vertexBuffer < vertexBuffersEnd; ++vertexBuffer)
			{
				(*vertexBuffer)->releaseReference();
			}

			// Cleanup
			delete [] mVertexBuffers;
		}
	}

	void VertexArray::bindVulkanBuffers(VkCommandBuffer vkCommandBuffer) const
	{
		// Set the Vulkan vertex buffers
		if (nullptr != mVertexVkBuffers)
		{
			vkCmdBindVertexBuffers(vkCommandBuffer, 0, mNumberOfSlots, mVertexVkBuffers, mOffsets);
		}
		else
		{
			// Do nothing since the Vulkan specification says "bindingCount must be greater than 0"
			// vkCmdBindVertexBuffers(vkCommandBuffer, 0, 0, nullptr, nullptr);
		}

		// Set the used index buffer
		// -> In case of no index buffer we don't set null indices, there's not really a point in it
		if (nullptr != mIndexBuffer)
		{
			vkCmdBindIndexBuffer(vkCommandBuffer, mIndexBuffer->getVkBuffer(), 0, mIndexBuffer->getVkIndexType());
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
