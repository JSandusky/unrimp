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
	VertexArray::VertexArray(Direct3D9Renderer &direct3D9Renderer, uint32_t numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, IndexBuffer *indexBuffer) :
		IVertexArray(direct3D9Renderer),
		mDirect3DDevice9(direct3D9Renderer.getDirect3DDevice9()),
		mIndexBuffer(indexBuffer),
		mDirect3DVertexDeclaration9(nullptr),
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
			VertexBuffer **currentVertexBuffer = mVertexBuffers;
			const Renderer::VertexArrayVertexBuffer *vertexBufferEnd = vertexBuffers + mNumberOfSlots;
			for (const Renderer::VertexArrayVertexBuffer *vertexBuffer = vertexBuffers; vertexBuffer < vertexBufferEnd; ++vertexBuffer, ++currentDirect3DVertexBuffer9, ++currentStride, ++currentInstancesPerElement, ++currentVertexBuffer)
			{
				// TODO(co) Add security check: Is the given resource one of the currently used renderer?
				*currentStride = vertexBuffer->strideInBytes;
				*currentInstancesPerElement = 0;
				*currentVertexBuffer = static_cast<VertexBuffer*>(vertexBuffer->vertexBuffer);
				*currentDirect3DVertexBuffer9 = (*currentVertexBuffer)->getDirect3DVertexBuffer9();
				(*currentVertexBuffer)->addReference();
			}
		}

		// Create Direct3D 9 vertex elements
		// TODO(co) We could manage in here without new/delete when using a fixed maximum supported number of elements
		D3DVERTEXELEMENT9 *d3dVertexElements   = new D3DVERTEXELEMENT9[numberOfAttributes + 1];	// +1 for D3DDECL_END()
		D3DVERTEXELEMENT9 *d3dVertexElement    = d3dVertexElements;
		D3DVERTEXELEMENT9 *d3dVertexElementEnd = d3dVertexElements + numberOfAttributes;
		for (; d3dVertexElement < d3dVertexElementEnd; ++d3dVertexElement, ++attributes)
		{
			// Fill the "D3DVERTEXELEMENT9"-content
			d3dVertexElement->Stream     = static_cast<WORD>(attributes->inputSlot);									// Stream index (WORD)
			d3dVertexElement->Offset     = static_cast<WORD>(attributes->alignedByteOffset);							// Offset in the stream in bytes (WORD)
			d3dVertexElement->Type       = Mapping::getDirect3D9Type(attributes->vertexArrayFormat);					// Data type (BYTE)
			d3dVertexElement->Method     = D3DDECLMETHOD_DEFAULT;														// Processing method (BYTE)
			d3dVertexElement->Usage      = static_cast<BYTE>(Mapping::getDirect3D9Semantic(attributes->semanticName));	// Semantic name (BYTE)
			d3dVertexElement->UsageIndex = static_cast<BYTE>(attributes->semanticIndex);								// Semantic index (BYTE)

			// Set instances per element
			// TODO(co) This will not work when multiple attributes using the same slot, but with a different setting. On the other hand, Direct3D 9 is totally out-of-date and this is just a proof-of-concept.
			mInstancesPerElement[attributes->inputSlot] = attributes->instancesPerElement;
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
			VertexBuffer **vertexBuffersEnd = mVertexBuffers + mNumberOfSlots;
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
