/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
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
#include "RendererRuntime/Core/Renderer/FramebufferSignature.h"
#include "RendererRuntime/Core/Math/Math.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	FramebufferSignature::FramebufferSignature(uint8_t numberOfColorTextures, AssetId colorTextureAssetIds[8], AssetId depthStencilTextureAssetId) :
		mNumberOfColorTextures(numberOfColorTextures),
		mColorTextureAssetIds{colorTextureAssetIds[0], colorTextureAssetIds[1], colorTextureAssetIds[2], colorTextureAssetIds[3], colorTextureAssetIds[4], colorTextureAssetIds[5], colorTextureAssetIds[6], colorTextureAssetIds[7]},
		mDepthStencilTextureAssetId(depthStencilTextureAssetId),
		mFramebufferSignatureId(Math::FNV1a_INITIAL_HASH_32)
	{
		mFramebufferSignatureId = Math::calculateFNV1a32(reinterpret_cast<const uint8_t*>(&mNumberOfColorTextures), sizeof(AssetId), mFramebufferSignatureId);
		for (uint8_t i = 0; i < mNumberOfColorTextures; ++i)
		{
			mFramebufferSignatureId = Math::calculateFNV1a32(reinterpret_cast<const uint8_t*>(&mColorTextureAssetIds[i]), sizeof(AssetId), mFramebufferSignatureId);
		}
		mFramebufferSignatureId = Math::calculateFNV1a32(reinterpret_cast<const uint8_t*>(&mDepthStencilTextureAssetId), sizeof(AssetId), mFramebufferSignatureId);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
