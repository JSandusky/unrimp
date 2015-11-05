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
#include "Direct3D9Renderer/Direct3D9Renderer.h"
#include "Direct3D9Renderer/d3d9.h"	// For "DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "Direct3D9Renderer/Direct3D9Debug.h"	// For "DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "Direct3D9Renderer/Direct3D9RuntimeLinking.h"
#include "Direct3D9Renderer/Texture2D.h"
#include "Direct3D9Renderer/BlendState.h"
#include "Direct3D9Renderer/VertexArray.h"
#include "Direct3D9Renderer/SwapChain.h"
#include "Direct3D9Renderer/Framebuffer.h"
#include "Direct3D9Renderer/IndexBuffer.h"
#include "Direct3D9Renderer/SamplerState.h"
#include "Direct3D9Renderer/VertexBuffer.h"
#include "Direct3D9Renderer/PipelineState.h"
#include "Direct3D9Renderer/RasterizerState.h"
#include "Direct3D9Renderer/DepthStencilState.h"
#include "Direct3D9Renderer/TextureCollection.h"
#include "Direct3D9Renderer/SamplerStateCollection.h"
#include "Direct3D9Renderer/Shader/ProgramHlsl.h"
#include "Direct3D9Renderer/Shader/VertexShaderHlsl.h"
#include "Direct3D9Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D9Renderer/Shader/FragmentShaderHlsl.h"


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef DIRECT3D9RENDERER_EXPORTS
	#define DIRECT3D9RENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define DIRECT3D9RENDERER_API_EXPORT
