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
#include "OpenGLRenderer/Detail/BlendState.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"
#include "OpenGLRenderer/Mapping.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	BlendState::BlendState(const Renderer::BlendState &blendState) :
		mBlendState(blendState),
		mOpenGLSrcBlend(Mapping::getOpenGLBlendType(mBlendState.renderTarget[0].srcBlend)),
		mOpenGLDstBlend(Mapping::getOpenGLBlendType(mBlendState.renderTarget[0].destBlend))
	{
		// Nothing to do in here
	}

	void BlendState::setOpenGLBlendStates() const
	{
		// TODO(co) Add support for blend state per render target
		if (mBlendState.renderTarget[0].blendEnable)
		{
			glEnable(GL_BLEND);
			glBlendFunc(mOpenGLSrcBlend, mOpenGLDstBlend);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		// TODO(co) Map the rest of the blend states
		// GL_EXT_blend_func_separate
		// (GL_EXT_blend_equation_separate)
		// GL_EXT_blend_color
		// GL_EXT_blend_minmax
		// GL_EXT_blend_subtract
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
