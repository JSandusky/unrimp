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
#include <Renderer/Buffer/IIndexBuffer.h>
#include <Renderer/Buffer/IndexBufferTypes.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{
	class OpenGLRenderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract OpenGL index buffer object (IBO, "element array buffer" in OpenGL terminology) interface
	*/
	class IndexBuffer : public Renderer::IIndexBuffer
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		virtual ~IndexBuffer();

		/**
		*  @brief
		*    Return the OpenGL element array buffer
		*
		*  @return
		*    The OpenGL element array buffer, can be zero if no resource is allocated, do not destroy the returned resource (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline uint32_t getOpenGLElementArrayBuffer() const;

		/**
		*  @brief
		*    Return the OpenGL element array buffer data type
		*
		*  @return
		*    The OpenGL element array buffer data type (type "GLenum" not used in here in order to keep the header slim)
		*/
		inline uint32_t getOpenGLType() const;

		/**
		*  @brief
		*    Return the number of bytes of an index
		*
		*  @return
		*    The number of bytes of an index
		*/
		inline uint32_t getIndexSizeInBytes() const;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] indexBufferFormat
		*    Index buffer data format
		*/
		IndexBuffer(OpenGLRenderer &openGLRenderer, Renderer::IndexBufferFormat::Enum indexBufferFormat);


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		uint32_t mOpenGLElementArrayBuffer;	///< OpenGL element array buffer, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)
		uint32_t mOpenGLType;				///< OpenGL element array buffer data type (type "GLenum" not used in here in order to keep the header slim)
		uint32_t mIndexSizeInBytes;			///< Number of bytes of an index


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/Buffer/IndexBuffer.inl"
