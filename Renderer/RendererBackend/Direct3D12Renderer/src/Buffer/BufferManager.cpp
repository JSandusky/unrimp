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
#include "Direct3D12Renderer/Buffer/BufferManager.h"
#include "Direct3D12Renderer/Buffer/IndirectBuffer.h"
#include "Direct3D12Renderer/Buffer/TextureBuffer.h"
#include "Direct3D12Renderer/Buffer/UniformBuffer.h"
#include "Direct3D12Renderer/Buffer/VertexBuffer.h"
#include "Direct3D12Renderer/Buffer/VertexArray.h"
#include "Direct3D12Renderer/Buffer/IndexBuffer.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"

#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	BufferManager::BufferManager(Direct3D12Renderer& direct3D12Renderer) :
		IBufferManager(direct3D12Renderer)
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IBufferManager methods       ]
	//[-------------------------------------------------------]
	Renderer::IVertexBuffer* BufferManager::createVertexBuffer(uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return RENDERER_NEW(getRenderer().getContext(), VertexBuffer)(static_cast<Direct3D12Renderer&>(getRenderer()), numberOfBytes, data, bufferUsage);
	}

	Renderer::IIndexBuffer* BufferManager::createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return RENDERER_NEW(getRenderer().getContext(), IndexBuffer)(static_cast<Direct3D12Renderer&>(getRenderer()), numberOfBytes, indexBufferFormat, data, bufferUsage);
	}

	Renderer::IVertexArray* BufferManager::createVertexArray(const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, Renderer::IIndexBuffer* indexBuffer)
	{
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		return RENDERER_NEW(getRenderer().getContext(), VertexArray)(static_cast<Direct3D12Renderer&>(getRenderer()), vertexAttributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
	}

	Renderer::IUniformBuffer* BufferManager::createUniformBuffer(uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return RENDERER_NEW(getRenderer().getContext(), UniformBuffer)(static_cast<Direct3D12Renderer&>(getRenderer()), numberOfBytes, data, bufferUsage);
	}

	Renderer::ITextureBuffer* BufferManager::createTextureBuffer(uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return RENDERER_NEW(getRenderer().getContext(), TextureBuffer)(static_cast<Direct3D12Renderer&>(getRenderer()), numberOfBytes, textureFormat, data, bufferUsage);
	}

	Renderer::IIndirectBuffer* BufferManager::createIndirectBuffer(uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return RENDERER_NEW(getRenderer().getContext(), IndirectBuffer)(static_cast<Direct3D12Renderer&>(getRenderer()), numberOfBytes, data, bufferUsage);
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void BufferManager::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), BufferManager, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
