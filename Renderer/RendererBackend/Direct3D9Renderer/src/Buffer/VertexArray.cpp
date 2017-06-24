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
#include "Direct3D9Renderer/Buffer/VertexArray.h"
#include "Direct3D9Renderer/Buffer/IndexBuffer.h"
#include "Direct3D9Renderer/Buffer/VertexBuffer.h"
#include "Direct3D9Renderer/d3d9.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"

#include <Renderer/Buffer/VertexArrayTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArray::VertexArray(Direct3D9Renderer& direct3D9Renderer, const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, IndexBuffer* indexBuffer) :
		IVertexArray(direct3D9Renderer),
		mDirect3DDevice9(direct3D9Renderer.getDirect3DDevice9()),
		mIndexBuffer(indexBuffer),
		mNumberOfSlots(numberOfVertexBuffers),
		mDirect3DVertexBuffer9(nullptr),
		mStrides(nullptr),
		mInstancesPerElement(nullptr),
		mVertexBuffers(nullptr)
	{
		// Acquire our Direct3D 9 device reference
		mDirect3DDevice9->AddRef();

		// Add a reference to the given index buffer
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->addReference();
		}

		// Add a reference to the used vertex buffers
		if (mNumberOfSlots > 0)
		{
			mDirect3DVertexBuffer9 = new IDirect3DVertexBuffer9*[mNumberOfSlots];
			mStrides = new uint32_t[mNumberOfSlots];
			mInstancesPerElement = new uint32_t[mNumberOfSlots];
			mVertexBuffers = new VertexBuffer*[mNumberOfSlots];

			// Loop through all vertex buffers
			IDirect3DVertexBuffer9** currentDirect3DVertexBuffer9 = mDirect3DVertexBuffer9;
			UINT* currentStride = mStrides;
			UINT* currentInstancesPerElement = mInstancesPerElement;
			VertexBuffer** currentVertexBuffer = mVertexBuffers;
			const Renderer::VertexArrayVertexBuffer* vertexBufferEnd = vertexBuffers + mNumberOfSlots;
			for (const Renderer::VertexArrayVertexBuffer* vertexBuffer = vertexBuffers; vertexBuffer < vertexBufferEnd; ++vertexBuffer, ++currentDirect3DVertexBuffer9, ++currentStride, ++currentInstancesPerElement, ++currentVertexBuffer)
			{
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*currentStride = vertexBuffer->strideInBytes;
				*currentInstancesPerElement = 0;
				*currentVertexBuffer = static_cast<VertexBuffer*>(vertexBuffer->vertexBuffer);
				*currentDirect3DVertexBuffer9 = (*currentVertexBuffer)->getDirect3DVertexBuffer9();
				(*currentVertexBuffer)->addReference();
			}

			// Gather instances per element
			// TODO(co) This will not work when multiple attributes using the same slot, but with a different setting. On the other hand, Direct3D 9 is totally out-of-date and this is just a proof-of-concept.
			const Renderer::VertexAttribute* attribute = vertexAttributes.attributes;
			const Renderer::VertexAttribute* attributesEnd = attribute + vertexAttributes.numberOfAttributes;
			for (; attribute < attributesEnd;  ++attribute)
			{
				mInstancesPerElement[attribute->inputSlot] = attribute->instancesPerElement;
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

		// Cleanup Direct3D 9 input slot data
		if (mNumberOfSlots > 0)
		{
			delete [] mDirect3DVertexBuffer9;
			delete [] mStrides;
			delete [] mInstancesPerElement;
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

		// Release our Direct3D 9 device reference
		mDirect3DDevice9->Release();
	}

	void VertexArray::enableDirect3DVertexDeclarationAndStreamSource() const
	{
		{ // Set the Direct3D 9 stream sources
			IDirect3DVertexBuffer9 **currentDirect3DVertexBuffer9AtSlot = mDirect3DVertexBuffer9;
			uint32_t* currentStrideAtSlot = mStrides;
			uint32_t* currentInstancesPerElementAtSlot = mInstancesPerElement;
			for (uint32_t slot = 0; slot < mNumberOfSlots; ++slot, ++currentDirect3DVertexBuffer9AtSlot, ++currentStrideAtSlot, ++currentInstancesPerElementAtSlot)
			{
				// Vertex buffer offset is not supported by OpenGL, so our renderer API doesn't support it either
				mDirect3DDevice9->SetStreamSource(slot, *currentDirect3DVertexBuffer9AtSlot, 0, *currentStrideAtSlot);

				// "D3DSTREAMSOURCE_INDEXEDDATA" is set within "Direct3D9Renderer::Direct3D9Renderer::DrawIndexedInstanced()"
				mDirect3DDevice9->SetStreamSourceFreq(1, (0 == *currentInstancesPerElementAtSlot) ? 1 : (D3DSTREAMSOURCE_INSTANCEDATA | *currentInstancesPerElementAtSlot));
			}
		}

		// Get the used index buffer
		// -> In case of no index buffer we don't set null indices, there's not really a point in it
		if (nullptr != mIndexBuffer)
		{
			// Set the Direct3D 9 indices
			mDirect3DDevice9->SetIndices(mIndexBuffer->getDirect3DIndexBuffer9());
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
