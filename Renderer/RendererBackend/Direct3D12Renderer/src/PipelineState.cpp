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
#include "Direct3D12Renderer/PipelineState.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/D3D12X.h"
#include "Direct3D12Renderer/Mapping.h"
#include "Direct3D12Renderer/RootSignature.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"
#include "Direct3D12Renderer/Shader/ProgramHlsl.h"
#include "Direct3D12Renderer/Shader/VertexShaderHlsl.h"
#include "Direct3D12Renderer/Shader/GeometryShaderHlsl.h"
#include "Direct3D12Renderer/Shader/FragmentShaderHlsl.h"
#include "Direct3D12Renderer/Shader/TessellationControlShaderHlsl.h"
#include "Direct3D12Renderer/Shader/TessellationEvaluationShaderHlsl.h"

#include <Renderer/PipelineStateTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	PipelineState::PipelineState(Direct3D12Renderer &direct3D12Renderer, const Renderer::PipelineState& pipelineState) :
		IPipelineState(direct3D12Renderer),
		mD3D12PipelineState(nullptr),
		mRootSignature(pipelineState.rootSignature),
		mProgram(pipelineState.program)
	{
		// Add a reference to the given root signature and program
		mRootSignature->addReference();
		mProgram->addReference();

		// Define the vertex input layout
		// -> No dynamic allocations/deallocations in here, a fixed maximum number of supported attributes must be sufficient
		static const uint32_t MAXIMUM_NUMBER_OF_ATTRIBUTES = 16;	// 16 elements per vertex are already pretty many
		uint32_t numberOfVertexAttributes = pipelineState.vertexAttributes.numberOfAttributes;
		if (numberOfVertexAttributes > MAXIMUM_NUMBER_OF_ATTRIBUTES)
		{
			RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 12 error: Too many vertex attributes (%d) provided. The limit is %d.", numberOfVertexAttributes, MAXIMUM_NUMBER_OF_ATTRIBUTES);
			numberOfVertexAttributes = MAXIMUM_NUMBER_OF_ATTRIBUTES;
		}
		D3D12_INPUT_ELEMENT_DESC d3d12InputElementDescs[MAXIMUM_NUMBER_OF_ATTRIBUTES];
		for (uint32_t vertexAttribute = 0; vertexAttribute < numberOfVertexAttributes; ++vertexAttribute)
		{
			const Renderer::VertexAttribute& currentVertexAttribute = pipelineState.vertexAttributes.attributes[vertexAttribute];
			D3D12_INPUT_ELEMENT_DESC& d3d12InputElementDesc = d3d12InputElementDescs[vertexAttribute];

			d3d12InputElementDesc.SemanticName		= currentVertexAttribute.semanticName;
			d3d12InputElementDesc.SemanticIndex		= currentVertexAttribute.semanticIndex;
			d3d12InputElementDesc.Format			= static_cast<DXGI_FORMAT>(Mapping::getDirect3D12Format(currentVertexAttribute.vertexAttributeFormat));
			d3d12InputElementDesc.InputSlot			= currentVertexAttribute.inputSlot;
			d3d12InputElementDesc.AlignedByteOffset	= currentVertexAttribute.alignedByteOffset;

			// Per-instance instead of per-vertex?
			if (currentVertexAttribute.instancesPerElement > 0)
			{
				d3d12InputElementDesc.InputSlotClass       = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;	// Input classification (D3D12_INPUT_CLASSIFICATION)
				d3d12InputElementDesc.InstanceDataStepRate = currentVertexAttribute.instancesPerElement;	// Instance data step rate (UINT)
			}
			else
			{
				d3d12InputElementDesc.InputSlotClass       = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;	// Input classification (D3D12_INPUT_CLASSIFICATION)
				d3d12InputElementDesc.InstanceDataStepRate = 0;												// Instance data step rate (UINT)
			}
		}

		// Describe and create the graphics pipeline state object (PSO)
		D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d12GraphicsPipelineState = {};
		d3d12GraphicsPipelineState.InputLayout = { d3d12InputElementDescs, numberOfVertexAttributes };
		d3d12GraphicsPipelineState.pRootSignature = static_cast<RootSignature*>(mRootSignature)->getD3D12RootSignature();
		{ // Set shaders
			ProgramHlsl* programHlsl = static_cast<ProgramHlsl*>(mProgram);
			{ // Vertex shader
				const VertexShaderHlsl* vertexShaderHlsl = programHlsl->getVertexShaderHlsl();
				if (nullptr != vertexShaderHlsl)
				{
					ID3DBlob* d3dBlobVertexShader = vertexShaderHlsl->getD3DBlobVertexShader();
					d3d12GraphicsPipelineState.VS = { reinterpret_cast<UINT8*>(d3dBlobVertexShader->GetBufferPointer()), d3dBlobVertexShader->GetBufferSize() };
				}
			}
			{ // Tessellation control shader (TCS, "hull shader" in Direct3D terminology)
				const TessellationControlShaderHlsl* tessellationControlShaderHlsl = programHlsl->getTessellationControlShaderHlsl();
				if (nullptr != tessellationControlShaderHlsl)
				{
					ID3DBlob* d3dBlobHullShader = tessellationControlShaderHlsl->getD3DBlobHullShader();
					d3d12GraphicsPipelineState.HS = { reinterpret_cast<UINT8*>(d3dBlobHullShader->GetBufferPointer()), d3dBlobHullShader->GetBufferSize() };
				}
			}
			{ // Tessellation evaluation shader (TES, "domain shader" in Direct3D terminology)
				const TessellationEvaluationShaderHlsl* tessellationEvaluationShaderHlsl = programHlsl->getTessellationEvaluationShaderHlsl();
				if (nullptr != tessellationEvaluationShaderHlsl)
				{
					ID3DBlob* d3dBlobDomainShader = tessellationEvaluationShaderHlsl->getD3DBlobDomainShader();
					d3d12GraphicsPipelineState.DS = { reinterpret_cast<UINT8*>(d3dBlobDomainShader->GetBufferPointer()), d3dBlobDomainShader->GetBufferSize() };
				}
			}
			{ // Geometry shader
				const GeometryShaderHlsl* geometryShaderHlsl = programHlsl->getGeometryShaderHlsl();
				if (nullptr != geometryShaderHlsl)
				{
					ID3DBlob* d3dBlobGeometryShader = geometryShaderHlsl->getD3DBlobGeometryShader();
					d3d12GraphicsPipelineState.GS = { reinterpret_cast<UINT8*>(d3dBlobGeometryShader->GetBufferPointer()), d3dBlobGeometryShader->GetBufferSize() };
				}
			}
			{ // Fragment shader (FS, "pixel shader" in Direct3D terminology)
				const FragmentShaderHlsl* fragmentShaderHlsl = programHlsl->getFragmentShaderHlsl();
				if (nullptr != fragmentShaderHlsl)
				{
					ID3DBlob* d3dBlobFragmentShader = programHlsl->getFragmentShaderHlsl()->getD3DBlobFragmentShader();
					d3d12GraphicsPipelineState.PS = { reinterpret_cast<UINT8*>(d3dBlobFragmentShader->GetBufferPointer()), d3dBlobFragmentShader->GetBufferSize() };
				}
			}
		}
		d3d12GraphicsPipelineState.PrimitiveTopologyType = static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(pipelineState.primitiveTopologyType);
		memcpy(&d3d12GraphicsPipelineState.RasterizerState, &pipelineState.rasterizerState, sizeof(D3D12_RASTERIZER_DESC));
		memcpy(&d3d12GraphicsPipelineState.DepthStencilState, &pipelineState.depthStencilState, sizeof(D3D12_DEPTH_STENCIL_DESC));
		d3d12GraphicsPipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		d3d12GraphicsPipelineState.SampleMask = UINT_MAX;
		d3d12GraphicsPipelineState.NumRenderTargets = pipelineState.numberOfRenderTargets;
		for (uint32_t i = 0; i < pipelineState.numberOfRenderTargets; ++i)
		{
			d3d12GraphicsPipelineState.RTVFormats[i] = static_cast<DXGI_FORMAT>(Mapping::getDirect3D12Format(pipelineState.renderTargetViewFormats[i]));
		}
		d3d12GraphicsPipelineState.DSVFormat = static_cast<DXGI_FORMAT>(Mapping::getDirect3D12Format(pipelineState.depthStencilViewFormat));
		d3d12GraphicsPipelineState.SampleDesc.Count = 1;
		if (FAILED(direct3D12Renderer.getD3D12Device()->CreateGraphicsPipelineState(&d3d12GraphicsPipelineState, IID_PPV_ARGS(&mD3D12PipelineState))))
		{
			RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create the pipeline state object")
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("Pipeline state");
		#endif
	}

	PipelineState::~PipelineState()
	{
		// Release the Direct3D 12 pipeline state
		if (nullptr != mD3D12PipelineState)
		{
			mD3D12PipelineState->Release();
		}

		// Release the root signature and program reference
		mRootSignature->release();
		mProgram->release();
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void PipelineState::setDebugName(const char *name)
	{
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			// Valid Direct3D 12 pipeline state?
			if (nullptr != mD3D12PipelineState)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mD3D12PipelineState->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12PipelineState->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
