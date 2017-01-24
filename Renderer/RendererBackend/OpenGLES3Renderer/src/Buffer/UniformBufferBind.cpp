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
#include "OpenGLES3Renderer/Buffer/UniformBufferBind.h"
#include "OpenGLES3Renderer/ExtensionsRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	UniformBufferBind::UniformBufferBind(OpenGLES3Renderer &openGLES3Renderer, uint32_t numberOfBytes, const void *data, Renderer::BufferUsage bufferUsage) :
		UniformBuffer(openGLES3Renderer)
	{
		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL ES uniform buffer
			GLint openGLESUniformBufferBackup = 0;
			glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &openGLESUniformBufferBackup);
		#endif

		// TODO(co) Review OpenGL uniform buffer alignment topic

		// Bind this OpenGL uniform buffer and upload the data
		glBindBuffer(GL_UNIFORM_BUFFER, mOpenGLESUniformBuffer);
		// -> Usage: These constants directly map to GL_EXT_vertex_buffer_object and OpenGL ES 2 constants, do not change them
		glBufferData(GL_UNIFORM_BUFFER, static_cast<GLsizeiptr>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES uniform buffer
			glBindBuffer(GL_UNIFORM_BUFFER, static_cast<GLuint>(openGLESUniformBufferBackup));
		#endif
	}

	UniformBufferBind::~UniformBufferBind()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IUniformBuffer methods       ]
	//[-------------------------------------------------------]
	void UniformBufferBind::copyDataFrom(uint32_t numberOfBytes, const void *data)
	{
		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL uniform buffer
			GLint openGLESUniformBufferBackup = 0;
			glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &openGLESUniformBufferBackup);
		#endif

		// Bind this OpenGL uniform buffer and upload the data
		// -> Subdata is quite optimized for uniform buffers, see http://on-demand.gputechconf.com/siggraph/2014/presentation/SG4117-OpenGL-Scene-Rendering-Techniques.pdf
		glBindBuffer(GL_UNIFORM_BUFFER, mOpenGLESUniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, static_cast<GLsizeiptr>(numberOfBytes), data);

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL uniform buffer
			glBindBuffer(GL_UNIFORM_BUFFER, static_cast<GLuint>(openGLESUniformBufferBackup));
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
