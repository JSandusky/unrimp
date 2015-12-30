/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "OpenGLRenderer/Shader/UniformBufferGlslBind.h"
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
	UniformBufferGlslBind::UniformBufferGlslBind(OpenGLRenderer &openGLRenderer, uint32_t numberOfBytes, const void *data, Renderer::BufferUsage bufferUsage) :
		UniformBufferGlsl(openGLRenderer)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL uniform buffer
			GLint openGLUniformBufferBackup = 0;
			glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &openGLUniformBufferBackup);
		#endif

		// TODO(co) Review OpenGL uniform buffer alignment topic

		// Bind this OpenGL uniform buffer and upload the data
		glBindBufferARB(GL_UNIFORM_BUFFER, mOpenGLUniformBuffer);
		// -> Usage: These constants directly map to GL_ARB_vertex_buffer_object and OpenGL ES 2 constants, do not change them
		glBufferDataARB(GL_UNIFORM_BUFFER, static_cast<GLsizeiptrARB>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL uniform buffer
			glBindBufferARB(GL_UNIFORM_BUFFER, openGLUniformBufferBackup);
		#endif
	}

	UniformBufferGlslBind::~UniformBufferGlslBind()
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IUniformBuffer methods       ]
	//[-------------------------------------------------------]
	void UniformBufferGlslBind::copyDataFrom(uint32_t numberOfBytes, const void *data)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL uniform buffer
			GLint openGLUniformBufferBackup = 0;
			glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &openGLUniformBufferBackup);
		#endif

		// Bind this OpenGL uniform buffer and upload the data
		glBindBufferARB(GL_UNIFORM_BUFFER, mOpenGLUniformBuffer);
		glBufferSubDataARB(GL_UNIFORM_BUFFER, 0, static_cast<GLsizeiptrARB>(numberOfBytes), data);

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL uniform buffer
			glBindBufferARB(GL_UNIFORM_BUFFER, openGLUniformBufferBackup);
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
