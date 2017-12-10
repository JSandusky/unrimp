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
#include "OpenGLES3Renderer/Buffer/IndexBuffer.h"
#include "OpenGLES3Renderer/Mapping.h"
#include "OpenGLES3Renderer/IExtensions.h"
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"

#include <Renderer/IAssert.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IndexBuffer::IndexBuffer(OpenGLES3Renderer& openGLES3Renderer, uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void* data, Renderer::BufferUsage bufferUsage) :
		IIndexBuffer(openGLES3Renderer),
		mOpenGLES3ElementArrayBuffer(0),
		mOpenGLES3Type(GL_UNSIGNED_SHORT),
		mIndexSizeInBytes(Renderer::IndexBufferFormat::getNumberOfBytesPerElement(indexBufferFormat)),
		mBufferSize(numberOfBytes)
	{
		// "GL_UNSIGNED_INT" is only allowed when the "GL_OES_element_index_uint" extension is there
		if (Renderer::IndexBufferFormat::UNSIGNED_INT != indexBufferFormat || openGLES3Renderer.getOpenGLES3Context().getExtensions().isGL_OES_element_index_uint())
		{
			// Create the OpenGL ES 3 element array buffer
			glGenBuffers(1, &mOpenGLES3ElementArrayBuffer);

			// Set the OpenGL ES 3 index buffer data type
			mOpenGLES3Type = Mapping::getOpenGLES3Type(indexBufferFormat);

			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Backup the currently bound OpenGL ES 3 element array buffer
				GLint openGLES3ElementArrayBufferBackup = 0;
				glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &openGLES3ElementArrayBufferBackup);
			#endif

			// Bind this OpenGL ES 3 element array buffer and upload the data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mOpenGLES3ElementArrayBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(numberOfBytes), data, Mapping::getOpenGLES3Type(bufferUsage));

			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous bound OpenGL ES 3 element array buffer
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(openGLES3ElementArrayBufferBackup));
			#endif
		}
		else
		{
			// Error!
			RENDERER_ASSERT(openGLES3Renderer.getContext(), false, "\"GL_UNSIGNED_INT\" is only allowed in case the \"GL_OES_element_index_uint\" extension is there")
		}
	}

	IndexBuffer::~IndexBuffer()
	{
		// Destroy the OpenGL ES 3 element array buffer
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteBuffers(1, &mOpenGLES3ElementArrayBuffer);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
