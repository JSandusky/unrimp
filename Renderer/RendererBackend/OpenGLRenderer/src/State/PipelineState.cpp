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
#include "OpenGLRenderer/State/PipelineState.h"
#include "OpenGLRenderer/State/BlendState.h"
#include "OpenGLRenderer/State/RasterizerState.h"
#include "OpenGLRenderer/State/DepthStencilState.h"
#include "OpenGLRenderer/Shader/Monolithic/ProgramMonolithic.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/Mapping.h"

#include <Renderer/IAssert.h>
#include <Renderer/RenderTarget/IRenderPass.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	PipelineState::PipelineState(OpenGLRenderer& openGLRenderer, const Renderer::PipelineState& pipelineState) :
		IPipelineState(openGLRenderer),
		mOpenGLPrimitiveTopology(0xFFFF),	// Unknown default setting
		mNumberOfVerticesPerPatch(0),
		mProgram(pipelineState.program),
		mRenderPass(pipelineState.renderPass),
		mRasterizerState(new RasterizerState(pipelineState.rasterizerState)),
		mDepthStencilState(new DepthStencilState(pipelineState.depthStencilState)),
		mBlendState(new BlendState(pipelineState.blendState))
	{
		// Tessellation support: Up to 32 vertices per patch are supported "Renderer::PrimitiveTopology::PATCH_LIST_1" ... "Renderer::PrimitiveTopology::PATCH_LIST_32"
		if (pipelineState.primitiveTopology >= Renderer::PrimitiveTopology::PATCH_LIST_1)
		{
			// Use tessellation

			// Get number of vertices that will be used to make up a single patch primitive
			// -> There's no need to check for the "GL_ARB_tessellation_shader" extension, it's there if "Renderer::Capabilities::maximumNumberOfPatchVertices" is not 0
			const int numberOfVerticesPerPatch = static_cast<int>(pipelineState.primitiveTopology) - static_cast<int>(Renderer::PrimitiveTopology::PATCH_LIST_1) + 1;
			if (numberOfVerticesPerPatch <= static_cast<int>(openGLRenderer.getCapabilities().maximumNumberOfPatchVertices))
			{
				// Set number of vertices that will be used to make up a single patch primitive
				mNumberOfVerticesPerPatch = numberOfVerticesPerPatch;

				// Set OpenGL primitive topology
				mOpenGLPrimitiveTopology = GL_PATCHES;
			}
			else
			{
				// Error!
				RENDERER_ASSERT(openGLRenderer.getContext(), false, "Invalid number of OpenGL vertices per patch");
			}
		}
		else
		{
			// Do not use tessellation

			// Map and backup the set OpenGL primitive topology
			mOpenGLPrimitiveTopology = Mapping::getOpenGLType(pipelineState.primitiveTopology);
		}

		// Add a reference to the given program and render pass
		mProgram->addReference();
		mRenderPass->addReference();
	}

	PipelineState::~PipelineState()
	{
		// Destroy states
		delete mRasterizerState;
		delete mDepthStencilState;
		delete mBlendState;

		// Release the program and render pass reference
		mProgram->releaseReference();
		mRenderPass->releaseReference();
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
