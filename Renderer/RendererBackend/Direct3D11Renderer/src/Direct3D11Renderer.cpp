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
#include "Direct3D11Renderer/Direct3D11Renderer.h"
#include "Direct3D11Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D11Renderer/Direct3D11Debug.h"	// For "DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "Direct3D11Renderer/Direct3D9RuntimeLinking.h"	//  For the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box) used for debugging
#include "Direct3D11Renderer/Direct3D11RuntimeLinking.h"
#include "Direct3D11Renderer/ProgramHlsl.h"
#include "Direct3D11Renderer/ShaderLanguageHlsl.h"
#include "Direct3D11Renderer/Texture2D.h"
#include "Direct3D11Renderer/BlendState.h"
#include "Direct3D11Renderer/VertexArray.h"
#include "Direct3D11Renderer/SwapChain.h"
#include "Direct3D11Renderer/Framebuffer.h"
#include "Direct3D11Renderer/IndexBuffer.h"
#include "Direct3D11Renderer/SamplerState.h"
#include "Direct3D11Renderer/VertexBuffer.h"
#include "Direct3D11Renderer/UniformBuffer.h"
#include "Direct3D11Renderer/TextureBuffer.h"
#include "Direct3D11Renderer/Texture2DArray.h"
#include "Direct3D11Renderer/RasterizerState.h"
#include "Direct3D11Renderer/VertexShaderHlsl.h"
#include "Direct3D11Renderer/DepthStencilState.h"
#include "Direct3D11Renderer/TextureCollection.h"
#include "Direct3D11Renderer/SamplerStateCollection.h"
#include "Direct3D11Renderer/GeometryShaderHlsl.h"
#include "Direct3D11Renderer/FragmentShaderHlsl.h"
#include "Direct3D11Renderer/TessellationControlShaderHlsl.h"
#include "Direct3D11Renderer/TessellationEvaluationShaderHlsl.h"


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
		mRenderTarget(nullptr)
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
				for (UINT i = 0; i < NUMBER_OF_DRIVER_TYPES; ++i)
				{
					D3D_FEATURE_LEVEL d3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;
					if (SUCCEEDED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPES[i], nullptr, flags, D3D_FEATURE_LEVELS, NUMBER_OF_FEATURE_LEVELS, D3D11_SDK_VERSION, &mD3D11Device, &d3dFeatureLevel, &mD3D11DeviceContext)))
					{
						// Done
						i = NUMBER_OF_DRIVER_TYPES;
					}
				}
			}

			// Is there a valid Direct3D 11 device and device contect?
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
					mMainSwapChain = static_cast<SwapChain*>(createSwapChain(nativeWindowHandle));
					RENDERER_SET_RESOURCE_DEBUG_NAME(mMainSwapChain, "Main swap chain")
					mMainSwapChain->addReference();	// Internal renderer reference

					// By default, set the created main swap chain as the currently used render target
					omSetRenderTarget(mMainSwapChain);
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

	Renderer::IVertexBuffer *Direct3D11Renderer::createVertexBuffer(uint32_t numberOfBytes, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new VertexBuffer(*this, numberOfBytes, data, bufferUsage);
	}

	Renderer::IIndexBuffer *Direct3D11Renderer::createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new IndexBuffer(*this, numberOfBytes, indexBufferFormat, data, bufferUsage);
	}

	Renderer::ITextureBuffer *Direct3D11Renderer::createTextureBuffer(uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new TextureBuffer(*this, numberOfBytes, textureFormat, data, bufferUsage);
	}

	Renderer::ITexture2D *Direct3D11Renderer::createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags, Renderer::TextureUsage::Enum textureUsage)
	{
		return new Texture2D(*this, width, height, textureFormat, data, flags, textureUsage);
	}

	Renderer::ITexture2DArray *Direct3D11Renderer::createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags, Renderer::TextureUsage::Enum textureUsage)
	{
		return new Texture2DArray(*this, width, height, numberOfSlices, textureFormat, data, flags, textureUsage);
	}

	Renderer::IRasterizerState *Direct3D11Renderer::createRasterizerState(const Renderer::RasterizerState &rasterizerState)
	{
		return new RasterizerState(*this, rasterizerState);
	}

	Renderer::IDepthStencilState *Direct3D11Renderer::createDepthStencilState(const Renderer::DepthStencilState &depthStencilState)
	{
		return new DepthStencilState(*this, depthStencilState);
	}

	Renderer::IBlendState *Direct3D11Renderer::createBlendState(const Renderer::BlendState &blendState)
	{
		return new BlendState(*this, blendState);
	}

	Renderer::ISamplerState *Direct3D11Renderer::createSamplerState(const Renderer::SamplerState &samplerState)
	{
		return new SamplerState(*this, samplerState);
	}

	Renderer::ITextureCollection *Direct3D11Renderer::createTextureCollection(uint32_t numberOfTextures, Renderer::ITexture **textures)
	{
		return new TextureCollection(*this, numberOfTextures, textures);
	}

	Renderer::ISamplerStateCollection *Direct3D11Renderer::createSamplerStateCollection(uint32_t numberOfSamplerStates, Renderer::ISamplerState **samplerStates)
	{
		return new SamplerStateCollection(*this, numberOfSamplerStates, samplerStates);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool Direct3D11Renderer::map(Renderer::IResource &resource, uint32_t subresource, Renderer::MapType::Enum mapType, uint32_t mapFlags, Renderer::MappedSubresource &mappedSubresource)
	{
		// The "Renderer::MapType::Enum" values directly map to Direct3D 10 & 11 constants, do not change them
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

	void Direct3D11Renderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology::Enum primitiveTopology)
	{
		// Set primitive topology
		// -> The "Renderer::PrimitiveTopology::Enum" values directly map to Direct3D 9 & 10 & 11 constants, do not change them
		mD3D11DeviceContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(primitiveTopology));
	}


	//[-------------------------------------------------------]
	//[ Vertex-shader (VS) stage                              ]
	//[-------------------------------------------------------]
	void Direct3D11Renderer::vsSetTexture(uint32_t unit, Renderer::ITexture *texture)
	{
		// Set a texture at that unit?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Evaluate the texture
			switch (texture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_BUFFER:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<TextureBuffer*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->VSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<Texture2D*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->VSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<Texture2DArray*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->VSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
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
			// Direct3D 11 needs a pointer to a pointer, so give it one
			ID3D11ShaderResourceView *d3d11ShaderResourceView = nullptr;
			mD3D11DeviceContext->VSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
		}
	}

	void Direct3D11Renderer::vsSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 11 resource views
			TextureCollection *direct3D11TextureCollection = static_cast<TextureCollection*>(textureCollection);
			mD3D11DeviceContext->VSSetShaderResources(startUnit, direct3D11TextureCollection->getNumberOfD3D11ShaderResourceViews(), direct3D11TextureCollection->getD3D11ShaderResourceViews());
		}
	}

	void Direct3D11Renderer::vsSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState)
	{
		// Set a sampler state at that unit?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Direct3D 11 needs a pointer to a pointer, so give it one
			ID3D11SamplerState *d3d11SamplerState = static_cast<SamplerState*>(samplerState)->getD3D11SamplerState();
			mD3D11DeviceContext->VSSetSamplers(unit, 1, &d3d11SamplerState);
		}
		else
		{
			// Set the default sampler state
			// -> Direct3D 10 needs a pointer to a pointer, so give it one
			// -> The default values of "Renderer::SamplerState" are identical to Direct3D 10
			ID3D11SamplerState *d3d11SamplerState = nullptr;
			mD3D11DeviceContext->VSSetSamplers(unit, 1, &d3d11SamplerState);
		}
	}

	void Direct3D11Renderer::vsSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 11 sampler states
			SamplerStateCollection *direct3D11SamplerStateCollection = static_cast<SamplerStateCollection*>(samplerStateCollection);
			mD3D11DeviceContext->VSSetSamplers(startUnit, direct3D11SamplerStateCollection->getNumberOfD3D11SamplerStates(), direct3D11SamplerStateCollection->getD3D11SamplerStates());
		}
	}

	void Direct3D11Renderer::vsSetUniformBuffer(uint32_t slot, Renderer::IUniformBuffer *uniformBuffer)
	{
		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}

		// Direct3D 11 needs a pointer to a pointer, so give it one
		ID3D11Buffer *d3d11Buffers = (nullptr != uniformBuffer) ? static_cast<UniformBuffer*>(uniformBuffer)->getD3D11Buffer() : nullptr;
		mD3D11DeviceContext->VSSetConstantBuffers(slot, 1, &d3d11Buffers);
	}


	//[-------------------------------------------------------]
	//[ Tessellation-control-shader (TCS) stage               ]
	//[-------------------------------------------------------]
	void Direct3D11Renderer::tcsSetTexture(uint32_t unit, Renderer::ITexture *texture)
	{
		// "hull shader" in Direct3D terminology

		// Set a texture at that unit?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Evaluate the texture
			switch (texture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_BUFFER:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<TextureBuffer*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->HSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<Texture2D*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->HSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<Texture2DArray*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->HSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
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
			// Direct3D 11 needs a pointer to a pointer, so give it one
			ID3D11ShaderResourceView *d3d11ShaderResourceView = nullptr;
			mD3D11DeviceContext->HSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
		}
	}

	void Direct3D11Renderer::tcsSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// "hull shader" in Direct3D terminology

		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 11 resource views
			TextureCollection *direct3D11TextureCollection = static_cast<TextureCollection*>(textureCollection);
			mD3D11DeviceContext->HSSetShaderResources(startUnit, direct3D11TextureCollection->getNumberOfD3D11ShaderResourceViews(), direct3D11TextureCollection->getD3D11ShaderResourceViews());
		}
	}

	void Direct3D11Renderer::tcsSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState)
	{
		// "hull shader" in Direct3D terminology

		// Set a sampler state at that unit?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Direct3D 11 needs a pointer to a pointer, so give it one
			ID3D11SamplerState *d3d11SamplerState = static_cast<SamplerState*>(samplerState)->getD3D11SamplerState();
			mD3D11DeviceContext->HSSetSamplers(unit, 1, &d3d11SamplerState);
		}
		else
		{
			// Set the default sampler state
			// -> Direct3D 10 needs a pointer to a pointer, so give it one
			// -> The default values of "Renderer::SamplerState" are identical to Direct3D 10
			ID3D11SamplerState *d3d11SamplerState = nullptr;
			mD3D11DeviceContext->HSSetSamplers(unit, 1, &d3d11SamplerState);
		}
	}

	void Direct3D11Renderer::tcsSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// "hull shader" in Direct3D terminology

		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 11 sampler states
			SamplerStateCollection *direct3D11SamplerStateCollection = static_cast<SamplerStateCollection*>(samplerStateCollection);
			mD3D11DeviceContext->HSSetSamplers(startUnit, direct3D11SamplerStateCollection->getNumberOfD3D11SamplerStates(), direct3D11SamplerStateCollection->getD3D11SamplerStates());
		}
	}

	void Direct3D11Renderer::tcsSetUniformBuffer(uint32_t slot, Renderer::IUniformBuffer *uniformBuffer)
	{
		// "hull shader" in Direct3D terminology

		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}

		// Direct3D 11 needs a pointer to a pointer, so give it one
		ID3D11Buffer *d3d11Buffers = (nullptr != uniformBuffer) ? static_cast<UniformBuffer*>(uniformBuffer)->getD3D11Buffer() : nullptr;
		mD3D11DeviceContext->HSSetConstantBuffers(slot, 1, &d3d11Buffers);
	}


	//[-------------------------------------------------------]
	//[ Tessellation-evaluation-shader (TES) stage            ]
	//[-------------------------------------------------------]
	void Direct3D11Renderer::tesSetTexture(uint32_t unit, Renderer::ITexture *texture)
	{
		// "domain shader" in Direct3D terminology

		// Set a texture at that unit?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Evaluate the texture
			switch (texture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_BUFFER:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<TextureBuffer*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->DSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<Texture2D*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->DSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<Texture2DArray*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->DSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
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
			// Direct3D 11 needs a pointer to a pointer, so give it one
			ID3D11ShaderResourceView *d3d11ShaderResourceView = nullptr;
			mD3D11DeviceContext->DSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
		}
	}

	void Direct3D11Renderer::tesSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// "domain shader" in Direct3D terminology

		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 11 resource views
			TextureCollection *direct3D11TextureCollection = static_cast<TextureCollection*>(textureCollection);
			mD3D11DeviceContext->DSSetShaderResources(startUnit, direct3D11TextureCollection->getNumberOfD3D11ShaderResourceViews(), direct3D11TextureCollection->getD3D11ShaderResourceViews());
		}
	}

	void Direct3D11Renderer::tesSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState)
	{
		// "domain shader" in Direct3D terminology

		// Set a sampler state at that unit?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Direct3D 11 needs a pointer to a pointer, so give it one
			ID3D11SamplerState *d3d11SamplerState = static_cast<SamplerState*>(samplerState)->getD3D11SamplerState();
			mD3D11DeviceContext->DSSetSamplers(unit, 1, &d3d11SamplerState);
		}
		else
		{
			// Set the default sampler state
			// -> Direct3D 10 needs a pointer to a pointer, so give it one
			// -> The default values of "Renderer::SamplerState" are identical to Direct3D 10
			ID3D11SamplerState *d3d11SamplerState = nullptr;
			mD3D11DeviceContext->DSSetSamplers(unit, 1, &d3d11SamplerState);
		}
	}

	void Direct3D11Renderer::tesSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// "domain shader" in Direct3D terminology

		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 11 sampler states
			SamplerStateCollection *direct3D11SamplerStateCollection = static_cast<SamplerStateCollection*>(samplerStateCollection);
			mD3D11DeviceContext->DSSetSamplers(startUnit, direct3D11SamplerStateCollection->getNumberOfD3D11SamplerStates(), direct3D11SamplerStateCollection->getD3D11SamplerStates());
		}
	}

	void Direct3D11Renderer::tesSetUniformBuffer(uint32_t slot, Renderer::IUniformBuffer *uniformBuffer)
	{
		// "domain shader" in Direct3D terminology

		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}

		// Direct3D 11 needs a pointer to a pointer, so give it one
		ID3D11Buffer *d3d11Buffers = (nullptr != uniformBuffer) ? static_cast<UniformBuffer*>(uniformBuffer)->getD3D11Buffer() : nullptr;
		mD3D11DeviceContext->DSSetConstantBuffers(slot, 1, &d3d11Buffers);
	}


	//[-------------------------------------------------------]
	//[ Geometry-shader (GS) stage                            ]
	//[-------------------------------------------------------]
	void Direct3D11Renderer::gsSetTexture(uint32_t unit, Renderer::ITexture *texture)
	{
		// Set a texture at that unit?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Evaluate the texture
			switch (texture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_BUFFER:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<TextureBuffer*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->GSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<Texture2D*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->GSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<Texture2DArray*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->GSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
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
			// Direct3D 11 needs a pointer to a pointer, so give it one
			ID3D11ShaderResourceView *d3d11ShaderResourceView = nullptr;
			mD3D11DeviceContext->GSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
		}
	}

	void Direct3D11Renderer::gsSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 11 resource views
			TextureCollection *direct3D11TextureCollection = static_cast<TextureCollection*>(textureCollection);
			mD3D11DeviceContext->GSSetShaderResources(startUnit, direct3D11TextureCollection->getNumberOfD3D11ShaderResourceViews(), direct3D11TextureCollection->getD3D11ShaderResourceViews());
		}
	}

	void Direct3D11Renderer::gsSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState)
	{
		// Set a sampler state at that unit?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Direct3D 11 needs a pointer to a pointer, so give it one
			ID3D11SamplerState *d3d11SamplerState = static_cast<SamplerState*>(samplerState)->getD3D11SamplerState();
			mD3D11DeviceContext->GSSetSamplers(unit, 1, &d3d11SamplerState);
		}
		else
		{
			// Set the default sampler state
			// -> Direct3D 10 needs a pointer to a pointer, so give it one
			// -> The default values of "Renderer::SamplerState" are identical to Direct3D 10
			ID3D11SamplerState *d3d11SamplerState = nullptr;
			mD3D11DeviceContext->GSSetSamplers(unit, 1, &d3d11SamplerState);
		}
	}

	void Direct3D11Renderer::gsSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 11 sampler states
			SamplerStateCollection *direct3D11SamplerStateCollection = static_cast<SamplerStateCollection*>(samplerStateCollection);
			mD3D11DeviceContext->GSSetSamplers(startUnit, direct3D11SamplerStateCollection->getNumberOfD3D11SamplerStates(), direct3D11SamplerStateCollection->getD3D11SamplerStates());
		}
	}

	void Direct3D11Renderer::gsSetUniformBuffer(uint32_t slot, Renderer::IUniformBuffer *uniformBuffer)
	{
		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}

		// Direct3D 11 needs a pointer to a pointer, so give it one
		ID3D11Buffer *d3d11Buffers = (nullptr != uniformBuffer) ? static_cast<UniformBuffer*>(uniformBuffer)->getD3D11Buffer() : nullptr;
		mD3D11DeviceContext->GSSetConstantBuffers(slot, 1, &d3d11Buffers);
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

	void Direct3D11Renderer::rsSetState(Renderer::IRasterizerState *rasterizerState)
	{
		if (nullptr != rasterizerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *rasterizerState)

			// Set the Direct3D 11 rasterizer state
			mD3D11DeviceContext->RSSetState(static_cast<RasterizerState*>(rasterizerState)->getD3D11RasterizerState());
		}
		else
		{
			// Set the default rasterizer state
			// -> The default values of "Renderer::RasterizerState" are identical to Direct3D 11
			// -> The Direct3D documentation does not tell what happens when "ID3D11DeviceContext::RSSetState()" is called with a null pointer
			//    -> When looking at the samples within "Microsoft DirectX SDK (June 2010)", I assume this sets the default values
			mD3D11DeviceContext->RSSetState(nullptr);
		}
	}


	//[-------------------------------------------------------]
	//[ Fragment-shader (FS) stage                            ]
	//[-------------------------------------------------------]
	void Direct3D11Renderer::fsSetTexture(uint32_t unit, Renderer::ITexture *texture)
	{
		// "pixel shader" in Direct3D terminology

		// Set a texture at that unit?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Evaluate the texture
			switch (texture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_BUFFER:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<TextureBuffer*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->PSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<Texture2D*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->PSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
					break;
				}

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
				{
					// Direct3D 11 needs a pointer to a pointer, so give it one
					ID3D11ShaderResourceView *d3d11ShaderResourceView = static_cast<Texture2DArray*>(texture)->getD3D11ShaderResourceView();
					mD3D11DeviceContext->PSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
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
			// Direct3D 11 needs a pointer to a pointer, so give it one
			ID3D11ShaderResourceView *d3d11ShaderResourceView = nullptr;
			mD3D11DeviceContext->PSSetShaderResources(unit, 1, &d3d11ShaderResourceView);
		}
	}

	void Direct3D11Renderer::fsSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 11 resource views
			TextureCollection *direct3D11TextureCollection = static_cast<TextureCollection*>(textureCollection);
			mD3D11DeviceContext->PSSetShaderResources(startUnit, direct3D11TextureCollection->getNumberOfD3D11ShaderResourceViews(), direct3D11TextureCollection->getD3D11ShaderResourceViews());
		}
	}

	void Direct3D11Renderer::fsSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState)
	{
		// "pixel shader" in Direct3D terminology

		// Set a sampler state at that unit?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Direct3D 11 needs a pointer to a pointer, so give it one
			ID3D11SamplerState *d3d11SamplerState = static_cast<SamplerState*>(samplerState)->getD3D11SamplerState();
			mD3D11DeviceContext->PSSetSamplers(unit, 1, &d3d11SamplerState);
		}
		else
		{
			// Set the default sampler state
			// -> Direct3D 10 needs a pointer to a pointer, so give it one
			// -> The default values of "Renderer::SamplerState" are identical to Direct3D 10
			ID3D11SamplerState *d3d11SamplerState = nullptr;
			mD3D11DeviceContext->PSSetSamplers(unit, 1, &d3d11SamplerState);
		}
	}

	void Direct3D11Renderer::fsSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set the Direct3D 11 sampler states
			SamplerStateCollection *direct3D11SamplerStateCollection = static_cast<SamplerStateCollection*>(samplerStateCollection);
			mD3D11DeviceContext->PSSetSamplers(startUnit, direct3D11SamplerStateCollection->getNumberOfD3D11SamplerStates(), direct3D11SamplerStateCollection->getD3D11SamplerStates());
		}
	}

	void Direct3D11Renderer::fsSetUniformBuffer(uint32_t slot, Renderer::IUniformBuffer *uniformBuffer)
	{
		// "pixel shader" in Direct3D terminology

		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}

		// Direct3D 11 needs a pointer to a pointer, so give it one
		ID3D11Buffer *d3d11Buffers = (nullptr != uniformBuffer) ? static_cast<UniformBuffer*>(uniformBuffer)->getD3D11Buffer() : nullptr;
		mD3D11DeviceContext->PSSetConstantBuffers(slot, 1, &d3d11Buffers);
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

	void Direct3D11Renderer::omSetDepthStencilState(Renderer::IDepthStencilState *depthStencilState)
	{
		if (nullptr != depthStencilState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *depthStencilState)

			// Set Direct3D 11 depth stencil state
			mD3D11DeviceContext->OMSetDepthStencilState(static_cast<DepthStencilState*>(depthStencilState)->getD3D11DepthStencilState(), 0);
		}
		else
		{
			// Set the default depth stencil state
			// -> The default values of "Renderer::DepthStencilState" are identical to Direct3D 11
			mD3D11DeviceContext->OMSetDepthStencilState(nullptr, 0);
		}
	}

	void Direct3D11Renderer::omSetBlendState(Renderer::IBlendState *blendState)
	{
		if (nullptr != blendState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D11RENDERER_RENDERERMATCHCHECK_RETURN(*this, *blendState)

			// Set Direct3D 11 blend state
			mD3D11DeviceContext->OMSetBlendState(static_cast<BlendState*>(blendState)->getD3D11BlendState(), 0, 0xffffffff);
		}
		else
		{
			// Set the default blend state
			// -> The default values of "Renderer::BlendState" are identical to Direct3D 11
			mD3D11DeviceContext->OMSetBlendState(nullptr, 0, 0xffffffff);
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

	bool Direct3D11Renderer::beginScene()
	{
		// Not required when using Direct3D 11

		// Done
		return true;
	}

	void Direct3D11Renderer::endScene()
	{
		// Not required when using Direct3D 11
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


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
