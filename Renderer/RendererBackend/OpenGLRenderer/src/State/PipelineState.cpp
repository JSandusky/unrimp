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
#include "OpenGLRenderer/State/PipelineState.h"
#include "OpenGLRenderer/State/BlendState.h"
#include "OpenGLRenderer/State/RasterizerState.h"
#include "OpenGLRenderer/State/DepthStencilState.h"
#include "OpenGLRenderer/Shader/Monolithic/ProgramMonolithic.h"
#include "OpenGLRenderer/OpenGLRenderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	PipelineState::PipelineState(OpenGLRenderer &openGLRenderer, const Renderer::PipelineState& pipelineState) :
		IPipelineState(openGLRenderer),
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
		static_cast<OpenGLRenderer&>(getRenderer()).setProgram(mProgram);

		// Set the OpenGL rasterizer state
		mRasterizerState->setOpenGLRasterizerStates();

		// Set OpenGL depth stencil state
		mDepthStencilState->setOpenGLDepthStencilStates();

		// Set OpenGL blend state
		mBlendState->setOpenGLBlendStates();
	}

	const Renderer::RasterizerState& PipelineState::getRasterizerState() const
	{
		return mRasterizerState->getRasterizerState();
	}

	const Renderer::DepthStencilState& PipelineState::getDepthStencilState() const
	{
		return mDepthStencilState->getDepthStencilState();
	}

	const Renderer::BlendState& PipelineState::getBlendState() const
	{
		return mBlendState->getBlendState();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
