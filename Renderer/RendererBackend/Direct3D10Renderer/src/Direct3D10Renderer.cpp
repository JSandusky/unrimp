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
#include "Direct3D10Renderer/Direct3D10Renderer.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/Direct3D10Debug.h"	// For "DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "Direct3D10Renderer/Direct3D9RuntimeLinking.h"	//  For the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box) used for debugging, also works directly within VisualStudio 2012 out-of-the-box
#include "Direct3D10Renderer/Direct3D10RuntimeLinking.h"
#include "Direct3D10Renderer/Texture2D.h"
#include "Direct3D10Renderer/BlendState.h"
#include "Direct3D10Renderer/VertexArray.h"
#include "Direct3D10Renderer/SwapChain.h"
#include "Direct3D10Renderer/Framebuffer.h"
#include "Direct3D10Renderer/IndexBuffer.h"
#include "Direct3D10Renderer/SamplerState.h"
#include "Direct3D10Renderer/VertexBuffer.h"
#include "Direct3D10Renderer/RootSignature.h"
#include "Direct3D10Renderer/PipelineState.h"
#include "Direct3D10Renderer/TextureBuffer.h"
#include "Direct3D10Renderer/Texture2DArray.h"
#include "Direct3D10Renderer/RasterizerState.h"
#include "Direct3D10Renderer/DepthStencilState.h"
#include "Direct3D10Renderer/TextureCollection.h"
#include "Direct3D10Renderer/SamplerStateCollection.h"
#include "Direct3D10Renderer/Shader/ProgramHlsl.h"
#include "Direct3D10Renderer/Shader/UniformBuffer.h"
#include "Direct3D10Renderer/Shader/VertexShaderHlsl.h"
#include "Direct3D10Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D10Renderer/Shader/GeometryShaderHlsl.h"
#include "Direct3D10Renderer/Shader/FragmentShaderHlsl.h"


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
		mRenderTarget(nullptr)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		// Is Direct3D 10 available?
		if (mDirect3D10RuntimeLinking->isDirect3D10Avaiable())
		{
			{ // Create the Direct3D 10 device
				// Flags
				UINT flags = 0;
			#ifdef _DEBUG
				flags |= D3D10_CREATE_DEVICE_DEBUG;
			#endif

				// Driver types
				static const D3D10_DRIVER_TYPE D3D10_DRIVER_TYPES[] =
				{
					D3D10_DRIVER_TYPE_HARDWARE,
					D3D10_DRIVER_TYPE_WARP,
					D3D10_DRIVER_TYPE_REFERENCE,
				};
				static const UINT NUMBER_OF_DRIVER_TYPES = sizeof(D3D10_DRIVER_TYPES) / sizeof(D3D10_DRIVER_TYPE);

				// Create the Direct3D 10 device
				for (UINT i = 0; i < NUMBER_OF_DRIVER_TYPES; ++i)
				{
					if (SUCCEEDED(D3D10CreateDevice(nullptr, D3D10_DRIVER_TYPES[i], nullptr, flags, D3D10_SDK_VERSION, &mD3D10Device)))
					{
						// Done
						i = NUMBER_OF_DRIVER_TYPES;
					}
				}
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

				// Initialize the capabilities
				initializeCapabilities();

				// Create a main swap chain instance?
				if (NULL_HANDLE != nativeWindowHandle)
				{
					// Create a main swap chain instance
					mMainSwapChain = static_cast<SwapChain*>(createSwapChain(nativeWindowHandle));
					RENDERER_SET_RESOURCE_DEBUG_NAME(mMainSwapChain, "Main swap chain")
					mMainSwapChain->addReference();	// Internal renderer reference

					// By default, set the created main swap chain as the currently used render target
					omSetRenderTarget(mMainSwapChain);
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
			mShaderLanguageHlsl->release();
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
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
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

	Renderer::IVertexBuffer *Direct3D10Renderer::createVertexBuffer(uint32_t numberOfBytes, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new VertexBuffer(*this, numberOfBytes, data, bufferUsage);
	}

	Renderer::IIndexBuffer *Direct3D10Renderer::createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new IndexBuffer(*this, numberOfBytes, indexBufferFormat, data, bufferUsage);
	}

	Renderer::IVertexArray *Direct3D10Renderer::createVertexArray(const Renderer::VertexAttributes&, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, Renderer::IIndexBuffer *indexBuffer)
	{
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		return new VertexArray(*this, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
	}

	Renderer::ITextureBuffer *Direct3D10Renderer::createTextureBuffer(uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new TextureBuffer(*this, numberOfBytes, textureFormat, data, bufferUsage);
	}

	Renderer::ITexture2D *Direct3D10Renderer::createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags, Renderer::TextureUsage::Enum textureUsage)
	{
		return new Texture2D(*this, width, height, textureFormat, data, flags, textureUsage);
	}

	Renderer::ITexture2DArray *Direct3D10Renderer::createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags, Renderer::TextureUsage::Enum textureUsage)
	{
		return new Texture2DArray(*this, width, height, numberOfSlices, textureFormat, data, flags, textureUsage);
	}

	Renderer::IRootSignature *Direct3D10Renderer::createRootSignature(const Renderer::RootSignature &rootSignature)
	{
		return new RootSignature(*this, rootSignature);
	}

	Renderer::IPipelineState *Direct3D10Renderer::createPipelineState(const Renderer::PipelineState &pipelineState)
	{
		return new PipelineState(*this, pipelineState);
	}

	Renderer::IRasterizerState *Direct3D10Renderer::createRasterizerState(const Renderer::RasterizerState &rasterizerState)
	{
		return new RasterizerState(*this, rasterizerState);
	}

	Renderer::IDepthStencilState *Direct3D10Renderer::createDepthStencilState(const Renderer::DepthStencilState &depthStencilState)
	{
		return new DepthStencilState(*this, depthStencilState);
	}

	Renderer::IBlendState *Direct3D10Renderer::createBlendState(const Renderer::BlendState &blendState)
	{
		return new BlendState(*this, blendState);
	}

	Renderer::ISamplerState *Direct3D10Renderer::createSamplerState(const Renderer::SamplerState &samplerState)
	{
		return new SamplerState(*this, samplerState);
	}

	Renderer::ITextureCollection *Direct3D10Renderer::createTextureCollection(uint32_t numberOfTextures, Renderer::ITexture **textures)
	{
		return new TextureCollection(*this, numberOfTextures, textures);
	}

	Renderer::ISamplerStateCollection *Direct3D10Renderer::createSamplerStateCollection(uint32_t numberOfSamplerStates, Renderer::ISamplerState **samplerStates)
	{
		return new SamplerStateCollection(*this, numberOfSamplerStates, samplerStates);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool Direct3D10Renderer::map(Renderer::IResource &resource, uint32_t subresource, Renderer::MapType::Enum mapType, uint32_t mapFlags, Renderer::MappedSubresource &mappedSubresource)
	{
		// The "Renderer::MapType::Enum" values directly map to Direct3D 10 & 11 constants, do not change them
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
			case Renderer::ResourceType::TEXTURE_COLLECTION:
			case Renderer::ResourceType::SAMPLER_STATE_COLLECTION:
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
			case Renderer::ResourceType::TEXTURE_COLLECTION:
			case Renderer::ResourceType::SAMPLER_STATE_COLLECTION:
			default:
				// Nothing we can unmap
				break;
		}
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void Direct3D10Renderer::setGraphicsRootSignature(Renderer::IRootSignature *)
	{
		// TODO(co) Implement me
	}

	void Direct3D10Renderer::setGraphicsRootDescriptorTable(uint32_t, Renderer::IResource* resource)
	{
		if (nullptr != resource)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *resource)

			switch (resource->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
				{
					// TODO(co) Test
					fsSetTexture(0, static_cast<Renderer::ITexture*>(resource));
					break;
				}

				case Renderer::ResourceType::SAMPLER_STATE:
				{
					fsSetSamplerState(0, static_cast<Renderer::ISamplerState*>(resource));
					break;
				}
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

			// Evaluate the internal program type of the new program to set
			switch (static_cast<Program*>(program)->getInternalResourceType())
			{
				case Program::InternalResourceType::HLSL:
				{
					// Get shaders
					const ProgramHlsl		 *programHlsl		 = static_cast<ProgramHlsl*>(program);
					const VertexShaderHlsl	 *vertexShaderHlsl	 = programHlsl->getVertexShaderHlsl();
					const GeometryShaderHlsl *geometryShaderHlsl = programHlsl->getGeometryShaderHlsl();
					const FragmentShaderHlsl *fragmentShaderHlsl = programHlsl->getFragmentShaderHlsl();

					// Set shaders
					mD3D10Device->VSSetShader(vertexShaderHlsl	 ? vertexShaderHlsl->  getD3D10VertexShader()	: nullptr);
					mD3D10Device->GSSetShader(geometryShaderHlsl ? geometryShaderHlsl->getD3D10GeometryShader() : nullptr);
					mD3D10Device->PSSetShader(fragmentShaderHlsl ? fragmentShaderHlsl->getD3D10PixelShader()	: nullptr);
					break;
				}
			}
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

	void Direct3D10Renderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology::Enum primitiveTopology)
	{
		// Set primitive topology
		// -> The "Renderer::PrimitiveTopology::Enum" values directly map to Direct3D 9 & 10 & 11 constants, do not change them
		mD3D10Device->IASetPrimitiveTopology(static_cast<D3D10_PRIMITIVE_TOPOLOGY>(primitiveTopology));
	}


	//[-------------------------------------------------------]
	//[ Vertex-shader (VS) stage                              ]
	//[-------------------------------------------------------]
	void Direct3D10Renderer::vsSetTexture(uint32_t unit, Renderer::ITexture *texture)
	{
		// Set a texture at that unit?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Evaluate the texture
			switch (texture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_BUFFER:
				{
					// Direct3D 10 needs a pointer to a pointer, so give it one
					ID3D10ShaderResourceView *d3d10ShaderResourceView = static_cast<TextureBuffer*>(texture)->getD3D10ShaderResourceView();
					mD3D10Device->VSSetShaderResources(unit, 1, &d3d10ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Direct3D 10 needs a pointer to a pointer, so give it one
					ID3D10ShaderResourceView *d3d10ShaderResourceView = static_cast<Texture2D*>(texture)->getD3D10ShaderResourceView();
					mD3D10Device->VSSetShaderResources(unit, 1, &d3d10ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					// Direct3D 10 needs a pointer to a pointer, so give it one
					ID3D10ShaderResourceView *d3d10ShaderResourceView = static_cast<Texture2DArray*>(texture)->getD3D10ShaderResourceView();
					mD3D10Device->VSSetShaderResources(unit, 1, &d3d10ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::RASTERIZER_STATE:
				case Renderer::ResourceType::DEPTH_STENCIL_STATE:
				case Renderer::ResourceType::BLEND_STATE:
				case Renderer::ResourceType::SAMPLER_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
				case Renderer::ResourceType::TEXTURE_COLLECTION:
				case Renderer::ResourceType::SAMPLER_STATE_COLLECTION:
				default:
					// Not handled in here
					break;
			}
		}
		else
		{
			// Direct3D 10 needs a pointer to a pointer, so give it one
			ID3D10ShaderResourceView *d3d10ShaderResourceView = nullptr;
			mD3D10Device->VSSetShaderResources(unit, 1, &d3d10ShaderResourceView);
		}
	}

	void Direct3D10Renderer::vsSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 10 resource views
			TextureCollection *direct3D10TextureCollection = static_cast<TextureCollection*>(textureCollection);
			mD3D10Device->VSSetShaderResources(startUnit, direct3D10TextureCollection->getNumberOfD3D10ShaderResourceViews(), direct3D10TextureCollection->getD3D10ShaderResourceViews());
		}
	}

	void Direct3D10Renderer::vsSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState)
	{
		// Set a sampler state at that unit?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Direct3D 10 needs a pointer to a pointer, so give it one
			ID3D10SamplerState *d3d10SamplerState = static_cast<SamplerState*>(samplerState)->getD3D10SamplerState();
			mD3D10Device->VSSetSamplers(unit, 1, &d3d10SamplerState);
		}
		else
		{
			// Set the default sampler state
			// -> Direct3D 10 needs a pointer to a pointer, so give it one
			// -> The default values of "Renderer::SamplerState" are identical to Direct3D 10
			ID3D10SamplerState *d3d10SamplerState = nullptr;
			mD3D10Device->VSSetSamplers(unit, 1, &d3d10SamplerState);
		}
	}

	void Direct3D10Renderer::vsSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 10 sampler states
			SamplerStateCollection *direct3D10SamplerStateCollection = static_cast<SamplerStateCollection*>(samplerStateCollection);
			mD3D10Device->VSSetSamplers(startUnit, direct3D10SamplerStateCollection->getNumberOfD3D10SamplerStates(), direct3D10SamplerStateCollection->getD3D10SamplerStates());
		}
	}

	void Direct3D10Renderer::vsSetUniformBuffer(uint32_t slot, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Is the given uniform buffer valid?
		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}

		// Direct3D 10 needs a pointer to a pointer, so give it one
		ID3D10Buffer *d3d10Buffers = (nullptr != uniformBuffer) ? static_cast<UniformBuffer*>(uniformBuffer)->getD3D10Buffer() : nullptr;
		mD3D10Device->VSSetConstantBuffers(slot, 1, &d3d10Buffers);
	}


	//[-------------------------------------------------------]
	//[ Tessellation-control-shader (TCS) stage               ]
	//[-------------------------------------------------------]
	void Direct3D10Renderer::tcsSetTexture(uint32_t, Renderer::ITexture *texture)
	{
		// Direct3D 10 has no tessellation control shader support ("hull shader" in Direct3D terminology)

		// Is the given texture valid?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)
		}
	}

	void Direct3D10Renderer::tcsSetTextureCollection(uint32_t, Renderer::ITextureCollection *textureCollection)
	{
		// Direct3D 10 has no tessellation control shader support ("hull shader" in Direct3D terminology)

		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)
		}
	}

	void Direct3D10Renderer::tcsSetSamplerState(uint32_t, Renderer::ISamplerState *samplerState)
	{
		// Direct3D 10 has no tessellation control shader support ("hull shader" in Direct3D terminology)

		// Is the given sampler state valid?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)
		}
	}

	void Direct3D10Renderer::tcsSetSamplerStateCollection(uint32_t, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Direct3D 10 has no tessellation control shader support ("hull shader" in Direct3D terminology)

		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)
		}
	}

	void Direct3D10Renderer::tcsSetUniformBuffer(uint32_t, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Direct3D 10 has no tessellation control shader support ("hull shader" in Direct3D terminology)

		// Is the given uniform buffer valid?
		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}
	}


	//[-------------------------------------------------------]
	//[ Tessellation-evaluation-shader (TES) stage            ]
	//[-------------------------------------------------------]
	void Direct3D10Renderer::tesSetTexture(uint32_t, Renderer::ITexture *texture)
	{
		// Direct3D 10 has no tessellation evaluation shader support ("domain shader" in Direct3D terminology)

		// Is the given texture valid?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)
		}
	}

	void Direct3D10Renderer::tesSetTextureCollection(uint32_t, Renderer::ITextureCollection *textureCollection)
	{
		// Direct3D 10 has no tessellation evaluation shader support ("domain shader" in Direct3D terminology)

		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)
		}
	}

	void Direct3D10Renderer::tesSetSamplerState(uint32_t, Renderer::ISamplerState *samplerState)
	{
		// Direct3D 10 has no tessellation evaluation shader support ("domain shader" in Direct3D terminology)

		// Is the given sampler state valid?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)
		}
	}

	void Direct3D10Renderer::tesSetSamplerStateCollection(uint32_t, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Direct3D 10 has no tessellation evaluation shader support ("domain shader" in Direct3D terminology)

		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)
		}
	}

	void Direct3D10Renderer::tesSetUniformBuffer(uint32_t, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Direct3D 10 has no tessellation evaluation shader support ("domain shader" in Direct3D terminology)

		// Is the given uniform buffer valid?
		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}
	}


	//[-------------------------------------------------------]
	//[ Geometry-shader (GS) stage                            ]
	//[-------------------------------------------------------]
	void Direct3D10Renderer::gsSetTexture(uint32_t unit, Renderer::ITexture *texture)
	{
		// Set a texture at that unit?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Evaluate the texture
			switch (texture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_BUFFER:
				{
					// Direct3D 10 needs a pointer to a pointer, so give it one
					ID3D10ShaderResourceView *d3d10ShaderResourceView = static_cast<TextureBuffer*>(texture)->getD3D10ShaderResourceView();
					mD3D10Device->GSSetShaderResources(unit, 1, &d3d10ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Direct3D 10 needs a pointer to a pointer, so give it one
					ID3D10ShaderResourceView *d3d10ShaderResourceView = static_cast<Texture2D*>(texture)->getD3D10ShaderResourceView();
					mD3D10Device->GSSetShaderResources(unit, 1, &d3d10ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					// Direct3D 10 needs a pointer to a pointer, so give it one
					ID3D10ShaderResourceView *d3d10ShaderResourceView = static_cast<Texture2DArray*>(texture)->getD3D10ShaderResourceView();
					mD3D10Device->GSSetShaderResources(unit, 1, &d3d10ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::RASTERIZER_STATE:
				case Renderer::ResourceType::DEPTH_STENCIL_STATE:
				case Renderer::ResourceType::BLEND_STATE:
				case Renderer::ResourceType::SAMPLER_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
				case Renderer::ResourceType::TEXTURE_COLLECTION:
				case Renderer::ResourceType::SAMPLER_STATE_COLLECTION:
				default:
					// Not handled in here
					break;
			}
		}
		else
		{
			// Direct3D 10 needs a pointer to a pointer, so give it one
			ID3D10ShaderResourceView *d3d10ShaderResourceView = nullptr;
			mD3D10Device->GSSetShaderResources(unit, 1, &d3d10ShaderResourceView);
		}
	}

	void Direct3D10Renderer::gsSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 10 resource views
			TextureCollection *direct3D10TextureCollection = static_cast<TextureCollection*>(textureCollection);
			mD3D10Device->GSSetShaderResources(startUnit, direct3D10TextureCollection->getNumberOfD3D10ShaderResourceViews(), direct3D10TextureCollection->getD3D10ShaderResourceViews());
		}
	}

	void Direct3D10Renderer::gsSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState)
	{
		// Set a sampler state at that unit?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Direct3D 10 needs a pointer to a pointer, so give it one
			ID3D10SamplerState *d3d10SamplerState = static_cast<SamplerState*>(samplerState)->getD3D10SamplerState();
			mD3D10Device->GSSetSamplers(unit, 1, &d3d10SamplerState);
		}
		else
		{
			// Set the default sampler state
			// -> Direct3D 10 needs a pointer to a pointer, so give it one
			// -> The default values of "Renderer::SamplerState" are identical to Direct3D 10
			ID3D10SamplerState *d3d10SamplerState = nullptr;
			mD3D10Device->GSSetSamplers(unit, 1, &d3d10SamplerState);
		}
	}

	void Direct3D10Renderer::gsSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 10 sampler states
			SamplerStateCollection *direct3D10SamplerStateCollection = static_cast<SamplerStateCollection*>(samplerStateCollection);
			mD3D10Device->GSSetSamplers(startUnit, direct3D10SamplerStateCollection->getNumberOfD3D10SamplerStates(), direct3D10SamplerStateCollection->getD3D10SamplerStates());
		}
	}

	void Direct3D10Renderer::gsSetUniformBuffer(uint32_t slot, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Is the given uniform buffer valid?
		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}

		// Direct3D 10 needs a pointer to a pointer, so give it one
		ID3D10Buffer *d3d10Buffers = (nullptr != uniformBuffer) ? static_cast<UniformBuffer*>(uniformBuffer)->getD3D10Buffer() : nullptr;
		mD3D10Device->GSSetConstantBuffers(slot, 1, &d3d10Buffers);
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

	void Direct3D10Renderer::rsSetState(Renderer::IRasterizerState *rasterizerState)
	{
		if (nullptr != rasterizerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *rasterizerState)

			// Set the Direct3D 10 rasterizer state
			mD3D10Device->RSSetState(static_cast<RasterizerState*>(rasterizerState)->getD3D10RasterizerState());
		}
		else
		{
			// Set the default rasterizer state
			// -> The default values of "Renderer::RasterizerState" are identical to Direct3D 10
			// -> The Direct3D documentation does not tell what happens when "ID3D10Device::RSSetState()" is called with a null pointer
			//    -> When looking at the samples within "Microsoft DirectX SDK (June 2010)", I assume this sets the default values
			mD3D10Device->RSSetState(nullptr);
		}
	}


	//[-------------------------------------------------------]
	//[ Fragment-shader (FS) stage                            ]
	//[-------------------------------------------------------]
	void Direct3D10Renderer::fsSetTexture(uint32_t unit, Renderer::ITexture *texture)
	{
		// "pixel shader" in Direct3D terminology

		// Set a texture at that unit?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Evaluate the texture
			switch (texture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_BUFFER:
				{
					// Direct3D 10 needs a pointer to a pointer, so give it one
					ID3D10ShaderResourceView *d3d10ShaderResourceView = static_cast<TextureBuffer*>(texture)->getD3D10ShaderResourceView();
					mD3D10Device->PSSetShaderResources(unit, 1, &d3d10ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Direct3D 10 needs a pointer to a pointer, so give it one
					ID3D10ShaderResourceView *d3d10ShaderResourceView = static_cast<Texture2D*>(texture)->getD3D10ShaderResourceView();
					mD3D10Device->PSSetShaderResources(unit, 1, &d3d10ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					// Direct3D 10 needs a pointer to a pointer, so give it one
					ID3D10ShaderResourceView *d3d10ShaderResourceView = static_cast<Texture2DArray*>(texture)->getD3D10ShaderResourceView();
					mD3D10Device->PSSetShaderResources(unit, 1, &d3d10ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::RASTERIZER_STATE:
				case Renderer::ResourceType::DEPTH_STENCIL_STATE:
				case Renderer::ResourceType::BLEND_STATE:
				case Renderer::ResourceType::SAMPLER_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
				case Renderer::ResourceType::TEXTURE_COLLECTION:
				case Renderer::ResourceType::SAMPLER_STATE_COLLECTION:
				default:
					// Not handled in here
					break;
			}
		}
		else
		{
			// Direct3D 10 needs a pointer to a pointer, so give it one
			ID3D10ShaderResourceView *d3d10ShaderResourceView = nullptr;
			mD3D10Device->PSSetShaderResources(unit, 1, &d3d10ShaderResourceView);
		}
	}

	void Direct3D10Renderer::fsSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 10 resource views
			TextureCollection *direct3D10TextureCollection = static_cast<TextureCollection*>(textureCollection);
			mD3D10Device->PSSetShaderResources(startUnit, direct3D10TextureCollection->getNumberOfD3D10ShaderResourceViews(), direct3D10TextureCollection->getD3D10ShaderResourceViews());
		}
	}

	void Direct3D10Renderer::fsSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState)
	{
		// "pixel shader" in Direct3D terminology

		// Set a sampler state at that unit?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Direct3D 10 needs a pointer to a pointer, so give it one
			ID3D10SamplerState *d3d10SamplerState = static_cast<SamplerState*>(samplerState)->getD3D10SamplerState();
			mD3D10Device->PSSetSamplers(unit, 1, &d3d10SamplerState);
		}
		else
		{
			// Set the default sampler state
			// -> Direct3D 10 needs a pointer to a pointer, so give it one
			// -> The default values of "Renderer::SamplerState" are identical to Direct3D 10
			ID3D10SamplerState *d3d10SamplerState = nullptr;
			mD3D10Device->PSSetSamplers(unit, 1, &d3d10SamplerState);
		}
	}

	void Direct3D10Renderer::fsSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 10 sampler states
			SamplerStateCollection *direct3D10SamplerStateCollection = static_cast<SamplerStateCollection*>(samplerStateCollection);
			mD3D10Device->PSSetSamplers(startUnit, direct3D10SamplerStateCollection->getNumberOfD3D10SamplerStates(), direct3D10SamplerStateCollection->getD3D10SamplerStates());
		}
	}

	void Direct3D10Renderer::fsSetUniformBuffer(uint32_t slot, Renderer::IUniformBuffer *uniformBuffer)
	{
		// "pixel shader" in Direct3D terminology

		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}

		// Direct3D 10 needs a pointer to a pointer, so give it one
		ID3D10Buffer *d3d10Buffers = (nullptr != uniformBuffer) ? static_cast<UniformBuffer*>(uniformBuffer)->getD3D10Buffer() : nullptr;
		mD3D10Device->PSSetConstantBuffers(slot, 1, &d3d10Buffers);
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	Renderer::IRenderTarget *Direct3D10Renderer::omGetRenderTarget()
	{
		return mRenderTarget;
	}

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
						mD3D10Device->OMSetRenderTargets(framebuffer->getNumberOfD3D10RenderTargetViews(), framebuffer->getD3D10RenderTargetViews(), framebuffer->getD3D10DepthStencilView());
						break;
					}

					case Renderer::ResourceType::PROGRAM:
					case Renderer::ResourceType::VERTEX_ARRAY:
					case Renderer::ResourceType::INDEX_BUFFER:
					case Renderer::ResourceType::VERTEX_BUFFER:
					case Renderer::ResourceType::UNIFORM_BUFFER:
					case Renderer::ResourceType::TEXTURE_BUFFER:
					case Renderer::ResourceType::TEXTURE_2D:
					case Renderer::ResourceType::TEXTURE_2D_ARRAY:
					case Renderer::ResourceType::RASTERIZER_STATE:
					case Renderer::ResourceType::DEPTH_STENCIL_STATE:
					case Renderer::ResourceType::BLEND_STATE:
					case Renderer::ResourceType::SAMPLER_STATE:
					case Renderer::ResourceType::VERTEX_SHADER:
					case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
					case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
					case Renderer::ResourceType::GEOMETRY_SHADER:
					case Renderer::ResourceType::FRAGMENT_SHADER:
					case Renderer::ResourceType::TEXTURE_COLLECTION:
					case Renderer::ResourceType::SAMPLER_STATE_COLLECTION:
					default:
						// Not handled in here
						break;
				}
			}
			else
			{
				// Set the Direct3D 10 render targets
				mD3D10Device->OMSetRenderTargets(0, nullptr, nullptr);

				// Release the render target reference, in case we have one
				if (nullptr != mRenderTarget)
				{
					mRenderTarget->release();
					mRenderTarget = nullptr;
				}
			}
		}
	}

	void Direct3D10Renderer::omSetDepthStencilState(Renderer::IDepthStencilState *depthStencilState)
	{
		if (nullptr != depthStencilState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *depthStencilState)

			// Set Direct3D 10 depth stencil state
			mD3D10Device->OMSetDepthStencilState(static_cast<DepthStencilState*>(depthStencilState)->getD3D10DepthStencilState(), 0);
		}
		else
		{
			// Set the default depth stencil state
			// -> The default values of "Renderer::DepthStencilState" are identical to Direct3D 10
			// -> The Direct3D documentation does not tell what happens when "ID3D10Device::OMSetDepthStencilState()" is called with a null pointer
			//    -> Within "Microsoft DirectX SDK (June 2010)" no samples are using this case, I assume this sets the default values (as in Direct3D 11 were it's documented)
			mD3D10Device->OMSetDepthStencilState(nullptr, 0);
		}
	}

	void Direct3D10Renderer::omSetBlendState(Renderer::IBlendState *blendState)
	{
		if (nullptr != blendState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D10RENDERER_RENDERERMATCHCHECK_RETURN(*this, *blendState)

			// Set Direct3D 10 blend state
			mD3D10Device->OMSetBlendState(static_cast<BlendState*>(blendState)->getD3D10BlendState(), 0, 0xffffffff);
		}
		else
		{
			// Set the default blend state
			// -> The default values of "Renderer::BlendState" are identical to Direct3D 10
			mD3D10Device->OMSetBlendState(nullptr, 0, 0xffffffff);
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
						ID3D10RenderTargetView **d3d10RenderTargetViewsEnd = framebuffer->getD3D10RenderTargetViews() + framebuffer->getNumberOfD3D10RenderTargetViews();
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

				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::TEXTURE_BUFFER:
				case Renderer::ResourceType::TEXTURE_2D:
				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				case Renderer::ResourceType::RASTERIZER_STATE:
				case Renderer::ResourceType::DEPTH_STENCIL_STATE:
				case Renderer::ResourceType::BLEND_STATE:
				case Renderer::ResourceType::SAMPLER_STATE:
				case Renderer::ResourceType::VERTEX_SHADER:
				case Renderer::ResourceType::TESSELLATION_CONTROL_SHADER:
				case Renderer::ResourceType::TESSELLATION_EVALUATION_SHADER:
				case Renderer::ResourceType::GEOMETRY_SHADER:
				case Renderer::ResourceType::FRAGMENT_SHADER:
				case Renderer::ResourceType::TEXTURE_COLLECTION:
				case Renderer::ResourceType::SAMPLER_STATE_COLLECTION:
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

	bool Direct3D10Renderer::beginScene()
	{
		// Not required when using Direct3D 10

		// Done
		return true;
	}

	void Direct3D10Renderer::endScene()
	{
		// Not required when using Direct3D 10
	}


	//[-------------------------------------------------------]
	//[ Draw call                                             ]
	//[-------------------------------------------------------]
	void Direct3D10Renderer::draw(uint32_t startVertexLocation, uint32_t numberOfVertices)
	{
		mD3D10Device->Draw(
			numberOfVertices,	// Vertex count (UINT)
			startVertexLocation	// Start index location (UINT)
		);
	}

	void Direct3D10Renderer::drawInstanced(uint32_t startVertexLocation, uint32_t numberOfVertices, uint32_t numberOfInstances)
	{
		mD3D10Device->DrawInstanced(
			numberOfVertices,		// Vertex count per instance (UINT)
			numberOfInstances,		// Instance count (UINT)
			startVertexLocation,	// Start vertex location (UINT)
			0						// Start instance location (UINT)
		);
	}

	void Direct3D10Renderer::drawIndexed(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t, uint32_t)
	{
		// "minimumIndex" & "numberOfVertices" are not supported by Direct3D 10

		// Draw
		mD3D10Device->DrawIndexed(
			numberOfIndices,						// Index count (UINT)
			startIndexLocation,						// Start index location (UINT)
			static_cast<INT>(baseVertexLocation)	// Base vertex location (INT)
		);
	}

	void Direct3D10Renderer::drawIndexedInstanced(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t, uint32_t, uint32_t numberOfInstances)
	{
		// "minimumIndex" & "numberOfVertices" are not supported by Direct3D 10

		// Draw
		mD3D10Device->DrawIndexedInstanced(
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
	//[ Debug                                                 ]
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

	void Direct3D10Renderer::setDebugMarker(const wchar_t *name)
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
				D3DPERF_SetMarker(D3DCOLOR_RGBA(255, 0, 255, 255), name);
			}
		#endif
	}

	void Direct3D10Renderer::beginDebugEvent(const wchar_t *name)
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
				D3DPERF_BeginEvent(D3DCOLOR_RGBA(255, 255, 255, 255), name);
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

		// Uniform buffer object (UBO, "constant buffer" in Direct3D terminology) supported?
		mCapabilities.uniformBuffer = true;

		// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
		mCapabilities.maximumTextureBufferSize = 2000;	// TODO(co) http://msdn.microsoft.com/en-us/library/cc308052%28VS.85%29.aspx does not mention the texture buffer? Figure out the correct size!

		// Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		mCapabilities.individualUniforms = false;

		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
		mCapabilities.instancedArrays = true;

		// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID)
		mCapabilities.drawInstanced = true;

		// Base vertex supported for draw calls?
		mCapabilities.baseVertex = true;

		// Is there support for vertex shaders (VS)?
		mCapabilities.vertexShader = true;

		// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
		mCapabilities.maximumNumberOfPatchVertices = 0;	// Direct3D 10 has no tessellation support

		// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
		mCapabilities.maximumNumberOfGsOutputVertices = 1024;

		// Is there support for fragment shaders (FS)?
		mCapabilities.fragmentShader = true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
