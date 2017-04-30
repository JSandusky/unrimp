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
#include "OpenGLES3Renderer/Buffer/TextureBuffer.h"

#include <Renderer/Buffer/BufferTypes.h>
#include <Renderer/Texture/TextureTypes.h>


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
	*    OpenGL texture buffer object (TBO) class, traditional bind version and uniform buffer to emulate texture buffer behaviour (with limitations, of course)
	*/
	class TextureBufferBindEmulation : public TextureBuffer
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
		*    Owner OpenGLES renderer instance
		*  @param[in] numberOfBytes
		*    Number of bytes within the texture buffer, must be valid
		*  @param[in] textureFormat
		*    Texture buffer data format
		*  @param[in] data
		*    Texture buffer data, can be a null pointer (empty buffer)
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*/
		TextureBufferBindEmulation(OpenGLES3Renderer &openGLES3Renderer, uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data = nullptr, Renderer::BufferUsage bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~TextureBufferBindEmulation();


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureBuffer methods       ]
	//[-------------------------------------------------------]
	public:
		virtual void copyDataFrom(uint32_t numberOfBytes, const void *data) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit TextureBufferBindEmulation(const TextureBufferBindEmulation& source) = delete;
		TextureBufferBindEmulation& operator =(const TextureBufferBindEmulation& source) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
