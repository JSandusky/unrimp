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
#ifndef __OPENGLRENDERER_VERTEXARRAY_CG_H__
#define __OPENGLRENDERER_VERTEXARRAY_CG_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/VertexArray.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct _CGparameter;
namespace Renderer
{
	struct VertexArrayAttribute;
}
namespace OpenGLRenderer
{
	class ProgramCg;
	class IndexBuffer;
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
	*    OpenGL Cg vertex array class
	*
	*  @remarks
	*    This vertex array implementation has to keep a reference to the program it was created from.
	*    While that's not pretty, it's also not critical. In general the vertex array instance does not
	*    depend on the program instance it was created from. "cgGLDisableClientState()" is using Cg parameters
	*    from the program, in case the program gets destroyed before this vertex array Cg will tell us
	*    "Invalid parameter handle." and will fail to properly disable the client state. Cloning Cg parameters
	*    is possible in general, but not for the required type "CG_VARYING". So, keeping a reference to the
	*    program is the only option to ensure a proper behaviour. It should still be possible to use this
	*    vertex array for multiple programes with the same VAO signature.
	*/
	class VertexArrayCg : public VertexArray
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] programCg
		*    Cg program to use this vertex array for (we keep a reference to it)
		*  @param[in] numberOfAttributes
		*    Number of attributes (position, color, texture coordinate, normal...), having zero attributes is valid
		*  @param[in] attributes
		*    At least "numberOfAttributes" instances of vertex array attributes, can be a null pointer in case there are zero attributes
		*  @param[in] indexBuffer
		*    Optional index buffer to use, can be a null pointer, the vertex array instance keeps a reference to the index buffer
		*/
		VertexArrayCg(ProgramCg &programCg, unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, IndexBuffer *indexBuffer);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VertexArrayCg();

		/**
		*  @brief
		*    Enable OpenGL Cg vertex attribute arrays
		*/
		void enableOpenGLVertexAttribArrays();

		/**
		*  @brief
		*    Disable OpenGL Cg vertex attribute arrays
		*/
		void disableOpenGLVertexAttribArrays();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		ProgramCg						*mProgramCg;			///< Cg program to use this vertex array for (we keep a reference to it), always valid
		unsigned int					 mNumberOfAttributes;	///< Number of attributes (position, color, texture coordinate, normal...), having zero attributes is valid
		Renderer::VertexArrayAttribute  *mAttributes;			///< At least nNumberOfAttributes instances of vertex array attributes, can be a null pointer in case there are zero attributes
		_CGparameter 				   **mCgParameters;			///< Cg parameters, can be a null pointer in case there are zero attributes


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_VERTEXARRAY_CG_H__
