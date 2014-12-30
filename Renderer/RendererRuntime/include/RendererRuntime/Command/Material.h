/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	// TODO(co) Work in progress
	struct Material
	{
		Renderer::IProgramPtr			program;
		Renderer::IRasterizerStatePtr	rasterizerState;	///< Rasterizer state (rasterizer stage (RS))
		Renderer::IDepthStencilStatePtr	depthStencilState;	///< Depth stencil state (output-merger (OM) stage)
		Renderer::IBlendStatePtr		blendState;			///< Blend state (output-merger (OM) stage)

		void clear()
		{
			program = nullptr;
			rasterizerState = nullptr;
			depthStencilState = nullptr;
			blendState = nullptr;
		}

		void bind(Renderer::IRenderer& renderer)
		{
			renderer.setProgram(program);
			renderer.rsSetState(rasterizerState);
			renderer.omSetDepthStencilState(depthStencilState);
			renderer.omSetBlendState(blendState);
		}
	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
