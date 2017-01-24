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
#include "OpenGLES3Renderer/Texture/Texture2DArray.h"
#include "OpenGLES3Renderer/Mapping.h"
#include "OpenGLES3Renderer/IExtensions.h"
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture2DArray::Texture2DArray(OpenGLES3Renderer &openGLES3Renderer, uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags) :
		ITexture2DArray(openGLES3Renderer, width, height, numberOfSlices),
		mOpenGLES3Texture(0)
	{
		// TODO(co) Check support formats

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Backup the currently set alignment
			GLint openGLES2AlignmentBackup = 0;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &openGLES2AlignmentBackup);

			// Backup the currently bound OpenGL ES 3 texture
			GLint openGLES2TextureBackup = 0;
			glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY_EXT, &openGLES2TextureBackup);
		#endif

		// Set correct alignment
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Create the OpenGL ES 3 texture instance
		glGenTextures(1, &mOpenGLES3Texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, mOpenGLES3Texture);

		// Upload the base map of the texture (mipmaps are automatically created as soon as the base map is changed)
		glTexImage3DOES(GL_TEXTURE_2D_ARRAY_EXT, 0, Mapping::getOpenGLES3InternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(numberOfSlices), 0, Mapping::getOpenGLES3Format(textureFormat), Mapping::getOpenGLES3Type(textureFormat), data);

		// Build mipmaps automatically on the GPU? (or GPU driver)
		if (flags & Renderer::TextureFlag::GENERATE_MIPMAPS)
		{
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY_EXT);
			glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 3 texture
			glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, openGLES2TextureBackup);

			// Restore previous alignment
			glPixelStorei(GL_UNPACK_ALIGNMENT, openGLES2AlignmentBackup);
		#endif
	}

	Texture2DArray::~Texture2DArray()
	{
		// Destroy the OpenGL ES 3 texture instance
		// -> Silently ignores 0's and names that do not correspond to existing textures
		glDeleteTextures(1, &mOpenGLES3Texture);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
