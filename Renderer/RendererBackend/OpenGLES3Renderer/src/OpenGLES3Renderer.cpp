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
#include "OpenGLES3Renderer/OpenGLES3Renderer.h"
#include "OpenGLES3Renderer/OpenGLES3Debug.h"	// For "OPENGLES3RENDERER_RENDERERMATCHCHECK_ASSERT()"
#include "OpenGLES3Renderer/Mapping.h"
#include "OpenGLES3Renderer/IExtensions.h"
#include "OpenGLES3Renderer/RootSignature.h"
#include "OpenGLES3Renderer/ResourceGroup.h"
#include "OpenGLES3Renderer/OpenGLES3ContextRuntimeLinking.h"
#include "OpenGLES3Renderer/RenderTarget/SwapChain.h"
#include "OpenGLES3Renderer/RenderTarget/RenderPass.h"
#include "OpenGLES3Renderer/RenderTarget/Framebuffer.h"
#include "OpenGLES3Renderer/Buffer/BufferManager.h"
#include "OpenGLES3Renderer/Buffer/IndexBuffer.h"
#include "OpenGLES3Renderer/Buffer/TextureBufferBind.h"
#include "OpenGLES3Renderer/Buffer/UniformBuffer.h"
#include "OpenGLES3Renderer/Buffer/VertexBuffer.h"
#include "OpenGLES3Renderer/Buffer/VertexArray.h"
#include "OpenGLES3Renderer/Buffer/IndirectBuffer.h"
#include "OpenGLES3Renderer/Texture/TextureManager.h"
#include "OpenGLES3Renderer/Texture/Texture1D.h"
#include "OpenGLES3Renderer/Texture/Texture2D.h"
#include "OpenGLES3Renderer/Texture/Texture3D.h"
#include "OpenGLES3Renderer/Texture/TextureCube.h"
#include "OpenGLES3Renderer/Texture/Texture2DArray.h"
#include "OpenGLES3Renderer/State/SamplerState.h"
#include "OpenGLES3Renderer/State/PipelineState.h"
#include "OpenGLES3Renderer/Shader/ProgramGlsl.h"
#include "OpenGLES3Renderer/Shader/ShaderLanguageGlsl.h"
#include "OpenGLES3Renderer/OpenGLES3ContextRuntimeLinking.h"

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>
#include <Renderer/IAllocator.h>
#include <Renderer/Buffer/CommandBuffer.h>
#include <Renderer/Buffer/IndirectBufferTypes.h>

#include <GLES3/gl2ext.h>


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef OPENGLES3RENDERER_EXPORTS
	#define OPENGLES3RENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define OPENGLES3RENDERER_API_EXPORT
