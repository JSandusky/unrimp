/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "Direct3D10Renderer/VertexArray.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/D3D10.h"
#include "Direct3D10Renderer/Mapping.h"
#include "Direct3D10Renderer/IndexBuffer.h"
#include "Direct3D10Renderer/VertexBuffer.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArray::VertexArray(Direct3D10Renderer &direct3D10Renderer, ID3DBlob &d3dBlob, uint32_t numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, IndexBuffer *indexBuffer) :
		IVertexArray(direct3D10Renderer),
		mD3D10Device(direct3D10Renderer.getD3D10Device()),
		mIndexBuffer(indexBuffer),
		mD3D10InputLayout(nullptr),
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
			VertexBuffer **currentVertexBuffer = mVertexBuffers;
			const Renderer::VertexArrayVertexBuffer *vertexBufferEnd = vertexBuffers + mNumberOfSlots;
			for (const Renderer::VertexArrayVertexBuffer *vertexBuffer = vertexBuffers; vertexBuffer < vertexBufferEnd; ++vertexBuffer, ++currentD3D10Buffer, ++currentStride, ++currentVertexBuffer)
			{
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*currentStride = vertexBuffer->strideInBytes;
				*currentVertexBuffer = static_cast<VertexBuffer*>(vertexBuffer->vertexBuffer);
				*currentD3D10Buffer = (*currentVertexBuffer)->getD3D10Buffer();
				(*currentVertexBuffer)->addReference();
			}
		}

		// Create Direct3D 10 input element descriptions
		// TODO(co) We could manage in here without new/delete when using a fixed maximum supported number of elements
		D3D10_INPUT_ELEMENT_DESC *d3d10InputElementDescs   = numberOfAttributes ? new D3D10_INPUT_ELEMENT_DESC[numberOfAttributes] : new D3D10_INPUT_ELEMENT_DESC[1];
		D3D10_INPUT_ELEMENT_DESC *d3d10InputElementDesc    = d3d10InputElementDescs;
		D3D10_INPUT_ELEMENT_DESC *d3d10InputElementDescEnd = d3d10InputElementDescs + numberOfAttributes;
		for (; d3d10InputElementDesc < d3d10InputElementDescEnd; ++d3d10InputElementDesc, ++attributes)
		{
			// Fill the "D3D10_INPUT_ELEMENT_DESC"-content
			d3d10InputElementDesc->SemanticName      = attributes->semanticName;																// Semantic name (LPCSTR)
			d3d10InputElementDesc->SemanticIndex     = attributes->semanticIndex;																// Semantic index (UINT)
			d3d10InputElementDesc->Format            = static_cast<DXGI_FORMAT>(Mapping::getDirect3D10Format(attributes->vertexArrayFormat));	// Format (DXGI_FORMAT)
			d3d10InputElementDesc->InputSlot         = static_cast<UINT>(attributes->inputSlot);												// Input slot (UINT)
			d3d10InputElementDesc->AlignedByteOffset = attributes->alignedByteOffset;															// Aligned byte offset (UINT)

			// Per-instance instead of per-vertex?
			if (attributes->instancesPerElement > 0)
			{
				d3d10InputElementDesc->InputSlotClass       = D3D10_INPUT_PER_INSTANCE_DATA;	// Input classification (D3D10_INPUT_CLASSIFICATION)
				d3d10InputElementDesc->InstanceDataStepRate = attributes->instancesPerElement;	// Instance data step rate (UINT)
			}
			else
			{
				d3d10InputElementDesc->InputSlotClass       = D3D10_INPUT_PER_VERTEX_DATA;	// Input classification (D3D10_INPUT_CLASSIFICATION)
				d3d10InputElementDesc->InstanceDataStepRate = 0;							// Instance data step rate (UINT)
			}
		}

		// Create the Direct3D 10 input layout
		direct3D10Renderer.getD3D10Device()->CreateInputLayout(d3d10InputElementDescs, numberOfAttributes, d3dBlob.GetBufferPointer(), d3dBlob.GetBufferSize(), &mD3D10InputLayout);

		// Destroy Direct3D 10 input element descriptions
		delete [] d3d10InputElementDescs;

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			setDebugName("VAO");
		#endif
	}

	VertexArray::~VertexArray()
	{
		// Release the index buffer reference
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->release();
		}

		// Release the Direct3D 10 input layout
		if (nullptr != mD3D10InputLayout)
		{
			mD3D10InputLayout->Release();
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
			VertexBuffer **vertexBuffersEnd = mVertexBuffers + mNumberOfSlots;
			for (VertexBuffer **vertexBuffer = mVertexBuffers; vertexBuffer < vertexBuffersEnd; ++vertexBuffer)
			{
				(*vertexBuffer)->release();
			}

			// Cleanup
			delete [] mVertexBuffers;
		}

		// Release our Direct3D 10 device reference
		mD3D10Device->Release();
	}

	void VertexArray::setDirect3DIASetInputLayoutAndStreamSource() const
	{
		// Valid Direct3D 10 input layout?
		if (nullptr != mD3D10InputLayout)
		{
			// Set the Direct3D 10 input layout
			mD3D10Device->IASetInputLayout(mD3D10InputLayout);

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
		}

		// Get the used index buffer
		// -> In case of no index buffer we don't set null indices, there's not really a point in it
		if (nullptr != mIndexBuffer)
		{
			// Set the Direct3D 10 indices
			mD3D10Device->IASetIndexBuffer(mIndexBuffer->getD3D10Buffer(), static_cast<DXGI_FORMAT>(mIndexBuffer->getDXGIFormat()), 0);
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void VertexArray::setDebugName(const char *name)
	{
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			// Valid Direct3D 10 input layout?
			if (nullptr != mD3D10InputLayout)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D10InputLayout->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10InputLayout->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
