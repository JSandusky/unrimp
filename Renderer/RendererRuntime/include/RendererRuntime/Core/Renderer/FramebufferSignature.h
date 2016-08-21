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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/NonCopyable.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t FramebufferSignatureId;	///< Framebuffer signature identifier, result of hashing framebuffer properties


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Framebuffer signature
	*/
	class FramebufferSignature : private NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		inline FramebufferSignature();

		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] width
		*    Width
		*  @param[in] height
		*    Height
		*  @param[in] textureFormat
		*    Texture format
		*/
		FramebufferSignature(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] framebufferSignature
		*    Framebuffer signature to copy from
		*/
		explicit FramebufferSignature(const FramebufferSignature& framebufferSignature);

		/**
		*  @brief
		*    Destructor
		*/
		inline ~FramebufferSignature();

		/**
		*  @brief
		*    Copy operator
		*/
		FramebufferSignature& operator=(const FramebufferSignature& framebufferSignature);

		//[-------------------------------------------------------]
		//[ Getter for input data                                 ]
		//[-------------------------------------------------------]
		inline uint32_t getWidth() const;
		inline uint32_t getHeight() const;
		inline Renderer::TextureFormat::Enum getTextureFormat() const;

		//[-------------------------------------------------------]
		//[ Getter for derived data                               ]
		//[-------------------------------------------------------]
		inline FramebufferSignatureId getFramebufferSignatureId() const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Input data
		uint32_t					  mWidth;
		uint32_t					  mHeight;
		Renderer::TextureFormat::Enum mTextureFormat;
		// Derived data
		FramebufferSignatureId mFramebufferSignatureId;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/Renderer/FramebufferSignature.inl"
