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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D9Renderer/Buffer/IndirectBuffer.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"

#include <Renderer/IAssert.h>
#include <Renderer/IAllocator.h>

#include <cstring>	// For "memcpy()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IndirectBuffer::IndirectBuffer(Direct3D9Renderer& direct3D9Renderer, uint32_t numberOfBytes, const void* data) :
		IIndirectBuffer(direct3D9Renderer),
		mNumberOfBytes(numberOfBytes),
		mData(nullptr)
	{
		if (mNumberOfBytes > 0)
		{
			mData = RENDERER_MALLOC_TYPED(direct3D9Renderer.getContext(), uint8_t, mNumberOfBytes);
			if (nullptr != data)
			{
				memcpy(mData, data, mNumberOfBytes);
			}
		}
		else
		{
			RENDERER_ASSERT(direct3D9Renderer.getContext(), nullptr == data, "Invalid Direct3D 9 indirect buffer data")
		}
	}

	IndirectBuffer::~IndirectBuffer()
	{
		RENDERER_FREE(getRenderer().getContext(), mData);
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void IndirectBuffer::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), IndirectBuffer, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
