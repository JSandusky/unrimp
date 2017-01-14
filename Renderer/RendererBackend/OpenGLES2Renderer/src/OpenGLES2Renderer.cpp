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
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"
#include "OpenGLES2Renderer/OpenGLES2Debug.h"	// For "OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "OpenGLES2Renderer/Mapping.h"
#include "OpenGLES2Renderer/IExtensions.h"
#include "OpenGLES2Renderer/RootSignature.h"
#include "OpenGLES2Renderer/ContextRuntimeLinking.h"
#include "OpenGLES2Renderer/RenderTarget/SwapChain.h"
#include "OpenGLES2Renderer/RenderTarget/Framebuffer.h"
#include "OpenGLES2Renderer/Buffer/BufferManager.h"
#include "OpenGLES2Renderer/Buffer/IndexBuffer.h"
#include "OpenGLES2Renderer/Buffer/TextureBufferBind.h"
#include "OpenGLES2Renderer/Buffer/UniformBufferBind.h"
#include "OpenGLES2Renderer/Buffer/VertexBuffer.h"
#include "OpenGLES2Renderer/Buffer/VertexArrayVao.h"
#include "OpenGLES2Renderer/Buffer/VertexArrayNoVao.h"
#include "OpenGLES2Renderer/Buffer/IndirectBuffer.h"
#include "OpenGLES2Renderer/Texture/TextureManager.h"
#include "OpenGLES2Renderer/Texture/Texture2D.h"
#include "OpenGLES2Renderer/Texture/Texture2DArray.h"
#include "OpenGLES2Renderer/State/SamplerState.h"
#include "OpenGLES2Renderer/State/PipelineState.h"
#include "OpenGLES2Renderer/Shader/ProgramGlsl.h"
#include "OpenGLES2Renderer/Shader/ShaderLanguageGlsl.h"
#include "OpenGLES2Renderer/ContextRuntimeLinking.h"

#include <Renderer/Buffer/CommandBuffer.h>
#include <Renderer/Buffer/IndirectBufferTypes.h>

#include <GLES2/gl2ext.h>

#include <cassert>


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef OPENGLES2RENDERER_EXPORTS
	#define OPENGLES2RENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define OPENGLES2RENDERER_API_EXPORT
#endif
OPENGLES2RENDERER_API_EXPORT Renderer::IRenderer *createOpenGLES2RendererInstance2(handle nativeWindowHandle, bool useExternalContext)
{
	return new OpenGLES2Renderer::OpenGLES2Renderer(nativeWindowHandle, useExternalContext);
}

