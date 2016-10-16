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
#include "OpenGLES2Renderer/Texture/SamplerState.h"
#include "OpenGLES2Renderer/Mapping.h"
#include "OpenGLES2Renderer/IContext.h"	// We need to include this header, else the linker won't find our defined OpenGL ES 2 functions
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"

#include <GLES2/gl2.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SamplerState::SamplerState(OpenGLES2Renderer &openGLES2Renderer, const Renderer::SamplerState &samplerState) :
		ISamplerState(openGLES2Renderer),
		mOpenGLMagFilterMode(Mapping::getOpenGLES2MagFilterMode(samplerState.filter)),
		mOpenGLMinFilterMode(Mapping::getOpenGLES2MinFilterMode(samplerState.filter, samplerState.maxLOD > 0.0f)),
		mOpenGLTextureAddressModeS(Mapping::getOpenGLES2TextureAddressMode(samplerState.addressU)),
		mOpenGLTextureAddressModeT(Mapping::getOpenGLES2TextureAddressMode(samplerState.addressV)),
		mOpenGLTextureAddressModeR(Mapping::getOpenGLES2TextureAddressMode(samplerState.addressW)),
		mMipLODBias(samplerState.mipLODBias),
		mMaxAnisotropy(static_cast<float>(samplerState.maxAnisotropy)),	// Maximum anisotropy is "uint32_t" in Direct3D 10 & 11
		mOpenGLCompareMode(Mapping::getOpenGLES2CompareMode(samplerState.filter)),
		mOpenGLComparisonFunc(Mapping::getOpenGLES2ComparisonFunc(samplerState.comparisonFunc)),
		mMinLOD(samplerState.minLOD),
		mMaxLOD(samplerState.maxLOD)
	{
		// Ignore "Renderer::SamplerState.borderColor", border color is not supported by OpenGL ES 2

		// TODO(co)  "GL_COMPARE_REF_TO_TEXTURE" is not supported by OpenGL ES 2, check/inform the user?
		// TODO(co)  "GL_CLAMP_TO_BORDER" is not supported by OpenGL ES 2, check/inform the user?
	}

	SamplerState::~SamplerState()
	{
		// Nothing to do in here
	}

	void SamplerState::setOpenGLES2SamplerStates() const
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
		glSamplerParameterf(mOpenGLSampler, GL_TEXTURE_LOD_BIAS, samplerState.mipLODBias);

		// Renderer::SamplerState::maxAnisotropy
		glSamplerParameterf(mOpenGLSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, samplerState.maxAnisotropy);

		// Renderer::SamplerState::comparisonFunc
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
} // OpenGLES2Renderer
