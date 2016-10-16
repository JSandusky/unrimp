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
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanDebug.h"	// For "VULKANRENDERER_RENDERERMATCHCHECK_RETURN()"
#include "VulkanRenderer/Mapping.h"
#include "VulkanRenderer/Extensions.h"
#include "VulkanRenderer/SwapChain.h"
#include "VulkanRenderer/Framebuffer.h"
#include "VulkanRenderer/RootSignature.h"
#include "VulkanRenderer/VulkanRuntimeLinking.h"
#include "VulkanRenderer/Buffer/VertexArray.h"
#include "VulkanRenderer/Buffer/IndexBuffer.h"
#include "VulkanRenderer/Buffer/VertexBuffer.h"
#include "VulkanRenderer/Buffer/UniformBuffer.h"
#include "VulkanRenderer/Texture/Texture2D.h"
#include "VulkanRenderer/Texture/SamplerState.h"
#include "VulkanRenderer/Texture/TextureBuffer.h"
#include "VulkanRenderer/Texture/Texture2DArray.h"
#include "VulkanRenderer/PipelineState/PipelineState.h"
#include "VulkanRenderer/Shader/ShaderLanguageGlsl.h"
#include "VulkanRenderer/Shader/ProgramGlsl.h"
#ifdef WIN32
	#include "VulkanRenderer/Windows/ContextWindows.h"
#elif defined LINUX
	#include "VulkanRenderer/Linux/ContextLinux.h"
#endif


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef VULKANRENDERER_EXPORTS
	#define VULKANRENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define VULKANRENDERER_API_EXPORT
#endif
// TODO(co) We might want to give this function a better name so one knows what it's about
VULKANRENDERER_API_EXPORT Renderer::IRenderer *createVulkanRendererInstance2(handle nativeWindowHandle, bool externalContext)
{
	return new VulkanRenderer::VulkanRenderer(nativeWindowHandle, externalContext);
}

