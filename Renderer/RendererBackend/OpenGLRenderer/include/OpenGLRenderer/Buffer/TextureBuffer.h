/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include <Renderer/Buffer/ITextureBuffer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{
	class OpenGLRenderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract OpenGL texture buffer object (TBO) interface
	*/
	class TextureBuffer : public Renderer::ITextureBuffer
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		virtual ~TextureBuffer() override;

		/**
		*  @brief
		*    Return the OpenGL texture buffer instance
		*
		*  @return
		*    The OpenGL texture buffer instance, can be zero if no resource is allocated, do not destroy the returned resource (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline uint32_t getOpenGLTextureBuffer() const;

		/**
		*  @brief
		*    Return the OpenGL texture instance
		*
		*  @return
		*    The OpenGL texture instance, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline uint32_t getOpenGLTexture() const;


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	protected:
		virtual void selfDestruct() override;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*/
		explicit TextureBuffer(OpenGLRenderer& openGLRenderer);


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		uint32_t mOpenGLTextureBuffer;	///< OpenGL texture buffer, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)
		uint32_t mOpenGLTexture;		///< OpenGL texture, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit TextureBuffer(const TextureBuffer& source) = delete;
		TextureBuffer& operator =(const TextureBuffer& source) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/Buffer/TextureBuffer.inl"
