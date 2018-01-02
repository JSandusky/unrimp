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
#include "OpenGLES3Renderer/State/RasterizerState.h"
#include "OpenGLES3Renderer/IExtensions.h"	// We need to include this in here for the definitions of the OpenGL ES 3 functions
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RasterizerState::RasterizerState(const Renderer::RasterizerState& rasterizerState) :
		mRasterizerState(rasterizerState),
		mOpenGLES3FrontFaceMode(static_cast<GLenum>(mRasterizerState.frontCounterClockwise ? GL_CCW : GL_CW))
	{
		// Nothing here
	}

	void RasterizerState::setOpenGLES3RasterizerStates() const
	{
		// Renderer::RasterizerState::fillMode
		switch (mRasterizerState.fillMode)
		{
			// Wireframe
			case Renderer::FillMode::WIREFRAME:
				// OpenGL ES 3 has no support for polygon mode
				// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;

			// Solid
			default:
			case Renderer::FillMode::SOLID:
				// OpenGL ES 3 has no support for polygon mode
				// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
		glFrontFace(mOpenGLES3FrontFaceMode);

		// TODO(co) Map the rest of the rasterizer states

		// RasterizerState::depthBias

		// RasterizerState::depthBiasClamp

		// RasterizerState::slopeScaledDepthBias

		// RasterizerState::depthClipEnable
			/*
			TODO(co) OpenGL has the "GL_ARB_depth_clamp"-extension
			if (mRasterizerState.depthClipEnable)
			{
				glDisable(GL_DEPTH_CLAMP);
			}
			else
			{
				glEnable(GL_DEPTH_CLAMP);
			}

			Odd possible workaround mentioned at https://stackoverflow.com/questions/5960757/how-to-emulate-gl-depth-clamp-nv
			"
			You can emulate ARB_depth_clamp by using a separate varying for the z-component.

			Vertex Shader:

			varying float z;
			void main()
			{
				gl_Position = ftransform();

				// transform z to window coordinates
				z = gl_Position.z / gl_Position.w;
				z = (gl_DepthRange.diff * z + gl_DepthRange.near + gl_DepthRange.far) * 0.5;

				// prevent z-clipping
				gl_Position.z = 0.0;
			}

			Fragment shader:

			varying float z;
			void main()
			{
				gl_FragColor = vec4(vec3(z), 1.0);
				gl_FragDepth = clamp(z, 0.0, 1.0);
			}
			"
			*/

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
		// -> Anti-aliased lines are not supported by OpenGL ES 3
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
