/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "OpenGLRenderer/ResourceGroup.h"
#include "OpenGLRenderer/RootSignature.h"

#include <Renderer/IAssert.h>
#include <Renderer/IRenderer.h>
#include <Renderer/IAllocator.h>
#include <Renderer/State/ISamplerState.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ResourceGroup::ResourceGroup(RootSignature& rootSignature, uint32_t rootParameterIndex, uint32_t numberOfResources, Renderer::IResource** resources, Renderer::ISamplerState** samplerStates) :
		IResourceGroup(rootSignature.getRenderer()),
		mRootParameterIndex(rootParameterIndex),
		mNumberOfResources(numberOfResources),
		mResources(RENDERER_MALLOC_TYPED(rootSignature.getRenderer().getContext(), Renderer::IResource*, mNumberOfResources)),
		mSamplerStates(nullptr),
		mResourceIndexToUniformBlockBindingIndex(nullptr)
	{
		// Get the uniform block binding start index
		const Renderer::Context& context = rootSignature.getRenderer().getContext();
		const Renderer::RootSignature& rootSignatureData = rootSignature.getRootSignature();
		uint32_t uniformBlockBindingIndex = 0;
		for (uint32_t currentRootParameterIndex = 0; currentRootParameterIndex < rootParameterIndex; ++currentRootParameterIndex)
		{
			const Renderer::RootParameter& rootParameter = rootSignatureData.parameters[currentRootParameterIndex];
			if (Renderer::RootParameterType::DESCRIPTOR_TABLE == rootParameter.parameterType)
			{
				RENDERER_ASSERT(rootSignature.getRenderer().getContext(), nullptr != reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges), "Invalid OpenGL descriptor ranges")
				const uint32_t numberOfDescriptorRanges = rootParameter.descriptorTable.numberOfDescriptorRanges;
				for (uint32_t descriptorRangeIndex = 0; descriptorRangeIndex < numberOfDescriptorRanges; ++descriptorRangeIndex)
				{
					if (Renderer::DescriptorRangeType::UBV == reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges)[descriptorRangeIndex].rangeType)
					{
						++uniformBlockBindingIndex;
					}
				}
			}
		}

		// Process all resources and add our reference to the renderer resource
		const Renderer::RootParameter& rootParameter = rootSignatureData.parameters[rootParameterIndex];
		for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex, ++resources)
		{
			Renderer::IResource* resource = *resources;
			mResources[resourceIndex] = resource;
			resource->addReference();

			// Uniform block binding index handling
			const Renderer::DescriptorRange& descriptorRange = reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges)[resourceIndex];
			if (Renderer::DescriptorRangeType::UBV == descriptorRange.rangeType)
			{
				if (nullptr == mResourceIndexToUniformBlockBindingIndex)
				{
					mResourceIndexToUniformBlockBindingIndex = RENDERER_MALLOC_TYPED(context, uint32_t, mNumberOfResources);
					memset(mResourceIndexToUniformBlockBindingIndex, 0, sizeof(uint32_t) * mNumberOfResources);
				}
				mResourceIndexToUniformBlockBindingIndex[resourceIndex] = uniformBlockBindingIndex;
				++uniformBlockBindingIndex;
			}
		}
		if (nullptr != samplerStates)
		{
			mSamplerStates = RENDERER_MALLOC_TYPED(context, Renderer::ISamplerState*, mNumberOfResources);
			for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex)
			{
				Renderer::ISamplerState* samplerState = mSamplerStates[resourceIndex] = samplerStates[resourceIndex];
				if (nullptr != samplerState)
				{
					samplerState->addReference();
				}
			}
		}
	}

	ResourceGroup::~ResourceGroup()
	{
		// Remove our reference from the renderer resources
		const Renderer::Context& context = getRenderer().getContext();
		if (nullptr != mSamplerStates)
		{
			for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex)
			{
				Renderer::ISamplerState* samplerState = mSamplerStates[resourceIndex];
				if (nullptr != samplerState)
				{
					samplerState->releaseReference();
				}
			}
			RENDERER_FREE(context, mSamplerStates);
		}
		for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex)
		{
			mResources[resourceIndex]->releaseReference();
		}
		RENDERER_FREE(context, mResources);
		RENDERER_FREE(context, mResourceIndexToUniformBlockBindingIndex);
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void ResourceGroup::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), ResourceGroup, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
