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
#include "VulkanRenderer/Texture/Texture2D.h"
#include "VulkanRenderer/Texture/Texture2DArray.h"
#include "VulkanRenderer/State/SamplerState.h"
#include "VulkanRenderer/State/PipelineState.h"
#include "VulkanRenderer/Shader/ShaderLanguageGlsl.h"
#include "VulkanRenderer/Shader/ProgramGlsl.h"

#include <Renderer/ILog.h>
#include <Renderer/Buffer/CommandBuffer.h>


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

			void SetPrimitiveTopology(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetPrimitiveTopology* realData = static_cast<const Renderer::Command::SetPrimitiveTopology*>(data);
				static_cast<VulkanRenderer::VulkanRenderer&>(renderer).iaSetPrimitiveTopology(realData->primitiveTopology);
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
					// No resource owner security check in here, we only support emulated indirect buffer
					static_cast<VulkanRenderer::VulkanRenderer&>(renderer).drawEmulated(realData->indirectBuffer->getEmulationData(), realData->indirectBufferOffset, realData->numberOfDraws);
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
					// No resource owner security check in here, we only support emulated indirect buffer
					static_cast<VulkanRenderer::VulkanRenderer&>(renderer).drawIndexedEmulated(realData->indirectBuffer->getEmulationData(), realData->indirectBufferOffset, realData->numberOfDraws);
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
			&BackendDispatch::SetPrimitiveTopology,
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
		mVkCommandBuffer(VK_NULL_HANDLE),
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

				{ // Create Vulkan command buffer instance
					const VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo =
					{
						VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,	// sType (VkStructureType)
						nullptr,										// pNext (const void*)
						mVulkanContext->getVkCommandPool(),				// commandPool (VkCommandPool)
						VK_COMMAND_BUFFER_LEVEL_PRIMARY,				// level (VkCommandBufferLevel)
						1												// commandBufferCount (uint32_t)
					};
					VkResult vkResult = vkAllocateCommandBuffers(mVulkanContext->getVkDevice(), &vkCommandBufferAllocateInfo, &mVkCommandBuffer);
					if (VK_SUCCESS == vkResult)
					{
						const VkCommandBufferBeginInfo vkCommandBufferBeginInfo =
						{
							VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,	// sType (VkStructureType)
							nullptr,										// pNext (const void*)
							0,												// flags (VkCommandBufferUsageFlags)
							nullptr,										// pInheritanceInfo (const VkCommandBufferInheritanceInfo*)
						};
						vkResult = vkBeginCommandBuffer(mVkCommandBuffer, &vkCommandBufferBeginInfo);
						if (VK_SUCCESS != vkResult)
						{
							// Error!
							RENDERER_LOG(mContext, CRITICAL, "Failed to begin Vulkan command buffer instance")
						}
					}
					else
					{
						// Error!
						RENDERER_LOG(mContext, CRITICAL, "Failed to create Vulkan command buffer instance")
					}
				}

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

					// Done
					mVulkanContext->flushSetupVkCommandBuffer();
				}
			}
		}
	}

	VulkanRenderer::~VulkanRenderer()
	{
		// Destroy Vulkan command buffer instance
		VkResult vkResult = vkEndCommandBuffer(mVkCommandBuffer);
		if (VK_SUCCESS == vkResult)
		{
			const VkSubmitInfo vkSubmitInfo =
			{
				VK_STRUCTURE_TYPE_SUBMIT_INFO,	// sType (VkStructureType)
				nullptr,						// pNext (const void*)
				0,								// waitSemaphoreCount (uint32_t)
				nullptr,						// pWaitSemaphores (const VkSemaphore*)
				nullptr,						// pWaitDstStageMask (const VkPipelineStageFlags*)
				1,								// commandBufferCount (uint32_t)
				&mVkCommandBuffer,				// pCommandBuffers (const VkCommandBuffer*)
				0,								// signalSemaphoreCount (uint32_t)
				nullptr							// pSignalSemaphores (const VkSemaphore*)
			};
			vkResult = vkQueueSubmit(mVulkanContext->getGraphicsVkQueue(), 1, &vkSubmitInfo, VK_NULL_HANDLE);
			if (VK_SUCCESS == vkResult)
			{
				vkResult = vkQueueWaitIdle(mVulkanContext->getGraphicsVkQueue());
				if (VK_SUCCESS != vkResult)
				{
					// Error!
					RENDERER_LOG(mContext, CRITICAL, "Failed to wait for Vulkan command buffer instance flush")
				}
			}
			else
			{
				// Error!
				RENDERER_LOG(mContext, CRITICAL, "Failed to submit the Vulkan command buffer instance")
			}
		}
		else
		{
			// Error!
			RENDERER_LOG(mContext, CRITICAL, "Failed to end the Vulkan command buffer instance")
		}

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

			// Get the root signature parameter instance
			// TODO(co) Implement me
			// const Renderer::RootParameter& rootParameter = mGraphicsRootSignature->getRootSignature().parameters[rootParameterIndex];
			// const Renderer::DescriptorRange* descriptorRange = rootParameter.descriptorTable.descriptorRanges;

			// Check the type of resource to set
			// TODO(co) Some additional resource type root signature security checks in debug build?
			// TODO(co) There's room for binding API call related optimization in here (will certainly be no huge overall efficiency gain)
			const Renderer::ResourceType resourceType = resource->getResourceType();
			switch (resourceType)
			{
				case Renderer::ResourceType::UNIFORM_BUFFER:
					// TODO(co) Implement me
					break;

				case Renderer::ResourceType::TEXTURE_BUFFER:
				case Renderer::ResourceType::TEXTURE_1D:
				case Renderer::ResourceType::TEXTURE_2D:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				case Renderer::ResourceType::TEXTURE_3D:
				case Renderer::ResourceType::TEXTURE_CUBE:
				{
					// TODO(co) Implement me
					break;
				}

				case Renderer::ResourceType::SAMPLER_STATE:
				{
					// TODO(co) Implement me
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

			// Set pipeline state
			static_cast<PipelineState*>(pipelineState)->bindPipelineState();
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

				// TODO(co) Implement me
			}
			else
			{
				// Unset the currently used vertex array
				iaUnsetVertexArray();
			}
		}
	}

	void VulkanRenderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology primitiveTopology)
	{
		// Tessellation support: Up to 32 vertices per patch are supported "Renderer::PrimitiveTopology::PATCH_LIST_1" ... "Renderer::PrimitiveTopology::PATCH_LIST_32"
		if (primitiveTopology >= Renderer::PrimitiveTopology::PATCH_LIST_1)
		{
			// Use tessellation

			// TODO(co) Implement me
		}
		else
		{
			// Do not use tessellation

			// TODO(co) Implement me
		}
	}


	//[-------------------------------------------------------]
	//[ Rasterizer (RS) stage                                 ]
	//[-------------------------------------------------------]
	void VulkanRenderer::rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport* viewports)
	{
		// Are the given viewports valid?
		if (numberOfViewports > 0 && nullptr != viewports)
		{
			vkCmdSetViewport(mVkCommandBuffer, 0, numberOfViewports, reinterpret_cast<const VkViewport*>(viewports));
		}
		else
		{
			// Error!
			assert(false);
		}
	}

	void VulkanRenderer::rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle* scissorRectangles)
	{
		// Are the given scissor rectangles valid?
		if (numberOfScissorRectangles > 0 && nullptr != scissorRectangles)
		{
			// TODO(co) Add support for multiple scissor rectangles. Change "Renderer::ScissorRectangle" to Vulkan style to make it the primary API on the long run?
			const VkRect2D vkRect2D =
			{
				{ static_cast<int32_t>(scissorRectangles[0].topLeftX), static_cast<int32_t>(scissorRectangles[0].topLeftY) },
				{ static_cast<uint32_t>(scissorRectangles[0].bottomRightX - scissorRectangles[0].topLeftX), static_cast<uint32_t>(scissorRectangles[0].bottomRightY - scissorRectangles[0].topLeftY) }
			};
			vkCmdSetScissor(mVkCommandBuffer, 0, 1, &vkRect2D);
		}
		else
		{
			// Error!
			assert(false);
		}
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	void VulkanRenderer::omSetRenderTarget(Renderer::IRenderTarget* renderTarget)
	{
		// New render target?
		if (mRenderTarget != renderTarget)
		{
			// Set a render target?
			if (nullptr != renderTarget)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				VULKANRENDERER_RENDERERMATCHCHECK_RETURN(*this, *renderTarget)

				// Release the render target reference, in case we have one
				if (nullptr != mRenderTarget)
				{
					// TODO(co) Implement me

					// Release
					mRenderTarget->releaseReference();
				}

				// Set new render target and add a reference to it
				mRenderTarget = renderTarget;
				mRenderTarget->addReference();

				// Evaluate the render target type
				switch (mRenderTarget->getResourceType())
				{
					case Renderer::ResourceType::SWAP_CHAIN:
					{
						// TODO(co) Implement me
						// static_cast<SwapChain*>(mRenderTarget)->getVulkanContext().makeCurrent();
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
			else
			{
				// TODO(co) Set no active render target

				// Release the render target reference, in case we have one
				if (nullptr != mRenderTarget)
				{
					mRenderTarget->releaseReference();
					mRenderTarget = nullptr;
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Operations                                            ]
	//[-------------------------------------------------------]
	void VulkanRenderer::clear(uint32_t, const float[4], float, uint32_t)
	{
		// TODO(co) Implement me
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
	void VulkanRenderer::drawEmulated(const uint8_t*, uint32_t, uint32_t)
	{
		// Is currently an vertex array set?
		if (nullptr != mVertexArray)
		{
			// Get the used index buffer
			IndexBuffer* indexBuffer = mVertexArray->getIndexBuffer();
			if (nullptr != indexBuffer)
			{
				// TODO(co) Implement me
			}
		}
	}

	void VulkanRenderer::drawIndexedEmulated(const uint8_t*, uint32_t, uint32_t)
	{
		// Is currently an vertex array set?
		if (nullptr != mVertexArray)
		{
			// Get the used index buffer
			IndexBuffer* indexBuffer = mVertexArray->getIndexBuffer();
			if (nullptr != indexBuffer)
			{
				// TODO(co) Implement me
			}
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
		// Optimization: Check for shader language name pointer match, first
		if (nullptr != shaderLanguageName && (shaderLanguageName == ShaderLanguageGlsl::NAME || !stricmp(shaderLanguageName, ShaderLanguageGlsl::NAME)))
		{
			// If required, create the GLSL shader language instance right now
			if (nullptr == mShaderLanguageGlsl)
			{
				mShaderLanguageGlsl = new ShaderLanguageGlsl(*this);
				mShaderLanguageGlsl->addReference();	// Internal renderer reference
			}

			// Return the shader language instance
			return mShaderLanguageGlsl;
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
		// Not required when using Vulkan

		// Done
		return true;
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
			// TODO(co) Implement me

			// Release reference
			mVertexArray->releaseReference();
			mVertexArray = nullptr;
		}
	}

	void VulkanRenderer::setProgram(Renderer::IProgram* program)
	{
		// TODO(co) Avoid changing already set program

		if (nullptr != program)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			VULKANRENDERER_RENDERERMATCHCHECK_RETURN(*this, *program)

			// TODO(co) GLSL buffer settings, unset previous program
			// TODO(co) Implement me
		}
		else
		{
			// TODO(co) GLSL buffer settings
			// TODO(co) Implement me
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
