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
#include "OpenGLES2Renderer/Texture/Texture2D.h"
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
	Texture2D::Texture2D(OpenGLES2Renderer &openGLES2Renderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags) :
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
			glGetIntegerv(GL_TEXTURE_2D_BINDING, &openGLES2TextureBackup);
		#endif

		// Set correct alignment
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Create the OpenGL ES 2 texture instance
		glGenTextures(1, &mOpenGLES2Texture);
		glBindTexture(GL_TEXTURE_2D, mOpenGLES2Texture);

		// Upload the texture data
		if (Renderer::TextureFormat::isCompressed(textureFormat))
		{
			// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
			if (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS)
			{
				// Calculate the number of mipmaps
				const uint32_t numberOfMipmaps = getNumberOfMipmaps(width, height);

				// Upload all mipmaps
				const uint32_t internalFormat = Mapping::getOpenGLES2InternalFormat(textureFormat);
				for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
				{
					// Upload the current mipmap
					const GLsizei numberOfBytesPerSlice = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height));
					glCompressedTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(mipmap), internalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, numberOfBytesPerSlice, data);

					// Move on to the next mipmap
					data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
					width = std::max(width >> 1, 1u);	// /= 2
					height = std::max(height >> 1, 1u);	// /= 2
				}
			}
			else
			{
				// The user only provided us with the base texture, no mipmaps
				glCompressedTexImage2D(GL_TEXTURE_2D, 0, Mapping::getOpenGLES2InternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height)), data);
			}
		}
		else
		{
			// Texture format is not compressed

			// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
			if (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS)
			{
				// Calculate the number of mipmaps
				const uint32_t numberOfMipmaps = getNumberOfMipmaps(width, height);

				// Upload all mipmaps
				const uint32_t internalFormat = Mapping::getOpenGLES2InternalFormat(textureFormat);
				const uint32_t format = Mapping::getOpenGLES2Format(textureFormat);
				const uint32_t type = Mapping::getOpenGLES2Type(textureFormat);
				for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
				{
					// Upload the current mipmap
					const GLsizei numberOfBytesPerSlice = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height));
					glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(mipmap), internalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, format, type, data);

					// Move on to the next mipmap
					data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
					width = std::max(width >> 1, 1u);	// /= 2
					height = std::max(height >> 1, 1u);	// /= 2
				}
			}
			else
			{
				// The user only provided us with the base texture, no mipmaps
				glTexImage2D(GL_TEXTURE_2D, 0, Mapping::getOpenGLES2InternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, Mapping::getOpenGLES2Format(textureFormat), Mapping::getOpenGLES2Type(textureFormat), data);
			}
		}

		// Build mipmaps automatically on the GPU? (or GPU driver)
		if (flags & Renderer::TextureFlag::GENERATE_MIPMAPS)
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
