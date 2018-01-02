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
#include "OpenGLRenderer/Buffer/VertexBufferBind.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexBufferBind::VertexBufferBind(OpenGLRenderer& openGLRenderer, uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage) :
		VertexBuffer(openGLRenderer)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL array buffer
			GLint openGLArrayBufferBackup = 0;
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING_ARB, &openGLArrayBufferBackup);
		#endif

		// Create the OpenGL array buffer
		glGenBuffersARB(1, &mOpenGLArrayBuffer);

		// Bind this OpenGL array buffer and upload the data
		// -> Usage: These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 3 constants, do not change them
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, mOpenGLArrayBuffer);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, static_cast<GLsizeiptrARB>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL array buffer
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, static_cast<GLuint>(openGLArrayBufferBackup));
		#endif
	}

	VertexBufferBind::~VertexBufferBind()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
