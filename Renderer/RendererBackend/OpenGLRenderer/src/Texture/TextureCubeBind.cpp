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
#include "OpenGLRenderer/Texture/TextureCubeBind.h"
#include "OpenGLRenderer/Mapping.h"
#include "OpenGLRenderer/IContext.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureCubeBind::TextureCubeBind(OpenGLRenderer& openGLRenderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags) :
		TextureCube(openGLRenderer, width, height)
	{
		// Sanity checks
		assert(0 == (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS) || nullptr != data);

		// Create the OpenGL texture instance
		glGenTextures(1, &mOpenGLTexture);

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently set alignment
			GLint openGLAlignmentBackup = 0;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &openGLAlignmentBackup);

			// Backup the currently bound OpenGL texture
			GLint openGLTextureBackup = 0;
			glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &openGLTextureBackup);
		#endif

		// Set correct unpack alignment
		glPixelStorei(GL_UNPACK_ALIGNMENT, (Renderer::TextureFormat::getNumberOfBytesPerElement(textureFormat) & 3) ? 1 : 4);

		// Calculate the number of mipmaps
		const bool dataContainsMipmaps = (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
		const bool generateMipmaps = (!dataContainsMipmaps && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		const uint32_t numberOfMipmaps = (dataContainsMipmaps || generateMipmaps) ? getNumberOfMipmaps(width, height) : 1;
		mGenerateMipmaps = (generateMipmaps && (flags & Renderer::TextureFlag::RENDER_TARGET));

		// Make this OpenGL texture instance to the currently used one
		glBindTexture(GL_TEXTURE_CUBE_MAP, mOpenGLTexture);

		// Upload the texture data
		if (Renderer::TextureFormat::isCompressed(textureFormat))
		{
			// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
			if (dataContainsMipmaps)
			{
				// Upload all mipmaps of all faces
				const uint32_t internalFormat = Mapping::getOpenGLInternalFormat(textureFormat);
				for (uint32_t face = 0; face < 6; ++face)
				{
					uint32_t currentWidth = width;
					uint32_t currentHeight = height;
					for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
					{
						// Upload the current mipmap
						const GLsizei numberOfBytesPerSlice = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, currentWidth, currentHeight));
						glCompressedTexImage2DARB(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, static_cast<GLint>(mipmap), internalFormat, static_cast<GLsizei>(currentWidth), static_cast<GLsizei>(currentHeight), 0, numberOfBytesPerSlice, data);

						// Move on to the next mipmap
						data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
						currentWidth = std::max(currentWidth >> 1, 1u);		// /= 2
						currentHeight = std::max(currentHeight >> 1, 1u);	// /= 2
					}
				}
			}
			else
			{
				// The user only provided us with the base texture, no mipmaps
				const uint32_t numberOfBytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
				const uint32_t openGLInternalFormat = Mapping::getOpenGLInternalFormat(textureFormat);
				for (uint32_t face = 0; face < 6; ++face)
				{
					glCompressedTexImage2DARB(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, openGLInternalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, static_cast<GLsizei>(numberOfBytesPerSlice), data);
					data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
				}
			}
		}
		else
		{
			// Texture format is not compressed

			// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
			if (dataContainsMipmaps)
			{
				// Upload all mipmaps of all faces
				const GLint internalFormat = static_cast<GLint>(Mapping::getOpenGLInternalFormat(textureFormat));
				const uint32_t format = Mapping::getOpenGLFormat(textureFormat);
				const uint32_t type = Mapping::getOpenGLType(textureFormat);
				for (uint32_t face = 0; face < 6; ++face)
				{
					uint32_t currentWidth = width;
					uint32_t currentHeight = height;
					for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
					{
						// Upload the current mipmap
						const GLsizei numberOfBytesPerSlice = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, currentWidth, currentHeight));
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, static_cast<GLint>(mipmap), internalFormat, static_cast<GLsizei>(currentWidth), static_cast<GLsizei>(currentHeight), 0, format, type, data);

						// Move on to the next mipmap
						data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
						currentWidth = std::max(currentWidth >> 1, 1u);		// /= 2
						currentHeight = std::max(currentHeight >> 1, 1u);	// /= 2
					}
				}
			}
			else
			{
				// The user only provided us with the base texture, no mipmaps
				const uint32_t numberOfBytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
				const GLint openGLInternalFormat = static_cast<GLint>(Mapping::getOpenGLInternalFormat(textureFormat));
				const uint32_t openGLFormat = Mapping::getOpenGLFormat(textureFormat);
				const uint32_t openGLType = Mapping::getOpenGLType(textureFormat);
				for (uint32_t face = 0; face < 6; ++face)
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, openGLInternalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, openGLFormat, openGLType, data);
					data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
				}
			}
		}

		// Build mipmaps automatically on the GPU? (or GPU driver)
		if ((flags & Renderer::TextureFlag::GENERATE_MIPMAPS) && openGLRenderer.getExtensions().isGL_ARB_framebuffer_object())
		{
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL texture
			glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<GLuint>(openGLTextureBackup));

			// Restore previous alignment
			glPixelStorei(GL_UNPACK_ALIGNMENT, openGLAlignmentBackup);
		#endif
	}

	TextureCubeBind::~TextureCubeBind()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
