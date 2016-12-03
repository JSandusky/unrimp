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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Clear/CompositorInstancePassClear.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Clear/CompositorResourcePassClear.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeInstance.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceInstance.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::ICompositorInstancePass methods ]
	//[-------------------------------------------------------]
	void CompositorInstancePassClear::onExecute()
	{
		// Clear the color buffer of the current render target, do also clear the depth buffer
		// TODO(co) "RendererRuntime::CompositorInstancePassClear": Add the other properties like stencil or z-buffer
		mRenderer.clear(Renderer::ClearFlag::COLOR_DEPTH, glm::value_ptr(static_cast<const CompositorResourcePassClear&>(mCompositorResourcePass).getClearColor()), 1.0f, 0);
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CompositorInstancePassClear::CompositorInstancePassClear(const CompositorResourcePassClear& compositorResourcePassClear, const CompositorNodeInstance& compositorNodeInstance) :
		ICompositorInstancePass(compositorResourcePassClear, compositorNodeInstance),
		mRenderer(compositorNodeInstance.getCompositorWorkspaceInstance().getRendererRuntime().getRenderer())
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
