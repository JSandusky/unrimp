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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"
#include "OpenGLES3Renderer/Shader/VertexShaderGlsl.h"
#include "OpenGLES3Renderer/Shader/ShaderLanguageGlsl.h"
#include "OpenGLES3Renderer/IExtensions.h"	// We need to include this in here for the definitions of the OpenGL ES 3 functions

#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexShaderGlsl::VertexShaderGlsl(OpenGLES3Renderer& openGLES3Renderer, const char* sourceCode) :
		IVertexShader(openGLES3Renderer),
		mOpenGLES3Shader(ShaderLanguageGlsl::loadShaderFromSourcecode(openGLES3Renderer, GL_VERTEX_SHADER, sourceCode))
	{
		// Nothing here
	}

	VertexShaderGlsl::~VertexShaderGlsl()
	{
		// Destroy the OpenGL ES 3 shader
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteShader(mOpenGLES3Shader);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void VertexShaderGlsl::setDebugName(const char* name)
		{
			// Valid OpenGL ES 3 shader and "GL_KHR_debug"-extension available?
			if (0 != mOpenGLES3Shader && static_cast<OpenGLES3Renderer&>(getRenderer()).getOpenGLES3Context().getExtensions().isGL_KHR_debug())
			{
				glObjectLabelKHR(GL_SHADER_KHR, mOpenGLES3Shader, -1, name);
			}
		}
	#else
		void VertexShaderGlsl::setDebugName(const char*)
		{
			// Nothing here
		}
	#endif


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char* VertexShaderGlsl::getShaderLanguageName() const
	{
		return ShaderLanguageGlsl::NAME;
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void VertexShaderGlsl::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), VertexShaderGlsl, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
