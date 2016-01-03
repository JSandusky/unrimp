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
#include "Direct3D11Renderer/Direct3D11Renderer.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/Direct3D11Debug.h"	// For "DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "Direct3D11Renderer/Direct3D9RuntimeLinking.h"	//  For the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box) used for debugging
#include "Direct3D11Renderer/Direct3D11RuntimeLinking.h"
#include "Direct3D11Renderer/Texture2D.h"
#include "Direct3D11Renderer/VertexArray.h"
#include "Direct3D11Renderer/SwapChain.h"
#include "Direct3D11Renderer/Framebuffer.h"
#include "Direct3D11Renderer/IndexBuffer.h"
#include "Direct3D11Renderer/SamplerState.h"
#include "Direct3D11Renderer/VertexBuffer.h"
#include "Direct3D11Renderer/RootSignature.h"
#include "Direct3D11Renderer/PipelineState.h"
#include "Direct3D11Renderer/TextureBuffer.h"
#include "Direct3D11Renderer/Texture2DArray.h"
#include "Direct3D11Renderer/Shader/ProgramHlsl.h"
#include "Direct3D11Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D11Renderer/Shader/UniformBuffer.h"
#include "Direct3D11Renderer/Shader/VertexShaderHlsl.h"
#include "Direct3D11Renderer/Shader/GeometryShaderHlsl.h"
#include "Direct3D11Renderer/Shader/FragmentShaderHlsl.h"
#include "Direct3D11Renderer/Shader/TessellationControlShaderHlsl.h"
#include "Direct3D11Renderer/Shader/TessellationEvaluationShaderHlsl.h"


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef DIRECT3D11RENDERER_EXPORTS
	#define DIRECT3D11RENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define DIRECT3D11RENDERER_API_EXPORT
