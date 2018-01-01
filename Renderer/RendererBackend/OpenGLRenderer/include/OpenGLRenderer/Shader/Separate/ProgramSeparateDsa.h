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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/Shader/Separate/ProgramSeparate.h"


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
	*    Separate program class, effective direct state access (DSA)
	*/
	class ProgramSeparateDsa : public ProgramSeparate
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
		*  @param[in] rootSignature
		*    Root signature
		*  @param[in] vertexShaderSeparate
		*    Vertex shader the program is using, can be a null pointer
		*  @param[in] tessellationControlShaderSeparate
		*    Tessellation control shader the program is using, can be a null pointer
		*  @param[in] tessellationEvaluationShaderSeparate
		*    Tessellation evaluation shader the program is using, can be a null pointer
		*  @param[in] geometryShaderSeparate
		*    Geometry shader the program is using, can be a null pointer
		*  @param[in] fragmentShaderSeparate
		*    Fragment shader the program is using, can be a null pointer
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required
		*/
		ProgramSeparateDsa(OpenGLRenderer& openGLRenderer, const Renderer::IRootSignature& rootSignature, VertexShaderSeparate* vertexShaderSeparate, TessellationControlShaderSeparate* tessellationControlShaderSeparate, TessellationEvaluationShaderSeparate* tessellationEvaluationShaderSeparate, GeometryShaderSeparate* geometryShaderSeparate, FragmentShaderSeparate* fragmentShaderSeparate);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ProgramSeparateDsa() override;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	public:
		virtual void setUniform1f(handle uniformHandle, float value) override;
		virtual void setUniform2fv(handle uniformHandle, const float* value) override;
		virtual void setUniform3fv(handle uniformHandle, const float* value) override;
		virtual void setUniform4fv(handle uniformHandle, const float* value) override;
		virtual void setUniformMatrix3fv(handle uniformHandle, const float* value) override;
		virtual void setUniformMatrix4fv(handle uniformHandle, const float* value) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit ProgramSeparateDsa(const ProgramSeparateDsa& source) = delete;
		ProgramSeparateDsa& operator =(const ProgramSeparateDsa& source) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
