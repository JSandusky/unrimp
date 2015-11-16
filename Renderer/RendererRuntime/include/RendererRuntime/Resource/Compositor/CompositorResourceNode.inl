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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline CompositorResourceNodeId CompositorResourceNode::getCompositorResourceNodeId() const
	{
		return mCompositorResourceNodeId;
	}

	inline void CompositorResourceNode::setNumberOfInputChannels(uint32_t numberOfInputChannels)
	{
		mInputChannels.reserve(numberOfInputChannels);
	}

	inline void CompositorResourceNode::addInputChannel(CompositorChannelId compositorChannelId)
	{
		mInputChannels.push_back(compositorChannelId);
	}

	inline const CompositorResourceNode::CompositorChannels& CompositorResourceNode::getInputChannels() const
	{
		return mInputChannels;
	}

	inline void CompositorResourceNode::setNumberOfCompositorResourceTargets(uint32_t numberOfCompositorResourceTargets)
	{
		mCompositorResourceTargets.reserve(numberOfCompositorResourceTargets);
	}

	inline CompositorResourceTarget& CompositorResourceNode::addCompositorResourceTarget(CompositorChannelId compositorChannelId)
	{
		// TODO(co) Ensure move copy constructor is used
		mCompositorResourceTargets.push_back(CompositorResourceTarget(compositorChannelId));
		return mCompositorResourceTargets.back();
	}

	inline const CompositorResourceNode::CompositorResourceTargets& CompositorResourceNode::getCompositorResourceTargets() const
	{
		return mCompositorResourceTargets;
	}

	inline void CompositorResourceNode::setNumberOfOutputChannels(uint32_t numberOfOutputChannels)
	{
		mOutputChannels.reserve(numberOfOutputChannels);
	}

	inline void CompositorResourceNode::addOutputChannel(CompositorChannelId compositorChannelId)
	{
		mOutputChannels.push_back(compositorChannelId);
	}

	inline const CompositorResourceNode::CompositorChannels& CompositorResourceNode::getOutputChannels() const
	{
		return mOutputChannels;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
