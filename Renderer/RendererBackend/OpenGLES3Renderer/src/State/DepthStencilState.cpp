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
#include "OpenGLES3Renderer/State/DepthStencilState.h"
#include "OpenGLES3Renderer/IExtensions.h"	// We need to include this in here for the definitions of the OpenGL ES 3 functions
#include "OpenGLES3Renderer/Mapping.h"
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	DepthStencilState::DepthStencilState(const Renderer::DepthStencilState& depthStencilState) :
		mDepthStencilState(depthStencilState),
		mOpenGLES3DepthMaskEnabled(static_cast<GLboolean>((Renderer::DepthWriteMask::ALL == mDepthStencilState.depthWriteMask) ? GL_TRUE : GL_FALSE)),
		mOpenGLES3DepthFunc(Mapping::getOpenGLES3ComparisonFunc(depthStencilState.depthFunc))
	{
		// Nothing here
	}

	void DepthStencilState::setOpenGLES3DepthStencilStates() const
	{
		// Renderer::DepthStencilState::depthEnable
		if (mDepthStencilState.depthEnable)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		// Renderer::DepthStencilState::depthWriteMask
		glDepthMask(mOpenGLES3DepthMaskEnabled);

		// Renderer::DepthStencilState::depthFunc
		glDepthFunc(static_cast<GLenum>(mOpenGLES3DepthFunc));

		// TODO(co) Map the rest of the depth stencil states
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
