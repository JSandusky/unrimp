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
#include "OpenGLES3Renderer/Buffer/TextureBuffer.h"
#include "OpenGLES3Renderer/ExtensionsRuntimeLinking.h"


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
		// Destroy the OpenGL texture instance
		// -> Silently ignores 0's and names that do not correspond to existing textures
		glDeleteTextures(1, &mOpenGLESTexture);

		// Destroy the OpenGL texture buffer
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteBuffers(1, &mOpenGLESTextureBuffer);
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	TextureBuffer::TextureBuffer(OpenGLES3Renderer &openGLES3Renderer) :
		ITextureBuffer(reinterpret_cast<Renderer::IRenderer&>(openGLES3Renderer)),
		mOpenGLESTextureBuffer(0),
		mOpenGLESTexture(0)
	{
		// Create the OpenGL texture buffer
		glGenBuffers(1, &mOpenGLESTextureBuffer);

		// Create the OpenGL texture instance
		glGenTextures(1, &mOpenGLESTexture);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
