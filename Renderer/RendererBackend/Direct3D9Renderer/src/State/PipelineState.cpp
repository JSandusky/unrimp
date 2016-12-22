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
#include "Direct3D9Renderer/State/PipelineState.h"
#include "Direct3D9Renderer/d3d9.h"
#include "Direct3D9Renderer/Mapping.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"
#include "Direct3D9Renderer/Shader/ProgramHlsl.h"
#include "Direct3D9Renderer/State/BlendState.h"
#include "Direct3D9Renderer/State/RasterizerState.h"
#include "Direct3D9Renderer/State/DepthStencilState.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	PipelineState::PipelineState(Direct3D9Renderer &direct3D9Renderer, const Renderer::PipelineState& pipelineState) :
		IPipelineState(direct3D9Renderer),
		mDirect3DDevice9(direct3D9Renderer.getDirect3DDevice9()),
		mProgram(pipelineState.program),
		mDirect3DVertexDeclaration9(nullptr),
		mRasterizerState(new RasterizerState(pipelineState.rasterizerState)),
		mDepthStencilState(new DepthStencilState(pipelineState.depthStencilState)),
		mBlendState(new BlendState(pipelineState.blendState))
	{
		// Acquire our Direct3D 9 device reference
		mDirect3DDevice9->AddRef();

		// Add a reference to the given program
		mProgram->addReference();

		{ // Create Direct3D 9 vertex elements
			const uint32_t numberOfAttributes = pipelineState.vertexAttributes.numberOfAttributes;
			const Renderer::VertexAttribute* attributes = pipelineState.vertexAttributes.attributes;

			// TODO(co) We could manage in here without new/delete when using a fixed maximum supported number of elements
			D3DVERTEXELEMENT9 *d3dVertexElements   = new D3DVERTEXELEMENT9[numberOfAttributes + 1];	// +1 for D3DDECL_END()
			D3DVERTEXELEMENT9 *d3dVertexElement    = d3dVertexElements;
			D3DVERTEXELEMENT9 *d3dVertexElementEnd = d3dVertexElements + numberOfAttributes;
			for (; d3dVertexElement < d3dVertexElementEnd; ++d3dVertexElement, ++attributes)
			{
				// Fill the "D3DVERTEXELEMENT9"-content
				d3dVertexElement->Stream     = static_cast<WORD>(attributes->inputSlot);									// Stream index (WORD)
				d3dVertexElement->Offset     = static_cast<WORD>(attributes->alignedByteOffset);							// Offset in the stream in bytes (WORD)
				d3dVertexElement->Type       = Mapping::getDirect3D9Type(attributes->vertexAttributeFormat);				// Data type (BYTE)
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
		}
	}

	PipelineState::~PipelineState()
	{
		// Destroy states
		delete mRasterizerState;
		delete mDepthStencilState;
		delete mBlendState;

		// Release the program reference
		mProgram->releaseReference();

		// Release the Direct3D 9 vertex declaration
		if (nullptr != mDirect3DVertexDeclaration9)
		{
			mDirect3DVertexDeclaration9->Release();
		}

		// Release our Direct3D 9 device reference
		mDirect3DDevice9->Release();
	}

	void PipelineState::bindPipelineState() const
	{
		// Set the Direct3D 9 vertex declaration
		mDirect3DDevice9->SetVertexDeclaration(mDirect3DVertexDeclaration9);

		// Set the program
		static_cast<Direct3D9Renderer&>(getRenderer()).setProgram(mProgram);

		// Set the Direct3D 9 rasterizer state
		mRasterizerState->setDirect3D9RasterizerStates(*mDirect3DDevice9);

		// Set Direct3D 9 depth stencil state
		mDepthStencilState->setDirect3D9DepthStencilStates(*mDirect3DDevice9);

		// Set Direct3D 9 blend state
		mBlendState->setDirect3D9BlendStates(*mDirect3DDevice9);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void PipelineState::setDebugName(const char *)
	{
		// "IDirect3DVertexDeclaration9" is not derived from "IDirect3DResource9", meaning we can't use the "IDirect3DResource9::SetPrivateData()"-method
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
