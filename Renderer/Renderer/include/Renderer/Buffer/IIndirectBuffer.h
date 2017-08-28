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
	*    Abstract indirect buffer object interface
	*
	*  @note
	*    - Contains instances of "Renderer::DrawInstancedArguments" and "Renderer::DrawIndexedInstancedArguments"
	*    - Indirect buffers where originally introduced to be able to let the GPU have some more control over draw commands,
	*      but with the introduction of multi indirect draw it became also interesting for reducing renderer API overhead (AZDO ("Almost Zero Driver Overhead"))
	*/
	class IIndirectBuffer : public IBuffer
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~IIndirectBuffer() override;


	//[-------------------------------------------------------]
	//[ Public virtual IIndirectBuffer methods                ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return indirect buffer emulation data pointer
		*
		*  @return
		*    Indirect buffer emulation data pointer, can be a null pointer, don't destroy the returned instance
		*/
		virtual const uint8_t* getEmulationData() const = 0;

		/**
		*  @brief
		*    Copy data into the indirect buffer object
		*
		*  @param[in] numberOfBytes
		*    Number of bytes within the indirect buffer, must be valid
		*  @param[in] data
		*    Indirect buffer data, must be valid
		*/
		virtual void copyDataFrom(uint32_t numberOfBytes, const void* data) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Default constructor
		*
		*  @note
		*    - Only used for rare border cases, use the constructor with the renderer reference whenever possible
		*/
		inline IIndirectBuffer();

		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Owner renderer instance
		*/
		inline explicit IIndirectBuffer(IRenderer& renderer);

		explicit IIndirectBuffer(const IIndirectBuffer& source) = delete;
		IIndirectBuffer& operator =(const IIndirectBuffer& source) = delete;


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<IIndirectBuffer> IIndirectBufferPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/Buffer/IIndirectBuffer.inl"
