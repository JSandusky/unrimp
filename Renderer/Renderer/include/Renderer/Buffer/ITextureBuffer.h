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
#include "Renderer/Buffer/IBuffer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract texture buffer object (TBO) interface
	*
	*  @remarks
	*    General usage hint
	*      - Maximum size:                 128MByte (or more)
	*      - Memory access pattern:        Random access
	*      - Memory storage:               Global texture memory
	*    OpenGL - http://www.opengl.org/wiki/Buffer_Texture
	*      - Core in version:              4.2
	*      - Adopted into core in version: 3.0
	*      - ARB extension:                GL_ARB_texture_buffer_object
	*      - EXT extension:                GL_EXT_texture_buffer_object
	*    Direct3D - "Shader Constants"-documentation - http://msdn.microsoft.com/en-us/library/windows/desktop/bb509581%28v=vs.85%29.aspx
	*      - Direct3D version:              10 and 11
	*      - Shader model:                 4
	*/
	class ITextureBuffer : public IBuffer
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~ITextureBuffer();


	//[-------------------------------------------------------]
	//[ Public virtual ITextureBuffer methods                 ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Copy data into the texture buffer object
		*
		*  @param[in] numberOfBytes
		*    Number of bytes within the texture buffer, must be valid
		*  @param[in] data
		*    Texture buffer data, must be valid
		*/
		virtual void copyDataFrom(uint32_t numberOfBytes, const void *data) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Owner renderer instance
		*/
		inline explicit ITextureBuffer(IRenderer &renderer);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit ITextureBuffer(const ITextureBuffer &source);

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		inline ITextureBuffer &operator =(const ITextureBuffer &source);


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<ITextureBuffer> ITextureBufferPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/Buffer/ITextureBuffer.inl"
