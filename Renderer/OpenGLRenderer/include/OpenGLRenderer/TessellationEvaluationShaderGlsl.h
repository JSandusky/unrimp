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
#ifndef __OPENGLRENDERER_TESSELATIONEVALUATIONSHADER_GLSL_H__
#define __OPENGLRENDERER_TESSELATIONEVALUATIONSHADER_GLSL_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/TessellationEvaluationShader.h"


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
	*    GLSL tessellation evaluation shader ("domain shader" in Direct3D terminology) class
	*/
	class TessellationEvaluationShaderGlsl : public TessellationEvaluationShader
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] sourceCode
		*    Shader ASCII source code, must be valid
		*/
		TessellationEvaluationShaderGlsl(OpenGLRenderer &openGLRenderer, const char *sourceCode);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~TessellationEvaluationShaderGlsl();

		/**
		*  @brief
		*    Return the OpenGL shader
		*
		*  @return
		*    The OpenGL shader, can be zero if no resource is allocated, do not destroy the returned resource (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline unsigned int getOpenGLShader() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	public:
		virtual const char *getShaderLanguageName() const override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		unsigned int mOpenGLShader;	///< OpenGL shader, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/TessellationEvaluationShaderGlsl.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_TESSELATIONEVALUATIONSHADER_GLSL_H__
