/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "OpenGLES2Renderer/VertexArrayVao.h"
#include "OpenGLES2Renderer/Mapping.h"
#include "OpenGLES2Renderer/Program.h"
#include "OpenGLES2Renderer/IExtensions.h"	// We need to include this in here for the definitions of the OpenGL ES 2 functions
#include "OpenGLES2Renderer/IndexBuffer.h"
#include "OpenGLES2Renderer/VertexBuffer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArrayVao::VertexArrayVao(Program &program, unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, IndexBuffer *indexBuffer) :
		VertexArray(program.getRenderer(), indexBuffer),
		mOpenGLES2VertexArray(0),
		mNumberOfVertexBuffers(numberOfAttributes),
		mVertexBuffers((numberOfAttributes > 0) ? new VertexBuffer*[numberOfAttributes] : nullptr)	// Guaranteed to be filled below, so we don't need to care to initialize the content in here
	{
		// Create the OpenGL ES 2 vertex array
		glGenVertexArraysOES(1, &mOpenGLES2VertexArray);

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL ES 2 array buffer
			GLint openGLES2ArrayBufferBackup = 0;
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &openGLES2ArrayBufferBackup);

			// Backup the currently bound OpenGL ES 2 element array buffer
			GLint openGLES2ElementArrayBufferBackup = 0;
			glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER, &openGLES2ElementArrayBufferBackup);

			// Backup the currently bound OpenGL ES 2 vertex array
			GLint openGLES2VertexArrayBackup = 0;
			glGetIntegerv(GL_VERTEX_ARRAY_BINDING_OES, &openGLES2VertexArrayBackup);
		#endif

		// Bind this OpenGL ES 2 vertex array
		glBindVertexArrayOES(mOpenGLES2VertexArray);

		{ // Enable OpenGL ES 2 vertex attribute arrays
			// Loop through all attributes
			VertexBuffer **vertexBuffers = mVertexBuffers;
			const Renderer::VertexArrayAttribute *attributeEnd = attributes + numberOfAttributes;
			for (const Renderer::VertexArrayAttribute *attribute = attributes; attribute < attributeEnd; ++attribute, ++vertexBuffers)
			{
				// Add a reference to the used vertex buffer
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*vertexBuffers = static_cast<VertexBuffer*>(attribute->vertexBuffer);
				(*vertexBuffers)->addReference();

				// Get the attribute location
				const int attributeLocation = program.getAttributeLocation(attribute->name);
				if (attributeLocation > -1)
				{
					// Set the OpenGL ES 2 vertex attribute pointer
					glBindBuffer(GL_ARRAY_BUFFER, (*vertexBuffers)->getOpenGLES2ArrayBuffer());
					glVertexAttribPointer(static_cast<GLuint>(attributeLocation), Mapping::getOpenGLES2Size(attribute->vertexArrayFormat), Mapping::getOpenGLES2Type(attribute->vertexArrayFormat), GL_FALSE, static_cast<GLsizei>(attribute->stride), reinterpret_cast<GLvoid*>(attribute->offset));

					// Enable OpenGL ES 2 vertex attribute array
					glEnableVertexAttribArray(static_cast<GLuint>(attributeLocation));
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL ES 2 warning: There's no active vertex attribute with the name \"%s\"\n", attribute->name)
				}
			}

			// Get the used index buffer
			// -> In case of no index buffer we don't bind buffer 0, there's not really a point in it
			const IndexBuffer *indexBuffer = getIndexBuffer();
			if (nullptr != indexBuffer)
			{
				// Bind OpenGL ES 2 element array buffer
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getOpenGLES2ElementArrayBuffer());
			}
		}

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 2 vertex array
			glBindVertexArrayOES(openGLES2VertexArrayBackup);

			// Be polite and restore the previous bound OpenGL ES 2 element array buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGLES2ElementArrayBufferBackup);

			// Be polite and restore the previous bound OpenGL ES 2 array buffer
			glBindBuffer(GL_ARRAY_BUFFER, openGLES2ArrayBufferBackup);
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
				(*vertexBuffer)->release();
			}

			// Cleanup
			delete [] mVertexBuffers;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
