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
#include "RendererRuntime/Core/GetUninitialized.h"

#include <cassert>
#include <cstring>	// For "memcpy()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline FramebufferSignature::FramebufferSignature() :
		mNumberOfColorTextures(0),
		mColorTextureAssetIds{getUninitialized<AssetId>(), getUninitialized<AssetId>(), getUninitialized<AssetId>(), getUninitialized<AssetId>(), getUninitialized<AssetId>(), getUninitialized<AssetId>(), getUninitialized<AssetId>(), getUninitialized<AssetId>()},
		mDepthStencilTextureAssetId(getUninitialized<AssetId>())
	{
		// Nothing here
	}

	inline FramebufferSignature::FramebufferSignature(const FramebufferSignature& framebufferSignature) :
		mNumberOfColorTextures(framebufferSignature.mNumberOfColorTextures),
		mColorTextureAssetIds{framebufferSignature.mColorTextureAssetIds[0], framebufferSignature.mColorTextureAssetIds[1], framebufferSignature.mColorTextureAssetIds[2], framebufferSignature.mColorTextureAssetIds[3], framebufferSignature.mColorTextureAssetIds[4], framebufferSignature.mColorTextureAssetIds[5], framebufferSignature.mColorTextureAssetIds[6], framebufferSignature.mColorTextureAssetIds[7]},
		mDepthStencilTextureAssetId(framebufferSignature.mDepthStencilTextureAssetId),
		mFramebufferSignatureId(framebufferSignature.mFramebufferSignatureId)
	{
		// Nothing here
	}

	inline FramebufferSignature::~FramebufferSignature()
	{
		// Nothing here
	}

	inline FramebufferSignature& FramebufferSignature::operator=(const FramebufferSignature& framebufferSignature)
	{
		mNumberOfColorTextures = framebufferSignature.mNumberOfColorTextures;
		memcpy(mColorTextureAssetIds, framebufferSignature.mColorTextureAssetIds, sizeof(AssetId) * 8);
		mDepthStencilTextureAssetId = framebufferSignature.mDepthStencilTextureAssetId;

		// Done
		return *this;
	}

	inline uint8_t FramebufferSignature::getNumberOfColorTextures() const
	{
		return mNumberOfColorTextures;
	}

	inline AssetId FramebufferSignature::getColorTextureAssetId(uint8_t index) const
	{
		assert(index < mNumberOfColorTextures);
		return mColorTextureAssetIds[index];
	}

	inline AssetId FramebufferSignature::getDepthStencilTextureAssetId() const
	{
		return mDepthStencilTextureAssetId;
	}

	inline FramebufferSignatureId FramebufferSignature::getFramebufferSignatureId() const
	{
		return mFramebufferSignatureId;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
