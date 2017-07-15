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
#include "Direct3D10Renderer/Buffer/VertexArray.h"
#include "Direct3D10Renderer/Buffer/IndexBuffer.h"
#include "Direct3D10Renderer/Buffer/VertexBuffer.h"
#include "Direct3D10Renderer/D3D10.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"

#include <Renderer/Buffer/VertexArrayTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArray::VertexArray(Direct3D10Renderer& direct3D10Renderer, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, IndexBuffer* indexBuffer) :
		IVertexArray(direct3D10Renderer),
		mD3D10Device(direct3D10Renderer.getD3D10Device()),
		mIndexBuffer(indexBuffer),
		mNumberOfSlots(numberOfVertexBuffers),
		mD3D10Buffers(nullptr),
		mStrides(nullptr),
		mOffsets(nullptr),
		mVertexBuffers(nullptr)
	{
		// Acquire our Direct3D 10 device reference
		mD3D10Device->AddRef();

		// Add a reference to the given index buffer
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->addReference();
		}

		// Add a reference to the used vertex buffers
		if (mNumberOfSlots > 0)
		{
			mD3D10Buffers = new ID3D10Buffer*[mNumberOfSlots];
			mStrides = new UINT[mNumberOfSlots];
			mOffsets = new UINT[mNumberOfSlots];
			mVertexBuffers = new VertexBuffer*[mNumberOfSlots];

			// Vertex buffer offset is not supported by OpenGL, so our renderer API doesn't support it either
			memset(mOffsets, 0, sizeof(uint32_t) * mNumberOfSlots);

			// Loop through all vertex buffers
			ID3D10Buffer** currentD3D10Buffer = mD3D10Buffers;
			UINT* currentStride = mStrides;
			VertexBuffer** currentVertexBuffer = mVertexBuffers;
			const Renderer::VertexArrayVertexBuffer* vertexBufferEnd = vertexBuffers + mNumberOfSlots;
			for (const Renderer::VertexArrayVertexBuffer* vertexBuffer = vertexBuffers; vertexBuffer < vertexBufferEnd; ++vertexBuffer, ++currentD3D10Buffer, ++currentStride, ++currentVertexBuffer)
			{
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*currentStride = vertexBuffer->strideInBytes;
				*currentVertexBuffer = static_cast<VertexBuffer*>(vertexBuffer->vertexBuffer);
				*currentD3D10Buffer = (*currentVertexBuffer)->getD3D10Buffer();
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

		// Cleanup Direct3D 10 input slot data
		if (mNumberOfSlots > 0)
		{
			delete [] mD3D10Buffers;
			delete [] mStrides;
			delete [] mOffsets;
		}

		// Release the reference to the used vertex buffers
		if (nullptr != mVertexBuffers)
		{
			// Release references
			VertexBuffer** vertexBuffersEnd = mVertexBuffers + mNumberOfSlots;
			for (VertexBuffer** vertexBuffer = mVertexBuffers; vertexBuffer < vertexBuffersEnd; ++vertexBuffer)
			{
				(*vertexBuffer)->releaseReference();
			}

			// Cleanup
			delete [] mVertexBuffers;
		}

		// Release our Direct3D 10 device reference
		mD3D10Device->Release();
	}

	void VertexArray::setDirect3DIASetInputLayoutAndStreamSource() const
	{
		// Set the Direct3D 10 vertex buffers
		if (nullptr != mD3D10Buffers)
		{
			// Just make a single API call
			mD3D10Device->IASetVertexBuffers(0, mNumberOfSlots, mD3D10Buffers, mStrides, mOffsets);
		}
		else
		{
			// Direct3D 10 says: "D3D10: INFO: ID3D10Device::IASetVertexBuffers: Since NumBuffers is 0, the operation effectively does nothing. This is probably not intentional, nor is the most efficient way to achieve this operation. Avoid calling the routine at all. [ STATE_SETTING INFO #240: DEVICE_IASETVERTEXBUFFERS_BUFFERS_EMPTY ]"
			// -> Direct3D 10 does not give us this message, but it's probably still no good thing to do
		}

		// Set the used index buffer
		// -> In case of no index buffer we don't set null indices, there's not really a point in it
		if (nullptr != mIndexBuffer)
		{
			// Set the Direct3D 10 indices
			mD3D10Device->IASetIndexBuffer(mIndexBuffer->getD3D10Buffer(), static_cast<DXGI_FORMAT>(mIndexBuffer->getDXGIFormat()), 0);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
