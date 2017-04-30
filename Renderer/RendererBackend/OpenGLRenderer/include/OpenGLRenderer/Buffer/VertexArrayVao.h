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
#include "OpenGLRenderer/Buffer/VertexArray.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	struct VertexArrayVertexBuffer;
}
namespace OpenGLRenderer
{
	class IndexBuffer;
	class VertexBuffer;
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
	*    Abstract OpenGL vertex array interface, effective vertex array object (VAO)
	*/
	class VertexArrayVao : public VertexArray
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VertexArrayVao();

		/**
		*  @brief
		*    Return the OpenGL vertex array
		*
		*  @return
		*    The OpenGL vertex array, can be zero if no resource is allocated, do not destroy the returned resource (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline uint32_t getOpenGLVertexArray() const;


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
		*  @param[in] numberOfVertexBuffers
		*    Number of vertex buffers, having zero vertex buffers is valid
		*  @param[in] vertexBuffers
		*    At least numberOfVertexBuffers instances of vertex array vertex buffers, can be a null pointer in case there are zero vertex buffers, the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] indexBuffer
		*    Optional index buffer to use, can be a null pointer, the vertex array instance keeps a reference to the index buffer
		*/
		VertexArrayVao(OpenGLRenderer &openGLRenderer, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, IndexBuffer *indexBuffer);


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		uint32_t	   mOpenGLVertexArray;		///< OpenGL vertex array ("container" object, not shared between OpenGL contexts), can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)
		uint32_t	   mNumberOfVertexBuffers;	///< Number of vertex buffers
		VertexBuffer **mVertexBuffers;			///< Vertex buffers (we keep a reference to it) used by this vertex array, can be a null pointer


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit VertexArrayVao(const VertexArrayVao& source) = delete;
		VertexArrayVao& operator =(const VertexArrayVao& source) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/Buffer/VertexArrayVao.inl"
