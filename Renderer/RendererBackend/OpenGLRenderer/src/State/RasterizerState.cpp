/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
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
#include "OpenGLRenderer/State/RasterizerState.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RasterizerState::RasterizerState(const Renderer::RasterizerState& rasterizerState) :
		mRasterizerState(rasterizerState)
	{
		// Nothing here
	}

	void RasterizerState::setOpenGLRasterizerStates() const
	{
		// Renderer::RasterizerState::fillMode
		switch (mRasterizerState.fillMode)
		{
			// Wireframe
			case Renderer::FillMode::WIREFRAME:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;

			// Solid
			default:
			case Renderer::FillMode::SOLID:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
		}

		// Renderer::RasterizerState::cullMode
		switch (mRasterizerState.cullMode)
		{
			// No culling
			default:
			case Renderer::CullMode::NONE:
				glDisable(GL_CULL_FACE);
				break;

			// Selects clockwise polygons as front-facing
			case Renderer::CullMode::FRONT:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;

			// Selects counterclockwise polygons as front-facing
			case Renderer::CullMode::BACK:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;
		}

		// Renderer::RasterizerState::frontCounterClockwise
		glFrontFace(static_cast<GLenum>(mRasterizerState.frontCounterClockwise ? GL_CCW : GL_CW));

		// TODO(co) Map the rest of the rasterizer states

		// RasterizerState::depthBias

		// RasterizerState::depthBiasClamp

		// RasterizerState::slopeScaledDepthBias

		// RasterizerState::depthClipEnable
		if (mRasterizerState.depthClipEnable)
		{
			glDisable(GL_DEPTH_CLAMP);
		}
		else
		{
			glEnable(GL_DEPTH_CLAMP);
		}

		// RasterizerState::scissorEnable
		if (mRasterizerState.scissorEnable)
		{
			glEnable(GL_SCISSOR_TEST);
		}
		else
		{
			glDisable(GL_SCISSOR_TEST);
		}

		// RasterizerState::multisampleEnable

		// RasterizerState::antialiasedLineEnable
		if (mRasterizerState.antialiasedLineEnable)
		{
			glEnable(GL_LINE_SMOOTH);
		}
		else
		{
			glDisable(GL_LINE_SMOOTH);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
