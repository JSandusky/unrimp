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
#include "Direct3D12Renderer/VertexArray.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/D3D12X.h"
#include "Direct3D12Renderer/Mapping.h"
#include "Direct3D12Renderer/IndexBuffer.h"
#include "Direct3D12Renderer/VertexBuffer.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"
#include "Direct3D12Renderer/Direct3D12RuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{

	// TODO(co) Just a first test
	ID3DBlob* g_D3dBlobErrorVertexShader = nullptr;
	ID3DBlob* g_D3dBlobErrorFragmentShader = nullptr;
	VertexBuffer* vertexBuffer = nullptr;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArray::VertexArray(Direct3D12Renderer &direct3D12Renderer, ID3DBlob &, uint32_t numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, IndexBuffer *indexBuffer) :
		IVertexArray(direct3D12Renderer),
		mD3D12RootSignature(nullptr),
		mD3D12PipelineState(nullptr),
	//	mD3D12DeviceContext(direct3D12Renderer.getD3D12DeviceContext()),	// TODO(co) Direct3D 12 update
		mIndexBuffer(indexBuffer),
	//	mD3D12InputLayout(nullptr),	// TODO(co) Direct3D 12 update
		mNumberOfSlots(0),
	//	mD3D12Buffers(nullptr),	// TODO(co) Direct3D 12 update
		mStrides(nullptr),
		mOffsets(nullptr),
		mNumberOfVertexBuffers(numberOfAttributes),
		mVertexBuffers(nullptr)
	{
		// TODO(co) This is only for the Direct3D 12 renderer backend kickoff. I'am sure the root signature must be integrated in another more efficient way.
		{ // Create an empty root signature
			CD3DX12_ROOT_SIGNATURE_DESC d3d12XRootSignatureDesc;
			d3d12XRootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ID3DBlob* d3dBlobSignature = nullptr;
			ID3DBlob* d3dBlobError = nullptr;
			if (SUCCEEDED(D3D12SerializeRootSignature(&d3d12XRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &d3dBlobSignature, &d3dBlobError)))
			{
				if (FAILED(direct3D12Renderer.getD3D12Device()->CreateRootSignature(0, d3dBlobSignature->GetBufferPointer(), d3dBlobSignature->GetBufferSize(), IID_PPV_ARGS(&mD3D12RootSignature))))
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create the root signature instance")
				}
				d3dBlobSignature->Release();
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create the root signature instance")
				d3dBlobError->Release();
			}
		}

		// Initialize the vertex buffer view
		vertexBuffer = static_cast<VertexBuffer*>(attributes->vertexBuffer);
		vertexBuffer->addReference();
		mD3D12VertexBufferView.BufferLocation = vertexBuffer->getID3D12Resource()->GetGPUVirtualAddress();
		mD3D12VertexBufferView.StrideInBytes = attributes->stride;
		mD3D12VertexBufferView.SizeInBytes = attributes->stride * 3;	// TODO(co) Number of bytes inside the vertex buffer

		{ // TODO(co) Just a first test
			// Define the vertex input layout.
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			// Describe and create the graphics pipeline state object (PSO).
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
			psoDesc.pRootSignature = mD3D12RootSignature;
			psoDesc.VS = { reinterpret_cast<UINT8*>(g_D3dBlobErrorVertexShader->GetBufferPointer()), g_D3dBlobErrorVertexShader->GetBufferSize() };
			psoDesc.PS = { reinterpret_cast<UINT8*>(g_D3dBlobErrorFragmentShader->GetBufferPointer()), g_D3dBlobErrorFragmentShader->GetBufferSize() };
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.StencilEnable = FALSE;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;
			if (FAILED(direct3D12Renderer.getD3D12Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mD3D12PipelineState))))
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create the pipeline state object")
			}
		}

		// TODO(co) Direct3D 12 update
		/*
		// Acquire our Direct3D 12 device context reference
		mD3D12DeviceContext->AddRef();

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
		uint32_t numberOfUsedSlots = 0;
		ID3D12Buffer *d3d12BufferAtSlot[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		memset(d3d12BufferAtSlot, 0, sizeof(ID3D12Buffer*) * D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
		UINT strideAtSlot[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		memset(strideAtSlot, 0, sizeof(UINT) * D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);

		// Create Direct3D 12 input element descriptions
		// -> While doing so, collect the per slot vertex buffer and stride information
		// -> Direct3D 12 don't like it when the first parameter of "D3D10Device::CreateInputLayout()" is a null pointer:
		//      "D3D12: ERROR: ID3D12Device::CreateInputLayout: NULL pDesc specified! [ STATE_CREATION ERROR #164: CREATEINPUTLAYOUT_NULLDESC ]"
		//    Handle this case.
		D3D12_INPUT_ELEMENT_DESC *d3d12InputElementDescs   = numberOfAttributes ? new D3D12_INPUT_ELEMENT_DESC[numberOfAttributes] : new D3D12_INPUT_ELEMENT_DESC[1];
		D3D12_INPUT_ELEMENT_DESC *d3d12InputElementDesc    = d3d12InputElementDescs;
		D3D12_INPUT_ELEMENT_DESC *d3d12InputElementDescEnd = d3d12InputElementDescs + numberOfAttributes;
		for (; d3d12InputElementDesc < d3d12InputElementDescEnd; ++d3d12InputElementDesc, ++attributes)
		{
			// Determine the Direct3D 12 input slot to use
			ID3D12Buffer **currentD3D12BufferAtSlot = d3d12BufferAtSlot;
			UINT *currentStrideAtSlot = strideAtSlot;
			int slotToUse = -1;
			for (uint32_t slot = 0; slot < numberOfUsedSlots; ++slot, ++currentD3D12BufferAtSlot, ++currentStrideAtSlot)
			{
				// Vertex buffer and stride match?
				if (*currentD3D12BufferAtSlot == static_cast<VertexBuffer*>(attributes->vertexBuffer)->getD3D12Buffer() && *currentStrideAtSlot == attributes->stride)
				{
					// Jap, this attribute can use an already used Direct3D 12 input slot
					slotToUse = static_cast<int>(slot);

					// Get us out of this loop right now
					slot = numberOfUsedSlots;
				}
			}
			if (slotToUse < 0)
			{
				// No Direct3D 12 input slot can be reused, so use a new slot

				// Are there still free Direct3D 12 input slots available?
				if (numberOfUsedSlots < D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT)
				{
					// Add the new Direct3D 12 input slot
					slotToUse = static_cast<int>(numberOfUsedSlots);
					// TODO(co) Add security check: Is the given resource one of the currently used renderer?
					d3d12BufferAtSlot[slotToUse] = static_cast<VertexBuffer*>(attributes->vertexBuffer)->getD3D12Buffer();
					strideAtSlot[slotToUse] = attributes->stride;
					++numberOfUsedSlots;
				}
				else
				{
					// Error! Clean up and get us out of here right now!
					delete [] d3d12InputElementDescs;
					return;	// Ugly return, but we haven't much choice in here
				}
			}

			// Fill the "D3D12_INPUT_ELEMENT_DESC"-content
			d3d12InputElementDesc->SemanticName      = attributes->semanticName;																// Semantic name (LPCSTR)
			d3d12InputElementDesc->SemanticIndex     = attributes->semanticIndex;																// Semantic index (UINT)
			d3d12InputElementDesc->Format            = static_cast<DXGI_FORMAT>(Mapping::getDirect3D12Format(attributes->vertexArrayFormat));	// Format (DXGI_FORMAT)
			d3d12InputElementDesc->InputSlot         = static_cast<UINT>(slotToUse);															// Input slot (UINT)
			d3d12InputElementDesc->AlignedByteOffset = attributes->alignedByteOffset;															// Aligned byte offset (UINT)

			// Per-instance instead of per-vertex?
			if (attributes->instancesPerElement > 0)
			{
				d3d12InputElementDesc->InputSlotClass       = D3D12_INPUT_PER_INSTANCE_DATA;	// Input classification (D3D12_INPUT_CLASSIFICATION)
				d3d12InputElementDesc->InstanceDataStepRate = attributes->instancesPerElement;	// Instance data step rate (UINT)
			}
			else
			{
				d3d12InputElementDesc->InputSlotClass       = D3D12_INPUT_PER_VERTEX_DATA;	// Input classification (D3D12_INPUT_CLASSIFICATION)
				d3d12InputElementDesc->InstanceDataStepRate = 0;							// Instance data step rate (UINT)
			}
		}

		// Create the Direct3D 12 input layout
		direct3D12Renderer.getD3D12Device()->CreateInputLayout(d3d12InputElementDescs, numberOfAttributes, d3dBlob.GetBufferPointer(), d3dBlob.GetBufferSize(), &mD3D12InputLayout);

		// Destroy Direct3D 12 input element descriptions
		delete [] d3d12InputElementDescs;

		// Valid Direct3D 12 input layout?
		if (nullptr != mD3D12InputLayout && numberOfAttributes > 0)
		{
			// Allocate final memory for the Direct3D 12 input slot data
			mNumberOfSlots = numberOfUsedSlots;

			// Buffers
			mD3D12Buffers = new ID3D12Buffer*[mNumberOfSlots];
			memcpy(mD3D12Buffers, d3d12BufferAtSlot, sizeof(ID3D12Buffer*) * mNumberOfSlots);

			// Strides
			mStrides = new UINT[mNumberOfSlots];
			memcpy(mStrides, strideAtSlot, sizeof(UINT) * mNumberOfSlots);

			// Offsets
			mOffsets = new UINT[mNumberOfSlots];
			memset(mOffsets, 0, sizeof(UINT) * mNumberOfSlots);
		}
		*/

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("VAO");
		#endif
	}

	VertexArray::~VertexArray()
	{
		if (nullptr != mD3D12RootSignature)
		{
			mD3D12RootSignature->Release();
		}
		if (nullptr != mD3D12PipelineState)
		{
			mD3D12PipelineState->Release();
		}
		vertexBuffer->release();

		// TODO(co) Direct3D 12 update
		/*
		// Release the index buffer reference
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->release();
		}

		// Release the Direct3D 12 input layout
		if (nullptr != mD3D12InputLayout)
		{
			mD3D12InputLayout->Release();
		}

		// Cleanup Direct3D 12 input slot data
		if (nullptr != mD3D12Buffers)
		{
			delete [] mD3D12Buffers;
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

		// Release our Direct3D 12 device context reference
		mD3D12DeviceContext->Release();
		*/
	}

	void VertexArray::setDirect3DIASetInputLayoutAndStreamSource(ID3D12GraphicsCommandList& d3d12GraphicsCommandList) const
	{
		d3d12GraphicsCommandList.SetGraphicsRootSignature(mD3D12RootSignature);
		d3d12GraphicsCommandList.SetPipelineState(mD3D12PipelineState);
		d3d12GraphicsCommandList.IASetVertexBuffers(0, 1, &mD3D12VertexBufferView);


		// TODO(co) Direct3D 12 update
		/*
		// Valid Direct3D 12 input layout?
		if (nullptr != mD3D12InputLayout)
		{
			// Set the Direct3D 12 input layout
			mD3D12DeviceContext->IASetInputLayout(mD3D12InputLayout);

			// Set the Direct3D 12 vertex buffers
			if (nullptr != mD3D12Buffers)
			{
				// Just make a single API call
				mD3D12DeviceContext->IASetVertexBuffers(0, mNumberOfSlots, mD3D12Buffers, mStrides, mOffsets);
				//m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
			}
			else
			{
				// Direct3D 10 says: "D3D10: INFO: ID3D10Device::IASetVertexBuffers: Since NumBuffers is 0, the operation effectively does nothing. This is probably not intentional, nor is the most efficient way to achieve this operation. Avoid calling the routine at all. [ STATE_SETTING INFO #240: DEVICE_IASETVERTEXBUFFERS_BUFFERS_EMPTY ]"
				// -> Direct3D 12 does not give us this message, but it's probably still no good thing to do
			}
		}

		// Get the used index buffer
		// -> In case of no index buffer we don't set null indices, there's not really a point in it
		if (nullptr != mIndexBuffer)
		{
			// Set the Direct3D 12 indices
			mD3D12DeviceContext->IASetIndexBuffer(mIndexBuffer->getD3D12Buffer(), static_cast<DXGI_FORMAT>(mIndexBuffer->getDXGIFormat()), 0);
		}
		*/
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void VertexArray::setDebugName(const char *)
	{
		// TODO(co) Direct3D 12 update
		/*
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			// Valid Direct3D 12 input layout?
			if (nullptr != mD3D12InputLayout)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D12InputLayout->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12InputLayout->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
		*/
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
