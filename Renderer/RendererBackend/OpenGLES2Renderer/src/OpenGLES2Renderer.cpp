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
#include "OpenGLES2Renderer/OpenGLES2Renderer.h"
#include "OpenGLES2Renderer/OpenGLES2Debug.h"	// For "OPENGLES2RENDERER_RENDERERMATCHCHECK_RETURN()"
#include "OpenGLES2Renderer/Mapping.h"
#include "OpenGLES2Renderer/Texture2D.h"
#include "OpenGLES2Renderer/IExtensions.h"
#include "OpenGLES2Renderer/IndexBuffer.h"
#include "OpenGLES2Renderer/SamplerState.h"
#include "OpenGLES2Renderer/VertexBuffer.h"
#include "OpenGLES2Renderer/VertexArrayVao.h"
#include "OpenGLES2Renderer/VertexArrayNoVao.h"
#include "OpenGLES2Renderer/RootSignature.h"
#include "OpenGLES2Renderer/PipelineState.h"
#include "OpenGLES2Renderer/SwapChain.h"
#include "OpenGLES2Renderer/Framebuffer.h"
#include "OpenGLES2Renderer/Texture2DArray.h"
#include "OpenGLES2Renderer/ContextRuntimeLinking.h"
#include "OpenGLES2Renderer/Shader/Program.h"
#include "OpenGLES2Renderer/Shader/ShaderLanguageGlsl.h"


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef OPENGLES2RENDERER_EXPORTS
	#define OPENGLES2RENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define OPENGLES2RENDERER_API_EXPORT
