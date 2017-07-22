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
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanDebug.h"	// For "VULKANRENDERER_RENDERERMATCHCHECK_RETURN()"
#include "VulkanRenderer/Mapping.h"
#include "VulkanRenderer/Extensions.h"
#include "VulkanRenderer/RootSignature.h"
#include "VulkanRenderer/VulkanContext.h"
#include "VulkanRenderer/VulkanRuntimeLinking.h"
#include "VulkanRenderer/RenderTarget/SwapChain.h"
#include "VulkanRenderer/RenderTarget/Framebuffer.h"
#include "VulkanRenderer/Buffer/BufferManager.h"
#include "VulkanRenderer/Buffer/VertexArray.h"
#include "VulkanRenderer/Buffer/IndexBuffer.h"
#include "VulkanRenderer/Buffer/VertexBuffer.h"
#include "VulkanRenderer/Buffer/UniformBuffer.h"
#include "VulkanRenderer/Buffer/TextureBuffer.h"
#include "VulkanRenderer/Buffer/IndirectBuffer.h"
#include "VulkanRenderer/Texture/TextureManager.h"
#include "VulkanRenderer/Texture/Texture1D.h"
#include "VulkanRenderer/Texture/Texture2D.h"
#include "VulkanRenderer/Texture/Texture3D.h"
#include "VulkanRenderer/Texture/Texture2DArray.h"
#include "VulkanRenderer/State/SamplerState.h"
#include "VulkanRenderer/State/PipelineState.h"
#include "VulkanRenderer/Shader/ShaderLanguageGlsl.h"
#include "VulkanRenderer/Shader/ProgramGlsl.h"

#include <Renderer/ILog.h>
#include <Renderer/Buffer/CommandBuffer.h>

#include <tuple>	// For "std::ignore"


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef VULKANRENDERER_EXPORTS
	#define VULKANRENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define VULKANRENDERER_API_EXPORT
