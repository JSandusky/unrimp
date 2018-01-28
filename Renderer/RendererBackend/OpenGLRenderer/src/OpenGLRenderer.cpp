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
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/OpenGLDebug.h"	// For "OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT()"
#include "OpenGLRenderer/Mapping.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/RootSignature.h"
#include "OpenGLRenderer/ResourceGroup.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"
#include "OpenGLRenderer/RenderTarget/SwapChain.h"
#include "OpenGLRenderer/RenderTarget/RenderPass.h"
#include "OpenGLRenderer/RenderTarget/FramebufferDsa.h"
#include "OpenGLRenderer/RenderTarget/FramebufferBind.h"
#include "OpenGLRenderer/Buffer/BufferManager.h"
#include "OpenGLRenderer/Buffer/TextureBuffer.h"
#include "OpenGLRenderer/Buffer/IndexBufferDsa.h"
#include "OpenGLRenderer/Buffer/IndexBufferBind.h"
#include "OpenGLRenderer/Buffer/VertexBufferDsa.h"
#include "OpenGLRenderer/Buffer/VertexBufferBind.h"
#include "OpenGLRenderer/Buffer/UniformBufferDsa.h"
#include "OpenGLRenderer/Buffer/UniformBufferBind.h"
#include "OpenGLRenderer/Buffer/VertexArrayNoVao.h"
#include "OpenGLRenderer/Buffer/VertexArrayVaoDsa.h"
#include "OpenGLRenderer/Buffer/VertexArrayVaoBind.h"
#include "OpenGLRenderer/Buffer/IndirectBuffer.h"
#include "OpenGLRenderer/Texture/TextureManager.h"
#include "OpenGLRenderer/Texture/Texture1D.h"
#include "OpenGLRenderer/Texture/Texture2D.h"
#include "OpenGLRenderer/Texture/Texture3D.h"
#include "OpenGLRenderer/Texture/TextureCube.h"
#include "OpenGLRenderer/Texture/Texture2DArray.h"
#include "OpenGLRenderer/State/SamplerStateSo.h"
#include "OpenGLRenderer/State/SamplerStateDsa.h"
#include "OpenGLRenderer/State/SamplerStateBind.h"
#include "OpenGLRenderer/State/PipelineState.h"
#include "OpenGLRenderer/Shader/Monolithic/ShaderLanguageMonolithic.h"
#include "OpenGLRenderer/Shader/Monolithic/ProgramMonolithic.h"
#include "OpenGLRenderer/Shader/Separate/ShaderLanguageSeparate.h"
#include "OpenGLRenderer/Shader/Separate/ProgramSeparate.h"
#include "OpenGLRenderer/Shader/Separate/VertexShaderSeparate.h"

#ifdef WIN32
	#include "OpenGLRenderer/Windows/OpenGLContextWindows.h"
#elif defined LINUX
	#include "OpenGLRenderer/Linux/OpenGLContextLinux.h"
#endif

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>
#include <Renderer/IAllocator.h>
#include <Renderer/Buffer/CommandBuffer.h>
#include <Renderer/Buffer/IndirectBufferTypes.h>


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef OPENGLRENDERER_EXPORTS
	#define OPENGLRENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define OPENGLRENDERER_API_EXPORT
