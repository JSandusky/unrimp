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
#include "OpenGLES2Renderer/Buffer/IndirectBuffer.h"
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"

#include <cassert>
#include <cstring>	// For "memcpy"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IndirectBuffer::IndirectBuffer(OpenGLES2Renderer& openGLES2Renderer, uint32_t numberOfBytes, const void* data) :
		IIndirectBuffer(openGLES2Renderer),
		mNumberOfBytes(numberOfBytes),
		mData(nullptr)
	{
		if (mNumberOfBytes > 0)
		{
			mData = new uint8_t[mNumberOfBytes];
			if (nullptr != data)
			{
				memcpy(mData, data, mNumberOfBytes);
			}
		}
		else
		{
			assert(nullptr == data);
		}
	}

	IndirectBuffer::~IndirectBuffer()
	{
		if (nullptr != mData)
		{
			delete [] mData;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IIndirectBuffer methods      ]
	//[-------------------------------------------------------]
	void IndirectBuffer::copyDataFrom(uint32_t numberOfBytes, const void* data)
	{
		assert(numberOfBytes <= mNumberOfBytes);
		assert(nullptr != data);
		memcpy(mData, data, numberOfBytes);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
