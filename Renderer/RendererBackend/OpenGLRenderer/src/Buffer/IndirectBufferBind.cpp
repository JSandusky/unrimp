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
#include "OpenGLRenderer/Buffer/IndirectBufferBind.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"

#include <Renderer/IAssert.h>
#include <Renderer/IRenderer.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IndirectBufferBind::IndirectBufferBind(OpenGLRenderer& openGLRenderer, uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage) :
		IndirectBuffer(openGLRenderer)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL indirect buffer
			GLint openGLIndirectBufferBackup = 0;
			glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING, &openGLIndirectBufferBackup);
		#endif

		// Create the OpenGL indirect buffer
		glGenBuffersARB(1, &mOpenGLIndirectBuffer);

		// Bind this OpenGL indirect buffer and upload the data
		// -> Usage: These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 3 constants, do not change them
		glBindBufferARB(GL_DRAW_INDIRECT_BUFFER, mOpenGLIndirectBuffer);
		glBufferDataARB(GL_DRAW_INDIRECT_BUFFER, static_cast<GLsizeiptrARB>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL indirect buffer
			glBindBufferARB(GL_DRAW_INDIRECT_BUFFER, static_cast<GLuint>(openGLIndirectBufferBackup));
		#endif
	}

	IndirectBufferBind::~IndirectBufferBind()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IIndirectBuffer methods      ]
	//[-------------------------------------------------------]
	void IndirectBufferBind::copyDataFrom(uint32_t numberOfBytes, const void* data)
	{
		// Sanity check
		RENDERER_ASSERT(getRenderer().getContext(), nullptr != data, "Invalid OpenGL indirect buffer data")

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL indirect buffer
			GLint openGLIndirectBufferBackup = 0;
			glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING, &openGLIndirectBufferBackup);
		#endif

		// Bind this OpenGL indirect buffer and upload the data
		glBindBufferARB(GL_DRAW_INDIRECT_BUFFER, mOpenGLIndirectBuffer);
		glBufferSubDataARB(GL_DRAW_INDIRECT_BUFFER, 0, static_cast<GLsizeiptrARB>(numberOfBytes), data);

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL indirect buffer
			glBindBufferARB(GL_DRAW_INDIRECT_BUFFER, static_cast<GLuint>(openGLIndirectBufferBackup));
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
