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
#include "OpenGLRenderer/Buffer/TextureBufferDsa.h"
#include "OpenGLRenderer/Mapping.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"
#include "OpenGLRenderer/OpenGLRenderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureBufferDsa::TextureBufferDsa(OpenGLRenderer &openGLRenderer, uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data, Renderer::BufferUsage bufferUsage) :
		TextureBuffer(openGLRenderer)
	{
		if(openGLRenderer.getExtensions().isGL_ARB_direct_state_access())
		{
			{ // Buffer part
				{ // For ARB DSA version the buffer object must be initialized.
					// TODO(sw) The base class uses glGenBuffersARB to create only the name for it, but the glNamedBufferData methods expects an initialized object
					// In OpenGL 4.5 there exists glCreateBuffers which also initializes the object. But we want support OpenGL 4.1 where the glCreateBuffers method doesn't exits
					// Backup the currently bound OpenGL array buffer
					GLint openGLBufferBackup = 0;
					glGetIntegerv(GL_TEXTURE_BINDING_BUFFER_ARB, &openGLBufferBackup);

					// Initialize our buffer
					glBindBufferARB(GL_TEXTURE_BUFFER_ARB, mOpenGLTextureBuffer);

					// Restore old binding because we needed the bind only to initialize the buffer object
					glBindBufferARB(GL_TEXTURE_BUFFER_ARB, static_cast<GLuint>(openGLBufferBackup));
				}
				// Upload the data
				// -> Usage: These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 2 constants, do not change them
				glNamedBufferData(mOpenGLTextureBuffer, static_cast<GLsizeiptr>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));
			}

			{ // Texture part
				// Attaches the storage for the buffer object to the buffer texture
				glTextureBuffer(mOpenGLTexture, Mapping::getOpenGLInternalFormat(textureFormat), mOpenGLTextureBuffer);
			}
		}
		else
		{
			{ // Buffer part
				// Upload the data
				// -> Usage: These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 2 constants, do not change them
				glNamedBufferDataEXT(mOpenGLTextureBuffer, static_cast<GLsizeiptr>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));
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
				// -> Sadly, there's no direct state access (DSA) function defined for this in "GL_EXT_direct_state_access"
				glTexBufferARB(GL_TEXTURE_BUFFER_ARB, Mapping::getOpenGLInternalFormat(textureFormat), mOpenGLTextureBuffer);

				#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL texture
					glBindTexture(GL_TEXTURE_BUFFER_ARB, static_cast<GLuint>(openGLTextureBackup));
				#endif
			}
		}
	}

	TextureBufferDsa::~TextureBufferDsa()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureBuffer methods       ]
	//[-------------------------------------------------------]
	void TextureBufferDsa::copyDataFrom(uint32_t numberOfBytes, const void *data)
	{
		if (static_cast<OpenGLRenderer&>(getRenderer()).getExtensions().isGL_ARB_direct_state_access())
		{
			// Upload the data
			glNamedBufferSubData(mOpenGLTextureBuffer, 0, static_cast<GLsizeiptr>(numberOfBytes), data);
		}
		else
		{
			// Upload the data
			glNamedBufferSubDataEXT(mOpenGLTextureBuffer, 0, static_cast<GLsizeiptr>(numberOfBytes), data);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
