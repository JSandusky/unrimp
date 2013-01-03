/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/VertexArray.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/D3D11.h"
#include "Direct3D11Renderer/Mapping.h"
#include "Direct3D11Renderer/IndexBuffer.h"
#include "Direct3D11Renderer/VertexBuffer.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	VertexArray::VertexArray(Direct3D11Renderer &direct3D11Renderer, ID3DBlob &d3dBlob, unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, IndexBuffer *indexBuffer) :
		IVertexArray(direct3D11Renderer),
		mD3D11DeviceContext(direct3D11Renderer.getD3D11DeviceContext()),
		mIndexBuffer(indexBuffer),
		mD3D11InputLayout(nullptr),
		mNumberOfSlots(0),
		mD3D11Buffers(nullptr),
		mStrides(nullptr),
		mOffsets(nullptr),
		mNumberOfVertexBuffers(numberOfAttributes),
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
		if (numberOfAttributes > 0)
		{
			mVertexBuffers = new VertexBuffer*[numberOfAttributes];

			// Loop through all attributes
			VertexBuffer **vertexBuffers = mVertexBuffers;
			const Renderer::VertexArrayAttribute *attributeEnd = attributes + numberOfAttributes;
			for (const Renderer::VertexArrayAttribute *attribute = attributes; attribute < attributeEnd; ++attribute, ++vertexBuffers)
			{
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*vertexBuffers = static_cast<VertexBuffer*>(attribute->vertexBuffer);
				(*vertexBuffers)->addReference();
			}
		}

		// Vertex buffer at slot
		unsigned int numberOfUsedSlots = 0;
		ID3D11Buffer *d3d11BufferAtSlot[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		memset(d3d11BufferAtSlot, 0, sizeof(ID3D11Buffer*) * D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
		UINT strideAtSlot[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		memset(strideAtSlot, 0, sizeof(UINT) * D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);

		// Create Direct3D 11 input element descriptions
		// -> While doing so, collect the per slot vertex buffer and stride information
		// -> Direct3D 11 don't like it when the first parameter of "D3D10Device::CreateInputLayout()" is a null pointer:
		//      "D3D11: ERROR: ID3D11Device::CreateInputLayout: NULL pDesc specified! [ STATE_CREATION ERROR #164: CREATEINPUTLAYOUT_NULLDESC ]"
		//    Handle this case.
		D3D11_INPUT_ELEMENT_DESC *d3d11InputElementDescs   = numberOfAttributes ? new D3D11_INPUT_ELEMENT_DESC[numberOfAttributes] : new D3D11_INPUT_ELEMENT_DESC[1];
		D3D11_INPUT_ELEMENT_DESC *d3d11InputElementDesc    = d3d11InputElementDescs;
		D3D11_INPUT_ELEMENT_DESC *d3d11InputElementDescEnd = d3d11InputElementDescs + numberOfAttributes;
		for (; d3d11InputElementDesc < d3d11InputElementDescEnd; ++d3d11InputElementDesc, ++attributes)
		{
			// Determine the Direct3D 11 input slot to use
			ID3D11Buffer **currentD3D11BufferAtSlot = d3d11BufferAtSlot;
			UINT *currentStrideAtSlot = strideAtSlot;
			int slotToUse = -1;
			for (unsigned int slot = 0; slot < numberOfUsedSlots; ++slot, ++currentD3D11BufferAtSlot, ++currentStrideAtSlot)
			{
				// Vertex buffer and stride match?
				if (*currentD3D11BufferAtSlot == static_cast<VertexBuffer*>(attributes->vertexBuffer)->getD3D11Buffer() && *currentStrideAtSlot == attributes->stride)
				{
					// Jap, this attribute can use an already used Direct3D 11 input slot
					slotToUse = static_cast<int>(slot);

					// Get us out of this loop right now
					slot = numberOfUsedSlots;
				}
			}
			if (slotToUse < 0)
			{
				// No Direct3D 11 input slot can be reused, so use a new slot

				// Are there still free Direct3D 11 input slots available?
				if (numberOfUsedSlots < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT)
				{
					// Add the new Direct3D 11 input slot
					slotToUse = static_cast<int>(numberOfUsedSlots);
					// TODO(co) Add security check: Is the given resource one of the currently used renderer?
					d3d11BufferAtSlot[slotToUse] = static_cast<VertexBuffer*>(attributes->vertexBuffer)->getD3D11Buffer();
					strideAtSlot[slotToUse] = attributes->stride;
					++numberOfUsedSlots;
				}
				else
				{
					// Error! Clean up and get us out of here right now!
					delete [] d3d11InputElementDescs;
					return;	// Ugly return, but we haven't much choice in here
				}
			}

			// Fill the "D3D11_INPUT_ELEMENT_DESC"-content
			d3d11InputElementDesc->SemanticName      = attributes->semantic;																	// Semantic name (LPCSTR)
			d3d11InputElementDesc->SemanticIndex     = attributes->semanticIndex;																// Semantic index (UINT)
			d3d11InputElementDesc->Format            = static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(attributes->vertexArrayFormat));	// Format (DXGI_FORMAT)
			d3d11InputElementDesc->InputSlot         = static_cast<UINT>(slotToUse);															// Input slow (UINT)
			d3d11InputElementDesc->AlignedByteOffset = attributes->offset;																		// Aligned byte offset (UINT)

			// Per-instance instead of per-vertex?
			if (attributes->instancesPerElement > 0)
			{
				d3d11InputElementDesc->InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA;	// Input classification (D3D11_INPUT_CLASSIFICATION)
				d3d11InputElementDesc->InstanceDataStepRate = attributes->instancesPerElement;	// Instance data step rate (UINT)
			}
			else
			{
				d3d11InputElementDesc->InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;	// Input classification (D3D11_INPUT_CLASSIFICATION)
				d3d11InputElementDesc->InstanceDataStepRate = 0;							// Instance data step rate (UINT)
			}
		}

		// Create the Direct3D 11 input layout
		direct3D11Renderer.getD3D11Device()->CreateInputLayout(d3d11InputElementDescs, numberOfAttributes, d3dBlob.GetBufferPointer(), d3dBlob.GetBufferSize(), &mD3D11InputLayout);

		// Destroy Direct3D 11 input element descriptions
		delete [] d3d11InputElementDescs;

		// Valid Direct3D 11 input layout?
		if (nullptr != mD3D11InputLayout && numberOfAttributes > 0)
		{
			// Allocate final memory for the Direct3D 11 input slot data
			mNumberOfSlots = numberOfUsedSlots;

			// Buffers
			mD3D11Buffers = new ID3D11Buffer*[mNumberOfSlots];
			memcpy(mD3D11Buffers, d3d11BufferAtSlot, sizeof(ID3D11Buffer*) * mNumberOfSlots);

			// Strides
			mStrides = new UINT[mNumberOfSlots];
			memcpy(mStrides, strideAtSlot, sizeof(UINT) * mNumberOfSlots);

			// Offsets
			mOffsets = new UINT[mNumberOfSlots];
			memset(mOffsets, 0, sizeof(UINT) * mNumberOfSlots);
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			setDebugName("VAO");
		#endif
	}

	/**
	*  @brief
	*    Destructor
	*/
	VertexArray::~VertexArray()
	{
		// Release the index buffer reference
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->release();
		}

		// Release the Direct3D 11 input layout
		if (nullptr != mD3D11InputLayout)
		{
			mD3D11InputLayout->Release();
		}

		// Cleanup Direct3D 11 input slot data
		if (nullptr != mD3D11Buffers)
		{
			delete [] mD3D11Buffers;
			delete [] mStrides;
			delete [] mOffsets;
		}

		// Release the reference to the used vertex buffers
		if (nullptr != mVertexBuffers)
		{
			// Release references
			VertexBuffer **vertexBuffersEnd = mVertexBuffers + mNumberOfVertexBuffers;
			for (VertexBuffer **vertexBuffer = mVertexBuffers; vertexBuffer < vertexBuffersEnd; ++vertexBuffer)
			{
				(*vertexBuffer)->release();
			}

			// Cleanup
			delete [] mVertexBuffers;
		}

		// Release our Direct3D 11 device context reference
		mD3D11DeviceContext->Release();
	}

	/**
	*  @brief
	*    Set the Direct3D 11 vertex declaration and stream source
	*/
	void VertexArray::setDirect3DIASetInputLayoutAndStreamSource() const
	{
		// Valid Direct3D 11 input layout?
		if (nullptr != mD3D11InputLayout)
		{
			// Set the Direct3D 11 input layout
			mD3D11DeviceContext->IASetInputLayout(mD3D11InputLayout);

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
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void VertexArray::setDebugName(const char *name)
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			// Valid Direct3D 11 input layout?
			if (nullptr != mD3D11InputLayout)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D11InputLayout->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D11InputLayout->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
