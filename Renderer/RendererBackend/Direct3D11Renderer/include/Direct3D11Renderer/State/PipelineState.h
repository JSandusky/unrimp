/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct ID3D11InputLayout;
struct ID3D11DeviceContext;
namespace Renderer
{
	class IProgram;
	struct PipelineState;
}
namespace Direct3D11Renderer
{
	class BlendState;
	class RasterizerState;
	class DepthStencilState;
	class Direct3D11Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 11 pipeline state class
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
		*  @param[in] direct3D11Renderer
		*    Owner Direct3D 11 renderer instance
		*  @param[in] pipelineState
		*    Pipeline state to use
		*/
		PipelineState(Direct3D11Renderer &direct3D11Renderer, const Renderer::PipelineState &pipelineState);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~PipelineState();

		/**
		*  @brief
		*    Return the Direct3D 11 input layout
		*
		*  @return
		*    Direct3D 11 input layout instance, can be a null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D11InputLayout *getD3D11InputLayout() const;

		/**
		*  @brief
		*    Bind the pipeline state
		*/
		void bindPipelineState() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char *name) override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Renderer::IProgram*  mProgram;
		ID3D11DeviceContext* mD3D11DeviceContext;	///< The Direct3D 11 device context instance (we keep a reference to it), null pointer on horrible error (so we don't check)
		ID3D11InputLayout*   mD3D11InputLayout;		///< Direct3D 11 input layout, can be a null pointer
		RasterizerState*	 mRasterizerState;
		DepthStencilState*	 mDepthStencilState;
		BlendState*			 mBlendState;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/State/PipelineState.inl"
