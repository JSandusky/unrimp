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
#include "OpenGLRenderer/Texture/Texture2DArrayBind.h"
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
	Texture2DArrayBind::Texture2DArrayBind(OpenGLRenderer& openGLRenderer, uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags) :
		Texture2DArray(openGLRenderer, width, height, numberOfSlices)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently set alignment
			GLint openGLAlignmentBackup = 0;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &openGLAlignmentBackup);

			// Backup the currently bound OpenGL texture
			GLint openGLTextureBackup = 0;
			glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY_EXT, &openGLTextureBackup);
		#endif

		// Set correct unpack alignment
		glPixelStorei(GL_UNPACK_ALIGNMENT, (Renderer::TextureFormat::getNumberOfBytesPerElement(textureFormat) & 3) ? 1 : 4);

		// Create the OpenGL texture instance
		glGenTextures(1, &mOpenGLTexture);

		// Make this OpenGL texture instance to the currently used one
		glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, mOpenGLTexture);

		// TODO(co) Add support for user provided mipmaps
		// Data layout: The renderer interface provides: CRN and KTX files are organized in mip-major order, like this:
		//   Mip0: Slice0, Slice1, Slice2, Slice3, Slice4, Slice5
		//   Mip1: Slice0, Slice1, Slice2, Slice3, Slice4, Slice5
		//   etc.

		// Upload the base map of the texture (mipmaps are automatically created as soon as the base map is changed)
		glTexImage3DEXT(GL_TEXTURE_2D_ARRAY_EXT, 0, Mapping::getOpenGLInternalFormat(textureFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(numberOfSlices), 0, Mapping::getOpenGLFormat(textureFormat), Mapping::getOpenGLType(textureFormat), data);

		// Build mipmaps automatically on the GPU? (or GPU driver)
		if ((flags & Renderer::TextureFlag::GENERATE_MIPMAPS) && openGLRenderer.getExtensions().isGL_ARB_framebuffer_object())
		{
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY_EXT);
			glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Be polite and restore the previous bound OpenGL texture
			glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, static_cast<GLuint>(openGLTextureBackup));

			// Restore previous alignment
			glPixelStorei(GL_UNPACK_ALIGNMENT, openGLAlignmentBackup);
		#endif
	}

	Texture2DArrayBind::~Texture2DArrayBind()
	{
		// Nothing to do in here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