#endif
OPENGLRENDERER_API_EXPORT Renderer::IRenderer* createOpenGLRendererInstance(const Renderer::Context& context)
{
	return RENDERER_NEW(context, OpenGLRenderer::OpenGLRenderer)(context);
}
#undef OPENGLRENDERER_API_EXPORT


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
		// Implementation from "08/02/2015 Better array 'countof' implementation with C++ 11 (updated)" - https://www.g-truc.net/post-0708.html
		template<typename T, std::size_t N>
		constexpr std::size_t countof(T const (&)[N])
		{
			return N;
		}

		bool mapBuffer(const Renderer::Context& context, const OpenGLRenderer::Extensions& extensions, GLenum target, GLenum bindingTarget, GLuint openGLBuffer, Renderer::MapType mapType, Renderer::MappedSubresource& mappedSubresource)
		{
			// TODO(co) This buffer update isn't efficient, use e.g. persistent buffer mapping

			// Is "GL_ARB_direct_state_access" there?
			if (extensions.isGL_ARB_direct_state_access())
			{
				// Effective direct state access (DSA)
				mappedSubresource.data		 = OpenGLRenderer::glMapNamedBuffer(openGLBuffer, OpenGLRenderer::Mapping::getOpenGLMapType(mapType));
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;
			}
			// Is "GL_EXT_direct_state_access" there?
			else if (extensions.isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				mappedSubresource.data		 = OpenGLRenderer::glMapNamedBufferEXT(openGLBuffer, OpenGLRenderer::Mapping::getOpenGLMapType(mapType));
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;
			}
			else
			{
				// Traditional bind version

				#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
					// Backup the currently bound OpenGL buffer
					GLint openGLBufferBackup = 0;
					OpenGLRenderer::glGetIntegerv(bindingTarget, &openGLBufferBackup);
				#else
					std::ignore = bindingTarget;
				#endif

				// Bind this OpenGL buffer
				OpenGLRenderer::glBindBufferARB(target, openGLBuffer);

				// Map
				mappedSubresource.data		 = OpenGLRenderer::glMapBufferARB(target, OpenGLRenderer::Mapping::getOpenGLMapType(mapType));
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;

				#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL buffer
					OpenGLRenderer::glBindBufferARB(target, static_cast<GLuint>(openGLBufferBackup));
				#endif
			}

			// Done
			RENDERER_ASSERT(context, nullptr != mappedSubresource.data, "Mapping of OpenGL buffer failed")
			return (nullptr != mappedSubresource.data);
		}

		void unmapBuffer(const OpenGLRenderer::Extensions& extensions, GLenum target, GLenum bindingTarget, GLuint openGLBuffer)
		{
			// Is "GL_ARB_direct_state_access" there?
			if (extensions.isGL_ARB_direct_state_access())
			{
				// Effective direct state access (DSA)
				OpenGLRenderer::glUnmapNamedBuffer(openGLBuffer);
			}
			// Is "GL_EXT_direct_state_access" there?
			else if (extensions.isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				OpenGLRenderer::glUnmapNamedBufferEXT(openGLBuffer);
			}
			else
			{
				// Traditional bind version

				#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
					// Backup the currently bound OpenGL buffer
					GLint openGLBufferBackup = 0;
					OpenGLRenderer::glGetIntegerv(bindingTarget, &openGLBufferBackup);
				#else
					std::ignore = bindingTarget;
				#endif

				// Bind this OpenGL buffer
				OpenGLRenderer::glBindBufferARB(target, openGLBuffer);

				// Unmap
				OpenGLRenderer::glUnmapBufferARB(target);

				#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL buffer
					OpenGLRenderer::glBindBufferARB(target, static_cast<GLuint>(openGLBufferBackup));
				#endif
			}
		}

		namespace BackendDispatch
		{


			//[-------------------------------------------------------]
			//[ Command buffer                                        ]
			//[-------------------------------------------------------]
			void ExecuteCommandBuffer(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::ExecuteCommandBuffer* realData = static_cast<const Renderer::Command::ExecuteCommandBuffer*>(data);
				RENDERER_ASSERT(renderer.getContext(), nullptr != realData->commandBufferToExecute, "The OpenGL command buffer to execute must be valid")
				renderer.submitCommandBuffer(*realData->commandBufferToExecute);
			}

			//[-------------------------------------------------------]
			//[ Graphics root                                         ]
			//[-------------------------------------------------------]
			void SetGraphicsRootSignature(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetGraphicsRootSignature* realData = static_cast<const Renderer::Command::SetGraphicsRootSignature*>(data);
				static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).setGraphicsRootSignature(realData->rootSignature);
			}

			void SetGraphicsResourceGroup(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetGraphicsResourceGroup* realData = static_cast<const Renderer::Command::SetGraphicsResourceGroup*>(data);
				static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).setGraphicsResourceGroup(realData->rootParameterIndex, realData->resourceGroup);
			}

			//[-------------------------------------------------------]
			//[ States                                                ]
			//[-------------------------------------------------------]
			void SetPipelineState(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetPipelineState* realData = static_cast<const Renderer::Command::SetPipelineState*>(data);
				static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).setPipelineState(realData->pipelineState);
			}

			//[-------------------------------------------------------]
			//[ Input-assembler (IA) stage                            ]
			//[-------------------------------------------------------]
			void SetVertexArray(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetVertexArray* realData = static_cast<const Renderer::Command::SetVertexArray*>(data);
				static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).iaSetVertexArray(realData->vertexArray);
			}

			//[-------------------------------------------------------]
			//[ Rasterizer (RS) stage                                 ]
			//[-------------------------------------------------------]
			void SetViewports(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetViewports* realData = static_cast<const Renderer::Command::SetViewports*>(data);
				static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).rsSetViewports(realData->numberOfViewports, (nullptr != realData->viewports) ? realData->viewports : reinterpret_cast<const Renderer::Viewport*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
			}

			void SetScissorRectangles(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetScissorRectangles* realData = static_cast<const Renderer::Command::SetScissorRectangles*>(data);
				static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).rsSetScissorRectangles(realData->numberOfScissorRectangles, (nullptr != realData->scissorRectangles) ? realData->scissorRectangles : reinterpret_cast<const Renderer::ScissorRectangle*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
			}

			//[-------------------------------------------------------]
			//[ Output-merger (OM) stage                              ]
			//[-------------------------------------------------------]
			void SetRenderTarget(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetRenderTarget* realData = static_cast<const Renderer::Command::SetRenderTarget*>(data);
				static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).omSetRenderTarget(realData->renderTarget);
			}

			//[-------------------------------------------------------]
			//[ Operations                                            ]
			//[-------------------------------------------------------]
			void Clear(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Clear* realData = static_cast<const Renderer::Command::Clear*>(data);
				static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).clear(realData->flags, realData->color, realData->z, realData->stencil);
			}

			void ResolveMultisampleFramebuffer(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::ResolveMultisampleFramebuffer* realData = static_cast<const Renderer::Command::ResolveMultisampleFramebuffer*>(data);
				static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).resolveMultisampleFramebuffer(*realData->destinationRenderTarget, *realData->sourceMultisampleFramebuffer);
			}

			void CopyResource(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::CopyResource* realData = static_cast<const Renderer::Command::CopyResource*>(data);
				static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).copyResource(*realData->destinationResource, *realData->sourceResource);
			}

			//[-------------------------------------------------------]
			//[ Draw call                                             ]
			//[-------------------------------------------------------]
			void Draw(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Draw* realData = static_cast<const Renderer::Command::Draw*>(data);
				if (nullptr != realData->indirectBuffer)
				{
					static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).draw(*realData->indirectBuffer, realData->indirectBufferOffset, realData->numberOfDraws);
				}
				else
				{
					static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).drawEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
				}
			}

			void DrawIndexed(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Draw* realData = static_cast<const Renderer::Command::Draw*>(data);
				if (nullptr != realData->indirectBuffer)
				{
					static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).drawIndexed(*realData->indirectBuffer, realData->indirectBufferOffset, realData->numberOfDraws);
				}
				else
				{
					static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).drawIndexedEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
				}
			}

			//[-------------------------------------------------------]
			//[ Debug                                                 ]
			//[-------------------------------------------------------]
			void SetDebugMarker(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetDebugMarker* realData = static_cast<const Renderer::Command::SetDebugMarker*>(data);
				static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).setDebugMarker(realData->name);
			}

			void BeginDebugEvent(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::BeginDebugEvent* realData = static_cast<const Renderer::Command::BeginDebugEvent*>(data);
				static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).beginDebugEvent(realData->name);
			}

			void EndDebugEvent(const void*, Renderer::IRenderer& renderer)
			{
				static_cast<OpenGLRenderer::OpenGLRenderer&>(renderer).endDebugEvent();
			}


		}


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		static const Renderer::BackendDispatchFunction DISPATCH_FUNCTIONS[Renderer::CommandDispatchFunctionIndex::NumberOfFunctions] =
		{
			// Command buffer
			&BackendDispatch::ExecuteCommandBuffer,
			// Graphics root
			&BackendDispatch::SetGraphicsRootSignature,
			&BackendDispatch::SetGraphicsResourceGroup,
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
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	OpenGLRenderer::OpenGLRenderer(const Renderer::Context& context) :
		IRenderer(context),
		mOpenGLRuntimeLinking(nullptr),
		mOpenGLContext(nullptr),
		mExtensions(nullptr),
		mShaderLanguage(nullptr),
		mGraphicsRootSignature(nullptr),
		mDefaultSamplerState(nullptr),
		mOpenGLCopyResourceFramebuffer(0),
		// States
		mPipelineState(nullptr),
		// Input-assembler (IA) stage
		mVertexArray(nullptr),
		mOpenGLPrimitiveTopology(0xFFFF),	// Unknown default setting
		mNumberOfVerticesPerPatch(0),
		// Output-merger (OM) stage
		mRenderTarget(nullptr),
		// State cache to avoid making redundant OpenGL calls
		mOpenGLClipControlOrigin(GL_INVALID_ENUM),
		mOpenGLProgramPipeline(0),
		mOpenGLProgram(0),
		mOpenGLIndirectBuffer(0),
		// Draw ID uniform location for "GL_ARB_base_instance"-emulation (see "17/11/2012 Surviving without gl_DrawID" - https://www.g-truc.net/post-0518.html)
		mOpenGLVertexProgram(0),
		mDrawIdUniformLocation(-1),
		mCurrentStartInstanceLocation(~0u)
	{
		// Is OpenGL available?
		mOpenGLRuntimeLinking = RENDERER_NEW(mContext, OpenGLRuntimeLinking)(*this);
		if (mOpenGLRuntimeLinking->isOpenGLAvaiable())
		{
			const handle nativeWindowHandle = mContext.getNativeWindowHandle();
			const Renderer::TextureFormat::Enum textureFormat = Renderer::TextureFormat::Enum::R8G8B8A8;
			const RenderPass renderPass(*this, 1, &textureFormat, Renderer::TextureFormat::Enum::UNKNOWN, 1);
			#ifdef WIN32
			{
				// TODO(co) Add external OpenGL context support
				mOpenGLContext = RENDERER_NEW(mContext, OpenGLContextWindows)(mOpenGLRuntimeLinking, renderPass, nativeWindowHandle);
			}
			#elif defined LINUX
				mOpenGLContext = RENDERER_NEW(mContext, OpenGLContextLinux)(*this, mOpenGLRuntimeLinking, renderPass, nativeWindowHandle, mContext.isUsingExternalContext());
			#else
				#error "Unsupported platform"
			#endif

			// We're using "this" in here, so we are not allowed to write the following within the initializer list
			mExtensions = RENDERER_NEW(mContext, Extensions)(*this, *mOpenGLContext);

			// Is the OpenGL context initialized?
			if (mOpenGLContext->isInitialized())
			{
				// Initialize the OpenGL extensions
				mExtensions->initialize();

				#ifdef RENDERER_DEBUG
					// "GL_ARB_debug_output"-extension available?
					if (mExtensions->isGL_ARB_debug_output())
					{
						// Synchronous debug output, please
						// -> Makes it easier to find the place causing the issue
						glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

						// Disable severity notifications, most drivers print many things with this severity
						glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, false);

						// Set the debug message callback function
						glDebugMessageCallbackARB(&OpenGLRenderer::debugMessageCallback, this);
					}
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
			}
		}
	}

	OpenGLRenderer::~OpenGLRenderer()
	{
		// Set no pipeline state reference, in case we have one
		if (nullptr != mPipelineState)
		{
			setPipelineState(nullptr);
		}

		// Set no vertex array reference, in case we have one
		if (nullptr != mVertexArray)
		{
			iaSetVertexArray(nullptr);
		}

		// Release instances
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

		// Destroy the OpenGL framebuffer used by "OpenGLRenderer::OpenGLRenderer::copyResource()" if the "GL_ARB_copy_image"-extension isn't available
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteFramebuffers(1, &mOpenGLCopyResourceFramebuffer);

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
					RENDERER_LOG(mContext, CRITICAL, "The OpenGL renderer backend is going to be destroyed, but there are still %lu resource instances left (memory leak)", numberOfCurrentResources)
				}
				else
				{
					RENDERER_LOG(mContext, CRITICAL, "The OpenGL renderer backend is going to be destroyed, but there is still one resource instance left (memory leak)")
				}

				// Use debug output to show the current number of resource instances
				getStatistics().debugOutputCurrentResouces(mContext);
			}
		}
		#endif

		// Release the shader language instance, in case we have one
		if (nullptr != mShaderLanguage)
		{
			mShaderLanguage->releaseReference();
		}

		// Destroy the extensions instance
		RENDERER_DELETE(mContext, Extensions, mExtensions);

		// Destroy the OpenGL context instance
		RENDERER_DELETE(mContext, IOpenGLContext, mOpenGLContext);

		// Destroy the OpenGL runtime linking instance
		RENDERER_DELETE(mContext, OpenGLRuntimeLinking, mOpenGLRuntimeLinking);
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::setGraphicsRootSignature(Renderer::IRootSignature* rootSignature)
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
			OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, *rootSignature)
		}
	}

	void OpenGLRenderer::setGraphicsResourceGroup(uint32_t rootParameterIndex, Renderer::IResourceGroup* resourceGroup)
	{
		// Security checks
		#ifdef RENDERER_DEBUG
		{
			if (nullptr == mGraphicsRootSignature)
			{
				RENDERER_LOG(mContext, CRITICAL, "No OpenGL renderer backend graphics root signature set")
				return;
			}
			const Renderer::RootSignature& rootSignature = mGraphicsRootSignature->getRootSignature();
			if (rootParameterIndex >= rootSignature.numberOfParameters)
			{
				RENDERER_LOG(mContext, CRITICAL, "The OpenGL renderer backend root parameter index is out of bounds")
				return;
			}
			const Renderer::RootParameter& rootParameter = rootSignature.parameters[rootParameterIndex];
			if (Renderer::RootParameterType::DESCRIPTOR_TABLE != rootParameter.parameterType)
			{
				RENDERER_LOG(mContext, CRITICAL, "The OpenGL renderer backend root parameter index doesn't reference a descriptor table")
				return;
			}
			if (nullptr == reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges))
			{
				RENDERER_LOG(mContext, CRITICAL, "The OpenGL renderer backend descriptor ranges is a null pointer")
				return;
			}
		}
		#endif

		if (nullptr != resourceGroup)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, *resourceGroup)

			// Set graphics resource group
			const ResourceGroup* openGLResourceGroup = static_cast<ResourceGroup*>(resourceGroup);
			const uint32_t numberOfResources = openGLResourceGroup->getNumberOfResources();
			Renderer::IResource** resources = openGLResourceGroup->getResources();
			const Renderer::RootParameter& rootParameter = mGraphicsRootSignature->getRootSignature().parameters[rootParameterIndex];
			for (uint32_t resourceIndex = 0; resourceIndex < numberOfResources; ++resourceIndex, ++resources)
			{
				Renderer::IResource* resource = *resources;
				RENDERER_ASSERT(mContext, nullptr != reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges), "Invalid OpenGL descriptor ranges")
				const Renderer::DescriptorRange& descriptorRange = reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges)[resourceIndex];

				// Check the type of resource to set
				// TODO(co) Some additional resource type root signature security checks in debug build?
				const Renderer::ResourceType resourceType = resource->getResourceType();
				switch (resourceType)
				{
					case Renderer::ResourceType::UNIFORM_BUFFER:
						// "GL_ARB_uniform_buffer_object" required
						if (mExtensions->isGL_ARB_uniform_buffer_object())
						{
							// Attach the buffer to the given UBO binding point
							// -> Explicit binding points ("layout(binding = 0)" in GLSL shader) requires OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension
							// -> Direct3D 10 and Direct3D 11 have explicit binding points
							RENDERER_ASSERT(mContext, nullptr != openGLResourceGroup->getResourceIndexToUniformBlockBindingIndex(), "Invalid OpenGL resource index to uniform block binding index")
							glBindBufferBase(GL_UNIFORM_BUFFER, openGLResourceGroup->getResourceIndexToUniformBlockBindingIndex()[resourceIndex], static_cast<UniformBuffer*>(resource)->getOpenGLUniformBuffer());
						}
						break;

					case Renderer::ResourceType::TEXTURE_BUFFER:
					case Renderer::ResourceType::TEXTURE_1D:
					case Renderer::ResourceType::TEXTURE_2D:
					case Renderer::ResourceType::TEXTURE_2D_ARRAY:
					case Renderer::ResourceType::TEXTURE_3D:
					case Renderer::ResourceType::TEXTURE_CUBE:
					{
						// In OpenGL, all shaders share the same texture units (= "Renderer::RootParameter::shaderVisibility" stays unused)

						// Is "GL_ARB_direct_state_access" or "GL_EXT_direct_state_access" there?
						if (mExtensions->isGL_ARB_direct_state_access() || mExtensions->isGL_EXT_direct_state_access())
						{
							// Effective direct state access (DSA)
							const bool isArbDsa = mExtensions->isGL_ARB_direct_state_access();

							// "glBindTextureUnit()" unit parameter is zero based so we can simply use the value we received
							const GLuint unit = descriptorRange.baseShaderRegister;

							// TODO(co) Some security checks might be wise *maximum number of texture units*
							// Evaluate the texture type
							switch (resourceType)
							{
								case Renderer::ResourceType::TEXTURE_BUFFER:
									if (isArbDsa)
									{
										glBindTextureUnit(unit, static_cast<TextureBuffer*>(resource)->getOpenGLTexture());
									}
									else
									{
										// "GL_TEXTURE0_ARB" is the first texture unit, while the unit we received is zero based
										glBindMultiTextureEXT(GL_TEXTURE0_ARB + unit, GL_TEXTURE_BUFFER_ARB, static_cast<TextureBuffer*>(resource)->getOpenGLTexture());
									}
									break;

								case Renderer::ResourceType::TEXTURE_1D:
									if (isArbDsa)
									{
										glBindTextureUnit(unit, static_cast<Texture1D*>(resource)->getOpenGLTexture());
									}
									else
									{
										// "GL_TEXTURE0_ARB" is the first texture unit, while the unit we received is zero based
										const Texture1D* texture1D = static_cast<Texture1D*>(resource);
										glBindMultiTextureEXT(GL_TEXTURE0_ARB + unit, GL_TEXTURE_1D, texture1D->getOpenGLTexture());
									}
									break;

								case Renderer::ResourceType::TEXTURE_2D:
									if (isArbDsa)
									{
										glBindTextureUnit(unit, static_cast<Texture2D*>(resource)->getOpenGLTexture());
									}
									else
									{
										// "GL_TEXTURE0_ARB" is the first texture unit, while the unit we received is zero based
										const Texture2D* texture2D = static_cast<Texture2D*>(resource);
										glBindMultiTextureEXT(GL_TEXTURE0_ARB + unit, static_cast<GLenum>((texture2D->getNumberOfMultisamples() > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D), texture2D->getOpenGLTexture());
									}
									break;

								case Renderer::ResourceType::TEXTURE_2D_ARRAY:
									// No texture 2D array extension check required, if we in here we already know it must exist
									if (isArbDsa)
									{
										glBindTextureUnit(unit, static_cast<Texture2DArray*>(resource)->getOpenGLTexture());
									}
									else
									{
										// "GL_TEXTURE0_ARB" is the first texture unit, while the unit we received is zero based
										glBindMultiTextureEXT(GL_TEXTURE0_ARB + unit, GL_TEXTURE_2D_ARRAY_EXT, static_cast<Texture2DArray*>(resource)->getOpenGLTexture());
									}
									break;

								case Renderer::ResourceType::TEXTURE_3D:
									if (isArbDsa)
									{
										glBindTextureUnit(unit, static_cast<Texture3D*>(resource)->getOpenGLTexture());
									}
									else
									{
										// "GL_TEXTURE0_ARB" is the first texture unit, while the unit we received is zero based
										const Texture3D* texture3D = static_cast<Texture3D*>(resource);
										glBindMultiTextureEXT(GL_TEXTURE0_ARB + unit, GL_TEXTURE_3D, texture3D->getOpenGLTexture());
									}
									break;

								case Renderer::ResourceType::TEXTURE_CUBE:
									if (isArbDsa)
									{
										glBindTextureUnit(unit, static_cast<TextureCube*>(resource)->getOpenGLTexture());
									}
									else
									{
										// "GL_TEXTURE0_ARB" is the first texture unit, while the unit we received is zero based
										const TextureCube* textureCube = static_cast<TextureCube*>(resource);
										glBindMultiTextureEXT(GL_TEXTURE0_ARB + unit, GL_TEXTURE_CUBE_MAP, textureCube->getOpenGLTexture());
									}
									break;

								case Renderer::ResourceType::ROOT_SIGNATURE:
								case Renderer::ResourceType::RESOURCE_GROUP:
								case Renderer::ResourceType::PROGRAM:
								case Renderer::ResourceType::VERTEX_ARRAY:
								case Renderer::ResourceType::RENDER_PASS:
								case Renderer::ResourceType::SWAP_CHAIN:
								case Renderer::ResourceType::FRAMEBUFFER:
								case Renderer::ResourceType::INDEX_BUFFER:
								case Renderer::ResourceType::VERTEX_BUFFER:
								case Renderer::ResourceType::UNIFORM_BUFFER:
								case Renderer::ResourceType::INDIRECT_BUFFER:
								case Renderer::ResourceType::PIPELINE_STATE:
								case Renderer::ResourceType::SAMPLER_STATE:
								case Renderer::ResourceType::VERTEX_SHADER:
								case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
								case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
								case Renderer::ResourceType::GEOMETRY_SHADER:
								case Renderer::ResourceType::FRAGMENT_SHADER:
									RENDERER_LOG(mContext, CRITICAL, "Invalid OpenGL renderer backend resource type")
									break;
							}

							// Set the OpenGL sampler states, if required (texture buffer has no sampler state), it's valid that there's no sampler state (e.g. texel fetch instead of sampling might be used)
							if (Renderer::ResourceType::TEXTURE_BUFFER != resourceType)
							{
								RENDERER_ASSERT(mContext, nullptr != openGLResourceGroup->getSamplerState(), "Invalid OpenGL sampler state")
								const SamplerState* samplerState = static_cast<const SamplerState*>(openGLResourceGroup->getSamplerState()[resourceIndex]);
								if (nullptr != samplerState)
								{
									// Is "GL_ARB_sampler_objects" there?
									if (mExtensions->isGL_ARB_sampler_objects())
									{
										// Effective sampler object (SO)
										glBindSampler(descriptorRange.baseShaderRegister, static_cast<const SamplerStateSo*>(samplerState)->getOpenGLSampler());
									}
									else
									{
										#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
											// Backup the currently active OpenGL texture
											GLint openGLActiveTextureBackup = 0;
											glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLActiveTextureBackup);
										#endif

										// TODO(co) Some security checks might be wise *maximum number of texture units*
										// Activate the texture unit we want to manipulate
										// -> "GL_TEXTURE0_ARB" is the first texture unit, while the unit we received is zero based
										glActiveTextureARB(GL_TEXTURE0_ARB + unit);

										// Is "GL_EXT_direct_state_access" there?
										if (mExtensions->isGL_EXT_direct_state_access())
										{
											// Direct state access (DSA) version to emulate a sampler object
											static_cast<const SamplerStateDsa*>(samplerState)->setOpenGLSamplerStates();
										}
										else
										{
											// Traditional bind version to emulate a sampler object
											static_cast<const SamplerStateBind*>(samplerState)->setOpenGLSamplerStates();
										}

										#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
											// Be polite and restore the previous active OpenGL texture
											glActiveTextureARB(static_cast<GLenum>(openGLActiveTextureBackup));
										#endif
									}
								}
							}
						}
						else
						{
							// Traditional bind version

							// "GL_ARB_multitexture" required
							if (mExtensions->isGL_ARB_multitexture())
							{
								#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
									// Backup the currently active OpenGL texture
									GLint openGLActiveTextureBackup = 0;
									glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLActiveTextureBackup);
								#endif

								// TODO(co) Some security checks might be wise *maximum number of texture units*
								// Activate the texture unit we want to manipulate
								// -> "GL_TEXTURE0_ARB" is the first texture unit, while the unit we received is zero based
								glActiveTextureARB(GL_TEXTURE0_ARB + descriptorRange.baseShaderRegister);

								// Evaluate the resource type
								switch (resourceType)
								{
									case Renderer::ResourceType::TEXTURE_BUFFER:
										glBindTexture(GL_TEXTURE_BUFFER_ARB, static_cast<TextureBuffer*>(resource)->getOpenGLTexture());
										break;

									case Renderer::ResourceType::TEXTURE_1D:
										glBindTexture(GL_TEXTURE_1D, static_cast<Texture1D*>(resource)->getOpenGLTexture());
										break;

									case Renderer::ResourceType::TEXTURE_2D:
									{
										const Texture2D* texture2D = static_cast<Texture2D*>(resource);
										glBindTexture(static_cast<GLenum>((texture2D->getNumberOfMultisamples() > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D), texture2D->getOpenGLTexture());
										break;
									}

									case Renderer::ResourceType::TEXTURE_2D_ARRAY:
										// No extension check required, if we in here we already know it must exist
										glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, static_cast<Texture2DArray*>(resource)->getOpenGLTexture());
										break;

									case Renderer::ResourceType::TEXTURE_3D:
										glBindTexture(GL_TEXTURE_3D, static_cast<Texture3D*>(resource)->getOpenGLTexture());
										break;

									case Renderer::ResourceType::TEXTURE_CUBE:
										glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<TextureCube*>(resource)->getOpenGLTexture());
										break;

									case Renderer::ResourceType::ROOT_SIGNATURE:
									case Renderer::ResourceType::RESOURCE_GROUP:
									case Renderer::ResourceType::PROGRAM:
									case Renderer::ResourceType::VERTEX_ARRAY:
									case Renderer::ResourceType::RENDER_PASS:
									case Renderer::ResourceType::SWAP_CHAIN:
									case Renderer::ResourceType::FRAMEBUFFER:
									case Renderer::ResourceType::INDEX_BUFFER:
									case Renderer::ResourceType::VERTEX_BUFFER:
									case Renderer::ResourceType::UNIFORM_BUFFER:
									case Renderer::ResourceType::INDIRECT_BUFFER:
									case Renderer::ResourceType::PIPELINE_STATE:
									case Renderer::ResourceType::SAMPLER_STATE:
									case Renderer::ResourceType::VERTEX_SHADER:
									case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
									case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
									case Renderer::ResourceType::GEOMETRY_SHADER:
									case Renderer::ResourceType::FRAGMENT_SHADER:
										RENDERER_LOG(mContext, CRITICAL, "Invalid OpenGL renderer backend resource type")
										break;
								}

								// Set the OpenGL sampler states, if required (texture buffer has no sampler state), it's valid that there's no sampler state (e.g. texel fetch instead of sampling might be used)
								if (Renderer::ResourceType::TEXTURE_BUFFER != resourceType)
								{
									RENDERER_ASSERT(mContext, nullptr != openGLResourceGroup->getSamplerState(), "Invalid OpenGL sampler state")
									const SamplerState* samplerState = static_cast<const SamplerState*>(openGLResourceGroup->getSamplerState()[resourceIndex]);
									if (nullptr != samplerState)
									{
										// Is "GL_ARB_sampler_objects" there?
										if (mExtensions->isGL_ARB_sampler_objects())
										{
											// Effective sampler object (SO)
											glBindSampler(descriptorRange.baseShaderRegister, static_cast<const SamplerStateSo*>(samplerState)->getOpenGLSampler());
										}
										// Is "GL_EXT_direct_state_access" there?
										else if (mExtensions->isGL_EXT_direct_state_access() || mExtensions->isGL_ARB_direct_state_access())
										{
											// Direct state access (DSA) version to emulate a sampler object
											static_cast<const SamplerStateDsa*>(samplerState)->setOpenGLSamplerStates();
										}
										else
										{
											// Traditional bind version to emulate a sampler object
											static_cast<const SamplerStateBind*>(samplerState)->setOpenGLSamplerStates();
										}
									}
								}

								#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
									// Be polite and restore the previous active OpenGL texture
									glActiveTextureARB(static_cast<GLenum>(openGLActiveTextureBackup));
								#endif
							}
						}
						break;
					}

					case Renderer::ResourceType::SAMPLER_STATE:
						// Unlike Direct3D >=10, OpenGL directly attaches the sampler settings to the texture (unless the sampler object extension is used)
						break;

					case Renderer::ResourceType::ROOT_SIGNATURE:
					case Renderer::ResourceType::RESOURCE_GROUP:
					case Renderer::ResourceType::PROGRAM:
					case Renderer::ResourceType::VERTEX_ARRAY:
					case Renderer::ResourceType::RENDER_PASS:
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
						RENDERER_LOG(mContext, CRITICAL, "Invalid Direct3D 11 renderer backend resource type")
						break;
				}
			}
		}
		else
		{
			// TODO(co) Handle this situation?
		}
	}

	void OpenGLRenderer::setPipelineState(Renderer::IPipelineState* pipelineState)
	{
		if (mPipelineState != pipelineState)
		{
			if (nullptr != pipelineState)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, *pipelineState)

				// Set new pipeline state and add a reference to it
				if (nullptr != mPipelineState)
				{
					mPipelineState->releaseReference();
				}
				mPipelineState = static_cast<PipelineState*>(pipelineState);
				mPipelineState->addReference();

				// Set OpenGL primitive topology
				mOpenGLPrimitiveTopology = mPipelineState->getOpenGLPrimitiveTopology();
				const int newNumberOfVerticesPerPatch = mPipelineState->getNumberOfVerticesPerPatch();
				if (0 != newNumberOfVerticesPerPatch && mNumberOfVerticesPerPatch != newNumberOfVerticesPerPatch)
				{
					mNumberOfVerticesPerPatch = newNumberOfVerticesPerPatch;
					glPatchParameteri(GL_PATCH_VERTICES, mNumberOfVerticesPerPatch);
				}

				// Set pipeline state
				mPipelineState->bindPipelineState();
			}
			else if (nullptr != mPipelineState)
			{
				// TODO(co) Handle this situation by resetting OpenGL states?
				mPipelineState->releaseReference();
				mPipelineState = nullptr;
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Input-assembler (IA) stage                            ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::iaSetVertexArray(Renderer::IVertexArray* vertexArray)
	{
		// New vertex array?
		if (mVertexArray != vertexArray)
		{
			// Set a vertex array?
			if (nullptr != vertexArray)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, *vertexArray)

				// Unset the currently used vertex array
				iaUnsetVertexArray();

				// Set new vertex array and add a reference to it
				mVertexArray = static_cast<VertexArray*>(vertexArray);
				mVertexArray->addReference();

				// Evaluate the internal array type of the new vertex array to set
				switch (static_cast<VertexArray*>(mVertexArray)->getInternalResourceType())
				{
					case VertexArray::InternalResourceType::NO_VAO:
						// Enable OpenGL vertex attribute arrays
						static_cast<VertexArrayNoVao*>(mVertexArray)->enableOpenGLVertexAttribArrays();
						break;

					case VertexArray::InternalResourceType::VAO:
						// Bind OpenGL vertex array
						glBindVertexArray(static_cast<VertexArrayVao*>(mVertexArray)->getOpenGLVertexArray());
						break;
				}
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
	void OpenGLRenderer::rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport* viewports)
	{
		// Sanity check
		RENDERER_ASSERT(mContext, numberOfViewports > 0 && nullptr != viewports, "Invalid OpenGL rasterizer state viewports")
		std::ignore = numberOfViewports;

		// In OpenGL, the origin of the viewport is left bottom while Direct3D is using a left top origin. To make the
		// Direct3D 11 implementation as efficient as possible the Direct3D convention is used and we have to convert in here.
		// -> This isn't influenced by the "GL_ARB_clip_control"-extension

		// Get the width and height of the current render target
		uint32_t renderTargetHeight = 1;
		if (nullptr != mRenderTarget)
		{
			uint32_t renderTargetWidth = 1;
			mRenderTarget->getWidthAndHeight(renderTargetWidth, renderTargetHeight);
		}

		// Set the OpenGL viewport
		// TODO(co) "GL_ARB_viewport_array" support ("OpenGLRenderer::rsSetViewports()")
		// TODO(co) Check for "numberOfViewports" out of range or are the debug events good enough?
		RENDERER_ASSERT(mContext, numberOfViewports <= 1, "OpenGL supports only one viewport")
		glViewport(static_cast<GLint>(viewports->topLeftX), static_cast<GLint>(renderTargetHeight - viewports->topLeftY - viewports->height), static_cast<GLsizei>(viewports->width), static_cast<GLsizei>(viewports->height));
		glDepthRange(static_cast<GLclampf>(viewports->minDepth), static_cast<GLclampf>(viewports->maxDepth));
	}

	void OpenGLRenderer::rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle* scissorRectangles)
	{
		// Sanity check
		RENDERER_ASSERT(mContext, numberOfScissorRectangles > 0 && nullptr != scissorRectangles, "Invalid OpenGL rasterizer state scissor rectangles")
		std::ignore = numberOfScissorRectangles;

		// In OpenGL, the origin of the scissor rectangle is left bottom while Direct3D is using a left top origin. To make the
		// Direct3D 9 & 10 & 11 implementation as efficient as possible the Direct3D convention is used and we have to convert in here.
		// -> This isn't influenced by the "GL_ARB_clip_control"-extension

		// Get the width and height of the current render target
		uint32_t renderTargetHeight = 1;
		if (nullptr != mRenderTarget)
		{
			uint32_t renderTargetWidth = 1;
			mRenderTarget->getWidthAndHeight(renderTargetWidth, renderTargetHeight);
		}

		// Set the OpenGL scissor rectangle
		// TODO(co) "GL_ARB_viewport_array" support ("OpenGLRenderer::rsSetViewports()")
		// TODO(co) Check for "numberOfViewports" out of range or are the debug events good enough?
		RENDERER_ASSERT(mContext, numberOfScissorRectangles <= 1, "OpenGL supports only one scissor rectangle")
		const GLsizei width  = scissorRectangles->bottomRightX - scissorRectangles->topLeftX;
		const GLsizei height = scissorRectangles->bottomRightY - scissorRectangles->topLeftY;
		glScissor(static_cast<GLint>(scissorRectangles->topLeftX), static_cast<GLint>(renderTargetHeight - scissorRectangles->topLeftY - height), width, height);
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::omSetRenderTarget(Renderer::IRenderTarget* renderTarget)
	{
		// New render target?
		if (mRenderTarget != renderTarget)
		{
			// Set a render target?
			if (nullptr != renderTarget)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, *renderTarget)

				// Release the render target reference, in case we have one
				Framebuffer* framebufferToGenerateMipmapsFor = nullptr;
				if (nullptr != mRenderTarget)
				{
					// Unbind OpenGL framebuffer?
					if (Renderer::ResourceType::FRAMEBUFFER == mRenderTarget->getResourceType() && Renderer::ResourceType::FRAMEBUFFER != renderTarget->getResourceType())
					{
						// Do we need to disable multisample?
						if (static_cast<Framebuffer*>(mRenderTarget)->isMultisampleRenderTarget())
						{
							glDisable(GL_MULTISAMPLE);
						}

						// We do not render into a OpenGL framebuffer
						glBindFramebuffer(GL_FRAMEBUFFER, 0);
					}

					// Generate mipmaps?
					if (Renderer::ResourceType::FRAMEBUFFER == mRenderTarget->getResourceType() && static_cast<Framebuffer*>(mRenderTarget)->getGenerateMipmaps())
					{
						framebufferToGenerateMipmapsFor = static_cast<Framebuffer*>(mRenderTarget);
					}
					else
					{
						// Release
						mRenderTarget->releaseReference();
					}
				}

				// Set new render target and add a reference to it
				mRenderTarget = renderTarget;
				mRenderTarget->addReference();

				// Evaluate the render target type
				GLenum clipControlOrigin = GL_UPPER_LEFT;
				switch (mRenderTarget->getResourceType())
				{
					case Renderer::ResourceType::SWAP_CHAIN:
					{
						static_cast<SwapChain*>(mRenderTarget)->getOpenGLContext().makeCurrent();
						clipControlOrigin = GL_LOWER_LEFT;	// Compensate OS window coordinate system y-flip
						break;
					}

					case Renderer::ResourceType::FRAMEBUFFER:
					{
						// Get the OpenGL framebuffer instance
						Framebuffer* framebuffer = static_cast<Framebuffer*>(mRenderTarget);

						// Bind the OpenGL framebuffer
						glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->getOpenGLFramebuffer());

						// Define the OpenGL buffers to draw into, "GL_ARB_draw_buffers"-extension required
						if (mExtensions->isGL_ARB_draw_buffers())
						{
							// https://www.opengl.org/registry/specs/ARB/draw_buffers.txt - "The draw buffer for output colors beyond <n> is set to NONE."
							// -> Meaning depth only rendering which has no color textures at all will work as well, no need for "glDrawBuffer(GL_NONE)"
							static const GLenum OPENGL_DRAW_BUFFER[16] =
							{
								GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1,  GL_COLOR_ATTACHMENT2,  GL_COLOR_ATTACHMENT3,
								GL_COLOR_ATTACHMENT4,  GL_COLOR_ATTACHMENT5,  GL_COLOR_ATTACHMENT6,  GL_COLOR_ATTACHMENT7,
								GL_COLOR_ATTACHMENT8,  GL_COLOR_ATTACHMENT9,  GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
								GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15
							};
							glDrawBuffersARB(static_cast<GLsizei>(framebuffer->getNumberOfColorTextures()), OPENGL_DRAW_BUFFER);
						}

						// Do we need to enable multisample?
						if (framebuffer->isMultisampleRenderTarget())
						{
							glEnable(GL_MULTISAMPLE);
						}
						break;
					}

					case Renderer::ResourceType::ROOT_SIGNATURE:
					case Renderer::ResourceType::RESOURCE_GROUP:
					case Renderer::ResourceType::PROGRAM:
					case Renderer::ResourceType::VERTEX_ARRAY:
					case Renderer::ResourceType::RENDER_PASS:
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

				// Generate mipmaps
				if (nullptr != framebufferToGenerateMipmapsFor)
				{
					framebufferToGenerateMipmapsFor->generateMipmaps();
					framebufferToGenerateMipmapsFor->releaseReference();
				}

				// Setup clip control
				if (mOpenGLClipControlOrigin != clipControlOrigin && mExtensions->isGL_ARB_clip_control())
				{
					// OpenGL default is "GL_LOWER_LEFT" and "GL_NEGATIVE_ONE_TO_ONE", change it to match Vulkan and Direct3D
					mOpenGLClipControlOrigin = clipControlOrigin;
					glClipControl(mOpenGLClipControlOrigin, GL_ZERO_TO_ONE);
				}
			}
			else if (nullptr != mRenderTarget)
			{
				// Evaluate the render target type
				if (Renderer::ResourceType::FRAMEBUFFER == mRenderTarget->getResourceType())
				{
					// We do not render into a OpenGL framebuffer
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
				}

				// TODO(co) Set no active render target

				// Release the render target reference, in case we have one
				mRenderTarget->releaseReference();
				mRenderTarget = nullptr;
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Operations                                            ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::clear(uint32_t flags, const float color[4], float z, uint32_t stencil)
	{
		// Get API flags
		uint32_t flagsApi = 0;
		if (flags & Renderer::ClearFlag::COLOR)
		{
			flagsApi |= GL_COLOR_BUFFER_BIT;
		}
		if (flags & Renderer::ClearFlag::DEPTH)
		{
			flagsApi |= GL_DEPTH_BUFFER_BIT;
		}
		if (flags & Renderer::ClearFlag::STENCIL)
		{
			flagsApi |= GL_STENCIL_BUFFER_BIT;
		}

		// Are API flags set?
		if (0 != flagsApi)
		{
			// Set clear settings
			if (flags & Renderer::ClearFlag::COLOR)
			{
				glClearColor(color[0], color[1], color[2], color[3]);
			}
			if (flags & Renderer::ClearFlag::DEPTH)
			{
				glClearDepth(z);
				if (nullptr != mPipelineState && Renderer::DepthWriteMask::ALL != mPipelineState->getDepthStencilState().depthWriteMask)
				{
					glDepthMask(GL_TRUE);
				}
			}
			if (flags & Renderer::ClearFlag::STENCIL)
			{
				glClearStencil(static_cast<GLint>(stencil));
			}

			// Unlike OpenGL, when using Direct3D 10 & 11 the scissor rectangle(s) do not affect the clear operation
			// -> We have to compensate the OpenGL behaviour in here

			// Disable OpenGL scissor test, in case it's not disabled, yet
			if (nullptr != mPipelineState && mPipelineState->getRasterizerState().scissorEnable)
			{
				glDisable(GL_SCISSOR_TEST);
			}

			// Clear
			glClear(flagsApi);

			// Restore the previously set OpenGL states
			if (nullptr != mPipelineState && mPipelineState->getRasterizerState().scissorEnable)
			{
				glEnable(GL_SCISSOR_TEST);
			}
			if ((flags & Renderer::ClearFlag::DEPTH) && nullptr != mPipelineState && Renderer::DepthWriteMask::ALL != mPipelineState->getDepthStencilState().depthWriteMask)
			{
				glDepthMask(GL_FALSE);
			}
		}
	}

	void OpenGLRenderer::resolveMultisampleFramebuffer(Renderer::IRenderTarget& destinationRenderTarget, Renderer::IFramebuffer& sourceMultisampleFramebuffer)
	{
		// Security check: Are the given resources owned by this renderer? (calls "return" in case of a mismatch)
		OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, destinationRenderTarget)
		OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, sourceMultisampleFramebuffer)

		// Evaluate the render target type
		switch (destinationRenderTarget.getResourceType())
		{
			case Renderer::ResourceType::SWAP_CHAIN:
			{
				// Get the OpenGL swap chain instance
				// TODO(co) Implement me, not that important in practice so not directly implemented
				// SwapChain& swapChain = static_cast<SwapChain&>(destinationRenderTarget);
				break;
			}

			case Renderer::ResourceType::FRAMEBUFFER:
			{
				// Get the OpenGL framebuffer instances
				const Framebuffer& openGLDestinationFramebuffer = static_cast<const Framebuffer&>(destinationRenderTarget);
				const Framebuffer& openGLSourceMultisampleFramebuffer = static_cast<const Framebuffer&>(sourceMultisampleFramebuffer);

				// Get the width and height of the destination and source framebuffer
				uint32_t destinationWidth = 1;
				uint32_t destinationHeight = 1;
				openGLDestinationFramebuffer.getWidthAndHeight(destinationWidth, destinationHeight);
				uint32_t sourceWidth = 1;
				uint32_t sourceHeight = 1;
				openGLSourceMultisampleFramebuffer.getWidthAndHeight(sourceWidth, sourceHeight);

				// Resolve multisample
				glBindFramebuffer(GL_READ_FRAMEBUFFER, openGLSourceMultisampleFramebuffer.getOpenGLFramebuffer());
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, openGLDestinationFramebuffer.getOpenGLFramebuffer());
				glBlitFramebuffer(
					0, 0, static_cast<GLint>(sourceWidth), static_cast<GLint>(sourceHeight),			// Source
					0, 0, static_cast<GLint>(destinationWidth), static_cast<GLint>(destinationHeight),	// Destination
					GL_COLOR_BUFFER_BIT, GL_NEAREST);
				break;
			}

			case Renderer::ResourceType::ROOT_SIGNATURE:
			case Renderer::ResourceType::RESOURCE_GROUP:
			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
			case Renderer::ResourceType::RENDER_PASS:
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

	void OpenGLRenderer::copyResource(Renderer::IResource& destinationResource, Renderer::IResource& sourceResource)
	{
		// Security check: Are the given resources owned by this renderer? (calls "return" in case of a mismatch)
		OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, destinationResource)
		OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, sourceResource)

		// Evaluate the render target type
		switch (destinationResource.getResourceType())
		{
			case Renderer::ResourceType::TEXTURE_2D:
				if (sourceResource.getResourceType() == Renderer::ResourceType::TEXTURE_2D)
				{
					// Get the OpenGL texture 2D instances
					const Texture2D& openGlDestinationTexture2D = static_cast<const Texture2D&>(destinationResource);
					const Texture2D& openGlSourceTexture2D = static_cast<const Texture2D&>(sourceResource);
					RENDERER_ASSERT(mContext, openGlDestinationTexture2D.getWidth() == openGlSourceTexture2D.getWidth(), "OpenGL source and destination width must be identical for resource copy")
					RENDERER_ASSERT(mContext, openGlDestinationTexture2D.getHeight() == openGlSourceTexture2D.getHeight(), "OpenGL source and destination height must be identical for resource copy")

					// Copy resource
					const GLsizei width = static_cast<GLsizei>(openGlDestinationTexture2D.getWidth());
					const GLsizei height = static_cast<GLsizei>(openGlDestinationTexture2D.getHeight());
					if (mExtensions->isGL_ARB_copy_image())
					{
						glCopyImageSubData(openGlSourceTexture2D.getOpenGLTexture(), GL_TEXTURE_2D, 0, 0, 0, 0,
										   openGlDestinationTexture2D.getOpenGLTexture(), GL_TEXTURE_2D, 0, 0, 0, 0,
										   width, height, 1);
					}
					else
					{
						#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
							// Backup the currently bound OpenGL framebuffer
							GLint openGLFramebufferBackup = 0;
							glGetIntegerv(GL_FRAMEBUFFER_BINDING, &openGLFramebufferBackup);
						#endif

						// Copy resource by using a framebuffer
						if (0 == mOpenGLCopyResourceFramebuffer)
						{
							glGenFramebuffers(1, &mOpenGLCopyResourceFramebuffer);
						}
						glBindFramebuffer(GL_FRAMEBUFFER, mOpenGLCopyResourceFramebuffer);
						glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGlSourceTexture2D.getOpenGLTexture(), 0);
						glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, openGlDestinationTexture2D.getOpenGLTexture(), 0);
						static const GLenum OPENGL_DRAW_BUFFER[1] =
						{
							GL_COLOR_ATTACHMENT1
						};
						glDrawBuffersARB(1, OPENGL_DRAW_BUFFER);	// We could use "glDrawBuffer(GL_COLOR_ATTACHMENT1);", but then we would also have to get the "glDrawBuffer()" function pointer, avoid OpenGL function overkill
						glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

						#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
							// Be polite and restore the previous bound OpenGL framebuffer
							glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(openGLFramebufferBackup));
						#endif
					}
				}
				else
				{
					// Error!
					RENDERER_ASSERT(mContext, false, "Failed to copy OpenGL resource")
				}
				break;

			case Renderer::ResourceType::ROOT_SIGNATURE:
			case Renderer::ResourceType::RESOURCE_GROUP:
			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
			case Renderer::ResourceType::RENDER_PASS:
			case Renderer::ResourceType::SWAP_CHAIN:
			case Renderer::ResourceType::FRAMEBUFFER:
			case Renderer::ResourceType::INDEX_BUFFER:
			case Renderer::ResourceType::VERTEX_BUFFER:
			case Renderer::ResourceType::UNIFORM_BUFFER:
			case Renderer::ResourceType::TEXTURE_BUFFER:
			case Renderer::ResourceType::INDIRECT_BUFFER:
			case Renderer::ResourceType::TEXTURE_1D:
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


	//[-------------------------------------------------------]
	//[ Draw call                                             ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::draw(const Renderer::IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Sanity check
		RENDERER_ASSERT(mContext, numberOfDraws > 0, "Number of OpenGL draws must not be zero")
		// It's possible to draw without "mVertexArray"

		// Tessellation support: "glPatchParameteri()" is called within "OpenGLRenderer::iaSetPrimitiveTopology()"

		// Before doing anything else: If there's emulation data, use it (for example "Renderer::IndirectBuffer" might have been used to generate the data)
		const uint8_t* emulationData = indirectBuffer.getEmulationData();
		if (nullptr != emulationData)
		{
			drawEmulated(emulationData, indirectBufferOffset, numberOfDraws);
		}
		else
		{
			// Sanity check
			RENDERER_ASSERT(mContext, mExtensions->isGL_ARB_draw_indirect(), "The GL_ARB_draw_indirect OpenGL extension isn't supported")

			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, indirectBuffer)

			{ // Bind indirect buffer
				const GLuint openGLIndirectBuffer = static_cast<const IndirectBuffer&>(indirectBuffer).getOpenGLIndirectBuffer();
				if (openGLIndirectBuffer != mOpenGLIndirectBuffer)
				{
					mOpenGLIndirectBuffer = openGLIndirectBuffer;
					glBindBufferARB(GL_DRAW_INDIRECT_BUFFER, mOpenGLIndirectBuffer);
				}
			}

			// Draw indirect
			if (1 == numberOfDraws)
			{
				glDrawArraysIndirect(mOpenGLPrimitiveTopology, reinterpret_cast<void*>(static_cast<uintptr_t>(indirectBufferOffset)));
			}
			else if (numberOfDraws > 1)
			{
				if (mExtensions->isGL_ARB_multi_draw_indirect())
				{
					glMultiDrawArraysIndirect(mOpenGLPrimitiveTopology, reinterpret_cast<void*>(static_cast<uintptr_t>(indirectBufferOffset)), static_cast<GLsizei>(numberOfDraws), 0);	// 0 = tightly packed
				}
				else
				{
					// TODO(co) Emulation (see specification for examples)
				}
			}
		}
	}

	void OpenGLRenderer::drawEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Sanity checks
		RENDERER_ASSERT(mContext, nullptr != emulationData, "The OpenGL emulation data must be valid")
		RENDERER_ASSERT(mContext, numberOfDraws > 0, "The number of OpenGL draws must not be zero")
		// It's possible to draw without "mVertexArray"

		// TODO(co) Currently no buffer overflow check due to lack of interface provided data
		emulationData += indirectBufferOffset;

		// Emit the draw calls
		for (uint32_t i = 0; i < numberOfDraws; ++i)
		{
			const Renderer::DrawInstancedArguments& drawInstancedArguments = *reinterpret_cast<const Renderer::DrawInstancedArguments*>(emulationData);
			updateGL_ARB_base_instanceEmulation(drawInstancedArguments.startInstanceLocation);

			// Draw and advance
			if ((drawInstancedArguments.instanceCount > 1 && mExtensions->isGL_ARB_draw_instanced()) || (drawInstancedArguments.startInstanceLocation > 0 && mExtensions->isGL_ARB_base_instance()))
			{
				// With instancing
				if (drawInstancedArguments.startInstanceLocation > 0 && mExtensions->isGL_ARB_base_instance())
				{
					glDrawArraysInstancedBaseInstance(mOpenGLPrimitiveTopology, static_cast<GLint>(drawInstancedArguments.startVertexLocation), static_cast<GLsizei>(drawInstancedArguments.vertexCountPerInstance), static_cast<GLsizei>(drawInstancedArguments.instanceCount), drawInstancedArguments.startInstanceLocation);
				}
				else
				{
					glDrawArraysInstancedARB(mOpenGLPrimitiveTopology, static_cast<GLint>(drawInstancedArguments.startVertexLocation), static_cast<GLsizei>(drawInstancedArguments.vertexCountPerInstance), static_cast<GLsizei>(drawInstancedArguments.instanceCount));
				}
			}
			else
			{
				// Without instancing
				RENDERER_ASSERT(mContext, drawInstancedArguments.instanceCount <= 1, "Invalid OpenGL instance count")
				glDrawArrays(mOpenGLPrimitiveTopology, static_cast<GLint>(drawInstancedArguments.startVertexLocation), static_cast<GLsizei>(drawInstancedArguments.vertexCountPerInstance));
			}
			emulationData += sizeof(Renderer::DrawInstancedArguments);
		}
	}

	void OpenGLRenderer::drawIndexed(const Renderer::IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Sanity checks
		RENDERER_ASSERT(mContext, numberOfDraws > 0, "Number of OpenGL draws must not be zero")
		RENDERER_ASSERT(mContext, nullptr != mVertexArray, "OpenGL draw indexed needs a set vertex array")
		RENDERER_ASSERT(mContext, nullptr != mVertexArray->getIndexBuffer(), "OpenGL draw indexed needs a set vertex array which contains an index buffer")

		// Tessellation support: "glPatchParameteri()" is called within "OpenGLRenderer::iaSetPrimitiveTopology()"

		// Before doing anything else: If there's emulation data, use it (for example "Renderer::IndirectBuffer" might have been used to generate the data)
		const uint8_t* emulationData = indirectBuffer.getEmulationData();
		if (nullptr != emulationData)
		{
			drawIndexedEmulated(emulationData, indirectBufferOffset, numberOfDraws);
		}
		else
		{
			// Sanity checks
			RENDERER_ASSERT(mContext, mExtensions->isGL_ARB_draw_indirect(), "The GL_ARB_draw_indirect OpenGL extension isn't supported")

			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, indirectBuffer)

			{ // Bind indirect buffer
				const GLuint openGLIndirectBuffer = static_cast<const IndirectBuffer&>(indirectBuffer).getOpenGLIndirectBuffer();
				if (openGLIndirectBuffer != mOpenGLIndirectBuffer)
				{
					mOpenGLIndirectBuffer = openGLIndirectBuffer;
					glBindBufferARB(GL_DRAW_INDIRECT_BUFFER, mOpenGLIndirectBuffer);
				}
			}

			// Draw indirect
			if (1 == numberOfDraws)
			{
				glDrawElementsIndirect(mOpenGLPrimitiveTopology, mVertexArray->getIndexBuffer()->getOpenGLType(), reinterpret_cast<void*>(static_cast<uintptr_t>(indirectBufferOffset)));
			}
			else if (numberOfDraws > 1)
			{
				if (mExtensions->isGL_ARB_multi_draw_indirect())
				{
					glMultiDrawElementsIndirect(mOpenGLPrimitiveTopology, mVertexArray->getIndexBuffer()->getOpenGLType(), reinterpret_cast<void*>(static_cast<uintptr_t>(indirectBufferOffset)), static_cast<GLsizei>(numberOfDraws), 0);	// 0 = tightly packed
				}
				else
				{
					// TODO(co) Emulation (see specification for examples)
				}
			}
		}
	}

	void OpenGLRenderer::drawIndexedEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Sanity checks
		RENDERER_ASSERT(mContext, nullptr != emulationData, "The OpenGL emulation data must be valid")
		RENDERER_ASSERT(mContext, numberOfDraws > 0, "The number of OpenGL draws must not be zero")
		RENDERER_ASSERT(mContext, nullptr != mVertexArray, "OpenGL draw indexed needs a set vertex array")
		RENDERER_ASSERT(mContext, nullptr != mVertexArray->getIndexBuffer(), "OpenGL draw indexed needs a set vertex array which contains an index buffer")

		// TODO(co) Currently no buffer overflow check due to lack of interface provided data
		emulationData += indirectBufferOffset;

		// Emit the draw calls
		IndexBuffer* indexBuffer = mVertexArray->getIndexBuffer();
		for (uint32_t i = 0; i < numberOfDraws; ++i)
		{
			const Renderer::DrawIndexedInstancedArguments& drawIndexedInstancedArguments = *reinterpret_cast<const Renderer::DrawIndexedInstancedArguments*>(emulationData);
			updateGL_ARB_base_instanceEmulation(drawIndexedInstancedArguments.startInstanceLocation);

			// Draw and advance
			if ((drawIndexedInstancedArguments.instanceCount > 1 && mExtensions->isGL_ARB_draw_instanced()) || (drawIndexedInstancedArguments.startInstanceLocation > 0 && mExtensions->isGL_ARB_base_instance()))
			{
				// With instancing
				if (drawIndexedInstancedArguments.baseVertexLocation > 0)
				{
					// Use start instance location?
					if (drawIndexedInstancedArguments.startInstanceLocation > 0 && mExtensions->isGL_ARB_base_instance())
					{
						// Draw with base vertex location and start instance location
						glDrawElementsInstancedBaseVertexBaseInstance(mOpenGLPrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLType(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())), static_cast<GLsizei>(drawIndexedInstancedArguments.instanceCount), static_cast<GLint>(drawIndexedInstancedArguments.baseVertexLocation), drawIndexedInstancedArguments.startInstanceLocation);
					}

					// Is the "GL_ARB_draw_elements_base_vertex" extension there?
					else if (mExtensions->isGL_ARB_draw_elements_base_vertex())
					{
						// Draw with base vertex location
						glDrawElementsInstancedBaseVertex(mOpenGLPrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLType(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())), static_cast<GLsizei>(drawIndexedInstancedArguments.instanceCount), static_cast<GLint>(drawIndexedInstancedArguments.baseVertexLocation));
					}
					else
					{
						// Error!
						RENDERER_ASSERT(mContext, false, "Failed to OpenGL draw indexed emulated")
					}
				}
				else if (drawIndexedInstancedArguments.startInstanceLocation > 0 && mExtensions->isGL_ARB_base_instance())
				{
					// Draw without base vertex location and with start instance location
					glDrawElementsInstancedBaseInstance(mOpenGLPrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLType(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())), static_cast<GLsizei>(drawIndexedInstancedArguments.instanceCount), drawIndexedInstancedArguments.startInstanceLocation);
				}
				else
				{
					// Draw without base vertex location
					glDrawElementsInstancedARB(mOpenGLPrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLType(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())), static_cast<GLsizei>(drawIndexedInstancedArguments.instanceCount));
				}
			}
			else
			{
				// Without instancing
				RENDERER_ASSERT(mContext, drawIndexedInstancedArguments.instanceCount <= 1, "Invalid OpenGL instance count")

				// Use base vertex location?
				if (drawIndexedInstancedArguments.baseVertexLocation > 0)
				{
					// Is the "GL_ARB_draw_elements_base_vertex" extension there?
					if (mExtensions->isGL_ARB_draw_elements_base_vertex())
					{
						// Draw with base vertex location
						glDrawElementsBaseVertex(mOpenGLPrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLType(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())), static_cast<GLint>(drawIndexedInstancedArguments.baseVertexLocation));
					}
					else
					{
						// Error!
						RENDERER_ASSERT(mContext, false, "Failed to OpenGL draw indexed emulated")
					}
				}
				else
				{
					// Draw without base vertex location
					glDrawElements(mOpenGLPrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLType(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())));
				}
			}
			emulationData += sizeof(Renderer::DrawIndexedInstancedArguments);
		}
	}


	//[-------------------------------------------------------]
	//[ Debug                                                 ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void OpenGLRenderer::setDebugMarker(const char* name)
		{
			// "GL_KHR_debug"-extension required
			if (mExtensions->isGL_KHR_debug())
			{
				RENDERER_ASSERT(mContext, nullptr != name, "OpenGL debug marker names must not be a null pointer")
				glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1, GL_DEBUG_SEVERITY_NOTIFICATION, -1, name);
			}
		}

		void OpenGLRenderer::beginDebugEvent(const char* name)
		{
			// "GL_KHR_debug"-extension required
			if (mExtensions->isGL_KHR_debug())
			{
				RENDERER_ASSERT(mContext, nullptr != name, "OpenGL debug event names must not be a null pointer")
				glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, name);
			}
		}

		void OpenGLRenderer::endDebugEvent()
		{
			// "GL_KHR_debug"-extension required
			if (mExtensions->isGL_KHR_debug())
			{
				glPopDebugGroup();
			}
		}
	#else
		void OpenGLRenderer::setDebugMarker(const char*)
		{
			// Nothing here
		}

		void OpenGLRenderer::beginDebugEvent(const char*)
		{
			// Nothing here
		}

		void OpenGLRenderer::endDebugEvent()
		{
			// Nothing here
		}
	#endif


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	const char* OpenGLRenderer::getName() const
	{
		return "OpenGL";
	}

	bool OpenGLRenderer::isInitialized() const
	{
		// Is the OpenGL context initialized?
		return (nullptr != mOpenGLContext && mOpenGLContext->isInitialized());
	}

	bool OpenGLRenderer::isDebugEnabled()
	{
		// OpenGL has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)

		// Debug disabled
		return false;
	}


	//[-------------------------------------------------------]
	//[ Shader language                                       ]
	//[-------------------------------------------------------]
	uint32_t OpenGLRenderer::getNumberOfShaderLanguages() const
	{
		uint32_t numberOfShaderLanguages = 0;

		// "GL_ARB_shader_objects" or "GL_ARB_separate_shader_objects" required
		if (mExtensions->isGL_ARB_shader_objects() || mExtensions->isGL_ARB_separate_shader_objects())
		{
			// GLSL supported
			++numberOfShaderLanguages;
		}

		// Done, return the number of supported shader languages
		return numberOfShaderLanguages;
	}

	const char* OpenGLRenderer::getShaderLanguageName(uint32_t index) const
	{
		// "GL_ARB_shader_objects" or "GL_ARB_separate_shader_objects" required
		if (mExtensions->isGL_ARB_shader_objects() || mExtensions->isGL_ARB_separate_shader_objects())
		{
			// GLSL supported
			if (0 == index)
			{
				return ShaderLanguageMonolithic::NAME;	// "ShaderLanguageSeparate::NAME" has the same value
			}
		}

		// Error!
		return nullptr;
	}

	Renderer::IShaderLanguage* OpenGLRenderer::getShaderLanguage(const char* shaderLanguageName)
	{
		// "GL_ARB_shader_objects" or "GL_ARB_separate_shader_objects" required
		if (mExtensions->isGL_ARB_shader_objects() || mExtensions->isGL_ARB_separate_shader_objects())
		{
			// In case "shaderLanguage" is a null pointer, use the default shader language
			if (nullptr != shaderLanguageName)
			{
				// Optimization: Check for shader language name pointer match, first
				// -> "ShaderLanguageSeparate::NAME" has the same value
				if (shaderLanguageName == ShaderLanguageMonolithic::NAME || !stricmp(shaderLanguageName, ShaderLanguageMonolithic::NAME))
				{
					// Prefer "GL_ARB_separate_shader_objects" over "GL_ARB_shader_objects"
					if (mExtensions->isGL_ARB_separate_shader_objects())
					{
						// If required, create the separate shader language instance right now
						if (nullptr == mShaderLanguage)
						{
							mShaderLanguage = RENDERER_NEW(mContext, ShaderLanguageSeparate)(*this);
							mShaderLanguage->addReference();	// Internal renderer reference
						}

						// Return the shader language instance
						return mShaderLanguage;
					}
					else if (mExtensions->isGL_ARB_shader_objects())
					{
						// If required, create the monolithic shader language instance right now
						if (nullptr == mShaderLanguage)
						{
							mShaderLanguage = RENDERER_NEW(mContext, ShaderLanguageMonolithic)(*this);
							mShaderLanguage->addReference();	// Internal renderer reference
						}

						// Return the shader language instance
						return mShaderLanguage;
					}
				}
			}
			else
			{
				// Return the shader language instance as default
				return getShaderLanguage(ShaderLanguageMonolithic::NAME);
			}
		}

		// Error!
		return nullptr;
	}


	//[-------------------------------------------------------]
	//[ Resource creation                                     ]
	//[-------------------------------------------------------]
	Renderer::IRenderPass* OpenGLRenderer::createRenderPass(uint32_t numberOfColorAttachments, const Renderer::TextureFormat::Enum* colorAttachmentTextureFormats, Renderer::TextureFormat::Enum depthStencilAttachmentTextureFormat, uint8_t numberOfMultisamples)
	{
		return RENDERER_NEW(mContext, RenderPass)(*this, numberOfColorAttachments, colorAttachmentTextureFormats, depthStencilAttachmentTextureFormat, numberOfMultisamples);
	}

	Renderer::ISwapChain* OpenGLRenderer::createSwapChain(Renderer::IRenderPass& renderPass, Renderer::WindowHandle windowHandle, bool useExternalContext)
	{
		// Sanity checks
		OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, renderPass)
		RENDERER_ASSERT(mContext, NULL_HANDLE != windowHandle.nativeWindowHandle || nullptr != windowHandle.renderWindow, "OpenGL: The provided native window handle or render window must not be a null handle / null pointer")

		// Create the swap chain
		return RENDERER_NEW(mContext, SwapChain)(renderPass, windowHandle, useExternalContext);
	}

	Renderer::IFramebuffer* OpenGLRenderer::createFramebuffer(Renderer::IRenderPass& renderPass, const Renderer::FramebufferAttachment* colorFramebufferAttachments, const Renderer::FramebufferAttachment* depthStencilFramebufferAttachment)
	{
		// Sanity check
		OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, renderPass)

		// "GL_ARB_framebuffer_object" required
		if (mExtensions->isGL_ARB_framebuffer_object())
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mExtensions->isGL_EXT_direct_state_access() || mExtensions->isGL_ARB_direct_state_access())
			{
				// Effective direct state access (DSA)
				// -> Validation is done inside the framebuffer implementation
				return RENDERER_NEW(mContext, FramebufferDsa)(renderPass, colorFramebufferAttachments, depthStencilFramebufferAttachment);
			}
			else
			{
				// Traditional bind version
				// -> Validation is done inside the framebuffer implementation
				return RENDERER_NEW(mContext, FramebufferBind)(renderPass, colorFramebufferAttachments, depthStencilFramebufferAttachment);
			}
		}
		else
		{
			// Error!
			return nullptr;
		}
	}

	Renderer::IBufferManager* OpenGLRenderer::createBufferManager()
	{
		return RENDERER_NEW(mContext, BufferManager)(*this);
	}

	Renderer::ITextureManager* OpenGLRenderer::createTextureManager()
	{
		return RENDERER_NEW(mContext, TextureManager)(*this);
	}

	Renderer::IRootSignature* OpenGLRenderer::createRootSignature(const Renderer::RootSignature& rootSignature)
	{
		return RENDERER_NEW(mContext, RootSignature)(*this, rootSignature);
	}

	Renderer::IPipelineState* OpenGLRenderer::createPipelineState(const Renderer::PipelineState& pipelineState)
	{
		return RENDERER_NEW(mContext, PipelineState)(*this, pipelineState);
	}

	Renderer::ISamplerState* OpenGLRenderer::createSamplerState(const Renderer::SamplerState& samplerState)
	{
		// Is "GL_ARB_sampler_objects" there?
		if (mExtensions->isGL_ARB_sampler_objects())
		{
			// Effective sampler object (SO)
			return RENDERER_NEW(mContext, SamplerStateSo)(*this, samplerState);
		}

		// Is "GL_EXT_direct_state_access" there?
		else if (mExtensions->isGL_EXT_direct_state_access() || mExtensions->isGL_ARB_direct_state_access())
		{
			// Direct state access (DSA) version to emulate a sampler object
			return RENDERER_NEW(mContext, SamplerStateDsa)(*this, samplerState);
		}
		else
		{
			// Traditional bind version to emulate a sampler object
			return RENDERER_NEW(mContext, SamplerStateBind)(*this, samplerState);
		}
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool OpenGLRenderer::map(Renderer::IResource& resource, uint32_t, Renderer::MapType mapType, uint32_t, Renderer::MappedSubresource& mappedSubresource)
	{
		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
				return ::detail::mapBuffer(mContext, *mExtensions, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB, static_cast<IndexBuffer&>(resource).getOpenGLElementArrayBuffer(), mapType, mappedSubresource);

			case Renderer::ResourceType::VERTEX_BUFFER:
				return ::detail::mapBuffer(mContext, *mExtensions, GL_ARRAY_BUFFER_ARB, GL_ARRAY_BUFFER_BINDING_ARB, static_cast<VertexBuffer&>(resource).getOpenGLArrayBuffer(), mapType, mappedSubresource);

			case Renderer::ResourceType::UNIFORM_BUFFER:
				return ::detail::mapBuffer(mContext, *mExtensions, GL_UNIFORM_BUFFER, GL_UNIFORM_BUFFER_BINDING, static_cast<UniformBuffer&>(resource).getOpenGLUniformBuffer(), mapType, mappedSubresource);

			case Renderer::ResourceType::TEXTURE_BUFFER:
				return ::detail::mapBuffer(mContext, *mExtensions, GL_TEXTURE_BUFFER_ARB, GL_TEXTURE_BINDING_BUFFER_ARB, static_cast<TextureBuffer&>(resource).getOpenGLTextureBuffer(), mapType, mappedSubresource);

			case Renderer::ResourceType::INDIRECT_BUFFER:
			{
				const IndirectBuffer& indirectBuffer = static_cast<IndirectBuffer&>(resource);
				uint8_t* emulationData = indirectBuffer.getWritableEmulationData();
				if (nullptr != emulationData)
				{
					mappedSubresource.data		 = emulationData;
					mappedSubresource.rowPitch   = 0;
					mappedSubresource.depthPitch = 0;

					// Done
					return true;
				}
				else
				{
					return ::detail::mapBuffer(mContext, *mExtensions, GL_DRAW_INDIRECT_BUFFER, GL_DRAW_INDIRECT_BUFFER_BINDING, indirectBuffer.getOpenGLIndirectBuffer(), mapType, mappedSubresource);
				}
				break;	// Impossible to reach, but still add it
			}

			case Renderer::ResourceType::TEXTURE_1D:
			{
				// TODO(co) Implement me
				return false;
			}

			case Renderer::ResourceType::TEXTURE_2D:
			{
				bool result = false;

				// TODO(co) Implement me
				/*
				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

				// Get the Direct3D 11 resource instance
				ID3D11Resource* d3d11Resource = nullptr;
				static_cast<Texture2D&>(resource).getD3D11ShaderResourceView()->GetResource(&d3d11Resource);
				if (nullptr != d3d11Resource)
				{
					// Map the Direct3D 11 resource
					result = (S_OK == mD3D11DeviceContext->Map(d3d11Resource, subresource, static_cast<D3D11_MAP>(mapType), mapFlags, reinterpret_cast<D3D11_MAPPED_SUBRESOURCE*>(&mappedSubresource)));

					// Release the Direct3D 11 resource instance
					d3d11Resource->Release();
				}

				// End debug event
				RENDERER_END_DEBUG_EVENT(this)
				*/

				// Done
				return result;
			}

			case Renderer::ResourceType::TEXTURE_2D_ARRAY:
			{
				bool result = false;

				// TODO(co) Implement me
				/*
				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

				// Get the Direct3D 11 resource instance
				ID3D11Resource* d3d11Resource = nullptr;
				static_cast<Texture2DArray&>(resource).getD3D11ShaderResourceView()->GetResource(&d3d11Resource);
				if (nullptr != d3d11Resource)
				{
					// Map the Direct3D 11 resource
					result = (S_OK == mD3D11DeviceContext->Map(d3d11Resource, subresource, static_cast<D3D11_MAP>(mapType), mapFlags, reinterpret_cast<D3D11_MAPPED_SUBRESOURCE*>(&mappedSubresource)));

					// Release the Direct3D 11 resource instance
					d3d11Resource->Release();
				}

				// End debug event
				RENDERER_END_DEBUG_EVENT(this)
				*/

				// Done
				return result;
			}

			case Renderer::ResourceType::TEXTURE_3D:
				return ::detail::mapBuffer(mContext, *mExtensions, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_BINDING_ARB, static_cast<Texture3D&>(resource).getOpenGLPixelUnpackBuffer(), mapType, mappedSubresource);

			case Renderer::ResourceType::TEXTURE_CUBE:
			{
				// TODO(co) Implement me
				return false;
			}

			case Renderer::ResourceType::ROOT_SIGNATURE:
			case Renderer::ResourceType::RESOURCE_GROUP:
			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
			case Renderer::ResourceType::RENDER_PASS:
			case Renderer::ResourceType::SWAP_CHAIN:
			case Renderer::ResourceType::FRAMEBUFFER:
			case Renderer::ResourceType::PIPELINE_STATE:
			case Renderer::ResourceType::SAMPLER_STATE:
			case Renderer::ResourceType::VERTEX_SHADER:
			case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
			case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
			case Renderer::ResourceType::GEOMETRY_SHADER:
			case Renderer::ResourceType::FRAGMENT_SHADER:
			default:
				// Nothing we can map, set known return values
				mappedSubresource.data		 = nullptr;
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;

				// Error!
				return false;
		}
	}

	void OpenGLRenderer::unmap(Renderer::IResource& resource, uint32_t)
	{
		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
				::detail::unmapBuffer(*mExtensions, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB, static_cast<IndexBuffer&>(resource).getOpenGLElementArrayBuffer());
				break;

			case Renderer::ResourceType::VERTEX_BUFFER:
				::detail::unmapBuffer(*mExtensions, GL_ARRAY_BUFFER_ARB, GL_ARRAY_BUFFER_BINDING_ARB, static_cast<VertexBuffer&>(resource).getOpenGLArrayBuffer());
				break;

			case Renderer::ResourceType::UNIFORM_BUFFER:
				::detail::unmapBuffer(*mExtensions, GL_UNIFORM_BUFFER, GL_UNIFORM_BUFFER_BINDING, static_cast<UniformBuffer&>(resource).getOpenGLUniformBuffer());
				break;

			case Renderer::ResourceType::TEXTURE_BUFFER:
				::detail::unmapBuffer(*mExtensions, GL_TEXTURE_BUFFER_ARB, GL_TEXTURE_BINDING_BUFFER_ARB, static_cast<TextureBuffer&>(resource).getOpenGLTextureBuffer());
				break;

			case Renderer::ResourceType::INDIRECT_BUFFER:
			{
				const IndirectBuffer& indirectBuffer = static_cast<IndirectBuffer&>(resource);
				if (nullptr == indirectBuffer.getEmulationData())
				{
					::detail::unmapBuffer(*mExtensions, GL_DRAW_INDIRECT_BUFFER, GL_DRAW_INDIRECT_BUFFER_BINDING, indirectBuffer.getOpenGLIndirectBuffer());
				}
				break;
			}

			case Renderer::ResourceType::TEXTURE_1D:
			{
				// TODO(co) Implement me
				break;
			}

			case Renderer::ResourceType::TEXTURE_2D:
			{
				// TODO(co) Implement me
				/*
				// Get the Direct3D 11 resource instance
				ID3D11Resource* d3d11Resource = nullptr;
				static_cast<Texture2D&>(resource).getD3D11ShaderResourceView()->GetResource(&d3d11Resource);
				if (nullptr != d3d11Resource)
				{
					// Unmap the Direct3D 11 resource
					mD3D11DeviceContext->Unmap(d3d11Resource, subresource);

					// Release the Direct3D 11 resource instance
					d3d11Resource->Release();
				}
				*/
				break;
			}

			case Renderer::ResourceType::TEXTURE_2D_ARRAY:
			{
				// TODO(co) Implement me
				/*
				// Get the Direct3D 11 resource instance
				ID3D11Resource* d3d11Resource = nullptr;
				static_cast<Texture2DArray&>(resource).getD3D11ShaderResourceView()->GetResource(&d3d11Resource);
				if (nullptr != d3d11Resource)
				{
					// Unmap the Direct3D 11 resource
					mD3D11DeviceContext->Unmap(d3d11Resource, subresource);

					// Release the Direct3D 11 resource instance
					d3d11Resource->Release();
				}
				*/
				break;
			}

			case Renderer::ResourceType::TEXTURE_3D:
			{
				// Unmap pixel unpack buffer
				const Texture3D& texture3D = static_cast<Texture3D&>(resource);
				const Renderer::TextureFormat::Enum textureFormat = texture3D.getTextureFormat();
				const uint32_t openGLPixelUnpackBuffer = texture3D.getOpenGLPixelUnpackBuffer();
				::detail::unmapBuffer(*mExtensions, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_BINDING_ARB, openGLPixelUnpackBuffer);

				// Backup the currently set alignment and currently bound OpenGL pixel unpack buffer
				#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
					GLint openGLAlignmentBackup = 0;
					glGetIntegerv(GL_UNPACK_ALIGNMENT, &openGLAlignmentBackup);
					GLint openGLUnpackBufferBackup = 0;
					glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING_ARB, &openGLUnpackBufferBackup);
				#endif
				glPixelStorei(GL_UNPACK_ALIGNMENT, (Renderer::TextureFormat::getNumberOfBytesPerElement(textureFormat) & 3) ? 1 : 4);

				// Copy pixel unpack buffer to texture
				glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, openGLPixelUnpackBuffer);
				if (mExtensions->isGL_EXT_direct_state_access() || mExtensions->isGL_ARB_direct_state_access())
				{
					// Effective direct state access (DSA)
					if (mExtensions->isGL_ARB_direct_state_access())
					{
						glTextureSubImage3D(texture3D.getOpenGLTexture(), 0, 0, 0, 0, static_cast<GLsizei>(texture3D.getWidth()), static_cast<GLsizei>(texture3D.getHeight()), static_cast<GLsizei>(texture3D.getDepth()), Mapping::getOpenGLFormat(textureFormat), Mapping::getOpenGLType(textureFormat), 0);
					}
					else
					{
						glTextureSubImage3DEXT(texture3D.getOpenGLTexture(), GL_TEXTURE_3D, 0, 0, 0, 0, static_cast<GLsizei>(texture3D.getWidth()), static_cast<GLsizei>(texture3D.getHeight()), static_cast<GLsizei>(texture3D.getDepth()), Mapping::getOpenGLFormat(textureFormat), Mapping::getOpenGLType(textureFormat), 0);
					}
				}
				else
				{
					// Traditional bind version

					// Backup the currently bound OpenGL texture
					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						GLint openGLTextureBackup = 0;
						glGetIntegerv(GL_TEXTURE_BINDING_3D, &openGLTextureBackup);
					#endif

					// Copy pixel unpack buffer to texture
					glBindTexture(GL_TEXTURE_3D, texture3D.getOpenGLTexture());
					glTexSubImage3DEXT(GL_TEXTURE_3D, 0, 0, 0, 0, static_cast<GLsizei>(texture3D.getWidth()), static_cast<GLsizei>(texture3D.getHeight()), static_cast<GLsizei>(texture3D.getDepth()), Mapping::getOpenGLFormat(textureFormat), Mapping::getOpenGLType(textureFormat), 0);

					// Be polite and restore the previous bound OpenGL texture
					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						glBindTexture(GL_TEXTURE_3D, static_cast<GLuint>(openGLTextureBackup));
					#endif
				}

				// Restore previous alignment and pixel unpack buffer
				#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
					glPixelStorei(GL_UNPACK_ALIGNMENT, openGLAlignmentBackup);
					glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, static_cast<GLuint>(openGLUnpackBufferBackup));
				#else
					glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
				#endif
				break;
			}

			case Renderer::ResourceType::TEXTURE_CUBE:
			{
				// TODO(co) Implement me
				break;
			}

			case Renderer::ResourceType::ROOT_SIGNATURE:
			case Renderer::ResourceType::RESOURCE_GROUP:
			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
			case Renderer::ResourceType::RENDER_PASS:
			case Renderer::ResourceType::SWAP_CHAIN:
			case Renderer::ResourceType::FRAMEBUFFER:
			case Renderer::ResourceType::PIPELINE_STATE:
			case Renderer::ResourceType::SAMPLER_STATE:
			case Renderer::ResourceType::VERTEX_SHADER:
			case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
			case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
			case Renderer::ResourceType::GEOMETRY_SHADER:
			case Renderer::ResourceType::FRAGMENT_SHADER:
			default:
				// Nothing we can unmap
				break;
		}
	}


	//[-------------------------------------------------------]
	//[ Operations                                            ]
	//[-------------------------------------------------------]
	bool OpenGLRenderer::beginScene()
	{
		// Not required when using OpenGL

		// Done
		return true;
	}

	void OpenGLRenderer::submitCommandBuffer(const Renderer::CommandBuffer& commandBuffer)
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

	void OpenGLRenderer::endScene()
	{
		// We need to forget about the currently set render target
		omSetRenderTarget(nullptr);

		// We need to forget about the currently set vertex array
		iaUnsetVertexArray();
	}


	//[-------------------------------------------------------]
	//[ Synchronization                                       ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::flush()
	{
		glFlush();
	}

	void OpenGLRenderer::finish()
	{
		glFinish();
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::selfDestruct()
	{
		RENDERER_DELETE(mContext, OpenGLRenderer, this);
	}


	//[-------------------------------------------------------]
	//[ Private static methods                                ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void OpenGLRenderer::debugMessageCallback(uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int, const char* message, const void* userParam)
		{
			// Source to string
			char debugSource[20 + 1]{0};	// +1 for terminating zero
			switch (source)
			{
				case GL_DEBUG_SOURCE_API_ARB:
					strncpy(debugSource, "OpenGL", 20);
					break;

				case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
					strncpy(debugSource, "Windows", 20);
					break;

				case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
					strncpy(debugSource, "Shader compiler", 20);
					break;

				case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
					strncpy(debugSource, "Third party", 20);
					break;

				case GL_DEBUG_SOURCE_APPLICATION_ARB:
					strncpy(debugSource, "Application", 20);
					break;

				case GL_DEBUG_SOURCE_OTHER_ARB:
					strncpy(debugSource, "Other", 20);
					break;

				default:
					strncpy(debugSource, "?", 20);
					break;
			}

			// Debug type to string
			Renderer::ILog::Type logType = Renderer::ILog::Type::CRITICAL;
			char debugType[25 + 1]{0};	// +1 for terminating zero
			switch (type)
			{
				case GL_DEBUG_TYPE_ERROR_ARB:
					strncpy(debugType, "Error", 25);
					break;

				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
					logType = Renderer::ILog::Type::COMPATIBILITY_WARNING;
					strncpy(debugType, "Deprecated behavior", 25);
					break;

				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
					strncpy(debugType, "Undefined behavior", 25);
					break;

				case GL_DEBUG_TYPE_PORTABILITY_ARB:
					logType = Renderer::ILog::Type::COMPATIBILITY_WARNING;
					strncpy(debugType, "Portability", 25);
					break;

				case GL_DEBUG_TYPE_PERFORMANCE_ARB:
					logType = Renderer::ILog::Type::PERFORMANCE_WARNING;
					strncpy(debugType, "Performance", 25);
					break;

				case GL_DEBUG_TYPE_OTHER_ARB:
					strncpy(debugType, "Other", 25);
					break;

				case GL_DEBUG_TYPE_MARKER:
					strncpy(debugType, "Marker", 25);
					break;

				case GL_DEBUG_TYPE_PUSH_GROUP:
					strncpy(debugType, "Push group", 25);
					break;

				case GL_DEBUG_TYPE_POP_GROUP:
					strncpy(debugType, "Pop group", 25);
					break;

				default:
					strncpy(debugType, "?", 25);
					break;
			}

			// Debug severity to string
			char debugSeverity[20 + 1]{0};	// +1 for terminating zero
			switch (severity)
			{
				case GL_DEBUG_SEVERITY_HIGH_ARB:
					strncpy(debugSeverity, "High", 20);
					break;

				case GL_DEBUG_SEVERITY_MEDIUM_ARB:
					strncpy(debugSeverity, "Medium", 20);
					break;

				case GL_DEBUG_SEVERITY_LOW_ARB:
					strncpy(debugSeverity, "Low", 20);
					break;

				case GL_DEBUG_SEVERITY_NOTIFICATION:
					strncpy(debugSeverity, "Notification", 20);
					break;

				default:
					strncpy(debugSeverity, "?", 20);
					break;
			}

			// Print into log
			if (static_cast<const OpenGLRenderer*>(userParam)->getContext().getLog().print(logType, nullptr, __FILE__, static_cast<uint32_t>(__LINE__), "OpenGL debug message\tSource:\"%s\"\tType:\"%s\"\tID:\"%d\"\tSeverity:\"%s\"\tMessage:\"%s\"", debugSource, debugType, id, debugSeverity, message))
			{
				DEBUG_BREAK;
			}
		}
	#else
		void OpenGLRenderer::debugMessageCallback(uint32_t, uint32_t, uint32_t, uint32_t, int, const char*, const void*)
		{
			// Nothing here
		}
	#endif


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::initializeCapabilities()
	{
		GLint openGLValue = 0;

		{ // Get device name
		  // -> OpenGL 4.3 Compatibility Profile Specification, section 22.2, page 627: "String queries return pointers to UTF-8 encoded, null-terminated static strings describing properties of the current GL context."
		  // -> For example "AMD Radeon R9 200 Series"
			const size_t numberOfCharacters = ::detail::countof(mCapabilities.deviceName) - 1;
			strncpy(mCapabilities.deviceName, reinterpret_cast<const char*>(glGetString(GL_RENDERER)), numberOfCharacters);
			mCapabilities.deviceName[numberOfCharacters] = '\0';
		}

		// Preferred swap chain texture format
		mCapabilities.preferredSwapChainColorTextureFormat		  = Renderer::TextureFormat::Enum::R8G8B8A8;
		mCapabilities.preferredSwapChainDepthStencilTextureFormat = Renderer::TextureFormat::Enum::D32_FLOAT;

		// Maximum number of viewports (always at least 1)
		// TODO(co) "GL_ARB_viewport_array" support ("OpenGLRenderer::rsSetViewports()")
		mCapabilities.maximumNumberOfViewports = 1;	// TODO(co) GL_ARB_viewport_array

		// Maximum number of simultaneous render targets (if <1 render to texture is not supported, "GL_ARB_draw_buffers" required)
		if (mExtensions->isGL_ARB_draw_buffers())
		{
			glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &openGLValue);
			mCapabilities.maximumNumberOfSimultaneousRenderTargets = static_cast<uint32_t>(openGLValue);
		}
		else
		{
			// "GL_ARB_framebuffer_object"-extension for render to texture required
			mCapabilities.maximumNumberOfSimultaneousRenderTargets = static_cast<uint32_t>(mExtensions->isGL_ARB_framebuffer_object());
		}

		// Maximum texture dimension
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &openGLValue);
		mCapabilities.maximumTextureDimension = static_cast<uint32_t>(openGLValue);

		// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
		if (mExtensions->isGL_EXT_texture_array())
		{
			glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS_EXT, &openGLValue);
			mCapabilities.maximumNumberOf2DTextureArraySlices = static_cast<uint32_t>(openGLValue);
		}
		else
		{
			mCapabilities.maximumNumberOf2DTextureArraySlices = 0;
		}

		// Maximum uniform buffer (UBO) size in bytes (usually at least 4096 * 16 bytes, in case there's no support for uniform buffer it's 0)
		if (mExtensions->isGL_ARB_uniform_buffer_object())
		{
			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &openGLValue);
			mCapabilities.maximumUniformBufferSize = static_cast<uint32_t>(openGLValue);
		}
		else
		{
			mCapabilities.maximumUniformBufferSize = 0;
		}

		// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
		if (mExtensions->isGL_ARB_texture_buffer_object())
		{
			glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE_EXT, &openGLValue);
			mCapabilities.maximumTextureBufferSize = static_cast<uint32_t>(openGLValue);
		}
		else
		{
			mCapabilities.maximumTextureBufferSize = 0;
		}

		// Maximum indirect buffer size in bytes
		if (mExtensions->isGL_ARB_draw_indirect())
		{
			// TODO(co) How to get the indirect buffer maximum size? Didn't find any information about this.
			mCapabilities.maximumIndirectBufferSize = 64 * 1024;	// 64 KiB
		}
		else
		{
			mCapabilities.maximumIndirectBufferSize = 64 * 1024;	// 64 KiB
		}

		// Maximum number of multisamples (always at least 1, usually 8)
		if (mExtensions->isGL_ARB_texture_multisample())
		{
			glGetIntegerv(GL_MAX_SAMPLES, &openGLValue);
			if (openGLValue > 8)
			{
				// Limit to known maximum we can test, even if e.g. GeForce 980m reports 32 here
				openGLValue = 8;
			}
			mCapabilities.maximumNumberOfMultisamples = static_cast<uint8_t>(openGLValue);
		}
		else
		{
			mCapabilities.maximumNumberOfMultisamples = 1;
		}

		// Maximum anisotropy (always at least 1, usually 16)
		// -> "GL_EXT_texture_filter_anisotropic"-extension
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &openGLValue);
		mCapabilities.maximumAnisotropy = static_cast<uint8_t>(openGLValue);

		// Coordinate system
		// -> If the "GL_ARB_clip_control"-extension is available: Left-handed coordinate system with clip space depth value range 0..1
		// -> If the "GL_ARB_clip_control"-extension isn't available: Right-handed coordinate system with clip space depth value range -1..1
		// -> For background theory see "Depth Precision Visualized" by Nathan Reed - https://developer.nvidia.com/content/depth-precision-visualized
		// -> For practical information see "Reversed-Z in OpenGL" by Nicolas Guillemot - https://nlguillemot.wordpress.com/2016/12/07/reversed-z-in-opengl/
		// -> Shaders might want to take the following into account: "Mac computers that use OpenCL and OpenGL graphics" - https://support.apple.com/en-us/HT202823 - "iMac (Retina 5K, 27-inch, 2017)" - OpenGL 4.1
		mCapabilities.upperLeftOrigin = mCapabilities.zeroToOneClipZ = mExtensions->isGL_ARB_clip_control();

		// Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		mCapabilities.individualUniforms = true;

		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex, "GL_ARB_instanced_arrays" required)
		mCapabilities.instancedArrays = mExtensions->isGL_ARB_instanced_arrays();

		// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID, "GL_ARB_draw_instanced" required)
		mCapabilities.drawInstanced = mExtensions->isGL_ARB_draw_instanced();

		// Base vertex supported for draw calls?
		mCapabilities.baseVertex = mExtensions->isGL_ARB_draw_elements_base_vertex();

		// OpenGL has no native multi-threading
		mCapabilities.nativeMultiThreading = false;

		// We don't support the OpenGL program binaries since those are operation system and graphics driver version dependent, which renders them useless for pre-compiled shaders shipping
		mCapabilities.shaderBytecode = mExtensions->isGL_ARB_gl_spirv();

		// Is there support for vertex shaders (VS)?
		mCapabilities.vertexShader = mExtensions->isGL_ARB_vertex_shader();

		// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
		if (mExtensions->isGL_ARB_tessellation_shader())
		{
			glGetIntegerv(GL_MAX_PATCH_VERTICES, &openGLValue);
			mCapabilities.maximumNumberOfPatchVertices = static_cast<uint32_t>(openGLValue);
		}
		else
		{
			mCapabilities.maximumNumberOfPatchVertices = 0;
		}

		// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
		if (mExtensions->isGL_ARB_geometry_shader4())
		{
			glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB, &openGLValue);
			mCapabilities.maximumNumberOfGsOutputVertices = static_cast<uint32_t>(openGLValue);
		}
		else
		{
			mCapabilities.maximumNumberOfGsOutputVertices = 0;
		}

		// Is there support for fragment shaders (FS)?
		mCapabilities.fragmentShader = mExtensions->isGL_ARB_fragment_shader();
	}

	void OpenGLRenderer::iaUnsetVertexArray()
	{
		// Release the currently used vertex array reference, in case we have one
		if (nullptr != mVertexArray)
		{
			// Evaluate the internal array type of the currently set vertex array
			switch (static_cast<VertexArray*>(mVertexArray)->getInternalResourceType())
			{
				case VertexArray::InternalResourceType::NO_VAO:
					// Disable OpenGL vertex attribute arrays
					static_cast<VertexArrayNoVao*>(mVertexArray)->disableOpenGLVertexAttribArrays();
					break;

				case VertexArray::InternalResourceType::VAO:
					// Unbind OpenGL vertex array
					// -> No need to check for "GL_ARB_vertex_array_object", in case were in here we know it must exist
					glBindVertexArray(0);
					break;
			}

			// Release reference
			mVertexArray->releaseReference();
			mVertexArray = nullptr;
		}
	}

	void OpenGLRenderer::setProgram(Renderer::IProgram* program)
	{
		if (nullptr != program)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLRENDERER_RENDERERMATCHCHECK_ASSERT(*this, *program)

			// Prefer "GL_ARB_separate_shader_objects" over "GL_ARB_shader_objects"
			if (mExtensions->isGL_ARB_separate_shader_objects())
			{
				// Bind the program pipeline, if required
				ProgramSeparate* programSeparate = static_cast<ProgramSeparate*>(program);
				const uint32_t openGLProgramPipeline = programSeparate->getOpenGLProgramPipeline();
				if (openGLProgramPipeline != mOpenGLProgramPipeline)
				{
					mOpenGLProgramPipeline = openGLProgramPipeline;
					{ // Draw ID uniform location for "GL_ARB_base_instance"-emulation (see "17/11/2012 Surviving without gl_DrawID" - https://www.g-truc.net/post-0518.html)
						const VertexShaderSeparate* vertexShaderSeparate = programSeparate->getVertexShaderSeparate();
						if (nullptr != vertexShaderSeparate)
						{
							mOpenGLVertexProgram = vertexShaderSeparate->getOpenGLShaderProgram();
							mDrawIdUniformLocation = vertexShaderSeparate->getDrawIdUniformLocation();
						}
						else
						{
							mOpenGLVertexProgram = 0;
							mDrawIdUniformLocation = -1;
						}
						mCurrentStartInstanceLocation = ~0u;
					}
					glBindProgramPipeline(mOpenGLProgramPipeline);
				}
			}
			else if (mExtensions->isGL_ARB_shader_objects())
			{
				// Bind the program, if required
				const ProgramMonolithic* programMonolithic = static_cast<ProgramMonolithic*>(program);
				const uint32_t openGLProgram = programMonolithic->getOpenGLProgram();
				if (openGLProgram != mOpenGLProgram)
				{
					mOpenGLProgram = mOpenGLVertexProgram = openGLProgram;
					mDrawIdUniformLocation = programMonolithic->getDrawIdUniformLocation();
					mCurrentStartInstanceLocation = ~0u;
					glUseProgramObjectARB(mOpenGLProgram);
				}
			}
		}
		else
		{
			// Prefer "GL_ARB_separate_shader_objects" over "GL_ARB_shader_objects"
			if (mExtensions->isGL_ARB_separate_shader_objects())
			{
				// Unbind the program pipeline, if required
				if (0 != mOpenGLProgramPipeline)
				{
					glBindProgramPipeline(0);
					mOpenGLProgramPipeline = 0;
				}
			}
			else if (mExtensions->isGL_ARB_shader_objects())
			{
				// Unbind the program, if required
				if (0 != mOpenGLProgram)
				{
					glUseProgramObjectARB(0);
					mOpenGLProgram = 0;
				}
			}
			mOpenGLVertexProgram = 0;
			mDrawIdUniformLocation = -1;
			mCurrentStartInstanceLocation = ~0u;
		}
	}

	void OpenGLRenderer::updateGL_ARB_base_instanceEmulation(uint32_t startInstanceLocation)
	{
		if (mDrawIdUniformLocation != -1 && 0 != mOpenGLVertexProgram && mCurrentStartInstanceLocation != startInstanceLocation)
		{
			glProgramUniform1ui(mOpenGLVertexProgram, mDrawIdUniformLocation, startInstanceLocation);
			mCurrentStartInstanceLocation = startInstanceLocation;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
