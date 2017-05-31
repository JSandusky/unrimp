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
#include "Direct3D11Renderer/Direct3D11Renderer.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/Direct3D11Debug.h"	// For "DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "Direct3D11Renderer/Direct3D11RuntimeLinking.h"
#include "Direct3D11Renderer/RootSignature.h"
#include "Direct3D11Renderer/Mapping.h"
#include "Direct3D11Renderer/RenderTarget/SwapChain.h"
#include "Direct3D11Renderer/RenderTarget/Framebuffer.h"
#include "Direct3D11Renderer/Buffer/BufferManager.h"
#include "Direct3D11Renderer/Buffer/VertexArray.h"
#include "Direct3D11Renderer/Buffer/IndexBuffer.h"
#include "Direct3D11Renderer/Buffer/VertexBuffer.h"
#include "Direct3D11Renderer/Buffer/UniformBuffer.h"
#include "Direct3D11Renderer/Buffer/TextureBuffer.h"
#include "Direct3D11Renderer/Buffer/IndirectBuffer.h"
#include "Direct3D11Renderer/Texture/TextureManager.h"
#include "Direct3D11Renderer/Texture/Texture1D.h"
#include "Direct3D11Renderer/Texture/Texture2D.h"
#include "Direct3D11Renderer/Texture/Texture3D.h"
#include "Direct3D11Renderer/Texture/TextureCube.h"
#include "Direct3D11Renderer/Texture/Texture2DArray.h"
#include "Direct3D11Renderer/State/SamplerState.h"
#include "Direct3D11Renderer/State/PipelineState.h"
#include "Direct3D11Renderer/Shader/ProgramHlsl.h"
#include "Direct3D11Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D11Renderer/Shader/VertexShaderHlsl.h"
#include "Direct3D11Renderer/Shader/GeometryShaderHlsl.h"
#include "Direct3D11Renderer/Shader/FragmentShaderHlsl.h"
#include "Direct3D11Renderer/Shader/TessellationControlShaderHlsl.h"
#include "Direct3D11Renderer/Shader/TessellationEvaluationShaderHlsl.h"

#include <Renderer/Buffer/CommandBuffer.h>
#include <Renderer/Buffer/IndirectBufferTypes.h>

#include <tuple>	// For "std::ignore"


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef DIRECT3D11RENDERER_EXPORTS
	#define DIRECT3D11RENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define DIRECT3D11RENDERER_API_EXPORT
