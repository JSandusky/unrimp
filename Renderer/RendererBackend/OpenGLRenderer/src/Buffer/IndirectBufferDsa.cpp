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
#include "OpenGLRenderer/Buffer/IndirectBufferDsa.h"
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
	IndirectBufferDsa::IndirectBufferDsa(OpenGLRenderer& openGLRenderer, uint32_t numberOfBytes, const void* data, Renderer::BufferUsage bufferUsage) :
		IndirectBuffer(openGLRenderer)
	{
		if (openGLRenderer.getExtensions().isGL_ARB_direct_state_access())
		{
			// Create the OpenGL indirect buffer
			glCreateBuffers(1, &mOpenGLIndirectBuffer);

			// Upload the data
			// -> Usage: These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 2 constants, do not change them
			glNamedBufferData(mOpenGLIndirectBuffer, static_cast<GLsizeiptr>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));
		}
		else
		{
			// Create the OpenGL indirect buffer
			glGenBuffersARB(1, &mOpenGLIndirectBuffer);

			// Upload the data
			// -> Usage: These constants directly map to "GL_ARB_vertex_buffer_object" and OpenGL ES 2 constants, do not change them
			glNamedBufferDataEXT(mOpenGLIndirectBuffer, static_cast<GLsizeiptr>(numberOfBytes), data, static_cast<GLenum>(bufferUsage));
		}
	}

	IndirectBufferDsa::~IndirectBufferDsa()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IIndirectBuffer methods      ]
	//[-------------------------------------------------------]
	void IndirectBufferDsa::copyDataFrom(uint32_t numberOfBytes, const void* data)
	{
		// Sanity check
		RENDERER_ASSERT(getRenderer().getContext(), nullptr != data, "Invalid OpenGL indirect buffer data")

		// Upload the data
		if (static_cast<OpenGLRenderer&>(getRenderer()).getExtensions().isGL_ARB_direct_state_access())
		{
			glNamedBufferSubData(mOpenGLIndirectBuffer, 0, static_cast<GLsizeiptr>(numberOfBytes), data);
		}
		else
		{
			glNamedBufferSubDataEXT(mOpenGLIndirectBuffer, 0, static_cast<GLsizeiptr>(numberOfBytes), data);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
