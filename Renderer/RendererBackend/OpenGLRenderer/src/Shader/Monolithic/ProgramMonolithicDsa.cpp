/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "OpenGLRenderer/Shader/Monolithic/ProgramMonolithicDsa.h"
#include "OpenGLRenderer/Extensions.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProgramMonolithicDsa::ProgramMonolithicDsa(OpenGLRenderer &openGLRenderer, const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, VertexShaderMonolithic *vertexShaderMonolithic, TessellationControlShaderMonolithic *tessellationControlShaderMonolithic, TessellationEvaluationShaderMonolithic *tessellationEvaluationShaderMonolithic, GeometryShaderMonolithic *geometryShaderMonolithic, FragmentShaderMonolithic *fragmentShaderMonolithic) :
		ProgramMonolithic(openGLRenderer, rootSignature, vertexAttributes, vertexShaderMonolithic, tessellationControlShaderMonolithic, tessellationEvaluationShaderMonolithic, geometryShaderMonolithic, fragmentShaderMonolithic)
	{
		// Nothing to do in here
	}

	ProgramMonolithicDsa::~ProgramMonolithicDsa()
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	void ProgramMonolithicDsa::setUniform1f(handle uniformHandle, float value)
	{
		glProgramUniform1fEXT(mOpenGLProgram, static_cast<GLint>(uniformHandle), value);
	}

	void ProgramMonolithicDsa::setUniform2fv(handle uniformHandle, const float *value)
	{
		glProgramUniform2fvEXT(mOpenGLProgram, static_cast<GLint>(uniformHandle), 1, value);
	}

	void ProgramMonolithicDsa::setUniform3fv(handle uniformHandle, const float *value)
	{
		glProgramUniform3fvEXT(mOpenGLProgram, static_cast<GLint>(uniformHandle), 1, value);
	}

	void ProgramMonolithicDsa::setUniform4fv(handle uniformHandle, const float *value)
	{
		glProgramUniform4fvEXT(mOpenGLProgram, static_cast<GLint>(uniformHandle), 1, value);
	}

	void ProgramMonolithicDsa::setUniformMatrix3fv(handle uniformHandle, const float *value)
	{
		glProgramUniformMatrix3fvEXT(mOpenGLProgram, static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
	}

	void ProgramMonolithicDsa::setUniformMatrix4fv(handle uniformHandle, const float *value)
	{
		glProgramUniformMatrix4fvEXT(mOpenGLProgram, static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
