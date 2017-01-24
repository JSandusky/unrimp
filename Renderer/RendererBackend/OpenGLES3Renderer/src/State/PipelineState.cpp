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
#include "OpenGLES2Renderer/State/PipelineState.h"
#include "OpenGLES2Renderer/State/BlendState.h"
#include "OpenGLES2Renderer/State/RasterizerState.h"
#include "OpenGLES2Renderer/State/DepthStencilState.h"
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"
#include "OpenGLES2Renderer/Shader/ProgramGlsl.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	PipelineState::PipelineState(OpenGLES2Renderer &openGLES2Renderer, const Renderer::PipelineState& pipelineState) :
		IPipelineState(openGLES2Renderer),
		mProgram(pipelineState.program),
		mRasterizerState(new RasterizerState(pipelineState.rasterizerState)),
		mDepthStencilState(new DepthStencilState(pipelineState.depthStencilState)),
		mBlendState(new BlendState(pipelineState.blendState))
	{
		// Add a reference to the given program
		mProgram->addReference();
	}

	PipelineState::~PipelineState()
	{
		// Destroy states
		delete mRasterizerState;
		delete mDepthStencilState;
		delete mBlendState;

		// Release the program reference
		if (nullptr != mProgram)
		{
			mProgram->releaseReference();
		}
	}

	void PipelineState::bindPipelineState() const
	{
		static_cast<OpenGLES2Renderer&>(getRenderer()).setProgram(mProgram);

		// Set the OpenGL ES 2 rasterizer state
		mRasterizerState->setOpenGLES2RasterizerStates();

		// Set OpenGL ES 2 depth stencil state
		mDepthStencilState->setOpenGLES2DepthStencilStates();

		// Set OpenGL ES 2 blend state
		mBlendState->setOpenGLES2BlendStates();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
