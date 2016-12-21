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


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline RenderTargetTextureSignature::RenderTargetTextureSignature() :
		mWidth(getUninitialized<uint32_t>()),
		mHeight(getUninitialized<uint32_t>()),
		mTextureFormat(Renderer::TextureFormat::UNKNOWN)
	{
		// Nothing here
	}

	inline RenderTargetTextureSignature::RenderTargetTextureSignature(const RenderTargetTextureSignature& renderTargetTextureSignature) :
		mWidth(renderTargetTextureSignature.mWidth),
		mHeight(renderTargetTextureSignature.mHeight),
		mTextureFormat(renderTargetTextureSignature.mTextureFormat),
		mRenderTargetTextureSignatureId(renderTargetTextureSignature.mRenderTargetTextureSignatureId)
	{
		// Nothing here
	}

	inline RenderTargetTextureSignature::~RenderTargetTextureSignature()
	{
		// Nothing here
	}

	inline RenderTargetTextureSignature& RenderTargetTextureSignature::operator=(const RenderTargetTextureSignature& renderTargetTextureSignature)
	{
		mWidth							= renderTargetTextureSignature.mWidth;
		mHeight							= renderTargetTextureSignature.mHeight;
		mTextureFormat					= renderTargetTextureSignature.mTextureFormat;
		mRenderTargetTextureSignatureId	= renderTargetTextureSignature.mRenderTargetTextureSignatureId;

		// Done
		return *this;
	}

	inline uint32_t RenderTargetTextureSignature::getWidth() const
	{
		return mWidth;
	}

	inline uint32_t RenderTargetTextureSignature::getHeight() const
	{
		return mHeight;
	}

	inline Renderer::TextureFormat::Enum RenderTargetTextureSignature::getTextureFormat() const
	{
		return mTextureFormat;
	}

	inline RenderTargetTextureSignatureId RenderTargetTextureSignature::getRenderTargetTextureSignatureId() const
	{
		return mRenderTargetTextureSignatureId;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime