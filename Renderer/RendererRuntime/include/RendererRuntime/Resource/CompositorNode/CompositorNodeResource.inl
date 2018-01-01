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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline void CompositorNodeResource::reserveInputChannels(uint32_t numberOfInputChannels)
	{
		mInputChannels.reserve(numberOfInputChannels);
	}

	inline void CompositorNodeResource::addInputChannel(CompositorChannelId compositorChannelId)
	{
		mInputChannels.push_back(compositorChannelId);
	}

	inline const CompositorNodeResource::CompositorChannels& CompositorNodeResource::getInputChannels() const
	{
		return mInputChannels;
	}

	inline void CompositorNodeResource::reserveRenderTargetTextures(uint32_t numberOfRenderTargetTextures)
	{
		mCompositorRenderTargetTextures.reserve(numberOfRenderTargetTextures);
	}

	inline void CompositorNodeResource::addRenderTargetTexture(AssetId assetId, const RenderTargetTextureSignature& renderTargetTextureSignature)
	{
		mCompositorRenderTargetTextures.emplace_back(assetId, renderTargetTextureSignature);
	}

	inline const CompositorNodeResource::CompositorRenderTargetTextures& CompositorNodeResource::getRenderTargetTextures() const
	{
		return mCompositorRenderTargetTextures;
	}

	inline void CompositorNodeResource::reserveFramebuffers(uint32_t numberOfFramebuffers)
	{
		mCompositorFramebuffers.reserve(numberOfFramebuffers);
	}

	inline void CompositorNodeResource::addFramebuffer(CompositorFramebufferId compositorFramebufferId, const FramebufferSignature& framebufferSignature)
	{
		mCompositorFramebuffers.emplace_back(compositorFramebufferId, framebufferSignature);
	}

	inline const CompositorNodeResource::CompositorFramebuffers& CompositorNodeResource::getFramebuffers() const
	{
		return mCompositorFramebuffers;
	}

	inline void CompositorNodeResource::reserveCompositorTargets(uint32_t numberOfCompositorTargets)
	{
		mCompositorTargets.reserve(numberOfCompositorTargets);
	}

	inline CompositorTarget& CompositorNodeResource::addCompositorTarget(CompositorChannelId compositorChannelId, CompositorFramebufferId compositorFramebufferId)
	{
		mCompositorTargets.emplace_back(compositorChannelId, compositorFramebufferId);
		return mCompositorTargets.back();
	}

	inline const CompositorNodeResource::CompositorTargets& CompositorNodeResource::getCompositorTargets() const
	{
		return mCompositorTargets;
	}

	inline void CompositorNodeResource::reserveOutputChannels(uint32_t numberOfOutputChannels)
	{
		mOutputChannels.reserve(numberOfOutputChannels);
	}

	inline void CompositorNodeResource::addOutputChannel(CompositorChannelId compositorChannelId)
	{
		mOutputChannels.push_back(compositorChannelId);
	}

	inline const CompositorNodeResource::CompositorChannels& CompositorNodeResource::getOutputChannels() const
	{
		return mOutputChannels;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline CompositorNodeResource::CompositorNodeResource()
	{
		// Nothing here
	}

	inline CompositorNodeResource::~CompositorNodeResource()
	{
		// Sanity checks
		assert(mInputChannels.empty());
		assert(mCompositorRenderTargetTextures.empty());
		assert(mCompositorFramebuffers.empty());
		assert(mCompositorTargets.empty());
		assert(mOutputChannels.empty());
	}

	inline void CompositorNodeResource::initializeElement(CompositorNodeResourceId compositorNodeResourceId)
	{
		// Sanity checks
		assert(mInputChannels.empty());
		assert(mCompositorRenderTargetTextures.empty());
		assert(mCompositorFramebuffers.empty());
		assert(mCompositorTargets.empty());
		assert(mOutputChannels.empty());

		// Call base implementation
		IResource::initializeElement(compositorNodeResourceId);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