#endif
OPENGLES3RENDERER_API_EXPORT Renderer::IRenderer* createOpenGLES3RendererInstance(const Renderer::Context& context)
{
	return RENDERER_NEW(context, OpenGLES3Renderer::OpenGLES3Renderer)(context);
}
#undef OPENGLES3RENDERER_API_EXPORT


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

		bool mapBuffer(const Renderer::Context& context, GLenum target, GLenum bindingTarget, GLuint openGLES3Buffer, uint32_t bufferSize, Renderer::MapType mapType, Renderer::MappedSubresource& mappedSubresource)
		{
			// TODO(co) This buffer update isn't efficient, use e.g. persistent buffer mapping

			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Backup the currently bound OpenGL ES 3 buffer
				GLint openGLES3BufferBackup = 0;
				OpenGLES3Renderer::glGetIntegerv(bindingTarget, &openGLES3BufferBackup);
			#endif

			// Bind this OpenGL ES 3 buffer
			OpenGLES3Renderer::glBindBuffer(target, openGLES3Buffer);

			// Map
			mappedSubresource.data		 = OpenGLES3Renderer::glMapBufferRange(target, 0, static_cast<GLsizeiptr>(bufferSize), OpenGLES3Renderer::Mapping::getOpenGLES3MapRangeType(mapType));
			mappedSubresource.rowPitch   = 0;
			mappedSubresource.depthPitch = 0;

			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous bound OpenGL ES 3 buffer
				OpenGLES3Renderer::glBindBuffer(target, static_cast<GLuint>(openGLES3BufferBackup));
			#endif

			// Done
			RENDERER_ASSERT(context, nullptr != mappedSubresource.data, "Mapping of OpenGL ES 3 buffer failed")
			return (nullptr != mappedSubresource.data);
		}

		void unmapBuffer(GLenum target, GLenum bindingTarget, GLuint openGLES3Buffer)
		{
			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Backup the currently bound OpenGL ES 3 buffer
				GLint openGLES3BufferBackup = 0;
				OpenGLES3Renderer::glGetIntegerv(bindingTarget, &openGLES3BufferBackup);
			#endif

			// Bind this OpenGL ES 3 buffer
			OpenGLES3Renderer::glBindBuffer(target, openGLES3Buffer);

			// Unmap
			OpenGLES3Renderer::glUnmapBuffer(target);

			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous bound OpenGL ES 3 buffer
				OpenGLES3Renderer::glBindBuffer(target, static_cast<GLuint>(openGLES3BufferBackup));
			#endif
		}

		namespace BackendDispatch
		{


			//[-------------------------------------------------------]
			//[ Command buffer                                        ]
			//[-------------------------------------------------------]
			void ExecuteCommandBuffer(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::ExecuteCommandBuffer* realData = static_cast<const Renderer::Command::ExecuteCommandBuffer*>(data);
				RENDERER_ASSERT(renderer.getContext(), nullptr != realData->commandBufferToExecute, "The OpenGL ES 3 command buffer to execute must be valid")
				renderer.submitCommandBuffer(*realData->commandBufferToExecute);
			}


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
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).setGraphicsRootSignature(realData->rootSignature);
			}

			void SetGraphicsResourceGroup(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetGraphicsResourceGroup* realData = static_cast<const Renderer::Command::SetGraphicsResourceGroup*>(data);
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).setGraphicsResourceGroup(realData->rootParameterIndex, realData->resourceGroup);
			}

			//[-------------------------------------------------------]
			//[ States                                                ]
			//[-------------------------------------------------------]
			void SetPipelineState(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetPipelineState* realData = static_cast<const Renderer::Command::SetPipelineState*>(data);
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).setPipelineState(realData->pipelineState);
			}

			//[-------------------------------------------------------]
			//[ Input-assembler (IA) stage                            ]
			//[-------------------------------------------------------]
			void SetVertexArray(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetVertexArray* realData = static_cast<const Renderer::Command::SetVertexArray*>(data);
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).iaSetVertexArray(realData->vertexArray);
			}

			//[-------------------------------------------------------]
			//[ Rasterizer (RS) stage                                 ]
			//[-------------------------------------------------------]
			void SetViewports(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetViewports* realData = static_cast<const Renderer::Command::SetViewports*>(data);
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).rsSetViewports(realData->numberOfViewports, (nullptr != realData->viewports) ? realData->viewports : reinterpret_cast<const Renderer::Viewport*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
			}

			void SetScissorRectangles(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetScissorRectangles* realData = static_cast<const Renderer::Command::SetScissorRectangles*>(data);
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).rsSetScissorRectangles(realData->numberOfScissorRectangles, (nullptr != realData->scissorRectangles) ? realData->scissorRectangles : reinterpret_cast<const Renderer::ScissorRectangle*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
			}

			//[-------------------------------------------------------]
			//[ Output-merger (OM) stage                              ]
			//[-------------------------------------------------------]
			void SetRenderTarget(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetRenderTarget* realData = static_cast<const Renderer::Command::SetRenderTarget*>(data);
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).omSetRenderTarget(realData->renderTarget);
			}

			//[-------------------------------------------------------]
			//[ Operations                                            ]
			//[-------------------------------------------------------]
			void Clear(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Clear* realData = static_cast<const Renderer::Command::Clear*>(data);
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).clear(realData->flags, realData->color, realData->z, realData->stencil);
			}

			void ResolveMultisampleFramebuffer(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::ResolveMultisampleFramebuffer* realData = static_cast<const Renderer::Command::ResolveMultisampleFramebuffer*>(data);
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).resolveMultisampleFramebuffer(*realData->destinationRenderTarget, *realData->sourceMultisampleFramebuffer);
			}

			void CopyResource(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::CopyResource* realData = static_cast<const Renderer::Command::CopyResource*>(data);
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).copyResource(*realData->destinationResource, *realData->sourceResource);
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
					static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).drawEmulated(realData->indirectBuffer->getEmulationData(), realData->indirectBufferOffset, realData->numberOfDraws);
				}
				else
				{
					static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).drawEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
				}
			}

			void DrawIndexed(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Draw* realData = static_cast<const Renderer::Command::Draw*>(data);
				if (nullptr != realData->indirectBuffer)
				{
					// No resource owner security check in here, we only support emulated indirect buffer
					static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).drawIndexedEmulated(realData->indirectBuffer->getEmulationData(), realData->indirectBufferOffset, realData->numberOfDraws);
				}
				else
				{
					static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).drawIndexedEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
				}
			}

			//[-------------------------------------------------------]
			//[ Debug                                                 ]
			//[-------------------------------------------------------]
			void SetDebugMarker(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetDebugMarker* realData = static_cast<const Renderer::Command::SetDebugMarker*>(data);
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).setDebugMarker(realData->name);
			}

			void BeginDebugEvent(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::BeginDebugEvent* realData = static_cast<const Renderer::Command::BeginDebugEvent*>(data);
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).beginDebugEvent(realData->name);
			}

			void EndDebugEvent(const void*, Renderer::IRenderer& renderer)
			{
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).endDebugEvent();
			}


		}


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		static const Renderer::BackendDispatchFunction DISPATCH_FUNCTIONS[Renderer::CommandDispatchFunctionIndex::NumberOfFunctions] =
		{
			// Command buffer
			&BackendDispatch::ExecuteCommandBuffer,
			// Resource handling
			&BackendDispatch::CopyUniformBufferData,
			&BackendDispatch::CopyTextureBufferData,
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
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	OpenGLES3Renderer::OpenGLES3Renderer(const Renderer::Context& context) :
		IRenderer(context),
		mOpenGLES3Context(nullptr),
		mShaderLanguageGlsl(nullptr),
		mGraphicsRootSignature(nullptr),
		mDefaultSamplerState(nullptr),
		mOpenGLES3CopyResourceFramebuffer(0),
		// States
		mPipelineState(nullptr),
		// Input-assembler (IA) stage
		mVertexArray(nullptr),
		mOpenGLES3PrimitiveTopology(0xFFFF),	// Unknown default setting
		// Output-merger (OM) stage
		mRenderTarget(nullptr),
		// State cache to avoid making redundant OpenGL ES 3 calls
		mOpenGLES3Program(0),
		// Draw ID uniform location for "GL_EXT_base_instance"-emulation (see "17/11/2012 Surviving without gl_DrawID" - https://www.g-truc.net/post-0518.html)
		mDrawIdUniformLocation(-1),
		mCurrentStartInstanceLocation(~0u)
	{
		// Initialize the OpenGL ES 3 context
		mOpenGLES3Context = RENDERER_NEW(mContext, OpenGLES3ContextRuntimeLinking)(*this, context.getNativeWindowHandle(), context.isUsingExternalContext());
		if (mOpenGLES3Context->initialize(0))
		{
			// Initialize the capabilities
			initializeCapabilities();

			// Create the default sampler state
			mDefaultSamplerState = createSamplerState(Renderer::ISamplerState::getDefaultSamplerState());

			#ifdef RENDERER_DEBUG
				// "GL_KHR_debug"-extension available?
				if (mOpenGLES3Context->getExtensions().isGL_KHR_debug())
				{
					// Synchronous debug output, please
					// -> Makes it easier to find the place causing the issue
					glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);

					// We don't need to configure the debug output by using "glDebugMessageControlARB()",
					// by default all messages are enabled and this is good this way

					// Set the debug message callback function
					glDebugMessageCallbackKHR(&OpenGLES3Renderer::debugMessageCallback, this);
				}
			#endif

			// Add references to the default sampler state and set it
			if (nullptr != mDefaultSamplerState)
			{
				mDefaultSamplerState->addReference();
				// TODO(co) Set default sampler states
			}
		}
	}

	OpenGLES3Renderer::~OpenGLES3Renderer()
	{
		// Set no pipeline state reference, in case we have one
		if (nullptr != mPipelineState)
		{
			setPipelineState(nullptr);
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

		// Destroy the OpenGL ES 3 framebuffer used by "OpenGLES3Renderer::OpenGLES3Renderer::copyResource()"
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		// -> Null test in here only to handle the situation of OpenGL ES 3 initialization failure, meaning "glDeleteFramebuffers" itself is a null pointer
		if (0 != mOpenGLES3CopyResourceFramebuffer)
		{
			glDeleteFramebuffers(1, &mOpenGLES3CopyResourceFramebuffer);
		}

		// Set no vertex array reference, in case we have one
		if (nullptr != mVertexArray)
		{
			iaSetVertexArray(nullptr);
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
					RENDERER_LOG(mContext, CRITICAL, "The OpenGL ES 3 renderer backend is going to be destroyed, but there are still %lu resource instances left (memory leak)", numberOfCurrentResources)
				}
				else
				{
					RENDERER_LOG(mContext, CRITICAL, "The OpenGL ES 3 renderer backend is going to be destroyed, but there is still one resource instance left (memory leak)")
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

		// Destroy the OpenGL ES 3 context instance
		RENDERER_DELETE(mContext, IOpenGLES3Context, mOpenGLES3Context);
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void OpenGLES3Renderer::setGraphicsRootSignature(Renderer::IRootSignature* rootSignature)
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
			OPENGLES3RENDERER_RENDERERMATCHCHECK_ASSERT(*this, *rootSignature)
		}
	}

	void OpenGLES3Renderer::setGraphicsResourceGroup(uint32_t rootParameterIndex, Renderer::IResourceGroup* resourceGroup)
	{
		// Security checks
		#ifdef RENDERER_DEBUG
		{
			if (nullptr == mGraphicsRootSignature)
			{
				RENDERER_LOG(mContext, CRITICAL, "No OpenGL ES 3 renderer backend graphics root signature set")
				return;
			}
			const Renderer::RootSignature& rootSignature = mGraphicsRootSignature->getRootSignature();
			if (rootParameterIndex >= rootSignature.numberOfParameters)
			{
				RENDERER_LOG(mContext, CRITICAL, "The OpenGL ES 3 renderer backend root parameter index is out of bounds")
				return;
			}
			const Renderer::RootParameter& rootParameter = rootSignature.parameters[rootParameterIndex];
			if (Renderer::RootParameterType::DESCRIPTOR_TABLE != rootParameter.parameterType)
			{
				RENDERER_LOG(mContext, CRITICAL, "The OpenGL ES 3 renderer backend root parameter index doesn't reference a descriptor table")
				return;
			}
			if (nullptr == reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges))
			{
				RENDERER_LOG(mContext, CRITICAL, "The OpenGL ES 3 renderer backend descriptor ranges is a null pointer")
				return;
			}
		}
		#endif

		if (nullptr != resourceGroup)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES3RENDERER_RENDERERMATCHCHECK_ASSERT(*this, *resourceGroup)

			// Set graphics resource group
			const ResourceGroup* openGLES3ResourceGroup = static_cast<ResourceGroup*>(resourceGroup);
			const uint32_t numberOfResources = openGLES3ResourceGroup->getNumberOfResources();
			Renderer::IResource** resources = openGLES3ResourceGroup->getResources();
			const Renderer::RootParameter& rootParameter = mGraphicsRootSignature->getRootSignature().parameters[rootParameterIndex];
			for (uint32_t resourceIndex = 0; resourceIndex < numberOfResources; ++resourceIndex, ++resources)
			{
				Renderer::IResource* resource = *resources;
				RENDERER_ASSERT(mContext, nullptr != reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges), "Invalid OpenGL ES 3 descriptor ranges")
				const Renderer::DescriptorRange& descriptorRange = reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges)[resourceIndex];

				// Check the type of resource to set
				// TODO(co) Some additional resource type root signature security checks in debug build?
				const Renderer::ResourceType resourceType = resource->getResourceType();
				switch (resourceType)
				{
					case Renderer::ResourceType::UNIFORM_BUFFER:
						// Attach the buffer to the given UBO binding point
						// -> Explicit binding points ("layout(binding = 0)" in GLSL shader) requires OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension
						// -> Direct3D 10 and Direct3D 11 have explicit binding points
						RENDERER_ASSERT(mContext, nullptr != openGLES3ResourceGroup->getResourceIndexToUniformBlockBindingIndex(), "Invalid OpenGL ES 3 resource index to uniform block binding index")
						glBindBufferBase(GL_UNIFORM_BUFFER, openGLES3ResourceGroup->getResourceIndexToUniformBlockBindingIndex()[resourceIndex], static_cast<UniformBuffer*>(resource)->getOpenGLES3UniformBuffer());
						break;

					case Renderer::ResourceType::TEXTURE_BUFFER:
						if (mOpenGLES3Context->getExtensions().isGL_EXT_texture_buffer())
						{
							// Fall through by intent
						}
						else
						{
							// We can only emulate the "Renderer::TextureFormat::R32G32B32A32F" texture format using an uniform buffer

							// Attach the buffer to the given UBO binding point
							// -> Explicit binding points ("layout(binding = 0)" in GLSL shader) requires OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension
							// -> Direct3D 10 and Direct3D 11 have explicit binding points
							RENDERER_ASSERT(mContext, nullptr != openGLES3ResourceGroup->getResourceIndexToUniformBlockBindingIndex(), "Invalid OpenGL ES 3 resource index to uniform block binding index")
							glBindBufferBase(GL_UNIFORM_BUFFER, openGLES3ResourceGroup->getResourceIndexToUniformBlockBindingIndex()[resourceIndex], static_cast<TextureBuffer*>(resource)->getOpenGLES3TextureBuffer());
							break;
						}

					case Renderer::ResourceType::TEXTURE_1D:
					case Renderer::ResourceType::TEXTURE_2D:
					case Renderer::ResourceType::TEXTURE_2D_ARRAY:
					case Renderer::ResourceType::TEXTURE_3D:
					case Renderer::ResourceType::TEXTURE_CUBE:
					{
						switch (descriptorRange.shaderVisibility)
						{
							// In OpenGL ES 3, all shaders share the same texture units
							case Renderer::ShaderVisibility::ALL:
							case Renderer::ShaderVisibility::VERTEX:
							case Renderer::ShaderVisibility::FRAGMENT:
							{
								#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
									// Backup the currently active OpenGL ES 3 texture
									GLint openGLES3ActiveTextureBackup = 0;
									glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLES3ActiveTextureBackup);
								#endif

								// TODO(co) Some security checks might be wise *maximum number of texture units*
								glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + descriptorRange.baseShaderRegister));

								// Bind texture or texture buffer
								if (resourceType == Renderer::ResourceType::TEXTURE_BUFFER)
								{
									glBindTexture(GL_TEXTURE_BUFFER_EXT, static_cast<TextureBuffer*>(resource)->getOpenGLES3Texture());
								}
								else if (resourceType == Renderer::ResourceType::TEXTURE_1D)
								{
									// OpenGL ES 3 has no 1D textures, just use a 2D texture with a height of one
									glBindTexture(GL_TEXTURE_2D, static_cast<Texture1D*>(resource)->getOpenGLES3Texture());
								}
								else if (resourceType == Renderer::ResourceType::TEXTURE_2D_ARRAY)
								{
									// No extension check required, if we in here we already know it must exist
									glBindTexture(GL_TEXTURE_2D_ARRAY, static_cast<Texture2DArray*>(resource)->getOpenGLES3Texture());
								}
								else if (resourceType == Renderer::ResourceType::TEXTURE_3D)
								{
									// No extension check required, if we in here we already know it must exist
									glBindTexture(GL_TEXTURE_3D, static_cast<Texture3D*>(resource)->getOpenGLES3Texture());
								}
								else if (resourceType == Renderer::ResourceType::TEXTURE_CUBE)
								{
									// No extension check required, if we in here we already know it must exist
									glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<TextureCube*>(resource)->getOpenGLES3Texture());
								}
								else
								{
									glBindTexture(GL_TEXTURE_2D, static_cast<Texture2D*>(resource)->getOpenGLES3Texture());
								}

								// Set the OpenGL ES 3 sampler states, if required (texture buffer has no sampler state), it's valid that there's no sampler state (e.g. texel fetch instead of sampling might be used)
								if (Renderer::ResourceType::TEXTURE_BUFFER != resourceType)
								{
									RENDERER_ASSERT(mContext, nullptr != openGLES3ResourceGroup->getSamplerState(), "Invalid OpenGL ES 3 sampler state")
									const SamplerState* samplerState = static_cast<const SamplerState*>(openGLES3ResourceGroup->getSamplerState()[resourceIndex]);
									if (nullptr != samplerState)
									{
										// Traditional bind version to emulate a sampler object
										samplerState->setOpenGLES3SamplerStates();
									}
								}

								#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
									// Be polite and restore the previous active OpenGL ES 3 texture
									glActiveTexture(static_cast<GLuint>(openGLES3ActiveTextureBackup));
								#endif
								break;
							}

							case Renderer::ShaderVisibility::TESSELLATION_CONTROL:
								RENDERER_LOG(mContext, CRITICAL, "OpenGL ES 3 has no tessellation control shader support (hull shader in Direct3D terminology)")
								break;

							case Renderer::ShaderVisibility::TESSELLATION_EVALUATION:
								RENDERER_LOG(mContext, CRITICAL, "OpenGL ES 3 has no tessellation evaluation shader support (domain shader in Direct3D terminology)")
								break;

							case Renderer::ShaderVisibility::GEOMETRY:
								RENDERER_LOG(mContext, CRITICAL, "OpenGL ES 3 has no geometry shader support")
								break;
						}
						break;
					}

					case Renderer::ResourceType::SAMPLER_STATE:
						// Unlike Direct3D >=10, OpenGL ES 3 directly attaches the sampler settings to the texture
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

	void OpenGLES3Renderer::setPipelineState(Renderer::IPipelineState* pipelineState)
	{
		if (mPipelineState != pipelineState)
		{
			if (nullptr != pipelineState)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLES3RENDERER_RENDERERMATCHCHECK_ASSERT(*this, *pipelineState)

				// Set new pipeline state and add a reference to it
				if (nullptr != mPipelineState)
				{
					mPipelineState->releaseReference();
				}
				mPipelineState = static_cast<PipelineState*>(pipelineState);
				mPipelineState->addReference();

				// Set pipeline state
				mOpenGLES3PrimitiveTopology = mPipelineState->getOpenGLES3PrimitiveTopology();
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
	void OpenGLES3Renderer::iaSetVertexArray(Renderer::IVertexArray* vertexArray)
	{
		// New vertex array?
		if (mVertexArray != vertexArray)
		{
			// Set a vertex array?
			if (nullptr != vertexArray)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLES3RENDERER_RENDERERMATCHCHECK_ASSERT(*this, *vertexArray)

				// Release the vertex array reference, in case we have one
				if (nullptr != mVertexArray)
				{
					// Release reference
					mVertexArray->releaseReference();
				}

				// Set new vertex array and add a reference to it
				mVertexArray = static_cast<VertexArray*>(vertexArray);
				mVertexArray->addReference();

				// Bind OpenGL ES 3 vertex array
				glBindVertexArray(static_cast<VertexArray*>(mVertexArray)->getOpenGLES3VertexArray());
			}

			// Release the vertex array reference, in case we have one
			else if (nullptr != mVertexArray)
			{
				// Unbind OpenGL ES 3 vertex array
				glBindVertexArray(0);

				// Release reference
				mVertexArray->releaseReference();
				mVertexArray = nullptr;
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Rasterizer (RS) stage                                 ]
	//[-------------------------------------------------------]
	void OpenGLES3Renderer::rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport* viewports)
	{
		// Sanity check
		RENDERER_ASSERT(mContext, numberOfViewports > 0 && nullptr != viewports, "Invalid OpenGL ES 3 rasterizer state viewports")
		std::ignore = numberOfViewports;

		// In OpenGL ES 3, the origin of the viewport is left bottom while Direct3D is using a left top origin. To make the
		// Direct3D 11 implementation as efficient as possible the Direct3D convention is used and we have to convert in here.

		// Get the width and height of the current render target
		uint32_t renderTargetHeight = 1;
		if (nullptr != mRenderTarget)
		{
			uint32_t renderTargetWidth = 1;
			mRenderTarget->getWidthAndHeight(renderTargetWidth, renderTargetHeight);
		}

		// Set the OpenGL ES 3 viewport
		// -> OpenGL ES 3 supports only one viewport
		RENDERER_ASSERT(mContext, numberOfViewports <= 1, "OpenGL ES 3 supports only one viewport")
		glViewport(static_cast<GLint>(viewports->topLeftX), static_cast<GLint>(renderTargetHeight - viewports->topLeftY - viewports->height), static_cast<GLsizei>(viewports->width), static_cast<GLsizei>(viewports->height));
		glDepthRangef(static_cast<GLclampf>(viewports->minDepth), static_cast<GLclampf>(viewports->maxDepth));
	}

	void OpenGLES3Renderer::rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle* scissorRectangles)
	{
		// Sanity check
		RENDERER_ASSERT(mContext, numberOfScissorRectangles > 0 && nullptr != scissorRectangles, "Invalid OpenGL ES 3 rasterizer state scissor rectangles")
		std::ignore = numberOfScissorRectangles;

		// In OpenGL ES 3, the origin of the scissor rectangle is left bottom while Direct3D is using a left top origin. To make the
		// Direct3D 9 & 10 & 11 implementation as efficient as possible the Direct3D convention is used and we have to convert in here.

		// Get the width and height of the current render target
		uint32_t renderTargetHeight = 1;
		if (nullptr != mRenderTarget)
		{
			uint32_t renderTargetWidth = 1;
			mRenderTarget->getWidthAndHeight(renderTargetWidth, renderTargetHeight);
		}

		// Set the OpenGL ES 3 scissor rectangle
		RENDERER_ASSERT(mContext, numberOfScissorRectangles <= 1, "OpenGL ES 3 supports only one scissor rectangle")
		const GLsizei width  = scissorRectangles->bottomRightX - scissorRectangles->topLeftX;
		const GLsizei height = scissorRectangles->bottomRightY - scissorRectangles->topLeftY;
		glScissor(static_cast<GLint>(scissorRectangles->topLeftX), static_cast<GLint>(renderTargetHeight - scissorRectangles->topLeftY - height), width, height);
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	void OpenGLES3Renderer::omSetRenderTarget(Renderer::IRenderTarget* renderTarget)
	{
		// New render target?
		if (mRenderTarget != renderTarget)
		{
			// Set a render target?
			if (nullptr != renderTarget)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLES3RENDERER_RENDERERMATCHCHECK_ASSERT(*this, *renderTarget)

				// Release the render target reference, in case we have one
				Framebuffer* framebufferToGenerateMipmapsFor = nullptr;
				if (nullptr != mRenderTarget)
				{
					// Unbind OpenGL ES 3 framebuffer?
					if (Renderer::ResourceType::FRAMEBUFFER == mRenderTarget->getResourceType() && Renderer::ResourceType::FRAMEBUFFER != renderTarget->getResourceType())
					{
						// We do not render into a OpenGL ES 3 framebuffer
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
				switch (mRenderTarget->getResourceType())
				{
					case Renderer::ResourceType::SWAP_CHAIN:
					{
						// TODO(co) Implement me
						break;
					}

					case Renderer::ResourceType::FRAMEBUFFER:
					{
						// Get the OpenGL ES 3 framebuffer instance
						Framebuffer* framebuffer = static_cast<Framebuffer*>(mRenderTarget);

						// Bind the OpenGL ES 3 framebuffer
						glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->getOpenGLES3Framebuffer());

						// Define the OpenGL buffers to draw into
						{
							// https://www.opengl.org/registry/specs/ARB/draw_buffers.txt - "The draw buffer for output colors beyond <n> is set to NONE."
							// -> Meaning depth only rendering which has no color textures at all will work as well, no need for "glDrawBuffer(GL_NONE)"
							// -> https://www.khronos.org/opengles/sdk/docs/man3/html/glDrawBuffers.xhtml for OpenGL ES 3 specifies the same behaviour
							// -> "GL_COLOR_ATTACHMENT0" and "GL_COLOR_ATTACHMENT0_NV" have the same value
							static const GLenum OPENGL_DRAW_BUFFER[16] =
							{
								GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1,  GL_COLOR_ATTACHMENT2,  GL_COLOR_ATTACHMENT3,
								GL_COLOR_ATTACHMENT4,  GL_COLOR_ATTACHMENT5,  GL_COLOR_ATTACHMENT6,  GL_COLOR_ATTACHMENT7,
								GL_COLOR_ATTACHMENT8,  GL_COLOR_ATTACHMENT9,  GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
								GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15
							};
							glDrawBuffers(static_cast<GLsizei>(framebuffer->getNumberOfColorTextures()), OPENGL_DRAW_BUFFER);
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
			}
			else if (nullptr != mRenderTarget)
			{
				// Evaluate the render target type
				if (Renderer::ResourceType::FRAMEBUFFER == mRenderTarget->getResourceType())
				{
					// We do not render into a OpenGL ES 3 framebuffer
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
	void OpenGLES3Renderer::clear(uint32_t flags, const float color[4], float z, uint32_t stencil)
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
				glClearDepthf(z);
				if (nullptr != mPipelineState && Renderer::DepthWriteMask::ALL != mPipelineState->getDepthStencilState().depthWriteMask)
				{
					glDepthMask(GL_TRUE);
				}
			}
			if (flags & Renderer::ClearFlag::STENCIL)
			{
				glClearStencil(static_cast<GLint>(stencil));
			}

			// Unlike OpenGL ES 3, when using Direct3D 10 & 11 the scissor rectangle(s) do not affect the clear operation
			// -> We have to compensate the OpenGL ES 3 behaviour in here

			// Disable OpenGL ES 3 scissor test, in case it's not disabled, yet
			if (nullptr != mPipelineState && mPipelineState->getRasterizerState().scissorEnable)
			{
				glDisable(GL_SCISSOR_TEST);
			}

			// Clear
			glClear(flagsApi);

			// Restore the previously set OpenGL ES 3 states
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

	void OpenGLES3Renderer::resolveMultisampleFramebuffer(Renderer::IRenderTarget&, Renderer::IFramebuffer&)
	{
		// TODO(co) Implement me
	}

	void OpenGLES3Renderer::copyResource(Renderer::IResource& destinationResource, Renderer::IResource& sourceResource)
	{
		// Security check: Are the given resources owned by this renderer? (calls "return" in case of a mismatch)
		OPENGLES3RENDERER_RENDERERMATCHCHECK_ASSERT(*this, destinationResource)
		OPENGLES3RENDERER_RENDERERMATCHCHECK_ASSERT(*this, sourceResource)

		// Evaluate the render target type
		switch (destinationResource.getResourceType())
		{
			case Renderer::ResourceType::TEXTURE_2D:
				if (sourceResource.getResourceType() == Renderer::ResourceType::TEXTURE_2D)
				{
					// Get the OpenGL ES 3 texture 2D instances
					const Texture2D& openGlEs3DestinationTexture2D = static_cast<const Texture2D&>(destinationResource);
					const Texture2D& openGlEs3SourceTexture2D = static_cast<const Texture2D&>(sourceResource);
					RENDERER_ASSERT(mContext, openGlEs3DestinationTexture2D.getWidth() == openGlEs3SourceTexture2D.getWidth(), "OpenGL source and destination width must be identical for resource copy")
					RENDERER_ASSERT(mContext, openGlEs3DestinationTexture2D.getHeight() == openGlEs3SourceTexture2D.getHeight(), "OpenGL source and destination height must be identical for resource copy")

					#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
						// Backup the currently bound OpenGL ES 3 framebuffer
						GLint openGLES3FramebufferBackup = 0;
						glGetIntegerv(GL_FRAMEBUFFER_BINDING, &openGLES3FramebufferBackup);
					#endif

					// Copy resource by using a framebuffer
					const GLint width = static_cast<GLint>(openGlEs3DestinationTexture2D.getWidth());
					const GLint height = static_cast<GLint>(openGlEs3DestinationTexture2D.getHeight());
					if (0 == mOpenGLES3CopyResourceFramebuffer)
					{
						glGenFramebuffers(1, &mOpenGLES3CopyResourceFramebuffer);
					}
					glBindFramebuffer(GL_FRAMEBUFFER, mOpenGLES3CopyResourceFramebuffer);
					glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGlEs3SourceTexture2D.getOpenGLES3Texture(), 0);
					glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, openGlEs3DestinationTexture2D.getOpenGLES3Texture(), 0);
					static const GLenum OPENGL_DRAW_BUFFER[1] =
					{
						GL_COLOR_ATTACHMENT1
					};
					glDrawBuffers(1, OPENGL_DRAW_BUFFER);
					glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

					#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
						// Be polite and restore the previous bound OpenGL ES 3 framebuffer
						glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(openGLES3FramebufferBackup));
					#endif
				}
				else
				{
					// Error!
					RENDERER_ASSERT(mContext, false, "Failed to copy OpenGL ES 3 resource")
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
	void OpenGLES3Renderer::drawEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Sanity checks
		RENDERER_ASSERT(mContext, nullptr != emulationData, "The OpenGL ES 3 emulation data must be valid")
		RENDERER_ASSERT(mContext, numberOfDraws > 0, "The number of OpenGL ES 3 draws must not be zero")
		// It's possible to draw without "mVertexArray"

		// TODO(co) Currently no buffer overflow check due to lack of interface provided data
		emulationData += indirectBufferOffset;

		// Emit the draw calls
		for (uint32_t i = 0; i < numberOfDraws; ++i)
		{
			const Renderer::DrawInstancedArguments& drawInstancedArguments = *reinterpret_cast<const Renderer::DrawInstancedArguments*>(emulationData);
			updateGL_EXT_base_instanceEmulation(drawInstancedArguments.startInstanceLocation);

			// Draw and advance
			if (drawInstancedArguments.instanceCount > 1 || (drawInstancedArguments.startInstanceLocation > 0 && mOpenGLES3Context->getExtensions().isGL_EXT_base_instance()))
			{
				// With instancing
				if (drawInstancedArguments.startInstanceLocation > 0 && mOpenGLES3Context->getExtensions().isGL_EXT_base_instance())
				{
					glDrawArraysInstancedBaseInstanceEXT(mOpenGLES3PrimitiveTopology, static_cast<GLint>(drawInstancedArguments.startVertexLocation), static_cast<GLsizei>(drawInstancedArguments.vertexCountPerInstance), static_cast<GLsizei>(drawInstancedArguments.instanceCount), drawInstancedArguments.startInstanceLocation);
				}
				else
				{
					glDrawArraysInstanced(mOpenGLES3PrimitiveTopology, static_cast<GLint>(drawInstancedArguments.startVertexLocation), static_cast<GLsizei>(drawInstancedArguments.vertexCountPerInstance), static_cast<GLsizei>(drawInstancedArguments.instanceCount));
				}
			}
			else
			{
				// Without instancing
				RENDERER_ASSERT(mContext, drawInstancedArguments.instanceCount <= 1, "Invalid OpenGL ES 3 instance count")
				glDrawArrays(mOpenGLES3PrimitiveTopology, static_cast<GLint>(drawInstancedArguments.startVertexLocation), static_cast<GLsizei>(drawInstancedArguments.vertexCountPerInstance));
			}
			emulationData += sizeof(Renderer::DrawInstancedArguments);
		}
	}

	void OpenGLES3Renderer::drawIndexedEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Sanity checks
		RENDERER_ASSERT(mContext, nullptr != emulationData, "The OpenGL ES 3 emulation data must be valid")
		RENDERER_ASSERT(mContext, numberOfDraws > 0, "The number of OpenGL ES 3 draws must not be zero")
		RENDERER_ASSERT(mContext, nullptr != mVertexArray, "Draw OpenGL ES 3 indexed needs a set vertex array")
		RENDERER_ASSERT(mContext, nullptr != mVertexArray->getIndexBuffer(), "Draw OpenGL ES 3 indexed needs a set vertex array which contains an index buffer")

		// TODO(co) Currently no buffer overflow check due to lack of interface provided data
		emulationData += indirectBufferOffset;

		// Emit the draw calls
		IndexBuffer* indexBuffer = mVertexArray->getIndexBuffer();
		for (uint32_t i = 0; i < numberOfDraws; ++i)
		{
			const Renderer::DrawIndexedInstancedArguments& drawIndexedInstancedArguments = *reinterpret_cast<const Renderer::DrawIndexedInstancedArguments*>(emulationData);
			updateGL_EXT_base_instanceEmulation(drawIndexedInstancedArguments.startInstanceLocation);

			// Draw and advance
			if (drawIndexedInstancedArguments.instanceCount > 1 || (drawIndexedInstancedArguments.startInstanceLocation > 0 && mOpenGLES3Context->getExtensions().isGL_EXT_base_instance()))
			{
				// With instancing
				if (drawIndexedInstancedArguments.baseVertexLocation > 0)
				{
					// Use start instance location?
					if (drawIndexedInstancedArguments.startInstanceLocation > 0 && mOpenGLES3Context->getExtensions().isGL_EXT_base_instance())
					{
						// Draw with base vertex location and start instance location
						glDrawElementsInstancedBaseVertexBaseInstanceEXT(mOpenGLES3PrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLES3Type(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())), static_cast<GLsizei>(drawIndexedInstancedArguments.instanceCount), static_cast<GLint>(drawIndexedInstancedArguments.baseVertexLocation), drawIndexedInstancedArguments.startInstanceLocation);
					}

					// Is the "GL_EXT_draw_elements_base_vertex" extension there?
					else if (mOpenGLES3Context->getExtensions().isGL_EXT_draw_elements_base_vertex())
					{
						// Draw with base vertex location
						glDrawElementsInstancedBaseVertexEXT(mOpenGLES3PrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLES3Type(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())), static_cast<GLsizei>(drawIndexedInstancedArguments.instanceCount), static_cast<GLint>(drawIndexedInstancedArguments.baseVertexLocation));
					}
					else
					{
						// Error!
						RENDERER_ASSERT(mContext, false, "Failed to OpenGL ES 3 draw indexed emulated")
					}
				}
				else if (drawIndexedInstancedArguments.startInstanceLocation > 0 && mOpenGLES3Context->getExtensions().isGL_EXT_base_instance())
				{
					// Draw without base vertex location and with start instance location
					glDrawElementsInstancedBaseInstanceEXT(mOpenGLES3PrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLES3Type(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())), static_cast<GLsizei>(drawIndexedInstancedArguments.instanceCount), drawIndexedInstancedArguments.startInstanceLocation);
				}
				else
				{
					// Draw without base vertex location
					glDrawElementsInstanced(mOpenGLES3PrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLES3Type(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())), static_cast<GLsizei>(drawIndexedInstancedArguments.instanceCount));
				}
			}
			else
			{
				// Without instancing

				// Use base vertex location?
				if (drawIndexedInstancedArguments.baseVertexLocation > 0)
				{
					// Is the "GL_EXT_draw_elements_base_vertex" extension there?
					if (mOpenGLES3Context->getExtensions().isGL_EXT_draw_elements_base_vertex())
					{
						// Draw with base vertex location
						glDrawElementsBaseVertexEXT(mOpenGLES3PrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLES3Type(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())), static_cast<GLint>(drawIndexedInstancedArguments.baseVertexLocation));
					}
					else
					{
						// Error!
						RENDERER_ASSERT(mContext, false, "Failed to OpenGL ES 3 draw indexed emulated")
					}
				}
				else
				{
					// Draw and advance
					glDrawElements(mOpenGLES3PrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLES3Type(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())));
					emulationData += sizeof(Renderer::DrawIndexedInstancedArguments);
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Debug                                                 ]
	//[-------------------------------------------------------]
	void OpenGLES3Renderer::setDebugMarker(const char*)
	{
		// TODO(co) Implement me. See
		// - https://www.opengl.org/registry/specs/EXT/EXT_debug_marker.txt
		// - https://www.opengl.org/registry/specs/EXT/EXT_debug_label.txt
		// - https://www.opengl.org/registry/specs/KHR/debug.txt
	}

	void OpenGLES3Renderer::beginDebugEvent(const char*)
	{
		// TODO(co) Implement me. See
		// - https://www.opengl.org/registry/specs/EXT/EXT_debug_marker.txt
		// - https://www.opengl.org/registry/specs/EXT/EXT_debug_label.txt
		// - https://www.opengl.org/registry/specs/KHR/debug.txt
	}

	void OpenGLES3Renderer::endDebugEvent()
	{
		// TODO(co) Implement me. See
		// - https://www.opengl.org/registry/specs/EXT/EXT_debug_marker.txt
		// - https://www.opengl.org/registry/specs/EXT/EXT_debug_label.txt
		// - https://www.opengl.org/registry/specs/KHR/debug.txt
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	const char* OpenGLES3Renderer::getName() const
	{
		return "OpenGLES3";
	}

	bool OpenGLES3Renderer::isInitialized() const
	{
		// Is the OpenGL ES 3 context initialized?
		return mOpenGLES3Context->isInitialized();
	}

	bool OpenGLES3Renderer::isDebugEnabled()
	{
		// OpenGL ES 3 has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)

		// Debug disabled
		return false;
	}


	//[-------------------------------------------------------]
	//[ Shader language                                       ]
	//[-------------------------------------------------------]
	uint32_t OpenGLES3Renderer::getNumberOfShaderLanguages() const
	{
		return 1;
	}

	const char* OpenGLES3Renderer::getShaderLanguageName(uint32_t index) const
	{
		// Evaluate the provided index
		switch (index)
		{
			case 0:
				return ShaderLanguageGlsl::NAME;
		}

		// Error!
		return nullptr;
	}

	Renderer::IShaderLanguage* OpenGLES3Renderer::getShaderLanguage(const char* shaderLanguageName)
	{
		// In case "shaderLanguage" is a null pointer, use the default shader language
		if (nullptr != shaderLanguageName)
		{
			// Optimization: Check for shader language name pointer match, first
			if (shaderLanguageName == ShaderLanguageGlsl::NAME || !stricmp(shaderLanguageName, ShaderLanguageGlsl::NAME))
			{
				// If required, create the GLSL shader language instance right now
				if (nullptr == mShaderLanguageGlsl)
				{
					mShaderLanguageGlsl = RENDERER_NEW(mContext, ShaderLanguageGlsl)(*this);
					mShaderLanguageGlsl->addReference();	// Internal renderer reference
				}

				// Return the shader language instance
				return mShaderLanguageGlsl;
			}

			// Error!
			return nullptr;
		}

		// Return the GLSL shader language instance as default
		return getShaderLanguage(ShaderLanguageGlsl::NAME);
	}


	//[-------------------------------------------------------]
	//[ Resource creation                                     ]
	//[-------------------------------------------------------]
	Renderer::IRenderPass* OpenGLES3Renderer::createRenderPass(uint32_t numberOfColorAttachments, const Renderer::TextureFormat::Enum* colorAttachmentTextureFormats, Renderer::TextureFormat::Enum depthStencilAttachmentTextureFormat, uint8_t numberOfMultisamples)
	{
		return RENDERER_NEW(mContext, RenderPass)(*this, numberOfColorAttachments, colorAttachmentTextureFormats, depthStencilAttachmentTextureFormat, numberOfMultisamples);
	}

	Renderer::ISwapChain* OpenGLES3Renderer::createSwapChain(Renderer::IRenderPass& renderPass, Renderer::WindowHandle windowHandle, bool)
	{
		// Sanity checks
		OPENGLES3RENDERER_RENDERERMATCHCHECK_ASSERT(*this, renderPass)
		RENDERER_ASSERT(mContext, NULL_HANDLE != windowHandle.nativeWindowHandle || nullptr != windowHandle.renderWindow, "OpenGL ES 3: The provided native window handle or render window must not be a null handle / null pointer")

		// Create the swap chain
		return RENDERER_NEW(mContext, SwapChain)(renderPass, windowHandle);
	}

	Renderer::IFramebuffer* OpenGLES3Renderer::createFramebuffer(Renderer::IRenderPass& renderPass, const Renderer::FramebufferAttachment* colorFramebufferAttachments, const Renderer::FramebufferAttachment* depthStencilFramebufferAttachment)
	{
		// Sanity check
		OPENGLES3RENDERER_RENDERERMATCHCHECK_ASSERT(*this, renderPass)

		// Create the framebuffer
		return RENDERER_NEW(mContext, Framebuffer)(renderPass, colorFramebufferAttachments, depthStencilFramebufferAttachment);
	}

	Renderer::IBufferManager* OpenGLES3Renderer::createBufferManager()
	{
		return RENDERER_NEW(mContext, BufferManager)(*this);
	}

	Renderer::ITextureManager* OpenGLES3Renderer::createTextureManager()
	{
		return RENDERER_NEW(mContext, TextureManager)(*this);
	}

	Renderer::IRootSignature* OpenGLES3Renderer::createRootSignature(const Renderer::RootSignature& rootSignature)
	{
		return RENDERER_NEW(mContext, RootSignature)(*this, rootSignature);
	}

	Renderer::IPipelineState* OpenGLES3Renderer::createPipelineState(const Renderer::PipelineState& pipelineState)
	{
		return RENDERER_NEW(mContext, PipelineState)(*this, pipelineState);
	}

	Renderer::ISamplerState* OpenGLES3Renderer::createSamplerState(const Renderer::SamplerState& samplerState)
	{
		return RENDERER_NEW(mContext, SamplerState)(*this, samplerState);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool OpenGLES3Renderer::map(Renderer::IResource& resource, uint32_t, Renderer::MapType mapType, uint32_t, Renderer::MappedSubresource& mappedSubresource)
	{
		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
			{
				const IndexBuffer& indexBuffer = static_cast<IndexBuffer&>(resource);
				return ::detail::mapBuffer(mContext, GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING, indexBuffer.getOpenGLES3ElementArrayBuffer(), indexBuffer.getBufferSize(), mapType, mappedSubresource);
			}

			case Renderer::ResourceType::VERTEX_BUFFER:
			{
				const VertexBuffer& vertexBuffer = static_cast<VertexBuffer&>(resource);
				return ::detail::mapBuffer(mContext, GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING, vertexBuffer.getOpenGLES3ArrayBuffer(), vertexBuffer.getBufferSize(), mapType, mappedSubresource);
			}

			case Renderer::ResourceType::UNIFORM_BUFFER:
			{
				const UniformBuffer& uniformBuffer = static_cast<UniformBuffer&>(resource);
				return ::detail::mapBuffer(mContext, GL_UNIFORM_BUFFER, GL_UNIFORM_BUFFER_BINDING, uniformBuffer.getOpenGLES3UniformBuffer(), uniformBuffer.getBufferSize(), mapType, mappedSubresource);
			}

			case Renderer::ResourceType::TEXTURE_BUFFER:
			{
				const TextureBuffer& textureBuffer = static_cast<TextureBuffer&>(resource);
				return ::detail::mapBuffer(mContext, GL_TEXTURE_BUFFER_EXT, GL_TEXTURE_BINDING_BUFFER_EXT, textureBuffer.getOpenGLES3TextureBuffer(), textureBuffer.getBufferSize(), mapType, mappedSubresource);
			}

			case Renderer::ResourceType::INDIRECT_BUFFER:
				mappedSubresource.data		 = static_cast<IndirectBuffer&>(resource).getWritableEmulationData();
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;
				return true;

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
			{
				// TODO(co) Implement me
				return false;
			}

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

	void OpenGLES3Renderer::unmap(Renderer::IResource& resource, uint32_t)
	{
		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
				::detail::unmapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING, static_cast<IndexBuffer&>(resource).getOpenGLES3ElementArrayBuffer());
				break;

			case Renderer::ResourceType::VERTEX_BUFFER:
				::detail::unmapBuffer(GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING, static_cast<VertexBuffer&>(resource).getOpenGLES3ArrayBuffer());
				break;

			case Renderer::ResourceType::UNIFORM_BUFFER:
				::detail::unmapBuffer(GL_UNIFORM_BUFFER, GL_UNIFORM_BUFFER_BINDING, static_cast<UniformBuffer&>(resource).getOpenGLES3UniformBuffer());
				break;

			case Renderer::ResourceType::TEXTURE_BUFFER:
				::detail::unmapBuffer(GL_TEXTURE_BUFFER_EXT, GL_TEXTURE_BINDING_BUFFER_EXT, static_cast<TextureBuffer&>(resource).getOpenGLES3TextureBuffer());
				break;

			case Renderer::ResourceType::INDIRECT_BUFFER:
				// Nothing here, it's a software emulated indirect buffer
				break;

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
				// TODO(co) Implement me
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
	bool OpenGLES3Renderer::beginScene()
	{
		// Not required when using OpenGL ES 3

		// Done
		return true;
	}

	void OpenGLES3Renderer::submitCommandBuffer(const Renderer::CommandBuffer& commandBuffer)
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

	void OpenGLES3Renderer::endScene()
	{
		// We need to forget about the currently set render target
		omSetRenderTarget(nullptr);

		// We need to forget about the currently set vertex array
		iaSetVertexArray(nullptr);
	}


	//[-------------------------------------------------------]
	//[ Synchronization                                       ]
	//[-------------------------------------------------------]
	void OpenGLES3Renderer::flush()
	{
		glFlush();
	}

	void OpenGLES3Renderer::finish()
	{
		glFinish();
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void OpenGLES3Renderer::selfDestruct()
	{
		RENDERER_DELETE(mContext, OpenGLES3Renderer, this);
	}


	//[-------------------------------------------------------]
	//[ Private static methods                                ]
	//[-------------------------------------------------------]
	#ifdef RENDERER_DEBUG
		void OpenGLES3Renderer::debugMessageCallback(uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int, const char* message, const void* userParam)
		{
			// Source to string
			char debugSource[20 + 1]{0};	// +1 for terminating zero
			switch (source)
			{
				case GL_DEBUG_SOURCE_API_KHR:
					strncpy(debugSource, "OpenGL", 20);
					break;

				case GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR:
					strncpy(debugSource, "Windows", 20);
					break;

				case GL_DEBUG_SOURCE_SHADER_COMPILER_KHR:
					strncpy(debugSource, "Shader compiler", 20);
					break;

				case GL_DEBUG_SOURCE_THIRD_PARTY_KHR:
					strncpy(debugSource, "Third party", 20);
					break;

				case GL_DEBUG_SOURCE_APPLICATION_KHR:
					strncpy(debugSource, "Application", 20);
					break;

				case GL_DEBUG_SOURCE_OTHER_KHR:
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
				case GL_DEBUG_TYPE_ERROR_KHR:
					strncpy(debugType, "Error", 25);
					break;

				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR:
					logType = Renderer::ILog::Type::COMPATIBILITY_WARNING;
					strncpy(debugType, "Deprecated behavior", 25);
					break;

				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR:
					strncpy(debugType, "Undefined behavior", 25);
					break;

				case GL_DEBUG_TYPE_PORTABILITY_KHR:
					logType = Renderer::ILog::Type::COMPATIBILITY_WARNING;
					strncpy(debugType, "Portability", 25);
					break;

				case GL_DEBUG_TYPE_PERFORMANCE_KHR:
					logType = Renderer::ILog::Type::PERFORMANCE_WARNING;
					strncpy(debugType, "Performance", 25);
					break;

				case GL_DEBUG_TYPE_OTHER_KHR:
					strncpy(debugType, "Other", 25);
					break;

				default:
					strncpy(debugType, "?", 25);
					break;
			}

			// Debug severity to string
			char debugSeverity[20 + 1]{0};	// +1 for terminating zero
			switch (severity)
			{
				case GL_DEBUG_SEVERITY_HIGH_KHR:
					strncpy(debugSeverity, "High", 20);
					break;

				case GL_DEBUG_SEVERITY_MEDIUM_KHR:
					strncpy(debugSeverity, "Medium", 20);
					break;

				case GL_DEBUG_SEVERITY_LOW_KHR:
					strncpy(debugSeverity, "Low", 20);
					break;

				case GL_DEBUG_SEVERITY_NOTIFICATION_KHR:
					strncpy(debugSeverity, "Notification", 20);
					break;

				default:
					strncpy(debugSeverity, "?", 20);
					break;
			}

			// Print into log
			if (static_cast<const OpenGLES3Renderer*>(userParam)->getContext().getLog().print(logType, nullptr, __FILE__, static_cast<uint32_t>(__LINE__), "OpenGL ES 3 debug message\tSource:\"%s\"\tType:\"%s\"\tID:\"%d\"\tSeverity:\"%s\"\tMessage:\"%s\"", debugSource, debugType, id, debugSeverity, message))
			{
				DEBUG_BREAK;
			}
		}
	#else
		void OpenGLES3Renderer::debugMessageCallback(uint32_t, uint32_t, uint32_t, uint32_t, int, const char*, const void*)
		{
			// Nothing here
		}
	#endif


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void OpenGLES3Renderer::initializeCapabilities()
	{
		GLint openGLValue = 0;

		{ // Get device name
		  // -> OpenGL ES Version 3.2 (November 3, 2016) Specification, section 20.2, page 440: "String queries return pointers to UTF-8 encoded, null-terminated static strings describing properties of the current GL context."
		  // -> For example "PVRVFrame 10.6 - None (Host : AMD Radeon R9 200 Series) (SDK Build: 17.1@4673912)" (used PowerVR_SDK OpenGL ES emulator)
			const size_t numberOfCharacters = ::detail::countof(mCapabilities.deviceName) - 1;
			strncpy(mCapabilities.deviceName, reinterpret_cast<const char*>(glGetString(GL_RENDERER)), numberOfCharacters);
			mCapabilities.deviceName[numberOfCharacters] = '\0';
		}

		// Preferred swap chain texture format
		mCapabilities.preferredSwapChainColorTextureFormat		  = Renderer::TextureFormat::Enum::R8G8B8A8;
		mCapabilities.preferredSwapChainDepthStencilTextureFormat = Renderer::TextureFormat::Enum::D32_FLOAT;

		// Maximum number of viewports (always at least 1)
		mCapabilities.maximumNumberOfViewports = 1;	// OpenGL ES 3 only supports a single viewport

		// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &openGLValue);
		mCapabilities.maximumNumberOfSimultaneousRenderTargets = static_cast<uint32_t>(openGLValue);

		// Maximum texture dimension
		openGLValue = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &openGLValue);
		mCapabilities.maximumTextureDimension = static_cast<uint32_t>(openGLValue);

		// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
		glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &openGLValue);
		mCapabilities.maximumNumberOf2DTextureArraySlices = static_cast<uint32_t>(openGLValue);

		// Maximum uniform buffer (UBO) size in bytes (usually at least 16384 bytes, in case there's no support for uniform buffer it's 0)
		openGLValue = 0;
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &openGLValue);
		mCapabilities.maximumUniformBufferSize = static_cast<uint32_t>(openGLValue);

		// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
		if (mOpenGLES3Context->getExtensions().isGL_EXT_texture_buffer())
		{
			openGLValue = 0;
			glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE_EXT, &openGLValue);
			mCapabilities.maximumTextureBufferSize = static_cast<uint32_t>(openGLValue);
		}
		else
		{
			// We can only emulate the "Renderer::TextureFormat::R32G32B32A32F" texture format using an uniform buffer
			mCapabilities.maximumTextureBufferSize = sizeof(float) * 4 * 4096;	// 64 KiB
		}

		// Maximum indirect buffer size in bytes (in case there's no support for indirect buffer it's 0)
		mCapabilities.maximumIndirectBufferSize = 64 * 1024;	// 64 KiB

		// Maximum number of multisamples (always at least 4 according to https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glGet.xhtml )
		glGetIntegerv(GL_MAX_SAMPLES, &openGLValue);
		if (openGLValue > 8)
		{
			// Limit to known maximum we can test
			openGLValue = 8;
		}
		mCapabilities.maximumNumberOfMultisamples = static_cast<uint8_t>(openGLValue);
		// TODO(co) Implement multisample support
		mCapabilities.maximumNumberOfMultisamples = 1;

		// Maximum anisotropy (always at least 1, usually 16)
		// -> "GL_EXT_texture_filter_anisotropic"-extension
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &openGLValue);
		mCapabilities.maximumAnisotropy = static_cast<uint8_t>(openGLValue);

		// Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		mCapabilities.individualUniforms = true;

		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
		mCapabilities.instancedArrays = true;	// Is core feature in OpenGL ES 3.0

		// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID, OpenGL ES 3 has no "GL_ARB_draw_instanced" extension)
		mCapabilities.drawInstanced = true;	// Is core feature in OpenGL ES 3.0

		// Base vertex supported for draw calls?
		mCapabilities.baseVertex = mOpenGLES3Context->getExtensions().isGL_EXT_draw_elements_base_vertex();

		// OpenGL ES 3 has no native multi-threading
		mCapabilities.nativeMultiThreading = false;

		// We don't support the OpenGL ES 3 program binaries since those are operation system and graphics driver version dependent, which renders them useless for pre-compiled shaders shipping
		mCapabilities.shaderBytecode = false;

		// Is there support for vertex shaders (VS)?
		mCapabilities.vertexShader = true;

		// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
		mCapabilities.maximumNumberOfPatchVertices = 0;	// OpenGL ES 3 has no tessellation support

		// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
		mCapabilities.maximumNumberOfGsOutputVertices = 0;	// OpenGL ES 3 has no support for geometry shaders

		// Is there support for fragment shaders (FS)?
		mCapabilities.fragmentShader = true;
	}

	void OpenGLES3Renderer::setProgram(Renderer::IProgram* program)
	{
		if (nullptr != program)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES3RENDERER_RENDERERMATCHCHECK_ASSERT(*this, *program)

			// Bind the program, if required
			const ProgramGlsl* programGlsl = static_cast<ProgramGlsl*>(program);
			const uint32_t openGLES3Program = programGlsl->getOpenGLES3Program();
			if (openGLES3Program != mOpenGLES3Program)
			{
				mOpenGLES3Program = openGLES3Program;
				mDrawIdUniformLocation = programGlsl->getDrawIdUniformLocation();
				mCurrentStartInstanceLocation = ~0u;
				glUseProgram(mOpenGLES3Program);
			}
		}
		else if (0 != mOpenGLES3Program)
		{
			// Unbind the program
			glUseProgram(0);
			mOpenGLES3Program = 0;
			mDrawIdUniformLocation = -1;
			mCurrentStartInstanceLocation = ~0u;
		}
	}

	void OpenGLES3Renderer::updateGL_EXT_base_instanceEmulation(uint32_t startInstanceLocation)
	{
		if (mDrawIdUniformLocation != -1 && 0 != mOpenGLES3Program && mCurrentStartInstanceLocation != startInstanceLocation)
		{
			glUniform1ui(mDrawIdUniformLocation, startInstanceLocation);
			mCurrentStartInstanceLocation = startInstanceLocation;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
