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
#include "Direct3D12Renderer/Buffer/VertexArray.h"
#include "Direct3D12Renderer/Buffer/IndexBuffer.h"
#include "Direct3D12Renderer/Buffer/VertexBuffer.h"
#include "Direct3D12Renderer/Mapping.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"
#include "Direct3D12Renderer/Direct3D12RuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArray::VertexArray(Direct3D12Renderer& direct3D12Renderer, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, IndexBuffer* indexBuffer) :
		IVertexArray(direct3D12Renderer),
		mIndexBuffer(indexBuffer),
		mNumberOfSlots(numberOfVertexBuffers),
		mD3D12VertexBufferViews(nullptr),
		mVertexBuffers(nullptr)
	{
		// Add a reference to the given index buffer
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->addReference();
		}

		// Add a reference to the used vertex buffers
		if (mNumberOfSlots > 0)
		{
			mD3D12VertexBufferViews = new D3D12_VERTEX_BUFFER_VIEW[mNumberOfSlots];
			mVertexBuffers = new VertexBuffer*[mNumberOfSlots];

			// Loop through all vertex buffers
			D3D12_VERTEX_BUFFER_VIEW* currentD3D12VertexBufferView = mD3D12VertexBufferViews;
			VertexBuffer **currentVertexBuffer = mVertexBuffers;
			const Renderer::VertexArrayVertexBuffer *vertexBufferEnd = vertexBuffers + mNumberOfSlots;
			for (const Renderer::VertexArrayVertexBuffer *vertexBuffer = vertexBuffers; vertexBuffer < vertexBufferEnd; ++vertexBuffer, ++currentD3D12VertexBufferView, ++currentVertexBuffer)
			{
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*currentVertexBuffer = static_cast<VertexBuffer*>(vertexBuffer->vertexBuffer);
				(*currentVertexBuffer)->addReference();
				currentD3D12VertexBufferView->BufferLocation = (*currentVertexBuffer)->getID3D12Resource()->GetGPUVirtualAddress();
				currentD3D12VertexBufferView->SizeInBytes = (*currentVertexBuffer)->getNumberOfBytes();
				currentD3D12VertexBufferView->StrideInBytes = vertexBuffer->strideInBytes;
			}
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("VAO");
		#endif
	}

	VertexArray::~VertexArray()
	{
		// Release the index buffer reference
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->releaseReference();
		}

		// Cleanup Direct3D 12 input slot data
		if (nullptr != mD3D12VertexBufferViews)
		{
			delete [] mD3D12VertexBufferViews;
		}

		// Release the reference to the used vertex buffers
		if (nullptr != mVertexBuffers)
		{
			// Release references
			VertexBuffer **vertexBuffersEnd = mVertexBuffers + mNumberOfSlots;
			for (VertexBuffer **vertexBuffer = mVertexBuffers; vertexBuffer < vertexBuffersEnd; ++vertexBuffer)
			{
				(*vertexBuffer)->releaseReference();
			}

			// Cleanup
			delete [] mVertexBuffers;
		}
	}

	void VertexArray::setDirect3DIASetInputLayoutAndStreamSource(ID3D12GraphicsCommandList& d3d12GraphicsCommandList) const
	{
		d3d12GraphicsCommandList.IASetVertexBuffers(0, mNumberOfSlots, mD3D12VertexBufferViews);

		// Get the used index buffer
		// -> In case of no index buffer we don't set null indices, there's not really a point in it
		if (nullptr != mIndexBuffer)
		{
			// Set the Direct3D 12 indices
			d3d12GraphicsCommandList.IASetIndexBuffer(&static_cast<IndexBuffer*>(mIndexBuffer)->getD3D12IndexBufferView());
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
