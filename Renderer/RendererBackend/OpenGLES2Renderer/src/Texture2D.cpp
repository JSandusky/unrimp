/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "OpenGLES2Renderer/Texture2D.h"
#include "OpenGLES2Renderer/Mapping.h"
#include "OpenGLES2Renderer/IContext.h"	// We need to include this header, else the linker won't find our defined OpenGL ES 2 functions
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture2D::Texture2D(OpenGLES2Renderer &openGLES2Renderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags) :
		ITexture2D(openGLES2Renderer, width, height),
		mOpenGLES2Texture(0)
	{
		// TODO(co) Check support formats

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently set alignment
			GLint openGLES2AlignmentBackup = 0;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &openGLES2AlignmentBackup);

			// Backup the currently bound OpenGL ES 2 texture
			GLint openGLES2TextureBackup = 0;
			glGetIntegerv(GL_TEXTURE_2D, &openGLES2TextureBackup);
		#endif

		// Set correct alignment
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Create the OpenGL ES 2 texture instance
		glGenTextures(1, &mOpenGLES2Texture);
		glBindTexture(GL_TEXTURE_2D, mOpenGLES2Texture);

		// Upload the base map of the texture (mipmaps are automatically created as soon as the base map is changed)
		glTexImage2D(GL_TEXTURE_2D, 0, Mapping::getOpenGLES2InternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, Mapping::getOpenGLES2Format(textureFormat), Mapping::getOpenGLES2Type(textureFormat), data);

		// Build mipmaps automatically on the GPU?
		if (flags & Renderer::TextureFlag::MIPMAPS)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 2 texture
			glBindTexture(GL_TEXTURE_2D, openGLES2TextureBackup);

			// Restore previous alignment
			glPixelStorei(GL_UNPACK_ALIGNMENT, openGLES2AlignmentBackup);
		#endif
	}

	Texture2D::~Texture2D()
	{
		// Destroy the OpenGL ES 2 texture instance
		// -> Silently ignores 0's and names that do not correspond to existing textures
		glDeleteTextures(1, &mOpenGLES2Texture);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
