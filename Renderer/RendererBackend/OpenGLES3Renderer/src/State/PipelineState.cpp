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
#include "OpenGLES3Renderer/State/PipelineState.h"
#include "OpenGLES3Renderer/Shader/ProgramGlsl.h"
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"
#include "OpenGLES3Renderer/Mapping.h"

#include <Renderer/RenderTarget/IRenderPass.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	PipelineState::PipelineState(OpenGLES3Renderer& openGLES3Renderer, const Renderer::PipelineState& pipelineState) :
		IPipelineState(openGLES3Renderer),
		mOpenGLES3PrimitiveTopology(Mapping::getOpenGLES3Type(pipelineState.primitiveTopology)),
		mProgram(pipelineState.program),
		mRenderPass(pipelineState.renderPass),
		mRasterizerState(pipelineState.rasterizerState),
		mDepthStencilState(pipelineState.depthStencilState),
		mBlendState(pipelineState.blendState)
	{
		// Add a reference to the given program and render pass
		mProgram->addReference();
		mRenderPass->addReference();
	}

	PipelineState::~PipelineState()
	{
		// Release the program and render pass reference
		mProgram->releaseReference();
		mRenderPass->releaseReference();
	}

	void PipelineState::bindPipelineState() const
	{
		static_cast<OpenGLES3Renderer&>(getRenderer()).setProgram(mProgram);

		// Set the OpenGL ES 3 rasterizer state
		mRasterizerState.setOpenGLES3RasterizerStates();

		// Set OpenGL ES 3 depth stencil state
		mDepthStencilState.setOpenGLES3DepthStencilStates();

		// Set OpenGL ES 3 blend state
		mBlendState.setOpenGLES3BlendStates();
	}

	const Renderer::RasterizerState& PipelineState::getRasterizerState() const
	{
		return mRasterizerState.getRasterizerState();
	}

	const Renderer::DepthStencilState& PipelineState::getDepthStencilState() const
	{
		return mDepthStencilState.getDepthStencilState();
	}

	const Renderer::BlendState& PipelineState::getBlendState() const
	{
		return mBlendState.getBlendState();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