VULKANRENDERER_API_EXPORT Renderer::IRenderer *createVulkanRendererInstance(handle nativeWindowHandle)
{
	return createVulkanRendererInstance2(nativeWindowHandle, false);
}
#undef VULKANRENDERER_API_EXPORT


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	#ifdef WIN32
	VulkanRenderer::VulkanRenderer(handle nativeWindowHandle, bool) :
	#elif defined LINUX
	VulkanRenderer::VulkanRenderer(handle nativeWindowHandle, bool useExternalContext) :
	#else
		#error "Unsupported platform"
	#endif
		mVulkanRuntimeLinking(new VulkanRuntimeLinking()),
		mContext(nullptr),
		mExtensions(nullptr),
		mShaderLanguageGlsl(nullptr),
		mGraphicsRootSignature(nullptr),
		mDefaultSamplerState(nullptr),
		mVertexArray(nullptr),
		mMainSwapChain(nullptr),
		mRenderTarget(nullptr)
	{
		// Is Vulkan available?
		if (mVulkanRuntimeLinking->isVulkanAvaiable())
		{
			#ifdef WIN32
				// TODO(co) Add external context support
				mContext = new ContextWindows(*this, nativeWindowHandle);
			#elif defined LINUX
				mContext = new ContextLinux(nativeWindowHandle, useExternalContext);
			#else
				#error "Unsupported platform"
			#endif

			// We're using "this" in here, so we are not allowed to write the following within the initializer list
			mExtensions = new Extensions(*mContext);

			// Is the context initialized?
			if (mContext->isInitialized())
			{
				// Initialize the Vulkan extensions
				mExtensions->initialize();

				#ifdef RENDERER_OUTPUT_DEBUG
					// TODO(co) Implement me
				#endif

				// Create the default sampler state
				mDefaultSamplerState = createSamplerState(Renderer::ISamplerState::getDefaultSamplerState());

				// Initialize the capabilities
				initializeCapabilities();

				// Add references to the default sampler state and set it
				if (nullptr != mDefaultSamplerState)
				{
					mDefaultSamplerState->addReference();
					// TODO(co) Set default sampler states
				}

				// Create a main swap chain instance?
				if (NULL_HANDLE != nativeWindowHandle)
				{
					// Create a main swap chain instance
					mMainSwapChain = new SwapChain(*this, nativeWindowHandle);
					RENDERER_SET_RESOURCE_DEBUG_NAME(mMainSwapChain, "Main swap chain")
					mMainSwapChain->addReference();	// Internal renderer reference

					// Done
					mContext->flushSetupVkCommandBuffer();
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
			mMainSwapChain->release();
			mMainSwapChain = nullptr;
		}
		if (nullptr != mRenderTarget)
		{
			mRenderTarget->release();
			mRenderTarget = nullptr;
		}
		if (nullptr != mDefaultSamplerState)
		{
			mDefaultSamplerState->release();
			mDefaultSamplerState = nullptr;
		}

		// Release the graphics root signature instance, in case we have one
		if (nullptr != mGraphicsRootSignature)
		{
			mGraphicsRootSignature->release();
		}

		{ // For debugging: At this point there should be no resource instances left, validate this!
			// -> Are the currently any resource instances?
			const unsigned long numberOfCurrentResources = getStatistics().getNumberOfCurrentResources();
			if (numberOfCurrentResources > 0)
			{
				// Error!
				if (numberOfCurrentResources > 1)
				{
					RENDERER_OUTPUT_DEBUG_PRINTF("Vulkan error: Renderer is going to be destroyed, but there are still %d resource instances left (memory leak)\n", numberOfCurrentResources)
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("Vulkan error: Renderer is going to be destroyed, but there is still one resource instance left (memory leak)\n")
				}

				// Use debug output to show the current number of resource instances
				getStatistics().debugOutputCurrentResouces();
			}
		}

		// Release the GLSL shader language instance, in case we have one
		if (nullptr != mShaderLanguageGlsl)
		{
			mShaderLanguageGlsl->release();
		}

		// Destroy the extensions instance
		delete mExtensions;

		// Destroy the Vulkan context instance
		delete mContext;

		// Destroy the Vulkan runtime linking instance
		delete mVulkanRuntimeLinking;
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	const char *VulkanRenderer::getName() const
	{
		return "Vulkan";
	}

	bool VulkanRenderer::isInitialized() const
	{
		// Is the context initialized?
		return mContext->isInitialized();
	}

	Renderer::ISwapChain *VulkanRenderer::getMainSwapChain() const
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

	const char *VulkanRenderer::getShaderLanguageName(uint32_t) const
	{
		return ShaderLanguageGlsl::NAME;
	}

	Renderer::IShaderLanguage *VulkanRenderer::getShaderLanguage(const char *shaderLanguageName)
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
	Renderer::ISwapChain *VulkanRenderer::createSwapChain(handle nativeWindowHandle)
	{
		// The provided native window handle must not be a null handle
		return (NULL_HANDLE != nativeWindowHandle) ? new SwapChain(*this, nativeWindowHandle) : nullptr;
	}

	Renderer::IFramebuffer *VulkanRenderer::createFramebuffer(uint32_t numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture)
	{
		return new Framebuffer(*this, numberOfColorTextures, colorTextures, depthStencilTexture);
	}

	Renderer::IVertexBuffer *VulkanRenderer::createVertexBuffer(uint32_t numberOfBytes, const void *data, Renderer::BufferUsage bufferUsage)
	{
		return new VertexBuffer(*this, numberOfBytes, data, bufferUsage);
	}

	Renderer::IIndexBuffer *VulkanRenderer::createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data, Renderer::BufferUsage bufferUsage)
	{
		return new IndexBuffer(*this, numberOfBytes, indexBufferFormat, data, bufferUsage);
	}

	Renderer::IVertexArray *VulkanRenderer::createVertexArray(const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, Renderer::IIndexBuffer *indexBuffer)
	{
		return new VertexArray(*this, vertexAttributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
	}

	Renderer::IUniformBuffer *VulkanRenderer::createUniformBuffer(uint32_t numberOfBytes, const void *data, Renderer::BufferUsage bufferUsage)
	{
		return new UniformBuffer(static_cast<VulkanRenderer&>(*this), numberOfBytes, data, bufferUsage);
	}

	Renderer::ITextureBuffer *VulkanRenderer::createTextureBuffer(uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data, Renderer::BufferUsage bufferUsage)
	{
		return new TextureBuffer(*this, numberOfBytes, textureFormat, data, bufferUsage);
	}

	Renderer::ITexture2D *VulkanRenderer::createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags, Renderer::TextureUsage, const Renderer::OptimizedTextureClearValue*)
	{
		// The indication of the texture usage is only relevant for Direct3D, Vulkan has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0)
		{
			return new Texture2D(*this, width, height, textureFormat, data, flags);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture2DArray *VulkanRenderer::createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, const void *data, uint32_t flags, Renderer::TextureUsage)
	{
		// The indication of the texture usage is only relevant for Direct3D, Vulkan has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0 && numberOfSlices > 0)
		{
			return new Texture2DArray(*this, width, height, numberOfSlices, textureFormat, data, flags);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::IRootSignature *VulkanRenderer::createRootSignature(const Renderer::RootSignature &rootSignature)
	{
		return new RootSignature(*this, rootSignature);
	}

	Renderer::IPipelineState *VulkanRenderer::createPipelineState(const Renderer::PipelineState & pipelineState)
	{
		return new PipelineState(*this, pipelineState);
	}

	Renderer::ISamplerState *VulkanRenderer::createSamplerState(const Renderer::SamplerState &samplerState)
	{
		return new SamplerState(*this, samplerState);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool VulkanRenderer::map(Renderer::IResource &, uint32_t, Renderer::MapType, uint32_t, Renderer::MappedSubresource &)
	{
		// TODO(co) Implement me
		return false;
	}

	void VulkanRenderer::unmap(Renderer::IResource &, uint32_t)
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void VulkanRenderer::setGraphicsRootSignature(Renderer::IRootSignature* rootSignature)
	{
		if (nullptr != mGraphicsRootSignature)
		{
			mGraphicsRootSignature->release();
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
				RENDERER_OUTPUT_DEBUG_STRING("Vulkan error: No graphics root signature set")
				return;
			}
			const Renderer::RootSignature& rootSignature = mGraphicsRootSignature->getRootSignature();
			if (rootParameterIndex >= rootSignature.numberOfParameters)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Vulkan error: Root parameter index is out of bounds")
				return;
			}
			const Renderer::RootParameter& rootParameter = rootSignature.parameters[rootParameterIndex];
			if (Renderer::RootParameterType::DESCRIPTOR_TABLE != rootParameter.parameterType)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Vulkan error: Root parameter index doesn't reference a descriptor table")
				return;
			}

			// TODO(co) For now, we only support a single descriptor range
			if (1 != rootParameter.descriptorTable.numberOfDescriptorRanges)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Vulkan error: Only a single descriptor range is supported")
				return;
			}
			if (nullptr == rootParameter.descriptorTable.descriptorRanges)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Vulkan error: Descriptor ranges is a null pointer")
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
				case Renderer::ResourceType::TEXTURE_2D:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
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
				case Renderer::ResourceType::PIPELINE_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
					RENDERER_OUTPUT_DEBUG_STRING("Vulkan error: Invalid resource type")
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
	void VulkanRenderer::iaSetVertexArray(Renderer::IVertexArray *vertexArray)
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
	void VulkanRenderer::rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport *viewports)
	{
		// Are the given viewports valid?
		if (numberOfViewports > 0 && nullptr != viewports)
		{
			// TODO(co) Implement me
		}
	}

	void VulkanRenderer::rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles)
	{
		// Are the given scissor rectangles valid?
		if (numberOfScissorRectangles > 0 && nullptr != scissorRectangles)
		{
			// TODO(co) Implement me
		}
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	Renderer::IRenderTarget *VulkanRenderer::omGetRenderTarget()
	{
		return mRenderTarget;
	}

	void VulkanRenderer::omSetRenderTarget(Renderer::IRenderTarget *renderTarget)
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
					mRenderTarget->release();
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
						// static_cast<SwapChain*>(mRenderTarget)->getContext().makeCurrent();
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
					case Renderer::ResourceType::TEXTURE_2D:
					case Renderer::ResourceType::TEXTURE_2D_ARRAY:
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
					mRenderTarget->release();
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

	bool VulkanRenderer::beginScene()
	{
		// Not required when using Vulkan

		// Done
		return true;
	}

	void VulkanRenderer::endScene()
	{
		// We need to forget about the currently set render target
		omSetRenderTarget(nullptr);

		// We need to forget about the currently set vertex array
		iaUnsetVertexArray();
	}


	//[-------------------------------------------------------]
	//[ Draw call                                             ]
	//[-------------------------------------------------------]
	void VulkanRenderer::draw(uint32_t, uint32_t)
	{
		// Is currently a vertex array set?
		if (nullptr != mVertexArray)
		{
			// Draw
			// TODO(co) Implement me
		}
	}

	void VulkanRenderer::drawInstanced(uint32_t, uint32_t, uint32_t)
	{
		// Is currently a vertex array set?
		if (nullptr != mVertexArray)
		{
			// Draw
			// TODO(co) Implement me
		}
	}

	void VulkanRenderer::drawIndexed(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
	{
		// Is currently an vertex array set?
		if (nullptr != mVertexArray)
		{
			// Get the used index buffer
			IndexBuffer *indexBuffer = mVertexArray->getIndexBuffer();
			if (nullptr != indexBuffer)
			{
				// TODO(co) Implement me
			}
		}
	}

	void VulkanRenderer::drawIndexedInstanced(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
	{
		// Is currently an vertex array set?
		if (nullptr != mVertexArray)
		{
			// Get the used index buffer
			IndexBuffer *indexBuffer = mVertexArray->getIndexBuffer();
			if (nullptr != indexBuffer)
			{
				// TODO(co) Implement me
			}
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
	//[ Debug                                                 ]
	//[-------------------------------------------------------]
	bool VulkanRenderer::isDebugEnabled()
	{
		// TODO(co) Implement me

		// Debug disabled
		return false;
	}

	void VulkanRenderer::setDebugMarker(const wchar_t *)
	{
		// TODO(co) Implement me
	}

	void VulkanRenderer::beginDebugEvent(const wchar_t *)
	{
		// TODO(co) Implement me
	}

	void VulkanRenderer::endDebugEvent()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void VulkanRenderer::initializeCapabilities()
	{
		// TODO(co) Implement me
	}

	void VulkanRenderer::iaUnsetVertexArray()
	{
		// Release the currently used vertex array reference, in case we have one
		if (nullptr != mVertexArray)
		{
			// TODO(co) Implement me

			// Release reference
			mVertexArray->release();
			mVertexArray = nullptr;
		}
	}

	void VulkanRenderer::setProgram(Renderer::IProgram *program)
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
