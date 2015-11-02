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


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArray::VertexArray(Direct3D12Renderer &direct3D12Renderer, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, IndexBuffer *indexBuffer) :
		IVertexArray(direct3D12Renderer),
		mD3D12PipelineState(nullptr),
		mIndexBuffer(indexBuffer),
		mNumberOfSlots(numberOfVertexBuffers),
		mD3D12VertexBufferViews(nullptr),
		mVertexBuffers(nullptr)
	{
		{ // TODO(co) Just a first test
			// Define the vertex input layout.
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			// Describe and create the graphics pipeline state object (PSO)
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
			psoDesc.pRootSignature = direct3D12Renderer.getD3D12RootSignature();
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
		// TODO(co) This is only for the Direct3D 12 renderer backend kickoff
		if (nullptr != mD3D12PipelineState)
		{
			mD3D12PipelineState->Release();
		}

		// Release the index buffer reference
		if (nullptr != mIndexBuffer)
		{
			mIndexBuffer->release();
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
				(*vertexBuffer)->release();
			}

			// Cleanup
			delete [] mVertexBuffers;
		}
	}

	void VertexArray::setDirect3DIASetInputLayoutAndStreamSource(ID3D12GraphicsCommandList& d3d12GraphicsCommandList) const
	{
		d3d12GraphicsCommandList.SetPipelineState(mD3D12PipelineState);
		d3d12GraphicsCommandList.IASetVertexBuffers(0, mNumberOfSlots, mD3D12VertexBufferViews);

		// TODO(co) Direct3D 12 update
		/*
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
