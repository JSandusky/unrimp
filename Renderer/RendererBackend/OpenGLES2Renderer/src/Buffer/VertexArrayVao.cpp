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
#include "OpenGLES2Renderer/Buffer/VertexArrayVao.h"
#include "OpenGLES2Renderer/Buffer/IndexBuffer.h"
#include "OpenGLES2Renderer/Buffer/VertexBuffer.h"
#include "OpenGLES2Renderer/Mapping.h"
#include "OpenGLES2Renderer/IExtensions.h"	// We need to include this in here for the definitions of the OpenGL ES 2 functions
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArrayVao::VertexArrayVao(OpenGLES2Renderer &openGLES2Renderer, const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, IndexBuffer *indexBuffer) :
		VertexArray(openGLES2Renderer, indexBuffer),
		mOpenGLES2VertexArray(0),
		mNumberOfVertexBuffers(numberOfVertexBuffers),
		mVertexBuffers((mNumberOfVertexBuffers > 0) ? new VertexBuffer*[mNumberOfVertexBuffers] : nullptr)	// Guaranteed to be filled below, so we don't need to care to initialize the content in here
	{
		// Create the OpenGL ES 2 vertex array
		glGenVertexArraysOES(1, &mOpenGLES2VertexArray);

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL ES 2 array buffer
			GLint openGLES2ArrayBufferBackup = 0;
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &openGLES2ArrayBufferBackup);

			// Backup the currently bound OpenGL ES 2 element array buffer
			GLint openGLES2ElementArrayBufferBackup = 0;
			glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &openGLES2ElementArrayBufferBackup);

			// Backup the currently bound OpenGL ES 2 vertex array
			GLint openGLES2VertexArrayBackup = 0;
			glGetIntegerv(GL_VERTEX_ARRAY_BINDING_OES, &openGLES2VertexArrayBackup);
		#endif

		// Bind this OpenGL ES 2 vertex array
		glBindVertexArrayOES(mOpenGLES2VertexArray);

		{ // Add a reference to the used vertex buffers
			VertexBuffer **currentVertexBuffers = mVertexBuffers;
			const Renderer::VertexArrayVertexBuffer *vertexBufferEnd = vertexBuffers + mNumberOfVertexBuffers;
			for (const Renderer::VertexArrayVertexBuffer *vertexBuffer = vertexBuffers; vertexBuffer < vertexBufferEnd; ++vertexBuffer, ++currentVertexBuffers)
			{
				// Add a reference to the used vertex buffer
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*currentVertexBuffers = static_cast<VertexBuffer*>(vertexBuffer->vertexBuffer);
				(*currentVertexBuffers)->addReference();
			}
		}

		{ // Enable OpenGL ES 2 vertex attribute arrays
			// Loop through all attributes
			// -> We're using "glBindAttribLocation()" when linking the program so we have known attribute locations (the vertex array can't know about the program)
			GLuint attributeLocation = 0;
			const Renderer::VertexAttribute *attributeEnd = vertexAttributes.attributes + vertexAttributes.numberOfAttributes;
			for (const Renderer::VertexAttribute *attribute = vertexAttributes.attributes; attribute < attributeEnd; ++attribute, ++attributeLocation)
			{
				// Set the OpenGL ES 2 vertex attribute pointer
				const Renderer::VertexArrayVertexBuffer& vertexArrayVertexBuffer = vertexBuffers[attribute->inputSlot];
				glBindBuffer(GL_ARRAY_BUFFER, static_cast<VertexBuffer*>(vertexArrayVertexBuffer.vertexBuffer)->getOpenGLES2ArrayBuffer());
				glVertexAttribPointer(attributeLocation,
									  Mapping::getOpenGLES2Size(attribute->vertexAttributeFormat),
									  Mapping::getOpenGLES2Type(attribute->vertexAttributeFormat),
									  static_cast<GLboolean>(Mapping::isOpenGLES2VertexAttributeFormatNormalized(attribute->vertexAttributeFormat)),
									  static_cast<GLsizei>(vertexArrayVertexBuffer.strideInBytes),
									  reinterpret_cast<GLvoid*>(attribute->alignedByteOffset));

				// Enable OpenGL ES 2 vertex attribute array
				glEnableVertexAttribArray(attributeLocation);
			}

			// Get the used index buffer
			// -> In case of no index buffer we don't bind buffer 0, there's not really a point in it
			if (nullptr != indexBuffer)
			{
				// Bind OpenGL ES 2 element array buffer
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getOpenGLES2ElementArrayBuffer());
			}
		}

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 2 vertex array
			glBindVertexArrayOES(static_cast<GLuint>(openGLES2VertexArrayBackup));

			// Be polite and restore the previous bound OpenGL ES 2 element array buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(openGLES2ElementArrayBufferBackup));

			// Be polite and restore the previous bound OpenGL ES 2 array buffer
			glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(openGLES2ArrayBufferBackup));
		#endif
	}

	VertexArrayVao::~VertexArrayVao()
	{
		// Destroy the OpenGL ES 2 vertex array
		// -> Silently ignores 0's and names that do not correspond to existing vertex array objects
		glDeleteVertexArraysOES(1, &mOpenGLES2VertexArray);

		// Release the reference to the used vertex buffers
		if (nullptr != mVertexBuffers)
		{
			// Release references
			VertexBuffer **vertexBuffersEnd = mVertexBuffers + mNumberOfVertexBuffers;
			for (VertexBuffer **vertexBuffer = mVertexBuffers; vertexBuffer < vertexBuffersEnd; ++vertexBuffer)
			{
				(*vertexBuffer)->releaseReference();
			}

			// Cleanup
			delete [] mVertexBuffers;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
