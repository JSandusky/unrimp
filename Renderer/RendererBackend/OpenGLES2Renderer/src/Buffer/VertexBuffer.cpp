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
#include "OpenGLES2Renderer/Buffer/VertexBuffer.h"
#include "OpenGLES2Renderer/Mapping.h"
#include "OpenGLES2Renderer/IContext.h"	// We need to include this header, else the linker won't find our defined OpenGL ES 2 functions
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexBuffer::VertexBuffer(OpenGLES2Renderer &openGLES2Renderer, uint32_t numberOfBytes, const void *data, Renderer::BufferUsage bufferUsage) :
		IVertexBuffer(openGLES2Renderer),
		mOpenGLES2ArrayBuffer(0),
		mBufferSize(numberOfBytes)
	{
		// Create the OpenGL ES 2 array buffer
		glGenBuffers(1, &mOpenGLES2ArrayBuffer);

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL ES 2 array buffer
			GLint openGLES2ArrayBufferBackup = 0;
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &openGLES2ArrayBufferBackup);
		#endif

		// Bind this OpenGL ES 2 array buffer and upload the data
		glBindBuffer(GL_ARRAY_BUFFER, mOpenGLES2ArrayBuffer);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(numberOfBytes), data, Mapping::getOpenGLES2Type(bufferUsage));

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 2 array buffer
			glBindBuffer(GL_ARRAY_BUFFER, openGLES2ArrayBufferBackup);
		#endif
	}

	VertexBuffer::~VertexBuffer()
	{
		// Destroy the OpenGL ES 2 array buffer
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteBuffers(1, &mOpenGLES2ArrayBuffer);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
