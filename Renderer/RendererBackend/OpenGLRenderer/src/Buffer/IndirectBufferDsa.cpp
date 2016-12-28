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
#include "OpenGLRenderer/Buffer/IndirectBufferDsa.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IndirectBufferDsa::IndirectBufferDsa(OpenGLRenderer &openGLRenderer, uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage) :
		IndirectBuffer(openGLRenderer)
	{
		if(openGLRenderer.getExtensions().isGL_ARB_direct_state_access())
		{
			{ // For ARB DSA version the buffer object must be initialized.
				// TODO(sw) The base class uses glGenBuffersARB to create only the name for it, but the glNamedBufferData methods expects an initialized object
				// In OpenGL 4.5 there exists glCreateBuffers which also initializes the object. But we want support OpenGL 4.1 where the glCreateBuffers method doesn't exits
				// Backup the currently bound OpenGL array buffer
				GLint openGLBufferBackup = 0;
				glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING, &openGLBufferBackup);

				// Initialize our buffer
				glBindBufferARB(GL_DRAW_INDIRECT_BUFFER, mOpenGLIndirectBuffer);

				// Restore old binding because we needed the bind only to initialize the buffer object
				glBindBufferARB(GL_DRAW_INDIRECT_BUFFER, static_cast<GLuint>(openGLBufferBackup));
			}

			// Upload the data
			// -> Usage: These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 2 constants, do not change them
			glNamedBufferData(mOpenGLIndirectBuffer, static_cast<GLsizeiptr>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));
		}
		else
		{
			// Upload the data
			// -> Usage: These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 2 constants, do not change them
			glNamedBufferDataEXT(mOpenGLIndirectBuffer, static_cast<GLsizeiptr>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));
		}
	}

	IndirectBufferDsa::~IndirectBufferDsa()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IIndirectBuffer methods      ]
	//[-------------------------------------------------------]
	void IndirectBufferDsa::copyDataFrom(uint32_t numberOfBytes, const void *data)
	{
		if (static_cast<OpenGLRenderer&>(getRenderer()).getExtensions().isGL_ARB_direct_state_access())
		{
			// Upload the data
			glNamedBufferSubData(mOpenGLIndirectBuffer, 0, static_cast<GLsizeiptr>(numberOfBytes), data);
		}
		else
		{
			// Upload the data
			glNamedBufferSubDataEXT(mOpenGLIndirectBuffer, 0, static_cast<GLsizeiptr>(numberOfBytes), data);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
