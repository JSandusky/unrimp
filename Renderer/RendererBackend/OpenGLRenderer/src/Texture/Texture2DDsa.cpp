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
#include "OpenGLRenderer/Texture/Texture2DDsa.h"
#include "OpenGLRenderer/Mapping.h"
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
	Texture2DDsa::Texture2DDsa(OpenGLRenderer& openGLRenderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, uint8_t numberOfMultisamples) :
		Texture2D(openGLRenderer, width, height, numberOfMultisamples)
	{
		// Sanity checks
		assert(numberOfMultisamples == 1 || numberOfMultisamples == 2 || numberOfMultisamples == 4 || numberOfMultisamples == 8);
		assert(numberOfMultisamples == 1 || nullptr == data);
		assert(numberOfMultisamples == 1 || 0 == (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS));
		assert(numberOfMultisamples == 1 || 0 == (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		assert(numberOfMultisamples == 1 || 0 != (flags & Renderer::TextureFlag::RENDER_TARGET));
		assert(0 == (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS) || nullptr != data);
		assert(((flags & Renderer::TextureFlag::RENDER_TARGET) == 0 || nullptr == data) && "Render target textures can't be filled using provided data");

		// Multisample texture?
		const bool isArbDsa = openGLRenderer.getExtensions().isGL_ARB_direct_state_access();
		if (numberOfMultisamples > 1)
		{
			if (isArbDsa)
			{
				// Create the OpenGL texture instance
				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &mOpenGLTexture);

				// Define the texture
				glTextureStorage2DMultisample(mOpenGLTexture, numberOfMultisamples, Mapping::getOpenGLInternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), GL_TRUE);
			}
			else
			{
				// Create the OpenGL texture instance
				glGenTextures(1, &mOpenGLTexture);

				#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
					// Backup the currently bound OpenGL texture
					GLint openGLTextureBackup = 0;
					glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &openGLTextureBackup);
				#endif

				// Make this OpenGL texture instance to the currently used one
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mOpenGLTexture);

				// Define the texture
				// -> Sadly, there's no direct state access (DSA) function defined for this in "GL_EXT_direct_state_access"
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numberOfMultisamples, Mapping::getOpenGLInternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), GL_TRUE);

				#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL texture
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, static_cast<GLuint>(openGLTextureBackup));
				#endif
			}
		}
		else
		{
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
			if (isArbDsa)
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &mOpenGLTexture);
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

					// Upload all mipmaps
					const uint32_t format = Mapping::getOpenGLFormat(textureFormat);
					for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
					{
						// Upload the current mipmap
						const GLsizei numberOfBytesPerSlice = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height));
						if (isArbDsa)
						{
							// We know that "data" must be valid when we're in here due to the "Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS"-flag
							glCompressedTextureSubImage2D(mOpenGLTexture, static_cast<GLint>(mipmap), 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), format, numberOfBytesPerSlice, data);
						}
						else
						{
							glCompressedTextureImage2DEXT(mOpenGLTexture, GL_TEXTURE_2D, static_cast<GLint>(mipmap), format, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, numberOfBytesPerSlice, data);
						}

						// Move on to the next mipmap
						data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
						width = std::max(width >> 1, 1u);	// /= 2
						height = std::max(height >> 1, 1u);	// /= 2
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
							glCompressedTextureSubImage2D(mOpenGLTexture, 0, 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), Mapping::getOpenGLFormat(textureFormat), static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height)), data);
						}
					}
					else
					{
						glCompressedTextureImage2DEXT(mOpenGLTexture, GL_TEXTURE_2D, 0, Mapping::getOpenGLInternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height)), data);
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

					// Upload all mipmaps
					const GLint internalFormat = static_cast<GLint>(Mapping::getOpenGLInternalFormat(textureFormat));
					const uint32_t format = Mapping::getOpenGLFormat(textureFormat);
					const uint32_t type = Mapping::getOpenGLType(textureFormat);
					for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
					{
						// Upload the current mipmap
						const GLsizei numberOfBytesPerSlice = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height));
						if (isArbDsa)
						{
							// We know that "data" must be valid when we're in here due to the "Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS"-flag
							glTextureSubImage2D(mOpenGLTexture, static_cast<GLint>(mipmap), 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), format, type, data);
						}
						else
						{
							glTextureImage2DEXT(mOpenGLTexture, GL_TEXTURE_2D, static_cast<GLint>(mipmap), internalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, format, type, data);
						}

						// Move on to the next mipmap
						data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
						width = std::max(width >> 1, 1u);	// /= 2
						height = std::max(height >> 1, 1u);	// /= 2
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
							glTextureSubImage2D(mOpenGLTexture, 0, 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), Mapping::getOpenGLFormat(textureFormat), Mapping::getOpenGLType(textureFormat), data);
						}
					}
					else
					{
						glTextureImage2DEXT(mOpenGLTexture, GL_TEXTURE_2D, 0, static_cast<GLint>(Mapping::getOpenGLInternalFormat(textureFormat)), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, Mapping::getOpenGLFormat(textureFormat), Mapping::getOpenGLType(textureFormat), data);
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
					glGenerateTextureMipmapEXT(mOpenGLTexture, GL_TEXTURE_2D);
					glTextureParameteriEXT(mOpenGLTexture, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
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
					glTextureParameteriEXT(mOpenGLTexture, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				}
			}

			if (isArbDsa)
			{
				glTextureParameteri(mOpenGLTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else
			{
				glTextureParameteriEXT(mOpenGLTexture, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}

			#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
				// Restore previous alignment
				glPixelStorei(GL_UNPACK_ALIGNMENT, openGLAlignmentBackup);
			#endif
		}
	}

	Texture2DDsa::~Texture2DDsa()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
