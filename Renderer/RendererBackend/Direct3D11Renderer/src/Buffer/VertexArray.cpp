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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/Buffer/VertexArray.h"
#include "Direct3D11Renderer/Buffer/IndexBuffer.h"
#include "Direct3D11Renderer/Buffer/VertexBuffer.h"
#include "Direct3D11Renderer/D3D11.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"

#include <Renderer/Buffer/VertexArrayTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArray::VertexArray(Direct3D11Renderer &direct3D11Renderer, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, IndexBuffer *indexBuffer) :
		IVertexArray(direct3D11Renderer),
		mD3D11DeviceContext(direct3D11Renderer.getD3D11DeviceContext()),
		mIndexBuffer(indexBuffer),
		mNumberOfSlots(numberOfVertexBuffers),
		mD3D11Buffers(nullptr),
		mStrides(nullptr),
		mOffsets(nullptr),
		mVertexBuffers(nullptr)
	{
		// Acquire our Direct3D 11 device context reference
		mD3D11DeviceContext->AddRef();

		// Add a reference to the given index buffer
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->addReference();
		}

		// Add a reference to the used vertex buffers
		if (mNumberOfSlots > 0)
		{
			mD3D11Buffers = new ID3D11Buffer*[mNumberOfSlots];
			mStrides = new UINT[mNumberOfSlots];
			mOffsets = new UINT[mNumberOfSlots];
			mVertexBuffers = new VertexBuffer*[mNumberOfSlots];

			// Vertex buffer offset is not supported by OpenGL, so our renderer API doesn't support it either
			memset(mOffsets, 0, sizeof(uint32_t) * mNumberOfSlots);

			// Loop through all vertex buffers
			ID3D11Buffer** currentD3D11Buffer = mD3D11Buffers;
			UINT* currentStride = mStrides;
			VertexBuffer **currentVertexBuffer = mVertexBuffers;
			const Renderer::VertexArrayVertexBuffer *vertexBufferEnd = vertexBuffers + mNumberOfSlots;
			for (const Renderer::VertexArrayVertexBuffer *vertexBuffer = vertexBuffers; vertexBuffer < vertexBufferEnd; ++vertexBuffer, ++currentD3D11Buffer, ++currentStride, ++currentVertexBuffer)
			{
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*currentStride = vertexBuffer->strideInBytes;
				*currentVertexBuffer = static_cast<VertexBuffer*>(vertexBuffer->vertexBuffer);
				*currentD3D11Buffer = (*currentVertexBuffer)->getD3D11Buffer();
				(*currentVertexBuffer)->addReference();
			}
		}
	}

	VertexArray::~VertexArray()
	{
		// Release the index buffer reference
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->releaseReference();
		}

		// Cleanup Direct3D 11 input slot data
		if (mNumberOfSlots > 0)
		{
			delete [] mD3D11Buffers;
			delete [] mStrides;
			delete [] mOffsets;
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

		// Release our Direct3D 11 device context reference
		mD3D11DeviceContext->Release();
	}

	void VertexArray::setDirect3DIASetInputLayoutAndStreamSource() const
	{
		// Set the Direct3D 11 vertex buffers
		if (nullptr != mD3D11Buffers)
		{
			// Just make a single API call
			mD3D11DeviceContext->IASetVertexBuffers(0, mNumberOfSlots, mD3D11Buffers, mStrides, mOffsets);
		}
		else
		{
			// Direct3D 10 says: "D3D10: INFO: ID3D10Device::IASetVertexBuffers: Since NumBuffers is 0, the operation effectively does nothing. This is probably not intentional, nor is the most efficient way to achieve this operation. Avoid calling the routine at all. [ STATE_SETTING INFO #240: DEVICE_IASETVERTEXBUFFERS_BUFFERS_EMPTY ]"
			// -> Direct3D 11 does not give us this message, but it's probably still no good thing to do
		}

		// Get the used index buffer
		// -> In case of no index buffer we don't set null indices, there's not really a point in it
		if (nullptr != mIndexBuffer)
		{
			// Set the Direct3D 11 indices
			mD3D11DeviceContext->IASetIndexBuffer(mIndexBuffer->getD3D11Buffer(), static_cast<DXGI_FORMAT>(mIndexBuffer->getDXGIFormat()), 0);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
