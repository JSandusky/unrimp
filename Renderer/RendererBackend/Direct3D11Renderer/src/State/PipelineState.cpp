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
#include "Direct3D11Renderer/State/PipelineState.h"
#include "Direct3D11Renderer/State/BlendState.h"
#include "Direct3D11Renderer/State/RasterizerState.h"
#include "Direct3D11Renderer/State/DepthStencilState.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/D3D11.h"
#include "Direct3D11Renderer/Mapping.h"
#include "Direct3D11Renderer/Direct3D11Renderer.h"
#include "Direct3D11Renderer/Shader/ProgramHlsl.h"
#include "Direct3D11Renderer/Shader/VertexShaderHlsl.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	PipelineState::PipelineState(Direct3D11Renderer &direct3D11Renderer, const Renderer::PipelineState& pipelineState) :
		IPipelineState(direct3D11Renderer),
		mProgram(pipelineState.program),
		mD3D11DeviceContext(direct3D11Renderer.getD3D11DeviceContext()),
		mD3D11InputLayout(nullptr),
		mRasterizerState(new RasterizerState(direct3D11Renderer, pipelineState.rasterizerState)),
		mDepthStencilState(new DepthStencilState(direct3D11Renderer, pipelineState.depthStencilState)),
		mBlendState(new BlendState(direct3D11Renderer, pipelineState.blendState))
	{
		// Acquire our Direct3D 11 device context reference
		mD3D11DeviceContext->AddRef();

		// Add a reference to the given program
		mProgram->addReference();

		// Create Direct3D 11 input element descriptions
		const VertexShaderHlsl* vertexShaderHlsl = static_cast<ProgramHlsl*>(mProgram)->getVertexShaderHlsl();
		if (nullptr != vertexShaderHlsl)
		{
			ID3DBlob* d3dBlobVertexShader = vertexShaderHlsl->getD3DBlobVertexShader();
			if (nullptr != d3dBlobVertexShader)
			{
				const uint32_t numberOfAttributes = pipelineState.vertexAttributes.numberOfAttributes;
				const Renderer::VertexAttribute* attributes = pipelineState.vertexAttributes.attributes;

				// TODO(co) We could manage in here without new/delete when using a fixed maximum supported number of elements
				D3D11_INPUT_ELEMENT_DESC *d3d11InputElementDescs   = numberOfAttributes ? new D3D11_INPUT_ELEMENT_DESC[numberOfAttributes] : new D3D11_INPUT_ELEMENT_DESC[1];
				D3D11_INPUT_ELEMENT_DESC *d3d11InputElementDesc    = d3d11InputElementDescs;
				D3D11_INPUT_ELEMENT_DESC *d3d11InputElementDescEnd = d3d11InputElementDescs + numberOfAttributes;
				for (; d3d11InputElementDesc < d3d11InputElementDescEnd; ++d3d11InputElementDesc, ++attributes)
				{
					// Fill the "D3D11_INPUT_ELEMENT_DESC"-content
					d3d11InputElementDesc->SemanticName      = attributes->semanticName;																	// Semantic name (LPCSTR)
					d3d11InputElementDesc->SemanticIndex     = attributes->semanticIndex;																	// Semantic index (UINT)
					d3d11InputElementDesc->Format            = static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(attributes->vertexAttributeFormat));	// Format (DXGI_FORMAT)
					d3d11InputElementDesc->InputSlot         = static_cast<UINT>(attributes->inputSlot);													// Input slot (UINT)
					d3d11InputElementDesc->AlignedByteOffset = attributes->alignedByteOffset;																// Aligned byte offset (UINT)

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
				direct3D11Renderer.getD3D11Device()->CreateInputLayout(d3d11InputElementDescs, numberOfAttributes, d3dBlobVertexShader->GetBufferPointer(), d3dBlobVertexShader->GetBufferSize(), &mD3D11InputLayout);

				// Destroy Direct3D 11 input element descriptions
				delete [] d3d11InputElementDescs;
			}
		}
		else
		{
			RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Failed to create the pipeline stage input layout because there's no vertex shader");
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("Pipeline state");
		#endif
	}

	PipelineState::~PipelineState()
	{
		// Destroy states
		delete mRasterizerState;
		delete mDepthStencilState;
		delete mBlendState;

		// Release the program reference
		if (nullptr != mProgram)
		{
			mProgram->release();
		}

		// Release the Direct3D 11 input layout
		if (nullptr != mD3D11InputLayout)
		{
			mD3D11InputLayout->Release();
		}

		// Release our Direct3D 11 device context reference
		mD3D11DeviceContext->Release();
	}

	void PipelineState::bindPipelineState() const
	{
		// Set the Direct3D 11 input layout
		mD3D11DeviceContext->IASetInputLayout(mD3D11InputLayout);

		// Set the program
		static_cast<Direct3D11Renderer&>(getRenderer()).setProgram(mProgram);

		// Set the Direct3D 11 rasterizer state
		mD3D11DeviceContext->RSSetState(mRasterizerState->getD3D11RasterizerState());

		// Set Direct3D 11 depth stencil state
		mD3D11DeviceContext->OMSetDepthStencilState(mDepthStencilState->getD3D11DepthStencilState(), 0);

		// Set Direct3D 11 blend state
		mD3D11DeviceContext->OMSetBlendState(mBlendState->getD3D11BlendState(), 0, 0xffffffff);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void PipelineState::setDebugName(const char *name)
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
