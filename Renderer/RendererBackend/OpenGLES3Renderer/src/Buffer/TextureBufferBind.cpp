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
#include "OpenGLES3Renderer/Buffer/TextureBufferBind.h"
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
	TextureBufferBind::TextureBufferBind(OpenGLES3Renderer& openGLES3Renderer, uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void* data, Renderer::BufferUsage bufferUsage) :
		TextureBuffer(openGLES3Renderer, numberOfBytes)
	{
		{ // Buffer part
			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Backup the currently bound OpenGL ES 3 texture buffer
				GLint openGLES3TextureBufferBackup = 0;
				glGetIntegerv(GL_TEXTURE_BINDING_BUFFER_EXT, &openGLES3TextureBufferBackup);
			#endif

			// Bind this OpenGL ES 3 texture buffer and upload the data
			glBindBuffer(GL_TEXTURE_BUFFER_EXT, mOpenGLES3TextureBuffer);
			// -> Usage: These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 3 constants, do not change them
			glBufferData(GL_TEXTURE_BUFFER_EXT, static_cast<GLsizeiptr>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));

			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous bound OpenGL ES 3 texture buffer
				glBindBuffer(GL_TEXTURE_BUFFER_EXT, static_cast<GLuint>(openGLES3TextureBufferBackup));
			#endif
		}

		{ // Texture part
			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Backup the currently bound OpenGL ES 3 texture
				GLint openGLESTextureBackup = 0;
				glGetIntegerv(GL_TEXTURE_BUFFER_BINDING_EXT, &openGLESTextureBackup);
			#endif

			// Make this OpenGL ES 3 texture instance to the currently used one
			glBindTexture(GL_TEXTURE_BUFFER_EXT, mOpenGLES3Texture);

			// Attaches the storage for the buffer object to the active buffer texture
			glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, Mapping::getOpenGLES3InternalFormat(textureFormat), mOpenGLES3TextureBuffer);

			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous bound OpenGL ES 3 texture
				glBindTexture(GL_TEXTURE_BUFFER_EXT, static_cast<GLuint>(openGLESTextureBackup));
			#endif
		}
	}

	TextureBufferBind::~TextureBufferBind()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
