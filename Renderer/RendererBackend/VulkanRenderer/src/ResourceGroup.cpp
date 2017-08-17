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
#include "VulkanRenderer/ResourceGroup.h"
#include "VulkanRenderer/VulkanContext.h"
#include "VulkanRenderer/RootSignature.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/Texture/Texture1D.h"
#include "VulkanRenderer/Texture/Texture2D.h"
#include "VulkanRenderer/Texture/Texture3D.h"
#include "VulkanRenderer/Texture/TextureCube.h"
#include "VulkanRenderer/Texture/Texture2DArray.h"
#include "VulkanRenderer/State/SamplerState.h"
#include "VulkanRenderer/Buffer/UniformBuffer.h"
#include "VulkanRenderer/Buffer/TextureBuffer.h"

#include <Renderer/ILog.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ResourceGroup::ResourceGroup(RootSignature& rootSignature, VkDescriptorSet vkDescriptorSet, uint32_t numberOfResources, Renderer::IResource** resources, Renderer::ISamplerState** samplerStates) :
		IResourceGroup(static_cast<VulkanRenderer&>(rootSignature.getRenderer())),
		mRootSignature(rootSignature),
		mVkDescriptorSet(vkDescriptorSet),
		mNumberOfResources(numberOfResources),
		mResources(new Renderer::IResource*[mNumberOfResources]),
		mSamplerStates(nullptr)
	{
		// Process all resources and add our reference to the renderer resource
		const VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());
		const VkDevice vkDevice = vulkanRenderer.getVulkanContext().getVkDevice();
		for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex, ++resources)
		{
			Renderer::IResource* resource = *resources;
			mResources[resourceIndex] = resource;
			resource->addReference();

			// Check the type of resource to set
			// TODO(co) Some additional resource type root signature security checks in debug build?
			const Renderer::ResourceType resourceType = resource->getResourceType();
			switch (resourceType)
			{
				case Renderer::ResourceType::UNIFORM_BUFFER:
				{
					const VkDescriptorBufferInfo vkDescriptorBufferInfo =
					{
						static_cast<UniformBuffer*>(resource)->getVkBuffer(),	// buffer (VkBuffer)
						0,														// offset (VkDeviceSize)
						VK_WHOLE_SIZE											// range (VkDeviceSize)
					};
					const VkWriteDescriptorSet vkWriteDescriptorSet =
					{
						VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,	// sType (VkStructureType)
						nullptr,								// pNext (const void*)
						mVkDescriptorSet,						// dstSet (VkDescriptorSet)
						resourceIndex,							// dstBinding (uint32_t)
						0,										// dstArrayElement (uint32_t)
						1,										// descriptorCount (uint32_t)
						VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,		// descriptorType (VkDescriptorType)
						nullptr,								// pImageInfo (const VkDescriptorImageInfo*)
						&vkDescriptorBufferInfo,				// pBufferInfo (const VkDescriptorBufferInfo*)
						nullptr									// pTexelBufferView (const VkBufferView*)
					};
					vkUpdateDescriptorSets(vkDevice, 1, &vkWriteDescriptorSet, 0, nullptr);
					break;
				}

				case Renderer::ResourceType::TEXTURE_BUFFER:
				{
					const VkBufferView vkBufferView = static_cast<TextureBuffer*>(resource)->getVkBufferView();
					const VkWriteDescriptorSet vkWriteDescriptorSet =
					{
						VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,		// sType (VkStructureType)
						nullptr,									// pNext (const void*)
						mVkDescriptorSet,							// dstSet (VkDescriptorSet)
						resourceIndex,								// dstBinding (uint32_t)
						0,											// dstArrayElement (uint32_t)
						1,											// descriptorCount (uint32_t)
						VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,	// descriptorType (VkDescriptorType)
						nullptr,									// pImageInfo (const VkDescriptorImageInfo*)
						nullptr,									// pBufferInfo (const VkDescriptorBufferInfo*)
						&vkBufferView								// pTexelBufferView (const VkBufferView*)
					};
					vkUpdateDescriptorSets(vkDevice, 1, &vkWriteDescriptorSet, 0, nullptr);
					break;
				}

				case Renderer::ResourceType::TEXTURE_1D:
				case Renderer::ResourceType::TEXTURE_2D:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				case Renderer::ResourceType::TEXTURE_3D:
				case Renderer::ResourceType::TEXTURE_CUBE:
				{
					// Evaluate the texture type and get the Vulkan image view
					VkImageView vkImageView = VK_NULL_HANDLE;
					VkImageLayout vkImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					switch (resourceType)
					{
						case Renderer::ResourceType::TEXTURE_1D:
						{
							const Texture1D* texture1D = static_cast<Texture1D*>(resource);
							vkImageView = texture1D->getVkImageView();
							vkImageLayout = texture1D->getVkImageLayout();
							break;
						}

						case Renderer::ResourceType::TEXTURE_2D:
						{
							const Texture2D* texture2D = static_cast<Texture2D*>(resource);
							vkImageView = texture2D->getVkImageView();
							vkImageLayout = texture2D->getVkImageLayout();
							break;
						}

						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
						{
							const Texture2DArray* texture2DArray = static_cast<Texture2DArray*>(resource);
							vkImageView = texture2DArray->getVkImageView();
							vkImageLayout = texture2DArray->getVkImageLayout();
							break;
						}

						case Renderer::ResourceType::TEXTURE_3D:
						{
							const Texture3D* texture3D = static_cast<Texture3D*>(resource);
							vkImageView = texture3D->getVkImageView();
							vkImageLayout = texture3D->getVkImageLayout();
							break;
						}

						case Renderer::ResourceType::TEXTURE_CUBE:
						{
							const TextureCube* textureCube = static_cast<TextureCube*>(resource);
							vkImageView = textureCube->getVkImageView();
							vkImageLayout = textureCube->getVkImageLayout();
							break;
						}

						case Renderer::ResourceType::ROOT_SIGNATURE:
						case Renderer::ResourceType::RESOURCE_GROUP:
						case Renderer::ResourceType::PROGRAM:
						case Renderer::ResourceType::VERTEX_ARRAY:
						case Renderer::ResourceType::SWAP_CHAIN:
						case Renderer::ResourceType::FRAMEBUFFER:
						case Renderer::ResourceType::INDEX_BUFFER:
						case Renderer::ResourceType::VERTEX_BUFFER:
						case Renderer::ResourceType::UNIFORM_BUFFER:
						case Renderer::ResourceType::INDIRECT_BUFFER:
						case Renderer::ResourceType::TEXTURE_BUFFER:
						case Renderer::ResourceType::PIPELINE_STATE:
						case Renderer::ResourceType::SAMPLER_STATE:
						case Renderer::ResourceType::VERTEX_SHADER:
						case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
						case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
						case Renderer::ResourceType::GEOMETRY_SHADER:
						case Renderer::ResourceType::FRAGMENT_SHADER:
							RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Invalid Vulkan renderer backend resource type")
							break;
					}

					// Get the sampler state
					assert(nullptr != samplerStates);
					const SamplerState* samplerState = static_cast<const SamplerState*>(samplerStates[resourceIndex]);
					assert(nullptr != samplerState);

					// Update Vulkan descriptor sets
					const VkDescriptorImageInfo vkDescriptorImageInfo =
					{
						samplerState->getVkSampler(),	// sampler (VkSampler)
						vkImageView,					// imageView (VkImageView)
						vkImageLayout					// imageLayout (VkImageLayout)
					};
					const VkWriteDescriptorSet vkWriteDescriptorSet =
					{
						VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,		// sType (VkStructureType)
						nullptr,									// pNext (const void*)
						mVkDescriptorSet,							// dstSet (VkDescriptorSet)
						resourceIndex,								// dstBinding (uint32_t)
						0,											// dstArrayElement (uint32_t)
						1,											// descriptorCount (uint32_t)
						VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	// descriptorType (VkDescriptorType)
						&vkDescriptorImageInfo,						// pImageInfo (const VkDescriptorImageInfo*)
						nullptr,									// pBufferInfo (const VkDescriptorBufferInfo*)
						nullptr										// pTexelBufferView (const VkBufferView*)
					};
					vkUpdateDescriptorSets(vkDevice, 1, &vkWriteDescriptorSet, 0, nullptr);
					break;
				}

				case Renderer::ResourceType::SAMPLER_STATE:
					// Nothing to do in here, Vulkan is using combined image samplers
					break;

				case Renderer::ResourceType::ROOT_SIGNATURE:
				case Renderer::ResourceType::RESOURCE_GROUP:
				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::INDIRECT_BUFFER:
				case Renderer::ResourceType::PIPELINE_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
					RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Invalid Vulkan renderer backend resource type")
					break;
			}
		}
		if (nullptr != samplerStates)
		{
			mSamplerStates = new Renderer::ISamplerState*[mNumberOfResources];
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
			delete [] mSamplerStates;
		}
		for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex)
		{
			mResources[resourceIndex]->releaseReference();
		}
		delete [] mResources;

		// Free Vulkan descriptor set
		if (VK_NULL_HANDLE != mVkDescriptorSet)
		{
			vkFreeDescriptorSets(static_cast<VulkanRenderer&>(mRootSignature.getRenderer()).getVulkanContext().getVkDevice(), mRootSignature.getVkDescriptorPool(), 1, &mVkDescriptorSet);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
