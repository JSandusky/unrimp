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
#include "Direct3D9Renderer/Direct3D9Renderer.h"
#include "Direct3D9Renderer/d3d9.h"				// For "DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "Direct3D9Renderer/Direct3D9Debug.h"	// For "DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "Direct3D9Renderer/Direct3D9RuntimeLinking.h"
#include "Direct3D9Renderer/RootSignature.h"
#include "Direct3D9Renderer/RenderTarget/SwapChain.h"
#include "Direct3D9Renderer/RenderTarget/Framebuffer.h"
#include "Direct3D9Renderer/Buffer/BufferManager.h"
#include "Direct3D9Renderer/Buffer/VertexArray.h"
#include "Direct3D9Renderer/Buffer/IndexBuffer.h"
#include "Direct3D9Renderer/Buffer/VertexBuffer.h"
#include "Direct3D9Renderer/Buffer/IndirectBuffer.h"
#include "Direct3D9Renderer/Texture/TextureManager.h"
#include "Direct3D9Renderer/Texture/Texture1D.h"
#include "Direct3D9Renderer/Texture/Texture2D.h"
#include "Direct3D9Renderer/Texture/Texture3D.h"
#include "Direct3D9Renderer/Texture/TextureCube.h"
#include "Direct3D9Renderer/State/SamplerState.h"
#include "Direct3D9Renderer/State/PipelineState.h"
#include "Direct3D9Renderer/Shader/ProgramHlsl.h"
#include "Direct3D9Renderer/Shader/VertexShaderHlsl.h"
#include "Direct3D9Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D9Renderer/Shader/FragmentShaderHlsl.h"

#include <Renderer/ILog.h>
#include <Renderer/Buffer/CommandBuffer.h>
#include <Renderer/Buffer/IndirectBufferTypes.h>


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef DIRECT3D9RENDERER_EXPORTS
	#define DIRECT3D9RENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define DIRECT3D9RENDERER_API_EXPORT
