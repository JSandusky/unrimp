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
#include "RendererRuntime/Resource/Compositor/CompositorInstanceNode.h"
#include "RendererRuntime/Resource/Compositor/Pass/ICompositorInstancePass.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	const CompositorResourceNode& CompositorInstanceNode::getCompositorResourceNode() const
	{
		return mCompositorResourceNode;
	}

	const CompositorInstance& CompositorInstanceNode::getCompositorInstance() const
	{
		return mCompositorInstance;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CompositorInstanceNode::CompositorInstanceNode(const CompositorResourceNode& compositorResourceNode, const CompositorInstance& compositorInstance) :
		mCompositorResourceNode(compositorResourceNode),
		mCompositorInstance(compositorInstance)
	{
		// Nothing here
	}

	CompositorInstanceNode::~CompositorInstanceNode()
	{
		// Nothing here
	}

	void CompositorInstanceNode::execute(CameraSceneItem* cameraSceneItem)
	{
		const size_t numberOfCompositorInstancePasses = mCompositorInstancePasses.size();
		for (size_t i = 0; i < numberOfCompositorInstancePasses; ++i)
		{
			mCompositorInstancePasses[i]->execute(cameraSceneItem);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
