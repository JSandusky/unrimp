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
#include "Renderer/Buffer/IIndirectBuffer.h"
#include "Renderer/Buffer/IndirectBufferTypes.h"


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
	*    Indexed indirect buffer object implementation for wrapping none indirect indexed draw calls into the same uniform draw interface
	*/
	class IndexedIndirectBuffer : public IIndirectBuffer
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Owner renderer instance
		*  @param[in] indexCountPerInstance
		*    Index count per instance
		*  @param[in] instanceCount
		*    Instance count
		*  @param[in] startIndexLocation
		*    Start index location
		*  @param[in] baseVertexLocation
		*    Base vertex location
		*  @param[in] startInstanceLocation
		*    Start instance location
		*/
		inline IndexedIndirectBuffer(uint32_t indexCountPerInstance, uint32_t instanceCount = 1, uint32_t startIndexLocation = 0, int32_t baseVertexLocation = 0, uint32_t startInstanceLocation = 0);

		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~IndexedIndirectBuffer();


	//[-------------------------------------------------------]
	//[ Public virtual IIndirectBuffer methods                ]
	//[-------------------------------------------------------]
	public:
		inline virtual const uint8_t* getEmulationData() const override;
		inline virtual void copyDataFrom(uint32_t numberOfBytes, const void *data) override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		DrawIndexedInstancedArguments mDrawIndexedInstancedArguments;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/Buffer/IndexedIndirectBuffer.inl"