#endif
OPENGLES2RENDERER_API_EXPORT Renderer::IRenderer *createOpenGLES2RendererInstance(handle nativeWindowHandle)
{
	return new OpenGLES2Renderer::OpenGLES2Renderer(nativeWindowHandle);
}
#undef OPENGLES2RENDERER_API_EXPORT


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	OpenGLES2Renderer::OpenGLES2Renderer(handle nativeWindowHandle) :
		mContext(new ContextRuntimeLinking(nativeWindowHandle)),
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
			mMainSwapChain->release();
			mMainSwapChain = nullptr;
		}
		if (nullptr != mRenderTarget)
		{
			mRenderTarget->release();
			mRenderTarget = nullptr;
		}
		if (nullptr != mDefaultSamplerState)
		{
			mDefaultSamplerState->release();
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
			mGraphicsRootSignature->release();
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
			mShaderLanguageGlsl->release();
		}

		// Destroy the context instance
		delete mContext;
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
		return (EGL_NO_CONTEXT != mContext->getEGLContext());
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

	Renderer::IVertexBuffer *OpenGLES2Renderer::createVertexBuffer(uint32_t numberOfBytes, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new VertexBuffer(*this, numberOfBytes, data, bufferUsage);
	}

	Renderer::IIndexBuffer *OpenGLES2Renderer::createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		return new IndexBuffer(*this, numberOfBytes, indexBufferFormat, data, bufferUsage);
	}

	Renderer::IVertexArray *OpenGLES2Renderer::createVertexArray(const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, Renderer::IIndexBuffer *indexBuffer)
	{
		// Is "GL_OES_vertex_array_object" there?
		if (mContext->getExtensions().isGL_OES_vertex_array_object())
		{
			// Effective vertex array object (VAO)
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			return new VertexArrayVao(*this, vertexAttributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
		}
		else
		{
			// Traditional version
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			return new VertexArrayNoVao(*this, vertexAttributes, numberOfVertexBuffers, vertexBuffers, static_cast<IndexBuffer*>(indexBuffer));
		}
	}

	Renderer::ITextureBuffer *OpenGLES2Renderer::createTextureBuffer(uint32_t, Renderer::TextureFormat::Enum, const void *, Renderer::BufferUsage::Enum)
	{
		// OpenGL ES 2 has no texture buffer support
		return nullptr;
	}

	Renderer::ITexture2D *OpenGLES2Renderer::createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags, Renderer::TextureUsage::Enum, const Renderer::OptimizedTextureClearValue*)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL ES 2 has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0)
		{
			return new Texture2D(*this, width, height, textureFormat, data, flags);
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture2DArray *OpenGLES2Renderer::createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data, uint32_t flags, Renderer::TextureUsage::Enum)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL ES 2 has no texture usage indication

		// Check whether or not the given texture dimension is valid, "GL_EXT_texture_array" extension required
		if (width > 0 && height > 0 && numberOfSlices > 0 && mContext->getExtensions().isGL_EXT_texture_array())
		{
			return new Texture2DArray(*this, width, height, numberOfSlices, textureFormat, data, flags);
		}
		else
		{
			return nullptr;
		}
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
	bool OpenGLES2Renderer::map(Renderer::IResource &, uint32_t, Renderer::MapType::Enum, uint32_t, Renderer::MappedSubresource &)
	{
		// TODO(co) Implement me
		return false;
	}

	void OpenGLES2Renderer::unmap(Renderer::IResource &, uint32_t)
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::setGraphicsRootSignature(Renderer::IRootSignature* rootSignature)
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
			const Renderer::ResourceType::Enum resourceType = resource->getResourceType();
			switch (resourceType)
			{
				case Renderer::ResourceType::TEXTURE_BUFFER:
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL ES 2 error: OpenGL ES 2 has no texture buffer support")
					break;

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

							// Bind texture
							if (resourceType == Renderer::ResourceType::TEXTURE_2D_ARRAY)
							{
								// No extension check required, if we in here we already know it must exist
								glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, static_cast<Texture2DArray*>(resource)->getOpenGLES2Texture());
							}
							else
							{
								glBindTexture(GL_TEXTURE_2D, static_cast<Texture2D*>(resource)->getOpenGLES2Texture());
							}

							// Set the OpenGL ES 2 sampler states
							mGraphicsRootSignature->setOpenGLES2SamplerStates(descriptorRange->samplerRootParameterIndex);

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
					mGraphicsRootSignature->setSamplerState(descriptorRange->samplerRootParameterIndex, static_cast<SamplerState*>(resource));
					break;
				}

				case Renderer::ResourceType::ROOT_SIGNATURE:
				case Renderer::ResourceType::PROGRAM:
				case Renderer::ResourceType::VERTEX_ARRAY:
				case Renderer::ResourceType::SWAP_CHAIN:
				case Renderer::ResourceType::FRAMEBUFFER:
				case Renderer::ResourceType::INDEX_BUFFER:
				case Renderer::ResourceType::VERTEX_BUFFER:
				case Renderer::ResourceType::UNIFORM_BUFFER:
				case Renderer::ResourceType::PIPELINE_STATE:
				case Renderer::ResourceType::RASTERIZER_STATE:
				case Renderer::ResourceType::DEPTH_STENCIL_STATE:
				case Renderer::ResourceType::BLEND_STATE:
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
						mVertexArray->release();
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
						mVertexArray->release();
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
					mVertexArray->release();
					mVertexArray = nullptr;
				}
			}
		}
	}

	void OpenGLES2Renderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology::Enum primitiveTopology)
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
			uint32_t renderTargetWidth =  0;
			uint32_t renderTargetHeight = 0;
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
			uint32_t renderTargetWidth =  0;
			uint32_t renderTargetHeight = 0;
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
	Renderer::IRenderTarget *OpenGLES2Renderer::omGetRenderTarget()
	{
		return mRenderTarget;
	}

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
				if (nullptr != mRenderTarget)
				{
					// Unbind OpenGL ES 2 framebuffer?
					if (Renderer::ResourceType::FRAMEBUFFER == mRenderTarget->getResourceType() && Renderer::ResourceType::FRAMEBUFFER != renderTarget->getResourceType())
					{
						// We do not render into a OpenGL ES 2 framebuffer
						glBindFramebuffer(GL_FRAMEBUFFER, 0);
					}

					// Release the reference
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
							// "GL_COLOR_ATTACHMENT0" and "GL_COLOR_ATTACHMENT0_NV" have the same value
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
					mRenderTarget->release();
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

	bool OpenGLES2Renderer::beginScene()
	{
		// Not required when using OpenGL ES 2

		// Done
		return true;
	}

	void OpenGLES2Renderer::endScene()
	{
		// We need to forget about the currently set render target
		omSetRenderTarget(nullptr);

		// We need to forget about the currently set vertex array
		iaSetVertexArray(nullptr);
	}


	//[-------------------------------------------------------]
	//[ Draw call                                             ]
	//[-------------------------------------------------------]
	void OpenGLES2Renderer::draw(uint32_t startVertexLocation, uint32_t numberOfVertices)
	{
		// Is currently a vertex array set?
		if (nullptr != mVertexArray)
		{
			// Draw
			glDrawArrays(mOpenGLES2PrimitiveTopology, static_cast<GLint>(startVertexLocation), static_cast<GLsizei>(numberOfVertices));
		}
	}

	void OpenGLES2Renderer::drawInstanced(uint32_t, uint32_t, uint32_t)
	{
		// Error! OpenGL ES 2 has no instancing support!
	}

	void OpenGLES2Renderer::drawIndexed(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t, uint32_t, uint32_t)
	{
		// Is currently an vertex array?
		if (nullptr != mVertexArray)
		{
			// Get the used index buffer
			IndexBuffer *indexBuffer = mVertexArray->getIndexBuffer();
			if (nullptr != indexBuffer)
			{
				// OpenGL ES 2 has no "GL_ARB_draw_elements_base_vertex" equivalent, so, we can't support "baseVertexLocation" in here
				// OpenGL ES 2 has no "GL_EXT_draw_range_elements" equivalent, so, we can't support "minimumIndex" & "numberOfVertices" in here

				// Draw
				glDrawElements(mOpenGLES2PrimitiveTopology, static_cast<GLsizei>(numberOfIndices), indexBuffer->getOpenGLES2Type(), reinterpret_cast<const GLvoid*>(startIndexLocation * sizeof(int)));
			}
		}
	}

	void OpenGLES2Renderer::drawIndexedInstanced(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)
	{
		// OpenGL ES 2 has no "GL_ARB_draw_elements_base_vertex" equivalent, so, we can't support "baseVertexLocation" in here
		// Error! OpenGL ES 2 has no instancing support!
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
	//[ Debug                                                 ]
	//[-------------------------------------------------------]
	bool OpenGLES2Renderer::isDebugEnabled()
	{
		// OpenGL ES 2 has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)

		// Debug disabled
		return false;
	}

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

		// Uniform buffer object (UBO, "constant buffer" in Direct3D terminology) supported?
		mCapabilities.uniformBuffer = false;

		// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
		mCapabilities.maximumTextureBufferSize = 0;

		// Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		mCapabilities.individualUniforms = true;

		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex, OpenGL ES 2 has no "GL_ARB_instanced_arrays" extension)
		mCapabilities.instancedArrays = false;

		// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID, OpenGL ES 2 has no "GL_ARB_draw_instanced" extension)
		mCapabilities.drawInstanced = false;

		// Base vertex supported for draw calls?
		mCapabilities.baseVertex = false;	// OpenGL ES 2 has no "GL_ARB_draw_elements_base_vertex" extension equivalent

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
			mOpenGLES2Program = static_cast<Program*>(program)->getOpenGLES2Program();

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