#endif
DIRECT3D11RENDERER_API_EXPORT Renderer::IRenderer *createDirect3D11RendererInstance(handle nativeWindowHandle)
{
	return new Direct3D11Renderer::Direct3D11Renderer(nativeWindowHandle);
}
#undef DIRECT3D11RENDERER_API_EXPORT


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	namespace detail
	{

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
				if (SUCCEEDED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPES[deviceType], nullptr, flags, D3D_FEATURE_LEVELS, NUMBER_OF_FEATURE_LEVELS, D3D11_SDK_VERSION, d3d11Device, &d3dFeatureLevel, d3d11DeviceContext)))
				{
					// Done
					return true;
				}
			}

			// Error!
			return false;
		}
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Direct3D11Renderer::Direct3D11Renderer(handle nativeWindowHandle) :
		mDirect3D9RuntimeLinking(nullptr),
		mDirect3D11RuntimeLinking(new Direct3D11RuntimeLinking()),
		mD3D11Device(nullptr),
		mD3D11DeviceContext(nullptr),
		mShaderLanguageHlsl(nullptr),
		mD3D11QueryFlush(nullptr),
		mMainSwapChain(nullptr),
		mRenderTarget(nullptr),
		mGraphicsRootSignature(nullptr)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		// Is Direct3D 11 available?
		if (mDirect3D11RuntimeLinking->isDirect3D11Avaiable())
		{
			{ // Create the Direct3D 11 device
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
			}

			// Is there a valid Direct3D 11 device and device context?
			if (nullptr != mD3D11Device && nullptr != mD3D11DeviceContext)
			{
				#ifdef DIRECT3D11RENDERER_NO_DEBUG
					// Create the Direct3D 9 runtime linking instance, we know there can't be one, yet
					mDirect3D9RuntimeLinking = new Direct3D9RuntimeLinking();

					// Call the Direct3D 9 PIX function
					if (mDirect3D9RuntimeLinking->isDirect3D9Avaiable())
					{
						// Disable debugging
						D3DPERF_SetOptions(1);
					}
				#endif

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
			mMainSwapChain->release();
			mMainSwapChain = nullptr;
		}
		if (nullptr != mRenderTarget)
		{
			mRenderTarget->release();
			mRenderTarget = nullptr;
		}
		if (nullptr != mGraphicsRootSignature)
		{
			mGraphicsRootSignature->release();
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

		// Release the Direct3D 11 query instance used for flush, in case we have one
		if (nullptr != mD3D11QueryFlush)
		{
			mD3D11QueryFlush->Release();
		}

		// Release the HLSL shader language instance, in case we have one
		if (nullptr != mShaderLanguageHlsl)
		{
			mShaderLanguageHlsl->release();
		}

		// Release the Direct3D 11 device we've created
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

		// Destroy the Direct3D 9 runtime linking instance, in case there's one
		if (nullptr != mDirect3D9RuntimeLinking)
		{
			delete mDirect3D9RuntimeLinking;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
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
	Renderer::ISwapChain *Direct3D11Renderer::createSwapChain(handle nativeWindowHandle)
	{
		// The provided native window handle must not be a null handle
		return (NULL_HANDLE != nativeWindowHandle) ? new SwapChain(*this, nativeWindowHandle) : nullptr;
	}

	Renderer::IFramebuffer *Direct3D11Renderer::createFramebuffer(uint32_t numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture)
	{
		// Validation is done inside the framebuffer implementation
		return new Framebuffer(*this, numberOfColorTextures, colorTextures, depthStencilTexture);
	}

	Renderer::IVertexBuffer *Direct3D11Renderer::createVertexBuffer(uint32_t numberOfBytes, const void *data, Renderer::BufferUsage bufferUsage)
	{
		return new VertexBuffer(*this, numberOfBytes, data, bufferUsage);
	}

	Renderer::IIndexBuffer *Direct3D11Renderer::createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data, Renderer::BufferUsage bufferUsage)
	{
		return new IndexBuffer(*this, numberOfBytes, indexBufferFormat, data, bufferUsage);
	}

	Renderer::IVertexArray *Direct3D11Renderer::createVertexArray(const Renderer::VertexAttributes&, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, Renderer::IIndexBuffer *indexBuffer)
	{
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		return new VertexArray(*this, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
	}

	Renderer::ITextureBuffer *Direct3D11Renderer::createTextureBuffer(uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data, Renderer::BufferUsage bufferUsage)
	{
		return new TextureBuffer(*this, numberOfBytes, textureFormat, data, bufferUsage);
	}

	Renderer::ITexture2D *Direct3D11Renderer::createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags, Renderer::TextureUsage textureUsage, const Renderer::OptimizedTextureClearValue*)
	{
		return new Texture2D(*this, width, height, textureFormat, data, flags, textureUsage);
	}

	Renderer::ITexture2DArray *Direct3D11Renderer::createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags, Renderer::TextureUsage textureUsage)
	{
		return new Texture2DArray(*this, width, height, numberOfSlices, textureFormat, data, flags, textureUsage);
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

			case Renderer::ResourceType::TEXTURE_2D:
			{
				bool result = false;

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

				// Done
				return result;
			}

			case Renderer::ResourceType::TEXTURE_2D_ARRAY:
			{
				bool result = false;

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

	void Direct3D11Renderer::unmap(Renderer::IResource &resource, uint32_t subresource)
	{
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

			case Renderer::ResourceType::TEXTURE_2D:
			{
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
				break;
			}

			case Renderer::ResourceType::TEXTURE_2D_ARRAY:
			{
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
	//[ States                                                ]
	//[-------------------------------------------------------]
	void Direct3D11Renderer::setGraphicsRootSignature(Renderer::IRootSignature *rootSignature)
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
			if (nullptr == rootParameter.descriptorTable.descriptorRanges)
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
			const Renderer::DescriptorRange* descriptorRange = rootParameter.descriptorTable.descriptorRanges;

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
				case Renderer::ResourceType::TEXTURE_2D:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					ID3D11ShaderResourceView *d3d11ShaderResourceView = nullptr;
					switch (resourceType)
					{
						case Renderer::ResourceType::TEXTURE_BUFFER:
							d3d11ShaderResourceView = static_cast<TextureBuffer*>(resource)->getD3D11ShaderResourceView();
							break;

						case Renderer::ResourceType::TEXTURE_2D:
							d3d11ShaderResourceView = static_cast<Texture2D*>(resource)->getD3D11ShaderResourceView();
							break;

						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
							d3d11ShaderResourceView = static_cast<Texture2DArray*>(resource)->getD3D11ShaderResourceView();
							break;

						case Renderer::ResourceType::ROOT_SIGNATURE:
						case Renderer::ResourceType::PROGRAM:
						case Renderer::ResourceType::VERTEX_ARRAY:
						case Renderer::ResourceType::SWAP_CHAIN:
						case Renderer::ResourceType::FRAMEBUFFER:
						case Renderer::ResourceType::INDEX_BUFFER:
						case Renderer::ResourceType::VERTEX_BUFFER:
						case Renderer::ResourceType::UNIFORM_BUFFER:
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
	Renderer::IRenderTarget *Direct3D11Renderer::omGetRenderTarget()
	{
		return mRenderTarget;
	}

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
				if (nullptr != mRenderTarget)
				{
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
						mD3D11DeviceContext->OMSetRenderTargets(framebuffer->getNumberOfD3D11RenderTargetViews(), framebuffer->getD3D11RenderTargetViews(), framebuffer->getD3D11DepthStencilView());
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
				// Set the Direct3D 11 render targets
				mD3D11DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

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
						ID3D11RenderTargetView **d3d11RenderTargetViewsEnd = framebuffer->getD3D11RenderTargetViews() + framebuffer->getNumberOfD3D11RenderTargetViews();
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

	bool Direct3D11Renderer::beginScene()
	{
		// Not required when using Direct3D 11

		// Done
		return true;
	}

	void Direct3D11Renderer::endScene()
	{
		// We need to forget about the currently set render target
		omSetRenderTarget(nullptr);
	}


	//[-------------------------------------------------------]
	//[ Draw call                                             ]
	//[-------------------------------------------------------]
	void Direct3D11Renderer::draw(uint32_t startVertexLocation, uint32_t numberOfVertices)
	{
		mD3D11DeviceContext->Draw(
			numberOfVertices,	// Vertex count (UINT)
			startVertexLocation	// Start index location (UINT)
		);
	}

	void Direct3D11Renderer::drawInstanced(uint32_t startVertexLocation, uint32_t numberOfVertices, uint32_t numberOfInstances)
	{
		mD3D11DeviceContext->DrawInstanced(
			numberOfVertices,		// Vertex count per instance (UINT)
			numberOfInstances,		// Instance count (UINT)
			startVertexLocation,	// Start vertex location (UINT)
			0						// Start instance location (UINT)
		);
	}

	void Direct3D11Renderer::drawIndexed(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t, uint32_t)
	{
		// "minimumIndex" & "numberOfVertices" are not supported by Direct3D 11

		// Draw
		mD3D11DeviceContext->DrawIndexed(
			numberOfIndices,						// Index count (UINT)
			startIndexLocation,						// Start index location (UINT)
			static_cast<INT>(baseVertexLocation)	// Base vertex location (INT)
		);
	}

	void Direct3D11Renderer::drawIndexedInstanced(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t, uint32_t, uint32_t numberOfInstances)
	{
		// "minimumIndex" & "numberOfVertices" are not supported by Direct3D 11

		// Draw
		mD3D11DeviceContext->DrawIndexedInstanced(
			numberOfIndices,						// Index count per instance (UINT)
			numberOfInstances,						// Instance count (UINT)
			startIndexLocation,						// Start index location (UINT)
			static_cast<INT>(baseVertexLocation),	// Base vertex location (INT)
			0										// Start instance location (UINT)
		);
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
					mD3D11QueryFlush->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(__FUNCTION__), __FUNCTION__);
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
	//[ Debug                                                 ]
	//[-------------------------------------------------------]
	bool Direct3D11Renderer::isDebugEnabled()
	{
		// Don't check for the "DIRECT3D9RENDERER_NO_DEBUG" preprocessor definition, even if debug
		// is disabled it has to be possible to use this function for an additional security check
		// -> Maybe a debugger/profiler ignores the debug state
		// -> Maybe someone manipulated the binary to enable the debug state, adding a second check
		//    makes it a little bit more time consuming to hack the binary :D (but of course, this is no 100% security)
		return (D3DPERF_GetStatus() != 0);
	}

	void Direct3D11Renderer::setDebugMarker(const wchar_t *name)
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			// Create the Direct3D 9 runtime linking instance, in case there's no one, yet
			if (nullptr == mDirect3D9RuntimeLinking)
			{
				mDirect3D9RuntimeLinking = new Direct3D9RuntimeLinking();
			}

			// Call the Direct3D 9 PIX function
			if (mDirect3D9RuntimeLinking->isDirect3D9Avaiable())
			{
				D3DPERF_SetMarker(D3DCOLOR_RGBA(255, 0, 255, 255), name);
			}
		#endif
	}

	void Direct3D11Renderer::beginDebugEvent(const wchar_t *name)
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
			// Create the Direct3D 9 runtime linking instance, in case there's no one, yet
			if (nullptr == mDirect3D9RuntimeLinking)
			{
				mDirect3D9RuntimeLinking = new Direct3D9RuntimeLinking();
			}

			// Call the Direct3D 9 PIX function
			if (mDirect3D9RuntimeLinking->isDirect3D9Avaiable())
			{
				D3DPERF_BeginEvent(D3DCOLOR_RGBA(255, 255, 255, 255), name);
			}
		#endif
	}

	void Direct3D11Renderer::endDebugEvent()
	{
		#ifndef DIRECT3D11RENDERER_NO_DEBUG
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
				mCapabilities.maximumNumberOfViewports = D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX + 1;

				// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
				mCapabilities.maximumNumberOfSimultaneousRenderTargets = D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;

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
				mCapabilities.maximumNumberOfViewports = D3D10_VIEWPORT_AND_SCISSORRECT_MAX_INDEX + 1;

				// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
				mCapabilities.maximumNumberOfSimultaneousRenderTargets = D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;

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
				// Maximum number of viewports (always at least 1)
				mCapabilities.maximumNumberOfViewports = D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX + 1;

				// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
				mCapabilities.maximumNumberOfSimultaneousRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;

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

	void Direct3D11Renderer::setProgram(Renderer::IProgram *program)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		// TODO(co) Avoid changing already set program

		if (nullptr != program)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *program)

			// TODO(co) HLSL buffer settings, unset previous program

			// Evaluate the internal program type of the new program to set
			switch (static_cast<Program*>(program)->getInternalResourceType())
			{
				case Program::InternalResourceType::HLSL:
				{
					// Get shaders
					const ProgramHlsl					   *programHlsl						 = static_cast<ProgramHlsl*>(program);
					const VertexShaderHlsl				   *vertexShaderHlsl				 = programHlsl->getVertexShaderHlsl();
					const TessellationControlShaderHlsl	   *tessellationControlShaderHlsl	 = programHlsl->getTessellationControlShaderHlsl();
					const TessellationEvaluationShaderHlsl *tessellationEvaluationShaderHlsl = programHlsl->getTessellationEvaluationShaderHlsl();
					const GeometryShaderHlsl			   *geometryShaderHlsl				 = programHlsl->getGeometryShaderHlsl();
					const FragmentShaderHlsl			   *fragmentShaderHlsl				 = programHlsl->getFragmentShaderHlsl();

					// Set shaders
					mD3D11DeviceContext->VSSetShader(vertexShaderHlsl				  ? vertexShaderHlsl->getD3D11VertexShader()				 : nullptr, nullptr, 0);
					mD3D11DeviceContext->HSSetShader(tessellationControlShaderHlsl	  ? tessellationControlShaderHlsl->getD3D11HullShader()		 : nullptr, nullptr, 0);
					mD3D11DeviceContext->DSSetShader(tessellationEvaluationShaderHlsl ? tessellationEvaluationShaderHlsl->getD3D11DomainShader() : nullptr, nullptr, 0);
					mD3D11DeviceContext->GSSetShader(geometryShaderHlsl				  ? geometryShaderHlsl->getD3D11GeometryShader()			 : nullptr, nullptr, 0);
					mD3D11DeviceContext->PSSetShader(fragmentShaderHlsl				  ? fragmentShaderHlsl->getD3D11PixelShader()				 : nullptr, nullptr, 0);
					break;
				}
			}
		}
		else
		{
			// TODO(co) HLSL buffer settings
			mD3D11DeviceContext->VSSetShader(nullptr, nullptr, 0);
			mD3D11DeviceContext->HSSetShader(nullptr, nullptr, 0);
			mD3D11DeviceContext->DSSetShader(nullptr, nullptr, 0);
			mD3D11DeviceContext->GSSetShader(nullptr, nullptr, 0);
			mD3D11DeviceContext->PSSetShader(nullptr, nullptr, 0);
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
