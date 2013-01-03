/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLES2Renderer/Texture2DArray.h"
#include "OpenGLES2Renderer/Mapping.h"
#include "OpenGLES2Renderer/IExtensions.h"
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	Texture2DArray::Texture2DArray(OpenGLES2Renderer &openGLES2Renderer, unsigned int width, unsigned int height, unsigned int numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data, unsigned int flags) :
		ITexture2DArray(openGLES2Renderer, width, height, numberOfSlices),
		mOpenGLES2Texture(0)
	{
		// TODO(co) Check support formats

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Backup the currently set alignment
			GLint openGLES2AlignmentBackup = 0;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &openGLES2AlignmentBackup);

			// Backup the currently bound OpenGL ES 2 texture
			GLint openGLES2TextureBackup = 0;
			glGetIntegerv(GL_TEXTURE_2D_ARRAY_EXT, &openGLES2TextureBackup);
		#endif

		// Set correct alignment
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Create the OpenGL ES 2 texture instance
		glGenTextures(1, &mOpenGLES2Texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, mOpenGLES2Texture);

		// Upload the base map of the texture (mipmaps are automatically created as soon as the base map is changed)
		glTexImage3DOES(GL_TEXTURE_2D_ARRAY_EXT, 0, Mapping::getOpenGLES2InternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(numberOfSlices), 0, Mapping::getOpenGLES2Format(textureFormat), Mapping::getOpenGLES2Type(textureFormat), data);

		// Build mipmaps automatically on the GPU?
		if (flags & Renderer::TextureFlag::MIPMAPS)
		{
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY_EXT);
			glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 2 texture
			glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, openGLES2TextureBackup);

			// Restore previous alignment
			glPixelStorei(GL_UNPACK_ALIGNMENT, openGLES2AlignmentBackup);
		#endif
	}

	/**
	*  @brief
	*    Destructor
	*/
	Texture2DArray::~Texture2DArray()
	{
		// Destroy the OpenGL ES 2 texture instance
		// -> Silently ignores 0's and names that do not correspond to existing textures
		glDeleteTextures(1, &mOpenGLES2Texture);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
