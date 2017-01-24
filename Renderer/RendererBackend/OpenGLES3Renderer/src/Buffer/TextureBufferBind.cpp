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
#include "OpenGLES3Renderer/Buffer/TextureBufferBind.h"
#include "OpenGLES3Renderer/Mapping.h"
#include "OpenGLES3Renderer/ExtensionsRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureBufferBind::TextureBufferBind(OpenGLES3Renderer &openGLES3Renderer, uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data, Renderer::BufferUsage bufferUsage) :
		TextureBuffer(openGLES3Renderer)
	{
		{ // Buffer part
			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Backup the currently bound OpenGL texture buffer
				GLint openGLESTextureBufferBackup = 0;
				glGetIntegerv(GL_TEXTURE_BINDING_BUFFER_EXT, &openGLESTextureBufferBackup);
			#endif

			// Bind this OpenGL texture buffer and upload the data
			glBindBuffer(GL_TEXTURE_BUFFER_EXT, mOpenGLESTextureBuffer);
			// -> Usage: These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 3 constants, do not change them
			glBufferData(GL_TEXTURE_BUFFER_EXT, static_cast<GLsizeiptr>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));

			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous bound OpenGL texture buffer
				glBindBuffer(GL_TEXTURE_BUFFER_ARB, static_cast<GLuint>(openGLESTextureBufferBackup));
			#endif
		}

		{ // Texture part
			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Backup the currently bound OpenGL texture
				GLint openGLESTextureBackup = 0;
				glGetIntegerv(GL_TEXTURE_BUFFER_BINDING_EXT, &openGLESTextureBackup);
			#endif

			// Make this OpenGL texture instance to the currently used one
			glBindTexture(GL_TEXTURE_BUFFER_EXT, mOpenGLESTexture);

			// Attaches the storage for the buffer object to the active buffer texture
			glTexBufferEXT(GL_TEXTURE_BUFFER_EXT, Mapping::getOpenGLES3InternalFormat(textureFormat), mOpenGLESTextureBuffer);

			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous bound OpenGL texture
				glBindTexture(GL_TEXTURE_BUFFER_EXT, static_cast<GLuint>(openGLESTextureBackup));
			#endif
		}
	}

	TextureBufferBind::~TextureBufferBind()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureBuffer methods       ]
	//[-------------------------------------------------------]
	void TextureBufferBind::copyDataFrom(uint32_t numberOfBytes, const void *data)
	{
		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGLES texture buffer
			GLint openGLESTextureBufferBackup = 0;
			glGetIntegerv(GL_TEXTURE_BINDING_BUFFER_EXT, &openGLESTextureBufferBackup);
		#endif

		// Bind this OpenGL texture buffer and upload the data
		glBindBuffer(GL_TEXTURE_BUFFER_EXT, mOpenGLESTextureBuffer);
		glBufferSubData(GL_TEXTURE_BUFFER_EXT, 0, static_cast<GLsizeiptr>(numberOfBytes), data);

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL texture buffer
			glBindBuffer(GL_TEXTURE_BUFFER_EXT, static_cast<GLuint>(openGLESTextureBufferBackup));
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
