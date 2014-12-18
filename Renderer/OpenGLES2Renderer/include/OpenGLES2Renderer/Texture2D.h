/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#ifndef __OPENGLES2RENDERER_TEXTURE2D_H__
#define __OPENGLES2RENDERER_TEXTURE2D_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/ITexture2D.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{
	class OpenGLES2Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    OpenGL ES 2 2D texture class
	*/
	class Texture2D : public Renderer::ITexture2D
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLES2Renderer
		*    Owner OpenGL ES 2 renderer instance
		*  @param[in] width
		*    Texture width, must be >1
		*  @param[in] height
		*    Texture height, must be >1
		*  @param[in] textureFormat
		*    Texture format
		*  @param[in] data
		*    Texture data, can be a null pointer
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*/
		Texture2D(OpenGLES2Renderer &openGLES2Renderer, unsigned int width, unsigned int height, Renderer::TextureFormat::Enum textureFormat, void *data, unsigned int flags);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Texture2D();

		/**
		*  @brief
		*    Return the OpenGL ES 2 texture
		*
		*  @return
		*    The OpenGL ES 2 texture, can be zero if no resource is allocated, do not destroy the returned resource (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline unsigned int getOpenGLES2Texture() const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		unsigned int mOpenGLES2Texture;	///< OpenGL ES 2 texture, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLES2Renderer/Texture2D.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLES2RENDERER_TEXTURE2D_H__
