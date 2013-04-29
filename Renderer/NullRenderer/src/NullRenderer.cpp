/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
#include "NullRenderer/NullRenderer.h"
#include "NullRenderer/NullDebug.h"	// For "NULLRENDERER_RENDERERMATCHCHECK_RETURN()"
#include "NullRenderer/Program.h"
#include "NullRenderer/Texture2D.h"
#include "NullRenderer/Texture2DArray.h"
#include "NullRenderer/IndexBuffer.h"
#include "NullRenderer/BlendState.h"
#include "NullRenderer/SamplerState.h"
#include "NullRenderer/TextureBuffer.h"
#include "NullRenderer/VertexBuffer.h"
#include "NullRenderer/VertexArray.h"
#include "NullRenderer/SwapChain.h"
#include "NullRenderer/Framebuffer.h"
#include "NullRenderer/RasterizerState.h"
#include "NullRenderer/DepthStencilState.h"
#include "NullRenderer/TextureCollection.h"
#include "NullRenderer/SamplerStateCollection.h"
#include "NullRenderer/ShaderLanguage.h"
#include "NullRenderer/UniformBuffer.h"

#include <string.h>


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef NULLRENDERER_EXPORTS
	#define NULLRENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define NULLRENDERER_API_EXPORT
#endif
NULLRENDERER_API_EXPORT Renderer::IRenderer *createNullRendererInstance(handle nativeWindowHandle)
{
	return new NullRenderer::NullRenderer(nativeWindowHandle);
}
#undef NULLRENDERER_API_EXPORT


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace NullRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	NullRenderer::NullRenderer(handle nativeWindowHandle) :
		mShaderLanguage(nullptr),
		mMainSwapChain(nullptr),
		mRenderTarget(nullptr)
	{
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

	NullRenderer::~NullRenderer()
	{
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
					RENDERER_OUTPUT_DEBUG_PRINTF("Null error: Renderer is going to be destroyed, but there are still %d resource instances left (memory leak)\n", numberOfCurrentResources)
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("Null error: Renderer is going to be destroyed, but there is still one resource instance left (memory leak)\n")
				}

				// Use debug output to show the current number of resource instances
				getStatistics().debugOutputCurrentResouces();
			}
		}

		// Release the null shader language instance, in case we have one
		if (nullptr != mShaderLanguage)
		{
			mShaderLanguage->release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	const char *NullRenderer::getName() const
	{
		return "Null";
	}

	bool NullRenderer::isInitialized() const
	{
		return true;
	}

	Renderer::ISwapChain *NullRenderer::getMainSwapChain() const
	{
		return mMainSwapChain;
	}


	//[-------------------------------------------------------]
	//[ Shader language                                       ]
	//[-------------------------------------------------------]
	unsigned int NullRenderer::getNumberOfShaderLanguages() const
	{
		// Only one shader language supported in here
		return 1;
	}

	const char *NullRenderer::getShaderLanguageName(unsigned int index) const
	{
		// Only one shader language supported in here
		if (0 == index)
		{
			return ShaderLanguage::NAME;
		}
		else
		{
			// Error!
			return nullptr;
		}
	}

	Renderer::IShaderLanguage *NullRenderer::getShaderLanguage(const char *shaderLanguageName)
	{
		// In case "shaderLanguage" is a null pointer, use the default shader language
		if (nullptr != shaderLanguageName)
		{
			// In case "shaderLanguage" is a null pointer, use the default shader language
			// -> Only one shader language supported in here
			if (nullptr == shaderLanguageName || !stricmp(shaderLanguageName, ShaderLanguage::NAME))
			{
				// If required, create the null shader language instance right now
				if (nullptr == mShaderLanguage)
				{
					mShaderLanguage = new ShaderLanguage(*this);
					mShaderLanguage->addReference();	// Internal renderer reference
				}

				// Return the shader language instance
				return mShaderLanguage;
			}

			// Error!
			return nullptr;
		}

		// Return the null shader language instance as default
		return getShaderLanguage(ShaderLanguage::NAME);
	}


	//[-------------------------------------------------------]
	//[ Resource creation                                     ]
	//[-------------------------------------------------------]
	Renderer::ISwapChain *NullRenderer::createSwapChain(handle nativeWindowHandle)
	{
		// The provided native window handle must not be a null handle
		return (NULL_HANDLE != nativeWindowHandle) ? new SwapChain(*this, nativeWindowHandle) : nullptr;
	}

	Renderer::IFramebuffer *NullRenderer::createFramebuffer(unsigned int numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture)
	{
		// We don't keep a reference to the provided textures in here
		// -> Ensure a correct reference counter behaviour

		// Are there any color textures?
		if (numberOfColorTextures > 0)
		{
			// Loop through all color textures
			Renderer::ITexture **colorTexturesEnd = colorTextures + numberOfColorTextures;
			for (Renderer::ITexture **colorTexture = colorTextures; colorTexture < colorTexturesEnd; ++colorTexture, ++colorTextures)
			{
				// Valid entry?
				if (nullptr != *colorTextures)
				{
					// TODO(co) Add security check: Is the given resource one of the currently used renderer?
					(*colorTextures)->addReference();
					(*colorTextures)->release();
				}
			}
		}

		// Add a reference to the used depth stencil texture
		if (nullptr != depthStencilTexture)
		{
			depthStencilTexture->addReference();
			depthStencilTexture->release();
		}

		// Create the framebuffer instance
		return new Framebuffer(*this);
	}

	Renderer::IVertexBuffer *NullRenderer::createVertexBuffer(unsigned int, const void *, Renderer::BufferUsage::Enum)
	{
		return new VertexBuffer(*this);
	}

	Renderer::IIndexBuffer *NullRenderer::createIndexBuffer(unsigned int, Renderer::IndexBufferFormat::Enum, const void *, Renderer::BufferUsage::Enum)
	{
		return new IndexBuffer(*this);
	}

	Renderer::ITextureBuffer *NullRenderer::createTextureBuffer(unsigned int, Renderer::TextureFormat::Enum, const void *, Renderer::BufferUsage::Enum)
	{
		return new TextureBuffer(*this);
	}

	Renderer::ITexture2D *NullRenderer::createTexture2D(unsigned int width, unsigned int height, Renderer::TextureFormat::Enum, void *, unsigned int, Renderer::TextureUsage::Enum)
	{
		return new Texture2D(*this, width, height);
	}

	Renderer::ITexture2DArray *NullRenderer::createTexture2DArray(unsigned int width, unsigned int height, unsigned int numberOfSlices, Renderer::TextureFormat::Enum, void *, unsigned int, Renderer::TextureUsage::Enum)
	{
		return new Texture2DArray(*this, width, height, numberOfSlices);
	}

	Renderer::IRasterizerState *NullRenderer::createRasterizerState(const Renderer::RasterizerState &)
	{
		return new RasterizerState(*this);
	}

	Renderer::IDepthStencilState *NullRenderer::createDepthStencilState(const Renderer::DepthStencilState &)
	{
		return new DepthStencilState(*this);
	}

	Renderer::IBlendState *NullRenderer::createBlendState(const Renderer::BlendState &)
	{
		return new BlendState(*this);
	}

	Renderer::ISamplerState *NullRenderer::createSamplerState(const Renderer::SamplerState &)
	{
		return new SamplerState(*this);
	}

	Renderer::ITextureCollection *NullRenderer::createTextureCollection(unsigned int numberOfTextures, Renderer::ITexture **textures)
	{
		return new TextureCollection(*this, numberOfTextures, textures);
	}

	Renderer::ISamplerStateCollection *NullRenderer::createSamplerStateCollection(unsigned int numberOfSamplerStates, Renderer::ISamplerState **samplerStates)
	{
		return new SamplerStateCollection(*this, numberOfSamplerStates, samplerStates);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool NullRenderer::map(Renderer::IResource &, unsigned int, Renderer::MapType::Enum, unsigned int, Renderer::MappedSubresource &)
	{
		// Not supported by the null renderer
		return false;
	}

	void NullRenderer::unmap(Renderer::IResource &, unsigned int)
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void NullRenderer::setProgram(Renderer::IProgram *program)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != program)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *program)
		}
	}


	//[-------------------------------------------------------]
	//[ Input-assembler (IA) stage                            ]
	//[-------------------------------------------------------]
	void NullRenderer::iaSetVertexArray(Renderer::IVertexArray *vertexArray)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != vertexArray)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *vertexArray)
		}
	}

	void NullRenderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology::Enum)
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Vertex-shader (VS) stage                              ]
	//[-------------------------------------------------------]
	void NullRenderer::vsSetTexture(unsigned int, Renderer::ITexture *texture)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)
		}
	}

	void NullRenderer::vsSetTextureCollection(unsigned int, Renderer::ITextureCollection *textureCollection)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)
		}
	}

	void NullRenderer::vsSetSamplerState(unsigned int, Renderer::ISamplerState *samplerState)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)
		}
	}

	void NullRenderer::vsSetSamplerStateCollection(unsigned int, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)
		}
	}

	void NullRenderer::vsSetUniformBuffer(unsigned int, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}
	}


	//[-------------------------------------------------------]
	//[ Tessellation-control-shader (TCS) stage               ]
	//[-------------------------------------------------------]
	void NullRenderer::tcsSetTexture(unsigned int, Renderer::ITexture *texture)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)
		}
	}

	void NullRenderer::tcsSetTextureCollection(unsigned int, Renderer::ITextureCollection *textureCollection)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)
		}
	}

	void NullRenderer::tcsSetSamplerState(unsigned int, Renderer::ISamplerState *samplerState)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)
		}
	}

	void NullRenderer::tcsSetSamplerStateCollection(unsigned int, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)
		}
	}

	void NullRenderer::tcsSetUniformBuffer(unsigned int, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}
	}


	//[-------------------------------------------------------]
	//[ Tessellation-evaluation-shader (TES) stage            ]
	//[-------------------------------------------------------]
	void NullRenderer::tesSetTexture(unsigned int, Renderer::ITexture *texture)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)
		}
	}

	void NullRenderer::tesSetTextureCollection(unsigned int, Renderer::ITextureCollection *textureCollection)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)
		}
	}

	void NullRenderer::tesSetSamplerState(unsigned int, Renderer::ISamplerState *samplerState)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)
		}
	}

	void NullRenderer::tesSetSamplerStateCollection(unsigned int, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)
		}
	}

	void NullRenderer::tesSetUniformBuffer(unsigned int, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}
	}


	//[-------------------------------------------------------]
	//[ Geometry-shader (GS) stage                            ]
	//[-------------------------------------------------------]
	void NullRenderer::gsSetTexture(unsigned int, Renderer::ITexture *texture)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)
		}
	}

	void NullRenderer::gsSetTextureCollection(unsigned int, Renderer::ITextureCollection *textureCollection)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)
		}
	}

	void NullRenderer::gsSetSamplerState(unsigned int, Renderer::ISamplerState *samplerState)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)
		}
	}

	void NullRenderer::gsSetSamplerStateCollection(unsigned int, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)
		}
	}

	void NullRenderer::gsSetUniformBuffer(unsigned int, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}
	}


	//[-------------------------------------------------------]
	//[ Rasterizer (RS) stage                                 ]
	//[-------------------------------------------------------]
	void NullRenderer::rsSetViewports(unsigned int, const Renderer::Viewport *)
	{
		// Nothing to do in here
	}

	void NullRenderer::rsSetScissorRectangles(unsigned int, const Renderer::ScissorRectangle *)
	{
		// Nothing to do in here
	}

	void NullRenderer::rsSetState(Renderer::IRasterizerState *rasterizerState)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != rasterizerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *rasterizerState)
		}
	}


	//[-------------------------------------------------------]
	//[ Fragment-shader (FS) stage                            ]
	//[-------------------------------------------------------]
	void NullRenderer::fsSetTexture(unsigned int, Renderer::ITexture *texture)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != texture)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)
		}
	}

	void NullRenderer::fsSetTextureCollection(unsigned int, Renderer::ITextureCollection *textureCollection)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)
		}
	}

	void NullRenderer::fsSetSamplerState(unsigned int, Renderer::ISamplerState *samplerState)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)
		}
	}

	void NullRenderer::fsSetSamplerStateCollection(unsigned int, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)
		}
	}

	void NullRenderer::fsSetUniformBuffer(unsigned int, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)
		}
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	Renderer::IRenderTarget *NullRenderer::omGetRenderTarget()
	{
		return mRenderTarget;
	}

	void NullRenderer::omSetRenderTarget(Renderer::IRenderTarget *renderTarget)
	{
		// New render target?
		if (mRenderTarget != renderTarget)
		{
			// Set a render target?
			if (nullptr != renderTarget)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *renderTarget)

				// Release the render target reference, in case we have one
				if (nullptr != mRenderTarget)
				{
					mRenderTarget->release();
				}

				// Set new render target and add a reference to it
				mRenderTarget = renderTarget;
				mRenderTarget->addReference();

				// That's all folks!
			}
			else
			{
				// Release the render target reference, in case we have one
				if (nullptr != mRenderTarget)
				{
					mRenderTarget->release();
					mRenderTarget = nullptr;
				}

				// That's all folks!
			}
		}
	}

	void NullRenderer::omSetDepthStencilState(Renderer::IDepthStencilState *depthStencilState)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != depthStencilState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *depthStencilState)
		}
	}

	void NullRenderer::omSetBlendState(Renderer::IBlendState *blendState)
	{
		// Nothing to do in here, the following is just for debugging
		if (nullptr != blendState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			NULLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *blendState)
		}
	}


	//[-------------------------------------------------------]
	//[ Operations                                            ]
	//[-------------------------------------------------------]
	void NullRenderer::clear(unsigned int, const float [4], float, unsigned int)
	{
		// Nothing to do in here
	}

	bool NullRenderer::beginScene()
	{
		// Nothing to do in here

		// Done
		return true;
	}

	void NullRenderer::endScene()
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Draw call                                             ]
	//[-------------------------------------------------------]
	void NullRenderer::draw(unsigned int, unsigned int)
	{
		// Nothing to do in here
	}

	void NullRenderer::drawInstanced(unsigned int, unsigned int, unsigned int)
	{
		// Nothing to do in here
	}

	void NullRenderer::drawIndexed(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int)
	{
		// Nothing to do in here
	}

	void NullRenderer::drawIndexedInstanced(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int)
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Synchronization                                       ]
	//[-------------------------------------------------------]
	void NullRenderer::flush()
	{
		// Nothing to do in here
	}

	void NullRenderer::finish()
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Debug                                                 ]
	//[-------------------------------------------------------]
	bool NullRenderer::isDebugEnabled()
	{
		// Nothing to do in here

		// Debug disabled
		return false;
	}

	void NullRenderer::setDebugMarker(const wchar_t *)
	{
		// Nothing to do in here
	}

	void NullRenderer::beginDebugEvent(const wchar_t *)
	{
		// Nothing to do in here
	}

	void NullRenderer::endDebugEvent()
	{
		// Nothing to do in here
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void NullRenderer::initializeCapabilities()
	{
		// Maximum number of viewports (always at least 1)
		mCapabilities.maximumNumberOfViewports = 1;

		// Maximum number of simultaneous render targets (if <1 render to texture is not supported)
		mCapabilities.maximumNumberOfSimultaneousRenderTargets = 8;

		// Maximum texture dimension
		mCapabilities.maximumTextureDimension = 42;

		// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
		mCapabilities.maximumNumberOf2DTextureArraySlices = 42;

		// Uniform buffer object (UBO, "constant buffer" in Direct3D terminology) supported?
		mCapabilities.uniformBuffer = true;

		// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
		mCapabilities.maximumTextureBufferSize = 42;

		// Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		mCapabilities.individualUniforms = true;

		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
		mCapabilities.instancedArrays = true;

		// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID)
		mCapabilities.drawInstanced = true;

		// Base vertex supported for draw calls?
		mCapabilities.baseVertex = true;

		// Is there support for vertex shaders (VS)?
		mCapabilities.vertexShader = true;

		// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
		mCapabilities.maximumNumberOfPatchVertices = 32;

		// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
		mCapabilities.maximumNumberOfGsOutputVertices = 1024;

		// Is there support for fragment shaders (FS)?
		mCapabilities.fragmentShader = true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // NullRenderer
