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
#include "OpenGLRenderer/Texture/Texture2DArrayDsa.h"
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
	Texture2DArrayDsa::Texture2DArrayDsa(OpenGLRenderer& openGLRenderer, uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags) :
		Texture2DArray(openGLRenderer, width, height, numberOfSlices)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently set alignment
			GLint openGLAlignmentBackup = 0;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &openGLAlignmentBackup);
		#endif

		// Set correct unpack alignment
		glPixelStorei(GL_UNPACK_ALIGNMENT, (Renderer::TextureFormat::getNumberOfBytesPerElement(textureFormat) & 3) ? 1 : 4);

		// Create the OpenGL texture instance
		#ifdef WIN32
			// TODO(co) It appears that DSA "glGenerateTextureMipmap()" is not working (one notices the noise) or we're using it wrong, tested with
			//			- "InstancedCubes"-example -> "CubeRendereDrawInstanced"
			//		    - AMD 290X Radeon software version 17.7.2 as well as with GeForce 980m 384.94
			//		    - Windows 10 x64
			const bool isArbDsa = (openGLRenderer.getExtensions().isGL_ARB_direct_state_access() && (flags & Renderer::TextureFlag::GENERATE_MIPMAPS) == 0);
		#else
			const bool isArbDsa = openGLRenderer.getExtensions().isGL_ARB_direct_state_access();
		#endif
		if (isArbDsa)
		{
			glCreateTextures(GL_TEXTURE_2D_ARRAY_EXT, 1, &mOpenGLTexture);
		}
		else
		{
			glGenTextures(1, &mOpenGLTexture);
		}

		// TODO(co) Add support for user provided mipmaps
		// Data layout: The renderer interface provides: CRN and KTX files are organized in mip-major order, like this:
		//   Mip0: Slice0, Slice1, Slice2, Slice3, Slice4, Slice5
		//   Mip1: Slice0, Slice1, Slice2, Slice3, Slice4, Slice5
		//   etc.

		// Upload the base map of the texture (mipmaps are automatically created as soon as the base map is changed)
		if (isArbDsa)
		{
			glTextureStorage3D(mOpenGLTexture, 1, Mapping::getOpenGLInternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(numberOfSlices));
			if (nullptr != data)
			{
				glTextureSubImage3D(mOpenGLTexture, 0, 0, 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(numberOfSlices), Mapping::getOpenGLFormat(textureFormat), Mapping::getOpenGLType(textureFormat), data);
			}
		}
		else
		{
			glTextureImage3DEXT(mOpenGLTexture, GL_TEXTURE_2D_ARRAY_EXT, 0, static_cast<GLint>(Mapping::getOpenGLInternalFormat(textureFormat)), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(numberOfSlices), 0, Mapping::getOpenGLFormat(textureFormat), Mapping::getOpenGLType(textureFormat), data);
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
				glGenerateTextureMipmapEXT(mOpenGLTexture, GL_TEXTURE_2D_ARRAY_EXT);
				glTextureParameteriEXT(mOpenGLTexture, GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
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
				glTextureParameteriEXT(mOpenGLTexture, GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
		}

		if (isArbDsa)
		{
			glTextureParameteri(mOpenGLTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTextureParameteriEXT(mOpenGLTexture, GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Restore previous alignment
			glPixelStorei(GL_UNPACK_ALIGNMENT, openGLAlignmentBackup);
		#endif
	}

	Texture2DArrayDsa::~Texture2DArrayDsa()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
