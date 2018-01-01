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
#include "OpenGLRenderer/Buffer/IndexBuffer.h"
#include "OpenGLRenderer/Mapping.h"
#include "OpenGLRenderer/Extensions.h"

#include <Renderer/IRenderer.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IndexBuffer::~IndexBuffer()
	{
		// Destroy the OpenGL element array buffer
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteBuffersARB(1, &mOpenGLElementArrayBuffer);
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void IndexBuffer::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), IndexBuffer, this);
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	IndexBuffer::IndexBuffer(OpenGLRenderer& openGLRenderer, Renderer::IndexBufferFormat::Enum indexBufferFormat) :
		IIndexBuffer(reinterpret_cast<Renderer::IRenderer&>(openGLRenderer)),
		mOpenGLElementArrayBuffer(0),
		mOpenGLType(Mapping::getOpenGLType(indexBufferFormat)),
		mIndexSizeInBytes(Renderer::IndexBufferFormat::getNumberOfBytesPerElement(indexBufferFormat))
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
