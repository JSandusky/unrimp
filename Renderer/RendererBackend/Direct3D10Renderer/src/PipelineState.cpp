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
#include "Direct3D10Renderer/PipelineState.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/D3D10.h"
#include "Direct3D10Renderer/Mapping.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"
#include "Direct3D10Renderer/Shader/ProgramHlsl.h"
#include "Direct3D10Renderer/Shader/VertexShaderHlsl.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	PipelineState::PipelineState(Direct3D10Renderer &direct3D10Renderer, const Renderer::PipelineState& pipelineState) :
		IPipelineState(direct3D10Renderer),
		mD3D10Device(direct3D10Renderer.getD3D10Device()),
		mProgram(pipelineState.program),
		mD3D10InputLayout(nullptr)
	{
		// Acquire our Direct3D 10 device reference
		mD3D10Device->AddRef();

		// Add a reference to the given program
		mProgram->addReference();

		// Create Direct3D 10 input element descriptions
		const VertexShaderHlsl* vertexShaderHlsl = static_cast<ProgramHlsl*>(mProgram)->getVertexShaderHlsl();
		if (nullptr != vertexShaderHlsl)
		{
			const uint32_t numberOfAttributes = pipelineState.numberOfVertexAttributes;
			const Renderer::VertexArrayAttribute* attributes = pipelineState.vertexAttributes;

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

			{ // Create the Direct3D 10 input layout
				ID3DBlob* d3dBlobVertexShader = vertexShaderHlsl->getD3DBlobVertexShader();
				direct3D10Renderer.getD3D10Device()->CreateInputLayout(d3d10InputElementDescs, numberOfAttributes, d3dBlobVertexShader->GetBufferPointer(), d3dBlobVertexShader->GetBufferSize(), &mD3D10InputLayout);
			}

			// Destroy Direct3D 10 input element descriptions
			delete [] d3d10InputElementDescs;
		}
		else
		{
			RENDERER_OUTPUT_DEBUG("Direct3D 10 error: Failed to create the pipeline stage input layout because there's no vertex shader");
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("Pipeline state");
		#endif
	}

	PipelineState::~PipelineState()
	{
		// Release the program reference
		if (nullptr != mProgram)
		{
			mProgram->release();
		}

		// Release the Direct3D 10 input layout
		if (nullptr != mD3D10InputLayout)
		{
			mD3D10InputLayout->Release();
		}

		// Release our Direct3D 10 device reference
		mD3D10Device->Release();
	}

	void PipelineState::bindPipelineState() const
	{
		// TODO(co) Directly move this into "Direct3D10Renderer"

		// Set the Direct3D 10 input layout
		mD3D10Device->IASetInputLayout(mD3D10InputLayout);

		// Set the program
		Direct3D10Renderer& direct3D10Renderer = static_cast<Direct3D10Renderer&>(getRenderer());
		direct3D10Renderer.setProgram(mProgram);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void PipelineState::setDebugName(const char *name)
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
