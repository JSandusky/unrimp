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
#include "OpenGLRenderer/Shader/VertexShaderGlsl.h"
#include "OpenGLRenderer/Shader/ShaderLanguageGlsl.h"
#include "OpenGLRenderer/Extensions.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexShaderGlsl::VertexShaderGlsl(OpenGLRenderer &openGLRenderer, const uint8_t *, uint32_t) :
		VertexShader(openGLRenderer),
		mOpenGLShader(0)
	{
		// TODO(co) Implement me
		// Nothing to do in here
	}

	VertexShaderGlsl::VertexShaderGlsl(OpenGLRenderer &openGLRenderer, const char *sourceCode) :
		VertexShader(openGLRenderer),
		mOpenGLShader(ShaderLanguageGlsl::loadShader(GL_VERTEX_SHADER_ARB, sourceCode))
	{
		// Nothing to do in here
	}

	VertexShaderGlsl::~VertexShaderGlsl()
	{
		// Destroy the OpenGL shader
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteObjectARB(mOpenGLShader);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char *VertexShaderGlsl::getShaderLanguageName() const
	{
		return ShaderLanguageGlsl::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
