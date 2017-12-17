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
#include "OpenGLES3Renderer/Texture/TextureCube.h"
#include "OpenGLES3Renderer/Mapping.h"
#include "OpenGLES3Renderer/IOpenGLES3Context.h"	// We need to include this header, else the linker won't find our defined OpenGL ES 3 functions
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"

#include <Renderer/IAssert.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureCube::TextureCube(OpenGLES3Renderer& openGLES3Renderer, uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags) :
		ITextureCube(openGLES3Renderer, width, height),
		mOpenGLES3Texture(0),
		mGenerateMipmaps(false)
	{
		// Sanity checks
		RENDERER_ASSERT(openGLES3Renderer.getContext(), 0 == (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS) || nullptr != data, "Invalid OpenGL ES 3 texture parameters")
		RENDERER_ASSERT(openGLES3Renderer.getContext(), (flags & Renderer::TextureFlag::RENDER_TARGET) == 0 || nullptr == data, "OpenGL ES 3 render target textures can't be filled using provided data")

		// TODO(co) Check support formats

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Backup the currently set alignment
			GLint openGLES3AlignmentBackup = 0;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &openGLES3AlignmentBackup);

			// Backup the currently bound OpenGL ES 3 texture
			GLint openGLES3TextureBackup = 0;
			glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &openGLES3TextureBackup);
		#endif

		// Set correct alignment
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Calculate the number of mipmaps
		const bool dataContainsMipmaps = (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
		const bool generateMipmaps = (!dataContainsMipmaps && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		const uint32_t numberOfMipmaps = (dataContainsMipmaps || generateMipmaps) ? getNumberOfMipmaps(width, height) : 1;
		mGenerateMipmaps = (generateMipmaps && (flags & Renderer::TextureFlag::RENDER_TARGET));

		// Create the OpenGL ES 3 texture instance
		glGenTextures(1, &mOpenGLES3Texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mOpenGLES3Texture);

		// Upload the texture data
		if (Renderer::TextureFormat::isCompressed(textureFormat))
		{
			// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
			if (dataContainsMipmaps)
			{
				// Data layout: The renderer interface provides: CRN and KTX files are organized in mip-major order, like this:
				//   Mip0: Face0, Face1, Face2, Face3, Face4, Face5
				//   Mip1: Face0, Face1, Face2, Face3, Face4, Face5
				//   etc.

				// Upload all mipmaps of all faces
				const uint32_t internalFormat = Mapping::getOpenGLES3InternalFormat(textureFormat);
				for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
				{
					const GLsizei numberOfBytesPerSlice = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height));
					for (uint32_t face = 0; face < 6; ++face)
					{
						// Upload the current face
						glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, static_cast<GLint>(mipmap), internalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, numberOfBytesPerSlice, data);

						// Move on to the next face
						data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
					}

					// Move on to the next mipmap
					width = std::max(width >> 1, 1u);	// /= 2
					height = std::max(height >> 1, 1u);	// /= 2
				}
			}
			else
			{
				// The user only provided us with the base texture, no mipmaps
				const GLsizei numberOfBytesPerSlice = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height));
				const uint32_t openGLES3InternalFormat = Mapping::getOpenGLES3InternalFormat(textureFormat);
				for (uint32_t face = 0; face < 6; ++face)
				{
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, openGLES3InternalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, numberOfBytesPerSlice, data);
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
				// Data layout: The renderer interface provides: CRN and KTX files are organized in mip-major order, like this:
				//   Mip0: Face0, Face1, Face2, Face3, Face4, Face5
				//   Mip1: Face0, Face1, Face2, Face3, Face4, Face5
				//   etc.

				// Upload all mipmaps of all faces
				const uint32_t internalFormat = Mapping::getOpenGLES3InternalFormat(textureFormat);
				const uint32_t format = Mapping::getOpenGLES3Format(textureFormat);
				const uint32_t type = Mapping::getOpenGLES3Type(textureFormat);
				for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
				{
					const GLsizei numberOfBytesPerSlice = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height));
					for (uint32_t face = 0; face < 6; ++face)
					{
						// Upload the current face
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, static_cast<GLint>(mipmap), internalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, format, type, data);

						// Move on to the next face
						data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
					}

					// Move on to the next mipmap
					width = std::max(width >> 1, 1u);	// /= 2
					height = std::max(height >> 1, 1u);	// /= 2
				}
			}
			else
			{
				// The user only provided us with the base texture, no mipmaps
				const uint32_t numberOfBytesPerSlice = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height);
				const uint32_t openGLES3InternalFormat = Mapping::getOpenGLES3InternalFormat(textureFormat);
				const uint32_t openGLES3Format = Mapping::getOpenGLES3Format(textureFormat);
				const uint32_t openGLES3Type = Mapping::getOpenGLES3Type(textureFormat);
				for (uint32_t face = 0; face < 6; ++face)
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, openGLES3InternalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, openGLES3Format, openGLES3Type, data);
					data = static_cast<const uint8_t*>(data) + numberOfBytesPerSlice;
				}
			}
		}

		// Build mipmaps automatically on the GPU? (or GPU driver)
		if (flags & Renderer::TextureFlag::GENERATE_MIPMAPS)
		{
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL ES 3 texture
			glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<GLuint>(openGLES3TextureBackup));

			// Restore previous alignment
			glPixelStorei(GL_UNPACK_ALIGNMENT, openGLES3AlignmentBackup);
		#endif
	}

	TextureCube::~TextureCube()
	{
		// Destroy the OpenGL ES 3 texture instance
		// -> Silently ignores 0's and names that do not correspond to existing textures
		glDeleteTextures(1, &mOpenGLES3Texture);
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void TextureCube::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), TextureCube, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
