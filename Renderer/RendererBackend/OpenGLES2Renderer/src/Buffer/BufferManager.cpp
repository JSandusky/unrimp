/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "OpenGLES2Renderer/Buffer/BufferManager.h"
#include "OpenGLES2Renderer/Buffer/VertexArrayNoVao.h"
#include "OpenGLES2Renderer/Buffer/VertexArrayVao.h"
#include "OpenGLES2Renderer/Buffer/VertexBuffer.h"
#include "OpenGLES2Renderer/Buffer/IndexBuffer.h"
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"
#include "OpenGLES2Renderer/IExtensions.h"
#include "OpenGLES2Renderer/IContext.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	BufferManager::BufferManager(OpenGLES2Renderer& openGLES2Renderer) :
		IBufferManager(openGLES2Renderer),
		mExtensions(&openGLES2Renderer.getContext().getExtensions())
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IBufferManager methods       ]
	//[-------------------------------------------------------]
	Renderer::IVertexBuffer* BufferManager::createVertexBuffer(uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return new VertexBuffer(static_cast<OpenGLES2Renderer&>(getRenderer()), numberOfBytes, data, bufferUsage);
	}

	Renderer::IIndexBuffer* BufferManager::createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void* data, Renderer::BufferUsage bufferUsage)
	{
		return new IndexBuffer(static_cast<OpenGLES2Renderer&>(getRenderer()), numberOfBytes, indexBufferFormat, data, bufferUsage);
	}

	Renderer::IVertexArray* BufferManager::createVertexArray(const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, Renderer::IIndexBuffer* indexBuffer)
	{
		// Is "GL_OES_vertex_array_object" there?
		if (mExtensions->isGL_OES_vertex_array_object())
		{
			// Effective vertex array object (VAO)
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			return new VertexArrayVao(static_cast<OpenGLES2Renderer&>(getRenderer()), vertexAttributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
		}
		else
		{
			// Traditional version
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			return new VertexArrayNoVao(static_cast<OpenGLES2Renderer&>(getRenderer()), vertexAttributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
		}
	}

	Renderer::IUniformBuffer* BufferManager::createUniformBuffer(uint32_t, const void*, Renderer::BufferUsage)
	{
		// Error! OpenGL ES 2 has no uniform buffer support.
		return nullptr;
	}

	Renderer::ITextureBuffer* BufferManager::createTextureBuffer(uint32_t, Renderer::TextureFormat::Enum, const void*, Renderer::BufferUsage)
	{
		// OpenGL ES 2 has no texture buffer support
		return nullptr;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
