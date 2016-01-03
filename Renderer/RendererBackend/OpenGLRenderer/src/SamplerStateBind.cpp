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
#include "OpenGLRenderer/SamplerStateBind.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"
#include "OpenGLRenderer/Mapping.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SamplerStateBind::SamplerStateBind(OpenGLRenderer &openGLRenderer, const Renderer::SamplerState &samplerState) :
		SamplerState(openGLRenderer),
		mOpenGLMagFilterMode(Mapping::getOpenGLMagFilterMode(samplerState.filter)),
		mOpenGLMinFilterMode(Mapping::getOpenGLMinFilterMode(samplerState.filter, samplerState.maxLOD > 0.0f)),
		mOpenGLTextureAddressModeS(Mapping::getOpenGLTextureAddressMode(samplerState.addressU)),
		mOpenGLTextureAddressModeT(Mapping::getOpenGLTextureAddressMode(samplerState.addressV)),
		mOpenGLTextureAddressModeR(Mapping::getOpenGLTextureAddressMode(samplerState.addressW)),
		mMipLODBias(samplerState.mipLODBias),
		mMaxAnisotropy(static_cast<float>(samplerState.maxAnisotropy)),	// Maximum anisotropy is "uint32_t" in Direct3D 10 & 11
		mOpenGLCompareMode(Mapping::getOpenGLCompareMode(samplerState.filter)),
		mOpenGLComparisonFunc(Mapping::getOpenGLComparisonFunc(samplerState.comparisonFunc)),
		mMinLOD(samplerState.minLOD),
		mMaxLOD(samplerState.maxLOD)
	{
		// Renderer::SamplerState::borderColor[4]
		mBorderColor[0] = samplerState.borderColor[0];
		mBorderColor[1] = samplerState.borderColor[1];
		mBorderColor[2] = samplerState.borderColor[2];
		mBorderColor[3] = samplerState.borderColor[3];
	}

	SamplerStateBind::~SamplerStateBind()
	{
		// Nothing to do in here
	}

	void SamplerStateBind::setOpenGLSamplerStates() const
	{
		// TODO(co) Support other targets, not just "GL_TEXTURE_2D"

		// Renderer::SamplerState::filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mOpenGLMagFilterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mOpenGLMinFilterMode);

		// Renderer::SamplerState::addressU
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mOpenGLTextureAddressModeS);

		// Renderer::SamplerState::addressV
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mOpenGLTextureAddressModeT);
		/*
		// TODO(co) Support for 3D textures
		// Renderer::SamplerState::addressW
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, mOpenGLTextureAddressModeR);

		// TODO(co) Complete me
		// Renderer::SamplerState::mipLODBias
		// -> "GL_EXT_texture_lod_bias"-extension
		glSamplerParameterf(mOpenGLSampler, GL_TEXTURE_LOD_BIAS, samplerState.mipLODBias);

		// Renderer::SamplerState::maxAnisotropy
		glSamplerParameterf(mOpenGLSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, samplerState.maxAnisotropy);

		// Renderer::SamplerState::comparisonFunc
		// -> "GL_EXT_shadow_funcs"/"GL_EXT_shadow_samplers"-extension
		glTexParameteri(mOpenGLSampler, GL_TEXTURE_COMPARE_MODE, Mapping::getOpenGLCompareMode(samplerState.filter));
		glTexParameteri(mOpenGLSampler, GL_TEXTURE_COMPARE_FUNC, Mapping::getOpenGLComparisonFunc(samplerState.comparisonFunc));

		// Renderer::SamplerState::borderColor[4]
		glSamplerParameterfv(mOpenGLSampler, GL_TEXTURE_BORDER_COLOR, samplerState.borderColor);

		// Renderer::SamplerState::minLOD
		glSamplerParameterf(mOpenGLSampler, GL_TEXTURE_MIN_LOD, samplerState.minLOD);

		// Renderer::SamplerState::maxLOD
		glSamplerParameterf(mOpenGLSampler, GL_TEXTURE_MAX_LOD, samplerState.maxLOD);
		*/
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
