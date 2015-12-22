/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "Direct3D12Renderer/Direct3D12Renderer.h"
#include "Direct3D12Renderer/D3D12X.h"
#include "Direct3D12Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D12Renderer/Direct3D12Debug.h"	// For "DIRECT3D12RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "Direct3D12Renderer/Direct3D12RuntimeLinking.h"
#include "Direct3D12Renderer/Texture2D.h"
#include "Direct3D12Renderer/VertexArray.h"
#include "Direct3D12Renderer/SwapChain.h"
#include "Direct3D12Renderer/Framebuffer.h"
#include "Direct3D12Renderer/IndexBuffer.h"
#include "Direct3D12Renderer/SamplerState.h"
#include "Direct3D12Renderer/VertexBuffer.h"
#include "Direct3D12Renderer/TextureBuffer.h"
#include "Direct3D12Renderer/RootSignature.h"
#include "Direct3D12Renderer/PipelineState.h"
#include "Direct3D12Renderer/Texture2DArray.h"
#include "Direct3D12Renderer/Shader/ProgramHlsl.h"
#include "Direct3D12Renderer/Shader/UniformBuffer.h"
#include "Direct3D12Renderer/Shader/VertexShaderHlsl.h"
#include "Direct3D12Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D12Renderer/Shader/GeometryShaderHlsl.h"
#include "Direct3D12Renderer/Shader/FragmentShaderHlsl.h"
#include "Direct3D12Renderer/Shader/TessellationControlShaderHlsl.h"
#include "Direct3D12Renderer/Shader/TessellationEvaluationShaderHlsl.h"


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef DIRECT3D12RENDERER_EXPORTS
	#define DIRECT3D12RENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define DIRECT3D12RENDERER_API_EXPORT
