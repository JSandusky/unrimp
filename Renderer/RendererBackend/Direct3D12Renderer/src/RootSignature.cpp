/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
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
#include "Direct3D12Renderer/RootSignature.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/Direct3D12Renderer.h"
#include "Direct3D12Renderer/Direct3D12RuntimeLinking.h"

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>
#include <Renderer/RootSignatureTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RootSignature::RootSignature(Direct3D12Renderer& direct3D12Renderer, const Renderer::RootSignature& rootSignature) :
		IRootSignature(direct3D12Renderer),
		mD3D12RootSignature(nullptr)
	{
		// Create temporary Direct3D 12 root signature instance data
		// -> "Renderer::RootSignature" is not identical to "D3D12_ROOT_SIGNATURE_DESC" because it had to be extended by information required by OpenGL
		D3D12_ROOT_SIGNATURE_DESC d3d12RootSignatureDesc;
		{
			{ // Copy the parameter data
				const uint32_t numberOfRootParameters = rootSignature.numberOfParameters;
				d3d12RootSignatureDesc.NumParameters = numberOfRootParameters;
				if (numberOfRootParameters > 0)
				{
					d3d12RootSignatureDesc.pParameters = new D3D12_ROOT_PARAMETER[numberOfRootParameters];
					D3D12_ROOT_PARAMETER* d3dRootParameters = const_cast<D3D12_ROOT_PARAMETER*>(d3d12RootSignatureDesc.pParameters);
					for (uint32_t parameterIndex = 0; parameterIndex < numberOfRootParameters; ++parameterIndex)
					{
						D3D12_ROOT_PARAMETER& d3dRootParameter = d3dRootParameters[parameterIndex];
						const Renderer::RootParameter& rootParameter = rootSignature.parameters[parameterIndex];

						// Copy the descriptor table data and determine the shader visibility of the Direct3D 12 root parameter
						uint32_t shaderVisibility = ~0u;
						if (D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE == d3dRootParameter.ParameterType)
						{
							const uint32_t numberOfDescriptorRanges = d3dRootParameter.DescriptorTable.NumDescriptorRanges;
							d3dRootParameter.DescriptorTable.NumDescriptorRanges = numberOfDescriptorRanges;
							d3dRootParameter.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[numberOfDescriptorRanges];

							// "Renderer::DescriptorRange" is not identical to "D3D12_DESCRIPTOR_RANGE" because it had to be extended by information required by OpenGL
							for (uint32_t descriptorRangeIndex = 0; descriptorRangeIndex < numberOfDescriptorRanges; ++descriptorRangeIndex)
							{
								const Renderer::DescriptorRange& descriptorRange = reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges)[descriptorRangeIndex];
								memcpy(const_cast<D3D12_DESCRIPTOR_RANGE*>(&d3dRootParameter.DescriptorTable.pDescriptorRanges[descriptorRangeIndex]), &descriptorRange, sizeof(D3D12_DESCRIPTOR_RANGE));
								if (~0u == shaderVisibility)
								{
									shaderVisibility = static_cast<uint32_t>(descriptorRange.shaderVisibility);
								}
								else if (shaderVisibility != static_cast<uint32_t>(descriptorRange.shaderVisibility))
								{
									shaderVisibility = static_cast<uint32_t>(Renderer::ShaderVisibility::ALL);
								}
							}
						}
						if (~0u == shaderVisibility)
						{
							shaderVisibility = static_cast<uint32_t>(Renderer::ShaderVisibility::ALL);
						}

						// Set root parameter
						d3dRootParameters->ParameterType = static_cast<D3D12_ROOT_PARAMETER_TYPE>(rootParameter.parameterType);
						d3dRootParameters->ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(shaderVisibility);
					}
				}
				else
				{
					d3d12RootSignatureDesc.pParameters = nullptr;
				}
			}

			{ // Copy the static sampler data
			  // -> "Renderer::StaticSampler" is identical to "D3D12_STATIC_SAMPLER_DESC" so there's no additional mapping work to be done in here
				const uint32_t numberOfStaticSamplers = rootSignature.numberOfStaticSamplers;
				d3d12RootSignatureDesc.NumStaticSamplers = numberOfStaticSamplers;
				if (numberOfStaticSamplers > 0)
				{
					d3d12RootSignatureDesc.pStaticSamplers = new D3D12_STATIC_SAMPLER_DESC[numberOfStaticSamplers];
					memcpy(const_cast<D3D12_STATIC_SAMPLER_DESC*>(d3d12RootSignatureDesc.pStaticSamplers), rootSignature.staticSamplers, sizeof(Renderer::StaticSampler) * numberOfStaticSamplers);
				}
				else
				{
					d3d12RootSignatureDesc.pStaticSamplers = nullptr;
				}
			}

			// Copy flags
			// -> "Renderer::RootSignatureFlags" is identical to "D3D12_ROOT_SIGNATURE_FLAGS" so there's no additional mapping work to be done in here
			d3d12RootSignatureDesc.Flags = static_cast<D3D12_ROOT_SIGNATURE_FLAGS>(rootSignature.flags);
		}

		{ // Create the Direct3D 12 root signature instance
			ID3DBlob* d3dBlobSignature = nullptr;
			ID3DBlob* d3dBlobError = nullptr;
			if (SUCCEEDED(D3D12SerializeRootSignature(&d3d12RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &d3dBlobSignature, &d3dBlobError)))
			{
				if (FAILED(direct3D12Renderer.getD3D12Device()->CreateRootSignature(0, d3dBlobSignature->GetBufferPointer(), d3dBlobSignature->GetBufferSize(), IID_PPV_ARGS(&mD3D12RootSignature))))
				{
					RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "Failed to create the Direct3D 12 root signature instance")
				}
				d3dBlobSignature->Release();
			}
			else
			{
				RENDERER_LOG(direct3D12Renderer.getContext(), CRITICAL, "Failed to create the Direct3D 12 root signature instance")
				d3dBlobError->Release();
			}
		}

		{ // Free temporary Direct3D 12 root signature instance data
			if (nullptr != d3d12RootSignatureDesc.pParameters)
			{
				for (uint32_t i = 0; i < d3d12RootSignatureDesc.NumParameters; ++i)
				{
					const D3D12_ROOT_PARAMETER& d3d12RootParameter = d3d12RootSignatureDesc.pParameters[i];
					if (D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE == d3d12RootParameter.ParameterType)
					{
						delete [] d3d12RootParameter.DescriptorTable.pDescriptorRanges;
					}
				}
				delete [] d3d12RootSignatureDesc.pParameters;
			}
			delete [] d3d12RootSignatureDesc.pStaticSamplers;
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			setDebugName("Root signature");
		#endif
	}

	RootSignature::~RootSignature()
	{
		// Release the Direct3D 12 root signature
		if (nullptr != mD3D12RootSignature)
		{
			mD3D12RootSignature->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRootSignature methods       ]
	//[-------------------------------------------------------]
	Renderer::IResourceGroup* RootSignature::createResourceGroup(uint32_t, uint32_t, Renderer::IResource**, Renderer::ISamplerState**)
	{
		// TODO(co) Implement resource group
		RENDERER_ASSERT(getRenderer().getContext(), false, "Direct3D 12 resource group creation isn't implemented, yet")
		return nullptr;
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void RootSignature::setDebugName(const char* name)
	{
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			// Valid Direct3D 12 root signature?
			if (nullptr != mD3D12RootSignature)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D12RootSignature->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D12RootSignature->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
