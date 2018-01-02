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
#include "OpenGLRenderer/Buffer/IndexBufferBind.h"
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
	IndexBufferBind::IndexBufferBind(OpenGLRenderer& openGLRenderer, uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void* data, Renderer::BufferUsage bufferUsage) :
		IndexBuffer(openGLRenderer, indexBufferFormat)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL element array buffer
			GLint openGLElementArrayBufferBackup = 0;
			glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB, &openGLElementArrayBufferBackup);
		#endif

		// Create the OpenGL element array buffer
		glGenBuffersARB(1, &mOpenGLElementArrayBuffer);

		// Bind this OpenGL element array buffer and upload the data
		// -> Usage: These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 3 constants, do not change them
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mOpenGLElementArrayBuffer);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, static_cast<GLsizeiptrARB>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL element array buffer
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, static_cast<GLuint>(openGLElementArrayBufferBackup));
		#endif
	}

	IndexBufferBind::~IndexBufferBind()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