#endif
DIRECT3D12RENDERER_API_EXPORT Renderer::IRenderer *createDirect3D12RendererInstance(handle nativeWindowHandle)
{
	return new Direct3D12Renderer::Direct3D12Renderer(nativeWindowHandle);
}
#undef DIRECT3D12RENDERER_API_EXPORT


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Direct3D12Renderer::Direct3D12Renderer(handle nativeWindowHandle) :
		mDirect3D12RuntimeLinking(new Direct3D12RuntimeLinking()),
		mDxgiFactory4(nullptr),
		mD3D12Device(nullptr),
		mD3D12CommandQueue(nullptr),
		mD3D12CommandAllocator(nullptr),
		mD3D12GraphicsCommandList(nullptr),
		mShaderLanguageHlsl(nullptr),
		mD3D12QueryFlush(nullptr),
		mMainSwapChain(nullptr),
		mRenderTarget(nullptr)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		// Is Direct3D 12 available?
		if (mDirect3D12RuntimeLinking->isDirect3D12Avaiable())
		{
			// Create the DXGI factory instance
			if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&mDxgiFactory4))))
			{
				// Enable the Direct3D 12 debug layer
				#ifndef DIRECT3D12RENDERER_NO_DEBUG
				{
					ID3D12Debug* d3d12Debug = nullptr;
					if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&d3d12Debug))))
					{
						d3d12Debug->EnableDebugLayer();
						d3d12Debug->Release();
					}
				}
				#endif

				// Create the Direct3D 12 device
				// -> In case of failure, create an emulated device instance so we can at least test the DirectX 12 API
				if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mD3D12Device))))
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create DirectX 12 device instance. Creating an emulated Direct3D 11 device instance instead.")

					// Create the DXGI adapter instance
					IDXGIAdapter* dxgiAdapter = nullptr;
					if (SUCCEEDED(mDxgiFactory4->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter))))
					{
						// Create the emulated Direct3D 12 device
						if (FAILED(D3D12CreateDevice(dxgiAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mD3D12Device))))
						{
							RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create the device instance")
						}

						// Release the DXGI adapter instance
						dxgiAdapter->Release();
					}
					else
					{
						RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create DXGI adapter instance")
					}
				}
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create DXGI factory instance")
			}

			// Is there a valid Direct3D 12 device instance?
			if (nullptr != mD3D12Device)
			{
				// Describe and create the command queue
				D3D12_COMMAND_QUEUE_DESC d3d12CommandQueueDesc;
				d3d12CommandQueueDesc.Type		= D3D12_COMMAND_LIST_TYPE_DIRECT;
				d3d12CommandQueueDesc.Priority	= D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
				d3d12CommandQueueDesc.Flags		= D3D12_COMMAND_QUEUE_FLAG_NONE;
				d3d12CommandQueueDesc.NodeMask	= 0;
				if (SUCCEEDED(mD3D12Device->CreateCommandQueue(&d3d12CommandQueueDesc, IID_PPV_ARGS(&mD3D12CommandQueue))))
				{
					// Create the command allocator
					if (SUCCEEDED(mD3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mD3D12CommandAllocator))))
					{
						// Create the command list
						if (SUCCEEDED(mD3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mD3D12CommandAllocator, nullptr, IID_PPV_ARGS(&mD3D12GraphicsCommandList))))
						{
							// Command lists are created in the recording state, but there is nothing to record yet. The main loop expects it to be closed, so close it now.
							if (SUCCEEDED(mD3D12GraphicsCommandList->Close()))
							{
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
								RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to close the command list instance")
							}
						}
						else
						{
							RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create the command list instance")
						}
					}
					else
					{
						RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create the command allocator instance")
					}
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Failed to create the command queue instance")
				}
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}

	Direct3D12Renderer::~Direct3D12Renderer()
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

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

		{ // For debugging: At this point there should be no resource instances left, validate this!
			// -> Are the currently any resource instances?
			const unsigned long numberOfCurrentResources = getStatistics().getNumberOfCurrentResources();
			if (numberOfCurrentResources > 0)
			{
				// Error!
				if (numberOfCurrentResources > 1)
				{
					RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 12 error: Renderer is going to be destroyed, but there are still %d resource instances left (memory leak)\n", numberOfCurrentResources)
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Renderer is going to be destroyed, but there is still one resource instance left (memory leak)\n")
				}

				// Use debug output to show the current number of resource instances
				getStatistics().debugOutputCurrentResouces();
			}
		}

		// Release the Direct3D 12 query instance used for flush, in case we have one
		// TODO(co) Direct3D 12 update
		/*
		if (nullptr != mD3D12QueryFlush)
		{
			mD3D12QueryFlush->Release();
		}
		*/

		// Release the HLSL shader language instance, in case we have one
		if (nullptr != mShaderLanguageHlsl)
		{
			mShaderLanguageHlsl->release();
		}

		// Release the Direct3D 12 command queue we've created
		if (nullptr != mD3D12GraphicsCommandList)
		{
			mD3D12GraphicsCommandList->Release();
			mD3D12GraphicsCommandList = nullptr;
		}
		if (nullptr != mD3D12CommandAllocator)
		{
			mD3D12CommandAllocator->Release();
			mD3D12CommandAllocator = nullptr;
		}
		if (nullptr != mD3D12CommandQueue)
		{
			mD3D12CommandQueue->Release();
			mD3D12CommandQueue = nullptr;
		}

		// Release the Direct3D 12 device we've created
		if (nullptr != mD3D12Device)
		{
			mD3D12Device->Release();
			mD3D12Device = nullptr;
		}

		// Release the DXGI factory instance
		if (nullptr != mDxgiFactory4)
		{
			mDxgiFactory4->Release();
			mDxgiFactory4 = nullptr;
		}

		// Destroy the Direct3D 12 runtime linking instance
		delete mDirect3D12RuntimeLinking;

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	Renderer::ISwapChain *Direct3D12Renderer::getMainSwapChain() const
	{
		return static_cast<Renderer::ISwapChain*>(mMainSwapChain);
	}


	//[-------------------------------------------------------]
	//[ Shader language                                       ]
	//[-------------------------------------------------------]
	uint32_t Direct3D12Renderer::getNumberOfShaderLanguages() const
	{
		uint32_t numberOfShaderLanguages = 1;	// HLSL support is always there

		// Done, return the number of supported shader languages
		return numberOfShaderLanguages;
	}

	const char *Direct3D12Renderer::getShaderLanguageName(uint32_t index) const
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

	Renderer::IShaderLanguage *Direct3D12Renderer::getShaderLanguage(const char *shaderLanguageName)
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
	Renderer::ISwapChain *Direct3D12Renderer::createSwapChain(handle nativeWindowHandle)
	{
		// The provided native window handle must not be a null handle
		return (NULL_HANDLE != nativeWindowHandle) ? new SwapChain(*this, nativeWindowHandle) : nullptr;
	}

	Renderer::IFramebuffer *Direct3D12Renderer::createFramebuffer(uint32_t numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture)
	{
		// Validation is done inside the framebuffer implementation
		return new Framebuffer(*this, numberOfColorTextures, colorTextures, depthStencilTexture);
	}

	Renderer::IVertexBuffer *Direct3D12Renderer::createVertexBuffer(uint32_t numberOfBytes, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new VertexBuffer(*this, numberOfBytes, data, bufferUsage);
	}

	Renderer::IIndexBuffer *Direct3D12Renderer::createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new IndexBuffer(*this, numberOfBytes, indexBufferFormat, data, bufferUsage);
	}

	Renderer::IVertexArray *Direct3D12Renderer::createVertexArray(const Renderer::VertexAttributes&, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, Renderer::IIndexBuffer *indexBuffer)
	{
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		return new VertexArray(*this, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
	}

	Renderer::ITextureBuffer *Direct3D12Renderer::createTextureBuffer(uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new TextureBuffer(*this, numberOfBytes, textureFormat, data, bufferUsage);
	}

	Renderer::ITexture2D *Direct3D12Renderer::createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags, Renderer::TextureUsage::Enum textureUsage, const Renderer::OptimizedTextureClearValue* optimizedTextureClearValue)
	{
		return new Texture2D(*this, width, height, textureFormat, data, flags, textureUsage, optimizedTextureClearValue);
	}

	Renderer::ITexture2DArray *Direct3D12Renderer::createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags, Renderer::TextureUsage::Enum textureUsage)
	{
		return new Texture2DArray(*this, width, height, numberOfSlices, textureFormat, data, flags, textureUsage);
	}

	Renderer::IRootSignature *Direct3D12Renderer::createRootSignature(const Renderer::RootSignature& rootSignature)
	{
		return new RootSignature(*this, rootSignature);
	}

	Renderer::IPipelineState *Direct3D12Renderer::createPipelineState(const Renderer::PipelineState &pipelineState)
	{
		return new PipelineState(*this, pipelineState);
	}

	Renderer::ISamplerState *Direct3D12Renderer::createSamplerState(const Renderer::SamplerState &samplerState)
	{
		return new SamplerState(*this, samplerState);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool Direct3D12Renderer::map(Renderer::IResource &, uint32_t, Renderer::MapType::Enum, uint32_t, Renderer::MappedSubresource &)
	{
		// TODO(co) Direct3D 12 update
		/*
		// The "Renderer::MapType::Enum" values directly map to Direct3D 10 & 11 & 12 constants, do not change them
		// The "Renderer::MappedSubresource" structure directly maps to Direct3D 12, do not change it

		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
				return (S_OK == mD3D12DeviceContext->Map(static_cast<IndexBuffer&>(resource).getD3D12Buffer(), subresource, static_cast<D3D12_MAP>(mapType), mapFlags, reinterpret_cast<D3D12_MAPPED_SUBRESOURCE*>(&mappedSubresource)));

			case Renderer::ResourceType::VERTEX_BUFFER:
				return (S_OK == mD3D12DeviceContext->Map(static_cast<VertexBuffer&>(resource).getD3D12Buffer(), subresource, static_cast<D3D12_MAP>(mapType), mapFlags, reinterpret_cast<D3D12_MAPPED_SUBRESOURCE*>(&mappedSubresource)));

			case Renderer::ResourceType::UNIFORM_BUFFER:
				return (S_OK == mD3D12DeviceContext->Map(static_cast<UniformBuffer&>(resource).getD3D12Buffer(), subresource, static_cast<D3D12_MAP>(mapType), mapFlags, reinterpret_cast<D3D12_MAPPED_SUBRESOURCE*>(&mappedSubresource)));

			case Renderer::ResourceType::TEXTURE_BUFFER:
				return (S_OK == mD3D12DeviceContext->Map(static_cast<TextureBuffer&>(resource).getD3D12Buffer(), subresource, static_cast<D3D12_MAP>(mapType), mapFlags, reinterpret_cast<D3D12_MAPPED_SUBRESOURCE*>(&mappedSubresource)));

			case Renderer::ResourceType::TEXTURE_2D:
			{
				bool result = false;

				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

				// Get the Direct3D 12 resource instance
				ID3D12Resource *d3d12Resource = nullptr;
				static_cast<Texture2D&>(resource).getD3D12ShaderResourceView()->GetResource(&d3d12Resource);
				if (nullptr != d3d12Resource)
				{
					// Map the Direct3D 12 resource
					result = (S_OK == mD3D12DeviceContext->Map(d3d12Resource, subresource, static_cast<D3D12_MAP>(mapType), mapFlags, reinterpret_cast<D3D12_MAPPED_SUBRESOURCE*>(&mappedSubresource)));

					// Release the Direct3D 12 resource instance
					d3d12Resource->Release();
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

				// Get the Direct3D 12 resource instance
				ID3D12Resource *d3d12Resource = nullptr;
				static_cast<Texture2DArray&>(resource).getD3D12ShaderResourceView()->GetResource(&d3d12Resource);
				if (nullptr != d3d12Resource)
				{
					// Map the Direct3D 12 resource
					result = (S_OK == mD3D12DeviceContext->Map(d3d12Resource, subresource, static_cast<D3D12_MAP>(mapType), mapFlags, reinterpret_cast<D3D12_MAPPED_SUBRESOURCE*>(&mappedSubresource)));

					// Release the Direct3D 12 resource instance
					d3d12Resource->Release();
				}

				// End debug event
				RENDERER_END_DEBUG_EVENT(this)

				// Done
				return result;
			}

			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
			case Renderer::ResourceType::SWAP_CHAIN:
			case Renderer::ResourceType::FRAMEBUFFER:
			case Renderer::ResourceType::RASTERIZER_STATE:
			case Renderer::ResourceType::DEPTH_STENCIL_STATE:
			case Renderer::ResourceType::BLEND_STATE:
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
		*/
		return false;
	}

	void Direct3D12Renderer::unmap(Renderer::IResource &, uint32_t)
	{
		// TODO(co) Direct3D 12 update
		/*
		// Evaluate the resource type
		switch (resource.getResourceType())
		{
			case Renderer::ResourceType::INDEX_BUFFER:
				mD3D12DeviceContext->Unmap(static_cast<IndexBuffer&>(resource).getD3D12Buffer(), subresource);
				break;

			case Renderer::ResourceType::VERTEX_BUFFER:
				mD3D12DeviceContext->Unmap(static_cast<VertexBuffer&>(resource).getD3D12Buffer(), subresource);
				break;

			case Renderer::ResourceType::UNIFORM_BUFFER:
				mD3D12DeviceContext->Unmap(static_cast<UniformBuffer&>(resource).getD3D12Buffer(), subresource);
				break;

			case Renderer::ResourceType::TEXTURE_BUFFER:
				mD3D12DeviceContext->Unmap(static_cast<TextureBuffer&>(resource).getD3D12Buffer(), subresource);
				break;

			case Renderer::ResourceType::TEXTURE_2D:
			{
				// Get the Direct3D 12 resource instance
				ID3D12Resource *d3d12Resource = nullptr;
				static_cast<Texture2D&>(resource).getD3D12ShaderResourceView()->GetResource(&d3d12Resource);
				if (nullptr != d3d12Resource)
				{
					// Unmap the Direct3D 12 resource
					mD3D12DeviceContext->Unmap(d3d12Resource, subresource);

					// Release the Direct3D 12 resource instance
					d3d12Resource->Release();
				}
				break;
			}

			case Renderer::ResourceType::TEXTURE_2D_ARRAY:
			{
				// Get the Direct3D 12 resource instance
				ID3D12Resource *d3d12Resource = nullptr;
				static_cast<Texture2DArray&>(resource).getD3D12ShaderResourceView()->GetResource(&d3d12Resource);
				if (nullptr != d3d12Resource)
				{
					// Unmap the Direct3D 12 resource
					mD3D12DeviceContext->Unmap(d3d12Resource, subresource);

					// Release the Direct3D 12 resource instance
					d3d12Resource->Release();
				}
				break;
			}

			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
			case Renderer::ResourceType::SWAP_CHAIN:
			case Renderer::ResourceType::FRAMEBUFFER:
			case Renderer::ResourceType::RASTERIZER_STATE:
			case Renderer::ResourceType::DEPTH_STENCIL_STATE:
			case Renderer::ResourceType::BLEND_STATE:
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
		*/
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void Direct3D12Renderer::setGraphicsRootSignature(Renderer::IRootSignature* rootSignature)
	{
		if (nullptr != rootSignature)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D12RENDERER_RENDERERMATCHCHECK_RETURN(*this, *rootSignature)

			// Set graphics root signature
			mD3D12GraphicsCommandList->SetGraphicsRootSignature(static_cast<RootSignature*>(rootSignature)->getD3D12RootSignature());
		}
		else
		{
			// TODO(co) Handle this situation?
		}
	}

	void Direct3D12Renderer::setGraphicsRootDescriptorTable(uint32_t rootParameterIndex, Renderer::IResource* resource)
	{
		if (nullptr != resource)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D12RENDERER_RENDERERMATCHCHECK_RETURN(*this, *resource)

			switch (resource->getResourceType())
			{
				case Renderer::ResourceType::UNIFORM_BUFFER:
				{
					ID3D12DescriptorHeap* d3D12DescriptorHeap = static_cast<UniformBuffer*>(resource)->getD3D12DescriptorHeap();
					if (nullptr != d3D12DescriptorHeap)
					{
						// TODO(co) Just a first Direct3D 12 test, don't call "ID3D12GraphicsCommandList::SetDescriptorHeaps()" that often (pipeline flush)
						ID3D12DescriptorHeap* ppHeaps[] = { d3D12DescriptorHeap };
						mD3D12GraphicsCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

						mD3D12GraphicsCommandList->SetGraphicsRootDescriptorTable(rootParameterIndex, d3D12DescriptorHeap->GetGPUDescriptorHandleForHeapStart());
					}
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D:
				{
					ID3D12DescriptorHeap* d3D12DescriptorHeap = static_cast<Texture2D*>(resource)->getD3D12DescriptorHeap();
					if (nullptr != d3D12DescriptorHeap)
					{
						// TODO(co) Just a first Direct3D 12 test, don't call "ID3D12GraphicsCommandList::SetDescriptorHeaps()" that often (pipeline flush)
						ID3D12DescriptorHeap* ppHeaps[] = { d3D12DescriptorHeap };
						mD3D12GraphicsCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

						mD3D12GraphicsCommandList->SetGraphicsRootDescriptorTable(rootParameterIndex, d3D12DescriptorHeap->GetGPUDescriptorHandleForHeapStart());
					}
					break;
				}

				case Renderer::ResourceType::SAMPLER_STATE:
				{
					ID3D12DescriptorHeap* d3D12DescriptorHeap = static_cast<SamplerState*>(resource)->getD3D12DescriptorHeap();
					if (nullptr != d3D12DescriptorHeap)
					{
						// TODO(co) Just a first Direct3D 12 test, don't call "ID3D12GraphicsCommandList::SetDescriptorHeaps()" that often (pipeline flush)
						ID3D12DescriptorHeap* ppHeaps[] = { d3D12DescriptorHeap };
						mD3D12GraphicsCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

						mD3D12GraphicsCommandList->SetGraphicsRootDescriptorTable(rootParameterIndex, d3D12DescriptorHeap->GetGPUDescriptorHandleForHeapStart());
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
				case Renderer::ResourceType::TEXTURE_BUFFER:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				case Renderer::ResourceType::PIPELINE_STATE:
				case Renderer::ResourceType::RASTERIZER_STATE:
				case Renderer::ResourceType::DEPTH_STENCIL_STATE:
				case Renderer::ResourceType::BLEND_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 12 error: Invalid resource type")
					break;
			}
		}
		else
		{
			// TODO(co) Handle this situation?
		}
	}

	void Direct3D12Renderer::setPipelineState(Renderer::IPipelineState* pipelineState)
	{
		if (nullptr != pipelineState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D12RENDERER_RENDERERMATCHCHECK_RETURN(*this, *pipelineState)

			// Set graphics pipeline state
			mD3D12GraphicsCommandList->SetPipelineState(static_cast<PipelineState*>(pipelineState)->getD3D12PipelineState());
		}
		else
		{
			// TODO(co) Handle this situation?
		}
	}


	//[-------------------------------------------------------]
	//[ Input-assembler (IA) stage                            ]
	//[-------------------------------------------------------]
	void Direct3D12Renderer::iaSetVertexArray(Renderer::IVertexArray *vertexArray)
	{
		if (nullptr != vertexArray)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D12RENDERER_RENDERERMATCHCHECK_RETURN(*this, *vertexArray)

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

			static_cast<VertexArray*>(vertexArray)->setDirect3DIASetInputLayoutAndStreamSource(*mD3D12GraphicsCommandList);

			// End debug event
			RENDERER_END_DEBUG_EVENT(this)
		}
		else
		{
			// Set no Direct3D 12 input layout
			mD3D12GraphicsCommandList->IASetVertexBuffers(0, 0, nullptr);
		}
	}

	void Direct3D12Renderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology::Enum primitiveTopology)
	{
		// Set primitive topology
		// -> The "Renderer::PrimitiveTopology::Enum" values directly map to Direct3D 9 & 10 & 11 && 12 constants, do not change them
		mD3D12GraphicsCommandList->IASetPrimitiveTopology(static_cast<D3D12_PRIMITIVE_TOPOLOGY>(primitiveTopology));
	}


	//[-------------------------------------------------------]
	//[ Rasterizer (RS) stage                                 ]
	//[-------------------------------------------------------]
	void Direct3D12Renderer::rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport *viewports)
	{
		// Are the given viewports valid?
		if (numberOfViewports > 0 && nullptr != viewports)
		{
			// Set the Direct3D 12 viewports
			// -> "Renderer::Viewport" directly maps to Direct3D 12, do not change it
			// -> Let Direct3D 12 perform the index validation for us (the Direct3D 12 debug features are pretty good)
			mD3D12GraphicsCommandList->RSSetViewports(numberOfViewports, reinterpret_cast<const D3D12_VIEWPORT*>(viewports));
		}
	}

	void Direct3D12Renderer::rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles)
	{
		// Are the given scissor rectangles valid?
		if (numberOfScissorRectangles > 0 && nullptr != scissorRectangles)
		{
			// Set the Direct3D 12 scissor rectangles
			// -> "Renderer::ScissorRectangle" directly maps to Direct3D 9 & 10 & 11 & 12, do not change it
			// -> Let Direct3D 12 perform the index validation for us (the Direct3D 12 debug features are pretty good)
			mD3D12GraphicsCommandList->RSSetScissorRects(numberOfScissorRectangles, reinterpret_cast<const D3D12_RECT*>(scissorRectangles));
		}
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	Renderer::IRenderTarget *Direct3D12Renderer::omGetRenderTarget()
	{
		return mRenderTarget;
	}

	void Direct3D12Renderer::omSetRenderTarget(Renderer::IRenderTarget *renderTarget)
	{
		// New render target?
		if (mRenderTarget != renderTarget)
		{
			// Unset the previous render target
			if (nullptr != mRenderTarget)
			{
				// Evaluate the render target type
				switch (mRenderTarget->getResourceType())
				{
					case Renderer::ResourceType::SWAP_CHAIN:
					{
						// Get the Direct3D 12 swap chain instance
						SwapChain *swapChain = static_cast<SwapChain*>(mRenderTarget);

						// Inform Direct3D 12 about the resource transition
						CD3DX12_RESOURCE_BARRIER d3d12XResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(swapChain->getBackD3D12ResourceRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
						mD3D12GraphicsCommandList->ResourceBarrier(1, &d3d12XResourceBarrier);
						break;
					}

					case Renderer::ResourceType::FRAMEBUFFER:
					{
						// TODO(co) Implement resource transition handling (first "Direct3D12Renderer::Texture2D" needs to be cleaned up)
						/*
						// Get the Direct3D 12 framebuffer instance
						Framebuffer *framebuffer = static_cast<Framebuffer*>(mRenderTarget);

						// Inform Direct3D 12 about the resource transitions
						const uint32_t numberOfColorTextures = framebuffer->getNumberOfColorTextures();
						for (uint32_t i = 0; i < numberOfColorTextures; ++i)
						{
							// TODO(co) Resource type handling, currently only 2D texture is supported
							CD3DX12_RESOURCE_BARRIER d3d12XResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(static_cast<Texture2D*>(framebuffer->getColorTextures()[i])->getD3D12Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
							mD3D12GraphicsCommandList->ResourceBarrier(1, &d3d12XResourceBarrier);
						}
						if (nullptr != framebuffer->getDepthStencilTexture())
						{
							// TODO(co) Resource type handling, currently only 2D texture is supported
							CD3DX12_RESOURCE_BARRIER d3d12XResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(static_cast<Texture2D*>(framebuffer->getDepthStencilTexture())->getD3D12Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
							mD3D12GraphicsCommandList->ResourceBarrier(1, &d3d12XResourceBarrier);
						}
						*/
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
					case Renderer::ResourceType::RASTERIZER_STATE:
					case Renderer::ResourceType::DEPTH_STENCIL_STATE:
					case Renderer::ResourceType::BLEND_STATE:
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

				// Release the render target reference, in case we have one
				mRenderTarget->release();
				mRenderTarget = nullptr;
			}

			// Set a render target?
			if (nullptr != renderTarget)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				DIRECT3D12RENDERER_RENDERERMATCHCHECK_RETURN(*this, *renderTarget)

				// Set new render target and add a reference to it
				mRenderTarget = renderTarget;
				mRenderTarget->addReference();

				// Evaluate the render target type
				switch (mRenderTarget->getResourceType())
				{
					case Renderer::ResourceType::SWAP_CHAIN:
					{
						// Get the Direct3D 12 swap chain instance
						SwapChain *swapChain = static_cast<SwapChain*>(mRenderTarget);

						{ // Inform Direct3D 12 about the resource transition
							CD3DX12_RESOURCE_BARRIER d3d12XResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(swapChain->getBackD3D12ResourceRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
							mD3D12GraphicsCommandList->ResourceBarrier(1, &d3d12XResourceBarrier);
						}

						// Set Direct3D 12 render target
						CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(swapChain->getD3D12DescriptorHeapRenderTargetView()->GetCPUDescriptorHandleForHeapStart(), static_cast<INT>(swapChain->getBackD3D12ResourceRenderTargetFrameIndex()), swapChain->getRenderTargetViewDescriptorSize());
						CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(swapChain->getD3D12DescriptorHeapDepthStencilView()->GetCPUDescriptorHandleForHeapStart());
						mD3D12GraphicsCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
						break;
					}

					case Renderer::ResourceType::FRAMEBUFFER:
					{
						// Get the Direct3D 12 framebuffer instance
						Framebuffer *framebuffer = static_cast<Framebuffer*>(mRenderTarget);

						// Set the Direct3D 12 render targets
						const uint32_t numberOfColorTextures = framebuffer->getNumberOfColorTextures();
						D3D12_CPU_DESCRIPTOR_HANDLE d3d12CpuDescriptorHandlesRenderTarget[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];
						for (uint32_t i = 0; i < numberOfColorTextures; ++i)
						{
							d3d12CpuDescriptorHandlesRenderTarget[i] = framebuffer->getD3D12DescriptorHeapRenderTargetViews()[i]->GetCPUDescriptorHandleForHeapStart();

							// TODO(co) Implement resource transition handling (first "Direct3D12Renderer::Texture2D" needs to be cleaned up)
							/*
							{ // Inform Direct3D 12 about the resource transition
								// TODO(co) Resource type handling, currently only 2D texture is supported
								CD3DX12_RESOURCE_BARRIER d3d12XResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(static_cast<Texture2D*>(framebuffer->getColorTextures()[i])->getD3D12Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
								mD3D12GraphicsCommandList->ResourceBarrier(1, &d3d12XResourceBarrier);
							}
							*/
						}
						ID3D12DescriptorHeap* d3d12DescriptorHeapDepthStencilView = framebuffer->getD3D12DescriptorHeapDepthStencilView();
						if (nullptr != d3d12DescriptorHeapDepthStencilView)
						{
							// TODO(co) Implement resource transition handling (first "Direct3D12Renderer::Texture2D" needs to be cleaned up)
							/*
							{ // Inform Direct3D 12 about the resource transition
								// TODO(co) Resource type handling, currently only 2D texture is supported
								CD3DX12_RESOURCE_BARRIER d3d12XResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(static_cast<Texture2D*>(framebuffer->getDepthStencilTexture())->getD3D12Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
								mD3D12GraphicsCommandList->ResourceBarrier(1, &d3d12XResourceBarrier);
							}
							*/

							// Set the Direct3D 12 render targets
							D3D12_CPU_DESCRIPTOR_HANDLE d3d12CpuDescriptorHandlesDepthStencil = d3d12DescriptorHeapDepthStencilView->GetCPUDescriptorHandleForHeapStart();
							mD3D12GraphicsCommandList->OMSetRenderTargets(numberOfColorTextures, d3d12CpuDescriptorHandlesRenderTarget, FALSE, &d3d12CpuDescriptorHandlesDepthStencil);
						}
						else
						{
							mD3D12GraphicsCommandList->OMSetRenderTargets(numberOfColorTextures, d3d12CpuDescriptorHandlesRenderTarget, FALSE, nullptr);
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
					case Renderer::ResourceType::TEXTURE_2D:
					case Renderer::ResourceType::TEXTURE_2D_ARRAY:
					case Renderer::ResourceType::PIPELINE_STATE:
					case Renderer::ResourceType::RASTERIZER_STATE:
					case Renderer::ResourceType::DEPTH_STENCIL_STATE:
					case Renderer::ResourceType::BLEND_STATE:
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
				mD3D12GraphicsCommandList->OMSetRenderTargets(0, nullptr, FALSE, nullptr);
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Operations                                            ]
	//[-------------------------------------------------------]
	void Direct3D12Renderer::clear(uint32_t flags, const float color[4], float z, uint32_t stencil)
	{
		// Unlike Direct3D 9, OpenGL or OpenGL ES 2, Direct3D 12 clears a given render target view and not the currently bound
		// -> No resource transition required in here, it's handled inside "Direct3D12Renderer::omSetRenderTarget()"

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
					// Get the Direct3D 12 swap chain instance
					SwapChain *swapChain = static_cast<SwapChain*>(mRenderTarget);

					// Clear the Direct3D 12 render target view?
					if (flags & Renderer::ClearFlag::COLOR)
					{
						CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(swapChain->getD3D12DescriptorHeapRenderTargetView()->GetCPUDescriptorHandleForHeapStart(), static_cast<INT>(swapChain->getBackD3D12ResourceRenderTargetFrameIndex()), swapChain->getRenderTargetViewDescriptorSize());
						mD3D12GraphicsCommandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
					}

					// Clear the Direct3D 12 depth stencil view?
					ID3D12DescriptorHeap* d3d12DescriptorHeapDepthStencilView = swapChain->getD3D12DescriptorHeapDepthStencilView();
					if (nullptr != d3d12DescriptorHeapDepthStencilView)
					{
						// Get the Direct3D 12 clear flags
						UINT direct3D12ClearFlags = (flags & Renderer::ClearFlag::DEPTH) ? D3D12_CLEAR_FLAG_DEPTH : 0u;
						if (flags & Renderer::ClearFlag::STENCIL)
						{
							direct3D12ClearFlags |= D3D12_CLEAR_FLAG_STENCIL;
						}
						if (0 != direct3D12ClearFlags)
						{
							// Clear the Direct3D 12 depth stencil view
							mD3D12GraphicsCommandList->ClearDepthStencilView(d3d12DescriptorHeapDepthStencilView->GetCPUDescriptorHandleForHeapStart(), static_cast<D3D12_CLEAR_FLAGS>(direct3D12ClearFlags), z, static_cast<UINT8>(stencil), 0, nullptr);
						}
					}
					break;
				}

				case Renderer::ResourceType::FRAMEBUFFER:
				{
					// Get the Direct3D 12 framebuffer instance
					Framebuffer *framebuffer = static_cast<Framebuffer*>(mRenderTarget);

					// Clear all Direct3D 12 render target views?
					if (flags & Renderer::ClearFlag::COLOR)
					{
						// Loop through all Direct3D 12 render target views
						ID3D12DescriptorHeap **d3d12DescriptorHeapRenderTargetViews = framebuffer->getD3D12DescriptorHeapRenderTargetViews() + framebuffer->getNumberOfColorTextures();
						for (ID3D12DescriptorHeap **d3d12DescriptorHeapRenderTargetView = framebuffer->getD3D12DescriptorHeapRenderTargetViews(); d3d12DescriptorHeapRenderTargetView < d3d12DescriptorHeapRenderTargetViews; ++d3d12DescriptorHeapRenderTargetView)
						{
							// Valid Direct3D 12 render target view?
							if (nullptr != *d3d12DescriptorHeapRenderTargetView)
							{
								mD3D12GraphicsCommandList->ClearRenderTargetView((*d3d12DescriptorHeapRenderTargetView)->GetCPUDescriptorHandleForHeapStart(), color, 0, nullptr);
							}
						}
					}

					// Clear the Direct3D 12 depth stencil view?
					ID3D12DescriptorHeap* d3d12DescriptorHeapDepthStencilView = framebuffer->getD3D12DescriptorHeapDepthStencilView();
					if (nullptr != d3d12DescriptorHeapDepthStencilView)
					{
						// Get the Direct3D 12 clear flags
						UINT direct3D12ClearFlags = (flags & Renderer::ClearFlag::DEPTH) ? D3D12_CLEAR_FLAG_DEPTH : 0u;
						if (flags & Renderer::ClearFlag::STENCIL)
						{
							direct3D12ClearFlags |= D3D12_CLEAR_FLAG_STENCIL;
						}
						if (0 != direct3D12ClearFlags)
						{
							// Clear the Direct3D 12 depth stencil view
							mD3D12GraphicsCommandList->ClearDepthStencilView(d3d12DescriptorHeapDepthStencilView->GetCPUDescriptorHandleForHeapStart(), static_cast<D3D12_CLEAR_FLAGS>(direct3D12ClearFlags), z, static_cast<UINT8>(stencil), 0, nullptr);
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
				case Renderer::ResourceType::TEXTURE_2D:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				case Renderer::ResourceType::PIPELINE_STATE:
				case Renderer::ResourceType::RASTERIZER_STATE:
				case Renderer::ResourceType::DEPTH_STENCIL_STATE:
				case Renderer::ResourceType::BLEND_STATE:
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

	bool Direct3D12Renderer::beginScene()
	{
		bool result = false;	// Error by default

		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		// Not required when using Direct3D 12
		// TODO(co) Until we have a command list interface, we must perform the command list handling in here

		// Command list allocators can only be reset when the associated
		// command lists have finished execution on the GPU; apps should use
		// fences to determine GPU execution progress.
		if (SUCCEEDED(mD3D12CommandAllocator->Reset()))
		{
			// However, when ExecuteCommandList() is called on a particular command
			// list, that command list can then be reset at any time and must be before
			// re-recording.
			result = SUCCEEDED(mD3D12GraphicsCommandList->Reset(mD3D12CommandAllocator, nullptr));
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)

		// Done
		return result;
	}

	void Direct3D12Renderer::endScene()
	{
		// Not required when using Direct3D 12
		// TODO(co) Until we have a command list interface, we must perform the command list handling in here

		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		// We need to forget about the currently set render target
		omSetRenderTarget(nullptr);

		// Close and execute the command list
		if (SUCCEEDED(mD3D12GraphicsCommandList->Close()))
		{
			ID3D12CommandList* commandLists[] = { mD3D12GraphicsCommandList };
			mD3D12CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}


	//[-------------------------------------------------------]
	//[ Draw call                                             ]
	//[-------------------------------------------------------]
	void Direct3D12Renderer::draw(uint32_t startVertexLocation, uint32_t numberOfVertices)
	{
		mD3D12GraphicsCommandList->DrawInstanced(
			numberOfVertices,		// Number of vertices to draw (UINT)
			1,						// Number of instances to draw (UINT)
			startVertexLocation,	// Index of the first vertex (UINT)
			0);						// A value added to each index before reading per-instance data from a vertex buffer (UINT)
	}

	void Direct3D12Renderer::drawInstanced(uint32_t startVertexLocation, uint32_t numberOfVertices, uint32_t numberOfInstances)
	{
		mD3D12GraphicsCommandList->DrawInstanced(
			numberOfVertices,		// Number of vertices to draw (UINT)
			numberOfInstances,		// Number of instances to draw (UINT)
			startVertexLocation,	// Index of the first vertex (UINT)
			0);						// A value added to each index before reading per-instance data from a vertex buffer (UINT)
	}

	void Direct3D12Renderer::drawIndexed(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t, uint32_t)
	{
		// "minimumIndex" & "numberOfVertices" are not supported by Direct3D 12

		// Draw
		mD3D12GraphicsCommandList->DrawIndexedInstanced(
			numberOfIndices,						// Number of indices read from the index buffer for each instance (UINT)
			1,										// Number of instances to draw (UINT)
			startIndexLocation,						// The location of the first index read by the GPU from the index buffer (UINT)
			static_cast<INT>(baseVertexLocation),	// A value added to each index before reading a vertex from the vertex buffer (INT)
			0);										// A value added to each index before reading per-instance data from a vertex buffer (UINT)
	}

	void Direct3D12Renderer::drawIndexedInstanced(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t, uint32_t, uint32_t numberOfInstances)
	{
		// "minimumIndex" & "numberOfVertices" are not supported by Direct3D 12

		// Draw
		mD3D12GraphicsCommandList->DrawIndexedInstanced(
			numberOfIndices,						// Number of indices read from the index buffer for each instance (UINT)
			numberOfInstances,						// Number of instances to draw (UINT)
			startIndexLocation,						// The location of the first index read by the GPU from the index buffer (UINT)
			static_cast<INT>(baseVertexLocation),	// A value added to each index before reading a vertex from the vertex buffer (INT)
			0);										// A value added to each index before reading per-instance data from a vertex buffer (UINT)
	}


	//[-------------------------------------------------------]
	//[ Synchronization                                       ]
	//[-------------------------------------------------------]
	void Direct3D12Renderer::flush()
	{
		// TODO(co) Direct3D 12 update
	//	mD3D12DeviceContext->Flush();
	}

	void Direct3D12Renderer::finish()
	{
		// TODO(co) Direct3D 12 update
		/*
		// Create the Direct3D 12 query instance used for flush right now?
		if (nullptr == mD3D12QueryFlush)
		{
			D3D12_QUERY_DESC d3d12QueryDesc;
			d3d12QueryDesc.Query	 = D3D12_QUERY_EVENT;
			d3d12QueryDesc.MiscFlags = 0;
			mD3D12Device->CreateQuery(&d3d12QueryDesc, &mD3D12QueryFlush);

			#ifndef DIRECT3D12RENDERER_NO_DEBUG
				// Set the debug name
				if (nullptr != mD3D12QueryFlush)
				{
					// No need to reset the previous private data, there shouldn't be any...
					mD3D12QueryFlush->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(__FUNCTION__), __FUNCTION__);
				}
			#endif
		}
		if (nullptr != mD3D12QueryFlush)
		{
			// Perform the flush and wait
			mD3D12DeviceContext->End(mD3D12QueryFlush);
			mD3D12DeviceContext->Flush();
			BOOL result = FALSE;
			do
			{
				// Spin-wait
				mD3D12DeviceContext->GetData(mD3D12QueryFlush, &result, sizeof(BOOL), 0);
			} while (!result);
		}
		*/
	}


	//[-------------------------------------------------------]
	//[ Debug                                                 ]
	//[-------------------------------------------------------]
	bool Direct3D12Renderer::isDebugEnabled()
	{
		#ifdef DIRECT3D12RENDERER_NO_DEBUG
			return false;
		#else
			return true;
		#endif
	}

	void Direct3D12Renderer::setDebugMarker(const wchar_t *name)
	{
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			if (nullptr != mD3D12GraphicsCommandList)
			{
				const UINT size = static_cast<UINT>((wcslen(name) + 1) * sizeof(name[0]));
				mD3D12GraphicsCommandList->SetMarker(PIX_EVENT_UNICODE_VERSION, name, size);
			}
		#endif
	}

	void Direct3D12Renderer::beginDebugEvent(const wchar_t *name)
	{
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			if (nullptr != mD3D12GraphicsCommandList)
			{
				const UINT size = static_cast<UINT>((wcslen(name) + 1) * sizeof(name[0]));
				mD3D12GraphicsCommandList->BeginEvent(PIX_EVENT_UNICODE_VERSION, name, size);
			}
		#endif
	}

	void Direct3D12Renderer::endDebugEvent()
	{
		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			if (nullptr != mD3D12GraphicsCommandList)
			{
				mD3D12GraphicsCommandList->EndEvent();
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void Direct3D12Renderer::initializeCapabilities()
	{
		// TODO(co) Direct3D 12 update

		// There are no Direct3D 12 device capabilities we could query on runtime, they depend on the chosen feature level
		// -> Have a look at "Devices -> Direct3D 12 on Downlevel Hardware -> Introduction" at MSDN http://msdn.microsoft.com/en-us/library/ff476876%28v=vs.85%29.aspx
		//    for a table with a list of the minimum resources supported by Direct3D 12 at the different feature levels

		// Evaluate the chosen feature level
		switch (D3D_FEATURE_LEVEL_12_0)
		// switch (mD3D12Device->GetFeatureLevel())	// TODO(co) Direct3D 12 update
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
				mCapabilities.maximumTextureBufferSize = 0;	// TODO(co) http://msdn.microsoft.com/en-us/library/ff476876%28v=vs.85%29.aspx does not mention the texture buffer?

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
				mCapabilities.maximumTextureBufferSize = 0;	// TODO(co) http://msdn.microsoft.com/en-us/library/ff476876%28v=vs.85%29.aspx does not mention the texture buffer?

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
				mCapabilities.maximumTextureBufferSize = 0;	// TODO(co) http://msdn.microsoft.com/en-us/library/ff476876%28v=vs.85%29.aspx does not mention the texture buffer?

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
				// TODO(co) Direct3D 12 update
				// mCapabilities.maximumNumberOfViewports = D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX + 1;
				mCapabilities.maximumNumberOfViewports = 8;

				// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
				// TODO(co) Direct3D 12 update
				//mCapabilities.maximumNumberOfSimultaneousRenderTargets = D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;
				mCapabilities.maximumNumberOfSimultaneousRenderTargets = 8;

				// Maximum texture dimension
				mCapabilities.maximumTextureDimension = 8192;

				// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
				mCapabilities.maximumNumberOf2DTextureArraySlices = 512;

				// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
				mCapabilities.maximumTextureBufferSize = 2000;	// TODO(co) http://msdn.microsoft.com/en-us/library/ff476876%28v=vs.85%29.aspx does not mention the texture buffer?

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
				// TODO(co) Direct3D 12 update
				//mCapabilities.maximumNumberOfViewports = D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX + 1;
				mCapabilities.maximumNumberOfViewports = 8;

				// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
				// TODO(co) Direct3D 12 update
				//mCapabilities.maximumNumberOfSimultaneousRenderTargets = D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;
				mCapabilities.maximumNumberOfSimultaneousRenderTargets = 8;

				// Maximum texture dimension
				mCapabilities.maximumTextureDimension = 8192;

				// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
				mCapabilities.maximumNumberOf2DTextureArraySlices = 512;

				// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
				mCapabilities.maximumTextureBufferSize = 2000;	// TODO(co) http://msdn.microsoft.com/en-us/library/ff476876%28v=vs.85%29.aspx does not mention the texture buffer?

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
			case D3D_FEATURE_LEVEL_11_1:
			case D3D_FEATURE_LEVEL_12_0:
			case D3D_FEATURE_LEVEL_12_1:
				// Maximum number of viewports (always at least 1)
				// TODO(co) Direct3D 12 update
				//mCapabilities.maximumNumberOfViewports = D3D12_VIEWPORT_AND_SCISSORRECT_MAX_INDEX + 1;
				mCapabilities.maximumNumberOfViewports = 8;

				// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
				mCapabilities.maximumNumberOfSimultaneousRenderTargets = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;

				// Maximum texture dimension
				mCapabilities.maximumTextureDimension = 16384;

				// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
				mCapabilities.maximumNumberOf2DTextureArraySlices = 512;

				// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
				mCapabilities.maximumTextureBufferSize = 2000;	// TODO(co) http://msdn.microsoft.com/en-us/library/ff476876%28v=vs.85%29.aspx does not mention the texture buffer?

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

		// Uniform buffer object (UBO, "constant buffer" in Direct3D terminology) supported?
		mCapabilities.uniformBuffer = true;

		// Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		mCapabilities.individualUniforms = false;

		// Base vertex supported for draw calls?
		mCapabilities.baseVertex = true;

		// Is there support for vertex shaders (VS)?
		mCapabilities.vertexShader = true;

		// Is there support for fragment shaders (FS)?
		mCapabilities.fragmentShader = true;
	}

	#ifndef DIRECT3D12RENDERER_NO_DEBUG
		void Direct3D12Renderer::debugReportLiveDeviceObjects()
		{
			ID3D12DebugDevice* d3d12DebugDevice = nullptr;
			if (SUCCEEDED(mD3D12Device->QueryInterface(IID_PPV_ARGS(&d3d12DebugDevice))))
			{
				d3d12DebugDevice->ReportLiveDeviceObjects(static_cast<D3D12_RLDO_FLAGS>(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
				d3d12DebugDevice->Release();
			}
		}
	#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
