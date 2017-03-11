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
#include "OpenGLES3Renderer/Buffer/BufferManager.h"
#include "OpenGLES3Renderer/Buffer/VertexArray.h"
#include "OpenGLES3Renderer/Buffer/IndirectBuffer.h"
#include "OpenGLES3Renderer/Buffer/VertexBuffer.h"
#include "OpenGLES3Renderer/Buffer/IndexBuffer.h"
#include "OpenGLES3Renderer/Buffer/TextureBufferBind.h"
#include "OpenGLES3Renderer/Buffer/TextureBufferBindEmulation.h"
#include "OpenGLES3Renderer/Buffer/UniformBufferBind.h"
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"
#include "OpenGLES3Renderer/IExtensions.h"
#include "OpenGLES3Renderer/IContext.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	BufferManager::BufferManager(OpenGLES3Renderer& openGLES3Renderer) :
		IBufferManager(openGLES3Renderer),
		mExtensions(&openGLES3Renderer.getContext().getExtensions())
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IBufferManager methods       ]
	//[-------------------------------------------------------]
	Renderer::IVertexBuffer* BufferManager::createVertexBuffer(uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return new VertexBuffer(static_cast<OpenGLES3Renderer&>(getRenderer()), numberOfBytes, data, bufferUsage);
	}

	Renderer::IIndexBuffer* BufferManager::createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return new IndexBuffer(static_cast<OpenGLES3Renderer&>(getRenderer()), numberOfBytes, indexBufferFormat, data, bufferUsage);
	}

	Renderer::IVertexArray* BufferManager::createVertexArray(const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, Renderer::IIndexBuffer* indexBuffer)
	{
		// Effective vertex array object (VAO)
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		return new VertexArray(static_cast<OpenGLES3Renderer&>(getRenderer()), vertexAttributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
	}

	Renderer::IUniformBuffer* BufferManager::createUniformBuffer(uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return new UniformBufferBind(static_cast<OpenGLES3Renderer&>(getRenderer()), numberOfBytes, data, bufferUsage);
	}

	Renderer::ITextureBuffer* BufferManager::createTextureBuffer(uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void* data, Renderer::BufferUsage bufferUsage)
	{
		// Is "GL_EXT_texture_buffer" there?
		if (mExtensions->isGL_EXT_texture_buffer())
		{
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			return new TextureBufferBind(static_cast<OpenGLES3Renderer&>(getRenderer()), numberOfBytes, textureFormat, data, bufferUsage);
		}

		// We can only emulate the "Renderer::TextureFormat::R32G32B32A32F" texture format using an uniform buffer
		else if (Renderer::TextureFormat::R32G32B32A32F == textureFormat)
		{
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			return new TextureBufferBindEmulation(static_cast<OpenGLES3Renderer&>(getRenderer()), numberOfBytes, textureFormat, data, bufferUsage);
		}

		// Error!
		return nullptr;
	}

	Renderer::IIndirectBuffer* BufferManager::createIndirectBuffer(uint32_t numberOfBytes, const void* data, Renderer::BufferUsage)
	{
		return new IndirectBuffer(static_cast<OpenGLES3Renderer&>(getRenderer()), numberOfBytes, data);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
