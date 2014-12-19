/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "OpenGLRenderer/Texture2DArrayDsa.h"
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
	Texture2DArrayDsa::Texture2DArrayDsa(OpenGLRenderer &openGLRenderer, uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags) :
		Texture2DArray(openGLRenderer, width, height, numberOfSlices)
	{
		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Backup the currently set alignment
			GLint openGLAlignmentBackup = 0;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &openGLAlignmentBackup);
		#endif

		// Set correct alignment
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Upload the base map of the texture (mipmaps are automatically created as soon as the base map is changed)
		glTextureImage3DEXT(mOpenGLTexture, GL_TEXTURE_2D_ARRAY_EXT, 0, static_cast<GLint>(Mapping::getOpenGLInternalFormat(textureFormat)), static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(numberOfSlices), 0, Mapping::getOpenGLFormat(textureFormat), Mapping::getOpenGLType(textureFormat), data);

		// Build mipmaps automatically on the GPU?
		if (flags & Renderer::TextureFlag::MIPMAPS)
		{
			glGenerateTextureMipmapEXT(mOpenGLTexture, GL_TEXTURE_2D_ARRAY_EXT);
			glTextureParameteriEXT(mOpenGLTexture, GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		}
		else
		{
			glTextureParameteriEXT(mOpenGLTexture, GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTextureParameteriEXT(mOpenGLTexture, GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
			// Restore previous alignment
			glPixelStorei(GL_UNPACK_ALIGNMENT, openGLAlignmentBackup);
		#endif
	}

	Texture2DArrayDsa::~Texture2DArrayDsa()
	{
		// Nothing to do in here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
