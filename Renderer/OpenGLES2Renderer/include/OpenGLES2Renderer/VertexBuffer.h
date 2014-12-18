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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __OPENGLES2RENDERER_VERTEXBUFFER_H__
#define __OPENGLES2RENDERER_VERTEXBUFFER_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/BufferTypes.h>
#include <Renderer/IVertexBuffer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{
	class OpenGLES2Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    OpenGL ES 2 vertex buffer object (VBO, "array buffer" in OpenGL terminology) class
	*/
	class VertexBuffer : public Renderer::IVertexBuffer
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLES2Renderer
		*    Owner OpenGL ES 2 renderer instance
		*  @param[in] numberOfBytes
		*    Number of bytes within the index buffer, must be valid
		*  @param[in] data
		*    Vertex buffer data, can be a null pointer (empty buffer)
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*/
		VertexBuffer(OpenGLES2Renderer &openGLES2Renderer, unsigned int numberOfBytes, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VertexBuffer();

		/**
		*  @brief
		*    Return the OpenGL ES 2 array buffer
		*
		*  @return
		*    The OpenGL ES 2 array buffer, can be zero if no resource is allocated, do not destroy the returned resource (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline unsigned int getOpenGLES2ArrayBuffer() const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		unsigned int mOpenGLES2ArrayBuffer;	///< OpenGL ES 2 array buffer, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLES2Renderer/VertexBuffer.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLES2RENDERER_VERTEXBUFFER_H__
