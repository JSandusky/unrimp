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
	inline CompositorResourceTarget::CompositorResourceTarget(CompositorChannelId compositorChannelId) :
		mCompositorChannelId(compositorChannelId)
	{
		// Nothing here
	}

	inline CompositorResourceTarget::CompositorResourceTarget(const CompositorResourceTarget& compositorResourceTarget) :
		mCompositorChannelId(compositorResourceTarget.mCompositorChannelId)
	{
		// Nothing here
	}

	inline CompositorResourceTarget::~CompositorResourceTarget()
	{
		removeAllCompositorResourcePasses();
	}

	inline CompositorChannelId CompositorResourceTarget::getCompositorChannelId() const
	{
		return mCompositorChannelId;
	}

	inline void CompositorResourceTarget::setNumberOfCompositorResourcePasses(uint32_t compositorResourcePasses)
	{
		mCompositorResourcePasses.reserve(compositorResourcePasses);
	}

	inline const CompositorResourceTarget::CompositorResourcePasses& CompositorResourceTarget::getCompositorResourcePasses() const
	{
		return mCompositorResourcePasses;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime