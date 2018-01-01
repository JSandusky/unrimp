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
#include "OpenGLRenderer/Buffer/VertexArrayVao.h"
#include "OpenGLRenderer/Buffer/VertexBuffer.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/Extensions.h"

#include <Renderer/IAllocator.h>
#include <Renderer/Buffer/VertexArrayTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArrayVao::~VertexArrayVao()
	{
		// Destroy the OpenGL vertex array
		// -> Silently ignores 0's and names that do not correspond to existing vertex array objects
		glDeleteVertexArrays(1, &mOpenGLVertexArray);

		// Release the reference to the used vertex buffers
		if (nullptr != mVertexBuffers)
		{
			// Release references
			VertexBuffer** vertexBuffersEnd = mVertexBuffers + mNumberOfVertexBuffers;
			for (VertexBuffer** vertexBuffer = mVertexBuffers; vertexBuffer < vertexBuffersEnd; ++vertexBuffer)
			{
				(*vertexBuffer)->releaseReference();
			}

			// Cleanup
			RENDERER_FREE(getRenderer().getContext(), mVertexBuffers);
		}
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	VertexArrayVao::VertexArrayVao(OpenGLRenderer& openGLRenderer, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, IndexBuffer* indexBuffer) :
		VertexArray(openGLRenderer, indexBuffer, InternalResourceType::VAO),
		mOpenGLVertexArray(0),
		mNumberOfVertexBuffers(numberOfVertexBuffers),
		mVertexBuffers(nullptr)
	{
		// Add a reference to the used vertex buffers
		if (numberOfVertexBuffers > 0)
		{
			mVertexBuffers = RENDERER_MALLOC_TYPED(openGLRenderer.getContext(), VertexBuffer*, numberOfVertexBuffers);

			// Loop through all vertex buffers
			VertexBuffer** currentVertexBuffers = mVertexBuffers;
			const Renderer::VertexArrayVertexBuffer* vertexBuffersEnd = vertexBuffers + numberOfVertexBuffers;
			for (const Renderer::VertexArrayVertexBuffer* vertexBuffer = vertexBuffers; vertexBuffer < vertexBuffersEnd; ++vertexBuffer, ++currentVertexBuffers)
			{
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*currentVertexBuffers = static_cast<VertexBuffer*>(vertexBuffer->vertexBuffer);
				(*currentVertexBuffers)->addReference();
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
