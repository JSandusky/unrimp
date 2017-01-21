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
#include "Direct3D10Renderer/Direct3D10Renderer.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/Direct3D10Debug.h"	// For "DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "Direct3D10Renderer/Direct3D9RuntimeLinking.h"	//  For the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box) used for debugging, also works directly within VisualStudio 2012 out-of-the-box
#include "Direct3D10Renderer/Direct3D10RuntimeLinking.h"
#include "Direct3D10Renderer/RootSignature.h"
#include "Direct3D10Renderer/Mapping.h"
#include "Direct3D10Renderer/RenderTarget/SwapChain.h"
#include "Direct3D10Renderer/RenderTarget/Framebuffer.h"
#include "Direct3D10Renderer/Buffer/BufferManager.h"
#include "Direct3D10Renderer/Buffer/VertexArray.h"
#include "Direct3D10Renderer/Buffer/IndexBuffer.h"
#include "Direct3D10Renderer/Buffer/VertexBuffer.h"
#include "Direct3D10Renderer/Buffer/UniformBuffer.h"
#include "Direct3D10Renderer/Buffer/TextureBuffer.h"
#include "Direct3D10Renderer/Buffer/IndirectBuffer.h"
#include "Direct3D10Renderer/Texture/TextureManager.h"
#include "Direct3D10Renderer/Texture/Texture2D.h"
#include "Direct3D10Renderer/Texture/Texture2DArray.h"
#include "Direct3D10Renderer/State/SamplerState.h"
#include "Direct3D10Renderer/State/PipelineState.h"
#include "Direct3D10Renderer/Shader/ProgramHlsl.h"
#include "Direct3D10Renderer/Shader/VertexShaderHlsl.h"
#include "Direct3D10Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D10Renderer/Shader/GeometryShaderHlsl.h"
#include "Direct3D10Renderer/Shader/FragmentShaderHlsl.h"

#include <Renderer/Buffer/CommandBuffer.h>
#include <Renderer/Buffer/IndirectBufferTypes.h>


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef DIRECT3D10RENDERER_EXPORTS
	#define DIRECT3D10RENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define DIRECT3D10RENDERER_API_EXPORT
