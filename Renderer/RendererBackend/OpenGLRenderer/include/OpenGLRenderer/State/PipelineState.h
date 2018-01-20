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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/State/IPipelineState.h>

#include "OpenGLRenderer/State/BlendState.h"
#include "OpenGLRenderer/State/RasterizerState.h"
#include "OpenGLRenderer/State/DepthStencilState.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IProgram;
	struct BlendState;
	struct PipelineState;
	struct RasterizerState;
	struct DepthStencilState;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    OpenGL pipeline state class
	*/
	class PipelineState final : public Renderer::IPipelineState
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] pipelineState
		*    Pipeline state to use
		*/
		PipelineState(OpenGLRenderer& openGLRenderer, const Renderer::PipelineState& pipelineState);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~PipelineState() override;

		/**
		*  @brief
		*    Return the OpenGL primitive topology describing the type of primitive to render
		*
		*  @return
		*    OpenGL primitive topology describing the type of primitive to render (type "GLenum" not used in here in order to keep the header slim)
		*/
		inline uint32_t getOpenGLPrimitiveTopology() const;

		/**
		*  @brief
		*    Return the number of vertices per patch
		*
		*  @return
		*    Return the number of vertices per patch (type "GLint" not used in here in order to keep the header slim)
		*/
		inline int getNumberOfVerticesPerPatch() const;

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
		uint32_t			   mOpenGLPrimitiveTopology;	///< OpenGL primitive topology describing the type of primitive to render (type "GLenum" not used in here in order to keep the header slim)
		int					   mNumberOfVerticesPerPatch;	///< Number of vertices per patch (type "GLint" not used in here in order to keep the header slim)
		Renderer::IProgram*	   mProgram;
		Renderer::IRenderPass* mRenderPass;
		RasterizerState		   mRasterizerState;
		DepthStencilState	   mDepthStencilState;
		BlendState			   mBlendState;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/State/PipelineState.inl"
