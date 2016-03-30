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
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"
#include "OpenGLES2Renderer/Shader/VertexShaderGlsl.h"
#include "OpenGLES2Renderer/Shader/ShaderLanguageGlsl.h"
#include "OpenGLES2Renderer/IExtensions.h"	// We need to include this in here for the definitions of the OpenGL ES 2 functions


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexShaderGlsl::VertexShaderGlsl(OpenGLES2Renderer &openGLES2Renderer, const uint8_t *, uint32_t) :
		IVertexShader(openGLES2Renderer),
		mOpenGLES2Shader(0)
	{
		// TODO(co) Implement me
		// Nothing to do in here
	}

	VertexShaderGlsl::VertexShaderGlsl(OpenGLES2Renderer &openGLES2Renderer, const char *sourceCode) :
		IVertexShader(openGLES2Renderer),
		mOpenGLES2Shader(ShaderLanguageGlsl::loadShader(GL_VERTEX_SHADER, sourceCode))
	{
		// Nothing to do in here
	}

	VertexShaderGlsl::~VertexShaderGlsl()
	{
		// Destroy the OpenGL ES 2 shader
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteShader(mOpenGLES2Shader);
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
} // OpenGLES2Renderer
