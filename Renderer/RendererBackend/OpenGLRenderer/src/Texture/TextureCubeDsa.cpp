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
#include "OpenGLRenderer/Texture/TextureCubeDsa.h"
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
	TextureCubeDsa::TextureCubeDsa(OpenGLRenderer &openGLRenderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags) :
		TextureCube(openGLRenderer, width, height)
	{
		// Sanity checks
		assert(0 == (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS) || nullptr != data);

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently set alignment
			GLint openGLAlignmentBackup = 0;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &openGLAlignmentBackup);
		#endif

		// Set correct unpack alignment
		glPixelStorei(GL_UNPACK_ALIGNMENT, (Renderer::TextureFormat::getNumberOfBytesPerElement(textureFormat) & 3) ? 1 : 4);

		// Calculate the number of mipmaps
		const bool dataContainsMipmaps = (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
		const bool generateMipmaps = (!dataContainsMipmaps && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		const uint32_t numberOfMipmaps = (dataContainsMipmaps || generateMipmaps) ? getNumberOfMipmaps(width, height) : 1;
		mGenerateMipmaps = (generateMipmaps && (flags & Renderer::TextureFlag::RENDER_TARGET));

		// Create the OpenGL texture instance
		// TODO(co) "GL_ARB_direct_state_access" AMD graphics card driver bug ahead
		// -> AMD graphics card: 13.02.2017 using Radeon software 17.1.1 on MS Windows: Looks like "GL_ARB_direct_state_access" is broken when trying to use "glCompressedTextureSubImage3D()" for upload
		// -> Describes the same problem: https://community.amd.com/thread/194748 - "Upload data to GL_TEXTURE_CUBE_MAP with glTextureSubImage3D (DSA) broken ?"
		// const bool isArbDsa = openGLRenderer.getExtensions().isGL_ARB_direct_state_access();
		const bool isArbDsa = false;
		if (isArbDsa)
		{
			glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &mOpenGLTexture);
		}
		else
		{
			glGenTextures(1, &mOpenGLTexture);
		}

		// Upload the texture data
		if (Renderer::TextureFormat::isCompressed(textureFormat))
		{
			// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
			if (dataContainsMipmaps)
			{
				// Allocate storage for all levels
				if (isArbDsa)
				{
					glTextureStorage2D(mOpenGLTexture, static_cast<GLsizei>(numberOfMipmaps), Mapping::getOpenGLInternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
				}

				// Upload all mipmaps of all faces
				const uint32_t format = Mapping::getOpenGLFormat(textureFormat);
				for (uint32_t face = 0; face < 6; ++face)
				{
					uint32_t currentWidth = width;
					uint32_t currentHeight = height;
					for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
					{
						// Upload the current mipmap
						const GLsizei numberOfBytesPerSlice = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, currentWidth, currentHeight));
						if (isArbDsa)
						{
							// We know that "data" must be valid when we're in here due to the "Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS"-flag
							glCompressedTextureSubImage3D(mOpenGLTexture, static_cast<GLint>(mipmap), 0, 0, static_cast<GLint>(face), static_cast<GLsizei>(currentWidth), static_cast<GLsizei>(currentHeight), 1, format, numberOfBytesPerSlice, data);
						}
						else
						{
							glCompressedTextureImage2DEXT(mOpenGLTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, static_cast<GLint>(mipmap), format, static_cast<GLsizei>(currentWidth), static_cast<GLsizei>(currentHeight), 0, numberOfBytesPerSlice, data);
						}

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
				if (isArbDsa)
				{
					// Allocate storage for all levels
					glTextureStorage2D(mOpenGLTexture, 1, Mapping::getOpenGLInternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
					if (nullptr != data)
					{
						glCompressedTextureSubImage3D(mOpenGLTexture, 0, 0, 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 6, Mapping::getOpenGLFormat(textureFormat), static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height)), data);
					}
				}
				else
				{
					const GLsizei numberOfBytesPerSlice = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height));
					const uint32_t openGLInternalFormat = Mapping::getOpenGLInternalFormat(textureFormat);
					for (uint32_t face = 0; face < 6; ++face)
					{
						glCompressedTextureImage2DEXT(mOpenGLTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, openGLInternalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, numberOfBytesPerSlice, data);
						data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
					}
				}
			}
		}
		else
		{
			// Texture format is not compressed

			// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
			if (dataContainsMipmaps)
			{
				// Allocate storage for all levels
				if (isArbDsa)
				{
					glTextureStorage2D(mOpenGLTexture, static_cast<GLsizei>(numberOfMipmaps), Mapping::getOpenGLInternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
				}

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
						if (isArbDsa)
						{
							// We know that "data" must be valid when we're in here due to the "Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS"-flag
							glTextureSubImage3D(mOpenGLTexture, static_cast<GLint>(mipmap), 0, 0, static_cast<GLint>(face), static_cast<GLsizei>(currentWidth), static_cast<GLsizei>(currentHeight), 1, format, type, data);
						}
						else
						{
							glTextureImage2DEXT(mOpenGLTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, static_cast<GLint>(mipmap), internalFormat, static_cast<GLsizei>(currentWidth), static_cast<GLsizei>(currentHeight), 0, format, type, data);
						}

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
				if (isArbDsa)
				{
					// Allocate storage for all levels
					glTextureStorage2D(mOpenGLTexture, static_cast<GLsizei>(numberOfMipmaps), Mapping::getOpenGLInternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
					if (nullptr != data)
					{
						glTextureSubImage3D(mOpenGLTexture, 0, 0, 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 6, Mapping::getOpenGLFormat(textureFormat), Mapping::getOpenGLType(textureFormat), data);
					}
				}
				else
				{
					const uint32_t numberOfBytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
					const GLint openGLInternalFormat = static_cast<GLint>(Mapping::getOpenGLInternalFormat(textureFormat));
					const uint32_t openGLFormat = Mapping::getOpenGLFormat(textureFormat);
					const uint32_t openGLType = Mapping::getOpenGLType(textureFormat);
					for (uint32_t face = 0; face < 6; ++face)
					{
						glTextureImage2DEXT(mOpenGLTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, openGLInternalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, openGLFormat, openGLType, data);
						data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
					}
				}
			}
		}

		// Build mipmaps automatically on the GPU? (or GPU driver)
		if (flags & Renderer::TextureFlag::GENERATE_MIPMAPS)
		{
			if (isArbDsa)
			{
				glGenerateTextureMipmap(mOpenGLTexture);
				glTextureParameteri(mOpenGLTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			}
			else
			{
				glGenerateTextureMipmapEXT(mOpenGLTexture, GL_TEXTURE_CUBE_MAP);
				glTextureParameteriEXT(mOpenGLTexture, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			}
		}
		else
		{
			if (isArbDsa)
			{
				glTextureParameteri(mOpenGLTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			else
			{
				glTextureParameteriEXT(mOpenGLTexture, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
		}

		if (isArbDsa)
		{
			glTextureParameteri(mOpenGLTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTextureParameteriEXT(mOpenGLTexture, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Restore previous alignment
			glPixelStorei(GL_UNPACK_ALIGNMENT, openGLAlignmentBackup);
		#endif
	}

	TextureCubeDsa::~TextureCubeDsa()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
