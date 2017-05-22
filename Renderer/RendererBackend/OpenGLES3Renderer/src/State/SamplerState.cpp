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
#include "OpenGLES3Renderer/State/SamplerState.h"
#include "OpenGLES3Renderer/Mapping.h"
#include "OpenGLES3Renderer/IContext.h"	// We need to include this header, else the linker won't find our defined OpenGL ES 3 functions
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"

#include <GLES3/gl3.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SamplerState::SamplerState(OpenGLES3Renderer &openGLES3Renderer, const Renderer::SamplerState &samplerState) :
		ISamplerState(openGLES3Renderer),
		mOpenGLMagFilterMode(Mapping::getOpenGLES3MagFilterMode(samplerState.filter)),
		mOpenGLMinFilterMode(Mapping::getOpenGLES3MinFilterMode(samplerState.filter, samplerState.maxLOD > 0.0f)),
		mOpenGLTextureAddressModeS(Mapping::getOpenGLES3TextureAddressMode(samplerState.addressU)),
		mOpenGLTextureAddressModeT(Mapping::getOpenGLES3TextureAddressMode(samplerState.addressV)),
		mOpenGLTextureAddressModeR(Mapping::getOpenGLES3TextureAddressMode(samplerState.addressW)),
		mMipLODBias(samplerState.mipLODBias),
		mMaxAnisotropy(static_cast<float>(samplerState.maxAnisotropy)),	// Maximum anisotropy is "uint32_t" in Direct3D 10 & 11
		mOpenGLCompareMode(Mapping::getOpenGLES3CompareMode(samplerState.filter)),
		mOpenGLComparisonFunc(Mapping::getOpenGLES3ComparisonFunc(samplerState.comparisonFunc)),
		mMinLOD(samplerState.minLOD),
		mMaxLOD(samplerState.maxLOD)
	{
		// Sanity check
		assert(samplerState.maxAnisotropy <= openGLES3Renderer.getCapabilities().maximumAnisotropy && "Maximum anisotropy value violated");

		// Ignore "Renderer::SamplerState.borderColor", border color is not supported by OpenGL ES 3

		// TODO(co)  "GL_COMPARE_REF_TO_TEXTURE" is not supported by OpenGL ES 3, check/inform the user?
		// TODO(co)  "GL_CLAMP_TO_BORDER" is not supported by OpenGL ES 3, check/inform the user?
	}

	SamplerState::~SamplerState()
	{
		// Nothing here
	}

	void SamplerState::setOpenGLES3SamplerStates() const
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
} // OpenGLES3Renderer
