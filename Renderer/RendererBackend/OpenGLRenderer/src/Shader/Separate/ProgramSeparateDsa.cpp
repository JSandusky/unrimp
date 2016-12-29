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
#include "OpenGLRenderer/Shader/Separate/ProgramSeparateDsa.h"
#include "OpenGLRenderer/Shader/Separate/VertexShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/GeometryShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/FragmentShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/TessellationControlShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/TessellationEvaluationShaderSeparate.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRenderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProgramSeparateDsa::ProgramSeparateDsa(OpenGLRenderer &openGLRenderer, const Renderer::IRootSignature& rootSignature, VertexShaderSeparate *vertexShaderSeparate, TessellationControlShaderSeparate *tessellationControlShaderSeparate, TessellationEvaluationShaderSeparate *tessellationEvaluationShaderSeparate, GeometryShaderSeparate *geometryShaderSeparate, FragmentShaderSeparate *fragmentShaderSeparate) :
		ProgramSeparate(openGLRenderer, rootSignature, vertexShaderSeparate, tessellationControlShaderSeparate, tessellationEvaluationShaderSeparate, geometryShaderSeparate, fragmentShaderSeparate)
	{
		// Nothing here
	}

	ProgramSeparateDsa::~ProgramSeparateDsa()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	void ProgramSeparateDsa::setUniform1f(handle uniformHandle, float value)
	{
		if (static_cast<OpenGLRenderer&>(getRenderer()).getExtensions().isGL_ARB_direct_state_access())
		{
			glProgramUniform1f(mVertexShaderSeparate->getOpenGLShaderProgram(), static_cast<GLint>(uniformHandle), value);
		}
		else
		{
			glProgramUniform1fEXT(mVertexShaderSeparate->getOpenGLShaderProgram(), static_cast<GLint>(uniformHandle), value);
		}
	}

	void ProgramSeparateDsa::setUniform2fv(handle uniformHandle, const float *value)
	{
		if (static_cast<OpenGLRenderer&>(getRenderer()).getExtensions().isGL_ARB_direct_state_access())
		{
			glProgramUniform2fv(mVertexShaderSeparate->getOpenGLShaderProgram(), static_cast<GLint>(uniformHandle), 1, value);
		}
		else
		{
			glProgramUniform2fvEXT(mVertexShaderSeparate->getOpenGLShaderProgram(), static_cast<GLint>(uniformHandle), 1, value);
		}
	}

	void ProgramSeparateDsa::setUniform3fv(handle uniformHandle, const float *value)
	{
		if (static_cast<OpenGLRenderer&>(getRenderer()).getExtensions().isGL_ARB_direct_state_access())
		{
			glProgramUniform3fv(mVertexShaderSeparate->getOpenGLShaderProgram(), static_cast<GLint>(uniformHandle), 1, value);
		}
		else
		{
			glProgramUniform3fvEXT(mVertexShaderSeparate->getOpenGLShaderProgram(), static_cast<GLint>(uniformHandle), 1, value);
		}
	}

	void ProgramSeparateDsa::setUniform4fv(handle uniformHandle, const float *value)
	{
		if (static_cast<OpenGLRenderer&>(getRenderer()).getExtensions().isGL_ARB_direct_state_access())
		{
			glProgramUniform4fv(mVertexShaderSeparate->getOpenGLShaderProgram(), static_cast<GLint>(uniformHandle), 1, value);
		}
		else
		{
			glProgramUniform4fvEXT(mVertexShaderSeparate->getOpenGLShaderProgram(), static_cast<GLint>(uniformHandle), 1, value);
		}
	}

	void ProgramSeparateDsa::setUniformMatrix3fv(handle uniformHandle, const float *value)
	{
		if (static_cast<OpenGLRenderer&>(getRenderer()).getExtensions().isGL_ARB_direct_state_access())
		{
			glProgramUniformMatrix3fv(mVertexShaderSeparate->getOpenGLShaderProgram(), static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
		}
		else
		{
			glProgramUniformMatrix3fvEXT(mVertexShaderSeparate->getOpenGLShaderProgram(), static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
		}
	}

	void ProgramSeparateDsa::setUniformMatrix4fv(handle uniformHandle, const float *value)
	{
		if (static_cast<OpenGLRenderer&>(getRenderer()).getExtensions().isGL_ARB_direct_state_access())
		{
			glProgramUniformMatrix4fv(mVertexShaderSeparate->getOpenGLShaderProgram(), static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
		}
		else
		{
			glProgramUniformMatrix4fvEXT(mVertexShaderSeparate->getOpenGLShaderProgram(), static_cast<GLint>(uniformHandle), 1, GL_FALSE, value);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
