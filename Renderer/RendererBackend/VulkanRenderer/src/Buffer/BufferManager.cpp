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
#include "VulkanRenderer/Buffer/BufferManager.h"
#include "VulkanRenderer/Buffer/IndirectBuffer.h"
#include "VulkanRenderer/Buffer/TextureBuffer.h"
#include "VulkanRenderer/Buffer/UniformBuffer.h"
#include "VulkanRenderer/Buffer/VertexBuffer.h"
#include "VulkanRenderer/Buffer/VertexArray.h"
#include "VulkanRenderer/Buffer/IndexBuffer.h"
#include "VulkanRenderer/VulkanRenderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	BufferManager::BufferManager(VulkanRenderer& vulkanRenderer) :
		IBufferManager(vulkanRenderer)
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IBufferManager methods       ]
	//[-------------------------------------------------------]
	Renderer::IVertexBuffer* BufferManager::createVertexBuffer(uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return new VertexBuffer(static_cast<VulkanRenderer&>(getRenderer()), numberOfBytes, data, bufferUsage);
	}

	Renderer::IIndexBuffer* BufferManager::createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return new IndexBuffer(static_cast<VulkanRenderer&>(getRenderer()), numberOfBytes, indexBufferFormat, data, bufferUsage);
	}

	Renderer::IVertexArray* BufferManager::createVertexArray(const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, Renderer::IIndexBuffer* indexBuffer)
	{
		return new VertexArray(static_cast<VulkanRenderer&>(getRenderer()), vertexAttributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
	}

	Renderer::IUniformBuffer* BufferManager::createUniformBuffer(uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return new UniformBuffer(static_cast<VulkanRenderer&>(getRenderer()), numberOfBytes, data, bufferUsage);
	}

	Renderer::ITextureBuffer* BufferManager::createTextureBuffer(uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return new TextureBuffer(static_cast<VulkanRenderer&>(getRenderer()), numberOfBytes, textureFormat, data, bufferUsage);
	}

	Renderer::IIndirectBuffer* BufferManager::createIndirectBuffer(uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return new IndirectBuffer(static_cast<VulkanRenderer&>(getRenderer()), numberOfBytes, data, bufferUsage);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