#endif
VULKANRENDERER_API_EXPORT Renderer::IRenderer* createVulkanRendererInstance(const Renderer::Context& context)
{
	return new VulkanRenderer::VulkanRenderer(context);
}
#undef VULKANRENDERER_API_EXPORT


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
		namespace BackendDispatch
		{


			//[-------------------------------------------------------]
			//[ Resource handling                                     ]
			//[-------------------------------------------------------]
			void CopyUniformBufferData(const void* data, Renderer::IRenderer&)
			{
				const Renderer::Command::CopyUniformBufferData* realData = static_cast<const Renderer::Command::CopyUniformBufferData*>(data);
				realData->uniformBuffer->copyDataFrom(realData->numberOfBytes, (nullptr != realData->data) ? realData->data : Renderer::CommandPacketHelper::getAuxiliaryMemory(realData));
			}

			void CopyTextureBufferData(const void* data, Renderer::IRenderer&)
			{
				const Renderer::Command::CopyTextureBufferData* realData = static_cast<const Renderer::Command::CopyTextureBufferData*>(data);
				realData->textureBuffer->copyDataFrom(realData->numberOfBytes, (nullptr != realData->data) ? realData->data : Renderer::CommandPacketHelper::getAuxiliaryMemory(realData));
			}

			//[-------------------------------------------------------]
			//[ Graphics root                                         ]
			//[-------------------------------------------------------]
			void SetGraphicsRootSignature(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetGraphicsRootSignature* realData = static_cast<const Renderer::Command::SetGraphicsRootSignature*>(data);
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).setGraphicsRootSignature(realData->rootSignature);
			}

			void SetGraphicsRootDescriptorTable(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetGraphicsRootDescriptorTable* realData = static_cast<const Renderer::Command::SetGraphicsRootDescriptorTable*>(data);
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).setGraphicsRootDescriptorTable(realData->rootParameterIndex, realData->resource);
			}

			//[-------------------------------------------------------]
			//[ States                                                ]
			//[-------------------------------------------------------]
			void SetPipelineState(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetPipelineState* realData = static_cast<const Renderer::Command::SetPipelineState*>(data);
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).setPipelineState(realData->pipelineState);
			}

			//[-------------------------------------------------------]
			//[ Input-assembler (IA) stage                            ]
			//[-------------------------------------------------------]
			void SetVertexArray(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetVertexArray* realData = static_cast<const Renderer::Command::SetVertexArray*>(data);
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).iaSetVertexArray(realData->vertexArray);
			}

			//[-------------------------------------------------------]
			//[ Rasterizer (RS) stage                                 ]
			//[-------------------------------------------------------]
			void SetViewports(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetViewports* realData = static_cast<const Renderer::Command::SetViewports*>(data);
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).rsSetViewports(realData->numberOfViewports, (nullptr != realData->viewports) ? realData->viewports : reinterpret_cast<const Renderer::Viewport*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
			}

			void SetScissorRectangles(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetScissorRectangles* realData = static_cast<const Renderer::Command::SetScissorRectangles*>(data);
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).rsSetScissorRectangles(realData->numberOfScissorRectangles, (nullptr != realData->scissorRectangles) ? realData->scissorRectangles : reinterpret_cast<const Renderer::ScissorRectangle*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
			}

			//[-------------------------------------------------------]
			//[ Output-merger (OM) stage                              ]
			//[-------------------------------------------------------]
			void SetRenderTarget(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetRenderTarget* realData = static_cast<const Renderer::Command::SetRenderTarget*>(data);
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).omSetRenderTarget(realData->renderTarget);
			}

			//[-------------------------------------------------------]
			//[ Operations                                            ]
			//[-------------------------------------------------------]
			void Clear(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Clear* realData = static_cast<const Renderer::Command::Clear*>(data);
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).clear(realData->flags, realData->color, realData->z, realData->stencil);
			}

			void ResolveMultisampleFramebuffer(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::ResolveMultisampleFramebuffer* realData = static_cast<const Renderer::Command::ResolveMultisampleFramebuffer*>(data);
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).resolveMultisampleFramebuffer(*realData->destinationRenderTarget, *realData->sourceMultisampleFramebuffer);
			}

			void CopyResource(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::CopyResource* realData = static_cast<const Renderer::Command::CopyResource*>(data);
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).copyResource(*realData->destinationResource, *realData->sourceResource);
			}

			//[-------------------------------------------------------]
			//[ Draw call                                             ]
			//[-------------------------------------------------------]
			void Draw(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Draw* realData = static_cast<const Renderer::Command::Draw*>(data);
				if (nullptr != realData->indirectBuffer)
				{
					static_cast<VulkanRenderer::VulkanRenderer&>(renderer).draw(*realData->indirectBuffer, realData->indirectBufferOffset, realData->numberOfDraws);
				}
				else
				{
					static_cast<VulkanRenderer::VulkanRenderer&>(renderer).drawEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
				}
			}

			void DrawIndexed(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Draw* realData = static_cast<const Renderer::Command::Draw*>(data);
				if (nullptr != realData->indirectBuffer)
				{
					static_cast<VulkanRenderer::VulkanRenderer&>(renderer).drawIndexed(*realData->indirectBuffer, realData->indirectBufferOffset, realData->numberOfDraws);
				}
				else
				{
					static_cast<VulkanRenderer::VulkanRenderer&>(renderer).drawIndexedEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
				}
			}

			//[-------------------------------------------------------]
			//[ Debug                                                 ]
			//[-------------------------------------------------------]
			void SetDebugMarker(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetDebugMarker* realData = static_cast<const Renderer::Command::SetDebugMarker*>(data);
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).setDebugMarker(realData->name);
			}

			void BeginDebugEvent(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::BeginDebugEvent* realData = static_cast<const Renderer::Command::BeginDebugEvent*>(data);
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).beginDebugEvent(realData->name);
			}

			void EndDebugEvent(const void*, Renderer::IRenderer& renderer)
			{
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).endDebugEvent();
			}


		}


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		static const Renderer::BackendDispatchFunction DISPATCH_FUNCTIONS[Renderer::CommandDispatchFunctionIndex::NumberOfFunctions] =
		{
			// Resource handling
			&BackendDispatch::CopyUniformBufferData,
			&BackendDispatch::CopyTextureBufferData,
			// Graphics root
			&BackendDispatch::SetGraphicsRootSignature,
			&BackendDispatch::SetGraphicsRootDescriptorTable,
			// States
			&BackendDispatch::SetPipelineState,
			// Input-assembler (IA) stage
			&BackendDispatch::SetVertexArray,
			// Rasterizer (RS) stage
			&BackendDispatch::SetViewports,
			&BackendDispatch::SetScissorRectangles,
			// Output-merger (OM) stage
			&BackendDispatch::SetRenderTarget,
			// Operations
			&BackendDispatch::Clear,
			&BackendDispatch::ResolveMultisampleFramebuffer,
			&BackendDispatch::CopyResource,
			// Draw call
			&BackendDispatch::Draw,
			&BackendDispatch::DrawIndexed,
			// Debug
			&BackendDispatch::SetDebugMarker,
			&BackendDispatch::BeginDebugEvent,
			&BackendDispatch::EndDebugEvent
		};


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VulkanRenderer::VulkanRenderer(const Renderer::Context& context) :
		IRenderer(context),
		mVulkanRuntimeLinking(nullptr),
		mVulkanContext(nullptr),
		mExtensions(nullptr),
		mShaderLanguageGlsl(nullptr),
		mGraphicsRootSignature(nullptr),
		mDefaultSamplerState(nullptr),
		mInsideVulkanRenderPass(false),
		mVertexArray(nullptr),
		mMainSwapChain(nullptr),
		mRenderTarget(nullptr)
	{
		// TODO(co) Make it possible to enable/disable validation from the outside?
		#ifdef _DEBUG
			const bool enableValidation = true;
		#else
			const bool enableValidation = false;
		#endif

		// Is Vulkan available?
		mVulkanRuntimeLinking = new VulkanRuntimeLinking(*this, enableValidation);
		if (mVulkanRuntimeLinking->isVulkanAvaiable())
		{
			// TODO(co) Add external Vulkan context support
			mVulkanContext = new VulkanContext(*this);

			// Is the Vulkan context initialized?
			if (mVulkanContext->isInitialized())
			{
				// Initialize the Vulkan extensions
				mExtensions = new Extensions(*mVulkanContext);
				mExtensions->initialize();

				#ifdef RENDERER_OUTPUT_DEBUG
					// TODO(co) Implement me
				#endif

				// Initialize the capabilities
				initializeCapabilities();

				// Create the default sampler state
				mDefaultSamplerState = createSamplerState(Renderer::ISamplerState::getDefaultSamplerState());

				// Add references to the default sampler state and set it
				if (nullptr != mDefaultSamplerState)
				{
					mDefaultSamplerState->addReference();
					// TODO(co) Set default sampler states
				}

				// Create a main swap chain instance?
				const handle nativeWindowHandle = mContext.getNativeWindowHandle();
				if (NULL_HANDLE != nativeWindowHandle)
				{
					// Create a main swap chain instance
					mMainSwapChain = new SwapChain(*this, nativeWindowHandle);
					RENDERER_SET_RESOURCE_DEBUG_NAME(mMainSwapChain, "Main swap chain")
					mMainSwapChain->addReference();	// Internal renderer reference
				}
			}
		}
	}

	VulkanRenderer::~VulkanRenderer()
	{
		// Set no vertex array reference, in case we have one
		if (nullptr != mVertexArray)
		{
			iaSetVertexArray(nullptr);
		}

		// Release instances
		if (nullptr != mMainSwapChain)
		{
			mMainSwapChain->releaseReference();
			mMainSwapChain = nullptr;
		}
		if (nullptr != mRenderTarget)
		{
			mRenderTarget->releaseReference();
			mRenderTarget = nullptr;
		}
		if (nullptr != mDefaultSamplerState)
		{
			mDefaultSamplerState->releaseReference();
			mDefaultSamplerState = nullptr;
		}

		// Release the graphics root signature instance, in case we have one
		if (nullptr != mGraphicsRootSignature)
		{
			mGraphicsRootSignature->releaseReference();
		}

		#ifndef RENDERER_NO_STATISTICS
		{ // For debugging: At this point there should be no resource instances left, validate this!
			// -> Are the currently any resource instances?
			const unsigned long numberOfCurrentResources = getStatistics().getNumberOfCurrentResources();
			if (numberOfCurrentResources > 0)
			{
				// Error!
				if (numberOfCurrentResources > 1)
				{
					RENDERER_LOG(mContext, CRITICAL, "The Vulkan renderer backend is going to be destroyed, but there are still %d resource instances left (memory leak)", numberOfCurrentResources)
				}
				else
				{
					RENDERER_LOG(mContext, CRITICAL, "The Vulkan renderer backend is going to be destroyed, but there is still one resource instance left (memory leak)")
				}

				// Use debug output to show the current number of resource instances
				getStatistics().debugOutputCurrentResouces(mContext);
			}
		}
		#endif

		// Release the GLSL shader language instance, in case we have one
		if (nullptr != mShaderLanguageGlsl)
		{
			mShaderLanguageGlsl->releaseReference();
		}

		// Destroy the extensions instance
		delete mExtensions;

		// Destroy the Vulkan context instance
		delete mVulkanContext;

		// Destroy the Vulkan runtime linking instance
		delete mVulkanRuntimeLinking;
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void VulkanRenderer::setGraphicsRootSignature(Renderer::IRootSignature* rootSignature)
	{
		if (nullptr != mGraphicsRootSignature)
		{
			mGraphicsRootSignature->releaseReference();
		}
		mGraphicsRootSignature = static_cast<RootSignature*>(rootSignature);
		if (nullptr != mGraphicsRootSignature)
		{
			mGraphicsRootSignature->addReference();

			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			VULKANRENDERER_RENDERERMATCHCHECK_RETURN(*this, *rootSignature)

			// Bind Vulkan descriptor sets
			const VkDescriptorSet vkDescriptorSet = mGraphicsRootSignature->getVkDescriptorSet();
			vkCmdBindDescriptorSets(getVulkanContext().getVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsRootSignature->getVkPipelineLayout(), 0, 1, &vkDescriptorSet, 0, nullptr);
		}
	}

	void VulkanRenderer::setGraphicsRootDescriptorTable(uint32_t rootParameterIndex, Renderer::IResource* resource)
	{
		// Security checks
		#ifndef VULKANRENDERER_NO_DEBUG
		{
			if (nullptr == mGraphicsRootSignature)
			{
				RENDERER_LOG(mContext, CRITICAL, "No Vulkan renderer backend graphics root signature set")
				return;
			}
			const Renderer::RootSignature& rootSignature = mGraphicsRootSignature->getRootSignature();
			if (rootParameterIndex >= rootSignature.numberOfParameters)
			{
				RENDERER_LOG(mContext, CRITICAL, "The Vulkan renderer backend root parameter index is out of bounds")
				return;
			}
			const Renderer::RootParameter& rootParameter = rootSignature.parameters[rootParameterIndex];
			if (Renderer::RootParameterType::DESCRIPTOR_TABLE != rootParameter.parameterType)
			{
				RENDERER_LOG(mContext, CRITICAL, "The Vulkan renderer backend root parameter index doesn't reference a descriptor table")
				return;
			}

			// TODO(co) For now, we only support a single descriptor range
			if (1 != rootParameter.descriptorTable.numberOfDescriptorRanges)
			{
				RENDERER_LOG(mContext, CRITICAL, "Only a single descriptor range is supported by the Vulkan renderer backend")
				return;
			}
			if (nullptr == reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges))
			{
				RENDERER_LOG(mContext, CRITICAL, "The Vulkan renderer backend descriptor ranges is a null pointer")
				return;
			}
		}
		#endif

		if (nullptr != resource)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			VULKANRENDERER_RENDERERMATCHCHECK_RETURN(*this, *resource)

			// Check the type of resource to set
			// TODO(co) Some additional resource type root signature security checks in debug build?
			// TODO(co) There's room for binding API call related optimization in here (will certainly be no huge overall efficiency gain)
			const Renderer::ResourceType resourceType = resource->getResourceType();
			switch (resourceType)
			{
				case Renderer::ResourceType::UNIFORM_BUFFER:
				{
					const UniformBuffer* uniformBuffer = static_cast<UniformBuffer*>(resource);
					const VkDescriptorBufferInfo vkDescriptorBufferInfo =
					{
						uniformBuffer->getVkBuffer(),	// buffer (VkBuffer)
						0,								// offset (VkDeviceSize)
						VK_WHOLE_SIZE					// range (VkDeviceSize)
					};
					const VkWriteDescriptorSet vkWriteDescriptorSet =
					{
						VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,			// sType (VkStructureType)
						nullptr,										// pNext (const void*)
						mGraphicsRootSignature->getVkDescriptorSet(),	// dstSet (VkDescriptorSet)
						rootParameterIndex,								// dstBinding (uint32_t)
						0,												// dstArrayElement (uint32_t)
						1,												// descriptorCount (uint32_t)
						VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,				// descriptorType (VkDescriptorType)
						nullptr,										// pImageInfo (const VkDescriptorImageInfo*)
						&vkDescriptorBufferInfo,						// pBufferInfo (const VkDescriptorBufferInfo*)
						nullptr											// pTexelBufferView (const VkBufferView*)
					};
					vkUpdateDescriptorSets(getVulkanContext().getVkDevice(), 1, &vkWriteDescriptorSet, 0, nullptr);
					break;
				}

				case Renderer::ResourceType::TEXTURE_1D:
				case Renderer::ResourceType::TEXTURE_2D:
				case Renderer::ResourceType::TEXTURE_3D:
				{
					// Evaluate the texture type and get the Vulkan image view
					VkImageView vkImageView = VK_NULL_HANDLE;
					switch (resourceType)
					{
						case Renderer::ResourceType::TEXTURE_1D:
							vkImageView = static_cast<Texture1D*>(resource)->getVkImageView();
							break;

						case Renderer::ResourceType::TEXTURE_2D:
							vkImageView = static_cast<Texture2D*>(resource)->getVkImageView();
							break;

						case Renderer::ResourceType::TEXTURE_3D:
							vkImageView = static_cast<Texture3D*>(resource)->getVkImageView();
							break;

						case Renderer::ResourceType::ROOT_SIGNATURE:
						case Renderer::ResourceType::PROGRAM:
						case Renderer::ResourceType::VERTEX_ARRAY:
						case Renderer::ResourceType::SWAP_CHAIN:
						case Renderer::ResourceType::FRAMEBUFFER:
						case Renderer::ResourceType::INDEX_BUFFER:
						case Renderer::ResourceType::VERTEX_BUFFER:
						case Renderer::ResourceType::UNIFORM_BUFFER:
						case Renderer::ResourceType::INDIRECT_BUFFER:
						case Renderer::ResourceType::TEXTURE_BUFFER:
						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
						case Renderer::ResourceType::TEXTURE_CUBE:
						case Renderer::ResourceType::PIPELINE_STATE:
						case Renderer::ResourceType::SAMPLER_STATE:
						case Renderer::ResourceType::VERTEX_SHADER:
						case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
						case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
						case Renderer::ResourceType::GEOMETRY_SHADER:
						case Renderer::ResourceType::FRAGMENT_SHADER:
							RENDERER_LOG(mContext, CRITICAL, "Invalid Vulkan renderer backend resource type")
							break;
					}

					// Get the root signature parameter instance
					const Renderer::RootParameter& rootParameter = mGraphicsRootSignature->getRootSignature().parameters[rootParameterIndex];
					const Renderer::DescriptorRange* descriptorRange = reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges);
					assert(nullptr != descriptorRange);

					// Get the sampler state
					const SamplerState* samplerState = mGraphicsRootSignature->getSamplerState(descriptorRange->samplerRootParameterIndex);
					assert(nullptr != samplerState);

					// Update Vulkan descriptor sets
					const VkDescriptorImageInfo vkDescriptorImageInfo =
					{
						samplerState->getVkSampler(),	// sampler (VkSampler)
						vkImageView,					// imageView (VkImageView)
						VK_IMAGE_LAYOUT_PREINITIALIZED	// imageLayout (VkImageLayout)
					};
					const VkWriteDescriptorSet vkWriteDescriptorSet =
					{
						VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,			// sType (VkStructureType)
						nullptr,										// pNext (const void*)
						mGraphicsRootSignature->getVkDescriptorSet(),	// dstSet (VkDescriptorSet)
						rootParameterIndex,								// dstBinding (uint32_t)
						0,												// dstArrayElement (uint32_t)
						1,												// descriptorCount (uint32_t)
						VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,		// descriptorType (VkDescriptorType)
						&vkDescriptorImageInfo,							// pImageInfo (const VkDescriptorImageInfo*)
						nullptr,										// pBufferInfo (const VkDescriptorBufferInfo*)
						nullptr											// pTexelBufferView (const VkBufferView*)
					};
					vkUpdateDescriptorSets(getVulkanContext().getVkDevice(), 1, &vkWriteDescriptorSet, 0, nullptr);
					break;
				}

				case Renderer::ResourceType::TEXTURE_BUFFER:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				case Renderer::ResourceType::TEXTURE_CUBE:
				{
					// TODO(co) Implement me
					break;
				}

				case Renderer::ResourceType::SAMPLER_STATE:
				{
					// Unlike Direct3D >=10, Vulkan directly attaches the sampler settings to the texture
					mGraphicsRootSignature->setSamplerState(rootParameterIndex, static_cast<SamplerState*>(resource));
					break;
				}

				case Renderer::ResourceType::ROOT_SIGNATURE:
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
					RENDERER_LOG(mContext, CRITICAL, "Invalid Vulkan renderer backend resource type")
					break;
			}
		}
		else
		{
			// TODO(co) Handle this situation?
		}
	}

	void VulkanRenderer::setPipelineState(Renderer::IPipelineState* pipelineState)
	{
		if (nullptr != pipelineState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			VULKANRENDERER_RENDERERMATCHCHECK_RETURN(*this, *pipelineState)

			// Bind Vulkan pipeline
			vkCmdBindPipeline(getVulkanContext().getVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<PipelineState*>(pipelineState)->getVkPipeline());
		}
		else
		{
			// TODO(co) Handle this situation?
		}
	}


	//[-------------------------------------------------------]
	//[ Input-assembler (IA) stage                            ]
	//[-------------------------------------------------------]
	void VulkanRenderer::iaSetVertexArray(Renderer::IVertexArray* vertexArray)
	{
		// New vertex array?
		if (mVertexArray != vertexArray)
		{
			// Set a vertex array?
			if (nullptr != vertexArray)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				VULKANRENDERER_RENDERERMATCHCHECK_RETURN(*this, *vertexArray)

				// Unset the currently used vertex array
				iaUnsetVertexArray();

				// Set new vertex array and add a reference to it
				mVertexArray = static_cast<VertexArray*>(vertexArray);
				mVertexArray->addReference();

				// Bind Vulkan buffers
				static_cast<VertexArray*>(vertexArray)->bindVulkanBuffers(getVulkanContext().getVkCommandBuffer());
			}
			else
			{
				// Unset the currently used vertex array
				iaUnsetVertexArray();
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Rasterizer (RS) stage                                 ]
	//[-------------------------------------------------------]
	void VulkanRenderer::rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport* viewports)
	{
		// Sanity check
		assert((numberOfViewports > 0 && nullptr != viewports) && "Invalid rasterizer state viewports");

		// Set Vulkan viewport
		vkCmdSetViewport(getVulkanContext().getVkCommandBuffer(), 0, numberOfViewports, reinterpret_cast<const VkViewport*>(viewports));
	}

	void VulkanRenderer::rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle* scissorRectangles)
	{
		// Sanity check
		assert((numberOfScissorRectangles > 0 && nullptr != scissorRectangles) && "Invalid rasterizer state scissor rectangles");
		std::ignore = numberOfScissorRectangles;

		// Set Vulkan scissor
		// TODO(co) Add support for multiple scissor rectangles. Change "Renderer::ScissorRectangle" to Vulkan style to make it the primary API on the long run?
		const VkRect2D vkRect2D =
		{
			{ static_cast<int32_t>(scissorRectangles[0].topLeftX), static_cast<int32_t>(scissorRectangles[0].topLeftY) },
			{ static_cast<uint32_t>(scissorRectangles[0].bottomRightX - scissorRectangles[0].topLeftX), static_cast<uint32_t>(scissorRectangles[0].bottomRightY - scissorRectangles[0].topLeftY) }
		};
		vkCmdSetScissor(getVulkanContext().getVkCommandBuffer(), 0, 1, &vkRect2D);
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	void VulkanRenderer::omSetRenderTarget(Renderer::IRenderTarget* renderTarget)
	{
		// New render target?
		if (mRenderTarget != renderTarget)
		{
			// Release the render target reference, in case we have one
			if (nullptr != mRenderTarget)
			{
				// End Vulkan render pass if necessary
				if (mInsideVulkanRenderPass)
				{
					// Implicit image layout change transition
					vkCmdEndRenderPass(getVulkanContext().getVkCommandBuffer());
					mInsideVulkanRenderPass = false;
				}
				else
				{
					// Explicit image layout change transition: Handle Vulkan image memory barrier
					// -> Only needed when there's no Vulkan render pass which performs this implicit
					switch (mRenderTarget->getResourceType())
					{
						case Renderer::ResourceType::SWAP_CHAIN:
						{
							// Vulkan image memory barrier: Writing to present transition
							const SwapChain* swapChain = static_cast<SwapChain*>(mRenderTarget);
							const VkImageSubresourceRange vkImageSubresourceRange =
							{
								VK_IMAGE_ASPECT_COLOR_BIT,	// aspectMask (VkImageAspectFlags)
								0,							// baseMipLevel (uint32_t)
								1,							// levelCount (uint32_t)
								0,							// baseArrayLayer (uint32_t)
								1							// layerCount (uint32_t)
							};
							const VkImageMemoryBarrier vkImageMemoryBarrier =
							{
								VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,	// sType (VkStructureType)
								nullptr,								// pNext (const void*)
								VK_ACCESS_TRANSFER_WRITE_BIT,			// srcAccessMask (VkAccessFlags)
								VK_ACCESS_MEMORY_READ_BIT,				// dstAccessMask (VkAccessFlags)
								VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,	// oldLayout (VkImageLayout)
								VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,		// newLayout (VkImageLayout)
								VK_QUEUE_FAMILY_IGNORED,				// srcQueueFamilyIndex (uint32_t)
								VK_QUEUE_FAMILY_IGNORED,				// dstQueueFamilyIndex (uint32_t)
								swapChain->getCurrentVkImage(),			// image (VkImage)
								vkImageSubresourceRange					// subresourceRange (VkImageSubresourceRange)
							};
							vkCmdPipelineBarrier(getVulkanContext().getVkCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vkImageMemoryBarrier);
							break;
						}

						case Renderer::ResourceType::FRAMEBUFFER:
						{
							// TODO(co) Implement me
							break;
						}

						case Renderer::ResourceType::ROOT_SIGNATURE:
						case Renderer::ResourceType::PROGRAM:
						case Renderer::ResourceType::VERTEX_ARRAY:
						case Renderer::ResourceType::INDEX_BUFFER:
						case Renderer::ResourceType::VERTEX_BUFFER:
						case Renderer::ResourceType::UNIFORM_BUFFER:
						case Renderer::ResourceType::TEXTURE_BUFFER:
						case Renderer::ResourceType::INDIRECT_BUFFER:
						case Renderer::ResourceType::TEXTURE_1D:
						case Renderer::ResourceType::TEXTURE_2D:
						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
						case Renderer::ResourceType::TEXTURE_3D:
						case Renderer::ResourceType::TEXTURE_CUBE:
						case Renderer::ResourceType::PIPELINE_STATE:
						case Renderer::ResourceType::SAMPLER_STATE:
						case Renderer::ResourceType::VERTEX_SHADER:
						case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
						case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
						case Renderer::ResourceType::GEOMETRY_SHADER:
						case Renderer::ResourceType::FRAGMENT_SHADER:
						default:
							// Not handled in here
							break;
					}
				}

				// Release
				mRenderTarget->releaseReference();
				mRenderTarget = nullptr;
			}

			// Set a render target?
			if (nullptr != renderTarget)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				VULKANRENDERER_RENDERERMATCHCHECK_RETURN(*this, *renderTarget)

				// Set new render target and add a reference to it
				mRenderTarget = renderTarget;
				mRenderTarget->addReference();

				// Handle Vulkan image memory barrier
				switch (mRenderTarget->getResourceType())
				{
					case Renderer::ResourceType::SWAP_CHAIN:
					{
						// Vulkan image memory barrier: Present to writing transition
						const SwapChain* swapChain = static_cast<SwapChain*>(mRenderTarget);
						const VkImageSubresourceRange vkImageSubresourceRange =
						{
							VK_IMAGE_ASPECT_COLOR_BIT,	// aspectMask (VkImageAspectFlags)
							0,							// baseMipLevel (uint32_t)
							1,							// levelCount (uint32_t)
							0,							// baseArrayLayer (uint32_t)
							1							// layerCount (uint32_t)
						};
						const VkImageMemoryBarrier vkImageMemoryBarrier =
						{
							VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,	// sType (VkStructureType)
							nullptr,								// pNext (const void*)
							0,										// srcAccessMask (VkAccessFlags)
							VK_ACCESS_TRANSFER_WRITE_BIT,			// dstAccessMask (VkAccessFlags)
							VK_IMAGE_LAYOUT_UNDEFINED,				// oldLayout (VkImageLayout)
							VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,	// newLayout (VkImageLayout)
							VK_QUEUE_FAMILY_IGNORED,				// srcQueueFamilyIndex (uint32_t)
							VK_QUEUE_FAMILY_IGNORED,				// dstQueueFamilyIndex (uint32_t)
							swapChain->getCurrentVkImage(),			// image (VkImage)
							vkImageSubresourceRange					// subresourceRange (VkImageSubresourceRange)
						};
						vkCmdPipelineBarrier(getVulkanContext().getVkCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &vkImageMemoryBarrier);
						break;
					}

					case Renderer::ResourceType::FRAMEBUFFER:
					{
						// TODO(co) Implement me
						break;
					}

					case Renderer::ResourceType::ROOT_SIGNATURE:
					case Renderer::ResourceType::PROGRAM:
					case Renderer::ResourceType::VERTEX_ARRAY:
					case Renderer::ResourceType::INDEX_BUFFER:
					case Renderer::ResourceType::VERTEX_BUFFER:
					case Renderer::ResourceType::UNIFORM_BUFFER:
					case Renderer::ResourceType::TEXTURE_BUFFER:
					case Renderer::ResourceType::INDIRECT_BUFFER:
					case Renderer::ResourceType::TEXTURE_1D:
					case Renderer::ResourceType::TEXTURE_2D:
					case Renderer::ResourceType::TEXTURE_2D_ARRAY:
					case Renderer::ResourceType::TEXTURE_3D:
					case Renderer::ResourceType::TEXTURE_CUBE:
					case Renderer::ResourceType::PIPELINE_STATE:
					case Renderer::ResourceType::SAMPLER_STATE:
					case Renderer::ResourceType::VERTEX_SHADER:
					case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
					case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
					case Renderer::ResourceType::GEOMETRY_SHADER:
					case Renderer::ResourceType::FRAGMENT_SHADER:
					default:
						// Not handled in here
						break;
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Operations                                            ]
	//[-------------------------------------------------------]
	void VulkanRenderer::clear(uint32_t flags, const float color[4], float, uint32_t)
	{
		// Sanity check
		assert(nullptr != mRenderTarget && "Can't execute clear command without a render target set");
		assert(!mInsideVulkanRenderPass && "Can't execute clear command inside a Vulkan render pass");

		// Clear color
		if (flags & Renderer::ClearFlag::COLOR)
		{
			// Get the Vulkan image to clear
			VkImage vkImage = VK_NULL_HANDLE;
			switch (mRenderTarget->getResourceType())
			{
				case Renderer::ResourceType::SWAP_CHAIN:
					vkImage = static_cast<SwapChain*>(mRenderTarget)->getCurrentVkImage();
					break;

				case Renderer::ResourceType::FRAMEBUFFER:
					// TODO(co) Implement me
					break;

				case Renderer::ResourceType::ROOT_SIGNATURE:
				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::TEXTURE_BUFFER:
				case Renderer::ResourceType::INDIRECT_BUFFER:
				case Renderer::ResourceType::TEXTURE_1D:
				case Renderer::ResourceType::TEXTURE_2D:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				case Renderer::ResourceType::TEXTURE_3D:
				case Renderer::ResourceType::TEXTURE_CUBE:
				case Renderer::ResourceType::PIPELINE_STATE:
				case Renderer::ResourceType::SAMPLER_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
				default:
					// Not handled in here
					break;
			}
			assert(VK_NULL_HANDLE != vkImage && "Failed to get a Vulkan image");

			// Clear the Vulkan color image
			const VkClearColorValue vkClearColorValue =
			{
				color[0], color[1], color[2], color[3]
			};
			const VkImageSubresourceRange vkImageSubresourceRange =
			{
				VK_IMAGE_ASPECT_COLOR_BIT,	// aspectMask (VkImageAspectFlags)
				0,							// baseMipLevel (uint32_t)
				1,							// levelCount (uint32_t)
				0,							// baseArrayLayer (uint32_t)
				1							// layerCount (uint32_t)
			};
			vkCmdClearColorImage(getVulkanContext().getVkCommandBuffer(), vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &vkClearColorValue, 1, &vkImageSubresourceRange);
		}

		// Clear depth stencil
		if ((flags & Renderer::ClearFlag::DEPTH) || (flags & Renderer::ClearFlag::STENCIL))
		{
			// TODO(co) Implement me
			NOP;
			/*
			VKAPI_ATTR void VKAPI_CALL vkCmdClearDepthStencilImage(
				VkCommandBuffer                             commandBuffer,
				VkImage                                     image,
				VkImageLayout                               imageLayout,
				const VkClearDepthStencilValue*             pDepthStencil,
				uint32_t                                    rangeCount,
				const VkImageSubresourceRange*              pRanges);
			*/
		}
	}

	void VulkanRenderer::resolveMultisampleFramebuffer(Renderer::IRenderTarget&, Renderer::IFramebuffer&)
	{
		// TODO(co) Implement me
	}

	void VulkanRenderer::copyResource(Renderer::IResource&, Renderer::IResource&)
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Draw call                                             ]
	//[-------------------------------------------------------]
	void VulkanRenderer::draw(const Renderer::IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Sanity check
		assert(numberOfDraws > 0 && "Number of draws must not be zero");
		// It's possible to draw without "mVertexArray"

		// Before doing anything else: If there's emulation data, use it (for example "Renderer::IndirectBuffer" might have been used to generate the data)
		const uint8_t* emulationData = indirectBuffer.getEmulationData();
		if (nullptr != emulationData)
		{
			drawEmulated(emulationData, indirectBufferOffset, numberOfDraws);
		}
		else
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			VULKANRENDERER_RENDERERMATCHCHECK_RETURN(*this, indirectBuffer)

			// Start Vulkan render pass, if necessary
			if (!mInsideVulkanRenderPass)
			{
				beginVulkanRenderPass();
			}

			// Vulkan draw indirect command
			vkCmdDrawIndirect(getVulkanContext().getVkCommandBuffer(), static_cast<const IndirectBuffer&>(indirectBuffer).getVkBuffer(), indirectBufferOffset, numberOfDraws, sizeof(VkDrawIndirectCommand));
		}
	}

	void VulkanRenderer::drawEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Sanity checks
		assert(nullptr != emulationData);
		assert(numberOfDraws > 0 && "Number of draws must not be zero");
		// It's possible to draw without "mVertexArray"

		// TODO(co) Currently no buffer overflow check due to lack of interface provided data
		emulationData += indirectBufferOffset;

		// Start Vulkan render pass, if necessary
		if (!mInsideVulkanRenderPass)
		{
			beginVulkanRenderPass();
		}

		// Emit the draw calls
		const VkCommandBuffer vkCommandBuffer = getVulkanContext().getVkCommandBuffer();
		for (uint32_t i = 0; i < numberOfDraws; ++i)
		{
			// Draw and advance
			const Renderer::DrawInstancedArguments& drawInstancedArguments = *reinterpret_cast<const Renderer::DrawInstancedArguments*>(emulationData);
			vkCmdDraw(vkCommandBuffer, drawInstancedArguments.vertexCountPerInstance, drawInstancedArguments.instanceCount, drawInstancedArguments.startVertexLocation, drawInstancedArguments.startInstanceLocation);
			emulationData += sizeof(Renderer::DrawInstancedArguments);
		}
	}

	void VulkanRenderer::drawIndexed(const Renderer::IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Sanity checks
		assert(numberOfDraws > 0 && "Number of draws must not be zero");
		assert(nullptr != mVertexArray && "Draw indexed needs a set vertex array");
		assert(nullptr != mVertexArray->getIndexBuffer() && "Draw indexed needs a set vertex array which contains an index buffer");

		// Before doing anything else: If there's emulation data, use it (for example "Renderer::IndirectBuffer" might have been used to generate the data)
		const uint8_t* emulationData = indirectBuffer.getEmulationData();
		if (nullptr != emulationData)
		{
			drawIndexedEmulated(emulationData, indirectBufferOffset, numberOfDraws);
		}
		else
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			VULKANRENDERER_RENDERERMATCHCHECK_RETURN(*this, indirectBuffer)

			// Start Vulkan render pass, if necessary
			if (!mInsideVulkanRenderPass)
			{
				beginVulkanRenderPass();
			}

			// Vulkan draw indexed indirect command
			vkCmdDrawIndexedIndirect(getVulkanContext().getVkCommandBuffer(), static_cast<const IndirectBuffer&>(indirectBuffer).getVkBuffer(), indirectBufferOffset, numberOfDraws, sizeof(VkDrawIndexedIndirectCommand));
		}
	}

	void VulkanRenderer::drawIndexedEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Sanity checks
		assert(nullptr != emulationData);
		assert(numberOfDraws > 0 && "Number of draws must not be zero");
		assert(nullptr != mVertexArray && "Draw indexed needs a set vertex array");
		assert(nullptr != mVertexArray->getIndexBuffer() && "Draw indexed needs a set vertex array which contains an index buffer");

		// TODO(co) Currently no buffer overflow check due to lack of interface provided data
		emulationData += indirectBufferOffset;

		// Start Vulkan render pass, if necessary
		if (!mInsideVulkanRenderPass)
		{
			beginVulkanRenderPass();
		}

		// Emit the draw calls
		const VkCommandBuffer vkCommandBuffer = getVulkanContext().getVkCommandBuffer();
		for (uint32_t i = 0; i < numberOfDraws; ++i)
		{
			// Draw and advance
			const Renderer::DrawIndexedInstancedArguments& drawIndexedInstancedArguments = *reinterpret_cast<const Renderer::DrawIndexedInstancedArguments*>(emulationData);
			vkCmdDrawIndexed(vkCommandBuffer, drawIndexedInstancedArguments.indexCountPerInstance, drawIndexedInstancedArguments.instanceCount, drawIndexedInstancedArguments.startIndexLocation, drawIndexedInstancedArguments.baseVertexLocation, drawIndexedInstancedArguments.startInstanceLocation);
			emulationData += sizeof(Renderer::DrawIndexedInstancedArguments);
		}
	}


	//[-------------------------------------------------------]
	//[ Debug                                                 ]
	//[-------------------------------------------------------]
	void VulkanRenderer::setDebugMarker(const char*)
	{
		// TODO(co) Implement me
	}

	void VulkanRenderer::beginDebugEvent(const char*)
	{
		// TODO(co) Implement me
	}

	void VulkanRenderer::endDebugEvent()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	const char* VulkanRenderer::getName() const
	{
		return "Vulkan";
	}

	bool VulkanRenderer::isInitialized() const
	{
		// Is the Vulkan context initialized?
		return mVulkanContext->isInitialized();
	}

	bool VulkanRenderer::isDebugEnabled()
	{
		// TODO(co) Implement me

		// Debug disabled
		return false;
	}

	Renderer::ISwapChain* VulkanRenderer::getMainSwapChain() const
	{
		return mMainSwapChain;
	}


	//[-------------------------------------------------------]
	//[ Shader language                                       ]
	//[-------------------------------------------------------]
	uint32_t VulkanRenderer::getNumberOfShaderLanguages() const
	{
		// Done, return the number of supported shader languages
		return 1;
	}

	const char* VulkanRenderer::getShaderLanguageName(uint32_t) const
	{
		return ShaderLanguageGlsl::NAME;
	}

	Renderer::IShaderLanguage* VulkanRenderer::getShaderLanguage(const char* shaderLanguageName)
	{
		// In case "shaderLanguage" is a null pointer, use the default shader language
		if (nullptr != shaderLanguageName)
		{
			// Optimization: Check for shader language name pointer match, first
			// -> "ShaderLanguageSeparate::NAME" has the same value
			if (shaderLanguageName == ShaderLanguageGlsl::NAME || !stricmp(shaderLanguageName, ShaderLanguageGlsl::NAME))
			{
				// If required, create the GLSL shader language instance right now
				if (nullptr == mShaderLanguageGlsl)
				{
					mShaderLanguageGlsl = new ShaderLanguageGlsl(*this);
					mShaderLanguageGlsl->addReference();	// Internal renderer reference
				}
				return mShaderLanguageGlsl;
			}
		}
		else
		{
			// Return the shader language instance as default
			return getShaderLanguage(ShaderLanguageGlsl::NAME);
		}

		// Error!
		return nullptr;
	}


	//[-------------------------------------------------------]
	//[ Resource creation                                     ]
	//[-------------------------------------------------------]
	Renderer::ISwapChain* VulkanRenderer::createSwapChain(handle nativeWindowHandle, bool)
	{
		// The provided native window handle must not be a null handle
		return (NULL_HANDLE != nativeWindowHandle) ? new SwapChain(*this, nativeWindowHandle) : nullptr;
	}

	Renderer::IFramebuffer* VulkanRenderer::createFramebuffer(uint32_t numberOfColorFramebufferAttachments, const Renderer::FramebufferAttachment* colorFramebufferAttachments, const Renderer::FramebufferAttachment* depthStencilFramebufferAttachment)
	{
		return new Framebuffer(*this, numberOfColorFramebufferAttachments, colorFramebufferAttachments, depthStencilFramebufferAttachment);
	}

	Renderer::IBufferManager* VulkanRenderer::createBufferManager()
	{
		return new BufferManager(*this);
	}

	Renderer::ITextureManager* VulkanRenderer::createTextureManager()
	{
		return new TextureManager(*this);
	}

	Renderer::IRootSignature* VulkanRenderer::createRootSignature(const Renderer::RootSignature& rootSignature)
	{
		return new RootSignature(*this, rootSignature);
	}

	Renderer::IPipelineState* VulkanRenderer::createPipelineState(const Renderer::PipelineState& pipelineState)
	{
		return new PipelineState(*this, pipelineState);
	}

	Renderer::ISamplerState* VulkanRenderer::createSamplerState(const Renderer::SamplerState& samplerState)
	{
		return new SamplerState(*this, samplerState);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool VulkanRenderer::map(Renderer::IResource&, uint32_t, Renderer::MapType, uint32_t, Renderer::MappedSubresource&)
	{
		// TODO(co) Implement me
		return false;
	}

	void VulkanRenderer::unmap(Renderer::IResource&, uint32_t)
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Operations                                            ]
	//[-------------------------------------------------------]
	bool VulkanRenderer::beginScene()
	{
		// Begin Vulkan command buffer
		// -> This automatically resets the Vulkan command buffer in case it was previously already recorded
		const VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,	// sType (VkStructureType)
			nullptr,										// pNext (const void*)
			0,												// flags (VkCommandBufferUsageFlags)
			nullptr											// pInheritanceInfo (const VkCommandBufferInheritanceInfo*)
		};
		if (vkBeginCommandBuffer(getVulkanContext().getVkCommandBuffer(), &vkCommandBufferBeginInfo) == VK_SUCCESS)
		{
			// Done
			return true;
		}
		else
		{
			// Error!
			RENDERER_LOG(getContext(), CRITICAL, "Failed to begin Vulkan command buffer instance")
			return false;
		}
	}

	void VulkanRenderer::submitCommandBuffer(const Renderer::CommandBuffer& commandBuffer)
	{
		// Loop through all commands
		uint8_t* commandPacketBuffer = const_cast<uint8_t*>(commandBuffer.getCommandPacketBuffer());	// TODO(co) Get rid of the evil const-cast
		Renderer::CommandPacket commandPacket = commandPacketBuffer;
		while (nullptr != commandPacket)
		{
			{ // Submit command packet
				const Renderer::CommandDispatchFunctionIndex commandDispatchFunctionIndex = Renderer::CommandPacketHelper::loadCommandDispatchFunctionIndex(commandPacket);
				const void* command = Renderer::CommandPacketHelper::loadCommand(commandPacket);
				detail::DISPATCH_FUNCTIONS[commandDispatchFunctionIndex](command, *this);
			}

			{ // Next command
				const uint32_t nextCommandPacketByteIndex = Renderer::CommandPacketHelper::getNextCommandPacketByteIndex(commandPacket);
				commandPacket = (~0u != nextCommandPacketByteIndex) ? &commandPacketBuffer[nextCommandPacketByteIndex] : nullptr;
			}
		}
	}

	void VulkanRenderer::endScene()
	{
		// We need to forget about the currently set render target
		omSetRenderTarget(nullptr);

		// We need to forget about the currently set vertex array
		iaUnsetVertexArray();

		// End Vulkan command buffer
		if (vkEndCommandBuffer(getVulkanContext().getVkCommandBuffer()) != VK_SUCCESS)
		{
			// Error!
			RENDERER_LOG(getContext(), CRITICAL, "Failed to end Vulkan command buffer instance")
		}
	}


	//[-------------------------------------------------------]
	//[ Synchronization                                       ]
	//[-------------------------------------------------------]
	void VulkanRenderer::flush()
	{
		// TODO(co) Implement me
	}

	void VulkanRenderer::finish()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void VulkanRenderer::initializeCapabilities()
	{
		// TODO(co) Implement me, this in here is just a placeholder implementation

		{ // "D3D_FEATURE_LEVEL_11_0"
			// Maximum number of viewports (always at least 1)
			mCapabilities.maximumNumberOfViewports = 16;

			// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
			mCapabilities.maximumNumberOfSimultaneousRenderTargets = 8;

			// Maximum texture dimension
			mCapabilities.maximumTextureDimension = 16384;

			// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
			mCapabilities.maximumNumberOf2DTextureArraySlices = 512;

			// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
			mCapabilities.maximumTextureBufferSize = 128 * 1024 * 1024;	// TODO(co) http://msdn.microsoft.com/en-us/library/ff476876%28v=vs.85%29.aspx does not mention the texture buffer? Currently the OpenGL 3 minimum is used: 128 MiB.

			// Maximum indirect buffer size in bytes (in case there's no support for indirect buffer it's 0)
			// TODO(co) Implement indirect buffer support
			mCapabilities.maximumIndirectBufferSize = 64 * 1024;	// 64 KiB

			// Maximum number of multisamples (always at least 1, usually 8)
			mCapabilities.maximumNumberOfMultisamples = 8;

			// Maximum anisotropy (always at least 1, usually 16)
			mCapabilities.maximumAnisotropy = 16;

			// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
			mCapabilities.instancedArrays = true;

			// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID)
			mCapabilities.drawInstanced = true;

			// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
			mCapabilities.maximumNumberOfPatchVertices = 32;

			// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
			mCapabilities.maximumNumberOfGsOutputVertices = 1024;	// TODO(co) http://msdn.microsoft.com/en-us/library/ff476876%28v=vs.85%29.aspx does not mention it, so I assume it's 1024
		}

		// The rest is the same for all feature levels

		// Maximum uniform buffer (UBO) size in bytes (usually at least 4096 * 16 bytes, in case there's no support for uniform buffer it's 0)
		// -> See https://msdn.microsoft.com/en-us/library/windows/desktop/ff819065(v=vs.85).aspx - "Resource Limits (Direct3D 11)" - "Number of elements in a constant buffer D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT (4096)"
		// -> One element = float4 = 16 bytes
		mCapabilities.maximumUniformBufferSize = 4096 * 16;

		// Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		mCapabilities.individualUniforms = false;

		// Base vertex supported for draw calls?
		mCapabilities.baseVertex = true;

		// Vulkan has native multi-threading
		mCapabilities.nativeMultiThreading = false;	// TODO(co) Enable native multi-threading when done

		// Direct3D 11 has shader bytecode support
		mCapabilities.shaderBytecode = true;

		// Is there support for vertex shaders (VS)?
		mCapabilities.vertexShader = true;

		// Is there support for fragment shaders (FS)?
		mCapabilities.fragmentShader = true;
	}

	void VulkanRenderer::iaUnsetVertexArray()
	{
		// Release the currently used vertex array reference, in case we have one
		if (nullptr != mVertexArray)
		{
			// Do nothing since the Vulkan specification says "bindingCount must be greater than 0"
			// vkCmdBindVertexBuffers(getVulkanContext().getVkCommandBuffer(), 0, 0, nullptr, nullptr);

			// Release reference
			mVertexArray->releaseReference();
			mVertexArray = nullptr;
		}
	}

	void VulkanRenderer::beginVulkanRenderPass()
	{
		// Sanity checks
		assert(!mInsideVulkanRenderPass && "We're already inside a Vulkan render pass");
		assert(nullptr != mRenderTarget && "Can't begin a Vulkan render pass without a render target set");

		// Start Vulkan render pass
		switch (mRenderTarget->getResourceType())
		{
			case Renderer::ResourceType::SWAP_CHAIN:
			{
				const SwapChain* swapChain = static_cast<SwapChain*>(mRenderTarget);

				// Get render target dimension
				uint32_t width = 1;
				uint32_t height = 1;
				mRenderTarget->getWidthAndHeight(width, height);

				// Begin Vulkan render pass
				const VkRenderPassBeginInfo vkRenderPassBeginInfo =
				{
					VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,	// sType (VkStructureType)
					nullptr,									// pNext (const void*)
					swapChain->getVkRenderPass(),				// renderPass (VkRenderPass)
					swapChain->getCurrentVkFramebuffer(),		// framebuffer (VkFramebuffer)
					{ // renderArea (VkRect2D)
						{ 0, 0 },								// offset (VkOffset2D)
						{ width, height }						// extent (VkExtent2D)
					},
					0,											// clearValueCount (uint32_t)
					nullptr										// pClearValues (const VkClearValue*)
				};
				vkCmdBeginRenderPass(getVulkanContext().getVkCommandBuffer(), &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
				break;
			}

			case Renderer::ResourceType::FRAMEBUFFER:
			{
				// TODO(co) Implement me
				break;
			}

			case Renderer::ResourceType::ROOT_SIGNATURE:
			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
			case Renderer::ResourceType::INDEX_BUFFER:
			case Renderer::ResourceType::VERTEX_BUFFER:
			case Renderer::ResourceType::UNIFORM_BUFFER:
			case Renderer::ResourceType::TEXTURE_BUFFER:
			case Renderer::ResourceType::INDIRECT_BUFFER:
			case Renderer::ResourceType::TEXTURE_1D:
			case Renderer::ResourceType::TEXTURE_2D:
			case Renderer::ResourceType::TEXTURE_2D_ARRAY:
			case Renderer::ResourceType::TEXTURE_3D:
			case Renderer::ResourceType::TEXTURE_CUBE:
			case Renderer::ResourceType::PIPELINE_STATE:
			case Renderer::ResourceType::SAMPLER_STATE:
			case Renderer::ResourceType::VERTEX_SHADER:
			case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
			case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
			case Renderer::ResourceType::GEOMETRY_SHADER:
			case Renderer::ResourceType::FRAGMENT_SHADER:
			default:
				// Not handled in here
				break;
		}
		mInsideVulkanRenderPass = true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
