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
#include "OpenGLES3Renderer/Buffer/TextureBuffer.h"
#include "OpenGLES3Renderer/ExtensionsRuntimeLinking.h"
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"
#include "OpenGLES3Renderer/IExtensions.h"

#include <Renderer/IRenderer.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureBuffer::~TextureBuffer()
	{
		// Destroy the OpenGL ES 3 texture instance
		// -> Silently ignores 0's and names that do not correspond to existing textures
		glDeleteTextures(1, &mOpenGLES3Texture);

		// Destroy the OpenGL ES 3  texture buffer
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteBuffers(1, &mOpenGLES3TextureBuffer);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void TextureBuffer::setDebugName(const char* name)
		{
			// "GL_KHR_debug"-extension available?
			if (static_cast<OpenGLES3Renderer&>(getRenderer()).getOpenGLES3Context().getExtensions().isGL_KHR_debug())
			{
				if (0 != mOpenGLES3Texture)
				{
					glObjectLabelKHR(GL_TEXTURE, mOpenGLES3Texture, -1, name);
				}
				if (0 != mOpenGLES3TextureBuffer)
				{
					glObjectLabelKHR(GL_BUFFER_KHR, mOpenGLES3TextureBuffer, -1, name);
				}
			}
		}
	#else
		void TextureBuffer::setDebugName(const char*)
		{
			// Nothing here
		}
	#endif


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void TextureBuffer::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), TextureBuffer, this);
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	TextureBuffer::TextureBuffer(OpenGLES3Renderer& openGLES3Renderer, uint32_t numberOfBytes) :
		ITextureBuffer(static_cast<Renderer::IRenderer&>(openGLES3Renderer)),
		mOpenGLES3TextureBuffer(0),
		mOpenGLES3Texture(0),
		mBufferSize(numberOfBytes)
	{
		// Create the OpenGL ES 3 texture buffer
		glGenBuffers(1, &mOpenGLES3TextureBuffer);

		// Create the OpenGL ES 3 texture instance
		glGenTextures(1, &mOpenGLES3Texture);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
