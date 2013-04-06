/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
#ifndef __OPENGLES2RENDERER_RASTERIZERSTATE_H__
#define __OPENGLES2RENDERER_RASTERIZERSTATE_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/IRasterizerState.h>
#include <Renderer/RasterizerStateTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Forward declarations                                  ]
	//[-------------------------------------------------------]
	class OpenGLES2Renderer;


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    OpenGL ES 2 rasterizer state class
	*/
	class RasterizerState : public Renderer::IRasterizerState
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
		*  @param[in] rasterizerState
		*    Rasterizer state to use
		*/
		RasterizerState(OpenGLES2Renderer &openGLES2Renderer, const Renderer::RasterizerState &rasterizerState);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~RasterizerState();

		/**
		*  @brief
		*    Return the rasterizer state
		*
		*  @return
		*    The rasterizer state
		*/
		inline const Renderer::RasterizerState &getRasterizerState() const;

		/**
		*  @brief
		*    Set the OpenGL ES 2 rasterizer states
		*/
		void setOpenGLES2RasterizerStates() const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Renderer::RasterizerState mRasterizerState;	///< Rasterizer state


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLES2Renderer/RasterizerState.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLES2RENDERER_RASTERIZERSTATE_H__