#endif
DIRECT3D9RENDERER_API_EXPORT Renderer::IRenderer *createDirect3D9RendererInstance(handle nativeWindowHandle)
{
	return new Direct3D9Renderer::Direct3D9Renderer(nativeWindowHandle);
}
#undef DIRECT3D9RENDERER_API_EXPORT


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Direct3D9Renderer::Direct3D9Renderer(handle nativeWindowHandle) :
		mDirect3D9RuntimeLinking(new Direct3D9RuntimeLinking()),
		mDirect3D9(nullptr),
		mDirect3DDevice9(nullptr),
		mShaderLanguageHlsl(nullptr),
		mDirect3DQuery9Flush(nullptr),
		mDefaultSamplerState(nullptr),
		mPrimitiveTopology(Renderer::PrimitiveTopology::UNKNOWN),
		mDefaultRasterizerState(nullptr),
		mRasterizerState(nullptr),
		mMainSwapChain(nullptr),
		mRenderTarget(nullptr),
		mDefaultDepthStencilState(nullptr),
		mDepthStencilState(nullptr),
		mDefaultBlendState(nullptr),
		mBlendState(nullptr)
	{
		// Is Direct3D 9 available?
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
				D3DPRESENT_PARAMETERS d3dPresentParameters;
				::ZeroMemory(&d3dPresentParameters, sizeof(D3DPRESENT_PARAMETERS));
				d3dPresentParameters.BackBufferWidth		= 1;
				d3dPresentParameters.BackBufferHeight		= 1;
				d3dPresentParameters.BackBufferCount		= 1;
				d3dPresentParameters.SwapEffect				= D3DSWAPEFFECT_DISCARD;
				d3dPresentParameters.Windowed				= TRUE;
				d3dPresentParameters.EnableAutoDepthStencil = FALSE;

				// Create the Direct3D 9 device instance
				// -> In Direct3D 9, there is always at least one swap chain for each device, known as the implicit swap chain
				// -> The size of the swap chain can be changed by using "IDirect3DDevice9::Reset()"...but this results in a
				//    loss of ALL resources and EVERYTHING has to be rebuild and configured from scatch!
				// -> We really don't want to use the implicit swap chain, so we're creating a tiny one (because we have to!)
				//    and then using "IDirect3DDevice9::CreateAdditionalSwapChain()" later on for the real main swap chain
				mDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL_HANDLE, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dPresentParameters, &mDirect3DDevice9);
				if (nullptr != mDirect3DDevice9)
				{
					#ifdef DIRECT3D9RENDERER_NO_DEBUG
						// Disable debugging
						D3DPERF_SetOptions(1);
					#endif

					// Create the default state objects
					mDefaultSamplerState	  = createSamplerState(Renderer::ISamplerState::getDefaultSamplerState());
					mDefaultRasterizerState	  = createRasterizerState(Renderer::IRasterizerState::getDefaultRasterizerState());
					mDefaultDepthStencilState = createDepthStencilState(Renderer::IDepthStencilState::getDefaultDepthStencilState());
					mDefaultBlendState		  = createBlendState(Renderer::IBlendState::getDefaultBlendState());

					// Initialize the capabilities
					initializeCapabilities();

					// Add references to the default state objects and set them
					if (nullptr != mDefaultRasterizerState)
					{
						mDefaultRasterizerState->addReference();
						rsSetState(mDefaultRasterizerState);
					}
					if (nullptr != mDefaultDepthStencilState)
					{
						mDefaultDepthStencilState->addReference();
						omSetDepthStencilState(mDefaultDepthStencilState);
					}
					if (nullptr != mDefaultBlendState)
					{
						mDefaultBlendState->addReference();
						omSetBlendState(mDefaultBlendState);
					}
					if (nullptr != mDefaultSamplerState)
					{
						mDefaultSamplerState->addReference();
						// TODO(co) Set default sampler states
					}

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
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 9 error: Failed to create device instance")
				}
			}
			else
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 9 error: Failed to create Direct3D 9 instance")
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
		if (nullptr != mDefaultRasterizerState)
		{
			mDefaultRasterizerState->release();
			mDefaultRasterizerState = nullptr;
		}
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
		if (nullptr != mDefaultDepthStencilState)
		{
			mDefaultDepthStencilState->release();
			mDefaultDepthStencilState = nullptr;
		}
		if (nullptr != mDefaultBlendState)
		{
			mDefaultBlendState->release();
			mDefaultBlendState = nullptr;
		}
		if (nullptr != mDefaultSamplerState)
		{
			mDefaultSamplerState->release();
			mDefaultSamplerState = nullptr;
		}

		// Set no blend state reference, in case we have one
		if (nullptr != mBlendState)
		{
			omSetBlendState(nullptr);
		}

		// Set no depth stencil reference, in case we have one
		if (nullptr != mDepthStencilState)
		{
			omSetDepthStencilState(nullptr);
		}

		// Set no rasterizer state reference, in case we have one
		if (nullptr != mRasterizerState)
		{
			rsSetState(nullptr);
		}

		{ // For debugging: At this point there should be no resource instances left, validate this!
			// -> Are the currently any resource instances?
			const unsigned long numberOfCurrentResources = getStatistics().getNumberOfCurrentResources();
			if (numberOfCurrentResources > 0)
			{
				// Error!
				if (numberOfCurrentResources > 1)
				{
					RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 9 error: Renderer is going to be destroyed, but there are still %d resource instances left (memory leak)\n", numberOfCurrentResources)
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("Direct3D 9 error: Renderer is going to be destroyed, but there is still one resource instance left (memory leak)\n")
				}

				// Use debug output to show the current number of resource instances
				getStatistics().debugOutputCurrentResouces();
			}
		}

		// Release the Direct3D 9 query instance used for flush, in case we have one
		if (nullptr != mDirect3DQuery9Flush)
		{
			mDirect3DQuery9Flush->Release();
		}

		// Release the HLSL shader language instance, in case we have one
		if (nullptr != mShaderLanguageHlsl)
		{
			mShaderLanguageHlsl->release();
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
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	Renderer::ISwapChain *Direct3D9Renderer::getMainSwapChain() const
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

	const char *Direct3D9Renderer::getShaderLanguageName(uint32_t index) const
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

	Renderer::IShaderLanguage *Direct3D9Renderer::getShaderLanguage(const char *shaderLanguageName)
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
	Renderer::ISwapChain *Direct3D9Renderer::createSwapChain(handle nativeWindowHandle)
	{
		// The provided native window handle must not be a null handle
		return (NULL_HANDLE != nativeWindowHandle) ? new SwapChain(*this, nativeWindowHandle) : nullptr;
	}

	Renderer::IFramebuffer *Direct3D9Renderer::createFramebuffer(uint32_t numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture)
	{
		// Validation is done inside the framebuffer implementation
		return new Framebuffer(*this, numberOfColorTextures, colorTextures, depthStencilTexture);
	}

	Renderer::IVertexBuffer *Direct3D9Renderer::createVertexBuffer(uint32_t numberOfBytes, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		// TODO(co) Security checks
		return new VertexBuffer(*this, numberOfBytes, data, bufferUsage);
	}

	Renderer::IIndexBuffer *Direct3D9Renderer::createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		// TODO(co) Security checks
		return new IndexBuffer(*this, numberOfBytes, indexBufferFormat, data, bufferUsage);
	}

	Renderer::IVertexArray *Direct3D9Renderer::createVertexArray(const Renderer::VertexArrayAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, Renderer::IIndexBuffer *indexBuffer)
	{
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		return new VertexArray(*this, vertexAttributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
	}

	Renderer::ITextureBuffer *Direct3D9Renderer::createTextureBuffer(uint32_t, Renderer::TextureFormat::Enum, const void *, Renderer::BufferUsage::Enum)
	{
		// Direct3D 9 has no texture buffer support
		return nullptr;
	}

	Renderer::ITexture2D *Direct3D9Renderer::createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags, Renderer::TextureUsage::Enum textureUsage)
	{
		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0)
		{
			return new Texture2D(*this, width, height, textureFormat, data, flags, textureUsage);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture2DArray *Direct3D9Renderer::createTexture2DArray(uint32_t, uint32_t, uint32_t, Renderer::TextureFormat::Enum, void *, uint32_t, Renderer::TextureUsage::Enum)
	{
		// Direct3D 9 has no 2D texture arrays
		return nullptr;
	}

	Renderer::IRootSignature *Direct3D9Renderer::createRootSignature(const Renderer::RootSignature &)
	{
		// TODO(co) Implement me
		return nullptr;
	}

	Renderer::IPipelineState *Direct3D9Renderer::createPipelineState(const Renderer::PipelineState &pipelineState)
	{
		return new PipelineState(*this, pipelineState);
	}

	Renderer::IRasterizerState *Direct3D9Renderer::createRasterizerState(const Renderer::RasterizerState &rasterizerState)
	{
		return new RasterizerState(*this, rasterizerState);
	}

	Renderer::IDepthStencilState *Direct3D9Renderer::createDepthStencilState(const Renderer::DepthStencilState &depthStencilState)
	{
		return new DepthStencilState(*this, depthStencilState);
	}

	Renderer::IBlendState *Direct3D9Renderer::createBlendState(const Renderer::BlendState &blendState)
	{
		return new BlendState(*this, blendState);
	}

	Renderer::ISamplerState *Direct3D9Renderer::createSamplerState(const Renderer::SamplerState &samplerState)
	{
		return new SamplerState(*this, samplerState);
	}

	Renderer::ITextureCollection *Direct3D9Renderer::createTextureCollection(uint32_t numberOfTextures, Renderer::ITexture **textures)
	{
		return new TextureCollection(*this, numberOfTextures, textures);
	}

	Renderer::ISamplerStateCollection *Direct3D9Renderer::createSamplerStateCollection(uint32_t numberOfSamplerStates, Renderer::ISamplerState **samplerStates)
	{
		return new SamplerStateCollection(*this, numberOfSamplerStates, samplerStates);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool Direct3D9Renderer::map(Renderer::IResource &resource, uint32_t subresource, Renderer::MapType::Enum mapType, uint32_t, Renderer::MappedSubresource &mappedSubresource)
	{
		// The "Renderer::MapType::Enum" values directly map to Direct3D 10 & 11 constants, do not change them
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

			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
			case Renderer::ResourceType::SWAP_CHAIN:
			case Renderer::ResourceType::FRAMEBUFFER:
			case Renderer::ResourceType::UNIFORM_BUFFER:
			case Renderer::ResourceType::TEXTURE_BUFFER:
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
				// Nothing we can map, set known return values
				mappedSubresource.data		 = nullptr;
				mappedSubresource.rowPitch   = 0;
				mappedSubresource.depthPitch = 0;

				// Error!
				return false;
		}
	}

	void Direct3D9Renderer::unmap(Renderer::IResource &resource, uint32_t subresource)
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

			case Renderer::ResourceType::TEXTURE_2D:
				static_cast<Texture2D&>(resource).getDirect3DTexture9()->UnlockRect(subresource);
				break;

			case Renderer::ResourceType::PROGRAM:
			case Renderer::ResourceType::VERTEX_ARRAY:
			case Renderer::ResourceType::SWAP_CHAIN:
			case Renderer::ResourceType::FRAMEBUFFER:
			case Renderer::ResourceType::UNIFORM_BUFFER:
			case Renderer::ResourceType::TEXTURE_BUFFER:
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
				// Nothing we can unmap
				break;
		}
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::setGraphicsRootSignature(Renderer::IRootSignature *)
	{
		// TODO(co) Implement me
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

	void Direct3D9Renderer::setProgram(Renderer::IProgram *program)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

		// TODO(co) Avoid changing already set program

		if (nullptr != program)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *program)

			// TODO(co) HLSL buffer settings, unset previous program

			// Evaluate the internal program type of the new program to set
			switch (static_cast<Program*>(program)->getInternalResourceType())
			{
				case Program::InternalResourceType::HLSL:
				{
					// Get shaders
					const ProgramHlsl		 *programHlsl		 = static_cast<ProgramHlsl*>(program);
					const VertexShaderHlsl	 *vertexShaderHlsl	 = programHlsl->getVertexShaderHlsl();
					const FragmentShaderHlsl *fragmentShaderHlsl = programHlsl->getFragmentShaderHlsl();

					// Set shaders
					mDirect3DDevice9->SetVertexShader(vertexShaderHlsl  ? vertexShaderHlsl->getDirect3DVertexShader9()  : nullptr);
					mDirect3DDevice9->SetPixelShader(fragmentShaderHlsl ? fragmentShaderHlsl->getDirect3DPixelShader9() : nullptr);
					break;
				}
			}
		}
		else
		{
			// TODO(co) HLSL buffer settings
			mDirect3DDevice9->SetVertexShader(nullptr);
			mDirect3DDevice9->SetPixelShader(nullptr);
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(this)
	}


	//[-------------------------------------------------------]
	//[ Input-assembler (IA) stage                            ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::iaSetVertexArray(Renderer::IVertexArray *vertexArray)
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

	void Direct3D9Renderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology::Enum primitiveTopology)
	{
		// Backup the set primitive topology
		mPrimitiveTopology = primitiveTopology;
	}


	//[-------------------------------------------------------]
	//[ Vertex-shader (VS) stage                              ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::vsSetTexture(uint32_t unit, Renderer::ITexture *texture)
	{
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
		unit += D3DVERTEXTEXTURESAMPLER1;

		// Set a texture at that unit?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Evaluate the texture
			switch (texture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
					mDirect3DDevice9->SetTexture(unit, static_cast<Texture2D*>(texture)->getDirect3DTexture9());
					break;

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
					// 2D array textures are not supported by Direct3D 9
					break;

				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::TEXTURE_BUFFER:
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
			mDirect3DDevice9->SetTexture(unit, nullptr);
		}
	}

	void Direct3D9Renderer::vsSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

			// Loop through all textures within the given texture collection
			// -> "+ D3DVERTEXTEXTURESAMPLER1", see "Direct3D9Renderer::vsSetTexture()" above for details
			Renderer::ITexture **currentTexture = static_cast<TextureCollection*>(textureCollection)->getTextures();
			Renderer::ITexture **textureEnd	    = currentTexture + static_cast<TextureCollection*>(textureCollection)->getNumberOfTextures();
			for (uint32_t unit = startUnit + D3DVERTEXTEXTURESAMPLER1; currentTexture < textureEnd; ++currentTexture, ++unit)
			{
				// Get the current texture
				Renderer::ITexture *texture = *currentTexture;

				// Set a texture at that unit?
				if (nullptr != texture)
				{
					// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
					// -> Not required in here, this is already done within the texture collection

					// TODO(co) Some security checks might be wise *maximum number of texture units*
					// Evaluate the texture
					switch (texture->getResourceType())
					{
						case Renderer::ResourceType::TEXTURE_2D:
							mDirect3DDevice9->SetTexture(unit, static_cast<Texture2D*>(texture)->getDirect3DTexture9());
							break;

						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
							// 2D array textures are not supported by Direct3D 9
							break;

						case Renderer::ResourceType::PROGRAM:
						case Renderer::ResourceType::VERTEX_ARRAY:
						case Renderer::ResourceType::SWAP_CHAIN:
						case Renderer::ResourceType::FRAMEBUFFER:
						case Renderer::ResourceType::INDEX_BUFFER:
						case Renderer::ResourceType::VERTEX_BUFFER:
						case Renderer::ResourceType::UNIFORM_BUFFER:
						case Renderer::ResourceType::TEXTURE_BUFFER:
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
					mDirect3DDevice9->SetTexture(unit, nullptr);
				}
			}

			// End debug event
			RENDERER_END_DEBUG_EVENT(this)
		}
	}

	void Direct3D9Renderer::vsSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState)
	{
		// Update the given zero based texture unit (the constants are linear, so the following is fine)
		// -> "+ D3DVERTEXTEXTURESAMPLER1", see "Direct3D9Renderer::vsSetTexture()" above for details
		unit += D3DVERTEXTEXTURESAMPLER1;

		// Set a sampler at that unit?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

			// Set the Direct3D 9 sampler states
			static_cast<SamplerState*>(samplerState)->setDirect3D9SamplerStates(unit, *mDirect3DDevice9);

			// End debug event
			RENDERER_END_DEBUG_EVENT(this)
		}
		else
		{
			// Set the default sampler state
			if (nullptr != mDefaultSamplerState)
			{
				vsSetSamplerState(unit, mDefaultSamplerState);
			}
			else
			{
				// Fallback in case everything goes wrong

				// TODO(co) Set defaul settings
			}
		}
	}

	void Direct3D9Renderer::vsSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

			// Loop through all sampler states within the given sampler state collection
			// -> "+ D3DVERTEXTEXTURESAMPLER1", see "Direct3D9Renderer::vsSetTexture()" above for details
			Renderer::ISamplerState **currentSamplerState = static_cast<SamplerStateCollection*>(samplerStateCollection)->getSamplerStates();
			Renderer::ISamplerState **samplerStateEnd	  = currentSamplerState + static_cast<SamplerStateCollection*>(samplerStateCollection)->getNumberOfSamplerStates();
			for (uint32_t unit = startUnit + D3DVERTEXTEXTURESAMPLER1; currentSamplerState < samplerStateEnd; ++currentSamplerState, ++unit)
			{
				// Get the current sampler state
				Renderer::ISamplerState *samplerState = *currentSamplerState;

				// Set a sampler at that unit?
				if (nullptr != samplerState)
				{
					// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
					// -> Not required in here, this is already done within the texture collection

					// Set the Direct3D 9 sampler states
					static_cast<SamplerState*>(samplerState)->setDirect3D9SamplerStates(unit, *mDirect3DDevice9);
				}
				else
				{
					// Set the default sampler state
					if (nullptr != mDefaultSamplerState)
					{
						fsSetSamplerState(unit, mDefaultSamplerState);
					}
					else
					{
						// Fallback in case everything goes wrong

						// TODO(co) Set defaul settings
					}
				}
			}

			// End debug event
			RENDERER_END_DEBUG_EVENT(this)
		}
	}

	void Direct3D9Renderer::vsSetUniformBuffer(uint32_t, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Direct3D 9 has no uniform buffer support (UBO, "constant buffer" in Direct3D terminology)

		// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
		DIRECT3D9RENDERER_RENDERERMATCHCHECK_NOTNULL_RETURN(uniformBuffer)
	}


	//[-------------------------------------------------------]
	//[ Tessellation-control-shader (TCS) stage               ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::tcsSetTexture(uint32_t, Renderer::ITexture *texture)
	{
		// Direct3D 9 has no tessellation control shader support ("hull shader" in Direct3D terminology)

		// Is the given texture valid?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)
		}
	}

	void Direct3D9Renderer::tcsSetTextureCollection(uint32_t, Renderer::ITextureCollection *textureCollection)
	{
		// Direct3D 9 has no tessellation control shader support ("hull shader" in Direct3D terminology)

		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)
		}
	}

	void Direct3D9Renderer::tcsSetSamplerState(uint32_t, Renderer::ISamplerState *samplerState)
	{
		// Direct3D 9 has no tessellation control shader support ("hull shader" in Direct3D terminology)

		// Is the given sampler state valid?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)
		}
	}

	void Direct3D9Renderer::tcsSetSamplerStateCollection(uint32_t, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Direct3D 9 has no tessellation control shader support ("hull shader" in Direct3D terminology)

		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)
		}
	}

	void Direct3D9Renderer::tcsSetUniformBuffer(uint32_t, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Direct3D 9 has no tessellation control shader support ("hull shader" in Direct3D terminology)
		// Direct3D 9 has no uniform buffer support (UBO, "constant buffer" in Direct3D terminology)

		// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
		DIRECT3D9RENDERER_RENDERERMATCHCHECK_NOTNULL_RETURN(uniformBuffer)
	}


	//[-------------------------------------------------------]
	//[ Tessellation-evaluation-shader (TES) stage            ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::tesSetTexture(uint32_t, Renderer::ITexture *texture)
	{
		// Direct3D 9 has no tessellation evaluation shader support ("domain shader" in Direct3D terminology)

		// Is the given texture valid?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)
		}
	}

	void Direct3D9Renderer::tesSetTextureCollection(uint32_t, Renderer::ITextureCollection *textureCollection)
	{
		// Direct3D 9 has no tessellation evaluation shader support ("domain shader" in Direct3D terminology)

		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)
		}
	}

	void Direct3D9Renderer::tesSetSamplerState(uint32_t, Renderer::ISamplerState *samplerState)
	{
		// Direct3D 9 has no tessellation evaluation shader support ("domain shader" in Direct3D terminology)

		// Is the given sampler state valid?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)
		}
	}

	void Direct3D9Renderer::tesSetSamplerStateCollection(uint32_t, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Direct3D 9 has no tessellation evaluation shader support ("domain shader" in Direct3D terminology)

		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)
		}
	}

	void Direct3D9Renderer::tesSetUniformBuffer(uint32_t, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Direct3D 9 has no tessellation evaluation shader support ("domain shader" in Direct3D terminology)
		// Direct3D 9 has no uniform buffer support (UBO, "constant buffer" in Direct3D terminology)

		// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
		DIRECT3D9RENDERER_RENDERERMATCHCHECK_NOTNULL_RETURN(uniformBuffer)
	}


	//[-------------------------------------------------------]
	//[ Geometry-shader (GS) stage                            ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::gsSetTexture(uint32_t, Renderer::ITexture *texture)
	{
		// Direct3D 9 has no geometry shader support

		// Is the given texture valid?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)
		}
	}

	void Direct3D9Renderer::gsSetTextureCollection(uint32_t, Renderer::ITextureCollection *textureCollection)
	{
		// Direct3D 9 has no geometry shader support

		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)
		}
	}

	void Direct3D9Renderer::gsSetSamplerState(uint32_t, Renderer::ISamplerState *samplerState)
	{
		// Direct3D 9 has no geometry shader support

		// Is the given sampler state valid?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)
		}
	}

	void Direct3D9Renderer::gsSetSamplerStateCollection(uint32_t, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Direct3D 9 has no geometry shader support

		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)
		}
	}

	void Direct3D9Renderer::gsSetUniformBuffer(uint32_t, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Direct3D 9 has no geometry shader support
		// Direct3D 9 has no uniform buffer support (UBO, "constant buffer" in Direct3D terminology)

		// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
		DIRECT3D9RENDERER_RENDERERMATCHCHECK_NOTNULL_RETURN(uniformBuffer)
	}


	//[-------------------------------------------------------]
	//[ Rasterizer (RS) stage                                 ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport *viewports)
	{
		// Are the given viewports valid?
		if (numberOfViewports > 0 && nullptr != viewports)
		{
			// Set the Direct3D 9 viewport
			// -> Direct3D 9 supports only one viewport
		#ifndef RENDERER_NO_DEBUG
			if (numberOfViewports > 1)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 9 error: Direct3D 9 supports only one viewport")
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
	}

	void Direct3D9Renderer::rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles)
	{
		// Are the given scissor rectangles valid?
		if (numberOfScissorRectangles > 0 && nullptr != scissorRectangles)
		{
			// Set the Direct3D 9 scissor rectangles
			// -> "Renderer::ScissorRectangle" directly maps to Direct3D 9 & 10 & 11, do not change it
			// -> Direct3D 9 supports only one viewport
		#ifndef RENDERER_NO_DEBUG
			if (numberOfScissorRectangles > 1)
			{
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 9 error: Direct3D 9 supports only one scissor rectangle")
			}
		#endif
			mDirect3DDevice9->SetScissorRect(reinterpret_cast<const RECT*>(scissorRectangles));
		}
	}

	void Direct3D9Renderer::rsSetState(Renderer::IRasterizerState *rasterizerState)
	{
		// New rasterizer state?
		if (mRasterizerState != rasterizerState)
		{
			// Set a rasterizer state?
			if (nullptr != rasterizerState)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *rasterizerState)

				// Release the rasterizer state reference, in case we have one
				if (nullptr != mRasterizerState)
				{
					mRasterizerState->release();
				}

				// Set new rasterizer state and add a reference to it
				mRasterizerState = static_cast<RasterizerState*>(rasterizerState);
				mRasterizerState->addReference();

				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

				// Set the Direct3D 9 rasterizer states
				// TODO(co) Reduce state changes: Maybe it's a good idea to have alternative methods allowing to pass through the previous states and then performing per-state-change-comparison in order to reduce graphics-API calls
				mRasterizerState->setDirect3D9RasterizerStates(*mDirect3DDevice9);

				// End debug event
				RENDERER_END_DEBUG_EVENT(this)
			}
			else
			{
				// Set the default rasterizer state
				if (nullptr != mDefaultRasterizerState)
				{
					rsSetState(mDefaultRasterizerState);
				}
				else
				{
					// Fallback in case everything goes wrong

					// Release the rasterizer state reference, in case we have one
					if (nullptr != mRasterizerState)
					{
						// Release reference
						mRasterizerState->release();
						mRasterizerState = nullptr;
					}
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Fragment-shader (FS) stage                            ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::fsSetTexture(uint32_t unit, Renderer::ITexture *texture)
	{
		// Set a texture at that unit?
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Evaluate the texture
			switch (texture->getResourceType())
			{
				case Renderer::ResourceType::TEXTURE_2D:
					mDirect3DDevice9->SetTexture(unit, static_cast<Texture2D*>(texture)->getDirect3DTexture9());
					break;

				case Renderer::ResourceType::TEXTURE_2D_ARRAY:
					// 2D array textures are not supported by Direct3D 9
					break;

				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::TEXTURE_BUFFER:
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
			mDirect3DDevice9->SetTexture(unit, nullptr);
		}
	}

	void Direct3D9Renderer::fsSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

			// Loop through all textures within the given texture collection
			Renderer::ITexture **currentTexture = static_cast<TextureCollection*>(textureCollection)->getTextures();
			Renderer::ITexture **textureEnd	    = currentTexture + static_cast<TextureCollection*>(textureCollection)->getNumberOfTextures();
			for (uint32_t unit = startUnit; currentTexture < textureEnd; ++currentTexture, ++unit)
			{
				// Get the current texture
				Renderer::ITexture *texture = *currentTexture;

				// Set a texture at that unit?
				if (nullptr != texture)
				{
					// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
					// -> Not required in here, this is already done within the texture collection

					// TODO(co) Some security checks might be wise *maximum number of texture units*
					// Evaluate the texture
					switch (texture->getResourceType())
					{
						case Renderer::ResourceType::TEXTURE_2D:
							mDirect3DDevice9->SetTexture(unit, static_cast<Texture2D*>(texture)->getDirect3DTexture9());
							break;

						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
							// 2D array textures are not supported by Direct3D 9
							break;

						case Renderer::ResourceType::PROGRAM:
						case Renderer::ResourceType::VERTEX_ARRAY:
						case Renderer::ResourceType::SWAP_CHAIN:
						case Renderer::ResourceType::FRAMEBUFFER:
						case Renderer::ResourceType::INDEX_BUFFER:
						case Renderer::ResourceType::VERTEX_BUFFER:
						case Renderer::ResourceType::UNIFORM_BUFFER:
						case Renderer::ResourceType::TEXTURE_BUFFER:
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
					mDirect3DDevice9->SetTexture(unit, nullptr);
				}
			}

			// End debug event
			RENDERER_END_DEBUG_EVENT(this)
		}
	}

	void Direct3D9Renderer::fsSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState)
	{
		// Set a sampler at that unit?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

			// Set the Direct3D 9 sampler states
			static_cast<SamplerState*>(samplerState)->setDirect3D9SamplerStates(unit, *mDirect3DDevice9);

			// End debug event
			RENDERER_END_DEBUG_EVENT(this)
		}
		else
		{
			// Set the default sampler state
			if (nullptr != mDefaultSamplerState)
			{
				fsSetSamplerState(unit, mDefaultSamplerState);
			}
			else
			{
				// Fallback in case everything goes wrong

				// TODO(co) Set defaul settings
			}
		}
	}

	void Direct3D9Renderer::fsSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

			// Loop through all sampler states within the given sampler state collection
			Renderer::ISamplerState **currentSamplerState = static_cast<SamplerStateCollection*>(samplerStateCollection)->getSamplerStates();
			Renderer::ISamplerState **samplerStateEnd	  = currentSamplerState + static_cast<SamplerStateCollection*>(samplerStateCollection)->getNumberOfSamplerStates();
			for (uint32_t unit = startUnit; currentSamplerState < samplerStateEnd; ++currentSamplerState, ++unit)
			{
				// Get the current sampler state
				Renderer::ISamplerState *samplerState = *currentSamplerState;

				// Set a sampler at that unit?
				if (nullptr != samplerState)
				{
					// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
					// -> Not required in here, this is already done within the texture collection

					// Set the Direct3D 9 sampler states
					static_cast<SamplerState*>(samplerState)->setDirect3D9SamplerStates(unit, *mDirect3DDevice9);
				}
				else
				{
					// Set the default sampler state
					if (nullptr != mDefaultSamplerState)
					{
						fsSetSamplerState(unit, mDefaultSamplerState);
					}
					else
					{
						// Fallback in case everything goes wrong

						// TODO(co) Set defaul settings
					}
				}
			}

			// End debug event
			RENDERER_END_DEBUG_EVENT(this)
		}
	}

	void Direct3D9Renderer::fsSetUniformBuffer(uint32_t, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Direct3D 9 has no uniform buffer support (UBO, "constant buffer" in Direct3D terminology)

		// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
		DIRECT3D9RENDERER_RENDERERMATCHCHECK_NOTNULL_RETURN(uniformBuffer)
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	Renderer::IRenderTarget *Direct3D9Renderer::omGetRenderTarget()
	{
		return mRenderTarget;
	}

	void Direct3D9Renderer::omSetRenderTarget(Renderer::IRenderTarget *renderTarget)
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
						// Get the Direct3D 9 swap chain instance
						SwapChain *swapChain = static_cast<SwapChain*>(mRenderTarget);

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
						Framebuffer *framebuffer = static_cast<Framebuffer*>(mRenderTarget);

						// Set the Direct3D 9 color surfaces
						DWORD direct3D9RenderTargetIndex = 0;
						IDirect3DSurface9 **direct3D9ColorSurfacesEnd = framebuffer->getDirect3DSurface9Colors() + framebuffer->getNumberOfDirect3DSurface9Colors();
						for (IDirect3DSurface9 **direct3D9ColorSurface = framebuffer->getDirect3DSurface9Colors(); direct3D9ColorSurface < direct3D9ColorSurfacesEnd; ++direct3D9ColorSurface, ++direct3D9RenderTargetIndex)
						{
							mDirect3DDevice9->SetRenderTarget(direct3D9RenderTargetIndex, *direct3D9ColorSurface);
						}

						// Set the Direct3D 9 depth stencil surface
						mDirect3DDevice9->SetDepthStencilSurface(framebuffer->getDirect3DSurface9DepthStencil());
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
					mRenderTarget->release();
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

	void Direct3D9Renderer::omSetDepthStencilState(Renderer::IDepthStencilState *depthStencilState)
	{
		// New depth stencil state?
		if (mDepthStencilState != depthStencilState)
		{
			// Set a depth stencil state?
			if (nullptr != depthStencilState)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *depthStencilState)

				// Release the depth stencil state reference, in case we have one
				if (nullptr != mDepthStencilState)
				{
					mDepthStencilState->release();
				}

				// Set new depth stencil state and add a reference to it
				mDepthStencilState = static_cast<DepthStencilState*>(depthStencilState);
				mDepthStencilState->addReference();

				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

				// Set the Direct3D 9 depth stencil states
				// TODO(co) Reduce state changes: Maybe it's a good idea to have alternative methods allowing to pass through the previous states and then performing per-state-change-comparison in order to reduce graphics-API calls
				mDepthStencilState->setDirect3D9DepthStencilStates(*mDirect3DDevice9);

				// End debug event
				RENDERER_END_DEBUG_EVENT(this)
			}
			else
			{
				// Set the default depth stencil state
				if (nullptr != mDefaultDepthStencilState)
				{
					omSetDepthStencilState(mDefaultDepthStencilState);
				}
				else
				{
					// Fallback in case everything goes wrong

					// Release the depth stencil state reference, in case we have one
					if (nullptr != mDepthStencilState)
					{
						// Release reference
						mDepthStencilState->release();
						mDepthStencilState = nullptr;
					}
				}
			}
		}
	}

	void Direct3D9Renderer::omSetBlendState(Renderer::IBlendState *blendState)
	{
		// New blend state?
		if (mBlendState != blendState)
		{
			// Set a blend state?
			if (nullptr != blendState)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				DIRECT3D9RENDERER_RENDERERMATCHCHECK_RETURN(*this, *blendState)

				// Release the blend state reference, in case we have one
				if (nullptr != mBlendState)
				{
					mBlendState->release();
				}

				// Set new blend state and add a reference to it
				mBlendState = static_cast<BlendState*>(blendState);
				mBlendState->addReference();

				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(this)

				// Set the Direct3D 9 blend states
				// TODO(co) Reduce state changes: Maybe it's a good idea to have alternative methods allowing to pass through the previous states and then performing per-state-change-comparison in order to reduce graphics-API calls
				mBlendState->setDirect3D9BlendStates(*mDirect3DDevice9);

				// End debug event
				RENDERER_END_DEBUG_EVENT(this)
			}
			else
			{
				// Set the default blend state
				if (nullptr != mDefaultBlendState)
				{
					omSetBlendState(mDefaultBlendState);
				}
				else
				{
					// Fallback in case everything goes wrong

					// Release the blend state reference, in case we have one
					if (nullptr != mBlendState)
					{
						// Release reference
						mBlendState->release();
						mBlendState = nullptr;
					}
				}
			}
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
				RENDERER_OUTPUT_DEBUG_STRING("Direct3D 9 warning: The given clear color was clamped to [0, 1] because Direct3D 9 does not support values outside this range")
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
		IDirect3DSurface9 *direct3DSurface9 = nullptr;
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

	bool Direct3D9Renderer::beginScene()
	{
		return SUCCEEDED(mDirect3DDevice9->BeginScene());
	}

	void Direct3D9Renderer::endScene()
	{
		mDirect3DDevice9->EndScene();
	}


	//[-------------------------------------------------------]
	//[ Draw call                                             ]
	//[-------------------------------------------------------]
	void Direct3D9Renderer::draw(uint32_t startVertexLocation, uint32_t numberOfVertices)
	{
		// Get number of primitives
		uint32_t primitiveCount;
		switch (mPrimitiveTopology)
		{
			case Renderer::PrimitiveTopology::POINT_LIST:
				primitiveCount = numberOfVertices;
				break;

			case Renderer::PrimitiveTopology::LINE_LIST:
				primitiveCount = numberOfVertices - 1;
				break;

			case Renderer::PrimitiveTopology::LINE_STRIP:
				primitiveCount = numberOfVertices - 1;
				break;

			case Renderer::PrimitiveTopology::TRIANGLE_LIST:
				primitiveCount = numberOfVertices / 3;
				break;

			case Renderer::PrimitiveTopology::TRIANGLE_STRIP:
				primitiveCount = numberOfVertices - 2;
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

		// The "Renderer::PrimitiveTopology::Enum" values directly map to Direct3D 9 & 10 & 11 constants, do not change them
		mDirect3DDevice9->DrawPrimitive(static_cast<D3DPRIMITIVETYPE>(mPrimitiveTopology), startVertexLocation, primitiveCount);
	}

	void Direct3D9Renderer::drawInstanced(uint32_t, uint32_t, uint32_t)
	{
		// Error!
		// -> In Direct3D 9, instanced arrays is only possible when drawing indexed primitives, see
		//    "Efficiently Drawing Multiple Instances of Geometry (Direct3D 9)"-article at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb173349%28v=vs.85%29.aspx#Drawing_Non_Indexed_Geometry
		// -> This document states that this is not supported by hardware acceleration on any device, and it's long winded anyway
	}

	void Direct3D9Renderer::drawIndexed(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t minimumIndex, uint32_t numberOfVertices)
	{
		// Get number of primitives
		uint32_t primitiveCount;
		switch (mPrimitiveTopology)
		{
			case Renderer::PrimitiveTopology::POINT_LIST:
				primitiveCount = numberOfIndices;
				break;

			case Renderer::PrimitiveTopology::LINE_LIST:
				primitiveCount = numberOfIndices - 1;
				break;

			case Renderer::PrimitiveTopology::LINE_STRIP:
				primitiveCount = numberOfIndices - 1;
				break;

			case Renderer::PrimitiveTopology::TRIANGLE_LIST:
				primitiveCount = numberOfIndices / 3;
				break;

			case Renderer::PrimitiveTopology::TRIANGLE_STRIP:
				primitiveCount = numberOfIndices - 2;
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

		// The "Renderer::PrimitiveTopology::Enum" values directly map to Direct3D 9 & 10 & 11 constants, do not change them
		mDirect3DDevice9->DrawIndexedPrimitive(static_cast<D3DPRIMITIVETYPE>(mPrimitiveTopology), static_cast<INT>(baseVertexLocation), minimumIndex, numberOfVertices, startIndexLocation, primitiveCount);
	}

	void Direct3D9Renderer::drawIndexedInstanced(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t minimumIndex, uint32_t numberOfVertices, uint32_t numberOfInstances)
	{
		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
		if (mCapabilities.instancedArrays)
		{
			// The "Efficiently Drawing Multiple Instances of Geometry (Direct3D 9)"-article at MSDN http://msdn.microsoft.com/en-us/library/windows/desktop/bb173349%28v=vs.85%29.aspx#Drawing_Non_Indexed_Geometry
			// states: "Note that D3DSTREAMSOURCE_INDEXEDDATA and the number of instances to draw must always be set in stream zero."
			// -> "D3DSTREAMSOURCE_INSTANCEDATA" is set within "Direct3D9Renderer::VertexArray::enableDirect3DVertexDeclarationAndStreamSource()"
			mDirect3DDevice9->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | numberOfInstances);

			// Draw
			drawIndexed(startIndexLocation, numberOfIndices, baseVertexLocation, minimumIndex, numberOfVertices);

			// Reset the stream source frequency
			mDirect3DDevice9->SetStreamSourceFreq(0, 1);
		}
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
	//[ Debug                                                 ]
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

	void Direct3D9Renderer::setDebugMarker(const wchar_t *name)
	{
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			if (nullptr != D3DPERF_SetMarker)
			{
				D3DPERF_SetMarker(D3DCOLOR_RGBA(255, 0, 255, 255), name);
			}
		#endif
	}

	void Direct3D9Renderer::beginDebugEvent(const wchar_t *name)
	{
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			if (nullptr != D3DPERF_BeginEvent)
			{
				D3DPERF_BeginEvent(D3DCOLOR_RGBA(255, 255, 255, 255), name);
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

		// Uniform buffer object (UBO, "constant buffer" in Direct3D terminology) supported?
		mCapabilities.uniformBuffer = false;

		// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
		mCapabilities.maximumTextureBufferSize = 0;

		// Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		mCapabilities.individualUniforms = true;

		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
		mCapabilities.instancedArrays = (d3dCaps9.PixelShaderVersion >= D3DPS_VERSION(3, 0));

		// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID)
		mCapabilities.drawInstanced = false;

		// Base vertex supported for draw calls?
		mCapabilities.baseVertex = true;

		// Is there support for vertex shaders (VS)?
		mCapabilities.vertexShader = true;

		// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
		mCapabilities.maximumNumberOfPatchVertices = 0;	// Direct3D 9 has no tessellation support

		// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
		mCapabilities.maximumNumberOfGsOutputVertices = 0;	// Direct3D 9 has no support for geometry shaders

		// Is there support for fragment shaders (FS)?
		mCapabilities.fragmentShader = true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
