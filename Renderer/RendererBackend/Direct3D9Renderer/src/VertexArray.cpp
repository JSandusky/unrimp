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
#include "Direct3D9Renderer/VertexArray.h"
#include "Direct3D9Renderer/d3d9.h"
#include "Direct3D9Renderer/Mapping.h"
#include "Direct3D9Renderer/IndexBuffer.h"
#include "Direct3D9Renderer/VertexBuffer.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"

#include <Renderer/VertexArrayTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArray::VertexArray(Direct3D9Renderer &direct3D9Renderer, uint32_t numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, IndexBuffer *indexBuffer) :
		IVertexArray(direct3D9Renderer),
		mDirect3DDevice9(direct3D9Renderer.getDirect3DDevice9()),
		mIndexBuffer(indexBuffer),
		mDirect3DVertexDeclaration9(nullptr),
		mNumberOfSlots(0),
		mDirect3DVertexBuffer9(nullptr),
		mStrides(nullptr),
		mInstancesPerElement(nullptr),
		mNumberOfVertexBuffers(numberOfAttributes),
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

		// Get the maximum number of supported streams
		uint32_t maximumNumberOfStreams = 0;
		{
			// TODO(co) Optimization: Read this information only once and then backup the result
			// Get Direct3D 9 device capabilities and read the required information
			D3DCAPS9 d3dCaps9;
			mDirect3DDevice9->GetDeviceCaps(&d3dCaps9);
			maximumNumberOfStreams = d3dCaps9.MaxStreams;
		}

		// Vertex buffer at slot
		uint32_t numberOfUsedSlots = 0;
		IDirect3DVertexBuffer9 **direct3DVertexBuffer9AtSlot = new IDirect3DVertexBuffer9*[maximumNumberOfStreams];
		memset(direct3DVertexBuffer9AtSlot, 0, sizeof(IDirect3DVertexBuffer9*) * maximumNumberOfStreams);
		uint32_t *strideAtSlot = new uint32_t[maximumNumberOfStreams];
		memset(strideAtSlot, 0, sizeof(uint32_t) * maximumNumberOfStreams);
		uint32_t *instancesPerElementAtSlot = new uint32_t[maximumNumberOfStreams];
		memset(instancesPerElementAtSlot, 0, sizeof(uint32_t) * maximumNumberOfStreams);

		// Create Direct3D 9 vertex elements
		// -> While doing so, collect the per slot vertex buffer, stride and instances per element information
		D3DVERTEXELEMENT9 *d3dVertexElements   = new D3DVERTEXELEMENT9[numberOfAttributes + 1];	// +1 for D3DDECL_END()
		D3DVERTEXELEMENT9 *d3dVertexElement    = d3dVertexElements;
		D3DVERTEXELEMENT9 *d3dVertexElementEnd = d3dVertexElements + numberOfAttributes;
		for (; d3dVertexElement < d3dVertexElementEnd; ++d3dVertexElement, ++attributes)
		{
			// Determine the Direct3D 9 input slot to use
			IDirect3DVertexBuffer9 **currentDirect3DVertexBuffer9AtSlot = direct3DVertexBuffer9AtSlot;
			UINT *currentStrideAtSlot = strideAtSlot;
			UINT *currentInstancesPerElementAtSlot = instancesPerElementAtSlot;
			int slotToUse = -1;
			for (uint32_t slot = 0; slot < numberOfUsedSlots; ++slot, ++currentDirect3DVertexBuffer9AtSlot, ++currentStrideAtSlot, ++currentInstancesPerElementAtSlot)
			{
				// Vertex buffer, stride and instances per element match?
				if (*currentDirect3DVertexBuffer9AtSlot == static_cast<VertexBuffer*>(attributes->vertexBuffer)->getDirect3DVertexBuffer9() &&
					*currentStrideAtSlot == attributes->stride && *currentInstancesPerElementAtSlot == attributes->instancesPerElement)
				{
					// Jap, this attribute can use an already used Direct3D 9 input slot
					slotToUse = static_cast<int>(slot);

					// Get us out of this loop right now
					slot = numberOfUsedSlots;
				}
			}
			if (slotToUse < 0)
			{
				// No Direct3D 9 input slot can be reused, so use a new slot

				// Are there still free Direct3D 9 input slots available?
				if (numberOfUsedSlots < maximumNumberOfStreams)
				{
					// Add the new Direct3D 9 input slot
					slotToUse = static_cast<int>(numberOfUsedSlots);
					// TODO(co) Add security check: Is the given resource one of the currently used renderer?
					direct3DVertexBuffer9AtSlot[slotToUse] = static_cast<VertexBuffer*>(attributes->vertexBuffer)->getDirect3DVertexBuffer9();
					strideAtSlot[slotToUse] = attributes->stride;
					instancesPerElementAtSlot[slotToUse] = attributes->instancesPerElement;
					++numberOfUsedSlots;
				}
				else
				{
					// Error! Clean up and get us out of here right now!
					delete [] d3dVertexElements;
					return;	// Ugly return, but we haven't much choice in here
				}
			}

			// Fill the "D3DVERTEXELEMENT9"-content
			d3dVertexElement->Stream     = static_cast<WORD>(slotToUse);												// Stream index (WORD)
			d3dVertexElement->Offset     = static_cast<WORD>(attributes->alignedByteOffset);							// Offset in the stream in bytes (WORD)
			d3dVertexElement->Type       = Mapping::getDirect3D9Type(attributes->vertexArrayFormat);					// Data type (BYTE)
			d3dVertexElement->Method     = D3DDECLMETHOD_DEFAULT;														// Processing method (BYTE)
			d3dVertexElement->Usage      = static_cast<BYTE>(Mapping::getDirect3D9Semantic(attributes->semanticName));	// Semantic name (BYTE)
			d3dVertexElement->UsageIndex = static_cast<BYTE>(attributes->semanticIndex);								// Semantic index (BYTE)
		}
		// D3DDECL_END()
		d3dVertexElement->Stream     = 0xFF;				// Stream index (WORD)
		d3dVertexElement->Offset     = 0;					// Offset in the stream in bytes (WORD)
		d3dVertexElement->Type       = D3DDECLTYPE_UNUSED;	// Data type (BYTE)
		d3dVertexElement->Method     = 0;					// Processing method (BYTE)
		d3dVertexElement->Usage      = 0;					// Semantics (BYTE)
		d3dVertexElement->UsageIndex = 0;					// Semantic index (BYTE)

		// Create the Direct3D 9 vertex declaration
		direct3D9Renderer.getDirect3DDevice9()->CreateVertexDeclaration(d3dVertexElements, &mDirect3DVertexDeclaration9);

		// Destroy Direct3D 9 vertex elements
		delete [] d3dVertexElements;

		// Valid Direct3D 9 input layout?
		if (nullptr != mDirect3DVertexDeclaration9 && numberOfAttributes > 0)
		{
			// Allocate final memory for the Direct3D 9 input slot data
			mNumberOfSlots = numberOfUsedSlots;

			// Buffers
			mDirect3DVertexBuffer9 = new IDirect3DVertexBuffer9*[mNumberOfSlots];
			memcpy(mDirect3DVertexBuffer9, direct3DVertexBuffer9AtSlot, sizeof(IDirect3DVertexBuffer9*) * mNumberOfSlots);

			// Strides
			mStrides = new uint32_t[mNumberOfSlots];
			memcpy(mStrides, strideAtSlot, sizeof(uint32_t) * mNumberOfSlots);

			// Instances per element
			mInstancesPerElement = new uint32_t[mNumberOfSlots];
			memcpy(mInstancesPerElement, instancesPerElementAtSlot, sizeof(uint32_t) * mNumberOfSlots);
		}

		// Cleanup
		delete [] instancesPerElementAtSlot;
		delete [] strideAtSlot;
		delete [] direct3DVertexBuffer9AtSlot;
	}

	VertexArray::~VertexArray()
	{
		// Release the index buffer reference
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->release();
		}

		// Release the Direct3D 9 vertex declaration
		if (nullptr != mDirect3DVertexDeclaration9)
		{
			mDirect3DVertexDeclaration9->Release();
		}

		// Cleanup Direct3D 9 input slot data
		if (nullptr != mDirect3DVertexBuffer9)
		{
			delete [] mDirect3DVertexBuffer9;
			delete [] mStrides;
			delete [] mInstancesPerElement;
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

		// Release our Direct3D 9 device reference
		mDirect3DDevice9->Release();
	}

	void VertexArray::enableDirect3DVertexDeclarationAndStreamSource() const
	{
		// Valid Direct3D 9 vertex declaration?
		if (nullptr != mDirect3DVertexDeclaration9)
		{
			// Set the Direct3D 9 vertex declaration
			mDirect3DDevice9->SetVertexDeclaration(mDirect3DVertexDeclaration9);

			// Set the Direct3D 9 stream sources
			IDirect3DVertexBuffer9 **currentDirect3DVertexBuffer9AtSlot = mDirect3DVertexBuffer9;
			uint32_t *currentStrideAtSlot = mStrides;
			uint32_t *currentInstancesPerElementAtSlot = mInstancesPerElement;
			for (uint32_t slot = 0; slot < mNumberOfSlots; ++slot, ++currentDirect3DVertexBuffer9AtSlot, ++currentStrideAtSlot, ++currentInstancesPerElementAtSlot)
			{
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
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void VertexArray::setDebugName(const char *)
	{
		// "IDirect3DVertexDeclaration9" is not derived from "IDirect3DResource9", meaning we can't use the "IDirect3DResource9::SetPrivateData()"-method
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
