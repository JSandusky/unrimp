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
#include "OpenGLRenderer/Shader/Monolithic/TessellationControlShaderMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/ShaderLanguageMonolithic.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/Extensions.h"

#include <Renderer/IRenderer.h>
#include <Renderer/IAllocator.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TessellationControlShaderMonolithic::TessellationControlShaderMonolithic(OpenGLRenderer& openGLRenderer, const char* sourceCode) :
		ITessellationControlShader(static_cast<Renderer::IRenderer&>(openGLRenderer)),
		mOpenGLShader(ShaderLanguageMonolithic::loadShaderFromSourcecode(openGLRenderer, GL_TESS_CONTROL_SHADER, sourceCode))
	{
		// Nothing here
	}

	TessellationControlShaderMonolithic::~TessellationControlShaderMonolithic()
	{
		// Destroy the OpenGL shader
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteObjectARB(mOpenGLShader);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void TessellationControlShaderMonolithic::setDebugName(const char* name)
		{
			// Valid OpenGL shader and "GL_KHR_debug"-extension available?
			if (0 != mOpenGLShader && static_cast<OpenGLRenderer&>(getRenderer()).getExtensions().isGL_KHR_debug())
			{
				glObjectLabel(GL_SHADER, mOpenGLShader, -1, name);
			}
		}
	#else
		void TessellationControlShaderMonolithic::setDebugName(const char*)
		{
			// Nothing here
		}
	#endif


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char* TessellationControlShaderMonolithic::getShaderLanguageName() const
	{
		return ShaderLanguageMonolithic::NAME;
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void TessellationControlShaderMonolithic::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), TessellationControlShaderMonolithic, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
