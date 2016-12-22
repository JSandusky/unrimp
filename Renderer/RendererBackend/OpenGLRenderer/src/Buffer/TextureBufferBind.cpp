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
#include "OpenGLRenderer/Buffer/TextureBufferBind.h"
#include "OpenGLRenderer/Mapping.h"
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
	TextureBufferBind::TextureBufferBind(OpenGLRenderer &openGLRenderer, uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data, Renderer::BufferUsage bufferUsage) :
		TextureBuffer(openGLRenderer)
	{
		{ // Buffer part
			#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
				// Backup the currently bound OpenGL texture buffer
				GLint openGLTextureBufferBackup = 0;
				glGetIntegerv(GL_TEXTURE_BINDING_BUFFER_ARB, &openGLTextureBufferBackup);
			#endif

			// Bind this OpenGL texture buffer and upload the data
			glBindBufferARB(GL_TEXTURE_BUFFER_ARB, mOpenGLTextureBuffer);
			// -> Usage: These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 2 constants, do not change them
			glBufferDataARB(GL_TEXTURE_BUFFER_ARB, static_cast<GLsizeiptrARB>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));

			#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous bound OpenGL texture buffer
				glBindBufferARB(GL_TEXTURE_BUFFER_ARB, openGLTextureBufferBackup);
			#endif
		}

		{ // Texture part
			#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
				// Backup the currently bound OpenGL texture
				GLint openGLTextureBackup = 0;
				glGetIntegerv(GL_TEXTURE_BINDING_BUFFER_ARB, &openGLTextureBackup);
			#endif

			// Make this OpenGL texture instance to the currently used one
			glBindTexture(GL_TEXTURE_BUFFER_ARB, mOpenGLTexture);

			// Attaches the storage for the buffer object to the active buffer texture
			glTexBufferARB(GL_TEXTURE_BUFFER_ARB, Mapping::getOpenGLInternalFormat(textureFormat), mOpenGLTextureBuffer);

			#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous bound OpenGL texture
				glBindTexture(GL_TEXTURE_BUFFER_ARB, openGLTextureBackup);
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
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently bound OpenGL texture buffer
			GLint openGLTextureBufferBackup = 0;
			glGetIntegerv(GL_TEXTURE_BINDING_BUFFER_ARB, &openGLTextureBufferBackup);
		#endif

		// Bind this OpenGL texture buffer and upload the data
		glBindBufferARB(GL_TEXTURE_BUFFER_ARB, mOpenGLTextureBuffer);
		glBufferSubDataARB(GL_TEXTURE_BUFFER_ARB, 0, static_cast<GLsizeiptrARB>(numberOfBytes), data);

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL texture buffer
			glBindBufferARB(GL_TEXTURE_BUFFER_ARB, openGLTextureBufferBackup);
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
