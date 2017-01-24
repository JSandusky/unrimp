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
#include "OpenGLES3Renderer/Buffer/VertexBuffer.h"
#include "OpenGLES3Renderer/Mapping.h"
#include "OpenGLES3Renderer/IContext.h"	// We need to include this header, else the linker won't find our defined OpenGL ES 3 functions
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexBuffer::VertexBuffer(OpenGLES3Renderer &openGLES3Renderer, uint32_t numberOfBytes, const void *data, Renderer::BufferUsage bufferUsage) :
		IVertexBuffer(openGLES3Renderer),
		mOpenGLES3ArrayBuffer(0),
		mBufferSize(numberOfBytes)
	{
		// Create the OpenGL ES 3 array buffer
		glGenBuffers(1, &mOpenGLES3ArrayBuffer);

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL ES 3 array buffer
			GLint openGLES3ArrayBufferBackup = 0;
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &openGLES3ArrayBufferBackup);
		#endif

		// Bind this OpenGL ES 3 array buffer and upload the data
		glBindBuffer(GL_ARRAY_BUFFER, mOpenGLES3ArrayBuffer);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(numberOfBytes), data, Mapping::getOpenGLES3Type(bufferUsage));

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 3 array buffer
			glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(openGLES3ArrayBufferBackup));
		#endif
	}

	VertexBuffer::~VertexBuffer()
	{
		// Destroy the OpenGL ES 3 array buffer
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteBuffers(1, &mOpenGLES3ArrayBuffer);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
