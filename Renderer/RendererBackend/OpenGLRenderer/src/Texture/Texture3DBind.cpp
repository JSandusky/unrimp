/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "OpenGLRenderer/Texture/Texture3DBind.h"
#include "OpenGLRenderer/Mapping.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"

#include <Renderer/IAssert.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Texture3DBind::Texture3DBind(OpenGLRenderer& openGLRenderer, uint32_t width, uint32_t height, uint32_t depth, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags, Renderer::TextureUsage textureUsage) :
		Texture3D(openGLRenderer, width, height, depth, textureFormat)
	{
		// Sanity checks
		RENDERER_ASSERT(openGLRenderer.getContext(), 0 == (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS) || nullptr != data, "Invalid OpenGL texture parameters")
		RENDERER_ASSERT(openGLRenderer.getContext(), (flags & Renderer::TextureFlag::RENDER_TARGET) == 0 || nullptr == data, "OpenGL render target textures can't be filled using provided data")

		// Create the OpenGL texture instance
		glGenTextures(1, &mOpenGLTexture);

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently set alignment
			GLint openGLAlignmentBackup = 0;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &openGLAlignmentBackup);

			// Backup the currently bound OpenGL texture
			GLint openGLTextureBackup = 0;
			glGetIntegerv(GL_TEXTURE_BINDING_3D, &openGLTextureBackup);
		#endif

		// Set correct unpack alignment
		glPixelStorei(GL_UNPACK_ALIGNMENT, (Renderer::TextureFormat::getNumberOfBytesPerElement(textureFormat) & 3) ? 1 : 4);

		// Create OpenGL pixel unpack buffer for dynamic textures, if necessary
		if (Renderer::TextureUsage::IMMUTABLE != textureUsage)
		{
			// Backup the currently bound OpenGL pixel unpack buffer
			#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
				GLint openGLUnpackBufferBackup = 0;
				glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING_ARB, &openGLUnpackBufferBackup);
			#endif

			// Create the OpenGL pixel unpack buffer
			glGenBuffersARB(1, &mOpenGLPixelUnpackBuffer);

			// Bind this OpenGL pixel unpack buffer, the OpenGL pixel unpack buffer must be able to hold the top-level mipmap
			// TODO(co) Or must the OpenGL pixel unpack buffer able to hold the entire texture including all mipmaps? Depends on the later usage I assume.
			const uint32_t numberOfBytes = Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height) * depth;
			glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mOpenGLPixelUnpackBuffer);
			glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, static_cast<GLsizeiptrARB>(numberOfBytes), nullptr, static_cast<GLenum>(GL_STREAM_DRAW));

			// Be polite and restore the previous bound OpenGL pixel unpack buffer
			#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
				glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, static_cast<GLuint>(openGLUnpackBufferBackup));
			#else
				glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
			#endif
		}

		// Calculate the number of mipmaps
		const bool dataContainsMipmaps = (flags & Renderer::TextureFlag::DATA_CONTAINS_MIPMAPS);
		const bool generateMipmaps = (!dataContainsMipmaps && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS));
		RENDERER_ASSERT(openGLRenderer.getContext(), Renderer::TextureUsage::IMMUTABLE != textureUsage || !generateMipmaps, "OpenGL immutable texture usage can't be combined with automatic mipmap generation")
		const uint32_t numberOfMipmaps = (dataContainsMipmaps || generateMipmaps) ? getNumberOfMipmaps(width, height, depth) : 1;
		mGenerateMipmaps = (generateMipmaps && (flags & Renderer::TextureFlag::RENDER_TARGET));

		// Make this OpenGL texture instance to the currently used one
		glBindTexture(GL_TEXTURE_3D, mOpenGLTexture);

		// Upload the texture data
		if (Renderer::TextureFormat::isCompressed(textureFormat))
		{
			// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
			if (dataContainsMipmaps)
			{
				// Data layout: The renderer interface provides: CRN and KTX files are organized in mip-major order, like this:
				//   Mip0: Slice0, Slice1, Slice2, Slice3, Slice4, Slice5
				//   Mip1: Slice0, Slice1, Slice2, Slice3, Slice4, Slice5
				//   etc.

				// Upload all mipmaps
				const uint32_t internalFormat = Mapping::getOpenGLInternalFormat(textureFormat);
				for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
				{
					// Upload the current mipmap
					const GLsizei numberOfBytesPerMipmap = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height) * depth);
					glCompressedTexImage3DARB(GL_TEXTURE_3D, static_cast<GLint>(mipmap), internalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(depth), 0, numberOfBytesPerMipmap, data);

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
				glCompressedTexImage3DARB(GL_TEXTURE_3D, 0, Mapping::getOpenGLInternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(depth), 0, static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height)), data);
			}
		}
		else
		{
			// Texture format is not compressed

			// Did the user provided data containing mipmaps from 0-n down to 1x1 linearly in memory?
			if (dataContainsMipmaps)
			{
				// Data layout: The renderer interface provides: CRN and KTX files are organized in mip-major order, like this:
				//   Mip0: Slice0, Slice1, Slice2, Slice3, Slice4, Slice5
				//   Mip1: Slice0, Slice1, Slice2, Slice3, Slice4, Slice5
				//   etc.

				// Upload all mipmaps
				const GLenum internalFormat = static_cast<GLenum>(Mapping::getOpenGLInternalFormat(textureFormat));
				const uint32_t format = Mapping::getOpenGLFormat(textureFormat);
				const uint32_t type = Mapping::getOpenGLType(textureFormat);
				for (uint32_t mipmap = 0; mipmap < numberOfMipmaps; ++mipmap)
				{
					// Upload the current mipmap
					const GLsizei numberOfBytesPerMipmap = static_cast<GLsizei>(Renderer::TextureFormat::getNumberOfBytesPerSlice(textureFormat, width, height) * depth);
					glTexImage3DEXT(GL_TEXTURE_3D, static_cast<GLint>(mipmap), internalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(depth), 0, format, type, data);

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
				glTexImage3DEXT(GL_TEXTURE_3D, 0, static_cast<GLenum>(Mapping::getOpenGLInternalFormat(textureFormat)), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(depth), 0, Mapping::getOpenGLFormat(textureFormat), Mapping::getOpenGLType(textureFormat), data);
			}
		}

		// Build mipmaps automatically on the GPU? (or GPU driver)
		if ((flags & Renderer::TextureFlag::GENERATE_MIPMAPS) && openGLRenderer.getExtensions().isGL_ARB_framebuffer_object())
		{
			glGenerateMipmap(GL_TEXTURE_3D);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL texture
			glBindTexture(GL_TEXTURE_3D, static_cast<GLuint>(openGLTextureBackup));

			// Restore previous alignment
			glPixelStorei(GL_UNPACK_ALIGNMENT, openGLAlignmentBackup);
		#endif
	}

	Texture3DBind::~Texture3DBind()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
