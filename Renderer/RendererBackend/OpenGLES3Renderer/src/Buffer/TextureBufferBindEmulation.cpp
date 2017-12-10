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
#include "OpenGLES3Renderer/Buffer/TextureBufferBindEmulation.h"
#include "OpenGLES3Renderer/ExtensionsRuntimeLinking.h"
#include "OpenGLES3Renderer/Mapping.h"

#include <Renderer/IAssert.h>
#include <Renderer/IRenderer.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureBufferBindEmulation::TextureBufferBindEmulation(OpenGLES3Renderer& openGLES3Renderer, uint32_t numberOfBytes, Renderer::TextureFormat::Enum, const void* data, Renderer::BufferUsage bufferUsage) :
		TextureBuffer(openGLES3Renderer, numberOfBytes)
	{
		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL ES 3 uniform buffer
			GLint openGLES3UniformBufferBackup = 0;
			glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &openGLES3UniformBufferBackup);
		#endif

		// TODO(co) Review OpenGL ES 3 uniform buffer alignment topic

		// Bind this OpenGL ES 3 uniform buffer and upload the data
		glBindBuffer(GL_UNIFORM_BUFFER, mOpenGLES3TextureBuffer);
		// -> Usage: These constants directly map to GL_EXT_vertex_buffer_object and OpenGL ES 3 constants, do not change them
		glBufferData(GL_UNIFORM_BUFFER, static_cast<GLsizeiptr>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 3 uniform buffer
			glBindBuffer(GL_UNIFORM_BUFFER, static_cast<GLuint>(openGLES3UniformBufferBackup));
		#endif
	}

	TextureBufferBindEmulation::~TextureBufferBindEmulation()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureBuffer methods       ]
	//[-------------------------------------------------------]
	void TextureBufferBindEmulation::copyDataFrom(uint32_t numberOfBytes, const void* data)
	{
		// Sanity check
		RENDERER_ASSERT(getRenderer().getContext(), nullptr != data, "Invalid OpenGL ES 3 texture buffer data")

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL ES 3 uniform buffer
			GLint openGLES3UniformBufferBackup = 0;
			glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &openGLES3UniformBufferBackup);
		#endif

		// Bind this OpenGL ES 3 uniform buffer and upload the data
		// -> Subdata is quite optimized for uniform buffers, see http://on-demand.gputechconf.com/siggraph/2014/presentation/SG4117-OpenGL-Scene-Rendering-Techniques.pdf
		glBindBuffer(GL_UNIFORM_BUFFER, mOpenGLES3TextureBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, static_cast<GLsizeiptr>(numberOfBytes), data);

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 3 uniform buffer
			glBindBuffer(GL_UNIFORM_BUFFER, static_cast<GLuint>(openGLES3UniformBufferBackup));
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
