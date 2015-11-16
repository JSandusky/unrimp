/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "RendererRuntime/Resource/Compositor/Pass/Clear/CompositorInstancePassClear.h"
#include "RendererRuntime/Resource/Compositor/Pass/Clear/CompositorResourcePassClear.h"
#include "RendererRuntime/Resource/Compositor/CompositorInstanceNode.h"
#include "RendererRuntime/Resource/Compositor/CompositorInstance.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::ICompositorInstancePass methods ]
	//[-------------------------------------------------------]
	void CompositorInstancePassClear::execute()
	{
		// TODO(co) Just a test, backup instances later on
		Renderer::IRenderer& renderer = getCompositorInstanceNode().getCompositorInstance().getRendererRuntime().getRenderer();

		// Clear the color buffer of the current render target with gray, do also clear the depth buffer
		renderer.clear(Renderer::ClearFlag::COLOR_DEPTH, static_cast<const CompositorResourcePassClear&>(mCompositorResourcePass).getClearColor(), 1.0f, 0);
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CompositorInstancePassClear::CompositorInstancePassClear(const CompositorResourcePassClear& compositorResourcePassClear, const CompositorInstanceNode& compositorInstanceNode) :
		ICompositorInstancePass(compositorResourcePassClear, compositorInstanceNode)
	{
		// Nothing here
	}

	CompositorInstancePassClear::~CompositorInstancePassClear()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
