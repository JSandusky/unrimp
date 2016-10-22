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
#include "OpenGLRenderer/Buffer/BufferManager.h"
#include "OpenGLRenderer/Buffer/VertexArrayNoVao.h"
#include "OpenGLRenderer/Buffer/VertexArrayVaoDsa.h"
#include "OpenGLRenderer/Buffer/VertexArrayVaoBind.h"
#include "OpenGLRenderer/Buffer/VertexBufferDsa.h"
#include "OpenGLRenderer/Buffer/VertexBufferBind.h"
#include "OpenGLRenderer/Buffer/UniformBufferDsa.h"
#include "OpenGLRenderer/Buffer/UniformBufferBind.h"
#include "OpenGLRenderer/Buffer/IndexBufferDsa.h"
#include "OpenGLRenderer/Buffer/IndexBufferBind.h"
#include "OpenGLRenderer/Buffer/TextureBufferDsa.h"
#include "OpenGLRenderer/Buffer/TextureBufferBind.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/Extensions.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	BufferManager::BufferManager(OpenGLRenderer& openGLRenderer) :
		IBufferManager(openGLRenderer),
		mExtensions(&openGLRenderer.getExtensions())
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IBufferManager methods       ]
	//[-------------------------------------------------------]
	Renderer::IVertexBuffer* BufferManager::createVertexBuffer(uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage)
	{
		// "GL_ARB_vertex_buffer_object" required
		if (mExtensions->isGL_ARB_vertex_buffer_object())
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mExtensions->isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				return new VertexBufferDsa(static_cast<OpenGLRenderer&>(getRenderer()), numberOfBytes, data, bufferUsage);
			}
			else
			{
				// Traditional bind version
				return new VertexBufferBind(static_cast<OpenGLRenderer&>(getRenderer()), numberOfBytes, data, bufferUsage);
			}
		}
		else
		{
			// Error!
			return nullptr;
		}
	}

	Renderer::IIndexBuffer* BufferManager::createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void* data, Renderer::BufferUsage bufferUsage)
	{
		// "GL_ARB_vertex_buffer_object" required
		if (mExtensions->isGL_ARB_vertex_buffer_object())
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mExtensions->isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				return new IndexBufferDsa(static_cast<OpenGLRenderer&>(getRenderer()), numberOfBytes, indexBufferFormat, data, bufferUsage);
			}
			else
			{
				// Traditional bind version
				return new IndexBufferBind(static_cast<OpenGLRenderer&>(getRenderer()), numberOfBytes, indexBufferFormat, data, bufferUsage);
			}
		}
		else
		{
			// Error!
			return nullptr;
		}
	}

	Renderer::IVertexArray* BufferManager::createVertexArray(const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, Renderer::IIndexBuffer* indexBuffer)
	{
		// Is "GL_ARB_vertex_array_object" there?
		if (mExtensions->isGL_ARB_vertex_array_object())
		{
			// Effective vertex array object (VAO)

			// Is "GL_EXT_direct_state_access" there?
			if (mExtensions->isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				return new VertexArrayVaoDsa(static_cast<OpenGLRenderer&>(getRenderer()), vertexAttributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
			}
			else
			{
				// Traditional bind version
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				return new VertexArrayVaoBind(static_cast<OpenGLRenderer&>(getRenderer()), vertexAttributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
			}
		}
		else
		{
			// Traditional version
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			return new VertexArrayNoVao(static_cast<OpenGLRenderer&>(getRenderer()), vertexAttributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
		}
	}

	Renderer::IUniformBuffer* BufferManager::createUniformBuffer(uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage)
	{
		// "GL_ARB_uniform_buffer_object" required
		if (mExtensions->isGL_ARB_uniform_buffer_object())
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mExtensions->isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				return new UniformBufferDsa(static_cast<OpenGLRenderer&>(getRenderer()), numberOfBytes, data, bufferUsage);
			}
			else
			{
				// Traditional bind version
				return new UniformBufferBind(static_cast<OpenGLRenderer&>(getRenderer()), numberOfBytes, data, bufferUsage);
			}
		}
		else
		{
			// Error!
			return nullptr;
		}
	}

	Renderer::ITextureBuffer* BufferManager::createTextureBuffer(uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void* data, Renderer::BufferUsage bufferUsage)
	{
		// "GL_ARB_texture_buffer_object" required
		if (mExtensions->isGL_ARB_texture_buffer_object())
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mExtensions->isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				return new TextureBufferDsa(static_cast<OpenGLRenderer&>(getRenderer()), numberOfBytes, textureFormat, data, bufferUsage);
			}
			else
			{
				// Traditional bind version
				return new TextureBufferBind(static_cast<OpenGLRenderer&>(getRenderer()), numberOfBytes, textureFormat, data, bufferUsage);
			}
		}
		else
		{
			// Error!
			return nullptr;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