#endif
DIRECT3D11RENDERER_API_EXPORT Renderer::IRenderer *createDirect3D11RendererInstance(handle nativeWindowHandle, bool useExternalContext)
{
	std::ignore = useExternalContext;
	return new Direct3D11Renderer::Direct3D11Renderer(nativeWindowHandle);
}
#undef DIRECT3D11RENDERER_API_EXPORT


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
		bool createDevice(UINT flags, ID3D11Device** d3d11Device, ID3D11DeviceContext** d3d11DeviceContext)
		{
			// Driver types
			static const D3D_DRIVER_TYPE D3D_DRIVER_TYPES[] =
			{
				D3D_DRIVER_TYPE_HARDWARE,
				D3D_DRIVER_TYPE_WARP,
				D3D_DRIVER_TYPE_REFERENCE,
			};
			static const UINT NUMBER_OF_DRIVER_TYPES = sizeof(D3D_DRIVER_TYPES) / sizeof(D3D_DRIVER_TYPE);

			// Feature levels
			static const D3D_FEATURE_LEVEL D3D_FEATURE_LEVELS[] =
			{
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
			};
			static const UINT NUMBER_OF_FEATURE_LEVELS = sizeof(D3D_FEATURE_LEVELS) / sizeof(D3D_FEATURE_LEVEL);

			// Create the Direct3D 11 device
			for (UINT deviceType = 0; deviceType < NUMBER_OF_DRIVER_TYPES; ++deviceType)
			{
				D3D_FEATURE_LEVEL d3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;
				if (SUCCEEDED(Direct3D11Renderer::D3D11CreateDevice(nullptr, D3D_DRIVER_TYPES[deviceType], nullptr, flags, D3D_FEATURE_LEVELS, NUMBER_OF_FEATURE_LEVELS, D3D11_SDK_VERSION, d3d11Device, &d3dFeatureLevel, d3d11DeviceContext)))
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
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).setGraphicsRootSignature(realData->rootSignature);
			}

			void SetGraphicsRootDescriptorTable(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetGraphicsRootDescriptorTable* realData = static_cast<const Renderer::Command::SetGraphicsRootDescriptorTable*>(data);
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).setGraphicsRootDescriptorTable(realData->rootParameterIndex, realData->resource);
			}

			//[-------------------------------------------------------]
			//[ States                                                ]
			//[-------------------------------------------------------]
			void SetPipelineState(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetPipelineState* realData = static_cast<const Renderer::Command::SetPipelineState*>(data);
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).setPipelineState(realData->pipelineState);
			}

			//[-------------------------------------------------------]
			//[ Input-assembler (IA) stage                            ]
			//[-------------------------------------------------------]
			void SetVertexArray(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetVertexArray* realData = static_cast<const Renderer::Command::SetVertexArray*>(data);
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).iaSetVertexArray(realData->vertexArray);
			}

			void SetPrimitiveTopology(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetPrimitiveTopology* realData = static_cast<const Renderer::Command::SetPrimitiveTopology*>(data);
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).iaSetPrimitiveTopology(realData->primitiveTopology);
			}

			//[-------------------------------------------------------]
			//[ Rasterizer (RS) stage                                 ]
			//[-------------------------------------------------------]
			void SetViewports(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetViewports* realData = static_cast<const Renderer::Command::SetViewports*>(data);
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).rsSetViewports(realData->numberOfViewports, (nullptr != realData->viewports) ? realData->viewports : reinterpret_cast<const Renderer::Viewport*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
			}

			void SetScissorRectangles(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetScissorRectangles* realData = static_cast<const Renderer::Command::SetScissorRectangles*>(data);
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).rsSetScissorRectangles(realData->numberOfScissorRectangles, (nullptr != realData->scissorRectangles) ? realData->scissorRectangles : reinterpret_cast<const Renderer::ScissorRectangle*>(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData)));
			}

			//[-------------------------------------------------------]
			//[ Output-merger (OM) stage                              ]
			//[-------------------------------------------------------]
			void SetRenderTarget(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetRenderTarget* realData = static_cast<const Renderer::Command::SetRenderTarget*>(data);
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).omSetRenderTarget(realData->renderTarget);
			}

			//[-------------------------------------------------------]
			//[ Operations                                            ]
			//[-------------------------------------------------------]
			void Clear(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Clear* realData = static_cast<const Renderer::Command::Clear*>(data);
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).clear(realData->flags, realData->color, realData->z, realData->stencil);
			}

			void ResolveMultisampleFramebuffer(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::ResolveMultisampleFramebuffer* realData = static_cast<const Renderer::Command::ResolveMultisampleFramebuffer*>(data);
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).resolveMultisampleFramebuffer(*realData->destinationRenderTarget, *realData->sourceMultisampleFramebuffer);
			}

			void CopyResource(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::CopyResource* realData = static_cast<const Renderer::Command::CopyResource*>(data);
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).copyResource(*realData->destinationResource, *realData->sourceResource);
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
					// TODO(co) Implement indirect buffer support, see e.g. "Voxel visualization using DrawIndexedInstancedIndirect" - http://www.alexandre-pestana.com/tag/directx/ for hints
					static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).drawEmulated(realData->indirectBuffer->getEmulationData(), realData->indirectBufferOffset, realData->numberOfDraws);
				}
				else
				{
					static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).drawEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
				}
			}

			void DrawIndexed(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::Draw* realData = static_cast<const Renderer::Command::Draw*>(data);
				if (nullptr != realData->indirectBuffer)
				{
					// No resource owner security check in here, we only support emulated indirect buffer
					// TODO(co) Implement indirect buffer support, see e.g. "Voxel visualization using DrawIndexedInstancedIndirect" - http://www.alexandre-pestana.com/tag/directx/ for hints
					static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).drawIndexedEmulated(realData->indirectBuffer->getEmulationData(), realData->indirectBufferOffset, realData->numberOfDraws);
				}
				else
				{
					static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).drawIndexedEmulated(Renderer::CommandPacketHelper::getAuxiliaryMemory(realData), realData->indirectBufferOffset, realData->numberOfDraws);
				}
			}

			//[-------------------------------------------------------]
			//[ Debug                                                 ]
			//[-------------------------------------------------------]
			void SetDebugMarker(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::SetDebugMarker* realData = static_cast<const Renderer::Command::SetDebugMarker*>(data);
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).setDebugMarker(realData->name);
			}

			void BeginDebugEvent(const void* data, Renderer::IRenderer& renderer)
			{
				const Renderer::Command::BeginDebugEvent* realData = static_cast<const Renderer::Command::BeginDebugEvent*>(data);
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).beginDebugEvent(realData->name);
			}

			void EndDebugEvent(const void*, Renderer::IRenderer& renderer)
			{
				static_cast<Direct3D11Renderer::Direct3D11Renderer&>(renderer).endDebugEvent();
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
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Direct3D11Renderer::Direct3D11Renderer(handle nativeWindowHandle) :
		mDirect3D11RuntimeLinking(new Direct3D11RuntimeLinking()),
		mD3D11Device(nullptr),
		mD3D11DeviceContext(nullptr),
		mD3DUserDefinedAnnotation(nullptr),
		mShaderLanguageHlsl(nullptr),
		mD3D11QueryFlush(nullptr),
		mMainSwapChain(nullptr),
		mRenderTarget(nullptr),
		mGraphicsRootSignature(nullptr),
		mD3d11VertexShader(nullptr),
		mD3d11HullShader(nullptr),
		mD3d11DomainShader(nullptr),
		mD3d11GeometryShader(nullptr),
		mD3d11PixelShader(nullptr)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		// Is Direct3D 11 available?
		if (mDirect3D11RuntimeLinking->isDirect3D11Avaiable())
		{
			// Flags
			UINT flags = 0;
			#ifdef _DEBUG
				flags |= D3D11_CREATE_DEVICE_DEBUG;
			#endif

			// Create the Direct3D 11 device
			if (!detail::createDevice(flags, &mD3D11Device, &mD3D11DeviceContext) && (flags & D3D11_CREATE_DEVICE_DEBUG))
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Failed to create device instance, retrying without debug flag (maybe no Windows SDK is installed)")
				flags &= ~D3D11_CREATE_DEVICE_DEBUG;
				detail::createDevice(flags, &mD3D11Device, &mD3D11DeviceContext);
			}

			// Is there a valid Direct3D 11 device and device context?
			if (nullptr != mD3D11Device && nullptr != mD3D11DeviceContext)
			{
				// Direct3D 11 debug related stuff
				if (flags & D3D11_CREATE_DEVICE_DEBUG)
				{
					#ifndef DIRECT3D11RENDERER_NO_DEBUG
						// Try to get the Direct3D 11 user defined annotation interface, Direct3D 11.1 feature
						mD3D11DeviceContext->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), reinterpret_cast<LPVOID*>(&mD3DUserDefinedAnnotation));
					#endif

					// Direct3D 11 debug settings
					ID3D11Debug* d3d11Debug = nullptr;
					if (SUCCEEDED(mD3D11Device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<LPVOID*>(&d3d11Debug))))
					{
						ID3D11InfoQueue* d3d11InfoQueue = nullptr;
						if (SUCCEEDED(d3d11Debug->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<LPVOID*>(&d3d11InfoQueue))))
						{
							// When using render-to-texture, Direct3D 11 will quickly spam the log with
							//   "
							//   D3D11 WARNING: ID3D11DeviceContext::OMSetRenderTargets: Resource being set to OM RenderTarget slot 0 is still bound on input! [ STATE_SETTING WARNING #9: DEVICE_OMSETRENDERTARGETS_HAZARD]
							//   D3D11 WARNING: ID3D11DeviceContext::OMSetRenderTargets[AndUnorderedAccessViews]: Forcing VS shader resource slot 0 to NULL. [ STATE_SETTING WARNING #3: DEVICE_VSSETSHADERRESOURCES_HAZARD]
							//   D3D11 WARNING: ID3D11DeviceContext::OMSetRenderTargets[AndUnorderedAccessViews]: Forcing HS shader resource slot 0 to NULL. [ STATE_SETTING WARNING #2097173: DEVICE_HSSETSHADERRESOURCES_HAZARD]
							//   D3D11 WARNING: ID3D11DeviceContext::OMSetRenderTargets[AndUnorderedAccessViews]: Forcing DS shader resource slot 0 to NULL. [ STATE_SETTING WARNING #2097189: DEVICE_DSSETSHADERRESOURCES_HAZARD]
							//   D3D11 WARNING: ID3D11DeviceContext::OMSetRenderTargets[AndUnorderedAccessViews]: Forcing GS shader resource slot 0 to NULL. [ STATE_SETTING WARNING #5: DEVICE_GSSETSHADERRESOURCES_HAZARD]
							//   D3D11 WARNING: ID3D11DeviceContext::OMSetRenderTargets[AndUnorderedAccessViews]: Forcing PS shader resource slot 0 to NULL. [ STATE_SETTING WARNING #7: DEVICE_PSSETSHADERRESOURCES_HAZARD]
							//   "
							// When not unbinding render targets from shader resources, even if shaders never access the render target by reading. We could add extra
							// logic to avoid this situation, but on the other hand, the renderer backend should be as slim as possible. Since those Direct3D 11 warnings
							// are pretty annoying and introduce the risk of missing relevant warnings, let's suppress those warnings. Thought about this for a while, feels
							// like the best solution considering the alternatives even if suppressing warnings is not always the best idea.
							D3D11_MESSAGE_ID d3d11MessageIds[] =
							{
								D3D11_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD,
								D3D11_MESSAGE_ID_DEVICE_VSSETSHADERRESOURCES_HAZARD,
								D3D11_MESSAGE_ID_DEVICE_GSSETSHADERRESOURCES_HAZARD,
								D3D11_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD,
								D3D11_MESSAGE_ID_DEVICE_HSSETSHADERRESOURCES_HAZARD,
								D3D11_MESSAGE_ID_DEVICE_DSSETSHADERRESOURCES_HAZARD
							};
							D3D11_INFO_QUEUE_FILTER d3d11InfoQueueFilter = {};
							d3d11InfoQueueFilter.DenyList.NumIDs = _countof(d3d11MessageIds);
							d3d11InfoQueueFilter.DenyList.pIDList = d3d11MessageIds;
							d3d11InfoQueue->AddStorageFilterEntries(&d3d11InfoQueueFilter);

							// TODO(co) If would be nice to break by default on everything, on the other hand there's no usable callstack then which renders this somewhat useless
							// d3d11InfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
							// d3d11InfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
							// d3d11InfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, true);
							// d3d11InfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_INFO, true);

							d3d11InfoQueue->Release();
						}
						d3d11Debug->Release();
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
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Failed to create device and device context instance")
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}

	Direct3D11Renderer::~Direct3D11Renderer()
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

		#ifndef RENDERER_NO_STATISTICS
		{ // For debugging: At this point there should be no resource instances left, validate this!
			// -> Are the currently any resource instances?
			const unsigned long numberOfCurrentResources = getStatistics().getNumberOfCurrentResources();
			if (numberOfCurrentResources > 0)
			{
				// Error!
				if (numberOfCurrentResources > 1)
				{
					RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 11 error: Renderer is going to be destroyed, but there are still %d resource instances left (memory leak)\n", numberOfCurrentResources)
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Renderer is going to be destroyed, but there is still one resource instance left (memory leak)\n")
				}

				// Use debug output to show the current number of resource instances
				getStatistics().debugOutputCurrentResouces();
			}
		}
		#endif

		// Release the Direct3D 11 query instance used for flush, in case we have one
		if (nullptr != mD3D11QueryFlush)
		{
			mD3D11QueryFlush->Release();
		}

		// Release the HLSL shader language instance, in case we have one
		if (nullptr != mShaderLanguageHlsl)
		{
			mShaderLanguageHlsl->releaseReference();
		}

		// Release the Direct3D 11 device we've created
		if (nullptr != mD3DUserDefinedAnnotation)
		{
			mD3DUserDefinedAnnotation->Release();
			mD3DUserDefinedAnnotation = nullptr;
		}
		if (nullptr != mD3D11DeviceContext)
		{
			mD3D11DeviceContext->Release();
			mD3D11DeviceContext = nullptr;
		}
		if (nullptr != mD3D11Device)
		{
			mD3D11Device->Release();
			mD3D11Device = nullptr;
		}

		// Destroy the Direct3D 11 runtime linking instance
		delete mDirect3D11RuntimeLinking;

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void Direct3D11Renderer::setGraphicsRootSignature(Renderer::IRootSignature *rootSignature)
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
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *rootSignature)
		}
	}

	void Direct3D11Renderer::setGraphicsRootDescriptorTable(uint32_t rootParameterIndex, Renderer::IResource* resource)
	{
		// Security checks
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
		{
			if (nullptr == mGraphicsRootSignature)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: No graphics root signature set")
				return;
			}
			const Renderer::RootSignature& rootSignature = mGraphicsRootSignature->getRootSignature();
			if (rootParameterIndex >= rootSignature.numberOfParameters)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Root parameter index is out of bounds")
				return;
			}
			const Renderer::RootParameter& rootParameter = rootSignature.parameters[rootParameterIndex];
			if (Renderer::RootParameterType::DESCRIPTOR_TABLE != rootParameter.parameterType)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Root parameter index doesn't reference a descriptor table")
				return;
			}

			// TODO(co) For now, we only support a single descriptor range
			if (1 != rootParameter.descriptorTable.numberOfDescriptorRanges)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Only a single descriptor range is supported")
				return;
			}
			if (nullptr == reinterpret_cast<const Renderer::DescriptorRange*>(rootParameter.descriptorTable.descriptorRanges))
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Descriptor ranges is a null pointer")
				return;
			}
		}
		#endif

		if (nullptr != resource)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *resource)

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
					ID3D11Buffer *d3d11Buffers = static_cast<UniformBuffer*>(resource)->getD3D11Buffer();
					const UINT startSlot = descriptorRange->baseShaderRegister;
					switch (rootParameter.shaderVisibility)
					{
						case Renderer::ShaderVisibility::ALL:
							mD3D11DeviceContext->VSSetConstantBuffers(startSlot, 1, &d3d11Buffers);
							mD3D11DeviceContext->HSSetConstantBuffers(startSlot, 1, &d3d11Buffers);
							mD3D11DeviceContext->DSSetConstantBuffers(startSlot, 1, &d3d11Buffers);
							mD3D11DeviceContext->GSSetConstantBuffers(startSlot, 1, &d3d11Buffers);
							mD3D11DeviceContext->PSSetConstantBuffers(startSlot, 1, &d3d11Buffers);
							break;

						case Renderer::ShaderVisibility::VERTEX:
							mD3D11DeviceContext->VSSetConstantBuffers(startSlot, 1, &d3d11Buffers);
							break;

						case Renderer::ShaderVisibility::TESSELLATION_CONTROL:
							// "hull shader" in Direct3D terminology
							mD3D11DeviceContext->HSSetConstantBuffers(startSlot, 1, &d3d11Buffers);
							break;

						case Renderer::ShaderVisibility::TESSELLATION_EVALUATION:
							// "domain shader" in Direct3D terminology
							mD3D11DeviceContext->DSSetConstantBuffers(startSlot, 1, &d3d11Buffers);
							break;

						case Renderer::ShaderVisibility::GEOMETRY:
							mD3D11DeviceContext->GSSetConstantBuffers(startSlot, 1, &d3d11Buffers);
							break;

						case Renderer::ShaderVisibility::FRAGMENT:
							// "pixel shader" in Direct3D terminology
							mD3D11DeviceContext->PSSetConstantBuffers(startSlot, 1, &d3d11Buffers);
							break;
					}
					break;
				}

				case Renderer::ResourceType::TEXTURE_BUFFER:
				case Renderer::ResourceType::TEXTURE_1D:
				case Renderer::ResourceType::TEXTURE_2D:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				case Renderer::ResourceType::TEXTURE_3D:
				case Renderer::ResourceType::TEXTURE_CUBE:
				{
					ID3D11ShaderResourceView *d3d11ShaderResourceView = nullptr;
					switch (resourceType)
					{
						case Renderer::ResourceType::TEXTURE_BUFFER:
							d3d11ShaderResourceView = static_cast<TextureBuffer*>(resource)->getD3D11ShaderResourceView();
							break;

						case Renderer::ResourceType::TEXTURE_1D:
							d3d11ShaderResourceView = static_cast<Texture1D*>(resource)->getD3D11ShaderResourceView();
							break;

						case Renderer::ResourceType::TEXTURE_2D:
							d3d11ShaderResourceView = static_cast<Texture2D*>(resource)->getD3D11ShaderResourceView();
							break;

						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
							d3d11ShaderResourceView = static_cast<Texture2DArray*>(resource)->getD3D11ShaderResourceView();
							break;

						case Renderer::ResourceType::TEXTURE_3D:
							d3d11ShaderResourceView = static_cast<Texture3D*>(resource)->getD3D11ShaderResourceView();
							break;

						case Renderer::ResourceType::TEXTURE_CUBE:
							d3d11ShaderResourceView = static_cast<TextureCube*>(resource)->getD3D11ShaderResourceView();
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
							RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Invalid resource type")
							break;
					}
					const UINT startSlot = descriptorRange->baseShaderRegister;
					switch (rootParameter.shaderVisibility)
					{
						case Renderer::ShaderVisibility::ALL:
							mD3D11DeviceContext->VSSetShaderResources(startSlot, 1, &d3d11ShaderResourceView);
							mD3D11DeviceContext->HSSetShaderResources(startSlot, 1, &d3d11ShaderResourceView);
							mD3D11DeviceContext->DSSetShaderResources(startSlot, 1, &d3d11ShaderResourceView);
							mD3D11DeviceContext->GSSetShaderResources(startSlot, 1, &d3d11ShaderResourceView);
							mD3D11DeviceContext->PSSetShaderResources(startSlot, 1, &d3d11ShaderResourceView);
							break;

						case Renderer::ShaderVisibility::VERTEX:
							mD3D11DeviceContext->VSSetShaderResources(startSlot, 1, &d3d11ShaderResourceView);
							break;

						case Renderer::ShaderVisibility::TESSELLATION_CONTROL:
							// "hull shader" in Direct3D terminology
							mD3D11DeviceContext->HSSetShaderResources(startSlot, 1, &d3d11ShaderResourceView);
							break;

						case Renderer::ShaderVisibility::TESSELLATION_EVALUATION:
							// "domain shader" in Direct3D terminology
							mD3D11DeviceContext->DSSetShaderResources(startSlot, 1, &d3d11ShaderResourceView);
							break;

						case Renderer::ShaderVisibility::GEOMETRY:
							mD3D11DeviceContext->GSSetShaderResources(startSlot, 1, &d3d11ShaderResourceView);
							break;

						case Renderer::ShaderVisibility::FRAGMENT:
							// "pixel shader" in Direct3D terminology
							mD3D11DeviceContext->PSSetShaderResources(startSlot, 1, &d3d11ShaderResourceView);
							break;
					}
					break;
				}

				case Renderer::ResourceType::SAMPLER_STATE:
				{
					ID3D11SamplerState *d3d11SamplerState = static_cast<SamplerState*>(resource)->getD3D11SamplerState();
					const UINT startSlot = descriptorRange->baseShaderRegister;
					switch (rootParameter.shaderVisibility)
					{
						case Renderer::ShaderVisibility::ALL:
							mD3D11DeviceContext->VSSetSamplers(startSlot, 1, &d3d11SamplerState);
							mD3D11DeviceContext->HSSetSamplers(startSlot, 1, &d3d11SamplerState);
							mD3D11DeviceContext->DSSetSamplers(startSlot, 1, &d3d11SamplerState);
							mD3D11DeviceContext->GSSetSamplers(startSlot, 1, &d3d11SamplerState);
							mD3D11DeviceContext->PSSetSamplers(startSlot, 1, &d3d11SamplerState);
							break;

						case Renderer::ShaderVisibility::VERTEX:
							mD3D11DeviceContext->VSSetSamplers(startSlot, 1, &d3d11SamplerState);
							break;

						case Renderer::ShaderVisibility::TESSELLATION_CONTROL:
							// "hull shader" in Direct3D terminology
							mD3D11DeviceContext->HSSetSamplers(startSlot, 1, &d3d11SamplerState);
							break;

						case Renderer::ShaderVisibility::TESSELLATION_EVALUATION:
							// "domain shader" in Direct3D terminology
							mD3D11DeviceContext->DSSetSamplers(startSlot, 1, &d3d11SamplerState);
							break;

						case Renderer::ShaderVisibility::GEOMETRY:
							mD3D11DeviceContext->GSSetSamplers(startSlot, 1, &d3d11SamplerState);
							break;

						case Renderer::ShaderVisibility::FRAGMENT:
							// "pixel shader" in Direct3D terminology
							mD3D11DeviceContext->PSSetSamplers(startSlot, 1, &d3d11SamplerState);
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
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 11 error: Invalid resource type")
					break;
			}
		}
		else
		{
			// TODO(co) Handle this situation?
		}
	}

	void Direct3D11Renderer::setPipelineState(Renderer::IPipelineState* pipelineState)
	{
		if (nullptr != pipelineState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *pipelineState)

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
	void Direct3D11Renderer::iaSetVertexArray(Renderer::IVertexArray *vertexArray)
	{
		if (nullptr != vertexArray)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *vertexArray)

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

			static_cast<VertexArray*>(vertexArray)->setDirect3DIASetInputLayoutAndStreamSource();

			// End debug event
			RENDERER_END_DEBUG_EVENT(this)
		}
		else
		{
			// Set no Direct3D 11 input layout
			mD3D11DeviceContext->IASetInputLayout(nullptr);
		}
	}

	void Direct3D11Renderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology primitiveTopology)
	{
		// Set primitive topology
		// -> The "Renderer::PrimitiveTopology" values directly map to Direct3D 9 & 10 & 11 constants, do not change them
		mD3D11DeviceContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(primitiveTopology));
	}


	//[-------------------------------------------------------]
	//[ Rasterizer (RS) stage                                 ]
	//[-------------------------------------------------------]
	void Direct3D11Renderer::rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport *viewports)
	{
		// Are the given viewports valid?
		if (numberOfViewports > 0 && nullptr != viewports)
		{
			// Set the Direct3D 11 viewports
			// -> "Renderer::Viewport" directly maps to Direct3D 11, do not change it
			// -> Let Direct3D 11 perform the index validation for us (the Direct3D 11 debug features are pretty good)
			mD3D11DeviceContext->RSSetViewports(numberOfViewports, reinterpret_cast<const D3D11_VIEWPORT*>(viewports));
		}
	}

	void Direct3D11Renderer::rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles)
	{
		// Are the given scissor rectangles valid?
		if (numberOfScissorRectangles > 0 && nullptr != scissorRectangles)
		{
			// Set the Direct3D 11 scissor rectangles
			// -> "Renderer::ScissorRectangle" directly maps to Direct3D 9 & 10 & 11, do not change it
			// -> Let Direct3D 11 perform the index validation for us (the Direct3D 11 debug features are pretty good)
			mD3D11DeviceContext->RSSetScissorRects(numberOfScissorRectangles, reinterpret_cast<const D3D11_RECT*>(scissorRectangles));
		}
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	void Direct3D11Renderer::omSetRenderTarget(Renderer::IRenderTarget *renderTarget)
	{
		// New render target?
		if (mRenderTarget != renderTarget)
		{
			// Set a render target?
			if (nullptr != renderTarget)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *renderTarget)

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
						// Get the Direct3D 11 swap chain instance
						SwapChain *swapChain = static_cast<SwapChain*>(mRenderTarget);

						// Direct3D 11 needs a pointer to a pointer, so give it one
						ID3D11RenderTargetView *d3d11RenderTargetView = swapChain->getD3D11RenderTargetView();
						mD3D11DeviceContext->OMSetRenderTargets(1, &d3d11RenderTargetView, swapChain->getD3D11DepthStencilView());
						break;
					}

					case Renderer::ResourceType::FRAMEBUFFER:
					{
						// Get the Direct3D 11 framebuffer instance
						Framebuffer *framebuffer = static_cast<Framebuffer*>(mRenderTarget);

						// Set the Direct3D 11 render targets
						mD3D11DeviceContext->OMSetRenderTargets(framebuffer->getNumberOfColorTextures(), framebuffer->getD3D11RenderTargetViews(), framebuffer->getD3D11DepthStencilView());
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
					framebufferToGenerateMipmapsFor->generateMipmaps(*mD3D11DeviceContext);
					framebufferToGenerateMipmapsFor->releaseReference();
				}
			}
			else
			{
				// Set the Direct3D 11 render targets
				mD3D11DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

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
	void Direct3D11Renderer::clear(uint32_t flags, const float color[4], float z, uint32_t stencil)
	{
		// Unlike Direct3D 9, OpenGL or OpenGL ES 2, Direct3D 11 clears a given render target view and not the currently bound

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
					// Get the Direct3D 11 swap chain instance
					SwapChain *swapChain = static_cast<SwapChain*>(mRenderTarget);

					// Clear the Direct3D 11 render target view?
					if (flags & Renderer::ClearFlag::COLOR)
					{
						mD3D11DeviceContext->ClearRenderTargetView(swapChain->getD3D11RenderTargetView(), color);
					}

					// Clear the Direct3D 11 depth stencil view?
					if (nullptr != swapChain->getD3D11DepthStencilView())
					{
						// Get the Direct3D 11 clear flags
						UINT direct3D11ClearFlags = (flags & Renderer::ClearFlag::DEPTH) ? D3D11_CLEAR_DEPTH : 0u;
						if (flags & Renderer::ClearFlag::STENCIL)
						{
							direct3D11ClearFlags |= D3D11_CLEAR_STENCIL;
						}
						if (0 != direct3D11ClearFlags)
						{
							// Clear the Direct3D 11 depth stencil view
							mD3D11DeviceContext->ClearDepthStencilView(swapChain->getD3D11DepthStencilView(), direct3D11ClearFlags, z, static_cast<UINT8>(stencil));
						}
					}
					break;
				}

				case Renderer::ResourceType::FRAMEBUFFER:
				{
					// Get the Direct3D 11 framebuffer instance
					Framebuffer *framebuffer = static_cast<Framebuffer*>(mRenderTarget);

					// Clear all Direct3D 11 render target views?
					if (flags & Renderer::ClearFlag::COLOR)
					{
						// Loop through all Direct3D 11 render target views
						ID3D11RenderTargetView **d3d11RenderTargetViewsEnd = framebuffer->getD3D11RenderTargetViews() + framebuffer->getNumberOfColorTextures();
						for (ID3D11RenderTargetView **d3d11RenderTargetView = framebuffer->getD3D11RenderTargetViews(); d3d11RenderTargetView < d3d11RenderTargetViewsEnd; ++d3d11RenderTargetView)
						{
							// Valid Direct3D 11 render target view?
							if (nullptr != *d3d11RenderTargetView)
							{
								mD3D11DeviceContext->ClearRenderTargetView(*d3d11RenderTargetView, color);
							}
						}
					}

					// Clear the Direct3D 11 depth stencil view?
					if (nullptr != framebuffer->getD3D11DepthStencilView())
					{
						// Get the Direct3D 11 clear flags
						UINT direct3D11ClearFlags = (flags & Renderer::ClearFlag::DEPTH) ? D3D11_CLEAR_DEPTH : 0u;
						if (flags & Renderer::ClearFlag::STENCIL)
						{
							direct3D11ClearFlags |= D3D11_CLEAR_STENCIL;
						}
						if (0 != direct3D11ClearFlags)
						{
							// Clear the Direct3D 11 depth stencil view
							mD3D11DeviceContext->ClearDepthStencilView(framebuffer->getD3D11DepthStencilView(), direct3D11ClearFlags, z, static_cast<UINT8>(stencil));
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
			// In case no render target is currently set we don't have to do anything in here
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}

	void Direct3D11Renderer::resolveMultisampleFramebuffer(Renderer::IRenderTarget& destinationRenderTarget, Renderer::IFramebuffer& sourceMultisampleFramebuffer)
	{
		// Security check: Are the given resources owned by this renderer? (calls "return" in case of a mismatch)
		DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, destinationRenderTarget)
		DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, sourceMultisampleFramebuffer)

		// Evaluate the render target type
		switch (destinationRenderTarget.getResourceType())
		{
			case Renderer::ResourceType::SWAP_CHAIN:
			{
				// Get the Direct3D 11 swap chain instance
				// TODO(co) Implement me, not that important in practice so not directly implemented
				// SwapChain& swapChain = static_cast<SwapChain&>(destinationRenderTarget);
				break;
			}

			case Renderer::ResourceType::FRAMEBUFFER:
			{
				// Get the Direct3D 11 framebuffer instances
				const Framebuffer& direct3D11DestinationFramebuffer = static_cast<const Framebuffer&>(destinationRenderTarget);
				const Framebuffer& direct3D11SourceMultisampleFramebuffer = static_cast<const Framebuffer&>(sourceMultisampleFramebuffer);

				// Process all Direct3D 11 render target textures
				if (direct3D11DestinationFramebuffer.getNumberOfColorTextures() > 0 && direct3D11SourceMultisampleFramebuffer.getNumberOfColorTextures() > 0)
				{
					const uint32_t numberOfColorTextures = (direct3D11DestinationFramebuffer.getNumberOfColorTextures() < direct3D11SourceMultisampleFramebuffer.getNumberOfColorTextures()) ? direct3D11DestinationFramebuffer.getNumberOfColorTextures() : direct3D11SourceMultisampleFramebuffer.getNumberOfColorTextures();
					Renderer::ITexture** destinationTexture = direct3D11DestinationFramebuffer.getColorTextures();
					Renderer::ITexture** sourceTexture = direct3D11SourceMultisampleFramebuffer.getColorTextures();
					Renderer::ITexture** sourceTextureEnd = sourceTexture + numberOfColorTextures;
					for (; sourceTexture < sourceTextureEnd; ++sourceTexture, ++destinationTexture)
					{
						// Valid Direct3D 11 render target views?
						if (nullptr != *destinationTexture && nullptr != *sourceTexture)
						{
							const Texture2D* d3d11DestinationTexture2D = static_cast<const Texture2D*>(*destinationTexture);
							const Texture2D* d3d11SourceTexture2D = static_cast<const Texture2D*>(*sourceTexture);
							mD3D11DeviceContext->ResolveSubresource(d3d11DestinationTexture2D->getD3D11Texture2D(), D3D11CalcSubresource(0, 0, 1), d3d11SourceTexture2D->getD3D11Texture2D(), D3D11CalcSubresource(0, 0, 1), static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(d3d11DestinationTexture2D->getTextureFormat())));
						}
					}
				}

				// Process Direct3D 11 depth stencil texture
				if (nullptr != direct3D11DestinationFramebuffer.getDepthStencilTexture() && nullptr != direct3D11SourceMultisampleFramebuffer.getDepthStencilTexture())
				{
					const Texture2D* d3d11DestinationTexture2D = static_cast<const Texture2D*>(direct3D11DestinationFramebuffer.getDepthStencilTexture());
					const Texture2D* d3d11SourceTexture2D = static_cast<const Texture2D*>(direct3D11SourceMultisampleFramebuffer.getDepthStencilTexture());
					mD3D11DeviceContext->ResolveSubresource(d3d11DestinationTexture2D->getD3D11Texture2D(), D3D11CalcSubresource(0, 0, 1), d3d11SourceTexture2D->getD3D11Texture2D(), D3D11CalcSubresource(0, 0, 1), static_cast<DXGI_FORMAT>(Mapping::getDirect3D11Format(d3d11DestinationTexture2D->getTextureFormat())));
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
	}

	void Direct3D11Renderer::copyResource(Renderer::IResource& destinationResource, Renderer::IResource& sourceResource)
	{
		// Security check: Are the given resources owned by this renderer? (calls "return" in case of a mismatch)
		DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, destinationResource)
		DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, sourceResource)

		// Evaluate the render target type
		switch (destinationResource.getResourceType())
		{
			case Renderer::ResourceType::TEXTURE_2D:
				if (sourceResource.getResourceType() == Renderer::ResourceType::TEXTURE_2D)
				{
					// Get the Direct3D 11 texture 2D instances
					const Texture2D& direct3D11DestinationTexture2D = static_cast<const Texture2D&>(destinationResource);
					const Texture2D& direct3D11SourceTexture2D = static_cast<const Texture2D&>(sourceResource);

					// Copy resource
					mD3D11DeviceContext->CopyResource(direct3D11DestinationTexture2D.getD3D11Texture2D(), direct3D11SourceTexture2D.getD3D11Texture2D());
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
	void Direct3D11Renderer::drawEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
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
				mD3D11DeviceContext->DrawInstanced(
					drawInstancedArguments.vertexCountPerInstance,	// Vertex count per instance (UINT)
					drawInstancedArguments.instanceCount,			// Instance count (UINT)
					drawInstancedArguments.startVertexLocation,		// Start vertex location (UINT)
					drawInstancedArguments.startInstanceLocation	// Start instance location (UINT)
				);
			}
			else
			{
				// Without instancing
				mD3D11DeviceContext->Draw(
					drawInstancedArguments.vertexCountPerInstance,	// Vertex count (UINT)
					drawInstancedArguments.startVertexLocation		// Start index location (UINT)
				);
			}

			// Advance
			emulationData += sizeof(Renderer::DrawInstancedArguments);
		}
	}

	void Direct3D11Renderer::drawIndexedEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
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
				mD3D11DeviceContext->DrawIndexedInstanced(
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
				mD3D11DeviceContext->DrawIndexed(
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
	void Direct3D11Renderer::setDebugMarker(const char *name)
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			if (nullptr != mD3DUserDefinedAnnotation)
			{
				assert(strlen(name) < 256);
				wchar_t unicodeName[256];
				std::mbstowcs(unicodeName, name, 256);
				mD3DUserDefinedAnnotation->SetMarker(unicodeName);
			}
		#endif
	}

	void Direct3D11Renderer::beginDebugEvent(const char *name)
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			if (nullptr != mD3DUserDefinedAnnotation)
			{
				assert(strlen(name) < 256);
				wchar_t unicodeName[256];
				std::mbstowcs(unicodeName, name, 256);
				mD3DUserDefinedAnnotation->BeginEvent(unicodeName);
			}
		#endif
	}

	void Direct3D11Renderer::endDebugEvent()
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			if (nullptr != mD3DUserDefinedAnnotation)
			{
				mD3DUserDefinedAnnotation->EndEvent();
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	bool Direct3D11Renderer::isDebugEnabled()
	{
		// Don't check for the "DIRECT3D9RENDERER_NO_DEBUG" preprocessor definition, even if debug
		// is disabled it has to be possible to use this function for an additional security check
		// -> Maybe a debugger/profiler ignores the debug state
		// -> Maybe someone manipulated the binary to enable the debug state, adding a second check
		//    makes it a little bit more time consuming to hack the binary :D (but of course, this is no 100% security)
		return (nullptr != mD3DUserDefinedAnnotation && mD3DUserDefinedAnnotation->GetStatus() != 0);
	}

	Renderer::ISwapChain *Direct3D11Renderer::getMainSwapChain() const
	{
		return static_cast<Renderer::ISwapChain*>(mMainSwapChain);
	}


	//[-------------------------------------------------------]
	//[ Shader language                                       ]
	//[-------------------------------------------------------]
	uint32_t Direct3D11Renderer::getNumberOfShaderLanguages() const
	{
		uint32_t numberOfShaderLanguages = 1;	// HLSL support is always there

		// Done, return the number of supported shader languages
		return numberOfShaderLanguages;
	}

	const char *Direct3D11Renderer::getShaderLanguageName(uint32_t index) const
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

	Renderer::IShaderLanguage *Direct3D11Renderer::getShaderLanguage(const char *shaderLanguageName)
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
	Renderer::ISwapChain *Direct3D11Renderer::createSwapChain(handle nativeWindowHandle, bool)
	{
		// The provided native window handle must not be a null handle
		return (NULL_HANDLE != nativeWindowHandle) ? new SwapChain(*this, nativeWindowHandle) : nullptr;
	}

	Renderer::IFramebuffer *Direct3D11Renderer::createFramebuffer(uint32_t numberOfColorFramebufferAttachments, const Renderer::FramebufferAttachment *colorFramebufferAttachments, const Renderer::FramebufferAttachment *depthStencilFramebufferAttachment)
	{
		// Validation is done inside the framebuffer implementation
		return new Framebuffer(*this, numberOfColorFramebufferAttachments, colorFramebufferAttachments, depthStencilFramebufferAttachment);
	}

	Renderer::IBufferManager *Direct3D11Renderer::createBufferManager()
	{
		return new BufferManager(*this);
	}

	Renderer::ITextureManager *Direct3D11Renderer::createTextureManager()
	{
		return new TextureManager(*this);
	}

	Renderer::IRootSignature *Direct3D11Renderer::createRootSignature(const Renderer::RootSignature &rootSignature)
	{
		return new RootSignature(*this, rootSignature);
	}

	Renderer::IPipelineState *Direct3D11Renderer::createPipelineState(const Renderer::PipelineState &pipelineState)
	{
		return new PipelineState(*this, pipelineState);
	}

	Renderer::ISamplerState *Direct3D11Renderer::createSamplerState(const Renderer::SamplerState &samplerState)
	{
		return new SamplerState(*this, samplerState);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool Direct3D11Renderer::map(Renderer::IResource &resource, uint32_t subresource, Renderer::MapType mapType, uint32_t mapFlags, Renderer::MappedSubresource &mappedSubresource)
	{
		// The "Renderer::MapType" values directly map to Direct3D 10 & 11 constants, do not change them
		// The "Renderer::MappedSubresource" structure directly maps to Direct3D 11, do not change it

		// Define helper macro
		#define TEXTURE_RESOURCE(type, typeClass) \
			case type: \
			{ \
				bool result = false; \
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this) \
				ID3D11Resource *d3d11Resource = nullptr; \
				static_cast<typeClass&>(resource).getD3D11ShaderResourceView()->GetResource(&d3d11Resource); \
				if (nullptr != d3d11Resource) \
				{ \
					result = (S_OK == mD3D11DeviceContext->Map(d3d11Resource, subresource, static_cast<D3D11_MAP>(mapType), mapFlags, reinterpret_cast<D3D11_MAPPED_SUBRESOURCE*>(&mappedSubresource))); \
					d3d11Resource->Release(); \
				} \
				RENDERER_END_DEBUG_EVENT(this) \
				return result; \
			}

		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
				return (S_OK == mD3D11DeviceContext->Map(static_cast<IndexBuffer&>(resource).getD3D11Buffer(), subresource, static_cast<D3D11_MAP>(mapType), mapFlags, reinterpret_cast<D3D11_MAPPED_SUBRESOURCE*>(&mappedSubresource)));

			case Renderer::ResourceType::VERTEX_BUFFER:
				return (S_OK == mD3D11DeviceContext->Map(static_cast<VertexBuffer&>(resource).getD3D11Buffer(), subresource, static_cast<D3D11_MAP>(mapType), mapFlags, reinterpret_cast<D3D11_MAPPED_SUBRESOURCE*>(&mappedSubresource)));

			case Renderer::ResourceType::UNIFORM_BUFFER:
				return (S_OK == mD3D11DeviceContext->Map(static_cast<UniformBuffer&>(resource).getD3D11Buffer(), subresource, static_cast<D3D11_MAP>(mapType), mapFlags, reinterpret_cast<D3D11_MAPPED_SUBRESOURCE*>(&mappedSubresource)));

			case Renderer::ResourceType::TEXTURE_BUFFER:
				return (S_OK == mD3D11DeviceContext->Map(static_cast<TextureBuffer&>(resource).getD3D11Buffer(), subresource, static_cast<D3D11_MAP>(mapType), mapFlags, reinterpret_cast<D3D11_MAPPED_SUBRESOURCE*>(&mappedSubresource)));

			case Renderer::ResourceType::INDIRECT_BUFFER:
				// TODO(co) Implement indirect buffer support, see e.g. "Voxel visualization using DrawIndexedInstancedIndirect" - http://www.alexandre-pestana.com/tag/directx/ for hints
				// return (S_OK == mD3D11DeviceContext->Map(static_cast<IndirectBuffer&>(resource).getD3D11Buffer(), subresource, static_cast<D3D11_MAP>(mapType), mapFlags, reinterpret_cast<D3D11_MAPPED_SUBRESOURCE*>(&mappedSubresource)));
				mappedSubresource.data		 = static_cast<IndirectBuffer&>(resource).getWritableEmulationData();
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;
				return true;

			TEXTURE_RESOURCE(Renderer::ResourceType::TEXTURE_1D, Texture1D)
			TEXTURE_RESOURCE(Renderer::ResourceType::TEXTURE_2D, Texture2D)
			TEXTURE_RESOURCE(Renderer::ResourceType::TEXTURE_2D_ARRAY, Texture2DArray)
			TEXTURE_RESOURCE(Renderer::ResourceType::TEXTURE_3D, Texture3D)
			TEXTURE_RESOURCE(Renderer::ResourceType::TEXTURE_CUBE, TextureCube)

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

		// Undefine helper macro
		#undef TEXTURE_RESOURCE
	}

	void Direct3D11Renderer::unmap(Renderer::IResource &resource, uint32_t subresource)
	{
		// Define helper macro
		#define TEXTURE_RESOURCE(type, typeClass) \
			case type: \
			{ \
				ID3D11Resource *d3d11Resource = nullptr; \
				static_cast<typeClass&>(resource).getD3D11ShaderResourceView()->GetResource(&d3d11Resource); \
				if (nullptr != d3d11Resource) \
				{ \
					mD3D11DeviceContext->Unmap(d3d11Resource, subresource); \
					d3d11Resource->Release(); \
				} \
				break; \
			}

		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
				mD3D11DeviceContext->Unmap(static_cast<IndexBuffer&>(resource).getD3D11Buffer(), subresource);
				break;

			case Renderer::ResourceType::VERTEX_BUFFER:
				mD3D11DeviceContext->Unmap(static_cast<VertexBuffer&>(resource).getD3D11Buffer(), subresource);
				break;

			case Renderer::ResourceType::UNIFORM_BUFFER:
				mD3D11DeviceContext->Unmap(static_cast<UniformBuffer&>(resource).getD3D11Buffer(), subresource);
				break;

			case Renderer::ResourceType::TEXTURE_BUFFER:
				mD3D11DeviceContext->Unmap(static_cast<TextureBuffer&>(resource).getD3D11Buffer(), subresource);
				break;

			case Renderer::ResourceType::INDIRECT_BUFFER:
				// TODO(co) Implement indirect buffer support, see e.g. "Voxel visualization using DrawIndexedInstancedIndirect" - http://www.alexandre-pestana.com/tag/directx/ for hints
				// mD3D11DeviceContext->Unmap(static_cast<IndirectBuffer&>(resource).getD3D11Buffer(), subresource);
				break;

			TEXTURE_RESOURCE(Renderer::ResourceType::TEXTURE_1D, Texture1D)
			TEXTURE_RESOURCE(Renderer::ResourceType::TEXTURE_2D, Texture2D)
			TEXTURE_RESOURCE(Renderer::ResourceType::TEXTURE_2D_ARRAY, Texture2DArray)
			TEXTURE_RESOURCE(Renderer::ResourceType::TEXTURE_3D, Texture3D)
			TEXTURE_RESOURCE(Renderer::ResourceType::TEXTURE_CUBE, TextureCube)

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

		// Undefine helper macro
		#undef TEXTURE_RESOURCE
	}


	//[-------------------------------------------------------]
	//[ Operations                                            ]
	//[-------------------------------------------------------]
	bool Direct3D11Renderer::beginScene()
	{
		// Not required when using Direct3D 11

		// Done
		return true;
	}

	void Direct3D11Renderer::submitCommandBuffer(const Renderer::CommandBuffer& commandBuffer)
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

	void Direct3D11Renderer::endScene()
	{
		// We need to forget about the currently set render target
		omSetRenderTarget(nullptr);
	}


	//[-------------------------------------------------------]
	//[ Synchronization                                       ]
	//[-------------------------------------------------------]
	void Direct3D11Renderer::flush()
	{
		mD3D11DeviceContext->Flush();
	}

	void Direct3D11Renderer::finish()
	{
		// Create the Direct3D 11 query instance used for flush right now?
		if (nullptr == mD3D11QueryFlush)
		{
			D3D11_QUERY_DESC d3d11QueryDesc;
			d3d11QueryDesc.Query	 = D3D11_QUERY_EVENT;
			d3d11QueryDesc.MiscFlags = 0;
			mD3D11Device->CreateQuery(&d3d11QueryDesc, &mD3D11QueryFlush);

			#ifndef DIRECT3D11RENDERER_NO_DEBUG
				// Set the debug name
				if (nullptr != mD3D11QueryFlush)
				{
					// No need to reset the previous private data, there shouldn't be any...
					mD3D11QueryFlush->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(__FUNCTION__)), __FUNCTION__);
				}
			#endif
		}
		if (nullptr != mD3D11QueryFlush)
		{
			// Perform the flush and wait
			mD3D11DeviceContext->End(mD3D11QueryFlush);
			mD3D11DeviceContext->Flush();
			BOOL result = FALSE;
			do
			{
				// Spin-wait
				mD3D11DeviceContext->GetData(mD3D11QueryFlush, &result, sizeof(BOOL), 0);
			} while (!result);
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void Direct3D11Renderer::initializeCapabilities()
	{
		// There are no Direct3D 11 device capabilities we could query on runtime, they depend on the chosen feature level
		// -> Have a look at "Devices -> Direct3D 11 on Downlevel Hardware -> Introduction" at MSDN http://msdn.microsoft.com/en-us/library/ff476876%28v=vs.85%29.aspx
		//    for a table with a list of the minimum resources supported by Direct3D 11 at the different feature levels

		// Evaluate the chosen feature level
		switch (mD3D11Device->GetFeatureLevel())
		{
			case D3D_FEATURE_LEVEL_9_1:
				// Maximum number of viewports (always at least 1)
				mCapabilities.maximumNumberOfViewports = 1;	// Direct3D 9 only supports a single viewport

				// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
				mCapabilities.maximumNumberOfSimultaneousRenderTargets = 1;

				// Maximum texture dimension
				mCapabilities.maximumTextureDimension = 2048;

				// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
				mCapabilities.maximumNumberOf2DTextureArraySlices = 0;

				// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
				mCapabilities.maximumTextureBufferSize = 0;

				// Maximum indirect buffer size in bytes (in case there's no support for indirect buffer it's 0)
				// TODO(co) Implement indirect buffer support
				mCapabilities.maximumIndirectBufferSize = sizeof(Renderer::DrawIndexedInstancedArguments) * 4096;	// TODO(co) What is an usually decent emulated indirect buffer size?

				// Maximum number of multisamples (always at least 1, usually 8)
				mCapabilities.maximumNumberOfMultisamples = 1;	// Don't want to support the legacy DirectX 9 multisample support

				// Maximum anisotropy (always at least 1, usually 16)
				mCapabilities.maximumAnisotropy = 16;

				// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
				mCapabilities.instancedArrays = false;

				// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID)
				mCapabilities.drawInstanced = false;

				// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
				mCapabilities.maximumNumberOfPatchVertices = 0;	// Direct3D 9.1 has no tessellation support

				// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
				mCapabilities.maximumNumberOfGsOutputVertices = 0;	// Direct3D 9.1 has no geometry shader support
				break;

			case D3D_FEATURE_LEVEL_9_2:
				// Maximum number of viewports (always at least 1)
				mCapabilities.maximumNumberOfViewports = 1;	// Direct3D 9 only supports a single viewport

				// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
				mCapabilities.maximumNumberOfSimultaneousRenderTargets = 1;

				// Maximum texture dimension
				mCapabilities.maximumTextureDimension = 2048;

				// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
				mCapabilities.maximumNumberOf2DTextureArraySlices = 0;

				// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
				mCapabilities.maximumTextureBufferSize = 0;

				// Maximum indirect buffer size in bytes (in case there's no support for indirect buffer it's 0)
				// TODO(co) Implement indirect buffer support
				mCapabilities.maximumIndirectBufferSize = sizeof(Renderer::DrawIndexedInstancedArguments) * 4096;	// TODO(co) What is an usually decent emulated indirect buffer size?

				// Maximum number of multisamples (always at least 1, usually 8)
				mCapabilities.maximumNumberOfMultisamples = 1;	// Don't want to support the legacy DirectX 9 multisample support

				// Maximum anisotropy (always at least 1, usually 16)
				mCapabilities.maximumAnisotropy = 16;

				// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
				mCapabilities.instancedArrays = false;

				// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID)
				mCapabilities.drawInstanced = false;

				// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
				mCapabilities.maximumNumberOfPatchVertices = 0;	// Direct3D 9.2 has no tessellation support

				// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
				mCapabilities.maximumNumberOfGsOutputVertices = 0;	// Direct3D 9.2 has no geometry shader support
				break;

			case D3D_FEATURE_LEVEL_9_3:
				// Maximum number of viewports (always at least 1)
				mCapabilities.maximumNumberOfViewports = 1;	// Direct3D 9 only supports a single viewport

				// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
				mCapabilities.maximumNumberOfSimultaneousRenderTargets = 4;

				// Maximum texture dimension
				mCapabilities.maximumTextureDimension = 4096;

				// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
				mCapabilities.maximumNumberOf2DTextureArraySlices = 0;

				// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
				mCapabilities.maximumTextureBufferSize = 0;

				// Maximum indirect buffer size in bytes (in case there's no support for indirect buffer it's 0)
				// TODO(co) Implement indirect buffer support
				mCapabilities.maximumIndirectBufferSize = sizeof(Renderer::DrawIndexedInstancedArguments) * 4096;	// TODO(co) What is an usually decent emulated indirect buffer size?

				// Maximum number of multisamples (always at least 1, usually 8)
				mCapabilities.maximumNumberOfMultisamples = 1;	// Don't want to support the legacy DirectX 9 multisample support

				// Maximum anisotropy (always at least 1, usually 16)
				mCapabilities.maximumAnisotropy = 16;

				// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
				mCapabilities.instancedArrays = true;

				// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID)
				mCapabilities.drawInstanced = false;

				// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
				mCapabilities.maximumNumberOfPatchVertices = 0;	// Direct3D 9.3 has no tessellation support

				// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
				mCapabilities.maximumNumberOfGsOutputVertices = 0;	// Direct3D 9.3 has no geometry shader support
				break;

			case D3D_FEATURE_LEVEL_10_0:
				// Maximum number of viewports (always at least 1)
				mCapabilities.maximumNumberOfViewports = D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX + 1;

				// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
				mCapabilities.maximumNumberOfSimultaneousRenderTargets = D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;

				// Maximum texture dimension
				mCapabilities.maximumTextureDimension = 8192;

				// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
				mCapabilities.maximumNumberOf2DTextureArraySlices = 512;

				// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
				mCapabilities.maximumTextureBufferSize = 128 * 1024 * 1024;	// TODO(co) http://msdn.microsoft.com/en-us/library/ff476876%28v=vs.85%29.aspx does not mention the texture buffer? Currently the OpenGL 3 minimum is used: 128 MiB.

				// Maximum indirect buffer size in bytes (in case there's no support for indirect buffer it's 0)
				// TODO(co) Implement indirect buffer support
				mCapabilities.maximumIndirectBufferSize = sizeof(Renderer::DrawIndexedInstancedArguments) * 4096;	// TODO(co) What is an usually decent emulated indirect buffer size?

				// Maximum number of multisamples (always at least 1, usually 8)
				mCapabilities.maximumNumberOfMultisamples = 8;

				// Maximum anisotropy (always at least 1, usually 16)
				mCapabilities.maximumAnisotropy = 16;

				// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
				mCapabilities.instancedArrays = true;

				// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID)
				mCapabilities.drawInstanced = true;

				// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
				mCapabilities.maximumNumberOfPatchVertices = 0;	// Direct3D 10 has no tessellation support

				// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
				mCapabilities.maximumNumberOfGsOutputVertices = 1024;
				break;

			case D3D_FEATURE_LEVEL_10_1:
				// Maximum number of viewports (always at least 1)
				mCapabilities.maximumNumberOfViewports = D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX + 1;

				// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
				mCapabilities.maximumNumberOfSimultaneousRenderTargets = D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;

				// Maximum texture dimension
				mCapabilities.maximumTextureDimension = 8192;

				// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
				mCapabilities.maximumNumberOf2DTextureArraySlices = 512;

				// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
				mCapabilities.maximumTextureBufferSize = 128 * 1024 * 1024;	// TODO(co) http://msdn.microsoft.com/en-us/library/ff476876%28v=vs.85%29.aspx does not mention the texture buffer? Currently the OpenGL 3 minimum is used: 128 MiB.

				// Maximum indirect buffer size in bytes (in case there's no support for indirect buffer it's 0)
				// TODO(co) Implement indirect buffer support
				mCapabilities.maximumIndirectBufferSize = sizeof(Renderer::DrawIndexedInstancedArguments) * 4096;	// TODO(co) What is an usually decent emulated indirect buffer size?

				// Maximum number of multisamples (always at least 1, usually 8)
				mCapabilities.maximumNumberOfMultisamples = 8;

				// Maximum anisotropy (always at least 1, usually 16)
				mCapabilities.maximumAnisotropy = 16;

				// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
				mCapabilities.instancedArrays = true;

				// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID)
				mCapabilities.drawInstanced = true;

				// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
				mCapabilities.maximumNumberOfPatchVertices = 0;	// Direct3D 10.1 has no tessellation support

				// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
				mCapabilities.maximumNumberOfGsOutputVertices = 1024;
				break;

			case D3D_FEATURE_LEVEL_11_0:
				// Maximum number of viewports (always at least 1)
				mCapabilities.maximumNumberOfViewports = D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX + 1;

				// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
				mCapabilities.maximumNumberOfSimultaneousRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;

				// Maximum texture dimension
				mCapabilities.maximumTextureDimension = 16384;

				// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
				mCapabilities.maximumNumberOf2DTextureArraySlices = 512;

				// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
				mCapabilities.maximumTextureBufferSize = 128 * 1024 * 1024;	// TODO(co) http://msdn.microsoft.com/en-us/library/ff476876%28v=vs.85%29.aspx does not mention the texture buffer? Currently the OpenGL 3 minimum is used: 128 MiB.

				// Maximum indirect buffer size in bytes (in case there's no support for indirect buffer it's 0)
				// TODO(co) Implement indirect buffer support
				mCapabilities.maximumIndirectBufferSize = sizeof(Renderer::DrawIndexedInstancedArguments) * 4096;	// TODO(co) What is an usually decent emulated indirect buffer size?

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
				break;
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

		// Direct3D 11 has native multi-threading
		// -> When using user defined annotation for enhanced graphics debugging, disable native multi-threading or we'll get synchronization problems like
		//    "
		//    D3D11 CORRUPTION: ID3D11DeviceContext::Map: Two threads were found to be executing functions associated with the same Device[Context] at the same time. This will cause corruption of memory. Appropriate thread synchronization needs to occur external to the Direct3D API (or through the ID3D10Multithread interface). 7584 and 12900 are the implicated thread ids. [ MISCELLANEOUS CORRUPTION #28: CORRUPTED_MULTITHREADING]
		//    Exception thrown at 0x7645B782 (KernelBase.dll) in ExamplesD.exe: 0x0000087D (parameters: 0x00000000, 0x00C84D70, 0x00C841A8).
		//    "
		//    in case a thread is currently between "beginDebugEvent()"/"endDebugEvent()" while another thread is creating for example a texture resource.
		mCapabilities.nativeMultiThreading = (nullptr == mD3DUserDefinedAnnotation);

		// Direct3D 11 has shader bytecode support
		mCapabilities.shaderBytecode = true;

		// Is there support for vertex shaders (VS)?
		mCapabilities.vertexShader = true;

		// Is there support for fragment shaders (FS)?
		mCapabilities.fragmentShader = true;
	}

	void Direct3D11Renderer::setProgram(Renderer::IProgram *program)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		if (nullptr != program)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *program)

			// Get shaders
			const ProgramHlsl					   *programHlsl						 = static_cast<ProgramHlsl*>(program);
			const VertexShaderHlsl				   *vertexShaderHlsl				 = programHlsl->getVertexShaderHlsl();
			const TessellationControlShaderHlsl	   *tessellationControlShaderHlsl	 = programHlsl->getTessellationControlShaderHlsl();
			const TessellationEvaluationShaderHlsl *tessellationEvaluationShaderHlsl = programHlsl->getTessellationEvaluationShaderHlsl();
			const GeometryShaderHlsl			   *geometryShaderHlsl				 = programHlsl->getGeometryShaderHlsl();
			const FragmentShaderHlsl			   *fragmentShaderHlsl				 = programHlsl->getFragmentShaderHlsl();
			ID3D11VertexShader   *d3d11VertexShader   = (nullptr != vertexShaderHlsl)				  ? vertexShaderHlsl->getD3D11VertexShader()				 : nullptr;
			ID3D11HullShader     *d3d11HullShader     = (nullptr != tessellationControlShaderHlsl)	  ? tessellationControlShaderHlsl->getD3D11HullShader()		 : nullptr;
			ID3D11DomainShader   *d3d11DomainShader   = (nullptr != tessellationEvaluationShaderHlsl) ? tessellationEvaluationShaderHlsl->getD3D11DomainShader() : nullptr;
			ID3D11GeometryShader *d3d11GeometryShader = (nullptr != geometryShaderHlsl)				  ? geometryShaderHlsl->getD3D11GeometryShader()			 : nullptr;
			ID3D11PixelShader	 *d3d11PixelShader    = (nullptr != fragmentShaderHlsl)				  ? fragmentShaderHlsl->getD3D11PixelShader()				 : nullptr;

			// Set shaders
			if (mD3d11VertexShader != d3d11VertexShader)
			{
				mD3d11VertexShader = d3d11VertexShader;
				mD3D11DeviceContext->VSSetShader(mD3d11VertexShader, nullptr, 0);
			}
			if (mD3d11HullShader != d3d11HullShader)
			{
				mD3d11HullShader = d3d11HullShader;
				mD3D11DeviceContext->HSSetShader(mD3d11HullShader, nullptr, 0);
			}
			if (mD3d11DomainShader != d3d11DomainShader)
			{
				mD3d11DomainShader = d3d11DomainShader;
				mD3D11DeviceContext->DSSetShader(mD3d11DomainShader, nullptr, 0);
			}
			if (mD3d11GeometryShader != d3d11GeometryShader)
			{
				mD3d11GeometryShader = d3d11GeometryShader;
				mD3D11DeviceContext->GSSetShader(mD3d11GeometryShader, nullptr, 0);
			}
			if (mD3d11PixelShader != d3d11PixelShader)
			{
				mD3d11PixelShader = d3d11PixelShader;
				mD3D11DeviceContext->PSSetShader(mD3d11PixelShader, nullptr, 0);
			}
		}
		else
		{
			if (nullptr != mD3d11VertexShader)
			{
				mD3D11DeviceContext->VSSetShader(nullptr, nullptr, 0);
				mD3d11VertexShader = nullptr;
			}
			if (nullptr != mD3d11HullShader)
			{
				mD3D11DeviceContext->HSSetShader(nullptr, nullptr, 0);
				mD3d11HullShader = nullptr;
			}
			if (nullptr != mD3d11DomainShader)
			{
				mD3D11DeviceContext->DSSetShader(nullptr, nullptr, 0);
				mD3d11DomainShader = nullptr;
			}
			if (nullptr != mD3d11GeometryShader)
			{
				mD3D11DeviceContext->GSSetShader(nullptr, nullptr, 0);
				mD3d11GeometryShader = nullptr;
			}
			if (nullptr != mD3d11PixelShader)
			{
				mD3D11DeviceContext->PSSetShader(nullptr, nullptr, 0);
				mD3d11PixelShader = nullptr;
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
