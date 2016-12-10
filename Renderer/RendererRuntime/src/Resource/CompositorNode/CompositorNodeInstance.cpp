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
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeInstance.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ICompositorInstancePass.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CompositorNodeInstance::~CompositorNodeInstance()
	{
		for (ICompositorInstancePass* compositorInstancePass : mCompositorInstancePasses)
		{
			delete compositorInstancePass;
		}
	}

	void CompositorNodeInstance::compositorWorkspaceInstanceLoadingFinished() const
	{
		for (ICompositorInstancePass* compositorInstancePass : mCompositorInstancePasses)
		{
			compositorInstancePass->onCompositorWorkspaceInstanceLoadingFinished();
		}
	}

	void CompositorNodeInstance::fillCommandBuffer(Renderer::CommandBuffer& commandBuffer) const
	{
		for (ICompositorInstancePass* compositorInstancePass : mCompositorInstancePasses)
		{
			compositorInstancePass->onFillCommandBuffer(commandBuffer);
		}
	}

	void CompositorNodeInstance::frameEnded() const
	{
		for (ICompositorInstancePass* compositorInstancePass : mCompositorInstancePasses)
		{
			compositorInstancePass->onFrameEnded();
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