#endif
DIRECT3D10RENDERER_API_EXPORT Renderer::IRenderer *createDirect3D10RendererInstance(handle nativeWindowHandle)
{
	return new Direct3D10Renderer::Direct3D10Renderer(nativeWindowHandle);
}
#undef DIRECT3D10RENDERER_API_EXPORT


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
		bool createDevice(UINT flags, ID3D10Device** d3d10Device)
		{
			// Driver types
			static const D3D10_DRIVER_TYPE D3D10_DRIVER_TYPES[] =
			{
				D3D10_DRIVER_TYPE_HARDWARE,
				D3D10_DRIVER_TYPE_WARP,
				D3D10_DRIVER_TYPE_REFERENCE,
			};
			static const UINT NUMBER_OF_DRIVER_TYPES = sizeof(D3D10_DRIVER_TYPES) / sizeof(D3D10_DRIVER_TYPE);

			// Create the Direct3D 10 device
			for (UINT deviceType = 0; deviceType < NUMBER_OF_DRIVER_TYPES; ++deviceType)
			{
				if (SUCCEEDED(Direct3D10Renderer::D3D10CreateDevice(nullptr, D3D10_DRIVER_TYPES[deviceType], nullptr, flags, D3D10_SDK_VERSION, d3d10Device)))
				{
					// Done
					return true;
				}
			}

			// Error!
			return false;
		}


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
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).setGraphicsRootSignature(realData->rootSignature);
			}

			void SetGraphicsRootDescriptorTable(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetGraphicsRootDescriptorTable* realData = static_cast<const Renderer::Command::SetGraphicsRootDescriptorTable*>(data);
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).setGraphicsRootDescriptorTable(realData->rootParameterIndex, realData->resource);
			}

			//[-------------------------------------------------------]
			//[ States                                                ]
			//[-------------------------------------------------------]
			void SetPipelineState(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetPipelineState* realData = static_cast<const Renderer::Command::SetPipelineState*>(data);
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).setPipelineState(realData->pipelineState);
			}

			//[-------------------------------------------------------]
			//[ Input-assembler (IA) stage                            ]
			//[-------------------------------------------------------]
			void SetVertexArray(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetVertexArray* realData = static_cast<const Renderer::Command::SetVertexArray*>(data);
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).iaSetVertexArray(realData->vertexArray);
			}

			void SetPrimitiveTopology(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetPrimitiveTopology* realData = static_cast<const Renderer::Command::SetPrimitiveTopology*>(data);
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).iaSetPrimitiveTopology(realData->primitiveTopology);
			}

			//[-------------------------------------------------------]
			//[ Rasterizer (RS) stage                                 ]
			//[-------------------------------------------------------]
			void SetViewports(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetViewports* realData = static_cast<const Renderer::Command::SetViewports*>(data);
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).rsSetViewports(realData->numberOfViewports, (nullptr != realData->viewports) ? realData->viewports : reinterpret_cast<const Renderer::Viewport*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
			}

			void SetScissorRectangles(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetScissorRectangles* realData = static_cast<const Renderer::Command::SetScissorRectangles*>(data);
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).rsSetScissorRectangles(realData->numberOfScissorRectangles, (nullptr != realData->scissorRectangles) ? realData->scissorRectangles : reinterpret_cast<const Renderer::ScissorRectangle*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
			}

			//[-------------------------------------------------------]
			//[ Output-merger (OM) stage                              ]
			//[-------------------------------------------------------]
			void SetRenderTarget(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetRenderTarget* realData = static_cast<const Renderer::Command::SetRenderTarget*>(data);
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).omSetRenderTarget(realData->renderTarget);
			}

			//[-------------------------------------------------------]
			//[ Operations                                            ]
			//[-------------------------------------------------------]
			void Clear(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Clear* realData = static_cast<const Renderer::Command::Clear*>(data);
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).clear(realData->flags, realData->color, realData->z, realData->stencil);
			}

			void ResolveMultisampleFramebuffer(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::ResolveMultisampleFramebuffer* realData = static_cast<const Renderer::Command::ResolveMultisampleFramebuffer*>(data);
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).resolveMultisampleFramebuffer(*realData->destinationRenderTarget, *realData->sourceMultisampleFramebuffer);
			}

			void CopyResource(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::CopyResource* realData = static_cast<const Renderer::Command::CopyResource*>(data);
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).copyResource(*realData->destinationResource, *realData->sourceResource);
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
					static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).drawEmulated(realData->indirectBuffer->getEmulationData(), realData->indirectBufferOffset, realData->numberOfDraws);
				}
				else
				{
					static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).drawEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
				}
			}

			void DrawIndexed(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Draw* realData = static_cast<const Renderer::Command::Draw*>(data);
				if (nullptr != realData->indirectBuffer)
				{
					// No resource owner security check in here, we only support emulated indirect buffer
					static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).drawIndexedEmulated(realData->indirectBuffer->getEmulationData(), realData->indirectBufferOffset, realData->numberOfDraws);
				}
				else
				{
					static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).drawIndexedEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
				}
			}

			//[-------------------------------------------------------]
			//[ Debug                                                 ]
			//[-------------------------------------------------------]
			void SetDebugMarker(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetDebugMarker* realData = static_cast<const Renderer::Command::SetDebugMarker*>(data);
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).setDebugMarker(realData->name);
			}

			void BeginDebugEvent(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::BeginDebugEvent* realData = static_cast<const Renderer::Command::BeginDebugEvent*>(data);
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).beginDebugEvent(realData->name);
			}

			void EndDebugEvent(const void*, Renderer::IRenderer& renderer)
			{
				static_cast<Direct3D10Renderer::Direct3D10Renderer&>(renderer).endDebugEvent();
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
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Direct3D10Renderer::Direct3D10Renderer(handle nativeWindowHandle) :
		mDirect3D10RuntimeLinking(new Direct3D10RuntimeLinking()),
		mD3D10Device(nullptr),
		mDirect3D9RuntimeLinking(nullptr),
		mShaderLanguageHlsl(nullptr),
		mD3D10QueryFlush(nullptr),
		mMainSwapChain(nullptr),
		mRenderTarget(nullptr),
		mGraphicsRootSignature(nullptr)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		// Is Direct3D 10 available?
		if (mDirect3D10RuntimeLinking->isDirect3D10Avaiable())
		{
			// Flags
			UINT flags = 0;
			#ifdef _DEBUG
				flags |= D3D10_CREATE_DEVICE_DEBUG;
			#endif

			// Create the Direct3D 10 device
			if (!detail::createDevice(flags, &mD3D10Device) && (flags & D3D10_CREATE_DEVICE_DEBUG))
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Failed to create device instance, retrying without debug flag (maybe no Windows SDK is installed)")
				flags &= ~D3D10_CREATE_DEVICE_DEBUG;
				detail::createDevice(flags, &mD3D10Device);
			}

			// Is there a Direct3D 10 device?
			if (nullptr != mD3D10Device)
			{
				#ifdef DIRECT3D10RENDERER_NO_DEBUG
					// Create the Direct3D 9 runtime linking instance, we know there can't be one, yet
					mDirect3D9RuntimeLinking = new Direct3D9RuntimeLinking();

					// Call the Direct3D 9 PIX function
					if (mDirect3D9RuntimeLinking->isDirect3D9Avaiable())
					{
						// Disable debugging
						D3DPERF_SetOptions(1);
					}
				#endif

				// Direct3D 10 debug settings
				if (flags & D3D10_CREATE_DEVICE_DEBUG)
				{
					ID3D10Debug* d3d10Debug = nullptr;
					if (SUCCEEDED(mD3D10Device->QueryInterface(__uuidof(ID3D10Debug), reinterpret_cast<LPVOID*>(&d3d10Debug))))
					{
						ID3D10InfoQueue* d3d10InfoQueue = nullptr;
						if (SUCCEEDED(d3d10Debug->QueryInterface(__uuidof(ID3D10InfoQueue), reinterpret_cast<LPVOID*>(&d3d10InfoQueue))))
						{
							// When using render-to-texture, Direct3D 10 will quickly spam the log with
							//   "
							//   D3D11 WARNING: ID3D11DeviceContext::OMSetRenderTargets: Resource being set to OM RenderTarget slot 0 is still bound on input! [ STATE_SETTING WARNING #9: DEVICE_OMSETRENDERTARGETS_HAZARD]
							//   D3D11 WARNING: ID3D11DeviceContext::OMSetRenderTargets[AndUnorderedAccessViews]: Forcing VS shader resource slot 0 to NULL. [ STATE_SETTING WARNING #3: DEVICE_VSSETSHADERRESOURCES_HAZARD]
							//   D3D11 WARNING: ID3D11DeviceContext::OMSetRenderTargets[AndUnorderedAccessViews]: Forcing GS shader resource slot 0 to NULL. [ STATE_SETTING WARNING #5: DEVICE_GSSETSHADERRESOURCES_HAZARD]
							//   D3D11 WARNING: ID3D11DeviceContext::OMSetRenderTargets[AndUnorderedAccessViews]: Forcing PS shader resource slot 0 to NULL. [ STATE_SETTING WARNING #7: DEVICE_PSSETSHADERRESOURCES_HAZARD]
							//   "
							// (yes there's really D3D11 visible when using Windows 10 64 bit)
							// When not unbinding render targets from shader resources, even if shaders never access the render target by reading. We could add extra
							// logic to avoid this situation, but on the other hand, the renderer backend should be as slim as possible. Since those Direct3D 10 warnings
							// are pretty annoying and introduce the risk of missing relevant warnings, let's suppress those warnings. Thought about this for a while, feels
							// like the best solution considering the alternatives even if suppressing warnings is not always the best idea.
							D3D10_MESSAGE_ID d3d10MessageIds[] =
							{
								D3D10_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD,
								D3D10_MESSAGE_ID_DEVICE_VSSETSHADERRESOURCES_HAZARD,
								D3D10_MESSAGE_ID_DEVICE_GSSETSHADERRESOURCES_HAZARD,
								D3D10_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD
							};
							D3D10_INFO_QUEUE_FILTER d3d10InfoQueueFilter = {};
							d3d10InfoQueueFilter.DenyList.NumIDs = _countof(d3d10MessageIds);
							d3d10InfoQueueFilter.DenyList.pIDList = d3d10MessageIds;
							d3d10InfoQueue->AddStorageFilterEntries(&d3d10InfoQueueFilter);

							// TODO(co) If would be nice to break by default on everything, on the other hand there's no usable callstack then which renders this somewhat useless
							// d3d10InfoQueue->SetBreakOnSeverity(D3D10_MESSAGE_SEVERITY_CORRUPTION, true);
							// d3d10InfoQueue->SetBreakOnSeverity(D3D10_MESSAGE_SEVERITY_ERROR, true);
							// d3d10InfoQueue->SetBreakOnSeverity(D3D10_MESSAGE_SEVERITY_WARNING, true);
							// d3d10InfoQueue->SetBreakOnSeverity(D3D10_MESSAGE_SEVERITY_INFO, true);

							d3d10InfoQueue->Release();
						}
						d3d10Debug->Release();
					}
				}

				// Initialize the capabilities
				initializeCapabilities();

				// Create a main swap chain instance?
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
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Failed to create device instance")
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}

	Direct3D10Renderer::~Direct3D10Renderer()
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
		if (nullptr != mGraphicsRootSignature)
		{
			mGraphicsRootSignature->releaseReference();
			mGraphicsRootSignature = nullptr;
		}

		{ // For debugging: At this point there should be no resource instances left, validate this!
			// -> Are the currently any resource instances?
			const unsigned long numberOfCurrentResources = getStatistics().getNumberOfCurrentResources();
			if (numberOfCurrentResources > 0)
			{
				// Error!
				if (numberOfCurrentResources > 1)
				{
					RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 10 error: Renderer is going to be destroyed, but there are still %d resource instances left (memory leak)\n", numberOfCurrentResources)
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Renderer is going to be destroyed, but there is still one resource instance left (memory leak)\n")
				}

				// Use debug output to show the current number of resource instances
				getStatistics().debugOutputCurrentResouces();
			}
		}

		// Release the Direct3D 10 query instance used for flush, in case we have one
		if (nullptr != mD3D10QueryFlush)
		{
			mD3D10QueryFlush->Release();
		}

		// Release the HLSL shader language instance, in case we have one
		if (nullptr != mShaderLanguageHlsl)
		{
			mShaderLanguageHlsl->releaseReference();
		}

		// Release the Direct3D 10 we've created
		if (nullptr != mD3D10Device)
		{
			mD3D10Device->Release();
			mD3D10Device = nullptr;
		}

		// Destroy the Direct3D 10 runtime linking instance
		delete mDirect3D10RuntimeLinking;

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)

		// Destroy the Direct3D 9 runtime linking instance, in case there's one
		if (nullptr != mDirect3D9RuntimeLinking)
		{
			delete mDirect3D9RuntimeLinking;
		}
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void Direct3D10Renderer::setGraphicsRootSignature(Renderer::IRootSignature *rootSignature)
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
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *rootSignature)
		}
	}

	void Direct3D10Renderer::setGraphicsRootDescriptorTable(uint32_t rootParameterIndex, Renderer::IResource* resource)
	{
		// Security checks
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
		{
			if (nullptr == mGraphicsRootSignature)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: No graphics root signature set")
				return;
			}
			const Renderer::RootSignature& rootSignature = mGraphicsRootSignature->getRootSignature();
			if (rootParameterIndex >= rootSignature.numberOfParameters)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Root parameter index is out of bounds")
				return;
			}
			const Renderer::RootParameter& rootParameter = rootSignature.parameters[rootParameterIndex];
			if (Renderer::RootParameterType::DESCRIPTOR_TABLE != rootParameter.parameterType)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Root parameter index doesn't reference a descriptor table")
				return;
			}

			// TODO(co) For now, we only support a single descriptor range
			if (1 != rootParameter.descriptorTable.numberOfDescriptorRanges)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Only a single descriptor range is supported")
				return;
			}
			if (nullptr == reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges))
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Descriptor ranges is a null pointer")
				return;
			}
		}
		#endif

		if (nullptr != resource)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *resource)

			// Get the root signature parameter instance
			const Renderer::RootParameter& rootParameter = mGraphicsRootSignature->getRootSignature().parameters[rootParameterIndex];
			const Renderer::DescriptorRange* descriptorRange = reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges);

			// Check the type of resource to set
			// TODO(co) Some additional resource type root signature security checks in debug build?
			const Renderer::ResourceType resourceType = resource->getResourceType();
			switch (resourceType)
			{
				case Renderer::ResourceType::UNIFORM_BUFFER:
				{
					ID3D10Buffer *d3d10Buffers = static_cast<UniformBuffer*>(resource)->getD3D10Buffer();
					const UINT startSlot = descriptorRange->baseShaderRegister;
					switch (rootParameter.shaderVisibility)
					{
						case Renderer::ShaderVisibility::ALL:
							mD3D10Device->VSSetConstantBuffers(startSlot, 1, &d3d10Buffers);
							mD3D10Device->GSSetConstantBuffers(startSlot, 1, &d3d10Buffers);
							mD3D10Device->PSSetConstantBuffers(startSlot, 1, &d3d10Buffers);
							break;

						case Renderer::ShaderVisibility::VERTEX:
							mD3D10Device->VSSetConstantBuffers(startSlot, 1, &d3d10Buffers);
							break;

						case Renderer::ShaderVisibility::TESSELLATION_CONTROL:
							RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Direct3D 10 has no tessellation control shader support (hull shader in Direct3D terminology)")
							break;

						case Renderer::ShaderVisibility::TESSELLATION_EVALUATION:
							RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Direct3D 10 has no tessellation evaluation shader support (domain shader in Direct3D terminology)")
							break;

						case Renderer::ShaderVisibility::GEOMETRY:
							mD3D10Device->GSSetConstantBuffers(startSlot, 1, &d3d10Buffers);
							break;

						case Renderer::ShaderVisibility::FRAGMENT:
							// "pixel shader" in Direct3D terminology
							mD3D10Device->PSSetConstantBuffers(startSlot, 1, &d3d10Buffers);
							break;
					}
					break;
				}

				case Renderer::ResourceType::TEXTURE_BUFFER:
				case Renderer::ResourceType::TEXTURE_2D:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					ID3D10ShaderResourceView *d3d10ShaderResourceView = nullptr;
					switch (resourceType)
					{
						case Renderer::ResourceType::TEXTURE_BUFFER:
							d3d10ShaderResourceView = static_cast<TextureBuffer*>(resource)->getD3D10ShaderResourceView();
							break;

						case Renderer::ResourceType::TEXTURE_2D:
							d3d10ShaderResourceView = static_cast<Texture2D*>(resource)->getD3D10ShaderResourceView();
							break;

						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
							d3d10ShaderResourceView = static_cast<Texture2DArray*>(resource)->getD3D10ShaderResourceView();
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
						case Renderer::ResourceType::SAMPLER_STATE:
						case Renderer::ResourceType::VERTEX_SHADER:
						case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
						case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
						case Renderer::ResourceType::GEOMETRY_SHADER:
						case Renderer::ResourceType::FRAGMENT_SHADER:
							RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Invalid resource type")
							break;
					}
					const UINT startSlot = descriptorRange->baseShaderRegister;
					switch (rootParameter.shaderVisibility)
					{
						case Renderer::ShaderVisibility::ALL:
							mD3D10Device->VSSetShaderResources(startSlot, 1, &d3d10ShaderResourceView);
							mD3D10Device->GSSetShaderResources(startSlot, 1, &d3d10ShaderResourceView);
							mD3D10Device->PSSetShaderResources(startSlot, 1, &d3d10ShaderResourceView);
							break;

						case Renderer::ShaderVisibility::VERTEX:
							mD3D10Device->VSSetShaderResources(startSlot, 1, &d3d10ShaderResourceView);
							break;

						case Renderer::ShaderVisibility::TESSELLATION_CONTROL:
							RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Direct3D 10 has no tessellation control shader support (hull shader in Direct3D terminology)")
							break;

						case Renderer::ShaderVisibility::TESSELLATION_EVALUATION:
							RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Direct3D 10 has no tessellation evaluation shader support (domain shader in Direct3D terminology)")
							break;

						case Renderer::ShaderVisibility::GEOMETRY:
							mD3D10Device->GSSetShaderResources(startSlot, 1, &d3d10ShaderResourceView);
							break;

						case Renderer::ShaderVisibility::FRAGMENT:
							// "pixel shader" in Direct3D terminology
							mD3D10Device->PSSetShaderResources(startSlot, 1, &d3d10ShaderResourceView);
							break;
					}
					break;
				}

				case Renderer::ResourceType::SAMPLER_STATE:
				{
					ID3D10SamplerState *d3d10SamplerState = static_cast<SamplerState*>(resource)->getD3D10SamplerState();
					const UINT startSlot = descriptorRange->baseShaderRegister;
					switch (rootParameter.shaderVisibility)
					{
						case Renderer::ShaderVisibility::ALL:
							mD3D10Device->VSSetSamplers(startSlot, 1, &d3d10SamplerState);
							mD3D10Device->GSSetSamplers(startSlot, 1, &d3d10SamplerState);
							mD3D10Device->PSSetSamplers(startSlot, 1, &d3d10SamplerState);
							break;

						case Renderer::ShaderVisibility::VERTEX:
							mD3D10Device->VSSetSamplers(startSlot, 1, &d3d10SamplerState);
							break;

						case Renderer::ShaderVisibility::TESSELLATION_CONTROL:
							RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Direct3D 10 has no tessellation control shader support (hull shader in Direct3D terminology)")
							break;

						case Renderer::ShaderVisibility::TESSELLATION_EVALUATION:
							RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Direct3D 10 has no tessellation evaluation shader support (domain shader in Direct3D terminology)")
							break;

						case Renderer::ShaderVisibility::GEOMETRY:
							mD3D10Device->GSSetSamplers(startSlot, 1, &d3d10SamplerState);
							break;

						case Renderer::ShaderVisibility::FRAGMENT:
							// "pixel shader" in Direct3D terminology
							mD3D10Device->PSSetSamplers(startSlot, 1, &d3d10SamplerState);
							break;
					}
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
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 10 error: Invalid resource type")
					break;
			}
		}
		else
		{
			// TODO(co) Handle this situation?
		}
	}

	void Direct3D10Renderer::setPipelineState(Renderer::IPipelineState* pipelineState)
	{
		if (nullptr != pipelineState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *pipelineState)

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
	void Direct3D10Renderer::iaSetVertexArray(Renderer::IVertexArray *vertexArray)
	{
		if (nullptr != vertexArray)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *vertexArray)

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

			static_cast<VertexArray*>(vertexArray)->setDirect3DIASetInputLayoutAndStreamSource();

			// End debug event
			RENDERER_END_DEBUG_EVENT(this)
		}
		else
		{
			mD3D10Device->IASetInputLayout(nullptr);
		}
	}

	void Direct3D10Renderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology primitiveTopology)
	{
		// Set primitive topology
		// -> The "Renderer::PrimitiveTopology" values directly map to Direct3D 9 & 10 & 11 constants, do not change them
		mD3D10Device->IASetPrimitiveTopology(static_cast<D3D10_PRIMITIVE_TOPOLOGY>(primitiveTopology));
	}


	//[-------------------------------------------------------]
	//[ Rasterizer (RS) stage                                 ]
	//[-------------------------------------------------------]
	void Direct3D10Renderer::rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport *viewports)
	{
		// Are the given viewports valid?
		if (numberOfViewports > 0 && nullptr != viewports)
		{
			#ifndef RENDERER_NO_DEBUG
				// Is the given number of viewports valid?
				if (numberOfViewports > (D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX + 1))
				{
					RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 10 error: Direct3D 10 supports only %d viewports", D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX)
					numberOfViewports = D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX + 1;
				}
			#endif

			// Set the Direct3D 10 viewports
			D3D10_VIEWPORT d3dViewports[D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
			D3D10_VIEWPORT *d3dViewport = d3dViewports;
			for (uint32_t i = 0; i < numberOfViewports; ++i, ++d3dViewport, ++viewports)
			{
				d3dViewport->TopLeftX = static_cast<INT> (viewports->topLeftX);
				d3dViewport->TopLeftY = static_cast<INT> (viewports->topLeftY);
				d3dViewport->Width    = static_cast<UINT>(viewports->width);
				d3dViewport->Height   = static_cast<UINT>(viewports->height);
				d3dViewport->MinDepth = viewports->minDepth;
				d3dViewport->MaxDepth = viewports->maxDepth;
			}
			mD3D10Device->RSSetViewports(numberOfViewports, d3dViewports);
		}
	}

	void Direct3D10Renderer::rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles)
	{
		// Are the given scissor rectangles valid?
		if (numberOfScissorRectangles > 0 && nullptr != scissorRectangles)
		{
			// Set the Direct3D 10 scissor rectangles
			// -> "Renderer::ScissorRectangle" directly maps to Direct3D 9 & 10 & 11, do not change it
			// -> Let Direct3D 10 perform the index validation for us (the Direct3D 10 debug features are pretty good)
			mD3D10Device->RSSetScissorRects(numberOfScissorRectangles, reinterpret_cast<const D3D10_RECT*>(scissorRectangles));
		}
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	void Direct3D10Renderer::omSetRenderTarget(Renderer::IRenderTarget *renderTarget)
	{
		// New render target?
		if (mRenderTarget != renderTarget)
		{
			// Set a render target?
			if (nullptr != renderTarget)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *renderTarget)

				// Release the render target reference, in case we have one
				Framebuffer* framebufferToGenerateMipmapsFor = nullptr;
				if (nullptr != mRenderTarget)
				{
					// Generate mipmaps?
					if (Renderer::ResourceType::FRAMEBUFFER == mRenderTarget->getResourceType() && static_cast<Framebuffer*>(mRenderTarget)->getGenerateMipmaps())
					{
						framebufferToGenerateMipmapsFor = static_cast<Framebuffer*>(mRenderTarget);
					}
					else
					{
						// Release reference
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
						// Get the Direct3D 10 swap chain instance
						SwapChain *swapChain = static_cast<SwapChain*>(mRenderTarget);

						// Direct3D 10 needs a pointer to a pointer, so give it one
						ID3D10RenderTargetView *d3d10RenderTargetView = swapChain->getD3D10RenderTargetView();
						mD3D10Device->OMSetRenderTargets(1, &d3d10RenderTargetView, swapChain->getD3D10DepthStencilView());
						break;
					}

					case Renderer::ResourceType::FRAMEBUFFER:
					{
						// Get the Direct3D 10 framebuffer instance
						Framebuffer *framebuffer = static_cast<Framebuffer*>(mRenderTarget);

						// Set the Direct3D 10 render targets
						mD3D10Device->OMSetRenderTargets(framebuffer->getNumberOfColorTextures(), framebuffer->getD3D10RenderTargetViews(), framebuffer->getD3D10DepthStencilView());
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
					framebufferToGenerateMipmapsFor->generateMipmaps(*mD3D10Device);
					framebufferToGenerateMipmapsFor->releaseReference();
				}
			}
			else
			{
				// Set the Direct3D 10 render targets
				mD3D10Device->OMSetRenderTargets(0, nullptr, nullptr);

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
	void Direct3D10Renderer::clear(uint32_t flags, const float color[4], float z, uint32_t stencil)
	{
		// Unlike Direct3D 9, OpenGL or OpenGL ES 2, Direct3D 10 clears a given render target view and not the currently bound

		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		// Render target set?
		if (nullptr != mRenderTarget)
		{
			// Evaluate the render target type
			switch (mRenderTarget->getResourceType())
			{
				case Renderer::ResourceType::SWAP_CHAIN:
				{
					// Get the Direct3D 10 swap chain instance
					SwapChain *swapChain = static_cast<SwapChain*>(mRenderTarget);

					// Clear the Direct3D 10 render target view?
					if (flags & Renderer::ClearFlag::COLOR)
					{
						mD3D10Device->ClearRenderTargetView(swapChain->getD3D10RenderTargetView(), color);
					}

					// Clear the Direct3D 10 depth stencil view?
					if (nullptr != swapChain->getD3D10DepthStencilView())
					{
						// Get the Direct3D 10 clear flags
						UINT direct3D10ClearFlags = (flags & Renderer::ClearFlag::DEPTH) ? D3D10_CLEAR_DEPTH : 0u;
						if (flags & Renderer::ClearFlag::STENCIL)
						{
							direct3D10ClearFlags |= D3D10_CLEAR_STENCIL;
						}
						if (0 != direct3D10ClearFlags)
						{
							// Clear the Direct3D 10 depth stencil view
							mD3D10Device->ClearDepthStencilView(swapChain->getD3D10DepthStencilView(), direct3D10ClearFlags, z, static_cast<UINT8>(stencil));
						}
					}
					break;
				}

				case Renderer::ResourceType::FRAMEBUFFER:
				{
					// Get the Direct3D 10 framebuffer instance
					Framebuffer *framebuffer = static_cast<Framebuffer*>(mRenderTarget);

					// Clear all Direct3D 10 render target views?
					if (flags & Renderer::ClearFlag::COLOR)
					{
						// Loop through all Direct3D 10 render target views
						ID3D10RenderTargetView **d3d10RenderTargetViewsEnd = framebuffer->getD3D10RenderTargetViews() + framebuffer->getNumberOfColorTextures();
						for (ID3D10RenderTargetView **d3d10RenderTargetView = framebuffer->getD3D10RenderTargetViews(); d3d10RenderTargetView < d3d10RenderTargetViewsEnd; ++d3d10RenderTargetView)
						{
							// Valid Direct3D 10 render target view?
							if (nullptr != *d3d10RenderTargetView)
							{
								mD3D10Device->ClearRenderTargetView(*d3d10RenderTargetView, color);
							}
						}
					}

					// Clear the Direct3D 10 depth stencil view?
					if (nullptr != framebuffer->getD3D10DepthStencilView())
					{
						// Get the Direct3D 10 clear flags
						UINT direct3D10ClearFlags = (flags & Renderer::ClearFlag::DEPTH) ? D3D10_CLEAR_DEPTH : 0u;
						if (flags & Renderer::ClearFlag::STENCIL)
						{
							direct3D10ClearFlags |= D3D10_CLEAR_STENCIL;
						}
						if (0 != direct3D10ClearFlags)
						{
							// Clear the Direct3D 10 depth stencil view
							mD3D10Device->ClearDepthStencilView(framebuffer->getD3D10DepthStencilView(), direct3D10ClearFlags, z, static_cast<UINT8>(stencil));
						}
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
		}
		else
		{
			// In case no render target is currently set we don't have to do anything in here
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}

	void Direct3D10Renderer::resolveMultisampleFramebuffer(Renderer::IRenderTarget& destinationRenderTarget, Renderer::IFramebuffer& sourceMultisampleFramebuffer)
	{
		// Security check: Are the given resources owned by this renderer? (calls "return" in case of a mismatch)
		DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, destinationRenderTarget)
		DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, sourceMultisampleFramebuffer)

		// Evaluate the render target type
		switch (destinationRenderTarget.getResourceType())
		{
			case Renderer::ResourceType::SWAP_CHAIN:
			{
				// Get the Direct3D 10 swap chain instance
				// TODO(co) Implement me, not that important in practice so not directly implemented
				// SwapChain& swapChain = static_cast<SwapChain&>(destinationRenderTarget);
				break;
			}

			case Renderer::ResourceType::FRAMEBUFFER:
			{
				// Get the Direct3D 10 framebuffer instances
				const Framebuffer& direct3D10DestinationFramebuffer = static_cast<const Framebuffer&>(destinationRenderTarget);
				const Framebuffer& direct3D10SourceMultisampleFramebuffer = static_cast<const Framebuffer&>(sourceMultisampleFramebuffer);

				// Process all Direct3D 10 render target textures
				if (direct3D10DestinationFramebuffer.getNumberOfColorTextures() > 0 && direct3D10SourceMultisampleFramebuffer.getNumberOfColorTextures() > 0)
				{
					const uint32_t numberOfColorTextures = (direct3D10DestinationFramebuffer.getNumberOfColorTextures() < direct3D10SourceMultisampleFramebuffer.getNumberOfColorTextures()) ? direct3D10DestinationFramebuffer.getNumberOfColorTextures() : direct3D10SourceMultisampleFramebuffer.getNumberOfColorTextures();
					Renderer::ITexture** destinationTexture = direct3D10DestinationFramebuffer.getColorTextures();
					Renderer::ITexture** sourceTexture = direct3D10SourceMultisampleFramebuffer.getColorTextures();
					Renderer::ITexture** sourceTextureEnd = sourceTexture + numberOfColorTextures;
					for (; sourceTexture < sourceTextureEnd; ++sourceTexture, ++destinationTexture)
					{
						// Valid Direct3D 10 render target views?
						if (nullptr != *destinationTexture && nullptr != *sourceTexture)
						{
							const Texture2D* d3d10DestinationTexture2D = static_cast<const Texture2D*>(*destinationTexture);
							const Texture2D* d3d10SourceTexture2D = static_cast<const Texture2D*>(*sourceTexture);
							mD3D10Device->ResolveSubresource(d3d10DestinationTexture2D->getD3D10Texture2D(), D3D10CalcSubresource(0, 0, 1), d3d10SourceTexture2D->getD3D10Texture2D(), D3D10CalcSubresource(0, 0, 1), static_cast<DXGI_FORMAT>(Mapping::getDirect3D10Format(d3d10DestinationTexture2D->getTextureFormat())));
						}
					}
				}

				// Process Direct3D 10 depth stencil texture
				if (nullptr != direct3D10DestinationFramebuffer.getDepthStencilTexture() && nullptr != direct3D10SourceMultisampleFramebuffer.getDepthStencilTexture())
				{
					const Texture2D* d3d10DestinationTexture2D = static_cast<const Texture2D*>(direct3D10DestinationFramebuffer.getDepthStencilTexture());
					const Texture2D* d3d10SourceTexture2D = static_cast<const Texture2D*>(direct3D10SourceMultisampleFramebuffer.getDepthStencilTexture());
					mD3D10Device->ResolveSubresource(d3d10DestinationTexture2D->getD3D10Texture2D(), D3D10CalcSubresource(0, 0, 1), d3d10SourceTexture2D->getD3D10Texture2D(), D3D10CalcSubresource(0, 0, 1), static_cast<DXGI_FORMAT>(Mapping::getDirect3D10Format(d3d10DestinationTexture2D->getTextureFormat())));
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
	}

	void Direct3D10Renderer::copyResource(Renderer::IResource& destinationResource, Renderer::IResource& sourceResource)
	{
		// Security check: Are the given resources owned by this renderer? (calls "return" in case of a mismatch)
		DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, destinationResource)
		DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, sourceResource)

		// Evaluate the render target type
		switch (destinationResource.getResourceType())
		{
			case Renderer::ResourceType::TEXTURE_2D:
				if (sourceResource.getResourceType() == Renderer::ResourceType::TEXTURE_2D)
				{
					// Get the Direct3D 10 texture 2D instances
					const Texture2D& direct3D10DestinationTexture2D = static_cast<const Texture2D&>(destinationResource);
					const Texture2D& direct3D10SourceTexture2D = static_cast<const Texture2D&>(sourceResource);

					// Copy resource
					mD3D10Device->CopyResource(direct3D10DestinationTexture2D.getD3D10Texture2D(), direct3D10SourceTexture2D.getD3D10Texture2D());
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
	void Direct3D10Renderer::drawEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Get indirect buffer data and perform security checks
		assert(nullptr != emulationData);

		// TODO(co) Currently no buffer overflow check due to lack of interface provided data
		emulationData += indirectBufferOffset;

		// Emit the draw calls
		for (uint32_t i = 0; i < numberOfDraws; ++i)
		{
			const Renderer::DrawInstancedArguments& drawInstancedArguments = *reinterpret_cast<const Renderer::DrawInstancedArguments*>(emulationData);

			// Draw
			if (drawInstancedArguments.instanceCount > 1)
			{
				// With instancing
				mD3D10Device->DrawInstanced(
					drawInstancedArguments.vertexCountPerInstance,	// Vertex count per instance (UINT)
					drawInstancedArguments.instanceCount,			// Instance count (UINT)
					drawInstancedArguments.startVertexLocation,		// Start vertex location (UINT)
					drawInstancedArguments.startInstanceLocation	// Start instance location (UINT)
				);
			}
			else
			{
				// Without instancing
				mD3D10Device->Draw(
					drawInstancedArguments.vertexCountPerInstance,	// Vertex count (UINT)
					drawInstancedArguments.startVertexLocation		// Start index location (UINT)
				);
			}

			// Advance
			emulationData += sizeof(Renderer::DrawInstancedArguments);
		}
	}

	void Direct3D10Renderer::drawIndexedEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
	{
		// Get indirect buffer data and perform security checks
		assert(nullptr != emulationData);

		// TODO(co) Currently no buffer overflow check due to lack of interface provided data
		emulationData += indirectBufferOffset;

		// Emit the draw calls
		for (uint32_t i = 0; i < numberOfDraws; ++i)
		{
			const Renderer::DrawIndexedInstancedArguments& drawIndexedInstancedArguments = *reinterpret_cast<const Renderer::DrawIndexedInstancedArguments*>(emulationData);

			// Draw
			if (drawIndexedInstancedArguments.instanceCount > 1)
			{
				// With instancing
				mD3D10Device->DrawIndexedInstanced(
					drawIndexedInstancedArguments.indexCountPerInstance,	// Index count per instance (UINT)
					drawIndexedInstancedArguments.instanceCount,			// Instance count (UINT)
					drawIndexedInstancedArguments.startIndexLocation,		// Start index location (UINT)
					drawIndexedInstancedArguments.baseVertexLocation,		// Base vertex location (INT)
					drawIndexedInstancedArguments.startInstanceLocation		// Start instance location (UINT)
				);
			}
			else
			{
				// Without instancing
				mD3D10Device->DrawIndexed(
					drawIndexedInstancedArguments.indexCountPerInstance,	// Index count (UINT)
					drawIndexedInstancedArguments.startIndexLocation,		// Start index location (UINT)
					drawIndexedInstancedArguments.baseVertexLocation		// Base vertex location (INT)
				);
			}

			// Advance
			emulationData += sizeof(Renderer::DrawIndexedInstancedArguments);
		}
	}


	//[-------------------------------------------------------]
	//[ Debug                                                 ]
	//[-------------------------------------------------------]
	void Direct3D10Renderer::setDebugMarker(const char *name)
	{
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			// Create the Direct3D 9 runtime linking instance, in case there's no one, yet
			if (nullptr == mDirect3D9RuntimeLinking)
			{
				mDirect3D9RuntimeLinking = new Direct3D9RuntimeLinking();
			}

			// Call the Direct3D 9 PIX function
			if (mDirect3D9RuntimeLinking->isDirect3D9Avaiable())
			{
				assert(strlen(name) < 256);
				wchar_t unicodeName[256];
				std::mbstowcs(unicodeName, name, 256);
				D3DPERF_SetMarker(D3DCOLOR_RGBA(255, 0, 255, 255), unicodeName);
			}
		#endif
	}

	void Direct3D10Renderer::beginDebugEvent(const char *name)
	{
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			// Create the Direct3D 9 runtime linking instance, in case there's no one, yet
			if (nullptr == mDirect3D9RuntimeLinking)
			{
				mDirect3D9RuntimeLinking = new Direct3D9RuntimeLinking();
			}

			// Call the Direct3D 9 PIX function
			if (mDirect3D9RuntimeLinking->isDirect3D9Avaiable())
			{
				assert(strlen(name) < 256);
				wchar_t unicodeName[256];
				std::mbstowcs(unicodeName, name, 256);
				D3DPERF_BeginEvent(D3DCOLOR_RGBA(255, 255, 255, 255), unicodeName);
			}
		#endif
	}

	void Direct3D10Renderer::endDebugEvent()
	{
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			// Create the Direct3D 9 runtime linking instance, in case there's no one, yet
			if (nullptr == mDirect3D9RuntimeLinking)
			{
				mDirect3D9RuntimeLinking = new Direct3D9RuntimeLinking();
			}

			// Call the Direct3D 9 PIX function
			if (mDirect3D9RuntimeLinking->isDirect3D9Avaiable())
			{
				D3DPERF_EndEvent();
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	bool Direct3D10Renderer::isDebugEnabled()
	{
		// Don't check for the "DIRECT3D9RENDERER_NO_DEBUG" preprocessor definition, even if debug
		// is disabled it has to be possible to use this function for an additional security check
		// -> Maybe a debugger/profiler ignores the debug state
		// -> Maybe someone manipulated the binary to enable the debug state, adding a second check
		//    makes it a little bit more time consuming to hack the binary :D (but of course, this is no 100% security)
		return (D3DPERF_GetStatus() != 0);
	}

	Renderer::ISwapChain *Direct3D10Renderer::getMainSwapChain() const
	{
		return mMainSwapChain;
	}


	//[-------------------------------------------------------]
	//[ Shader language                                       ]
	//[-------------------------------------------------------]
	uint32_t Direct3D10Renderer::getNumberOfShaderLanguages() const
	{
		uint32_t numberOfShaderLanguages = 1;	// HLSL support is always there

		// Done, return the number of supported shader languages
		return numberOfShaderLanguages;
	}

	const char *Direct3D10Renderer::getShaderLanguageName(uint32_t index) const
	{
		uint32_t currentIndex = 0;

		// HLSL supported
		if (currentIndex == index)
		{
			return ShaderLanguageHlsl::NAME;
		}
		++currentIndex;

		// Error!
		return nullptr;
	}

	Renderer::IShaderLanguage *Direct3D10Renderer::getShaderLanguage(const char *shaderLanguageName)
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
	Renderer::ISwapChain *Direct3D10Renderer::createSwapChain(handle nativeWindowHandle)
	{
		// The provided native window handle must not be a null handle
		return (NULL_HANDLE != nativeWindowHandle) ? new SwapChain(*this, nativeWindowHandle) : nullptr;
	}

	Renderer::IFramebuffer *Direct3D10Renderer::createFramebuffer(uint32_t numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture)
	{
		// Validation is done inside the framebuffer implementation
		return new Framebuffer(*this, numberOfColorTextures, colorTextures, depthStencilTexture);
	}

	Renderer::IBufferManager *Direct3D10Renderer::createBufferManager()
	{
		return new BufferManager(*this);
	}

	Renderer::ITextureManager *Direct3D10Renderer::createTextureManager()
	{
		return new TextureManager(*this);
	}

	Renderer::IRootSignature *Direct3D10Renderer::createRootSignature(const Renderer::RootSignature &rootSignature)
	{
		return new RootSignature(*this, rootSignature);
	}

	Renderer::IPipelineState *Direct3D10Renderer::createPipelineState(const Renderer::PipelineState &pipelineState)
	{
		return new PipelineState(*this, pipelineState);
	}

	Renderer::ISamplerState *Direct3D10Renderer::createSamplerState(const Renderer::SamplerState &samplerState)
	{
		return new SamplerState(*this, samplerState);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool Direct3D10Renderer::map(Renderer::IResource &resource, uint32_t subresource, Renderer::MapType mapType, uint32_t mapFlags, Renderer::MappedSubresource &mappedSubresource)
	{
		// The "Renderer::MapType" values directly map to Direct3D 10 & 11 constants, do not change them
		// The "Renderer::MappedSubresource" structure directly maps to Direct3D 11, do not change it

		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;
				return (S_OK == static_cast<IndexBuffer&>(resource).getD3D10Buffer()->Map(static_cast<D3D10_MAP>(mapType), mapFlags, &mappedSubresource.data));

			case Renderer::ResourceType::VERTEX_BUFFER:
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;
				return (S_OK == static_cast<VertexBuffer&>(resource).getD3D10Buffer()->Map(static_cast<D3D10_MAP>(mapType), mapFlags, &mappedSubresource.data));

			case Renderer::ResourceType::UNIFORM_BUFFER:
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;
				return (S_OK == static_cast<UniformBuffer&>(resource).getD3D10Buffer()->Map(static_cast<D3D10_MAP>(mapType), mapFlags, &mappedSubresource.data));

			case Renderer::ResourceType::TEXTURE_BUFFER:
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;
				return (S_OK == static_cast<TextureBuffer&>(resource).getD3D10Buffer()->Map(static_cast<D3D10_MAP>(mapType), mapFlags, &mappedSubresource.data));

			case Renderer::ResourceType::INDIRECT_BUFFER:
				mappedSubresource.data		 = static_cast<IndirectBuffer&>(resource).getWritableEmulationData();
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;
				return true;

			case Renderer::ResourceType::TEXTURE_2D:
			{
				bool result = false;

				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

				// Get the Direct3D 10 resource instance
				// -> The user is asked to not manipulate the view, so the cast to "ID3D10Resource" is assumed to be safe in here
				ID3D10Texture2D *d3d10Texture2D = nullptr;
				D3D10_MAPPED_TEXTURE2D d3d10MappedTexture2D;
				static_cast<Texture2D&>(resource).getD3D10ShaderResourceView()->GetResource(reinterpret_cast<ID3D10Resource**>(&d3d10Texture2D));
				if (nullptr != d3d10Texture2D)
				{
					// Map the Direct3D 10 resource
					result = (S_OK == d3d10Texture2D->Map(subresource, static_cast<D3D10_MAP>(mapType), mapFlags, &d3d10MappedTexture2D));

					// Release the Direct3D 10 resource instance
					d3d10Texture2D->Release();
				}
				else
				{
					// Error!
					memset(&d3d10MappedTexture2D, 0, sizeof(D3D10_MAPPED_TEXTURE2D));
				}

				// Set result
				if (result)
				{
					// Copy over the data
					mappedSubresource.data		 = d3d10MappedTexture2D.pData;
					mappedSubresource.rowPitch   = d3d10MappedTexture2D.RowPitch;
					mappedSubresource.depthPitch = 0;
				}
				else
				{
					// Set known return values in case of an error
					mappedSubresource.data		 = nullptr;
					mappedSubresource.rowPitch   = 0;
					mappedSubresource.depthPitch = 0;
				}

				// End debug event
				RENDERER_END_DEBUG_EVENT(this)

				// Done
				return result;
			}

			case Renderer::ResourceType::TEXTURE_2D_ARRAY:
			{
				bool result = false;

				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

				// Get the Direct3D 10 resource instance
				// -> The user is asked to not manipulate the view, so the cast to "ID3D10Resource" is assumed to be safe in here
				ID3D10Texture2D *d3d10Texture2D = nullptr;
				D3D10_MAPPED_TEXTURE2D d3d10MappedTexture2D;
				static_cast<Texture2DArray&>(resource).getD3D10ShaderResourceView()->GetResource(reinterpret_cast<ID3D10Resource**>(&d3d10Texture2D));
				if (nullptr != d3d10Texture2D)
				{
					// Map the Direct3D 10 resource
					result = (S_OK == d3d10Texture2D->Map(subresource, static_cast<D3D10_MAP>(mapType), mapFlags, &d3d10MappedTexture2D));

					// Release the Direct3D 10 resource instance
					d3d10Texture2D->Release();
				}
				else
				{
					// Error!
					memset(&d3d10MappedTexture2D, 0, sizeof(D3D10_MAPPED_TEXTURE2D));
				}

				// Set result
				if (result)
				{
					// Copy over the data
					mappedSubresource.data		 = d3d10MappedTexture2D.pData;
					mappedSubresource.rowPitch   = d3d10MappedTexture2D.RowPitch;
					mappedSubresource.depthPitch = 0;
				}
				else
				{
					// Set known return values in case of an error
					mappedSubresource.data		 = nullptr;
					mappedSubresource.rowPitch   = 0;
					mappedSubresource.depthPitch = 0;
				}

				// End debug event
				RENDERER_END_DEBUG_EVENT(this)

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

	void Direct3D10Renderer::unmap(Renderer::IResource &resource, uint32_t subresource)
	{
		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
				static_cast<IndexBuffer&>(resource).getD3D10Buffer()->Unmap();
				break;

			case Renderer::ResourceType::VERTEX_BUFFER:
				static_cast<VertexBuffer&>(resource).getD3D10Buffer()->Unmap();
				break;

			case Renderer::ResourceType::UNIFORM_BUFFER:
				static_cast<UniformBuffer&>(resource).getD3D10Buffer()->Unmap();
				break;

			case Renderer::ResourceType::TEXTURE_BUFFER:
				static_cast<TextureBuffer&>(resource).getD3D10Buffer()->Unmap();
				break;

			case Renderer::ResourceType::INDIRECT_BUFFER:
				// Nothing here, it's a software emulated indirect buffer
				break;

			case Renderer::ResourceType::TEXTURE_2D:
			{
				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

				// Get the Direct3D 10 resource instance
				// -> The user is asked to not manipulate the view, so the cast to "ID3D10Resource" is assumed to be safe in here
				ID3D10Texture2D *d3d10Texture2D = nullptr;
				static_cast<Texture2D&>(resource).getD3D10ShaderResourceView()->GetResource(reinterpret_cast<ID3D10Resource**>(&d3d10Texture2D));
				if (nullptr != d3d10Texture2D)
				{
					// Unmap the Direct3D 10 resource
					d3d10Texture2D->Unmap(subresource);

					// Release the Direct3D 10 resource instance
					d3d10Texture2D->Release();
				}

				// End debug event
				RENDERER_END_DEBUG_EVENT(this)
				break;
			}

			case Renderer::ResourceType::TEXTURE_2D_ARRAY:
			{
				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

				// Get the Direct3D 10 resource instance
				// -> The user is asked to not manipulate the view, so the cast to "ID3D10Resource" is assumed to be safe in here
				ID3D10Texture2D *d3d10Texture2D = nullptr;
				static_cast<Texture2DArray&>(resource).getD3D10ShaderResourceView()->GetResource(reinterpret_cast<ID3D10Resource**>(&d3d10Texture2D));
				if (nullptr != d3d10Texture2D)
				{
					// Unmap the Direct3D 10 resource
					d3d10Texture2D->Unmap(subresource);

					// Release the Direct3D 10 resource instance
					d3d10Texture2D->Release();
				}

				// End debug event
				RENDERER_END_DEBUG_EVENT(this)
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
	bool Direct3D10Renderer::beginScene()
	{
		// Not required when using Direct3D 10

		// Done
		return true;
	}

	void Direct3D10Renderer::submitCommandBuffer(const Renderer::CommandBuffer& commandBuffer)
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

	void Direct3D10Renderer::endScene()
	{
		// We need to forget about the currently set render target
		omSetRenderTarget(nullptr);
	}


	//[-------------------------------------------------------]
	//[ Synchronization                                       ]
	//[-------------------------------------------------------]
	void Direct3D10Renderer::flush()
	{
		mD3D10Device->Flush();
	}

	void Direct3D10Renderer::finish()
	{
		// Create the Direct3D 10 query instance used for flush right now?
		if (nullptr == mD3D10QueryFlush)
		{
			D3D10_QUERY_DESC d3d10QueryDesc;
			d3d10QueryDesc.Query	  = D3D10_QUERY_EVENT;
			d3d10QueryDesc.MiscFlags = 0;
			mD3D10Device->CreateQuery(&d3d10QueryDesc, &mD3D10QueryFlush);

			#ifndef DIRECT3D10RENDERER_NO_DEBUG
				// Set the debug name
				if (nullptr != mD3D10QueryFlush)
				{
					// No need to reset the previous private data, there shouldn't be any...
					mD3D10QueryFlush->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(__FUNCTION__), __FUNCTION__);
				}
			#endif
		}
		if (nullptr != mD3D10QueryFlush)
		{
			// Perform the flush and wait
			mD3D10QueryFlush->End();
			mD3D10Device->Flush();
			BOOL result = FALSE;
			do
			{
				// Spin-wait
				mD3D10QueryFlush->GetData(&result, sizeof(BOOL), 0);
			} while (!result);
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void Direct3D10Renderer::initializeCapabilities()
	{
		// There are no Direct3D 10 device capabilities we could query on runtime
		// -> Have a look at "Resource Limits (Direct3D 10)" at MSDN http://msdn.microsoft.com/en-us/library/cc308052%28VS.85%29.aspx
		//    for a table with a list of the minimum resources supported by Direct3D 10

		// Maximum number of viewports (always at least 1)
		mCapabilities.maximumNumberOfViewports = D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX + 1;

		// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
		mCapabilities.maximumNumberOfSimultaneousRenderTargets = D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;

		// Maximum texture dimension
		mCapabilities.maximumTextureDimension = 8192;

		// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
		mCapabilities.maximumNumberOf2DTextureArraySlices = 512;

		// Maximum uniform buffer (UBO) size in bytes (usually at least 4096 * 16 bytes, in case there's no support for uniform buffer it's 0)
		// -> See https://msdn.microsoft.com/en-us/library/windows/desktop/cc308052(v=vs.85).aspx - "Resource Limits (Direct3D 10)" - "Number of elements in a constant buffer 4096"
		// -> One element = float4 = 16 bytes
		mCapabilities.maximumUniformBufferSize = 4096 * 16;

		// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
		mCapabilities.maximumTextureBufferSize = 128 * 1024 * 1024;	// TODO(co) http://msdn.microsoft.com/en-us/library/cc308052%28VS.85%29.aspx does not mention the texture buffer? Figure out the correct size! Currently the OpenGL 3 minimum is used: 128 MiB.

		// Maximum indirect buffer size in bytes (in case there's no support for indirect buffer it's 0)
		// -> DirectX 10 has no indirect buffer
		mCapabilities.maximumIndirectBufferSize = sizeof(Renderer::DrawIndexedInstancedArguments) * 4096;	// TODO(co) What is an usually decent emulated indirect buffer size?

		// Maximum number of multisamples (always at least 1, usually 8)
		// TODO(co) Currently Direct3D 10 instead of Direct3D 10.1 is used causing
		// "D3D11 ERROR: ID3D10Device::CreateTexture2D: If the feature level is less than D3D_FEATURE_LEVEL_10_1, a Texture2D with sample count > 1 cannot have both D3D11_BIND_DEPTH_STENCIL and D3D11_BIND_SHADER_RESOURCE.  This call may appear to incorrectly return success on older/current D3D runtimes due to missing validation, despite this debug layer message.  [ STATE_CREATION ERROR #99: CREATETEXTURE2D_INVALIDBINDFLAGS]"
		// error messages when trying to create a depth texture render target which one also wants to read from inside shaders. The Direct3D 10 renderer backend is still maintained for curiosity reasons,
		// but it's not really worth to put more effort into it to be able to handle the lack of certain features. So, just say this renderer backend doesn't support multisampling at all.
		mCapabilities.maximumNumberOfMultisamples = 1;

		// Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		mCapabilities.individualUniforms = false;

		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
		mCapabilities.instancedArrays = true;

		// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID)
		mCapabilities.drawInstanced = true;

		// Base vertex supported for draw calls?
		mCapabilities.baseVertex = true;

		// Direct3D 10 has native multi-threading
		// -> https://msdn.microsoft.com/de-de/library/windows/desktop/bb205068(v=vs.85).aspx - "Unlike Direct3D 9, the Direct3D 10 API defaults to fully thread-safe"
		mCapabilities.nativeMultiThreading = true;

		// Is there support for vertex shaders (VS)?
		mCapabilities.vertexShader = true;

		// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
		mCapabilities.maximumNumberOfPatchVertices = 0;	// Direct3D 10 has no tessellation support

		// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
		mCapabilities.maximumNumberOfGsOutputVertices = 1024;

		// Is there support for fragment shaders (FS)?
		mCapabilities.fragmentShader = true;
	}

	void Direct3D10Renderer::setProgram(Renderer::IProgram *program)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		// TODO(co) Avoid changing already set program

		if (nullptr != program)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *program)

			// TODO(co) HLSL buffer settings, unset previous program

			// Get shaders
			const ProgramHlsl		 *programHlsl		 = static_cast<ProgramHlsl*>(program);
			const VertexShaderHlsl	 *vertexShaderHlsl	 = programHlsl->getVertexShaderHlsl();
			const GeometryShaderHlsl *geometryShaderHlsl = programHlsl->getGeometryShaderHlsl();
			const FragmentShaderHlsl *fragmentShaderHlsl = programHlsl->getFragmentShaderHlsl();

			// Set shaders
			mD3D10Device->VSSetShader(vertexShaderHlsl	 ? vertexShaderHlsl->  getD3D10VertexShader()	: nullptr);
			mD3D10Device->GSSetShader(geometryShaderHlsl ? geometryShaderHlsl->getD3D10GeometryShader() : nullptr);
			mD3D10Device->PSSetShader(fragmentShaderHlsl ? fragmentShaderHlsl->getD3D10PixelShader()	: nullptr);
		}
		else
		{
			// TODO(co) HLSL buffer settings
			mD3D10Device->VSSetShader(nullptr);
			mD3D10Device->GSSetShader(nullptr);
			mD3D10Device->PSSetShader(nullptr);
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
