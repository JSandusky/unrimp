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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/Buffer/BufferTypes.h>
#include <Renderer/Buffer/IVertexBuffer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{
	class OpenGLES3Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    OpenGL ES 3 vertex buffer object (VBO, "array buffer" in OpenGL terminology) class
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
		*  @param[in] openGLES3Renderer
		*    Owner OpenGL ES 3 renderer instance
		*  @param[in] numberOfBytes
		*    Number of bytes within the vertex buffer, must be valid
		*  @param[in] data
		*    Vertex buffer data, can be a null pointer (empty buffer)
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*/
		VertexBuffer(OpenGLES3Renderer& openGLES3Renderer, uint32_t numberOfBytes, const void* data = nullptr, Renderer::BufferUsage bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VertexBuffer() override;

		/**
		*  @brief
		*    Return the OpenGL ES 3 array buffer
		*
		*  @return
		*    The OpenGL ES 3 array buffer, can be zero if no resource is allocated, do not destroy the returned resource (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline uint32_t getOpenGLES3ArrayBuffer() const;

		inline uint32_t getBufferSize() const;


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	protected:
		virtual void selfDestruct() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit VertexBuffer(const VertexBuffer& source) = delete;
		VertexBuffer& operator =(const VertexBuffer& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		uint32_t mOpenGLES3ArrayBuffer;	///< OpenGL ES 3 array buffer, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)
		uint32_t mBufferSize;			///< Holds the size of the buffer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLES3Renderer/Buffer/VertexBuffer.inl"
