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
#include "NullRenderer/Buffer/BufferManager.h"
#include "NullRenderer/Buffer/IndirectBuffer.h"
#include "NullRenderer/Buffer/TextureBuffer.h"
#include "NullRenderer/Buffer/UniformBuffer.h"
#include "NullRenderer/Buffer/VertexBuffer.h"
#include "NullRenderer/Buffer/VertexArray.h"
#include "NullRenderer/Buffer/IndexBuffer.h"
#include "NullRenderer/NullRenderer.h"

#include <Renderer/Buffer/VertexArrayTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace NullRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	BufferManager::BufferManager(NullRenderer& nullRenderer) :
		IBufferManager(nullRenderer)
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IBufferManager methods       ]
	//[-------------------------------------------------------]
	Renderer::IVertexBuffer* BufferManager::createVertexBuffer(uint32_t, const void*, Renderer::BufferUsage)
	{
		return new VertexBuffer(static_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IIndexBuffer* BufferManager::createIndexBuffer(uint32_t, Renderer::IndexBufferFormat::Enum, const void*, Renderer::BufferUsage)
	{
		return new IndexBuffer(static_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IVertexArray* BufferManager::createVertexArray(const Renderer::VertexAttributes&, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, Renderer::IIndexBuffer* indexBuffer)
	{
		// We don't keep a reference to the vertex buffers used by the vertex array attributes in here
		// -> Ensure a correct reference counter behaviour
		const Renderer::VertexArrayVertexBuffer *vertexBufferEnd = vertexBuffers + numberOfVertexBuffers;
		for (const Renderer::VertexArrayVertexBuffer *vertexBuffer = vertexBuffers; vertexBuffer < vertexBufferEnd; ++vertexBuffer)
		{
			vertexBuffer->vertexBuffer->addReference();
			vertexBuffer->vertexBuffer->releaseReference();
		}

		// We don't keep a reference to the index buffer in here
		// -> Ensure a correct reference counter behaviour
		if (nullptr != indexBuffer)
		{
			indexBuffer->addReference();
			indexBuffer->releaseReference();
		}

		// Create the vertex array instance
		return new VertexArray(static_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IUniformBuffer* BufferManager::createUniformBuffer(uint32_t, const void*, Renderer::BufferUsage)
	{
		return new UniformBuffer(static_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::ITextureBuffer* BufferManager::createTextureBuffer(uint32_t, Renderer::TextureFormat::Enum, const void*, Renderer::BufferUsage)
	{
		return new TextureBuffer(static_cast<NullRenderer&>(getRenderer()));
	}

	Renderer::IIndirectBuffer* BufferManager::createIndirectBuffer(uint32_t, const void*, Renderer::BufferUsage)
	{
		return new IndirectBuffer(static_cast<NullRenderer&>(getRenderer()));
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // NullRenderer
