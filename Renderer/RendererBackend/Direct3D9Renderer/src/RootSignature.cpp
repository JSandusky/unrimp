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
#include "Direct3D9Renderer/RootSignature.h"
#include "Direct3D9Renderer/ResourceGroup.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"
#include "Direct3D9Renderer/State/SamplerState.h"

#include <Renderer/ILog.h>

#include <memory.h>


#ifndef DIRECT3D9RENDERER_NO_DEBUG
	//[-------------------------------------------------------]
	//[ Anonymous detail namespace                            ]
	//[-------------------------------------------------------]
	namespace
	{
		namespace detail
		{


			//[-------------------------------------------------------]
			//[ Global functions                                      ]
			//[-------------------------------------------------------]
			void checkSamplerState(Direct3D9Renderer::Direct3D9Renderer& direct3D9Renderer, const Renderer::RootSignature& rootSignature, uint32_t samplerRootParameterIndex)
			{
				if (samplerRootParameterIndex >= rootSignature.numberOfParameters)
				{
					RENDERER_LOG(direct3D9Renderer.getContext(), CRITICAL, "Direct3D 9 renderer backend sampler root parameter index is out of bounds")
					return;
				}
				const Renderer::RootParameter& samplerRootParameter = rootSignature.parameters[samplerRootParameterIndex];
				if (Renderer::RootParameterType::DESCRIPTOR_TABLE != samplerRootParameter.parameterType)
				{
					RENDERER_LOG(direct3D9Renderer.getContext(), CRITICAL, "Direct3D 9 renderer backend sampler root parameter index doesn't point to a descriptor table")
					return;
				}

				// TODO(co) For now, we only support a single descriptor range
				if (1 != samplerRootParameter.descriptorTable.numberOfDescriptorRanges)
				{
					RENDERER_LOG(direct3D9Renderer.getContext(), CRITICAL, "Direct3D 9 renderer backend sampler root parameter: Only a single descriptor range is supported")
					return;
				}
				if (Renderer::DescriptorRangeType::SAMPLER != reinterpret_cast<const Renderer::DescriptorRange*>(samplerRootParameter.descriptorTable.descriptorRanges)[0].rangeType)
				{
					RENDERER_LOG(direct3D9Renderer.getContext(), CRITICAL, "Direct3D 9 renderer backend sampler root parameter index is out of bounds")
					return;
				}
			}


	//[-------------------------------------------------------]
	//[ Anonymous detail namespace                            ]
	//[-------------------------------------------------------]
		} // detail
	}
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RootSignature::RootSignature(Direct3D9Renderer& direct3D9Renderer, const Renderer::RootSignature& rootSignature) :
		IRootSignature(direct3D9Renderer),
		mRootSignature(rootSignature),
		mSamplerStates(nullptr)
	{
		{ // Copy the parameter data
			const uint32_t numberOfParameters = mRootSignature.numberOfParameters;
			if (numberOfParameters > 0)
			{
				mRootSignature.parameters = new Renderer::RootParameter[numberOfParameters];
				Renderer::RootParameter* destinationRootParameters = const_cast<Renderer::RootParameter*>(mRootSignature.parameters);
				memcpy(destinationRootParameters, rootSignature.parameters, sizeof(Renderer::RootParameter) * numberOfParameters);

				// Copy the descriptor table data
				for (uint32_t i = 0; i < numberOfParameters; ++i)
				{
					Renderer::RootParameter& destinationRootParameter = destinationRootParameters[i];
					const Renderer::RootParameter& sourceRootParameter = rootSignature.parameters[i];
					if (Renderer::RootParameterType::DESCRIPTOR_TABLE == destinationRootParameter.parameterType)
					{
						const uint32_t numberOfDescriptorRanges = destinationRootParameter.descriptorTable.numberOfDescriptorRanges;
						destinationRootParameter.descriptorTable.descriptorRanges = reinterpret_cast<uintptr_t>(new Renderer::DescriptorRange[numberOfDescriptorRanges]);
						memcpy(reinterpret_cast<Renderer::DescriptorRange*>(destinationRootParameter.descriptorTable.descriptorRanges), reinterpret_cast<const Renderer::DescriptorRange*>(sourceRootParameter.descriptorTable.descriptorRanges), sizeof(Renderer::DescriptorRange) * numberOfDescriptorRanges);
					}
				}
			}
		}

		{ // Copy the static sampler data
			const uint32_t numberOfStaticSamplers = mRootSignature.numberOfStaticSamplers;
			if (numberOfStaticSamplers > 0)
			{
				mRootSignature.staticSamplers = new Renderer::StaticSampler[numberOfStaticSamplers];
				memcpy(const_cast<Renderer::StaticSampler*>(mRootSignature.staticSamplers), rootSignature.staticSamplers, sizeof(Renderer::StaticSampler) * numberOfStaticSamplers);
			}
		}

		// Initialize sampler state references
		if (mRootSignature.numberOfParameters > 0)
		{
			mSamplerStates = new SamplerState*[mRootSignature.numberOfParameters];
			memset(mSamplerStates, 0, sizeof(SamplerState*) * mRootSignature.numberOfParameters);
		}
	}

	RootSignature::~RootSignature()
	{
		// Release all sampler state references
		if (nullptr != mSamplerStates)
		{
			for (uint32_t i = 0; i < mRootSignature.numberOfParameters; ++i)
			{
				SamplerState* samplerState = mSamplerStates[i];
				if (nullptr != samplerState)
				{
					samplerState->releaseReference();
				}
			}
			delete [] mSamplerStates;
		}

		// Destroy the root signature data
		if (nullptr != mRootSignature.parameters)
		{
			for (uint32_t i = 0; i < mRootSignature.numberOfParameters; ++i)
			{
				const Renderer::RootParameter& rootParameter = mRootSignature.parameters[i];
				if (Renderer::RootParameterType::DESCRIPTOR_TABLE == rootParameter.parameterType)
				{
					delete [] reinterpret_cast<Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges);
				}
			}
			delete [] mRootSignature.parameters;
		}
		delete [] mRootSignature.staticSamplers;
	}

	const SamplerState* RootSignature::getSamplerState(uint32_t samplerRootParameterIndex) const
	{
		// Security checks
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			Direct3D9Renderer& direct3D9Renderer = static_cast<Direct3D9Renderer&>(getRenderer());
			detail::checkSamplerState(direct3D9Renderer, mRootSignature, samplerRootParameterIndex);
			if (nullptr == mSamplerStates[samplerRootParameterIndex])
			{
				RENDERER_LOG(direct3D9Renderer.getContext(), CRITICAL, "Direct3D 9 renderer backend sampler root parameter index points to no sampler state instance")
				return nullptr;
			}
		#endif
		return mSamplerStates[samplerRootParameterIndex];
	}

	void RootSignature::setSamplerState(uint32_t samplerRootParameterIndex, SamplerState* samplerState) const
	{
		// Security checks
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			detail::checkSamplerState(static_cast<Direct3D9Renderer&>(getRenderer()), mRootSignature, samplerRootParameterIndex);
		#endif

		// Set sampler state
		SamplerState** samplerStateSlot = &mSamplerStates[samplerRootParameterIndex];
		if (samplerState != *samplerStateSlot)
		{
			if (nullptr != *samplerStateSlot)
			{
				(*samplerStateSlot)->releaseReference();
			}
			(*samplerStateSlot) = samplerState;
			if (nullptr != *samplerStateSlot)
			{
				(*samplerStateSlot)->addReference();
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRootSignature methods       ]
	//[-------------------------------------------------------]
	Renderer::IResourceGroup* RootSignature::createResourceGroup(uint32_t rootParameterIndex, uint32_t numberOfResources, Renderer::IResource** resources)
	{
		// Sanity checks
		assert(rootParameterIndex < mRootSignature.numberOfParameters && "The root parameter index is out-of-bounds");
		assert(numberOfResources > 0 && "The number of resources must not be zero");
		assert(nullptr != resources && "The resource pointers must be valid");

		// Create resource group
		return new ResourceGroup(getRenderer(), rootParameterIndex, numberOfResources, resources);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
