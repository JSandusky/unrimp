/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "OpenGLRenderer/Shader/ProgramGlslDsa.h"
#include "OpenGLRenderer/Extensions.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProgramGlslDsa::ProgramGlslDsa(OpenGLRenderer &openGLRenderer, const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, VertexShaderGlsl *vertexShaderGlsl, TessellationControlShaderGlsl *tessellationControlShaderGlsl, TessellationEvaluationShaderGlsl *tessellationEvaluationShaderGlsl, GeometryShaderGlsl *geometryShaderGlsl, FragmentShaderGlsl *fragmentShaderGlsl) :
		ProgramGlsl(openGLRenderer, rootSignature, vertexAttributes, vertexShaderGlsl, tessellationControlShaderGlsl, tessellationEvaluationShaderGlsl, geometryShaderGlsl, fragmentShaderGlsl)
	{
		// Nothing to do in here
	}

	ProgramGlslDsa::~ProgramGlslDsa()
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	uint32_t ProgramGlslDsa::setTextureUnit(handle uniformHandle, uint32_t unit)
	{
		// OpenGL/GLSL is not automatically assigning texture units to samplers, so, we have to take over this job
		// -> Explicit binding points ("layout(binding = 0)" in GLSL shader) requires OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension
		glProgramUniform1iEXT(mOpenGLProgram, static_cast<GLint>(uniformHandle), static_cast<GLint>(unit));
		return unit;
	}

	void ProgramGlslDsa::setUniform1f(handle uniformHandle, float value)
	{
		glProgramUniform1fEXT(mOpenGLProgram, static_cast<GLint>(uniformHandle), value);
	}

	void ProgramGlslDsa::setUniform2fv(handle uniformHandle, const float *value)
	{
		glProgramUniform2fvEXT(mOpenGLProgram, static_cast<GLint>(uniformHandle), 1, value);
	}

	void ProgramGlslDsa::setUniform3fv(handle uniformHandle, const float *value)
	{
		glProgramUniform3fvEXT(mOpenGLProgram, static_cast<GLint>(uniformHandle), 1, value);
	}

	void ProgramGlslDsa::setUniform4fv(handle uniformHandle, const float *value)
	{
		glProgramUniform4fvEXT(mOpenGLProgram, static_cast<GLint>(uniformHandle), 1, value);
	}

	void ProgramGlslDsa::setUniformMatrix3fv(handle uniformHandle, const float *value)
	{
		glProgramUniformMatrix3fvEXT(mOpenGLProgram, static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
	}

	void ProgramGlslDsa::setUniformMatrix4fv(handle uniformHandle, const float *value)
	{
		glProgramUniformMatrix4fvEXT(mOpenGLProgram, static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
