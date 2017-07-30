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
#include "OpenGLES3Renderer/ResourceGroup.h"
#include "OpenGLES3Renderer/RootSignature.h"
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"
#include "OpenGLES3Renderer/IExtensions.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ResourceGroup::ResourceGroup(RootSignature& rootSignature, uint32_t rootParameterIndex, uint32_t numberOfResources, Renderer::IResource** resources) :
		IResourceGroup(rootSignature.getRenderer()),
		mRootParameterIndex(rootParameterIndex),
		mNumberOfResources(numberOfResources),
		mResources(new Renderer::IResource*[mNumberOfResources]),
		mResourceIndexToUniformBlockBindingIndex(nullptr)
	{
		// Get the uniform block binding start index
		const bool isGL_EXT_texture_buffer = static_cast<OpenGLES3Renderer&>(getRenderer()).getOpenGLES3Context().getExtensions().isGL_EXT_texture_buffer();
		const Renderer::RootSignature& rootSignatureData = rootSignature.getRootSignature();
		uint32_t uniformBlockBindingIndex = 0;
		for (uint32_t currentRootParameterIndex = 0; currentRootParameterIndex < rootParameterIndex; ++currentRootParameterIndex)
		{
			const Renderer::RootParameter& rootParameter = rootSignatureData.parameters[currentRootParameterIndex];
			if (Renderer::RootParameterType::DESCRIPTOR_TABLE == rootParameter.parameterType)
			{
				assert(nullptr != reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges));
				const uint32_t numberOfDescriptorRanges = rootParameter.descriptorTable.numberOfDescriptorRanges;
				for (uint32_t descriptorRangeIndex = 0; descriptorRangeIndex < numberOfDescriptorRanges; ++descriptorRangeIndex)
				{
					const Renderer::DescriptorRange& descriptorRange = reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges)[descriptorRangeIndex];
					if (Renderer::DescriptorRangeType::UBV == descriptorRange.rangeType)
					{
						++uniformBlockBindingIndex;
					}
					else if (Renderer::DescriptorRangeType::SAMPLER != descriptorRange.rangeType && !isGL_EXT_texture_buffer && nullptr != strstr(descriptorRange.baseShaderRegisterName, "TextureBuffer"))
					{
						// Texture buffer emulation using uniform buffer
						++uniformBlockBindingIndex;
					}
				}
			}
		}

		// Process all resources
		const Renderer::RootParameter& rootParameter = rootSignatureData.parameters[rootParameterIndex];
		for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex, ++resources)
		{
			// Add our reference to the renderer resource
			Renderer::IResource* resource = *resources;
			mResources[resourceIndex] = resource;
			resource->addReference();

			// Uniform block binding index handling
			bool isUniformBuffer = false;
			const Renderer::DescriptorRange& descriptorRange = reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges)[resourceIndex];
			if (Renderer::DescriptorRangeType::UBV == descriptorRange.rangeType)
			{
				isUniformBuffer = true;
			}
			else if (Renderer::DescriptorRangeType::SAMPLER != descriptorRange.rangeType && !isGL_EXT_texture_buffer && nullptr != strstr(descriptorRange.baseShaderRegisterName, "TextureBuffer"))
			{
				// Texture buffer emulation using uniform buffer
				isUniformBuffer = true;
			}
			if (isUniformBuffer)
			{
				if (nullptr == mResourceIndexToUniformBlockBindingIndex)
				{
					mResourceIndexToUniformBlockBindingIndex = new uint32_t[mNumberOfResources];
					memset(mResourceIndexToUniformBlockBindingIndex, 0, sizeof(uint32_t) * mNumberOfResources);
				}
				mResourceIndexToUniformBlockBindingIndex[resourceIndex] = uniformBlockBindingIndex;
				++uniformBlockBindingIndex;
			}
		}
	}

	ResourceGroup::~ResourceGroup()
	{
		// Remove our reference from the renderer resources
		for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex)
		{
			mResources[resourceIndex]->releaseReference();
		}
		delete [] mResources;
		delete [] mResourceIndexToUniformBlockBindingIndex;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
