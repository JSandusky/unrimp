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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/IRenderer.h>

#include "VulkanRenderer/Vulkan.h"

#include <array>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IVertexArray;
	class IRenderTarget;
	class IIndirectBuffer;
	class IResourceGroup;
}
namespace VulkanRenderer
{
	class SwapChain;
	class Extensions;
	class VertexArray;
	class RootSignature;
	class VulkanContext;
	class VulkanRuntimeLinking;
}


//[-------------------------------------------------------]
//[ Definitions                                           ]
//[-------------------------------------------------------]
#ifdef WIN32
	#ifndef CALLBACK
		#define CALLBACK __stdcall
	#endif
#elif defined LINUX
	#define CALLBACK
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Vulkan renderer class
	*/
	class VulkanRenderer : public Renderer::IRenderer
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class PipelineState;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::array<VkClearValue, 9>	VkClearValues;	///< 8 color render targets and one depth stencil render target


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] context
		*    Renderer context, the renderer context instance must stay valid as long as the renderer instance exists
		*
		*  @note
		*    - Do never ever use a not properly initialized renderer! Use "Renderer::IRenderer::isInitialized()" to check the initialization state.
		*/
		explicit VulkanRenderer(const Renderer::Context& context);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VulkanRenderer();

		/**
		*  @brief
		*    Return the Vulkan runtime linking instance
		*
		*  @return
		*    The Vulkan runtime linking instance, do not free the memory the reference is pointing to
		*/
		inline const VulkanRuntimeLinking& getVulkanRuntimeLinking() const;

		/**
		*  @brief
		*    Return the Vulkan context instance
		*
		*  @return
		*    The Vulkan context instance, do not free the memory the reference is pointing to
		*/
		inline const VulkanContext& getVulkanContext() const;

		/**
		*  @brief
		*    Return the available extensions
		*
		*  @return
		*    The available extensions, do not free the memory the reference is pointing to
		*/
		inline const Extensions& getExtensions() const;

		/**
		*  @brief
		*    Return the available extensions
		*
		*  @return
		*    The available extensions, do not free the memory the reference is pointing to
		*/
		inline Extensions& getExtensions();

		//[-------------------------------------------------------]
		//[ States                                                ]
		//[-------------------------------------------------------]
		void setGraphicsRootSignature(Renderer::IRootSignature* rootSignature);
		void setGraphicsResourceGroup(uint32_t rootParameterIndex, Renderer::IResourceGroup* resourceGroup);
		void setPipelineState(Renderer::IPipelineState* pipelineState);
		//[-------------------------------------------------------]
		//[ Input-assembler (IA) stage                            ]
		//[-------------------------------------------------------]
		void iaSetVertexArray(Renderer::IVertexArray* vertexArray);
		//[-------------------------------------------------------]
		//[ Rasterizer (RS) stage                                 ]
		//[-------------------------------------------------------]
		void rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport* viewports);
		void rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle* scissorRectangles);
		//[-------------------------------------------------------]
		//[ Output-merger (OM) stage                              ]
		//[-------------------------------------------------------]
		void omSetRenderTarget(Renderer::IRenderTarget* renderTarget);
		//[-------------------------------------------------------]
		//[ Operations                                            ]
		//[-------------------------------------------------------]
		void clear(uint32_t flags, const float color[4], float z, uint32_t stencil);
		void resolveMultisampleFramebuffer(Renderer::IRenderTarget& destinationRenderTarget, Renderer::IFramebuffer& sourceMultisampleFramebuffer);
		void copyResource(Renderer::IResource& destinationResource, Renderer::IResource& sourceResource);
		//[-------------------------------------------------------]
		//[ Draw call                                             ]
		//[-------------------------------------------------------]
		void draw(const Renderer::IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset = 0, uint32_t numberOfDraws = 1);
		void drawEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset = 0, uint32_t numberOfDraws = 1);
		void drawIndexed(const Renderer::IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset = 0, uint32_t numberOfDraws = 1);
		void drawIndexedEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset = 0, uint32_t numberOfDraws = 1);
		//[-------------------------------------------------------]
		//[ Debug                                                 ]
		//[-------------------------------------------------------]
		void setDebugMarker(const char* name);
		void beginDebugEvent(const char* name);
		void endDebugEvent();


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	public:
		virtual const char* getName() const override;
		virtual bool isInitialized() const override;
		virtual bool isDebugEnabled() override;
		virtual Renderer::ISwapChain* getMainSwapChain() const override;
		//[-------------------------------------------------------]
		//[ Shader language                                       ]
		//[-------------------------------------------------------]
		virtual uint32_t getNumberOfShaderLanguages() const override;
		virtual const char* getShaderLanguageName(uint32_t index) const override;
		virtual Renderer::IShaderLanguage* getShaderLanguage(const char* shaderLanguageName = nullptr) override;
		//[-------------------------------------------------------]
		//[ Resource creation                                     ]
		//[-------------------------------------------------------]
		virtual Renderer::ISwapChain* createSwapChain(handle nativeWindowHandle, bool useExternalContext) override;
		virtual Renderer::IFramebuffer* createFramebuffer(uint32_t numberOfColorFramebufferAttachments, const Renderer::FramebufferAttachment* colorFramebufferAttachments, const Renderer::FramebufferAttachment* depthStencilFramebufferAttachment = nullptr) override;
		virtual Renderer::IBufferManager* createBufferManager() override;
		virtual Renderer::ITextureManager* createTextureManager() override;
		virtual Renderer::IRootSignature* createRootSignature(const Renderer::RootSignature& rootSignature) override;
		virtual Renderer::IPipelineState* createPipelineState(const Renderer::PipelineState& pipelineState) override;
		virtual Renderer::ISamplerState* createSamplerState(const Renderer::SamplerState& samplerState) override;
		//[-------------------------------------------------------]
		//[ Resource handling                                     ]
		//[-------------------------------------------------------]
		virtual bool map(Renderer::IResource& resource, uint32_t subresource, Renderer::MapType mapType, uint32_t mapFlags, Renderer::MappedSubresource& mappedSubresource) override;
		virtual void unmap(Renderer::IResource& resource, uint32_t subresource) override;
		//[-------------------------------------------------------]
		//[ Operations                                            ]
		//[-------------------------------------------------------]
		virtual bool beginScene() override;
		virtual void submitCommandBuffer(const Renderer::CommandBuffer& commandBuffer) override;
		virtual void endScene() override;
		//[-------------------------------------------------------]
		//[ Synchronization                                       ]
		//[-------------------------------------------------------]
		virtual void flush() override;
		virtual void finish() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit VulkanRenderer(const VulkanRenderer& source) = delete;
		VulkanRenderer& operator =(const VulkanRenderer& source) = delete;

		/**
		*  @brief
		*    Initialize the capabilities
		*/
		void initializeCapabilities();

		/**
		*  @brief
		*    Unset the currently used vertex array
		*/
		void iaUnsetVertexArray();

		/**
		*  @brief
		*    Begin Vulkan render pass
		*/
		void beginVulkanRenderPass();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		VulkanRuntimeLinking*		mVulkanRuntimeLinking;		///< Vulkan runtime linking instance, always valid
		VulkanContext*				mVulkanContext;				///< Vulkan context instance, always valid
		Extensions*					mExtensions;				///< Extensions instance, always valid
		Renderer::IShaderLanguage*	mShaderLanguageGlsl;		///< GLSL shader language instance (we keep a reference to it), can be a null pointer
		RootSignature*				mGraphicsRootSignature;		///< Currently set graphics root signature (we keep a reference to it), can be a null pointer
		Renderer::ISamplerState*	mDefaultSamplerState;		///< Default rasterizer state (we keep a reference to it), can be a null pointer
		bool						mInsideVulkanRenderPass;	///< Some Vulkan commands like "vkCmdClearColorImage()" can only be executed outside a Vulkan render pass, so need to delay starting a Vulkan render pass
		VkClearValues				mVkClearValues;
		//[-------------------------------------------------------]
		//[ Input-assembler (IA) stage                            ]
		//[-------------------------------------------------------]
		VertexArray* mVertexArray;	///< Currently set vertex array (we keep a reference to it), can be a null pointer
		//[-------------------------------------------------------]
		//[ Output-merger (OM) stage                              ]
		//[-------------------------------------------------------]
		SwapChain*				 mMainSwapChain;	///< In case the optional native main window handle within the "VulkanRenderer"-constructor was not a null handle, this holds the instance of the main swap chain (we keep a reference to it), can be a null pointer
		Renderer::IRenderTarget* mRenderTarget;		///< Currently set render target (we keep a reference to it), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "VulkanRenderer/VulkanRenderer.inl"
