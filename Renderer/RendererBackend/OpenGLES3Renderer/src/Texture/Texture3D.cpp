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
#include "OpenGLES3Renderer/Texture/Texture3D.h"
#include "OpenGLES3Renderer/Mapping.h"
#include "OpenGLES3Renderer/IContext.h"	// We need to include this header, else the linker won't find our defined OpenGL ES 3 functions
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture3D::Texture3D(OpenGLES3Renderer& openGLES3Renderer, uint32_t width, uint32_t height, uint32_t depth, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags) :
		ITexture3D(openGLES3Renderer, width, height, depth),
		mTextureFormat(textureFormat),
		mOpenGLES3Texture(0),
		mGenerateMipmaps(false)
	{
		// Sanity checks
		assert(0 == (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS) || nullptr != data);

		// TODO(co) Check support formats

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Backup the currently set alignment
			GLint openGLES3AlignmentBackup = 0;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &openGLES3AlignmentBackup);

			// Backup the currently bound OpenGL ES 3 texture
			GLint openGLES3TextureBackup = 0;
			glGetIntegerv(GL_TEXTURE_BINDING_3D, &openGLES3TextureBackup);
		#endif

		// Set correct alignment
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Calculate the number of mipmaps
		const bool dataContainsMipmaps = (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
		const bool generateMipmaps = (!dataContainsMipmaps && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		const uint32_t numberOfMipmaps = (dataContainsMipmaps || generateMipmaps) ? getNumberOfMipmaps(width, height, depth) : 1;
		mGenerateMipmaps = (generateMipmaps && (flags & Renderer::TextureFlag::RENDER_TARGET));

		// Create the OpenGL ES 3 texture instance
		glGenTextures(1, &mOpenGLES3Texture);
		glBindTexture(GL_TEXTURE_3D, mOpenGLES3Texture);

		// Upload the texture data
		if (Renderer::TextureFormat::isCompressed(textureFormat))
		{
			// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
			if (dataContainsMipmaps)
			{
				// Upload all mipmaps
				const uint32_t internalFormat = Mapping::getOpenGLES3InternalFormat(textureFormat);
				for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
				{
					// Upload the current mipmap
					const GLsizei numberOfBytesPerMipmap = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height) * depth);
					glCompressedTexImage3D(GL_TEXTURE_3D, static_cast<GLint>(mipmap), internalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(depth), 0, numberOfBytesPerMipmap, data);

					// Move on to the next mipmap
					data = static_cast<const uint8_t*>(data) + numberOfBytesPerMipmap;
					width = std::max(width >> 1, 1u);	// /= 2
					height = std::max(height >> 1, 1u);	// /= 2
					depth = std::max(depth >> 1, 1u);	// /= 2
				}
			}
			else
			{
				// The user only provided us with the base texture, no mipmaps
				glCompressedTexImage3D(GL_TEXTURE_3D, 0, Mapping::getOpenGLES3InternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(depth), 0, static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height)), data);
			}
		}
		else
		{
			// Texture format is not compressed

			// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
			if (dataContainsMipmaps)
			{
				// Upload all mipmaps
				const uint32_t internalFormat = Mapping::getOpenGLES3InternalFormat(textureFormat);
				const uint32_t format = Mapping::getOpenGLES3Format(textureFormat);
				const uint32_t type = Mapping::getOpenGLES3Type(textureFormat);
				for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
				{
					// Upload the current mipmap
					const GLsizei numberOfBytesPerMipmap = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height) * depth);
					glTexImage3D(GL_TEXTURE_3D, static_cast<GLint>(mipmap), internalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(depth), 0, format, type, data);

					// Move on to the next mipmap
					data = static_cast<const uint8_t*>(data) + numberOfBytesPerMipmap;
					width = std::max(width >> 1, 1u);	// /= 2
					height = std::max(height >> 1, 1u);	// /= 2
					depth = std::max(depth >> 1, 1u);	// /= 2
				}
			}
			else
			{
				// The user only provided us with the base texture, no mipmaps
				glTexImage3D(GL_TEXTURE_3D, 0, Mapping::getOpenGLES3InternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(depth), 0, Mapping::getOpenGLES3Format(textureFormat), Mapping::getOpenGLES3Type(textureFormat), data);
			}
		}

		// Build mipmaps automatically on the GPU? (or GPU driver)
		if (flags & Renderer::TextureFlag::GENERATE_MIPMAPS)
		{
			glGenerateMipmap(GL_TEXTURE_3D);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 3 texture
			glBindTexture(GL_TEXTURE_3D, static_cast<GLuint>(openGLES3TextureBackup));

			// Restore previous alignment
			glPixelStorei(GL_UNPACK_ALIGNMENT, openGLES3AlignmentBackup);
		#endif
	}

	Texture3D::~Texture3D()
	{
		// Destroy the OpenGL ES 3 texture instance
		// -> Silently ignores 0's and names that do not correspond to existing textures
		glDeleteTextures(1, &mOpenGLES3Texture);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITexture3D methods           ]
	//[-------------------------------------------------------]
	void Texture3D::copyDataFrom(uint32_t, const void* data)
	{
		// Sanity check
		assert(nullptr != data);

		// Copy data
		glTexImage3D(GL_TEXTURE_3D, 0, Mapping::getOpenGLES3InternalFormat(mTextureFormat), static_cast<GLsizei>(getWidth()), static_cast<GLsizei>(getHeight()), static_cast<GLsizei>(getDepth()), 0, Mapping::getOpenGLES3Format(mTextureFormat), Mapping::getOpenGLES3Type(mTextureFormat), data);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
