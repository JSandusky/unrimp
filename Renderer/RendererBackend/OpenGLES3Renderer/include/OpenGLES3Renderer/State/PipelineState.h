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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/State/IPipelineState.h>

#include "OpenGLES3Renderer/State/BlendState.h"
#include "OpenGLES3Renderer/State/RasterizerState.h"
#include "OpenGLES3Renderer/State/DepthStencilState.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IProgram;
	struct PipelineState;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    OpenGL ES 3 pipeline state class
	*/
	class PipelineState : public Renderer::IPipelineState
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLES3Renderer
		*    Owner OpenGL ES 3 renderer instance
		*  @param[in] pipelineState
		*    Pipeline state to use
		*/
		PipelineState(OpenGLES3Renderer& openGLES3Renderer, const Renderer::PipelineState& pipelineState);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~PipelineState() override;

		/**
		*  @brief
		*    Return the OpenGL ES 3 primitive topology describing the type of primitive to render
		*
		*  @return
		*    OpenGL ES 3 primitive topology describing the type of primitive to render (type "GLenum" not used in here in order to keep the header slim)
		*/
		inline uint32_t getOpenGLES3PrimitiveTopology() const;

		/**
		*  @brief
		*    Bind the pipeline state
		*/
		void bindPipelineState() const;

		//[-------------------------------------------------------]
		//[ Detail state access                                   ]
		//[-------------------------------------------------------]
		const Renderer::RasterizerState& getRasterizerState() const;
		const Renderer::DepthStencilState& getDepthStencilState() const;
		const Renderer::BlendState& getBlendState() const;


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	protected:
		virtual void selfDestruct() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit PipelineState(const PipelineState& source) = delete;
		PipelineState& operator =(const PipelineState& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		uint32_t			   mOpenGLES3PrimitiveTopology;	///< OpenGL ES 3 primitive topology describing the type of primitive to render (type "GLenum" not used in here in order to keep the header slim)
		Renderer::IProgram*    mProgram;
		Renderer::IRenderPass* mRenderPass;
		RasterizerState		   mRasterizerState;
		DepthStencilState	   mDepthStencilState;
		BlendState			   mBlendState;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLES3Renderer/State/PipelineState.inl"
