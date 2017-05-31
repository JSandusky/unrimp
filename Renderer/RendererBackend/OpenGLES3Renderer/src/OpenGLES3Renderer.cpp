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
#include "OpenGLES3Renderer/OpenGLES3Debug.h"	// For "OPENGLES3RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "OpenGLES3Renderer/Mapping.h"
#include "OpenGLES3Renderer/IExtensions.h"
#include "OpenGLES3Renderer/RootSignature.h"
#include "OpenGLES3Renderer/ContextRuntimeLinking.h"
#include "OpenGLES3Renderer/RenderTarget/SwapChain.h"
#include "OpenGLES3Renderer/RenderTarget/Framebuffer.h"
#include "OpenGLES3Renderer/Buffer/BufferManager.h"
#include "OpenGLES3Renderer/Buffer/IndexBuffer.h"
#include "OpenGLES3Renderer/Buffer/TextureBufferBind.h"
#include "OpenGLES3Renderer/Buffer/UniformBufferBind.h"
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
#include "OpenGLES3Renderer/ContextRuntimeLinking.h"

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
OPENGLES3RENDERER_API_EXPORT Renderer::IRenderer *createOpenGLES3RendererInstance(handle nativeWindowHandle, bool useExternalContext)
{
	return new OpenGLES3Renderer::OpenGLES3Renderer(nativeWindowHandle, useExternalContext);
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
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).setGraphicsRootSignature(realData->rootSignature);
			}

			void SetGraphicsRootDescriptorTable(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetGraphicsRootDescriptorTable* realData = static_cast<const Renderer::Command::SetGraphicsRootDescriptorTable*>(data);
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).setGraphicsRootDescriptorTable(realData->rootParameterIndex, realData->resource);
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

			void SetPrimitiveTopology(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetPrimitiveTopology* realData = static_cast<const Renderer::Command::SetPrimitiveTopology*>(data);
				static_cast<OpenGLES3Renderer::OpenGLES3Renderer&>(renderer).iaSetPrimitiveTopology(realData->primitiveTopology);
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
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	OpenGLES3Renderer::OpenGLES3Renderer(handle nativeWindowHandle, bool useExternalContext) :
		mContext(new ContextRuntimeLinking(nativeWindowHandle, useExternalContext)),
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
		mMainSwapChain(nullptr),
		mRenderTarget(nullptr),
		// State cache to avoid making redundant OpenGL ES 3 calls
		mOpenGLES3Program(0)
	{
		// Initialize the context
		if (mContext->initialize(0))
		{
			// Initialize the capabilities
			initializeCapabilities();

			// Create the default sampler state
			mDefaultSamplerState = createSamplerState(Renderer::ISamplerState::getDefaultSamplerState());

			#ifdef RENDERER_OUTPUT_DEBUG
				// "GL_KHR_debug"-extension available?
				if (mContext->getExtensions().isGL_KHR_debug())
				{
					// Synchronous debug output, please
					// -> Makes it easier to find the place causing the issue
					glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);

					// We don't need to configure the debug output by using "glDebugMessageControlARB()",
					// by default all messages are enabled and this is good this way

					// Set the debug message callback function
					glDebugMessageCallbackKHR(&OpenGLES3Renderer::debugMessageCallback, nullptr);
				}
			#endif

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
					RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL ES 3 error: Renderer is going to be destroyed, but there are still %lu resource instances left (memory leak)\n", numberOfCurrentResources)
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 3 error: Renderer is going to be destroyed, but there is still one resource instance left (memory leak)\n")
				}

				// Use debug output to show the current number of resource instances
				getStatistics().debugOutputCurrentResouces();
			}
		}
		#endif

		// Release the GLSL shader language instance, in case we have one
		if (nullptr != mShaderLanguageGlsl)
		{
			mShaderLanguageGlsl->releaseReference();
		}

		// Destroy the context instance
		delete mContext;
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
			OPENGLES3RENDERER_RENDERERMATCHCHECK_RETURN(*this, *rootSignature)
		}
	}

	void OpenGLES3Renderer::setGraphicsRootDescriptorTable(uint32_t rootParameterIndex, Renderer::IResource* resource)
	{
		// Security checks
		#ifndef OPENGLES3RENDERER_NO_DEBUG
		{
			if (nullptr == mGraphicsRootSignature)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 3 error: No graphics root signature set")
				return;
			}
			const Renderer::RootSignature& rootSignature = mGraphicsRootSignature->getRootSignature();
			if (rootParameterIndex >= rootSignature.numberOfParameters)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 3 error: Root parameter index is out of bounds")
				return;
			}
			const Renderer::RootParameter& rootParameter = rootSignature.parameters[rootParameterIndex];
			if (Renderer::RootParameterType::DESCRIPTOR_TABLE != rootParameter.parameterType)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 3 error: Root parameter index doesn't reference a descriptor table")
				return;
			}

			// TODO(co) For now, we only support a single descriptor range
			if (1 != rootParameter.descriptorTable.numberOfDescriptorRanges)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 3 error: Only a single descriptor range is supported")
				return;
			}
			if (nullptr == reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges))
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 3 error: Descriptor ranges is a null pointer")
				return;
			}
		}
		#endif

		if (nullptr != resource)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES3RENDERER_RENDERERMATCHCHECK_RETURN(*this, *resource)

			// Get the root signature parameter instance
			const Renderer::RootParameter& rootParameter = mGraphicsRootSignature->getRootSignature().parameters[rootParameterIndex];
			const Renderer::DescriptorRange* descriptorRange = reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges);

			// Check the type of resource to set
			// TODO(co) Some additional resource type root signature security checks in debug build?
			// TODO(co) There's room for binding API call related optimization in here (will certainly be no huge overall efficiency gain)
			const Renderer::ResourceType resourceType = resource->getResourceType();
			switch (resourceType)
			{
				case Renderer::ResourceType::UNIFORM_BUFFER:
				{
					// Attach the buffer to the given UBO binding point
					// -> Explicit binding points ("layout(binding = 0)" in GLSL shader) requires OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension
					// -> Direct3D 10 and Direct3D 11 have explicit binding points
					glBindBufferBase(GL_UNIFORM_BUFFER, rootParameterIndex, static_cast<UniformBuffer*>(resource)->getOpenGLESUniformBuffer());
					break;
				}

				case Renderer::ResourceType::TEXTURE_BUFFER:
					if (mContext->getExtensions().isGL_EXT_texture_buffer())
					{
						// Fall through by intent
					}
					else
					{
						// We can only emulate the "Renderer::TextureFormat::R32G32B32A32F" texture format using an uniform buffer

						// Attach the buffer to the given UBO binding point
						// -> Explicit binding points ("layout(binding = 0)" in GLSL shader) requires OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension
						// -> Direct3D 10 and Direct3D 11 have explicit binding points
						glBindBufferBase(GL_UNIFORM_BUFFER, rootParameterIndex, static_cast<TextureBuffer*>(resource)->getOpenGLESTextureBuffer());
						break;
					}

				case Renderer::ResourceType::TEXTURE_1D:
				case Renderer::ResourceType::TEXTURE_2D:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				case Renderer::ResourceType::TEXTURE_3D:
				case Renderer::ResourceType::TEXTURE_CUBE:
				{
					switch (rootParameter.shaderVisibility)
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
							glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + descriptorRange->baseShaderRegister));

							// Bind texture or texture buffer
							if (resourceType == Renderer::ResourceType::TEXTURE_BUFFER)
							{
								glBindTexture(GL_TEXTURE_BUFFER_EXT, static_cast<TextureBuffer*>(resource)->getOpenGLESTexture());
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

							if (Renderer::ResourceType::TEXTURE_BUFFER != resourceType)
							{
								// Set the OpenGL ES 3 sampler states
								mGraphicsRootSignature->setOpenGLES3SamplerStates(descriptorRange->samplerRootParameterIndex);
							}

							#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
								// Be polite and restore the previous active OpenGL ES 3 texture
								glActiveTexture(static_cast<GLuint>(openGLES3ActiveTextureBackup));
							#endif
							break;
						}

						case Renderer::ShaderVisibility::TESSELLATION_CONTROL:
							RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 3: OpenGL ES 3 has no tessellation control shader support (hull shader in Direct3D terminology)")
							break;

						case Renderer::ShaderVisibility::TESSELLATION_EVALUATION:
							RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 3 error: OpenGL ES 3 has no tessellation evaluation shader support (domain shader in Direct3D terminology)")
							break;

						case Renderer::ShaderVisibility::GEOMETRY:
							RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 3 error: OpenGL ES 3 has no geometry shader support")
							break;
					}
					break;
				}

				case Renderer::ResourceType::SAMPLER_STATE:
				{
					// Unlike Direct3D >=10, OpenGL ES 3 directly attaches the sampler settings to the texture
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
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 3 error: Invalid resource type")
					break;
			}
		}
		else
		{
			// TODO(co) Handle this situation?
			/*
			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Backup the currently active OpenGL ES 3 texture
				GLint openGLES3ActiveTextureBackup = 0;
				glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLES3ActiveTextureBackup);
			#endif

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			glActiveTexture(GL_TEXTURE0 + unit);

			// Unbind the texture at the given texture unit
			glBindTexture(GL_TEXTURE_2D, 0);

			#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous active OpenGL ES 3 texture
				glActiveTexture(openGLES3ActiveTextureBackup);
			#endif
			*/
		}
	}

	void OpenGLES3Renderer::setPipelineState(Renderer::IPipelineState* pipelineState)
	{
		if (mPipelineState != pipelineState)
		{
			if (nullptr != pipelineState)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLES3RENDERER_RENDERERMATCHCHECK_RETURN(*this, *pipelineState)

				// Set new pipeline state and add a reference to it
				if (nullptr != mPipelineState)
				{
					mPipelineState->releaseReference();
				}
				mPipelineState = static_cast<PipelineState*>(pipelineState);
				mPipelineState->addReference();

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
	void OpenGLES3Renderer::iaSetVertexArray(Renderer::IVertexArray *vertexArray)
	{
		// New vertex array?
		if (mVertexArray != vertexArray)
		{
			// Set a vertex array?
			if (nullptr != vertexArray)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLES3RENDERER_RENDERERMATCHCHECK_RETURN(*this, *vertexArray)

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
			else
			{
				// Release the vertex array reference, in case we have one
				if (nullptr != mVertexArray)
				{
					// Unbind OpenGL ES 3 vertex array
					glBindVertexArray(0);

					// Release reference
					mVertexArray->releaseReference();
					mVertexArray = nullptr;
				}
			}
		}
	}

	void OpenGLES3Renderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology primitiveTopology)
	{
		// Map and backup the set OpenGL ES 3 primitive topology
		mOpenGLES3PrimitiveTopology = Mapping::getOpenGLES3Type(primitiveTopology);
	}


	//[-------------------------------------------------------]
	//[ Rasterizer (RS) stage                                 ]
	//[-------------------------------------------------------]
	void OpenGLES3Renderer::rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport *viewports)
	{
		// Are the given viewports valid?
		if (numberOfViewports > 0 && nullptr != viewports)
		{
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
		#ifndef RENDERER_NO_DEBUG
			if (numberOfViewports > 1)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 3 error: OpenGL ES 3 supports only one viewport")
			}
		#endif
			glViewport(static_cast<GLint>(viewports->topLeftX), static_cast<GLint>(renderTargetHeight - viewports->topLeftY - viewports->height), static_cast<GLsizei>(viewports->width), static_cast<GLsizei>(viewports->height));
			glDepthRangef(static_cast<GLclampf>(viewports->minDepth), static_cast<GLclampf>(viewports->maxDepth));
		}
	}

	void OpenGLES3Renderer::rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles)
	{
		// Are the given scissor rectangles valid?
		if (numberOfScissorRectangles > 0 && nullptr != scissorRectangles)
		{
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
		#ifndef RENDERER_NO_DEBUG
			if (numberOfScissorRectangles > 1)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 3 error: OpenGL ES 3 supports only one scissor rectangle")
			}
		#endif
			const GLsizei width  = scissorRectangles->bottomRightX - scissorRectangles->topLeftX;
			const GLsizei height = scissorRectangles->bottomRightY - scissorRectangles->topLeftY;
			glScissor(static_cast<GLint>(scissorRectangles->topLeftX), static_cast<GLint>(renderTargetHeight - scissorRectangles->topLeftY - height), width, height);
		}
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	void OpenGLES3Renderer::omSetRenderTarget(Renderer::IRenderTarget *renderTarget)
	{
		// New render target?
		if (mRenderTarget != renderTarget)
		{
			// Set a render target?
			if (nullptr != renderTarget)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLES3RENDERER_RENDERERMATCHCHECK_RETURN(*this, *renderTarget)

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
						Framebuffer *framebuffer = static_cast<Framebuffer*>(mRenderTarget);

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
		OPENGLES3RENDERER_RENDERERMATCHCHECK_RETURN(*this, destinationResource)
		OPENGLES3RENDERER_RENDERERMATCHCHECK_RETURN(*this, sourceResource)

		// Evaluate the render target type
		switch (destinationResource.getResourceType())
		{
			case Renderer::ResourceType::TEXTURE_2D:
				if (sourceResource.getResourceType() == Renderer::ResourceType::TEXTURE_2D)
				{
					// Get the OpenGL ES 3 texture 2D instances
					const Texture2D& openGlEs3DestinationTexture2D = static_cast<const Texture2D&>(destinationResource);
					const Texture2D& openGlEs3SourceTexture2D = static_cast<const Texture2D&>(sourceResource);
					assert(openGlEs3DestinationTexture2D.getWidth() == openGlEs3SourceTexture2D.getWidth());
					assert(openGlEs3DestinationTexture2D.getHeight() == openGlEs3SourceTexture2D.getHeight());

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
					assert(false);
				}
				break;

			case Renderer::ResourceType::ROOT_SIGNATURE:
			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
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
		if (nullptr != mVertexArray && numberOfDraws > 0)
		{
			assert(nullptr != emulationData);

			// TODO(co) Currently no buffer overflow check due to lack of interface provided data
			emulationData += indirectBufferOffset;

			// Emit the draw calls
			for (uint32_t i = 0; i < numberOfDraws; ++i)
			{
				const Renderer::DrawInstancedArguments& drawInstancedArguments = *reinterpret_cast<const Renderer::DrawInstancedArguments*>(emulationData);

				// Draw and advance
				if (drawInstancedArguments.instanceCount > 1)
				{
					// With instancing
					glDrawArraysInstanced(mOpenGLES3PrimitiveTopology, static_cast<GLint>(drawInstancedArguments.startVertexLocation), static_cast<GLsizei>(drawInstancedArguments.vertexCountPerInstance), static_cast<GLsizei>(drawInstancedArguments.instanceCount));
				}
				else
				{
					// Without instancing
					assert(drawInstancedArguments.instanceCount <= 1);
					glDrawArrays(mOpenGLES3PrimitiveTopology, static_cast<GLint>(drawInstancedArguments.startVertexLocation), static_cast<GLsizei>(drawInstancedArguments.vertexCountPerInstance));
				}
				emulationData += sizeof(Renderer::DrawInstancedArguments);
			}
		}
	}

	void OpenGLES3Renderer::drawIndexedEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Is currently an vertex array?
		if (nullptr != mVertexArray && numberOfDraws > 0)
		{
			// Get the used index buffer
			IndexBuffer *indexBuffer = mVertexArray->getIndexBuffer();
			if (nullptr != indexBuffer)
			{
				assert(nullptr != emulationData);

				// TODO(co) Currently no buffer overflow check due to lack of interface provided data
				emulationData += indirectBufferOffset;

				// Emit the draw calls
				for (uint32_t i = 0; i < numberOfDraws; ++i)
				{
					const Renderer::DrawIndexedInstancedArguments& drawIndexedInstancedArguments = *reinterpret_cast<const Renderer::DrawIndexedInstancedArguments*>(emulationData);

					if (drawIndexedInstancedArguments.instanceCount > 1)
					{
						// With instancing

						// Use base vertex location?
						if (drawIndexedInstancedArguments.baseVertexLocation > 0)
						{
							// Is the "GL_EXT_draw_elements_base_vertex" extension there?
							if (mContext->getExtensions().isGL_EXT_draw_elements_base_vertex())
							{
								// Draw with base vertex location
								glDrawElementsInstancedBaseVertexEXT(mOpenGLES3PrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLES3Type(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())), static_cast<GLsizei>(drawIndexedInstancedArguments.instanceCount), static_cast<GLint>(drawIndexedInstancedArguments.baseVertexLocation));
							}
							else
							{
								// Error!
								assert(false);
							}
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
						assert(drawIndexedInstancedArguments.instanceCount <= 1);

						// Use base vertex location?
						if (drawIndexedInstancedArguments.baseVertexLocation > 0)
						{
							// Is the "GL_EXT_draw_elements_base_vertex" extension there?
							if (mContext->getExtensions().isGL_EXT_draw_elements_base_vertex())
							{
								// Draw with base vertex location
								glDrawElementsBaseVertexEXT(mOpenGLES3PrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLES3Type(), reinterpret_cast<void*>(static_cast<uintptr_t>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes())), static_cast<GLint>(drawIndexedInstancedArguments.baseVertexLocation));
							}
							else
							{
								// Error!
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
		}
	}


	//[-------------------------------------------------------]
	//[ Debug                                                 ]
	//[-------------------------------------------------------]
	void OpenGLES3Renderer::setDebugMarker(const char *)
	{
		// TODO(co) Implement me. See
		// - https://www.opengl.org/registry/specs/EXT/EXT_debug_marker.txt
		// - https://www.opengl.org/registry/specs/EXT/EXT_debug_label.txt
		// - https://www.opengl.org/registry/specs/KHR/debug.txt
	}

	void OpenGLES3Renderer::beginDebugEvent(const char *)
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
	const char *OpenGLES3Renderer::getName() const
	{
		return "OpenGLES3";
	}

	bool OpenGLES3Renderer::isInitialized() const
	{
		// Is the context initialized?
		return mContext->isInitialized();
	}

	bool OpenGLES3Renderer::isDebugEnabled()
	{
		// OpenGL ES 3 has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)

		// Debug disabled
		return false;
	}

	Renderer::ISwapChain *OpenGLES3Renderer::getMainSwapChain() const
	{
		return mMainSwapChain;
	}


	//[-------------------------------------------------------]
	//[ Shader language                                       ]
	//[-------------------------------------------------------]
	uint32_t OpenGLES3Renderer::getNumberOfShaderLanguages() const
	{
		return 1;
	}

	const char *OpenGLES3Renderer::getShaderLanguageName(uint32_t index) const
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

	Renderer::IShaderLanguage *OpenGLES3Renderer::getShaderLanguage(const char *shaderLanguageName)
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
					mShaderLanguageGlsl = new ShaderLanguageGlsl(*this);
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
	Renderer::ISwapChain *OpenGLES3Renderer::createSwapChain(handle nativeWindowHandle, bool)
	{
		// The provided native window handle must not be a null handle
		return (NULL_HANDLE != nativeWindowHandle) ? new SwapChain(*this, nativeWindowHandle) : nullptr;
	}

	Renderer::IFramebuffer *OpenGLES3Renderer::createFramebuffer(uint32_t numberOfColorFramebufferAttachments, const Renderer::FramebufferAttachment *colorFramebufferAttachments, const Renderer::FramebufferAttachment *depthStencilFramebufferAttachment)
	{
		// Validation is done inside the framebuffer implementation
		return new Framebuffer(*this, numberOfColorFramebufferAttachments, colorFramebufferAttachments, depthStencilFramebufferAttachment);
	}

	Renderer::IBufferManager *OpenGLES3Renderer::createBufferManager()
	{
		return new BufferManager(*this);
	}

	Renderer::ITextureManager *OpenGLES3Renderer::createTextureManager()
	{
		return new TextureManager(*this);
	}

	Renderer::IRootSignature *OpenGLES3Renderer::createRootSignature(const Renderer::RootSignature &rootSignature)
	{
		return new RootSignature(*this, rootSignature);
	}

	Renderer::IPipelineState *OpenGLES3Renderer::createPipelineState(const Renderer::PipelineState& pipelineState)
	{
		return new PipelineState(*this, pipelineState);
	}

	Renderer::ISamplerState *OpenGLES3Renderer::createSamplerState(const Renderer::SamplerState &samplerState)
	{
		return new SamplerState(*this, samplerState);
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
				// TODO(co) This buffer update isn't efficient, use e.g. persistent buffer mapping

				#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
					// Backup the currently bound OpenGL ES 3 array element buffer
					GLint openGLES3ArrayElementBufferBackup = 0;
					glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &openGLES3ArrayElementBufferBackup);
				#endif

				// Bind this OpenGL ES 3 element buffer and upload the data
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<IndexBuffer&>(resource).getOpenGLES3ElementArrayBuffer());

				// Map
				mappedSubresource.data		 = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(static_cast<IndexBuffer&>(resource).getBufferSize()), Mapping::getOpenGLES3MapRangeType(mapType));
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;

				#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL ES 3 array element buffer
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(openGLES3ArrayElementBufferBackup));
				#endif

				// Done
				return true;
			}

			case Renderer::ResourceType::VERTEX_BUFFER:
			{
				// TODO(co) This buffer update isn't efficient, use e.g. persistent buffer mapping

				#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
					// Backup the currently bound OpenGL ES 3 array buffer
					GLint openGLES3ArrayBufferBackup = 0;
					glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &openGLES3ArrayBufferBackup);
				#endif

				// Bind this OpenGL ES 3 array buffer and upload the data
				glBindBuffer(GL_ARRAY_BUFFER, static_cast<VertexBuffer&>(resource).getOpenGLES3ArrayBuffer());

				// Map
				mappedSubresource.data		 = glMapBufferRange(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(static_cast<VertexBuffer&>(resource).getBufferSize()), Mapping::getOpenGLES3MapRangeType(mapType));
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;

				#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL ES 3 array buffer
					glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(openGLES3ArrayBufferBackup));
				#endif

				// Done
				return true;
			}

			case Renderer::ResourceType::UNIFORM_BUFFER:
				// OpenGL ES 3 has no uniform buffer
				// TODO(co) Error handling
				return false;

			case Renderer::ResourceType::TEXTURE_BUFFER:
				// OpenGL ES 3 has no texture buffer
				// TODO(co) Error handling
				return false;

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
				ID3D11Resource *d3d11Resource = nullptr;
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
				ID3D11Resource *d3d11Resource = nullptr;
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
			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
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
			{
				#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
					// Backup the currently bound OpenGL ES 3 array element buffer
					GLint openGLES3ArrayElementBufferBackup = 0;
					glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &openGLES3ArrayElementBufferBackup);
				#endif

				// Bind this OpenGL ES 3 element buffer and upload the data
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<IndexBuffer&>(resource).getOpenGLES3ElementArrayBuffer());

				// Unmap
				glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

				#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL ES 3 array element buffer
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(openGLES3ArrayElementBufferBackup));
				#endif
				break;
			}

			case Renderer::ResourceType::VERTEX_BUFFER:
			{
				#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
					// Backup the currently bound OpenGL ES 3 array buffer
					GLint openGLES3ArrayBufferBackup = 0;
					glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &openGLES3ArrayBufferBackup);
				#endif

				// Bind this OpenGL ES 3 array buffer and upload the data
				glBindBuffer(GL_ARRAY_BUFFER, static_cast<VertexBuffer&>(resource).getOpenGLES3ArrayBuffer());

				// Unmap
				glUnmapBuffer(GL_ARRAY_BUFFER);

				#ifndef OPENGLES3RENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL ES 3 array buffer
					glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(openGLES3ArrayBufferBackup));
				#endif
				break;
			}

			case Renderer::ResourceType::UNIFORM_BUFFER:
				// OpenGL ES 3 has no uniform buffer
				// TODO(co) Error handling
				break;

			case Renderer::ResourceType::TEXTURE_BUFFER:
				// OpenGL ES 3 has no texture buffer
				// TODO(co) Error handling
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
				ID3D11Resource *d3d11Resource = nullptr;
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
				ID3D11Resource *d3d11Resource = nullptr;
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
			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
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
	//[ Private static methods                                ]
	//[-------------------------------------------------------]
	void OpenGLES3Renderer::debugMessageCallback(uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int, const char *message, const void *)
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
		char debugType[25 + 1]{0};	// +1 for terminating zero
		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR_KHR:
				strncpy(debugType, "Error", 25);
				break;

			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR:
				strncpy(debugType, "Deprecated behavior", 25);
				break;

			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR:
				strncpy(debugType, "Undefined behavior", 25);
				break;

			case GL_DEBUG_TYPE_PORTABILITY_KHR:
				strncpy(debugType, "Portability", 25);
				break;

			case GL_DEBUG_TYPE_PERFORMANCE_KHR:
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

		// Output the debug message
		#ifdef RENDERER_NO_DEBUG
			// Avoid "warning C4100: '<x>' : unreferenced formal parameter"-warning
			id = id;
			message = message;
		#else
			RENDERER_OUTPUT_DEBUG_PRINTF("OpenGLES error: OpenGL debug message\tSource:\"%s\"\tType:\"%s\"\tID:\"%d\"\tSeverity:\"%s\"\tMessage:\"%s\"\n", debugSource, debugType, id, debugSeverity, message)
		#endif
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void OpenGLES3Renderer::initializeCapabilities()
	{
		GLint openGLValue = 0;

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
		if (mContext->getExtensions().isGL_EXT_texture_buffer())
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
		mCapabilities.maximumIndirectBufferSize = sizeof(Renderer::DrawIndexedInstancedArguments) * 4096;	// TODO(co) What is an usually decent emulated indirect buffer size?

		// Maximum number of multisamples (always at least 1, usually 8)
		mCapabilities.maximumNumberOfMultisamples = 1;	// Don't want to support the legacy OpenGL ES 3 multisample support

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
		mCapabilities.baseVertex = mContext->getExtensions().isGL_EXT_draw_elements_base_vertex();

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

	void OpenGLES3Renderer::setProgram(Renderer::IProgram *program)
	{
		if (nullptr != program)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES3RENDERER_RENDERERMATCHCHECK_RETURN(*this, *program)

			// Bind the program, if required
			const uint32_t openGLES3Program = static_cast<ProgramGlsl*>(program)->getOpenGLES3Program();
			if (openGLES3Program != mOpenGLES3Program)
			{
				mOpenGLES3Program = openGLES3Program;
				glUseProgram(mOpenGLES3Program);
			}
		}
		else if (0 != mOpenGLES3Program)
		{
			// Unbind the program
			glUseProgram(0);
			mOpenGLES3Program = 0;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