#endif
DIRECT3D9RENDERER_API_EXPORT Renderer::IRenderer* createDirect3D9RendererInstance(const Renderer::Context& context)
{
	return new Direct3D9Renderer::Direct3D9Renderer(context);
}
#undef DIRECT3D9RENDERER_API_EXPORT


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
				// Not supported by Direct3D 9
				assert(false);
			}

			void CopyTextureBufferData(const void*, Renderer::IRenderer&)
			{
				// Not supported by Direct3D 9
				assert(false);
			}

			//[-------------------------------------------------------]
			//[ Graphics root                                         ]
			//[-------------------------------------------------------]
			void SetGraphicsRootSignature(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetGraphicsRootSignature* realData = static_cast<const Renderer::Command::SetGraphicsRootSignature*>(data);
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).setGraphicsRootSignature(realData->rootSignature);
			}

			void SetGraphicsRootDescriptorTable(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetGraphicsRootDescriptorTable* realData = static_cast<const Renderer::Command::SetGraphicsRootDescriptorTable*>(data);
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).setGraphicsRootDescriptorTable(realData->rootParameterIndex, realData->resource);
			}

			//[-------------------------------------------------------]
			//[ States                                                ]
			//[-------------------------------------------------------]
			void SetPipelineState(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetPipelineState* realData = static_cast<const Renderer::Command::SetPipelineState*>(data);
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).setPipelineState(realData->pipelineState);
			}

			//[-------------------------------------------------------]
			//[ Input-assembler (IA) stage                            ]
			//[-------------------------------------------------------]
			void SetVertexArray(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetVertexArray* realData = static_cast<const Renderer::Command::SetVertexArray*>(data);
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).iaSetVertexArray(realData->vertexArray);
			}

			void SetPrimitiveTopology(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetPrimitiveTopology* realData = static_cast<const Renderer::Command::SetPrimitiveTopology*>(data);
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).iaSetPrimitiveTopology(realData->primitiveTopology);
			}

			//[-------------------------------------------------------]
			//[ Rasterizer (RS) stage                                 ]
			//[-------------------------------------------------------]
			void SetViewports(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetViewports* realData = static_cast<const Renderer::Command::SetViewports*>(data);
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).rsSetViewports(realData->numberOfViewports, (nullptr != realData->viewports) ? realData->viewports : reinterpret_cast<const Renderer::Viewport*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
			}

			void SetScissorRectangles(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetScissorRectangles* realData = static_cast<const Renderer::Command::SetScissorRectangles*>(data);
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).rsSetScissorRectangles(realData->numberOfScissorRectangles, (nullptr != realData->scissorRectangles) ? realData->scissorRectangles : reinterpret_cast<const Renderer::ScissorRectangle*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
			}

			//[-------------------------------------------------------]
			//[ Output-merger (OM) stage                              ]
			//[-------------------------------------------------------]
			void SetRenderTarget(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetRenderTarget* realData = static_cast<const Renderer::Command::SetRenderTarget*>(data);
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).omSetRenderTarget(realData->renderTarget);
			}

			//[-------------------------------------------------------]
			//[ Operations                                            ]
			//[-------------------------------------------------------]
			void Clear(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Clear* realData = static_cast<const Renderer::Command::Clear*>(data);
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).clear(realData->flags, realData->color, realData->z, realData->stencil);
			}

			void ResolveMultisampleFramebuffer(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::ResolveMultisampleFramebuffer* realData = static_cast<const Renderer::Command::ResolveMultisampleFramebuffer*>(data);
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).resolveMultisampleFramebuffer(*realData->destinationRenderTarget, *realData->sourceMultisampleFramebuffer);
			}

			void CopyResource(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::CopyResource* realData = static_cast<const Renderer::Command::CopyResource*>(data);
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).copyResource(*realData->destinationResource, *realData->sourceResource);
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
					static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).drawEmulated(realData->indirectBuffer->getEmulationData(), realData->indirectBufferOffset, realData->numberOfDraws);
				}
				else
				{
					static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).drawEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
				}
			}

			void DrawIndexed(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Draw* realData = static_cast<const Renderer::Command::Draw*>(data);
				if (nullptr != realData->indirectBuffer)
				{
					// No resource owner security check in here, we only support emulated indirect buffer
					static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).drawIndexedEmulated(realData->indirectBuffer->getEmulationData(), realData->indirectBufferOffset, realData->numberOfDraws);
				}
				else
				{
					static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).drawIndexedEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
				}
			}

			//[-------------------------------------------------------]
			//[ Debug                                                 ]
			//[-------------------------------------------------------]
			void SetDebugMarker(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetDebugMarker* realData = static_cast<const Renderer::Command::SetDebugMarker*>(data);
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).setDebugMarker(realData->name);
			}

			void BeginDebugEvent(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::BeginDebugEvent* realData = static_cast<const Renderer::Command::BeginDebugEvent*>(data);
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).beginDebugEvent(realData->name);
			}

			void EndDebugEvent(const void*, Renderer::IRenderer& renderer)
			{
				static_cast<Direct3D9Renderer::Direct3D9Renderer&>(renderer).endDebugEvent();
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
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Direct3D9Renderer::Direct3D9Renderer(const Renderer::Context& context) :
		IRenderer(context),
		mDirect3D9RuntimeLinking(nullptr),
		mDirect3D9(nullptr),
		mDirect3DDevice9(nullptr),
		mShaderLanguageHlsl(nullptr),
		mDirect3DQuery9Flush(nullptr),
		mGraphicsRootSignature(nullptr),
		mDefaultSamplerState(nullptr),
		// Input-assembler (IA) stage
		mPrimitiveTopology(Renderer::PrimitiveTopology::UNKNOWN),
		// Output-merger (OM) stage
		mMainSwapChain(nullptr),
		mRenderTarget(nullptr),
		// State cache to avoid making redundant Direct3D 9 calls
		mDirect3DVertexShader9(nullptr),
		mDirect3DPixelShader9(nullptr)
	{
		// Is Direct3D 9 available?
		mDirect3D9RuntimeLinking = new Direct3D9RuntimeLinking(*this);
		if (mDirect3D9RuntimeLinking->isDirect3D9Avaiable())
		{
			// Begin debug event
			// -> Must be done in here because "Direct3D9Renderer::beginDebugEvent()" is using directly "D3DPERF_BeginEvent()" without loading the function entry point
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

			// Create the Direct3D instance
			mDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
			if (nullptr != mDirect3D9)
			{
				// Set up the structure used to create the D3DDevice instance
				// -> It appears that receiving and manually accessing the automatic depth stencil surface instance is not possible, so, we don't use the automatic depth stencil thing
				D3DPRESENT_PARAMETERS d3dPresentParameters = {};
				d3dPresentParameters.BackBufferWidth		= 1;
				d3dPresentParameters.BackBufferHeight		= 1;
				d3dPresentParameters.BackBufferCount		= 1;
				d3dPresentParameters.SwapEffect				= D3DSWAPEFFECT_DISCARD;
				d3dPresentParameters.Windowed				= TRUE;
				d3dPresentParameters.EnableAutoDepthStencil = FALSE;

				// Create the Direct3D 9 device instance
				// -> In Direct3D 9, there is always at least one swap chain for each device, known as the implicit swap chain
				// -> The size of the swap chain can be changed by using "IDirect3DDevice9::Reset()"...but this results in a
				//    loss of ALL resources and EVERYTHING has to be rebuild and configured from scratch!
				// -> We really don't want to use the implicit swap chain, so we're creating a tiny one (because we have to!)
				//    and then using "IDirect3DDevice9::CreateAdditionalSwapChain()" later on for the real main swap chain
				mDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL_HANDLE, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dPresentParameters, &mDirect3DDevice9);
				if (nullptr != mDirect3DDevice9)
				{
					#ifdef DIRECT3D9RENDERER_NO_DEBUG
						// Disable debugging
						D3DPERF_SetOptions(1);
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
				else
				{
					RENDERER_LOG(mContext, CRITICAL, "Failed to create the Direct3D 9 device instance")
				}
			}
			else
			{
				RENDERER_LOG(mContext, CRITICAL, "Failed to create the Direct3D 9 instance")
			}

			// End debug event
			RENDERER_END_DEBUG_EVENT(this)
		}
	}

	Direct3D9Renderer::~Direct3D9Renderer()
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

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

		// Release the graphics root signature instance
		if (nullptr != mGraphicsRootSignature)
		{
			mGraphicsRootSignature->releaseReference();
			mGraphicsRootSignature = nullptr;
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
					RENDERER_LOG(mContext, CRITICAL, "The Direct3D 9 renderer backend is going to be destroyed, but there are still %d resource instances left (memory leak)", numberOfCurrentResources)
				}
				else
				{
					RENDERER_LOG(mContext, CRITICAL, "The Direct3D 9 renderer backend is going to be destroyed, but there is still one resource instance left (memory leak)")
				}

				// Use debug output to show the current number of resource instances
				getStatistics().debugOutputCurrentResouces(mContext);
			}
		}
		#endif

		// Release the Direct3D 9 query instance used for flush, in case we have one
		if (nullptr != mDirect3DQuery9Flush)
		{
			mDirect3DQuery9Flush->Release();
		}

		// Release the HLSL shader language instance, in case we have one
		if (nullptr != mShaderLanguageHlsl)
		{
			mShaderLanguageHlsl->releaseReference();
		}

		// Release the Direct3D 9 device we've created
		if (nullptr != mDirect3DDevice9)
		{
			mDirect3DDevice9->Release();
			mDirect3DDevice9 = nullptr;
		}
		if (nullptr != mDirect3D9)
		{
			mDirect3D9->Release();
			mDirect3D9 = nullptr;
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)

		// Destroy the Direct3D 9 runtime linking instance
		delete mDirect3D9RuntimeLinking;
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::setGraphicsRootSignature(Renderer::IRootSignature* rootSignature)
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
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *rootSignature)
		}
	}

	void Direct3D9Renderer::setGraphicsRootDescriptorTable(uint32_t rootParameterIndex, Renderer::IResource* resource)
	{
		// Security checks
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
		{
			if (nullptr == mGraphicsRootSignature)
			{
				RENDERER_LOG(mContext, CRITICAL, "No Direct3D 9 renderer backend graphics root signature set")
				return;
			}
			const Renderer::RootSignature& rootSignature = mGraphicsRootSignature->getRootSignature();
			if (rootParameterIndex >= rootSignature.numberOfParameters)
			{
				RENDERER_LOG(mContext, CRITICAL, "The Direct3D 9 renderer backend root parameter index is out of bounds")
				return;
			}
			const Renderer::RootParameter& rootParameter = rootSignature.parameters[rootParameterIndex];
			if (Renderer::RootParameterType::DESCRIPTOR_TABLE != rootParameter.parameterType)
			{
				RENDERER_LOG(mContext, CRITICAL, "The Direct3D 9 renderer backend root parameter index doesn't reference a descriptor table")
				return;
			}

			// TODO(co) For now, we only support a single descriptor range
			if (1 != rootParameter.descriptorTable.numberOfDescriptorRanges)
			{
				RENDERER_LOG(mContext, CRITICAL, "Only a single descriptor range is supported by the Direct3D 9 renderer backend")
				return;
			}
			if (nullptr == reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges))
			{
				RENDERER_LOG(mContext, CRITICAL, "The Direct3D 9 renderer backend descriptor ranges is a null pointer")
				return;
			}
		}
		#endif

		if (nullptr != resource)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *resource)

			// Get the root signature parameter instance
			const Renderer::RootParameter& rootParameter = mGraphicsRootSignature->getRootSignature().parameters[rootParameterIndex];
			const Renderer::DescriptorRange* descriptorRange = reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges);

			// Check the type of resource to set
			// TODO(co) Some additional resource type root signature security checks in debug build?
			switch (resource->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_BUFFER:
					RENDERER_LOG(mContext, CRITICAL, "Direct3D 9 has no texture buffer support")
					break;

				case Renderer::ResourceType::TEXTURE_1D:
				case Renderer::ResourceType::TEXTURE_2D:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				case Renderer::ResourceType::TEXTURE_3D:
				case Renderer::ResourceType::TEXTURE_CUBE:
				{
					const UINT startSlot = descriptorRange->baseShaderRegister;

					// Get Direct3D 9 texture
					IDirect3DBaseTexture9* direct3DBaseTexture9 = nullptr;
					switch (resource->getResourceType())
					{
						case Renderer::ResourceType::TEXTURE_1D:
							direct3DBaseTexture9 = static_cast<Texture1D*>(resource)->getDirect3DTexture9();
							break;

						case Renderer::ResourceType::TEXTURE_2D:
							direct3DBaseTexture9 = static_cast<Texture2D*>(resource)->getDirect3DTexture9();
							break;

						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
							RENDERER_LOG(mContext, CRITICAL, "Direct3D 9 has no 2D array textures support")
							break;

						case Renderer::ResourceType::TEXTURE_3D:
							direct3DBaseTexture9 = static_cast<Texture3D*>(resource)->getDirect3DTexture9();
							break;

						case Renderer::ResourceType::TEXTURE_CUBE:
							direct3DBaseTexture9 = static_cast<TextureCube*>(resource)->getDirect3DTexture9();
							break;

						case Renderer::ResourceType::TEXTURE_BUFFER:
						case Renderer::ResourceType::SAMPLER_STATE:
						case Renderer::ResourceType::ROOT_SIGNATURE:
						case Renderer::ResourceType::PROGRAM:
						case Renderer::ResourceType::VERTEX_ARRAY:
						case Renderer::ResourceType::SWAP_CHAIN:
						case Renderer::ResourceType::FRAMEBUFFER:
						case Renderer::ResourceType::INDEX_BUFFER:
						case Renderer::ResourceType::VERTEX_BUFFER:
						case Renderer::ResourceType::UNIFORM_BUFFER:
						case Renderer::ResourceType::INDIRECT_BUFFER:
						case Renderer::ResourceType::PIPELINE_STATE:
						case Renderer::ResourceType::VERTEX_SHADER:
						case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
						case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
						case Renderer::ResourceType::GEOMETRY_SHADER:
						case Renderer::ResourceType::FRAGMENT_SHADER:
							// Nothing here
							break;
					}

					// Information about vertex texture fetch in Direct3D 9 can be found within:
					// Whitepaper: ftp://download.nvidia.com/developer/Papers/2004/Vertex_Textures/Vertex_Textures.pdf
					//    "Shader Model 3.0
					//     Using Vertex Textures"
					//    (DA-01373-001_v00 1 - 06/24/04)
					// From
					//    Philipp Gerasimov
					//    Randima (Randy) Fernando
					//    Simon Green
					//    NVIDIA Corporation
					// Four texture samplers are supported:
					//     D3DVERTEXTEXTURESAMPLER1
					//     D3DVERTEXTEXTURESAMPLER2
					//     D3DVERTEXTEXTURESAMPLER3
					//     D3DVERTEXTEXTURESAMPLER4
					// -> Update the given zero based texture unit (the constants are linear, so the following is fine)
					const UINT vertexFetchStartSlot = startSlot + D3DVERTEXTEXTURESAMPLER1;

					switch (rootParameter.shaderVisibility)
					{
						case Renderer::ShaderVisibility::ALL:
						{
							// Begin debug event
							RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

							// Set texture
							mDirect3DDevice9->SetTexture(vertexFetchStartSlot, direct3DBaseTexture9);
							mDirect3DDevice9->SetTexture(startSlot, direct3DBaseTexture9);

							{ // Set sampler
								const SamplerState* samplerState = mGraphicsRootSignature->getSamplerState(descriptorRange->samplerRootParameterIndex);
								samplerState->setDirect3D9SamplerStates(vertexFetchStartSlot, *mDirect3DDevice9);
								samplerState->setDirect3D9SamplerStates(startSlot, *mDirect3DDevice9);
							}

							// End debug event
							RENDERER_END_DEBUG_EVENT(this)
							break;
						}

						case Renderer::ShaderVisibility::VERTEX:
							// Begin debug event
							RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

							// Set texture
							mDirect3DDevice9->SetTexture(vertexFetchStartSlot, direct3DBaseTexture9);

							// Set sampler
							mGraphicsRootSignature->getSamplerState(descriptorRange->samplerRootParameterIndex)->setDirect3D9SamplerStates(vertexFetchStartSlot, *mDirect3DDevice9);

							// End debug event
							RENDERER_END_DEBUG_EVENT(this)
							break;

						case Renderer::ShaderVisibility::TESSELLATION_CONTROL:
							RENDERER_LOG(mContext, CRITICAL, "Direct3D 9 has no tessellation control shader support (hull shader in Direct3D terminology)")
							break;

						case Renderer::ShaderVisibility::TESSELLATION_EVALUATION:
							RENDERER_LOG(mContext, CRITICAL, "Direct3D 9 has no tessellation evaluation shader support (domain shader in Direct3D terminology)")
							break;

						case Renderer::ShaderVisibility::GEOMETRY:
							RENDERER_LOG(mContext, CRITICAL, "Direct3D 9 has no geometry shader support")
							break;

						case Renderer::ShaderVisibility::FRAGMENT:
							// "pixel shader" in Direct3D terminology

							// Begin debug event
							RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

							// Set texture
							mDirect3DDevice9->SetTexture(startSlot, direct3DBaseTexture9);

							// Set sampler
							mGraphicsRootSignature->getSamplerState(descriptorRange->samplerRootParameterIndex)->setDirect3D9SamplerStates(startSlot, *mDirect3DDevice9);

							// End debug event
							RENDERER_END_DEBUG_EVENT(this)
							break;
					}
					break;
				}

				case Renderer::ResourceType::SAMPLER_STATE:
					// Unlike Direct3D >=10, Direct3D 9 directly attaches the sampler settings to texture stages
					mGraphicsRootSignature->setSamplerState(rootParameterIndex, static_cast<SamplerState*>(resource));
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
				case Renderer::ResourceType::PIPELINE_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
					RENDERER_LOG(mContext, CRITICAL, "Invalid Direct3D 9 renderer backend resource type")
					break;
			}
		}
		else
		{
			// TODO(co) Handle this situation?
			/*
			// Set the default sampler state
			if (nullptr != mDefaultSamplerState)
			{
				fsSetSamplerState(unit, mDefaultSamplerState);
			}
			else
			{
				// Fallback in case everything goes wrong

				// TODO(co) Set default settings
			}
			*/
		}
	}

	void Direct3D9Renderer::setPipelineState(Renderer::IPipelineState* pipelineState)
	{
		if (nullptr != pipelineState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *pipelineState)

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
	void Direct3D9Renderer::iaSetVertexArray(Renderer::IVertexArray* vertexArray)
	{
		if (nullptr != vertexArray)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *vertexArray)

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

			// Enable the Direct3D 9 vertex declaration and stream source
			static_cast<VertexArray*>(vertexArray)->enableDirect3DVertexDeclarationAndStreamSource();

			// End debug event
			RENDERER_END_DEBUG_EVENT(this)
		}
		else
		{
			mDirect3DDevice9->SetVertexDeclaration(nullptr);
		}
	}

	void Direct3D9Renderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology primitiveTopology)
	{
		// Backup the set primitive topology
		mPrimitiveTopology = primitiveTopology;
	}


	//[-------------------------------------------------------]
	//[ Rasterizer (RS) stage                                 ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport* viewports)
	{
		// Sanity check
		assert((numberOfViewports > 0 && nullptr != viewports) && "Invalid rasterizer state viewports");

		// Set the Direct3D 9 viewport
		// -> Direct3D 9 supports only one viewport
	#ifndef RENDERER_NO_DEBUG
		if (numberOfViewports > 1)
		{
			RENDERER_LOG(mContext, CRITICAL, "Direct3D 9 supports only one viewport")
		}
	#endif
		const D3DVIEWPORT9 direct3D9Viewport =
		{
			static_cast<DWORD>(viewports->topLeftX),	// X (DWORD)
			static_cast<DWORD>(viewports->topLeftY),	// Y (DWORD)
			static_cast<DWORD>(viewports->width),		// Width (DWORD)
			static_cast<DWORD>(viewports->height),		// Height (DWORD)
			viewports->minDepth,						// MinZ (float)
			viewports->maxDepth							// MaxZ (float)
		};
		mDirect3DDevice9->SetViewport(&direct3D9Viewport);
	}

	void Direct3D9Renderer::rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle* scissorRectangles)
	{
		// Sanity check
		assert((numberOfScissorRectangles > 0 && nullptr != scissorRectangles) && "Invalid rasterizer state scissor rectangles");

		// Set the Direct3D 9 scissor rectangles
		// -> "Renderer::ScissorRectangle" directly maps to Direct3D 9 & 10 & 11, do not change it
		// -> Direct3D 9 supports only one viewport
	#ifndef RENDERER_NO_DEBUG
		if (numberOfScissorRectangles > 1)
		{
			RENDERER_LOG(mContext, CRITICAL, "Direct3D 9 supports only one scissor rectangle")
		}
	#endif
		mDirect3DDevice9->SetScissorRect(reinterpret_cast<const RECT*>(scissorRectangles));
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::omSetRenderTarget(Renderer::IRenderTarget* renderTarget)
	{
		// New render target?
		if (mRenderTarget != renderTarget)
		{
			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

			// The "IDirect3DDevice9::SetRenderTarget method"-documentation at MSDN http://msdn.microsoft.com/en-us/library/windows/desktop/bb174455%28v=vs.85%29.aspx states:
			//   "Setting a new render target will cause the viewport (see Viewports and Clipping (Direct3D 9)) to be set to the full size of the new render target."
			// -> Although it's not mentioned within the documentation, the same behaviour is true for the scissor rectangle
			// -> This behaviour is different from Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			// -> We have to compensate the Direct3D 9 behaviour in here

			// Backup the currently set Direct3D 9 viewport and scissor rectangle
			D3DVIEWPORT9 direct3D9ViewportBackup;
			mDirect3DDevice9->GetViewport(&direct3D9ViewportBackup);
			RECT direct3D9ScissorRectangleBackup;
			mDirect3DDevice9->GetScissorRect(&direct3D9ScissorRectangleBackup);

			// Set a render target?
			if (nullptr != renderTarget)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *renderTarget)

				// Release the render target reference, in case we have one
				if (nullptr != mRenderTarget)
				{
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
						// Get the Direct3D 9 swap chain instance
						SwapChain* swapChain = static_cast<SwapChain*>(mRenderTarget);

						// Set the Direct3D 9 default color surfaces
						mDirect3DDevice9->SetRenderTarget(0, swapChain->getDirect3DSurface9RenderTarget());
						for (DWORD direct3D9RenderTargetIndex = 1; direct3D9RenderTargetIndex < mCapabilities.maximumNumberOfSimultaneousRenderTargets; ++direct3D9RenderTargetIndex)
						{
							mDirect3DDevice9->SetRenderTarget(direct3D9RenderTargetIndex, nullptr);
						}

						// Set the Direct3D 9 default depth stencil surface
						mDirect3DDevice9->SetDepthStencilSurface(swapChain->getDirect3DSurface9DepthStencil());
						break;
					}

					case Renderer::ResourceType::FRAMEBUFFER:
					{
						// Get the Direct3D 9 framebuffer instance
						Framebuffer* framebuffer = static_cast<Framebuffer*>(mRenderTarget);

						// Set the Direct3D 9 color surfaces
						DWORD direct3D9RenderTargetIndex = 0;
						IDirect3DSurface9** direct3D9ColorSurfacesEnd = framebuffer->getDirect3DSurface9Colors() + framebuffer->getNumberOfDirect3DSurface9Colors();
						for (IDirect3DSurface9** direct3D9ColorSurface = framebuffer->getDirect3DSurface9Colors(); direct3D9ColorSurface < direct3D9ColorSurfacesEnd; ++direct3D9ColorSurface, ++direct3D9RenderTargetIndex)
						{
							mDirect3DDevice9->SetRenderTarget(direct3D9RenderTargetIndex, *direct3D9ColorSurface);
						}

						// Set the Direct3D 9 depth stencil surface
						mDirect3DDevice9->SetDepthStencilSurface(framebuffer->getDirect3DSurface9DepthStencil());
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
				// Set no Direct3D 9 color surfaces
				for (DWORD direct3D9RenderTargetIndex = 0; direct3D9RenderTargetIndex < mCapabilities.maximumNumberOfSimultaneousRenderTargets; ++direct3D9RenderTargetIndex)
				{
					mDirect3DDevice9->SetRenderTarget(direct3D9RenderTargetIndex, nullptr);
				}

				// Set no Direct3D 9 depth stencil surface
				mDirect3DDevice9->SetDepthStencilSurface(nullptr);

				// Release the render target reference, in case we have one
				if (nullptr != mRenderTarget)
				{
					mRenderTarget->releaseReference();
					mRenderTarget = nullptr;
				}
			}

			// Restore the previously set Direct3D 9 viewport and scissor rectangle
			mDirect3DDevice9->SetViewport(&direct3D9ViewportBackup);
			mDirect3DDevice9->SetScissorRect(&direct3D9ScissorRectangleBackup);

			// End debug event
			RENDERER_END_DEBUG_EVENT(this)
		}
	}


	//[-------------------------------------------------------]
	//[ Operations                                            ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::clear(uint32_t flags, const float color[4], float z, uint32_t stencil)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		// For Direct3D 9, the clear color must be between [0..1]
		float normalizedColor[4] = { color[0], color[1], color[2], color[3] };
		for (int i = 0; i < 4; ++i)
		{
			if (normalizedColor[i] < 0.0f)
			{
				normalizedColor[i] = 0.0f;
			}
			if (normalizedColor[i] > 1.0f)
			{
				normalizedColor[i] = 1.0f;
			}
		}
		#ifndef RENDERER_NO_DEBUG
			if (normalizedColor[0] != color[0] || normalizedColor[1] != color[1] || normalizedColor[2] != color[2] || normalizedColor[3] != color[3])
			{
				RENDERER_LOG(mContext, CRITICAL, "The given clear color was clamped to [0, 1] because Direct3D 9 does not support values outside this range")
			}
		#endif

		// Unlike Direct3D 9, when using Direct3D 10, Direct3D 11, OpenGL or OpenGL ES 2, the viewport(s) and scissor rectangle(s) do not affect the clear operation
		// -> We have to compensate the Direct3D 9 behaviour in here

		// Backup the currently set Direct3D 9 viewport
		D3DVIEWPORT9 direct3D9ViewportBackup;
		mDirect3DDevice9->GetViewport(&direct3D9ViewportBackup);

		// Backup the currently set Direct3D 9 scissor test state
		DWORD direct3D9ScissorTestBackup = 0;
		mDirect3DDevice9->GetRenderState(D3DRS_SCISSORTESTENABLE, &direct3D9ScissorTestBackup);

		// Get the current primary render target
		IDirect3DSurface9* direct3DSurface9 = nullptr;
		if (D3D_OK == mDirect3DDevice9->GetRenderTarget(0, &direct3DSurface9))
		{
			// Get the surface description of the primary render target
			D3DSURFACE_DESC d3dSurfaceDesc;
			direct3DSurface9->GetDesc(&d3dSurfaceDesc);

			// Set a Direct3D 9 viewport which covers the whole current render target
			const D3DVIEWPORT9 direct3D9Viewport =
			{
				0,						// X (DWORD)
				0,						// Y (DWORD)
				d3dSurfaceDesc.Width,	// Width (DWORD)
				d3dSurfaceDesc.Height,	// Height (DWORD)
				0.0f,					// MinZ (float)
				1.0f					// MaxZ (float)
			};
			mDirect3DDevice9->SetViewport(&direct3D9Viewport);

			// Release the render target
			direct3DSurface9->Release();
		}

		// Disable Direct3D 9 scissor test
		mDirect3DDevice9->SetRenderState(D3DRS_SCISSORTESTENABLE, 0);

		// Get API flags
		uint32_t flagsAPI = 0;
		if (flags & Renderer::ClearFlag::COLOR)
		{
			flagsAPI |= D3DCLEAR_TARGET;
		}
		if (flags & Renderer::ClearFlag::DEPTH)
		{
			flagsAPI |= D3DCLEAR_ZBUFFER;
		}
		if (flags & Renderer::ClearFlag::STENCIL)
		{
			flagsAPI |= D3DCLEAR_STENCIL;
		}

		// Clear
		mDirect3DDevice9->Clear(0, nullptr, flagsAPI, D3DCOLOR_COLORVALUE(normalizedColor[0], normalizedColor[1], normalizedColor[2], normalizedColor[3]), z, stencil);

		// Restore the previously set Direct3D 9 viewport
		mDirect3DDevice9->SetViewport(&direct3D9ViewportBackup);

		// Restore previously set Direct3D 9 scissor test state
		mDirect3DDevice9->SetRenderState(D3DRS_SCISSORTESTENABLE, direct3D9ScissorTestBackup);

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}

	void Direct3D9Renderer::resolveMultisampleFramebuffer(Renderer::IRenderTarget&, Renderer::IFramebuffer&)
	{
		// TODO(co) Implement me
	}

	void Direct3D9Renderer::copyResource(Renderer::IResource&, Renderer::IResource&)
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Draw call                                             ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::drawEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Sanity checks
		assert(nullptr != emulationData);
		assert(numberOfDraws > 0 && "Number of draws must not be zero");

		// TODO(co) Currently no buffer overflow check due to lack of interface provided data
		emulationData += indirectBufferOffset;

		// Emit the draw calls
		for (uint32_t i = 0; i < numberOfDraws; ++i)
		{
			const Renderer::DrawInstancedArguments& drawInstancedArguments = *reinterpret_cast<const Renderer::DrawInstancedArguments*>(emulationData);

			// No instancing supported here
			// -> In Direct3D 9, instanced arrays is only possible when drawing indexed primitives, see
			//    "Efficiently Drawing Multiple Instances of Geometry (Direct3D 9)"-article at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb173349%28v=vs.85%29.aspx#Drawing_Non_Indexed_Geometry
			// -> This document states that this is not supported by hardware acceleration on any device, and it's long winded anyway
			assert(1 == drawInstancedArguments.instanceCount);
			assert(0 == drawInstancedArguments.startInstanceLocation);

			{ // Draw
				// Get number of primitives
				uint32_t primitiveCount;
				switch (mPrimitiveTopology)
				{
					case Renderer::PrimitiveTopology::POINT_LIST:
						primitiveCount = drawInstancedArguments.vertexCountPerInstance;
						break;

					case Renderer::PrimitiveTopology::LINE_LIST:
						primitiveCount = drawInstancedArguments.vertexCountPerInstance - 1;
						break;

					case Renderer::PrimitiveTopology::LINE_STRIP:
						primitiveCount = drawInstancedArguments.vertexCountPerInstance - 1;
						break;

					case Renderer::PrimitiveTopology::TRIANGLE_LIST:
						primitiveCount = drawInstancedArguments.vertexCountPerInstance / 3;
						break;

					case Renderer::PrimitiveTopology::TRIANGLE_STRIP:
						primitiveCount = drawInstancedArguments.vertexCountPerInstance - 2;
						break;

					case Renderer::PrimitiveTopology::UNKNOWN:
					case Renderer::PrimitiveTopology::PATCH_LIST_1:
					case Renderer::PrimitiveTopology::PATCH_LIST_2:
					case Renderer::PrimitiveTopology::PATCH_LIST_3:
					case Renderer::PrimitiveTopology::PATCH_LIST_4:
					case Renderer::PrimitiveTopology::PATCH_LIST_5:
					case Renderer::PrimitiveTopology::PATCH_LIST_6:
					case Renderer::PrimitiveTopology::PATCH_LIST_7:
					case Renderer::PrimitiveTopology::PATCH_LIST_8:
					case Renderer::PrimitiveTopology::PATCH_LIST_9:
					case Renderer::PrimitiveTopology::PATCH_LIST_10:
					case Renderer::PrimitiveTopology::PATCH_LIST_11:
					case Renderer::PrimitiveTopology::PATCH_LIST_12:
					case Renderer::PrimitiveTopology::PATCH_LIST_13:
					case Renderer::PrimitiveTopology::PATCH_LIST_14:
					case Renderer::PrimitiveTopology::PATCH_LIST_15:
					case Renderer::PrimitiveTopology::PATCH_LIST_16:
					case Renderer::PrimitiveTopology::PATCH_LIST_17:
					case Renderer::PrimitiveTopology::PATCH_LIST_18:
					case Renderer::PrimitiveTopology::PATCH_LIST_19:
					case Renderer::PrimitiveTopology::PATCH_LIST_20:
					case Renderer::PrimitiveTopology::PATCH_LIST_21:
					case Renderer::PrimitiveTopology::PATCH_LIST_22:
					case Renderer::PrimitiveTopology::PATCH_LIST_23:
					case Renderer::PrimitiveTopology::PATCH_LIST_24:
					case Renderer::PrimitiveTopology::PATCH_LIST_25:
					case Renderer::PrimitiveTopology::PATCH_LIST_26:
					case Renderer::PrimitiveTopology::PATCH_LIST_27:
					case Renderer::PrimitiveTopology::PATCH_LIST_28:
					case Renderer::PrimitiveTopology::PATCH_LIST_29:
					case Renderer::PrimitiveTopology::PATCH_LIST_30:
					case Renderer::PrimitiveTopology::PATCH_LIST_31:
					case Renderer::PrimitiveTopology::PATCH_LIST_32:
					default:
						return;	// Error!
				}

				// The "Renderer::PrimitiveTopology" values directly map to Direct3D 9 & 10 & 11 constants, do not change them
				mDirect3DDevice9->DrawPrimitive(static_cast<D3DPRIMITIVETYPE>(mPrimitiveTopology), drawInstancedArguments.startVertexLocation, primitiveCount);
			}

			// Advance
			emulationData += sizeof(Renderer::DrawInstancedArguments);
		}
	}

	void Direct3D9Renderer::drawIndexedEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Sanity checks
		assert(nullptr != emulationData);
		assert(numberOfDraws > 0 && "Number of draws must not be zero");

		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
		if (mCapabilities.instancedArrays)
		{
			// TODO(co) Currently no buffer overflow check due to lack of interface provided data
			emulationData += indirectBufferOffset;

			// Emit the draw calls
			for (uint32_t i = 0; i < numberOfDraws; ++i)
			{
				const Renderer::DrawIndexedInstancedArguments& drawIndexedInstancedArguments = *reinterpret_cast<const Renderer::DrawIndexedInstancedArguments*>(emulationData);
				assert(0 == drawIndexedInstancedArguments.startInstanceLocation);	// Not supported by DirectX 9

				// The "Efficiently Drawing Multiple Instances of Geometry (Direct3D 9)"-article at MSDN http://msdn.microsoft.com/en-us/library/windows/desktop/bb173349%28v=vs.85%29.aspx#Drawing_Non_Indexed_Geometry
				// states: "Note that D3DSTREAMSOURCE_INDEXEDDATA and the number of instances to draw must always be set in stream zero."
				// -> "D3DSTREAMSOURCE_INSTANCEDATA" is set within "Direct3D9Renderer::VertexArray::enableDirect3DVertexDeclarationAndStreamSource()"
				mDirect3DDevice9->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | drawIndexedInstancedArguments.instanceCount);

				{ // Draw
					// Get number of primitives
					uint32_t primitiveCount;
					switch (mPrimitiveTopology)
					{
						case Renderer::PrimitiveTopology::POINT_LIST:
							primitiveCount = drawIndexedInstancedArguments.indexCountPerInstance;
							break;

						case Renderer::PrimitiveTopology::LINE_LIST:
							primitiveCount = drawIndexedInstancedArguments.indexCountPerInstance - 1;
							break;

						case Renderer::PrimitiveTopology::LINE_STRIP:
							primitiveCount = drawIndexedInstancedArguments.indexCountPerInstance - 1;
							break;

						case Renderer::PrimitiveTopology::TRIANGLE_LIST:
							primitiveCount = drawIndexedInstancedArguments.indexCountPerInstance / 3;
							break;

						case Renderer::PrimitiveTopology::TRIANGLE_STRIP:
							primitiveCount = drawIndexedInstancedArguments.indexCountPerInstance - 2;
							break;

						case Renderer::PrimitiveTopology::UNKNOWN:
						case Renderer::PrimitiveTopology::PATCH_LIST_1:
						case Renderer::PrimitiveTopology::PATCH_LIST_2:
						case Renderer::PrimitiveTopology::PATCH_LIST_3:
						case Renderer::PrimitiveTopology::PATCH_LIST_4:
						case Renderer::PrimitiveTopology::PATCH_LIST_5:
						case Renderer::PrimitiveTopology::PATCH_LIST_6:
						case Renderer::PrimitiveTopology::PATCH_LIST_7:
						case Renderer::PrimitiveTopology::PATCH_LIST_8:
						case Renderer::PrimitiveTopology::PATCH_LIST_9:
						case Renderer::PrimitiveTopology::PATCH_LIST_10:
						case Renderer::PrimitiveTopology::PATCH_LIST_11:
						case Renderer::PrimitiveTopology::PATCH_LIST_12:
						case Renderer::PrimitiveTopology::PATCH_LIST_13:
						case Renderer::PrimitiveTopology::PATCH_LIST_14:
						case Renderer::PrimitiveTopology::PATCH_LIST_15:
						case Renderer::PrimitiveTopology::PATCH_LIST_16:
						case Renderer::PrimitiveTopology::PATCH_LIST_17:
						case Renderer::PrimitiveTopology::PATCH_LIST_18:
						case Renderer::PrimitiveTopology::PATCH_LIST_19:
						case Renderer::PrimitiveTopology::PATCH_LIST_20:
						case Renderer::PrimitiveTopology::PATCH_LIST_21:
						case Renderer::PrimitiveTopology::PATCH_LIST_22:
						case Renderer::PrimitiveTopology::PATCH_LIST_23:
						case Renderer::PrimitiveTopology::PATCH_LIST_24:
						case Renderer::PrimitiveTopology::PATCH_LIST_25:
						case Renderer::PrimitiveTopology::PATCH_LIST_26:
						case Renderer::PrimitiveTopology::PATCH_LIST_27:
						case Renderer::PrimitiveTopology::PATCH_LIST_28:
						case Renderer::PrimitiveTopology::PATCH_LIST_29:
						case Renderer::PrimitiveTopology::PATCH_LIST_30:
						case Renderer::PrimitiveTopology::PATCH_LIST_31:
						case Renderer::PrimitiveTopology::PATCH_LIST_32:
						default:
							return; // Error!
					}

					// The "Renderer::PrimitiveTopology" values directly map to Direct3D 9 & 10 & 11 constants, do not change them
					const UINT numberOfVertices = drawIndexedInstancedArguments.indexCountPerInstance * 3;	// TODO(co) Review "numberOfVertices", might be wrong
					mDirect3DDevice9->DrawIndexedPrimitive(static_cast<D3DPRIMITIVETYPE>(mPrimitiveTopology), static_cast<INT>(drawIndexedInstancedArguments.baseVertexLocation), 0, numberOfVertices, drawIndexedInstancedArguments.startIndexLocation, primitiveCount);
				}

				// Advance
				emulationData += sizeof(Renderer::DrawIndexedInstancedArguments);
			}

			// Reset the stream source frequency
			mDirect3DDevice9->SetStreamSourceFreq(0, 1);
		}
	}


	//[-------------------------------------------------------]
	//[ Debug                                                 ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::setDebugMarker(const char* name)
	{
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			if (nullptr != D3DPERF_SetMarker)
			{
				assert(strlen(name) < 256);
				wchar_t unicodeName[256];
				std::mbstowcs(unicodeName, name, 256);
				D3DPERF_SetMarker(D3DCOLOR_RGBA(255, 0, 255, 255), unicodeName);
			}
		#endif
	}

	void Direct3D9Renderer::beginDebugEvent(const char* name)
	{
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			if (nullptr != D3DPERF_BeginEvent)
			{
				assert(strlen(name) < 256);
				wchar_t unicodeName[256];
				std::mbstowcs(unicodeName, name, 256);
				D3DPERF_BeginEvent(D3DCOLOR_RGBA(255, 255, 255, 255), unicodeName);
			}
		#endif
	}

	void Direct3D9Renderer::endDebugEvent()
	{
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			if (nullptr != D3DPERF_EndEvent)
			{
				D3DPERF_EndEvent();
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	bool Direct3D9Renderer::isDebugEnabled()
	{
		// Don't check for the "DIRECT3D9RENDERER_NO_DEBUG" preprocessor definition, even if debug
		// is disabled it has to be possible to use this function for an additional security check
		// -> Maybe a debugger/profiler ignores the debug state
		// -> Maybe someone manipulated the binary to enable the debug state, adding a second check
		//    makes it a little bit more time consuming to hack the binary :D (but of course, this is no 100% security)
		return (nullptr != D3DPERF_GetStatus && D3DPERF_GetStatus() != 0);
	}

	Renderer::ISwapChain* Direct3D9Renderer::getMainSwapChain() const
	{
		return mMainSwapChain;
	}


	//[-------------------------------------------------------]
	//[ Shader language                                       ]
	//[-------------------------------------------------------]
	uint32_t Direct3D9Renderer::getNumberOfShaderLanguages() const
	{
		uint32_t numberOfShaderLanguages = 1;	// HLSL support is always there

		// Done, return the number of supported shader languages
		return numberOfShaderLanguages;
	}

	const char* Direct3D9Renderer::getShaderLanguageName(uint32_t index) const
	{
		// HLSL supported
		if (0 == index)
		{
			return ShaderLanguageHlsl::NAME;
		}

		// Error!
		return nullptr;
	}

	Renderer::IShaderLanguage* Direct3D9Renderer::getShaderLanguage(const char* shaderLanguageName)
	{
		// In case "shaderLanguage" is a null pointer, use the default shader language
		if (nullptr != shaderLanguageName)
		{
			// Optimization: Check for shader language name pointer match, first
			if (ShaderLanguageHlsl::NAME == shaderLanguageName || !stricmp(shaderLanguageName, ShaderLanguageHlsl::NAME))
			{
				// If required, create the HLSL shader language instance right now
				if (nullptr == mShaderLanguageHlsl)
				{
					mShaderLanguageHlsl = new ShaderLanguageHlsl(*this);
					mShaderLanguageHlsl->addReference();	// Internal renderer reference
				}

				// Return the shader language instance
				return mShaderLanguageHlsl;
			}

			// Error!
			return nullptr;
		}

		// Return the HLSL shader language instance as default
		return getShaderLanguage(ShaderLanguageHlsl::NAME);
	}


	//[-------------------------------------------------------]
	//[ Resource creation                                     ]
	//[-------------------------------------------------------]
	Renderer::ISwapChain* Direct3D9Renderer::createSwapChain(handle nativeWindowHandle, bool)
	{
		// The provided native window handle must not be a null handle
		return (NULL_HANDLE != nativeWindowHandle) ? new SwapChain(*this, nativeWindowHandle) : nullptr;
	}

	Renderer::IFramebuffer* Direct3D9Renderer::createFramebuffer(uint32_t numberOfColorFramebufferAttachments, const Renderer::FramebufferAttachment* colorFramebufferAttachments, const Renderer::FramebufferAttachment* depthStencilFramebufferAttachment)
	{
		// Validation is done inside the framebuffer implementation
		return new Framebuffer(*this, numberOfColorFramebufferAttachments, colorFramebufferAttachments, depthStencilFramebufferAttachment);
	}

	Renderer::IBufferManager* Direct3D9Renderer::createBufferManager()
	{
		return new BufferManager(*this);
	}

	Renderer::ITextureManager* Direct3D9Renderer::createTextureManager()
	{
		return new TextureManager(*this);
	}

	Renderer::IRootSignature* Direct3D9Renderer::createRootSignature(const Renderer::RootSignature& rootSignature)
	{
		return new RootSignature(*this, rootSignature);
	}

	Renderer::IPipelineState* Direct3D9Renderer::createPipelineState(const Renderer::PipelineState& pipelineState)
	{
		return new PipelineState(*this, pipelineState);
	}

	Renderer::ISamplerState* Direct3D9Renderer::createSamplerState(const Renderer::SamplerState& samplerState)
	{
		return new SamplerState(*this, samplerState);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool Direct3D9Renderer::map(Renderer::IResource& resource, uint32_t subresource, Renderer::MapType mapType, uint32_t, Renderer::MappedSubresource& mappedSubresource)
	{
		// The "Renderer::MapType" values directly map to Direct3D 10 & 11 constants, do not change them
		// The "Renderer::MappedSubresource" structure directly maps to Direct3D 11, do not change it

		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
			{
				// Lock the Direct3D 9 resource
				DWORD flags = 0;
				// TODO(co) Map all flags correctly
				if (Renderer::MapType::READ == mapType)
				{
					flags = D3DLOCK_READONLY;
				}
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;
				return (D3D_OK == static_cast<IndexBuffer&>(resource).getDirect3DIndexBuffer9()->Lock(0, 0, &mappedSubresource.data, flags));
			}

			case Renderer::ResourceType::VERTEX_BUFFER:
			{
				// Lock the Direct3D 9 resource
				DWORD flags = 0;
				// TODO(co) Map all flags correctly
				if (Renderer::MapType::READ == mapType)
				{
					flags = D3DLOCK_READONLY;
				}
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;
				return (D3D_OK == static_cast<VertexBuffer&>(resource).getDirect3DVertexBuffer9()->Lock(0, 0, &mappedSubresource.data, flags));
			}

			case Renderer::ResourceType::INDIRECT_BUFFER:
				mappedSubresource.data		 = static_cast<IndirectBuffer&>(resource).getWritableEmulationData();
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;
				return true;

			case Renderer::ResourceType::TEXTURE_1D:
				// TODO(co) Implement Direct3D 9 1D texture
				RENDERER_LOG(mContext, CRITICAL, "The 1D texture support is not yet implemented inside the Direct3D 9 renderer backend")
				return false;

			case Renderer::ResourceType::TEXTURE_2D:
			{
				bool result = false;

				// TODO(co) In case this texture is a render target, we need to use "IDirect3DDevice9::GetRenderTargetData"-method http://msdn.microsoft.com/en-us/library/bb174405%28VS.85%29.aspx
				// Possible implementation hints from http://stackoverflow.com/questions/120066/doing-readback-from-direct3d-textures-and-surfaces
				/*
					bool GfxDeviceD3D9::ReadbackImage(  params  )
					{
						HRESULT hr;
						IDirect3DDevice9* dev = GetD3DDevice();
						SurfacePointer renderTarget;
						hr = dev->GetRenderTarget( 0, &renderTarget );
						if( !renderTarget || FAILED(hr) )
							return false;

						D3DSURFACE_DESC rtDesc;
						renderTarget->GetDesc( &rtDesc );

						SurfacePointer resolvedSurface;
						if( rtDesc.MultiSampleType != D3DMULTISAMPLE_NONE )
						{
							hr = dev->CreateRenderTarget( rtDesc.Width, rtDesc.Height, rtDesc.Format, D3DMULTISAMPLE_NONE, 0, FALSE, &resolvedSurface, NULL );
							if( FAILED(hr) )
								return false;
							hr = dev->StretchRect( renderTarget, NULL, resolvedSurface, NULL, D3DTEXF_NONE );
							if( FAILED(hr) )
								return false;
							renderTarget = resolvedSurface;
						}

						SurfacePointer offscreenSurface;
						hr = dev->CreateOffscreenPlainSurface( rtDesc.Width, rtDesc.Height, rtDesc.Format, D3DPOOL_SYSTEMMEM, &offscreenSurface, NULL );
						if( FAILED(hr) )
							return false;

						hr = dev->GetRenderTargetData( renderTarget, offscreenSurface );
						bool ok = SUCCEEDED(hr);
						if( ok )
						{
							// Here we have data in offscreenSurface.
							D3DLOCKED_RECT lr;
							RECT rect;
							rect.left = 0;
							rect.right = rtDesc.Width;
							rect.top = 0;
							rect.bottom = rtDesc.Height;
							// Lock the surface to read pixels
							hr = offscreenSurface->LockRect( &lr, &rect, D3DLOCK_READONLY );
							if( SUCCEEDED(hr) )
							{
								// Pointer to data is lt.pBits, each row is
								// lr.Pitch bytes apart (often it is the same as width*bpp, but
								// can be larger if driver uses padding)

								// Read the data here!
								offscreenSurface->UnlockRect();
							}
							else
							{
								ok = false;
							}
						}

						return ok;
					}
				*/

				// Lock the Direct3D 9 resource
				DWORD flags = 0;
				// TODO(co) Map all flags correctly
				if (Renderer::MapType::READ == mapType)
				{
					flags = D3DLOCK_READONLY;
				}
				D3DLOCKED_RECT d3dLockedRect;
				result = (D3D_OK == static_cast<Texture2D&>(resource).getDirect3DTexture9()->LockRect(subresource, &d3dLockedRect, nullptr, flags));

				// Copy over the data
				mappedSubresource.data		 = d3dLockedRect.pBits;
				mappedSubresource.rowPitch   = static_cast<UINT>(d3dLockedRect.Pitch);
				mappedSubresource.depthPitch = 0;

				// Done
				return result;
			}

			case Renderer::ResourceType::TEXTURE_3D:
				// TODO(co) Implement Direct3D 9 3D texture
				RENDERER_LOG(mContext, CRITICAL, "The 3D texture support is not yet implemented inside the Direct3D 9 renderer backend")
				return false;

			case Renderer::ResourceType::TEXTURE_CUBE:
				// TODO(co) Implement Direct3D 9 cube texture
				RENDERER_LOG(mContext, CRITICAL, "The cube texture support is not yet implemented inside the Direct3D 9 renderer backend")
				return false;

			case Renderer::ResourceType::ROOT_SIGNATURE:
			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
			case Renderer::ResourceType::SWAP_CHAIN:
			case Renderer::ResourceType::FRAMEBUFFER:
			case Renderer::ResourceType::UNIFORM_BUFFER:
			case Renderer::ResourceType::TEXTURE_BUFFER:
			case Renderer::ResourceType::TEXTURE_2D_ARRAY:
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

	void Direct3D9Renderer::unmap(Renderer::IResource& resource, uint32_t subresource)
	{
		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
				static_cast<IndexBuffer&>(resource).getDirect3DIndexBuffer9()->Unlock();
				break;

			case Renderer::ResourceType::VERTEX_BUFFER:
				static_cast<VertexBuffer&>(resource).getDirect3DVertexBuffer9()->Unlock();
				break;

			case Renderer::ResourceType::INDIRECT_BUFFER:
				// Nothing here, it's a software emulated indirect buffer
				break;

			case Renderer::ResourceType::TEXTURE_1D:
				// TODO(co) Implement Direct3D 9 1D texture
				RENDERER_LOG(mContext, CRITICAL, "The 1D texture support is not yet implemented inside the Direct3D 9 renderer backend")
				break;

			case Renderer::ResourceType::TEXTURE_2D:
				static_cast<Texture2D&>(resource).getDirect3DTexture9()->UnlockRect(subresource);
				break;

			case Renderer::ResourceType::TEXTURE_3D:
				// TODO(co) Implement Direct3D 9 3D texture
				RENDERER_LOG(mContext, CRITICAL, "The 3D texture support is not yet implemented inside the Direct3D 9 renderer backend")
				break;

			case Renderer::ResourceType::TEXTURE_CUBE:
				// TODO(co) Implement Direct3D 9 cube texture
				RENDERER_LOG(mContext, CRITICAL, "The cube texture support is not yet implemented inside the Direct3D 9 renderer backend")
				break;

			case Renderer::ResourceType::ROOT_SIGNATURE:
			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
			case Renderer::ResourceType::SWAP_CHAIN:
			case Renderer::ResourceType::FRAMEBUFFER:
			case Renderer::ResourceType::UNIFORM_BUFFER:
			case Renderer::ResourceType::TEXTURE_BUFFER:
			case Renderer::ResourceType::TEXTURE_2D_ARRAY:
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
	bool Direct3D9Renderer::beginScene()
	{
		return SUCCEEDED(mDirect3DDevice9->BeginScene());
	}

	void Direct3D9Renderer::submitCommandBuffer(const Renderer::CommandBuffer& commandBuffer)
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

	void Direct3D9Renderer::endScene()
	{
		// We need to forget about the currently set render target
		omSetRenderTarget(nullptr);

		mDirect3DDevice9->EndScene();
	}


	//[-------------------------------------------------------]
	//[ Synchronization                                       ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::flush()
	{
		// Create the Direct3D 9 query instance used for flush right now?
		if (nullptr == mDirect3DQuery9Flush)
		{
			mDirect3DDevice9->CreateQuery(D3DQUERYTYPE_EVENT, &mDirect3DQuery9Flush);

			// "IDirect3DQuery9" is not derived from "IDirect3DResource9", meaning we can't use the "IDirect3DResource9::SetPrivateData()"-method in order to set a debug name
		}
		if (nullptr != mDirect3DQuery9Flush)
		{
			// Perform the flush
			mDirect3DQuery9Flush->Issue(D3DISSUE_END);
			mDirect3DQuery9Flush->GetData(nullptr, 0, D3DGETDATA_FLUSH);
		}
	}

	void Direct3D9Renderer::finish()
	{
		// Create the Direct3D 9 query instance used for flush right now?
		if (nullptr == mDirect3DQuery9Flush)
		{
			mDirect3DDevice9->CreateQuery(D3DQUERYTYPE_EVENT, &mDirect3DQuery9Flush);

			// "IDirect3DQuery9" is not derived from "IDirect3DResource9", meaning we can't use the "IDirect3DResource9::SetPrivateData()"-method in order to set a debug name
		}
		if (nullptr != mDirect3DQuery9Flush)
		{
			// Perform the flush and wait
			mDirect3DQuery9Flush->Issue(D3DISSUE_END);
			while (mDirect3DQuery9Flush->GetData(nullptr, 0, D3DGETDATA_FLUSH) == S_FALSE)
			{
				// Spin-wait
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::initializeCapabilities()
	{
		// Get Direct3D 9 device capabilities
		D3DCAPS9 d3dCaps9;
		mDirect3DDevice9->GetDeviceCaps(&d3dCaps9);

		// Maximum number of viewports (always at least 1)
		mCapabilities.maximumNumberOfViewports = 1;	// Direct3D 9 only supports a single viewport

		// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
		// -> Direct3D 9 supports a maximum number of 4 simultaneous render targets
		mCapabilities.maximumNumberOfSimultaneousRenderTargets = d3dCaps9.NumSimultaneousRTs;

		// Maximum texture dimension
		mCapabilities.maximumTextureDimension = d3dCaps9.MaxTextureWidth;	// Width and height are usually identical, usually...

		// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
		mCapabilities.maximumNumberOf2DTextureArraySlices = 0;

		// Maximum uniform buffer (UBO) size in bytes (usually at least 4096 * 16 bytes, in case there's no support for uniform buffer it's 0)
		mCapabilities.maximumUniformBufferSize = 0;

		// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
		mCapabilities.maximumTextureBufferSize = 0;

		// Maximum indirect buffer size in bytes (in case there's no support for indirect buffer it's 0)
		mCapabilities.maximumIndirectBufferSize = 64 * 1024;	// 64 KiB

		// Maximum number of multisamples (always at least 1, usually 8)
		mCapabilities.maximumNumberOfMultisamples = 1;	// Don't want to support the legacy DirectX 9 multisample support

		// Maximum anisotropy (always at least 1, usually 16)
		mCapabilities.maximumAnisotropy = 16;

		// Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		mCapabilities.individualUniforms = true;

		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
		mCapabilities.instancedArrays = (d3dCaps9.PixelShaderVersion >= D3DPS_VERSION(3, 0));

		// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID)
		mCapabilities.drawInstanced = false;

		// Base vertex supported for draw calls?
		mCapabilities.baseVertex = true;

		// Direct3D 9 has no native multi-threading
		mCapabilities.nativeMultiThreading = false;

		// Direct3D 9 has shader bytecode support
		mCapabilities.shaderBytecode = true;

		// Is there support for vertex shaders (VS)?
		mCapabilities.vertexShader = true;

		// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
		mCapabilities.maximumNumberOfPatchVertices = 0;	// Direct3D 9 has no tessellation support

		// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
		mCapabilities.maximumNumberOfGsOutputVertices = 0;	// Direct3D 9 has no support for geometry shaders

		// Is there support for fragment shaders (FS)?
		mCapabilities.fragmentShader = true;

		// We only target graphics hardware which also supports ATI1N and ATI2N, so no need to add this inside the capabilities
		// -> The following is for debugging only, don't delete it
		#if 0
		{
			D3DDISPLAYMODE d3dDisplayMode;
			mDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dDisplayMode);

			// Check if ATI1N is supported
			bool ati1NSupported = (mDirect3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dDisplayMode.Format, 0, D3DRTYPE_TEXTURE, FOURCC_ATI1N) == D3D_OK);
			ati1NSupported = ati1NSupported;

			// Check if ATI2N is supported
			bool ati2NSupported = (mDirect3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dDisplayMode.Format, 0, D3DRTYPE_TEXTURE, FOURCC_ATI2N) == D3D_OK);
			ati2NSupported = ati2NSupported;
			NOP;
		}
		#endif
	}

	void Direct3D9Renderer::setProgram(Renderer::IProgram* program)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		if (nullptr != program)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *program)

			// Get shaders
			const ProgramHlsl*		  programHlsl			= static_cast<ProgramHlsl*>(program);
			const VertexShaderHlsl*	  vertexShaderHlsl		= programHlsl->getVertexShaderHlsl();
			const FragmentShaderHlsl* fragmentShaderHlsl	= programHlsl->getFragmentShaderHlsl();
			IDirect3DVertexShader9*   direct3DVertexShader9 = vertexShaderHlsl  ? vertexShaderHlsl->getDirect3DVertexShader9()  : nullptr;
			IDirect3DPixelShader9*    direct3DPixelShader9  = fragmentShaderHlsl ? fragmentShaderHlsl->getDirect3DPixelShader9() : nullptr;

			// Set shaders
			if (mDirect3DVertexShader9 != direct3DVertexShader9)
			{
				mDirect3DVertexShader9 = direct3DVertexShader9;
				mDirect3DDevice9->SetVertexShader(mDirect3DVertexShader9);
			}
			if (mDirect3DPixelShader9 != direct3DPixelShader9)
			{
				mDirect3DPixelShader9 = direct3DPixelShader9;
				mDirect3DDevice9->SetPixelShader(mDirect3DPixelShader9);
			}
		}
		else
		{
			if (nullptr != mDirect3DVertexShader9)
			{
				mDirect3DDevice9->SetVertexShader(nullptr);
				mDirect3DVertexShader9 = nullptr;
			}
			if (nullptr != mDirect3DPixelShader9)
			{
				mDirect3DDevice9->SetPixelShader(nullptr);
				mDirect3DPixelShader9 = nullptr;
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
