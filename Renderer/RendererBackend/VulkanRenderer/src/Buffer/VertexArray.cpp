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
#include "VulkanRenderer/Buffer/VertexArray.h"
#include "VulkanRenderer/Buffer/IndexBuffer.h"
#include "VulkanRenderer/Buffer/VertexBuffer.h"
#include "VulkanRenderer/Extensions.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArray::VertexArray(VulkanRenderer &vulkanRenderer, const Renderer::VertexAttributes&, uint32_t, const Renderer::VertexArrayVertexBuffer*, IndexBuffer* indexBuffer) :
		IVertexArray(reinterpret_cast<Renderer::IRenderer&>(vulkanRenderer)),
		mIndexBuffer(indexBuffer)
	{
		// Add a reference to the given index buffer
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->addReference();
		}
	}

	VertexArray::~VertexArray()
	{
		// Release the index buffer reference
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->releaseReference();
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
