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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/State/ISamplerState.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{
	class OpenGLES3Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    OpenGL ES 3 sampler state class
	*/
	class SamplerState : public Renderer::ISamplerState
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLES3Renderer
		*    Owner OpenGL ES 3 renderer instance
		*  @param[in] samplerState
		*    Sampler state to use
		*/
		SamplerState(OpenGLES3Renderer& openGLES3Renderer, const Renderer::SamplerState& samplerState);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~SamplerState() override;

		/**
		*  @brief
		*    Set the OpenGL ES 3 sampler states
		*/
		void setOpenGLES3SamplerStates() const;


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	protected:
		virtual void selfDestruct() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit SamplerState(const SamplerState& source) = delete;
		SamplerState& operator =(const SamplerState& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// "Renderer::SamplerState" translated into OpenGL ES 3
		int   mOpenGLMagFilterMode;			///< Renderer::SamplerState::filter (type "GLint" not used in here in order to keep the header slim)
		int   mOpenGLMinFilterMode;			///< Renderer::SamplerState::filter (type "GLint" not used in here in order to keep the header slim)
		int   mOpenGLTextureAddressModeS;	///< Renderer::SamplerState::addressU (type "GLint" not used in here in order to keep the header slim)
		int   mOpenGLTextureAddressModeT;	///< Renderer::SamplerState::addressV (type "GLint" not used in here in order to keep the header slim)
		int   mOpenGLTextureAddressModeR;	///< Renderer::SamplerState::addressW (type "GLint" not used in here in order to keep the header slim)
		float mMipLODBias;					///< Renderer::SamplerState::mipLODBias
		float mMaxAnisotropy;				///< Renderer::SamplerState::maxAnisotropy
		int   mOpenGLCompareMode;			///< Renderer::SamplerState::comparisonFunc (type "GLint" not used in here in order to keep the header slim)
		int   mOpenGLComparisonFunc;		///< Renderer::SamplerState::comparisonFunc (type "GLint" not used in here in order to keep the header slim)
		float mMinLOD;						///< Renderer::SamplerState::minLOD
		float mMaxLOD;						///< Renderer::SamplerState::maxLOD


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
