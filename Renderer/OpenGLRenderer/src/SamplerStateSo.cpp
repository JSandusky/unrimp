/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/SamplerStateSo.h"
#include "OpenGLRenderer/Mapping.h"
#include "OpenGLRenderer/Extensions.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	SamplerStateSo::SamplerStateSo(OpenGLRenderer &openGLRenderer, const Renderer::SamplerState &samplerState) :
		SamplerState(openGLRenderer),
		mOpenGLSampler(0)
	{
		// Create the OpenGL sampler
		glGenSamplers(1, &mOpenGLSampler);

		// Renderer::SamplerState::filter
		glSamplerParameteri(mOpenGLSampler, GL_TEXTURE_MAG_FILTER, Mapping::getOpenGLMagFilterMode(samplerState.filter));
		glSamplerParameteri(mOpenGLSampler, GL_TEXTURE_MIN_FILTER, Mapping::getOpenGLMinFilterMode(samplerState.filter));

		// Renderer::SamplerState::addressU
		glSamplerParameteri(mOpenGLSampler, GL_TEXTURE_WRAP_S, Mapping::getOpenGLTextureAddressMode(samplerState.addressU));

		// Renderer::SamplerState::addressV
		glSamplerParameteri(mOpenGLSampler, GL_TEXTURE_WRAP_T, Mapping::getOpenGLTextureAddressMode(samplerState.addressV));

		// Renderer::SamplerState::addressW
		glSamplerParameteri(mOpenGLSampler, GL_TEXTURE_WRAP_R, Mapping::getOpenGLTextureAddressMode(samplerState.addressW));

		// Renderer::SamplerState::mipLODBias
		// -> "GL_EXT_texture_lod_bias"-extension
		glSamplerParameterf(mOpenGLSampler, GL_TEXTURE_LOD_BIAS, samplerState.mipLODBias);

		// Renderer::SamplerState::maxAnisotropy
		// -> Maximum anisotropy is "unsigned int" in Direct3D 10 & 11
		glSamplerParameterf(mOpenGLSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<float>(samplerState.maxAnisotropy));

		// Renderer::SamplerState::comparisonFunc
		// -> "GL_EXT_shadow_funcs"/"GL_EXT_shadow_samplers"-extension
		glSamplerParameteri(mOpenGLSampler, GL_TEXTURE_COMPARE_MODE, Mapping::getOpenGLCompareMode(samplerState.filter));
		glSamplerParameteri(mOpenGLSampler, GL_TEXTURE_COMPARE_FUNC, Mapping::getOpenGLComparisonFunc(samplerState.comparisonFunc));

		// Renderer::SamplerState::borderColor[4]
		glSamplerParameterfv(mOpenGLSampler, GL_TEXTURE_BORDER_COLOR, samplerState.borderColor);

		// Renderer::SamplerState::minLOD
		glSamplerParameterf(mOpenGLSampler, GL_TEXTURE_MIN_LOD, samplerState.minLOD);

		// Renderer::SamplerState::maxLOD
		glSamplerParameterf(mOpenGLSampler, GL_TEXTURE_MAX_LOD, samplerState.maxLOD);
	}

	/**
	*  @brief
	*    Destructor
	*/
	SamplerStateSo::~SamplerStateSo()
	{
		// Destroy the OpenGL sampler
		// -> Silently ignores 0's and names that do not correspond to existing samplers
		glDeleteSamplers(1, &mOpenGLSampler);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