OPENGLES2RENDERER_API_EXPORT Renderer::IRenderer *createOpenGLES2RendererInstance(handle nativeWindowHandle)
{
	return new OpenGLES2Renderer::OpenGLES2Renderer(nativeWindowHandle, false);
}
#undef OPENGLES2RENDERER_API_EXPORT


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


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
				void CopyUniformBufferData(const void*, Renderer::IRenderer&)
				{
					// Not supported by OpenGL ES 2
				}

				void CopyTextureBufferData(const void*, Renderer::IRenderer&)
				{
					// Not supported by OpenGL ES 2
				}

				//[-------------------------------------------------------]
				//[ Graphics root                                         ]
				//[-------------------------------------------------------]
				void SetGraphicsRootSignature(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::SetGraphicsRootSignature* realData = static_cast<const Renderer::Command::SetGraphicsRootSignature*>(data);
					static_cast<OpenGLES2Renderer&>(renderer).setGraphicsRootSignature(realData->rootSignature);
				}

				void SetGraphicsRootDescriptorTable(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::SetGraphicsRootDescriptorTable* realData = static_cast<const Renderer::Command::SetGraphicsRootDescriptorTable*>(data);
					static_cast<OpenGLES2Renderer&>(renderer).setGraphicsRootDescriptorTable(realData->rootParameterIndex, realData->resource);
				}

				//[-------------------------------------------------------]
				//[ States                                                ]
				//[-------------------------------------------------------]
				void SetPipelineState(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::SetPipelineState* realData = static_cast<const Renderer::Command::SetPipelineState*>(data);
					static_cast<OpenGLES2Renderer&>(renderer).setPipelineState(realData->pipelineState);
				}

				//[-------------------------------------------------------]
				//[ Input-assembler (IA) stage                            ]
				//[-------------------------------------------------------]
				void SetVertexArray(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::SetVertexArray* realData = static_cast<const Renderer::Command::SetVertexArray*>(data);
					static_cast<OpenGLES2Renderer&>(renderer).iaSetVertexArray(realData->vertexArray);
				}

				void SetPrimitiveTopology(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::SetPrimitiveTopology* realData = static_cast<const Renderer::Command::SetPrimitiveTopology*>(data);
					static_cast<OpenGLES2Renderer&>(renderer).iaSetPrimitiveTopology(realData->primitiveTopology);
				}

				//[-------------------------------------------------------]
				//[ Rasterizer (RS) stage                                 ]
				//[-------------------------------------------------------]
				void SetViewports(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::SetViewports* realData = static_cast<const Renderer::Command::SetViewports*>(data);
					static_cast<OpenGLES2Renderer&>(renderer).rsSetViewports(realData->numberOfViewports, (nullptr != realData->viewports) ? realData->viewports : reinterpret_cast<const Renderer::Viewport*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
				}

				void SetScissorRectangles(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::SetScissorRectangles* realData = static_cast<const Renderer::Command::SetScissorRectangles*>(data);
					static_cast<OpenGLES2Renderer&>(renderer).rsSetScissorRectangles(realData->numberOfScissorRectangles, (nullptr != realData->scissorRectangles) ? realData->scissorRectangles : reinterpret_cast<const Renderer::ScissorRectangle*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
				}

				//[-------------------------------------------------------]
				//[ Output-merger (OM) stage                              ]
				//[-------------------------------------------------------]
				void SetRenderTarget(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::SetRenderTarget* realData = static_cast<const Renderer::Command::SetRenderTarget*>(data);
					static_cast<OpenGLES2Renderer&>(renderer).omSetRenderTarget(realData->renderTarget);
				}

				//[-------------------------------------------------------]
				//[ Operations                                            ]
				//[-------------------------------------------------------]
				void Clear(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::Clear* realData = static_cast<const Renderer::Command::Clear*>(data);
					static_cast<OpenGLES2Renderer&>(renderer).clear(realData->flags, realData->color, realData->z, realData->stencil);
				}

				void ResolveMultisampleFramebuffer(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::ResolveMultisampleFramebuffer* realData = static_cast<const Renderer::Command::ResolveMultisampleFramebuffer*>(data);
					static_cast<OpenGLES2Renderer&>(renderer).resolveMultisampleFramebuffer(*realData->destinationRenderTarget, *realData->sourceMultisampleFramebuffer);
				}

				void CopyResource(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::CopyResource* realData = static_cast<const Renderer::Command::CopyResource*>(data);
					static_cast<OpenGLES2Renderer&>(renderer).copyResource(*realData->destinationResource, *realData->sourceResource);
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
						static_cast<OpenGLES2Renderer&>(renderer).drawEmulated(realData->indirectBuffer->getEmulationData(), realData->indirectBufferOffset, realData->numberOfDraws);
					}
					else
					{
						static_cast<OpenGLES2Renderer&>(renderer).drawEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
					}
				}

				void DrawIndexed(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::Draw* realData = static_cast<const Renderer::Command::Draw*>(data);
					if (nullptr != realData->indirectBuffer)
					{
						// No resource owner security check in here, we only support emulated indirect buffer
						static_cast<OpenGLES2Renderer&>(renderer).drawIndexedEmulated(realData->indirectBuffer->getEmulationData(), realData->indirectBufferOffset, realData->numberOfDraws);
					}
					else
					{
						static_cast<OpenGLES2Renderer&>(renderer).drawIndexedEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
					}
				}

				//[-------------------------------------------------------]
				//[ Debug                                                 ]
				//[-------------------------------------------------------]
				void SetDebugMarker(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::SetDebugMarker* realData = static_cast<const Renderer::Command::SetDebugMarker*>(data);
					static_cast<OpenGLES2Renderer&>(renderer).setDebugMarker(realData->name);
				}

				void BeginDebugEvent(const void* data, Renderer::IRenderer& renderer)
				{
					const Renderer::Command::BeginDebugEvent* realData = static_cast<const Renderer::Command::BeginDebugEvent*>(data);
					static_cast<OpenGLES2Renderer&>(renderer).beginDebugEvent(realData->name);
				}

				void EndDebugEvent(const void*, Renderer::IRenderer& renderer)
				{
					static_cast<OpenGLES2Renderer&>(renderer).endDebugEvent();
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
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	OpenGLES2Renderer::OpenGLES2Renderer(handle nativeWindowHandle, bool useExternalContext) :
		mContext(new ContextRuntimeLinking(nativeWindowHandle, useExternalContext)),
		mShaderLanguageGlsl(nullptr),
		mGraphicsRootSignature(nullptr),
		mDefaultSamplerState(nullptr),
		mVertexArray(nullptr),
		mOpenGLES2PrimitiveTopology(0xFFFF),	// Unknown default setting
		mMainSwapChain(nullptr),
		mRenderTarget(nullptr),
		mOpenGLES2Program(0)
	{
		// Initialize the context
		if (mContext->initialize(0))
		{
			// Create the default sampler state
			mDefaultSamplerState = createSamplerState(Renderer::ISamplerState::getDefaultSamplerState());

			// Initialize the capabilities
			initializeCapabilities();

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
					glDebugMessageCallbackKHR(&OpenGLES2Renderer::debugMessageCallback, nullptr);
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

	OpenGLES2Renderer::~OpenGLES2Renderer()
	{
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

		{ // For debugging: At this point there should be no resource instances left, validate this!
			// -> Are the currently any resource instances?
			const unsigned long numberOfCurrentResources = getStatistics().getNumberOfCurrentResources();
			if (numberOfCurrentResources > 0)
			{
				// Error!
				if (numberOfCurrentResources > 1)
				{
					RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL ES 2 error: Renderer is going to be destroyed, but there are still %d resource instances left (memory leak)\n", numberOfCurrentResources)
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Renderer is going to be destroyed, but there is still one resource instance left (memory leak)\n")
				}

				// Use debug output to show the current number of resource instances
				getStatistics().debugOutputCurrentResouces();
			}
		}

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
	void OpenGLES2Renderer::setGraphicsRootSignature(Renderer::IRootSignature* rootSignature)
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
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *rootSignature)
		}
	}

	void OpenGLES2Renderer::setGraphicsRootDescriptorTable(uint32_t rootParameterIndex, Renderer::IResource* resource)
	{
		// Security checks
		#ifndef OPENGLES2RENDERER_NO_DEBUG
		{
			if (nullptr == mGraphicsRootSignature)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: No graphics root signature set")
				return;
			}
			const Renderer::RootSignature& rootSignature = mGraphicsRootSignature->getRootSignature();
			if (rootParameterIndex >= rootSignature.numberOfParameters)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Root parameter index is out of bounds")
				return;
			}
			const Renderer::RootParameter& rootParameter = rootSignature.parameters[rootParameterIndex];
			if (Renderer::RootParameterType::DESCRIPTOR_TABLE != rootParameter.parameterType)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Root parameter index doesn't reference a descriptor table")
				return;
			}

			// TODO(co) For now, we only support a single descriptor range
			if (1 != rootParameter.descriptorTable.numberOfDescriptorRanges)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Only a single descriptor range is supported")
				return;
			}
			if (nullptr == rootParameter.descriptorTable.descriptorRanges)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Descriptor ranges is a null pointer")
				return;
			}
		}
		#endif

		if (nullptr != resource)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *resource)

			// Get the root signature parameter instance
			const Renderer::RootParameter& rootParameter = mGraphicsRootSignature->getRootSignature().parameters[rootParameterIndex];
			const Renderer::DescriptorRange* descriptorRange = rootParameter.descriptorTable.descriptorRanges;

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
				case Renderer::ResourceType::TEXTURE_2D:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					switch (rootParameter.shaderVisibility)
					{
						// In OpenGL ES 2, all shaders share the same texture units
						case Renderer::ShaderVisibility::ALL:
						case Renderer::ShaderVisibility::VERTEX:
						case Renderer::ShaderVisibility::FRAGMENT:
						{
							#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
								// Backup the currently active OpenGL ES 2 texture
								GLint openGLES2ActiveTextureBackup = 0;
								glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLES2ActiveTextureBackup);
							#endif

							// TODO(co) Some security checks might be wise *maximum number of texture units*
							glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + descriptorRange->baseShaderRegister));

							// Bind texture buffer
							if (resourceType == Renderer::ResourceType::TEXTURE_BUFFER)
							{
								glBindTexture(GL_TEXTURE_BUFFER_EXT, static_cast<TextureBuffer*>(resource)->getOpenGLESTexture());
							}
							// Bind texture
							else if (resourceType == Renderer::ResourceType::TEXTURE_2D_ARRAY)
							{
								// No extension check required, if we in here we already know it must exist
								glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, static_cast<Texture2DArray*>(resource)->getOpenGLES2Texture());
							}
							else
							{
								glBindTexture(GL_TEXTURE_2D, static_cast<Texture2D*>(resource)->getOpenGLES2Texture());
							}

							if (Renderer::ResourceType::TEXTURE_BUFFER != resourceType)
							{
								// Set the OpenGL ES 2 sampler states
								mGraphicsRootSignature->setOpenGLES2SamplerStates(descriptorRange->samplerRootParameterIndex);
							}

							#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
								// Be polite and restore the previous active OpenGL ES 2 texture
								glActiveTexture(openGLES2ActiveTextureBackup);
							#endif
							break;
						}

						case Renderer::ShaderVisibility::TESSELLATION_CONTROL:
							RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2: OpenGL ES 2 has no tessellation control shader support (hull shader in Direct3D terminology)")
							break;

						case Renderer::ShaderVisibility::TESSELLATION_EVALUATION:
							RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: OpenGL ES 2 has no tessellation evaluation shader support (domain shader in Direct3D terminology)")
							break;

						case Renderer::ShaderVisibility::GEOMETRY:
							RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: OpenGL ES 2 has no geometry shader support")
							break;
					}
					break;
				}

				case Renderer::ResourceType::SAMPLER_STATE:
				{
					// Unlike Direct3D >=10, OpenGL ES 2 directly attaches the sampler settings to the texture
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
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: Invalid resource type")
					break;
			}
		}
		else
		{
			// TODO(co) Handle this situation?
			/*
			#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
				// Backup the currently active OpenGL ES 2 texture
				GLint openGLES2ActiveTextureBackup = 0;
				glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLES2ActiveTextureBackup);
			#endif

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			glActiveTexture(GL_TEXTURE0 + unit);

			// Unbind the texture at the given texture unit
			glBindTexture(GL_TEXTURE_2D, 0);

			#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
				// Be polite and restore the previous active OpenGL ES 2 texture
				glActiveTexture(openGLES2ActiveTextureBackup);
			#endif
			*/
		}
	}

	void OpenGLES2Renderer::setPipelineState(Renderer::IPipelineState* pipelineState)
	{
		if (nullptr != pipelineState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *pipelineState)

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
	void OpenGLES2Renderer::iaSetVertexArray(Renderer::IVertexArray *vertexArray)
	{
		// New vertex array?
		if (mVertexArray != vertexArray)
		{
			// Set a vertex array?
			if (nullptr != vertexArray)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *vertexArray)

				// Is GL_OES_vertex_array_object there?
				if (mContext->getExtensions().isGL_OES_vertex_array_object())
				{
					// Release the vertex array reference, in case we have one
					if (nullptr != mVertexArray)
					{
						// Release reference
						mVertexArray->releaseReference();
					}

					// Set new vertex array and add a reference to it
					mVertexArray = static_cast<VertexArray*>(vertexArray);
					mVertexArray->addReference();

					// Bind OpenGL ES 2 vertex array
					glBindVertexArrayOES(static_cast<VertexArrayVao*>(mVertexArray)->getOpenGLES2VertexArray());
				}
				else
				{
					// Release the vertex array reference, in case we have one
					if (nullptr != mVertexArray)
					{
						// Disable OpenGL ES 2 vertex attribute arrays
						static_cast<VertexArrayNoVao*>(mVertexArray)->disableOpenGLES2VertexAttribArrays();

						// Release reference
						mVertexArray->releaseReference();
					}

					// Set new vertex array and add a reference to it
					mVertexArray = static_cast<VertexArray*>(vertexArray);
					mVertexArray->addReference();

					// Enable OpenGL ES 2 vertex attribute arrays
					static_cast<VertexArrayNoVao*>(mVertexArray)->enableOpenGLES2VertexAttribArrays();
				}
			}
			else
			{
				// Release the vertex array reference, in case we have one
				if (nullptr != mVertexArray)
				{
					// Disable OpenGL ES 2 vertex attribute arrays - Is the "GL_OES_vertex_array_object" extension there?
					if (mContext->getExtensions().isGL_OES_vertex_array_object())
					{
						// Unbind OpenGL ES 2 vertex array
						glBindVertexArrayOES(0);
					}
					else
					{
						// Traditional version
						static_cast<VertexArrayNoVao*>(mVertexArray)->disableOpenGLES2VertexAttribArrays();
					}

					// Release reference
					mVertexArray->releaseReference();
					mVertexArray = nullptr;
				}
			}
		}
	}

	void OpenGLES2Renderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology primitiveTopology)
	{
		// Map and backup the set OpenGL ES 2 primitive topology
		mOpenGLES2PrimitiveTopology = Mapping::getOpenGLES2Type(primitiveTopology);
	}


	//[-------------------------------------------------------]
	//[ Rasterizer (RS) stage                                 ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport *viewports)
	{
		// Are the given viewports valid?
		if (numberOfViewports > 0 && nullptr != viewports)
		{
			// In OpenGL ES 2, the origin of the viewport is left bottom while Direct3D is using a left top origin. To make the
			// Direct3D 11 implementation as efficient as possible the Direct3D convention is used and we have to convert in here.

			// Get the width and height of the current render target
			uint32_t renderTargetWidth =  1;
			uint32_t renderTargetHeight = 1;
			if (nullptr != mRenderTarget)
			{
				mRenderTarget->getWidthAndHeight(renderTargetWidth, renderTargetHeight);
			}

			// Set the OpenGL ES 2 viewport
			// -> OpenGL ES 2 supports only one viewport
		#ifndef RENDERER_NO_DEBUG
			if (numberOfViewports > 1)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: OpenGL ES 2 supports only one viewport")
			}
		#endif
			glViewport(static_cast<GLint>(viewports->topLeftX), static_cast<GLint>(renderTargetHeight - viewports->topLeftY - viewports->height), static_cast<GLsizei>(viewports->width), static_cast<GLsizei>(viewports->height));
			glDepthRangef(static_cast<GLclampf>(viewports->minDepth), static_cast<GLclampf>(viewports->maxDepth));
		}
	}

	void OpenGLES2Renderer::rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles)
	{
		// Are the given scissor rectangles valid?
		if (numberOfScissorRectangles > 0 && nullptr != scissorRectangles)
		{
			// In OpenGL ES 2, the origin of the scissor rectangle is left bottom while Direct3D is using a left top origin. To make the
			// Direct3D 9 & 10 & 11 implementation as efficient as possible the Direct3D convention is used and we have to convert in here.

			// Get the width and height of the current render target
			uint32_t renderTargetWidth =  1;
			uint32_t renderTargetHeight = 1;
			if (nullptr != mRenderTarget)
			{
				mRenderTarget->getWidthAndHeight(renderTargetWidth, renderTargetHeight);
			}

			// Set the OpenGL ES 2 scissor rectangle
		#ifndef RENDERER_NO_DEBUG
			if (numberOfScissorRectangles > 1)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: OpenGL ES 2 supports only one scissor rectangle")
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
	void OpenGLES2Renderer::omSetRenderTarget(Renderer::IRenderTarget *renderTarget)
	{
		// New render target?
		if (mRenderTarget != renderTarget)
		{
			// Set a render target?
			if (nullptr != renderTarget)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *renderTarget)

				// Release the render target reference, in case we have one
				Framebuffer* framebufferToGenerateMipmapsFor = nullptr;
				if (nullptr != mRenderTarget)
				{
					// Unbind OpenGL ES 2 framebuffer?
					if (Renderer::ResourceType::FRAMEBUFFER == mRenderTarget->getResourceType() && Renderer::ResourceType::FRAMEBUFFER != renderTarget->getResourceType())
					{
						// We do not render into a OpenGL ES 2 framebuffer
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
						// Get the OpenGL ES 2 framebuffer instance
						Framebuffer *framebuffer = static_cast<Framebuffer*>(mRenderTarget);

						// Bind the OpenGL ES 2 framebuffer
						glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->getOpenGLES2Framebuffer());

						// Define the OpenGL buffers to draw into, "GL_NV_draw_buffers"-extension required
						if (mContext->getExtensions().isGL_NV_draw_buffers())
						{
							// https://www.opengl.org/registry/specs/ARB/draw_buffers.txt - "The draw buffer for output colors beyond <n> is set to NONE."
							// -> Meaning depth only rendering which has no color textures at all will work as well, no need for "glDrawBuffer(GL_NONE)"
							// -> https://www.khronos.org/opengles/sdk/docs/man3/html/glDrawBuffers.xhtml for OpenGL ES 3 specifies the same behaviour
							// -> "GL_COLOR_ATTACHMENT0" and "GL_COLOR_ATTACHMENT0_NV" have the same value
							static const GLenum OPENGL_DRAW_BUFFER[16] =
							{
								GL_COLOR_ATTACHMENT0_NV,  GL_COLOR_ATTACHMENT1_NV,  GL_COLOR_ATTACHMENT2_NV,  GL_COLOR_ATTACHMENT3_NV,
								GL_COLOR_ATTACHMENT4_NV,  GL_COLOR_ATTACHMENT5_NV,  GL_COLOR_ATTACHMENT6_NV,  GL_COLOR_ATTACHMENT7_NV,
								GL_COLOR_ATTACHMENT8_NV,  GL_COLOR_ATTACHMENT9_NV,  GL_COLOR_ATTACHMENT10_NV, GL_COLOR_ATTACHMENT11_NV,
								GL_COLOR_ATTACHMENT12_NV, GL_COLOR_ATTACHMENT13_NV, GL_COLOR_ATTACHMENT14_NV, GL_COLOR_ATTACHMENT15_NV
							};
							glDrawBuffersNV(static_cast<GLsizei>(framebuffer->getNumberOfColorTextures()), OPENGL_DRAW_BUFFER);
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

				// Generate mipmaps
				if (nullptr != framebufferToGenerateMipmapsFor)
				{
					framebufferToGenerateMipmapsFor->generateMipmaps();
					framebufferToGenerateMipmapsFor->releaseReference();
				}
			}
			else
			{
				// Evaluate the render target type
				if (Renderer::ResourceType::FRAMEBUFFER == mRenderTarget->getResourceType())
				{
					// We do not render into a OpenGL ES 2 framebuffer
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
				}

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
	void OpenGLES2Renderer::clear(uint32_t flags, const float color[4], float z, uint32_t stencil)
	{
		// Get API flags
		uint32_t flagsAPI = 0;
		if (flags & Renderer::ClearFlag::COLOR)
		{
			flagsAPI |= GL_COLOR_BUFFER_BIT;
		}
		if (flags & Renderer::ClearFlag::DEPTH)
		{
			flagsAPI |= GL_DEPTH_BUFFER_BIT;
		}
		if (flags & Renderer::ClearFlag::STENCIL)
		{
			flagsAPI |= GL_STENCIL_BUFFER_BIT;
		}

		// Are API flags set?
		if (flagsAPI)
		{
			// Set clear settings
			if (flags & Renderer::ClearFlag::COLOR)
			{
				glClearColor(color[0], color[1], color[2], color[3]);
			}
			if (flags & Renderer::ClearFlag::DEPTH)
			{
				glClearDepthf(z);
			}
			if (flags & Renderer::ClearFlag::STENCIL)
			{
				glClearStencil(static_cast<GLint>(stencil));
			}

			// Unlike OpenGL ES 2, when using Direct3D 10 & 11 the scissor rectangle(s) do not affect the clear operation
			// -> We have to compensate the OpenGL ES 2 behaviour in here

			// Disable OpenGL scissor test, in case it's not disabled, yet
			// TODO(co) Pipeline state update
			// if (mRasterizerState->getRasterizerState().scissorEnable)
			{
				glDisable(GL_SCISSOR_TEST);
			}

			// Clear
			glClear(flagsAPI);

			// Restore the previously set OpenGL viewport
			// TODO(co) Pipeline state update
			// if (mRasterizerState->getRasterizerState().scissorEnable)
			{
				glEnable(GL_SCISSOR_TEST);
			}
		}
	}

	void OpenGLES2Renderer::resolveMultisampleFramebuffer(Renderer::IRenderTarget&, Renderer::IFramebuffer&)
	{
		// TODO(co) Implement me
	}

	void OpenGLES2Renderer::copyResource(Renderer::IResource& destinationResource, Renderer::IResource& sourceResource)
	{
		// Security check: Are the given resources owned by this renderer? (calls "return" in case of a mismatch)
		OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, destinationResource)
		OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, sourceResource)

		// Evaluate the render target type
		switch (destinationResource.getResourceType())
		{
			case Renderer::ResourceType::TEXTURE_2D:
				if (sourceResource.getResourceType() == Renderer::ResourceType::TEXTURE_2D)
				{
					// Get the OpenGL ES 2 texture 2D instances
					// const Texture2D& openGlEs2DestinationTexture2D = static_cast<const Texture2D&>(destinationResource);
					// const Texture2D& openGlEs2SourceTexture2D = static_cast<const Texture2D&>(sourceResource);

					// TODO(co) Copy resource
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


	//[-------------------------------------------------------]
	//[ Draw call                                             ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::drawEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
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
					glDrawArraysInstanced(mOpenGLES2PrimitiveTopology, static_cast<GLint>(drawInstancedArguments.startVertexLocation), static_cast<GLsizei>(drawInstancedArguments.vertexCountPerInstance), static_cast<GLsizei>(drawInstancedArguments.instanceCount));
				}
				else
				{
					// Without instancing
					assert(drawInstancedArguments.instanceCount <= 1);
					glDrawArrays(mOpenGLES2PrimitiveTopology, static_cast<GLint>(drawInstancedArguments.startVertexLocation), static_cast<GLsizei>(drawInstancedArguments.vertexCountPerInstance));
				}
				emulationData += sizeof(Renderer::DrawInstancedArguments);
			}
		}
	}

	void OpenGLES2Renderer::drawIndexedEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
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
								glDrawElementsInstancedBaseVertexEXT(mOpenGLES2PrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLES2Type(), reinterpret_cast<const GLvoid*>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes()), static_cast<GLsizei>(drawIndexedInstancedArguments.instanceCount), static_cast<GLint>(drawIndexedInstancedArguments.baseVertexLocation));
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
							glDrawElementsInstanced(mOpenGLES2PrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLES2Type(), reinterpret_cast<const GLvoid*>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes()), static_cast<GLsizei>(drawIndexedInstancedArguments.instanceCount));
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
								glDrawElementsBaseVertexEXT(mOpenGLES2PrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLES2Type(), reinterpret_cast<const GLvoid*>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes()), static_cast<GLint>(drawIndexedInstancedArguments.baseVertexLocation));
							}
							else
							{
								// Error!
							}
						}
						else
						{
							// Draw and advance
							glDrawElements(mOpenGLES2PrimitiveTopology, static_cast<GLsizei>(drawIndexedInstancedArguments.indexCountPerInstance), indexBuffer->getOpenGLES2Type(), reinterpret_cast<const GLvoid*>(drawIndexedInstancedArguments.startIndexLocation * indexBuffer->getIndexSizeInBytes()));
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
	void OpenGLES2Renderer::setDebugMarker(const wchar_t *)
	{
		// OpenGL ES 2 has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)
	}

	void OpenGLES2Renderer::beginDebugEvent(const wchar_t *)
	{
		// OpenGL ES 2 has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)
	}

	void OpenGLES2Renderer::endDebugEvent()
	{
		// OpenGL ES 2 has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	const char *OpenGLES2Renderer::getName() const
	{
		return "OpenGLES2";
	}

	bool OpenGLES2Renderer::isInitialized() const
	{
		// Is the context initialized?
		return  mContext->isInitialized();
	}

	bool OpenGLES2Renderer::isDebugEnabled()
	{
		// OpenGL ES 2 has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)

		// Debug disabled
		return false;
	}

	Renderer::ISwapChain *OpenGLES2Renderer::getMainSwapChain() const
	{
		return mMainSwapChain;
	}


	//[-------------------------------------------------------]
	//[ Shader language                                       ]
	//[-------------------------------------------------------]
	uint32_t OpenGLES2Renderer::getNumberOfShaderLanguages() const
	{
		return 1;
	}

	const char *OpenGLES2Renderer::getShaderLanguageName(uint32_t index) const
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

	Renderer::IShaderLanguage *OpenGLES2Renderer::getShaderLanguage(const char *shaderLanguageName)
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
	Renderer::ISwapChain *OpenGLES2Renderer::createSwapChain(handle nativeWindowHandle)
	{
		// The provided native window handle must not be a null handle
		return (NULL_HANDLE != nativeWindowHandle) ? new SwapChain(*this, nativeWindowHandle) : nullptr;
	}

	Renderer::IFramebuffer *OpenGLES2Renderer::createFramebuffer(uint32_t numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture)
	{
		// Validation is done inside the framebuffer implementation
		return new Framebuffer(*this, numberOfColorTextures, colorTextures, depthStencilTexture);
	}

	Renderer::IBufferManager *OpenGLES2Renderer::createBufferManager()
	{
		return new BufferManager(*this);
	}

	Renderer::ITextureManager *OpenGLES2Renderer::createTextureManager()
	{
		return new TextureManager(*this);
	}

	Renderer::IRootSignature *OpenGLES2Renderer::createRootSignature(const Renderer::RootSignature &rootSignature)
	{
		return new RootSignature(*this, rootSignature);
	}

	Renderer::IPipelineState *OpenGLES2Renderer::createPipelineState(const Renderer::PipelineState& pipelineState)
	{
		return new PipelineState(*this, pipelineState);
	}

	Renderer::ISamplerState *OpenGLES2Renderer::createSamplerState(const Renderer::SamplerState &samplerState)
	{
		return new SamplerState(*this, samplerState);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool OpenGLES2Renderer::map(Renderer::IResource& resource, uint32_t, Renderer::MapType mapType, uint32_t, Renderer::MappedSubresource& mappedSubresource)
	{
		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
			{
				// TODO(co) This buffer update isn't efficient, use e.g. persistent buffer mapping

				#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
					// Backup the currently bound OpenGL ES 2 array element buffer
					GLint openGLES2ArrayElementBufferBackup = 0;
					glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &openGLES2ArrayElementBufferBackup);
				#endif

				// Bind this OpenGL ES 2 element buffer and upload the data
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<IndexBuffer&>(resource).getOpenGLES2ElementArrayBuffer());

				// Map
				if (mContext->getExtensions().isGL_OES_mapbuffer())
				{
					mappedSubresource.data = glMapBufferOES(GL_ELEMENT_ARRAY_BUFFER, Mapping::getOpenGLES2MapType(mapType));
				}
				else
				{
					mappedSubresource.data = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(static_cast<IndexBuffer&>(resource).getBufferSize()), Mapping::getOpenGLES2MapRangeType(mapType));
				}
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;

				#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL ES 2 array element buffer
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGLES2ArrayElementBufferBackup);
				#endif

				// Done
				return true;
			}

			case Renderer::ResourceType::VERTEX_BUFFER:
			{
				// TODO(co) This buffer update isn't efficient, use e.g. persistent buffer mapping

				#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
					// Backup the currently bound OpenGL ES 2 array buffer
					GLint openGLES2ArrayBufferBackup = 0;
					glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &openGLES2ArrayBufferBackup);
				#endif

				// Bind this OpenGL ES 2 array buffer and upload the data
				glBindBuffer(GL_ARRAY_BUFFER, static_cast<VertexBuffer&>(resource).getOpenGLES2ArrayBuffer());

				// Map
				if (mContext->getExtensions().isGL_OES_mapbuffer())
				{
					mappedSubresource.data = glMapBufferOES(GL_ARRAY_BUFFER, Mapping::getOpenGLES2MapType(mapType));
				}
				else
				{
					mappedSubresource.data = glMapBufferRange(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(static_cast<VertexBuffer&>(resource).getBufferSize()), Mapping::getOpenGLES2MapRangeType(mapType));
				}
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;

				#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL ES 2 array buffer
					glBindBuffer(GL_ARRAY_BUFFER, openGLES2ArrayBufferBackup);
				#endif

				// Done
				return true;
			}

			case Renderer::ResourceType::UNIFORM_BUFFER:
				// OpenGL ES 2 has no uniform buffer
				// TODO(co) Error handling
				return false;

			case Renderer::ResourceType::TEXTURE_BUFFER:
				// OpenGL ES 2 has no texture buffer
				// TODO(co) Error handling
				return false;

			case Renderer::ResourceType::INDIRECT_BUFFER:
				mappedSubresource.data		 = static_cast<IndirectBuffer&>(resource).getWritableEmulationData();
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;
				return true;

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

	void OpenGLES2Renderer::unmap(Renderer::IResource& resource, uint32_t)
	{
		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
			{
				#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
					// Backup the currently bound OpenGL ES 2 array element buffer
					GLint openGLES2ArrayElementBufferBackup = 0;
					glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &openGLES2ArrayElementBufferBackup);
				#endif

				// Bind this OpenGL ES 2 element buffer and upload the data
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<IndexBuffer&>(resource).getOpenGLES2ElementArrayBuffer());

				// Unmap
				if (mContext->getExtensions().isGL_OES_mapbuffer())
				{
					glUnmapBufferOES(GL_ELEMENT_ARRAY_BUFFER);
				}
				else
				{
					glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
				}

				#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL ES 2 array element buffer
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGLES2ArrayElementBufferBackup);
				#endif
				break;
			}

			case Renderer::ResourceType::VERTEX_BUFFER:
			{
				#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
					// Backup the currently bound OpenGL ES 2 array buffer
					GLint openGLES2ArrayBufferBackup = 0;
					glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &openGLES2ArrayBufferBackup);
				#endif

				// Bind this OpenGL ES 2 array buffer and upload the data
				glBindBuffer(GL_ARRAY_BUFFER, static_cast<VertexBuffer&>(resource).getOpenGLES2ArrayBuffer());

				// Unmap
				if (mContext->getExtensions().isGL_OES_mapbuffer())
				{
					glUnmapBufferOES(GL_ARRAY_BUFFER);
				}
				else
				{
					glUnmapBuffer(GL_ARRAY_BUFFER);
				}

				#ifndef OPENGLES2RENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL ES 2 array buffer
					glBindBuffer(GL_ARRAY_BUFFER, openGLES2ArrayBufferBackup);
				#endif
				break;
			}

			case Renderer::ResourceType::UNIFORM_BUFFER:
				// OpenGL ES 2 has no uniform buffer
				// TODO(co) Error handling
				break;

			case Renderer::ResourceType::TEXTURE_BUFFER:
				// OpenGL ES 2 has no texture buffer
				// TODO(co) Error handling
				break;

			case Renderer::ResourceType::INDIRECT_BUFFER:
				// Nothing here, it's a software emulated indirect buffer
				break;

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
	bool OpenGLES2Renderer::beginScene()
	{
		// Not required when using OpenGL ES 2

		// Done
		return true;
	}

	void OpenGLES2Renderer::submitCommandBuffer(const Renderer::CommandBuffer& commandBuffer)
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

	void OpenGLES2Renderer::endScene()
	{
		// We need to forget about the currently set render target
		omSetRenderTarget(nullptr);

		// We need to forget about the currently set vertex array
		iaSetVertexArray(nullptr);
	}


	//[-------------------------------------------------------]
	//[ Synchronization                                       ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::flush()
	{
		glFlush();
	}

	void OpenGLES2Renderer::finish()
	{
		glFinish();
	}


	//[-------------------------------------------------------]
	//[ Private static methods                                ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::debugMessageCallback(uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int, const char *message, const void *)
	{
		// Source to string
		char debugSource[20]{0};
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
		char debugType[25]{0};
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
		char debugSeverity[20]{0};
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
		#ifdef _DEBUG
			RENDERER_OUTPUT_DEBUG_PRINTF("OpenGLES error: OpenGL debug message\tSource:\"%s\"\tType:\"%s\"\tID:\"%d\"\tSeverity:\"%s\"\tMessage:\"%s\"\n", debugSource, debugType, id, debugSeverity, message)
		#else
			// Avoid "warning C4100: '<x>' : unreferenced formal parameter"-warning
			id = id;
			message = message;
		#endif
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::initializeCapabilities()
	{
		GLint openGLValue = 0;

		// Maximum number of viewports (always at least 1)
		mCapabilities.maximumNumberOfViewports = 1;	// OpenGL ES 2 only supports a single viewport

		// Maximum number of simultaneous render targets (if <1 render to texture is not supported, "GL_NV_draw_buffers" extension required)
		if (mContext->getExtensions().isGL_NV_draw_buffers())
		{
			glGetIntegerv(GL_MAX_DRAW_BUFFERS_NV, &openGLValue);
			mCapabilities.maximumNumberOfSimultaneousRenderTargets = static_cast<uint32_t>(openGLValue);
		}
		else
		{
			mCapabilities.maximumNumberOfSimultaneousRenderTargets = 1;
		}

		// Maximum texture dimension
		openGLValue = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &openGLValue);
		mCapabilities.maximumTextureDimension = static_cast<uint32_t>(openGLValue);

		// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
		if (mContext->getExtensions().isGL_EXT_texture_array())
		{
			glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS_EXT, &openGLValue);
			mCapabilities.maximumNumberOf2DTextureArraySlices = static_cast<uint32_t>(openGLValue);
		}
		else
		{
			mCapabilities.maximumNumberOf2DTextureArraySlices = 0;
		}

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
			mCapabilities.maximumTextureBufferSize = 0;
		}

		// Maximum indirect buffer size in bytes (in case there's no support for indirect buffer it's 0)
		mCapabilities.maximumIndirectBufferSize = sizeof(Renderer::DrawIndexedInstancedArguments) * 4096;	// TODO(co) What is an usually decent emulated indirect buffer size?

		// Maximum number of multisamples (always at least 1, usually 8)
		mCapabilities.maximumNumberOfMultisamples = 1;	// Don't want to support the legacy OpenGL ES 2 multisample support

		// Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		mCapabilities.individualUniforms = true;

		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
		mCapabilities.instancedArrays = true;	// Is core feature in OpenGL ES 3.0

		// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID, OpenGL ES 2 has no "GL_ARB_draw_instanced" extension)
		mCapabilities.drawInstanced = true;	// Is core feature in OpenGL ES 3.0

		// Base vertex supported for draw calls?
		mCapabilities.baseVertex = mContext->getExtensions().isGL_EXT_draw_elements_base_vertex();

		// OpenGL ES 2 has no native multi-threading
		mCapabilities.nativeMultiThreading = false;

		// Is there support for vertex shaders (VS)?
		mCapabilities.vertexShader = true;

		// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
		mCapabilities.maximumNumberOfPatchVertices = 0;	// OpenGL ES 2 has no tessellation support

		// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
		mCapabilities.maximumNumberOfGsOutputVertices = 0;	// OpenGL ES 2 has no support for geometry shaders

		// Is there support for fragment shaders (FS)?
		mCapabilities.fragmentShader = true;
	}

	void OpenGLES2Renderer::setProgram(Renderer::IProgram *program)
	{
		if (nullptr != program)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN(*this, *program)

			// Backup OpenGL ES 2 program identifier
			mOpenGLES2Program = static_cast<ProgramGlsl*>(program)->getOpenGLES2Program();

			// Bind the program
			glUseProgram(mOpenGLES2Program);
		}
		else
		{
			// Unbind the program
			glUseProgram(0);

			// Reset OpenGL ES 2 program identifier
			mOpenGLES2Program = 0;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
