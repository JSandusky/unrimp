/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/OpenGLDebug.h"	// For "OPENGLRENDERER_RENDERERMATCHCHECK_RETURN()"
#include "OpenGLRenderer/Mapping.h"
#include "OpenGLRenderer/BlendState.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/ShaderLanguageGlsl.h"
#include "OpenGLRenderer/ProgramGlsl.h"
#include "OpenGLRenderer/UniformBufferGlsl.h"
#include "OpenGLRenderer/SamplerStateSo.h"
#include "OpenGLRenderer/SamplerStateDsa.h"
#include "OpenGLRenderer/SamplerStateBind.h"
#include "OpenGLRenderer/Texture2DDsa.h"
#include "OpenGLRenderer/Texture2DBind.h"
#include "OpenGLRenderer/VertexArrayVao.h"
#include "OpenGLRenderer/VertexArrayNoVao.h"
#include "OpenGLRenderer/SwapChain.h"
#include "OpenGLRenderer/FramebufferDsa.h"
#include "OpenGLRenderer/FramebufferBind.h"
#include "OpenGLRenderer/IndexBufferDsa.h"
#include "OpenGLRenderer/IndexBufferBind.h"
#include "OpenGLRenderer/RasterizerState.h"
#include "OpenGLRenderer/VertexBufferDsa.h"
#include "OpenGLRenderer/VertexBufferBind.h"
#include "OpenGLRenderer/TextureBufferDsa.h"
#include "OpenGLRenderer/TextureBufferBind.h"
#include "OpenGLRenderer/Texture2DArrayDsa.h"
#include "OpenGLRenderer/Texture2DArrayBind.h"
#include "OpenGLRenderer/DepthStencilState.h"
#include "OpenGLRenderer/TextureCollection.h"
#include "OpenGLRenderer/SamplerStateCollection.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"
#ifdef WIN32
	#include "OpenGLRenderer/Windows/ContextWindows.h"
#elif defined LINUX
	#include "OpenGLRenderer/Linux//ContextLinux.h"
#endif
#ifndef OPENGLRENDERER_NO_CG
	#include "OpenGLRenderer/ProgramCg.h"
	#include "OpenGLRenderer/VertexArrayCg.h"
	#include "OpenGLRenderer/UniformBufferCg.h"
	#include "OpenGLRenderer/CgRuntimeLinking.h"
	#include "OpenGLRenderer/ShaderLanguageCg.h"
#endif


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef OPENGLRENDERER_EXPORTS
	#define OPENGLRENDERER_API_EXPORT GENERIC_API_EXPORT
#else
	#define OPENGLRENDERER_API_EXPORT
#endif
OPENGLRENDERER_API_EXPORT Renderer::IRenderer *createOpenGLRendererInstance(handle nativeWindowHandle)
{
	return new OpenGLRenderer::OpenGLRenderer(nativeWindowHandle);
}
#undef OPENGLRENDERER_API_EXPORT


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	OpenGLRenderer::OpenGLRenderer(handle nativeWindowHandle) :
		#ifdef WIN32
			mContext(new ContextWindows(nativeWindowHandle)),
		#elif defined LINUX
			mContext(new ContextLinux(nativeWindowHandle)),
		#else
			#error "Unsupported platform"
		#endif
		mShaderLanguageGlsl(nullptr),
		#ifndef OPENGLRENDERER_NO_CG
			mCgRuntimeLinking(new CgRuntimeLinking()),
			mShaderLanguageCg(nullptr),
		#endif
		mDefaultSamplerState(nullptr),
		mVertexArray(nullptr),
		mOpenGLPrimitiveTopology(0xFFFF),	// Unknown default setting
		mDefaultRasterizerState(nullptr),
		mRasterizerState(nullptr),
		mMainSwapChain(nullptr),
		mRenderTarget(nullptr),
		mDefaultDepthStencilState(nullptr),
		mDepthStencilState(nullptr),
		mDefaultBlendState(nullptr),
		mBlendState(nullptr),
		mOpenGLProgram(0)
	{
		// Is the context initialized?
		if (mContext->isInitialized())
		{
			// Create the default state objects
			mDefaultSamplerState		= createSamplerState(Renderer::ISamplerState::getDefaultSamplerState());
			mDefaultRasterizerState		= createRasterizerState(Renderer::IRasterizerState::getDefaultRasterizerState());
			mDefaultDepthStencilState	= createDepthStencilState(Renderer::IDepthStencilState::getDefaultDepthStencilState());
			mDefaultBlendState			= createBlendState(Renderer::IBlendState::getDefaultBlendState());

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
	}

	/**
	*  @brief
	*    Destructor
	*/
	OpenGLRenderer::~OpenGLRenderer()
	{
		// Set no vertex array reference, in case we have one
		if (nullptr != mVertexArray)
		{
			iaSetVertexArray(nullptr);
		}

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
		if (nullptr != mDefaultRasterizerState)
		{
			mDefaultRasterizerState->release();
			mDefaultRasterizerState = nullptr;
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

		// Set no rasterizer state reference, in case we have one
		if (nullptr != mRasterizerState)
		{
			rsSetState(nullptr);
		}

		// Set no depth stencil reference, in case we have one
		if (nullptr != mDepthStencilState)
		{
			omSetDepthStencilState(nullptr);
		}

		// Set no blend state reference, in case we have one
		if (nullptr != mBlendState)
		{
			omSetBlendState(nullptr);
		}

		{ // For debugging: At this point there should be no resource instances left, validate this!
			// -> Are the currently any resource instances?
			const unsigned long numberOfCurrentResources = getStatistics().getNumberOfCurrentResources();
			if (numberOfCurrentResources > 0)
			{
				// Error!
				if (numberOfCurrentResources > 1)
				{
					RENDERER_OUTPUT_DEBUG_PRINTF("OpenGL error: Renderer is going to be destroyed, but there are still %d resource instances left (memory leak)\n", numberOfCurrentResources)
				}
				else
				{
					RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: Renderer is going to be destroyed, but there is still one resource instance left (memory leak)\n")
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

		#ifndef OPENGLRENDERER_NO_CG
			// Release the Cg shader language instance, in case we have one
			if (nullptr != mShaderLanguageCg)
			{
				mShaderLanguageCg->release();
			}

			// Destroy the Cg runtime linking instance
			delete mCgRuntimeLinking;
		#endif

		// Destroy the OpenGL context instance
		delete mContext;
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	const char *OpenGLRenderer::getName() const
	{
		return "OpenGL";
	}

	bool OpenGLRenderer::isInitialized() const
	{
		// Is the context initialized?
		return mContext->isInitialized();
	}

	Renderer::ISwapChain *OpenGLRenderer::getMainSwapChain() const
	{
		return mMainSwapChain;
	}


	//[-------------------------------------------------------]
	//[ Shader language                                       ]
	//[-------------------------------------------------------]
	unsigned int OpenGLRenderer::getNumberOfShaderLanguages() const
	{
		unsigned int numberOfShaderLanguages = 0;

		// "GL_ARB_shader_objects" required
		if (mContext->getExtensions().isGL_ARB_shader_objects())
		{
			// GLSL supported
			++numberOfShaderLanguages;
		}

		#ifndef OPENGLRENDERER_NO_CG
			// Dynamically check on runtime whether or not Cg is available
			if (mCgRuntimeLinking->isCgAvaiable())
			{
				// Cg supported
				++numberOfShaderLanguages;
			}
		#endif

		// Done, return the number of supported shader languages
		return numberOfShaderLanguages;
	}

	const char *OpenGLRenderer::getShaderLanguageName(unsigned int index) const
	{
		unsigned int currentIndex = 0;

		// "GL_ARB_shader_objects" required
		if (mContext->getExtensions().isGL_ARB_shader_objects())
		{
			// GLSL supported
			if (currentIndex == index)
			{
				return ShaderLanguageGlsl::NAME;
			}
			++currentIndex;
		}

		#ifndef OPENGLRENDERER_NO_CG
			// Dynamically check on runtime whether or not Cg is available
			if (mCgRuntimeLinking->isCgAvaiable())
			{
				// Cg supported
				if (currentIndex == index)
				{
					return ShaderLanguageCg::NAME;
				}
				// ++currentIndex; // Not required
			}
		#endif

		// Error!
		return nullptr;
	}

	Renderer::IShaderLanguage *OpenGLRenderer::getShaderLanguage(const char *shaderLanguageName)
	{
		// "GL_ARB_shader_objects" required
		if (mContext->getExtensions().isGL_ARB_shader_objects())
		{
			// In case "shaderLanguage" is a null pointer, use the default shader language
			if (nullptr != shaderLanguageName)
			{
				// Optimization: Check for shader language name pointer match, first
				if (shaderLanguageName == ShaderLanguageGlsl::NAME || !stricmp(shaderLanguageName, ShaderLanguageGlsl::NAME))
				{
					// "GL_ARB_shader_objects" required
					if (mContext->getExtensions().isGL_ARB_shader_objects())
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
				}
				#ifndef OPENGLRENDERER_NO_CG
					else if (shaderLanguageName == ShaderLanguageCg::NAME || !stricmp(shaderLanguageName, ShaderLanguageCg::NAME))
					{
						// Dynamically check on runtime whether or not Cg is available
						if (mCgRuntimeLinking->isCgAvaiable())
						{
							// If required, create the Cg shader language instance right now
							if (nullptr == mShaderLanguageCg)
							{
								mShaderLanguageCg = new ShaderLanguageCg(*this);
								mShaderLanguageCg->addReference();	// Internal renderer reference
							}

							// Return the shader language instance
							return mShaderLanguageCg;
						}
					}
				#endif
			}
			else
			{
				// Return the GLSL shader language instance as default
				return getShaderLanguage(ShaderLanguageGlsl::NAME);
			}
		}

		// Error!
		return nullptr;
	}


	//[-------------------------------------------------------]
	//[ Resource creation                                     ]
	//[-------------------------------------------------------]
	Renderer::ISwapChain *OpenGLRenderer::createSwapChain(handle nativeWindowHandle)
	{
		// The provided native window handle must not be a null handle
		return (NULL_HANDLE != nativeWindowHandle) ? new SwapChain(*this, nativeWindowHandle) : nullptr;
	}

	Renderer::IFramebuffer *OpenGLRenderer::createFramebuffer(unsigned int numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture)
	{
		// "GL_ARB_framebuffer_object" required
		if (mContext->getExtensions().isGL_ARB_framebuffer_object())
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mContext->getExtensions().isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				// -> Validation is done inside the framebuffer implementation
				return new FramebufferDsa(*this, numberOfColorTextures, colorTextures, depthStencilTexture);
			}
			else
			{
				// Traditional bind version
				// -> Validation is done inside the framebuffer implementation
				return new FramebufferBind(*this, numberOfColorTextures, colorTextures, depthStencilTexture);
			}
		}
		else
		{
			// Error!
			return nullptr;
		}
	}

	Renderer::IVertexBuffer *OpenGLRenderer::createVertexBuffer(unsigned int numberOfBytes, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		// "GL_ARB_vertex_buffer_object" required
		if (mContext->getExtensions().isGL_ARB_vertex_buffer_object())
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mContext->getExtensions().isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				return new VertexBufferDsa(*this, numberOfBytes, data, bufferUsage);
			}
			else
			{
				// Traditional bind version
				return new VertexBufferBind(*this, numberOfBytes, data, bufferUsage);
			}
		}
		else
		{
			// Error!
			return nullptr;
		}
	}

	Renderer::IIndexBuffer *OpenGLRenderer::createIndexBuffer(unsigned int numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		// "GL_ARB_vertex_buffer_object" required
		if (mContext->getExtensions().isGL_ARB_vertex_buffer_object())
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mContext->getExtensions().isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				return new IndexBufferDsa(*this, numberOfBytes, indexBufferFormat, data, bufferUsage);
			}
			else
			{
				// Traditional bind version
				return new IndexBufferBind(*this, numberOfBytes, indexBufferFormat, data, bufferUsage);
			}
		}
		else
		{
			// Error!
			return nullptr;
		}
	}

	Renderer::ITextureBuffer *OpenGLRenderer::createTextureBuffer(unsigned int numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data, Renderer::BufferUsage::Enum bufferUsage)
	{
		// "GL_ARB_texture_buffer_object" required
		if (mContext->getExtensions().isGL_ARB_texture_buffer_object())
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mContext->getExtensions().isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				return new TextureBufferDsa(*this, numberOfBytes, textureFormat, data, bufferUsage);
			}
			else
			{
				// Traditional bind version
				return new TextureBufferBind(*this, numberOfBytes, textureFormat, data, bufferUsage);
			}
		}
		else
		{
			// Error!
			return nullptr;
		}
	}

	Renderer::ITexture2D *OpenGLRenderer::createTexture2D(unsigned int width, unsigned int height, Renderer::TextureFormat::Enum textureFormat, void *data, unsigned int flags, Renderer::TextureUsage::Enum)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL has no texture usage indication

		// Check whether or not the given texture dimension is valid
		if (width > 0 && height > 0)
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mContext->getExtensions().isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				return new Texture2DDsa(*this, width, height, textureFormat, data, flags);
			}
			else
			{
				// Traditional bind version
				return new Texture2DBind(*this, width, height, textureFormat, data, flags);
			}
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::ITexture2DArray *OpenGLRenderer::createTexture2DArray(unsigned int width, unsigned int height, unsigned int numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data, unsigned int flags, Renderer::TextureUsage::Enum)
	{
		// The indication of the texture usage is only relevant for Direct3D, OpenGL has no texture usage indication

		// Check whether or not the given texture dimension is valid, "GL_EXT_texture_array" required
		if (width > 0 && height > 0 && numberOfSlices > 0 && mContext->getExtensions().isGL_EXT_texture_array())
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mContext->getExtensions().isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)
				return new Texture2DArrayDsa(*this, width, height, numberOfSlices, textureFormat, data, flags);
			}
			else
			{
				// Traditional bind version
				return new Texture2DArrayBind(*this, width, height, numberOfSlices, textureFormat, data, flags);
			}
		}
		else
		{
			return nullptr;
		}
	}

	Renderer::IRasterizerState *OpenGLRenderer::createRasterizerState(const Renderer::RasterizerState &rasterizerState)
	{
		return new RasterizerState(*this, rasterizerState);
	}

	Renderer::IDepthStencilState *OpenGLRenderer::createDepthStencilState(const Renderer::DepthStencilState &depthStencilState)
	{
		return new DepthStencilState(*this, depthStencilState);
	}

	Renderer::IBlendState *OpenGLRenderer::createBlendState(const Renderer::BlendState &blendState)
	{
		return new BlendState(*this, blendState);
	}

	Renderer::ISamplerState *OpenGLRenderer::createSamplerState(const Renderer::SamplerState &samplerState)
	{
		// Is "GL_ARB_sampler_objects" there?
		if (mContext->getExtensions().isGL_ARB_sampler_objects())
		{
			// Effective sampler object (SO)
			return new SamplerStateSo(*this, samplerState);
		}
		else
		{
			// Is "GL_EXT_direct_state_access" there?
			if (mContext->getExtensions().isGL_EXT_direct_state_access())
			{
				// Direct state access (DSA) version to emulate a sampler object
				return new SamplerStateDsa(*this, samplerState);
			}
			else
			{
				// Traditional bind version to emulate a sampler object
				return new SamplerStateBind(*this, samplerState);
			}
		}
	}

	Renderer::ITextureCollection *OpenGLRenderer::createTextureCollection(unsigned int numberOfTextures, Renderer::ITexture **textures)
	{
		return new TextureCollection(*this, numberOfTextures, textures);
	}

	Renderer::ISamplerStateCollection *OpenGLRenderer::createSamplerStateCollection(unsigned int numberOfSamplerStates, Renderer::ISamplerState **samplerStates)
	{
		return new SamplerStateCollection(*this, numberOfSamplerStates, samplerStates);
	}


	//[-------------------------------------------------------]
	//[ Resource handling                                     ]
	//[-------------------------------------------------------]
	bool OpenGLRenderer::map(Renderer::IResource &, unsigned int, Renderer::MapType::Enum, unsigned int, Renderer::MappedSubresource &)
	{
		// TODO(co) Implement me
		return false;
	}

	void OpenGLRenderer::unmap(Renderer::IResource &, unsigned int)
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ States                                                ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::setProgram(Renderer::IProgram *program)
	{
		// TODO(co) Avoid changing already set program

		if (nullptr != program)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *program)

			// TODO(co) GLSL/Cg buffer settings, unset previous program

			// Evaluate the internal program type of the new program to set
			switch (static_cast<Program*>(program)->getInternalResourceType())
			{
				case Program::InternalResourceType::GLSL:
					// "GL_ARB_shader_objects" required
					if (mContext->getExtensions().isGL_ARB_shader_objects())
					{
						// Backup OpenGL program identifier
						mOpenGLProgram = static_cast<ProgramGlsl*>(program)->getOpenGLProgram();

						// Bind the program
						glUseProgramObjectARB(mOpenGLProgram);
					}
					break;

				#ifndef OPENGLRENDERER_NO_CG
					case Program::InternalResourceType::CG:
					{
						// Get the Cg program
						const CGprogram cgProgram = static_cast<ProgramCg*>(program)->getCgProgram();

						// Backup OpenGL program identifier
						mOpenGLProgram = cgGLGetProgramID(cgProgram);

						// Bind the Cg combined program
						cgGLBindProgram(cgProgram);

						// Iterate through all Cg programs of the Cg combined program and enable all required profiles
						const int numOfProgramDomains = cgGetNumProgramDomains(cgProgram);
						for (int i = 0; i < numOfProgramDomains; ++i)
						{
							// Enable the profile
							cgGLEnableProfile(cgGetProgramDomainProfile(cgProgram, i));
						}
						break;
					}
				#endif
			}
		}
		else
		{
			// TODO(co) GLSL/Cg buffer settings
			// "GL_ARB_shader_objects" required
			if (mContext->getExtensions().isGL_ARB_shader_objects())
			{
				// Unbind the program
				glUseProgramObjectARB(0);
			}

			// Reset OpenGL program identifier
			mOpenGLProgram = 0;
		}
	}


	//[-------------------------------------------------------]
	//[ Input-assembler (IA) stage                            ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::iaSetVertexArray(Renderer::IVertexArray *vertexArray)
	{
		// New vertex array?
		if (mVertexArray != vertexArray)
		{
			// Set a vertex array?
			if (nullptr != vertexArray)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *vertexArray)

				// Unset the currently used vertex array
				iaUnsetVertexArray();

				// Set new vertex array and add a reference to it
				mVertexArray = static_cast<VertexArray*>(vertexArray);
				mVertexArray->addReference();

				// Evaluate the internal array type type of the new vertex array to set
				switch (static_cast<VertexArray*>(mVertexArray)->getInternalResourceType())
				{
					case VertexArray::InternalResourceType::NO_VAO:
						// Enable OpenGL vertex attribute arrays
						static_cast<VertexArrayNoVao*>(mVertexArray)->enableOpenGLVertexAttribArrays();
						break;

					#ifndef OPENGLRENDERER_NO_CG
						case VertexArray::InternalResourceType::CG:
							// Enable OpenGL vertex attribute arrays
							static_cast<VertexArrayCg*>(mVertexArray)->enableOpenGLVertexAttribArrays();
							break;
					#endif

					case VertexArray::InternalResourceType::VAO:
						// Bind OpenGL vertex array
						glBindVertexArray(static_cast<VertexArrayVao*>(mVertexArray)->getOpenGLVertexArray());
						break;
				}
			}
			else
			{
				// Unset the currently used vertex array
				iaUnsetVertexArray();
			}
		}
	}

	void OpenGLRenderer::iaSetPrimitiveTopology(Renderer::PrimitiveTopology::Enum primitiveTopology)
	{
		// Tessellation support: Up to 32 vertices per patch are supported "Renderer::PrimitiveTopology::PATCH_LIST_1" ... "Renderer::PrimitiveTopology::PATCH_LIST_32"
		if (primitiveTopology >= Renderer::PrimitiveTopology::PATCH_LIST_1)
		{
			// Use tessellation

			// Get number of vertices that will be used to make up a single patch primitive
			// -> There's no need to check for the "GL_ARB_tessellation_shader" extension, it's there if "Renderer::Capabilities::maximumNumberOfPatchVertices" is not 0
			const GLint numberOfVerticesPerPatch = primitiveTopology - Renderer::PrimitiveTopology::PATCH_LIST_1 + 1;
			if (numberOfVerticesPerPatch <= static_cast<GLint>(mCapabilities.maximumNumberOfPatchVertices))
			{
				// Set number of vertices that will be used to make up a single patch primitive
				glPatchParameteri(GL_PATCH_VERTICES, numberOfVerticesPerPatch);

				// Set OpenGL primitive topology
				mOpenGLPrimitiveTopology = GL_PATCHES;
			}
			else
			{
				// Error!
			}
		}
		else
		{
			// Do not use tessellation

			// Map and backup the set OpenGL primitive topology
			mOpenGLPrimitiveTopology = Mapping::getOpenGLType(primitiveTopology);
		}
	}


	//[-------------------------------------------------------]
	//[ Vertex-shader (VS) stage                              ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::vsSetTexture(unsigned int unit, Renderer::ITexture *texture)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetTexture(unit, texture);
	}

	void OpenGLRenderer::vsSetTextureCollection(unsigned int startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetTextureCollection(startUnit, textureCollection);
	}

	void OpenGLRenderer::vsSetSamplerState(unsigned int unit, Renderer::ISamplerState *samplerState)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetSamplerState(unit, samplerState);
	}

	void OpenGLRenderer::vsSetSamplerStateCollection(unsigned int startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetSamplerStateCollection(startUnit, samplerStateCollection);
	}

	void OpenGLRenderer::vsSetUniformBuffer(unsigned int slot, Renderer::IUniformBuffer *uniformBuffer)
	{
		// In OpenGL a single program is set instead of individual shaders
		// -> This means that there are uniform binding slots per program, not slots per shader as in e.g. Direct3D 10
		// -> Reuse the fragment shader version of this method
		fsSetUniformBuffer(slot, uniformBuffer);
	}


	//[-------------------------------------------------------]
	//[ Tessellation-control-shader (TCS) stage               ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::tcsSetTexture(unsigned int unit, Renderer::ITexture *texture)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetTexture(unit, texture);
	}

	void OpenGLRenderer::tcsSetTextureCollection(unsigned int startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetTextureCollection(startUnit, textureCollection);
	}

	void OpenGLRenderer::tcsSetSamplerState(unsigned int unit, Renderer::ISamplerState *samplerState)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetSamplerState(unit, samplerState);
	}

	void OpenGLRenderer::tcsSetSamplerStateCollection(unsigned int startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetSamplerStateCollection(startUnit, samplerStateCollection);
	}

	void OpenGLRenderer::tcsSetUniformBuffer(unsigned int slot, Renderer::IUniformBuffer *uniformBuffer)
	{
		// In OpenGL a single program is set instead of individual shaders
		// -> This means that there are uniform binding slots per program, not slots per shader as in e.g. Direct3D 10
		// -> Reuse the fragment shader version of this method
		fsSetUniformBuffer(slot, uniformBuffer);
	}


	//[-------------------------------------------------------]
	//[ Tessellation-evaluation-shader (TES) stage            ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::tesSetTexture(unsigned int unit, Renderer::ITexture *texture)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetTexture(unit, texture);
	}

	void OpenGLRenderer::tesSetTextureCollection(unsigned int startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetTextureCollection(startUnit, textureCollection);
	}

	void OpenGLRenderer::tesSetSamplerState(unsigned int unit, Renderer::ISamplerState *samplerState)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetSamplerState(unit, samplerState);
	}

	void OpenGLRenderer::tesSetSamplerStateCollection(unsigned int startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetSamplerStateCollection(startUnit, samplerStateCollection);
	}

	void OpenGLRenderer::tesSetUniformBuffer(unsigned int slot, Renderer::IUniformBuffer *uniformBuffer)
	{
		// In OpenGL a single program is set instead of individual shaders
		// -> This means that there are uniform binding slots per program, not slots per shader as in e.g. Direct3D 10
		// -> Reuse the fragment shader version of this method
		fsSetUniformBuffer(slot, uniformBuffer);
	}


	//[-------------------------------------------------------]
	//[ Geometry-shader (GS) stage                            ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::gsSetTexture(unsigned int unit, Renderer::ITexture *texture)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetTexture(unit, texture);
	}

	void OpenGLRenderer::gsSetTextureCollection(unsigned int startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetTextureCollection(startUnit, textureCollection);
	}

	void OpenGLRenderer::gsSetSamplerState(unsigned int unit, Renderer::ISamplerState *samplerState)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetSamplerState(unit, samplerState);
	}

	void OpenGLRenderer::gsSetSamplerStateCollection(unsigned int startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// In OpenGL, all shaders share the same texture units
		fsSetSamplerStateCollection(startUnit, samplerStateCollection);
	}

	void OpenGLRenderer::gsSetUniformBuffer(unsigned int slot, Renderer::IUniformBuffer *uniformBuffer)
	{
		// In OpenGL a single program is set instead of individual shaders
		// -> This means that there are uniform binding slots per program, not slots per shader as in e.g. Direct3D 10
		// -> Reuse the fragment shader version of this method
		fsSetUniformBuffer(slot, uniformBuffer);
	}


	//[-------------------------------------------------------]
	//[ Rasterizer (RS) stage                                 ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::rsSetViewports(unsigned int numberOfViewports, const Renderer::Viewport *viewports)
	{
		// Are the given viewports valid?
		if (numberOfViewports > 0 && nullptr != viewports)
		{
			// In OpenGL, the origin of the viewport is left bottom while Direct3D is using a left top origin. To make the
			// Direct3D 11 implementation as efficient as possible the Direct3D convention is used and we have to convert in here.

			// Get the width and height of the current render target
			unsigned int renderTargetWidth =  0;
			unsigned int renderTargetHeight = 0;
			if (nullptr != mRenderTarget)
			{
				mRenderTarget->getWidthAndHeight(renderTargetWidth, renderTargetHeight);
			}

			// Set the OpenGL viewport
			// TODO(co) "GL_ARB_viewport_array" support ("OpenGLRenderer::rsSetViewports()")
			// TODO(co) Check for "numberOfViewports" out of range or are the debug events good enough?
		#ifndef RENDERER_NO_DEBUG
			if (numberOfViewports > 1)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: OpenGL supports only one viewport")
			}
		#endif
			glViewport(static_cast<GLint>(viewports->topLeftX), static_cast<GLint>(renderTargetHeight - viewports->topLeftY - viewports->height), static_cast<GLsizei>(viewports->width), static_cast<GLsizei>(viewports->height));
			glDepthRange(static_cast<GLclampf>(viewports->minDepth), static_cast<GLclampf>(viewports->maxDepth));
		}
	}

	void OpenGLRenderer::rsSetScissorRectangles(unsigned int numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles)
	{
		// Are the given scissor rectangles valid?
		if (numberOfScissorRectangles > 0 && nullptr != scissorRectangles)
		{
			// In OpenGL, the origin of the scissor rectangle is left bottom while Direct3D is using a left top origin. To make the
			// Direct3D 9 & 10 & 11 implementation as efficient as possible the Direct3D convention is used and we have to convert in here.

			// Get the width and height of the current render target
			unsigned int renderTargetWidth =  0;
			unsigned int renderTargetHeight = 0;
			if (nullptr != mRenderTarget)
			{
				mRenderTarget->getWidthAndHeight(renderTargetWidth, renderTargetHeight);
			}

			// Set the OpenGL scissor rectangle
			// TODO(co) "GL_ARB_viewport_array" support ("OpenGLRenderer::rsSetViewports()")
			// TODO(co) Check for "numberOfViewports" out of range or are the debug events good enough?
		#ifndef RENDERER_NO_DEBUG
			if (numberOfScissorRectangles > 1)
			{
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL error: OpenGL supports only one scissor rectangle")
			}
		#endif
			const GLsizei width  = scissorRectangles->bottomRightX - scissorRectangles->topLeftX;
			const GLsizei height = scissorRectangles->bottomRightY - scissorRectangles->topLeftY;
			glScissor(static_cast<GLint>(scissorRectangles->topLeftX), static_cast<GLint>(renderTargetHeight - scissorRectangles->topLeftY - height), width, height);
		}
	}

	void OpenGLRenderer::rsSetState(Renderer::IRasterizerState *rasterizerState)
	{
		// New rasterizer state?
		if (mRasterizerState != rasterizerState)
		{
			// Set a rasterizer state?
			if (nullptr != rasterizerState)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *rasterizerState)

				// Release the rasterizer state reference, in case we have one
				if (nullptr != mRasterizerState)
				{
					mRasterizerState->release();
				}

				// Set new rasterizer state and add a reference to it
				mRasterizerState = static_cast<RasterizerState*>(rasterizerState);
				mRasterizerState->addReference();

				// Set the OpenGL rasterizer states
				// TODO(co) Reduce state changes: Maybe it's a good idea to have alternative methods allowing to pass through the previous states and then performing per-state-change-comparison in order to reduce graphics-API calls
				mRasterizerState->setOpenGLRasterizerStates();
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
	void OpenGLRenderer::fsSetTexture(unsigned int unit, Renderer::ITexture *texture)
	{
		// Is "GL_EXT_direct_state_access" there?
		if (mContext->getExtensions().isGL_EXT_direct_state_access())
		{
			// Effective direct state access (DSA)

			// GL_TEXTURE0_ARB is the first texture unit, while nUnit we received is zero based
			unit += GL_TEXTURE0_ARB;

			// TODO(co) Some security checks might be wise *maximum number of texture units*
			// Set a texture at that unit?
			if (nullptr != texture)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)

				// Evaluate the texture type
				switch (texture->getResourceType())
				{
					case Renderer::ResourceType::TEXTURE_BUFFER:
						glBindMultiTextureEXT(unit, GL_TEXTURE_BUFFER_ARB, static_cast<TextureBuffer*>(texture)->getOpenGLTexture());
						break;

					case Renderer::ResourceType::TEXTURE_2D:
						glBindMultiTextureEXT(unit, GL_TEXTURE_2D, static_cast<Texture2D*>(texture)->getOpenGLTexture());
						break;

					case Renderer::ResourceType::TEXTURE_2D_ARRAY:
						// No extension check required, if we in here we already know it must exist
						glBindMultiTextureEXT(unit, GL_TEXTURE_2D_ARRAY_EXT, static_cast<Texture2DArray*>(texture)->getOpenGLTexture());
						break;

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
				// Unbind the texture at the given texture unit
				glBindMultiTextureEXT(unit, GL_TEXTURE_2D, 0);
			}
		}
		else
		{
			// Traditional bind version

			// "GL_ARB_multitexture" required
			if (mContext->getExtensions().isGL_ARB_multitexture())
			{
				// Set a texture at that unit?
				if (nullptr != texture)
				{
					// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
					OPENGLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *texture)

					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						// Backup the currently active OpenGL texture
						GLint openGLActiveTextureBackup = 0;
						glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLActiveTextureBackup);
					#endif

					// TODO(co) Some security checks might be wise *maximum number of texture units*
					// Activate the texture unit we want to manipulate
					glActiveTextureARB(GL_TEXTURE0_ARB + unit);

					// Evaluate the resource type
					switch (texture->getResourceType())
					{
						case Renderer::ResourceType::TEXTURE_BUFFER:
							glBindTexture(GL_TEXTURE_BUFFER_ARB, static_cast<TextureBuffer*>(texture)->getOpenGLTexture());
							break;

						case Renderer::ResourceType::TEXTURE_2D:
							glBindTexture(GL_TEXTURE_2D, static_cast<Texture2D*>(texture)->getOpenGLTexture());
							break;

						case Renderer::ResourceType::TEXTURE_2D_ARRAY:
							// No extension check required, if we in here we already know it must exist
							glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, static_cast<Texture2DArray*>(texture)->getOpenGLTexture());
							break;

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

					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						// Be polite and restore the previous active OpenGL texture
						glActiveTextureARB(openGLActiveTextureBackup);
					#endif
				}
				else
				{
					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						// Backup the currently active OpenGL texture
						GLint openGLActiveTextureBackup = 0;
						glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLActiveTextureBackup);
					#endif

					// TODO(co) Some security checks might be wise *maximum number of texture units*
					// Activate the texture unit we want to manipulate
					glActiveTextureARB(GL_TEXTURE0_ARB + unit);

					// Unbind the texture at the given texture unit
					glBindTexture(GL_TEXTURE_2D, 0);

					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						// Be polite and restore the previous active OpenGL texture
						glActiveTextureARB(openGLActiveTextureBackup);
					#endif
				}
			}
		}
	}

	void OpenGLRenderer::fsSetTextureCollection(unsigned int startUnit, Renderer::ITextureCollection *textureCollection)
	{
		// Is the given texture collection valid?
		if (nullptr != textureCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *textureCollection)

			// Is "GL_EXT_direct_state_access" there?
			if (mContext->getExtensions().isGL_EXT_direct_state_access())
			{
				// Effective direct state access (DSA)

				// Loop through all textures within the given texture collection
				// -> "GL_TEXTURE0_ARB" is the first texture unit, while nUnit we received is zero based
				Renderer::ITexture **currentTexture = static_cast<TextureCollection*>(textureCollection)->getTextures();
				Renderer::ITexture **textureEnd	    = currentTexture + static_cast<TextureCollection*>(textureCollection)->getNumberOfTextures();
				for (GLenum unit = static_cast<GLenum>(GL_TEXTURE0_ARB + startUnit); currentTexture < textureEnd; ++currentTexture, ++unit)
				{
					// Get the current texture
					Renderer::ITexture *texture = *currentTexture;

					// TODO(co) Some security checks might be wise *maximum number of texture units*
					// Set a texture at that unit?
					if (nullptr != texture)
					{
						// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
						// -> Not required in here, this is already done within the texture collection

						// Evaluate the texture type
						switch (texture->getResourceType())
						{
							case Renderer::ResourceType::TEXTURE_BUFFER:
								glBindMultiTextureEXT(unit, GL_TEXTURE_BUFFER_ARB, static_cast<TextureBuffer*>(texture)->getOpenGLTexture());
								break;

							case Renderer::ResourceType::TEXTURE_2D:
								glBindMultiTextureEXT(unit, GL_TEXTURE_2D, static_cast<Texture2D*>(texture)->getOpenGLTexture());
								break;

							case Renderer::ResourceType::TEXTURE_2D_ARRAY:
								// No extension check required, if we in here we already know it must exist
								glBindMultiTextureEXT(unit, GL_TEXTURE_2D_ARRAY_EXT, static_cast<Texture2DArray*>(texture)->getOpenGLTexture());
								break;

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
						// Unbind the texture at the given texture unit
						glBindMultiTextureEXT(unit, GL_TEXTURE_2D, 0);
					}
				}
			}
			else
			{
				// Traditional bind version

				// "GL_ARB_multitexture" required
				if (mContext->getExtensions().isGL_ARB_multitexture())
				{
					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						// Backup the currently active OpenGL texture
						GLint openGLActiveTextureBackup = 0;
						glGetIntegerv(GL_ACTIVE_TEXTURE, &openGLActiveTextureBackup);
					#endif

					// Loop through all textures within the given texture collection
					// -> GL_TEXTURE0_ARB is the first texture unit, while nUnit we received is zero based
					Renderer::ITexture **currentTexture = static_cast<TextureCollection*>(textureCollection)->getTextures();
					Renderer::ITexture **textureEnd	    = currentTexture + static_cast<TextureCollection*>(textureCollection)->getNumberOfTextures();
					for (GLenum unit = static_cast<GLenum>(GL_TEXTURE0_ARB + startUnit); currentTexture < textureEnd; ++currentTexture, ++unit)
					{
						// Get the current texture
						Renderer::ITexture *texture = *currentTexture;

						// TODO(co) Some security checks might be wise *maximum number of texture units*
						glActiveTextureARB(unit);

						// Set a texture at that unit?
						if (nullptr != texture)
						{
							// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
							// -> Not required in here, this is already done within the texture collection

							// Evaluate the texture
							switch (texture->getResourceType())
							{
								case Renderer::ResourceType::TEXTURE_BUFFER:
									glBindTexture(GL_TEXTURE_BUFFER_ARB, static_cast<TextureBuffer*>(texture)->getOpenGLTexture());
									break;

								case Renderer::ResourceType::TEXTURE_2D:
									glBindTexture(GL_TEXTURE_2D, static_cast<Texture2D*>(texture)->getOpenGLTexture());
									break;

								case Renderer::ResourceType::TEXTURE_2D_ARRAY:
									// No extension check required, if we in here we already know it must exist
									glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, static_cast<Texture2DArray*>(texture)->getOpenGLTexture());
									break;

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
							// Unbind the texture at the given texture unit
							glBindTexture(GL_TEXTURE_2D, 0);
						}
					}

					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						// Be polite and restore the previous active OpenGL texture
						glActiveTextureARB(openGLActiveTextureBackup);
					#endif
				}
			}
		}
	}

	void OpenGLRenderer::fsSetSamplerState(unsigned int unit, Renderer::ISamplerState *samplerState)
	{
		// Set a sampler state at that unit?
		if (nullptr != samplerState)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerState)

			// Is "GL_ARB_sampler_objects" there?
			if (mContext->getExtensions().isGL_ARB_sampler_objects())
			{
				// Effective sampler object (SO)
				glBindSampler(unit, static_cast<SamplerStateSo*>(samplerState)->getOpenGLSampler());
			}
			else
			{
				// TODO(co) Activate texture and so on

				// Is "GL_EXT_direct_state_access" there?
				if (mContext->getExtensions().isGL_EXT_direct_state_access())
				{
					// Direct state access (DSA) version to emulate a sampler object
					static_cast<SamplerStateDsa*>(samplerState)->setOpenGLSamplerStates();
				}
				else
				{
					// Traditional bind version to emulate a sampler object
					static_cast<SamplerStateBind*>(samplerState)->setOpenGLSamplerStates();
				}
			}
		}
		else
		{
			// Is "GL_ARB_sampler_objects" there?
			if (mContext->getExtensions().isGL_ARB_sampler_objects())
			{
				// Effective sampler object (SO)
				glBindSampler(unit, 0);
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

					// TODO(co) Implement me
				}
			}
		}
	}

	void OpenGLRenderer::fsSetSamplerStateCollection(unsigned int startUnit, Renderer::ISamplerStateCollection *samplerStateCollection)
	{
		// Is the given sampler state collection valid?
		if (nullptr != samplerStateCollection)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *samplerStateCollection)

			// Is "GL_ARB_sampler_objects" there?
			if (mContext->getExtensions().isGL_ARB_sampler_objects())
			{
				// Effective sampler object (SO)

				// Loop through all sampler states within the given sampler state collection
				Renderer::ISamplerState **currentSamplerState = static_cast<SamplerStateCollection*>(samplerStateCollection)->getSamplerStates();
				Renderer::ISamplerState **samplerStateEnd	  = currentSamplerState + static_cast<SamplerStateCollection*>(samplerStateCollection)->getNumberOfSamplerStates();
				for (unsigned int unit = startUnit; currentSamplerState < samplerStateEnd; ++currentSamplerState, ++unit)
				{
					// Get the current sampler state
					Renderer::ISamplerState *samplerState = *currentSamplerState;

					// Set a sampler state at that unit?
					glBindSampler(unit, (nullptr != samplerState) ? static_cast<SamplerStateSo*>(samplerState)->getOpenGLSampler() : 0);
				}
			}
			else
			{
				// Loop through all sampler states within the given sampler state collection
				Renderer::ISamplerState **currentSamplerState = static_cast<SamplerStateCollection*>(samplerStateCollection)->getSamplerStates();
				Renderer::ISamplerState **samplerStateEnd	  = currentSamplerState + static_cast<SamplerStateCollection*>(samplerStateCollection)->getNumberOfSamplerStates();
				for (unsigned int unit = startUnit; currentSamplerState < samplerStateEnd; ++currentSamplerState, ++unit)
				{
					// Get the current sampler state
					Renderer::ISamplerState *samplerState = *currentSamplerState;

					// TODO(co) Activate texture and so on

					// Set a sampler state at that unit?
					if (nullptr != samplerState)
					{
						// Is "GL_EXT_direct_state_access" there?
						if (mContext->getExtensions().isGL_EXT_direct_state_access())
						{
							// Direct state access (DSA) version to emulate a sampler object
							static_cast<SamplerStateDsa*>(samplerState)->setOpenGLSamplerStates();
						}
						else
						{
							// Traditional bind version to emulate a sampler object
							static_cast<SamplerStateBind*>(samplerState)->setOpenGLSamplerStates();
						}
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

							// TODO(co) Implement me
						}
					}
				}
			}
		}
	}

	void OpenGLRenderer::fsSetUniformBuffer(unsigned int slot, Renderer::IUniformBuffer *uniformBuffer)
	{
		// Set a uniform buffer at that unit?
		if (nullptr != uniformBuffer)
		{
			// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
			OPENGLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *uniformBuffer)

			// Evaluate the internal uniform buffer type of the new uniform buffer to set
			switch (static_cast<UniformBuffer*>(uniformBuffer)->getInternalResourceType())
			{
				case UniformBuffer::InternalResourceType::GLSL:
					// "GL_ARB_uniform_buffer_object" required
					if (mContext->getExtensions().isGL_ARB_uniform_buffer_object())
					{
						// Attach the buffer to the given UBO binding point
						// -> Explicit binding points ("layout(binding=0)" in GLSL shader) requires OpenGL 4.2
						// -> Direct3D 10 and Direct3D 11 have explicit binding points
						// -> TODO(co) For now, nStartSlot is set from outside to the uniform block index which is directly used as binding point - Review this, maybe there's a better cross-API solution
						glBindBufferBaseEXT(GL_UNIFORM_BUFFER, slot, uniformBuffer ? static_cast<UniformBufferGlsl*>(uniformBuffer)->getOpenGLUniformBuffer() : 0);

						// Associate the uniform block with the given binding point
						glUniformBlockBinding(mOpenGLProgram, slot, slot);
					}
					break;

				#ifndef OPENGLRENDERER_NO_CG
					case UniformBuffer::InternalResourceType::CG:
						// Attach the buffer to the given UBO binding point
						// -> Explicit binding points ("layout(binding=0)" in GLSL shader) requires OpenGL 4.2
						// -> Direct3D 10 and Direct3D 11 have explicit binding points
						// -> TODO(co) For now, nStartSlot is set from outside to the uniform block index which is directly used as binding point - Review this, maybe there's a better cross-API solution

						// TODO(co) GLSL/Cg buffer settings
						//glBindBufferBaseEXT(GL_UNIFORM_BUFFER, slot, uniformBuffer ? static_cast<UniformBufferGlsl*>(uniformBuffer)->getOpenGLUniformBuffer() : 0);
						// cgSetProgramBuffer(cgGetParameterProgram(mCgParameter), cgGetParameterBufferIndex(mCgParameter), static_cast<UniformBufferCg*>(uniformBuffer)->getCgBuffer());

						// Associate the uniform block with the given binding point
						glUniformBlockBinding(mOpenGLProgram, slot, slot);
						break;
				#endif
			}
		}
		else
		{
			// TODO(co) Implement me
		}
	}


	//[-------------------------------------------------------]
	//[ Output-merger (OM) stage                              ]
	//[-------------------------------------------------------]
	Renderer::IRenderTarget *OpenGLRenderer::omGetRenderTarget()
	{
		return mRenderTarget;
	}

	void OpenGLRenderer::omSetRenderTarget(Renderer::IRenderTarget *renderTarget)
	{
		// New render target?
		if (mRenderTarget != renderTarget)
		{
			// Set a render target?
			if (nullptr != renderTarget)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *renderTarget)

				// Release the render target reference, in case we have one
				if (nullptr != mRenderTarget)
				{
					// Unbind OpenGL framebuffer?
					if (Renderer::ResourceType::FRAMEBUFFER == mRenderTarget->getResourceType() && Renderer::ResourceType::FRAMEBUFFER != renderTarget->getResourceType())
					{
						// We do not render into a OpenGL framebuffer
						glBindFramebuffer(GL_FRAMEBUFFER, 0);
					}

					// Release
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
					#ifdef LINUX
						// TODO(sw) put this handling into Context? (e.g. Context::makeCurrent(Renderer::ISwapChain::getNativeWindowHandle()))
						ContextLinux *context = static_cast<ContextLinux*>(mContext);
						Display *display = context->getDisplay();
						Renderer::ISwapChain *chain = static_cast<Renderer::ISwapChain*>(mRenderTarget);
						glXMakeCurrent(context->getDisplay(), chain->getNativeWindowHandle(), context->getRenderContext());
					#endif
						// TODO(co) Implement me
						break;
					}

					case Renderer::ResourceType::FRAMEBUFFER:
					{
						// Get the OpenGL framebuffer instance
						Framebuffer *framebuffer = static_cast<Framebuffer*>(mRenderTarget);

						// Bind the OpenGL framebuffer
						glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->getOpenGLFramebuffer());

						// Define the OpenGL buffers to draw into, "GL_ARB_draw_buffers"-extension required
						if (mContext->getExtensions().isGL_ARB_draw_buffers())
						{
							static const GLenum OPENGL_DRAW_BUFFER[16] =
							{
								GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1,  GL_COLOR_ATTACHMENT2,  GL_COLOR_ATTACHMENT3,
								GL_COLOR_ATTACHMENT4,  GL_COLOR_ATTACHMENT5,  GL_COLOR_ATTACHMENT6,  GL_COLOR_ATTACHMENT7,
								GL_COLOR_ATTACHMENT8,  GL_COLOR_ATTACHMENT9,  GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
								GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15
							};
							glDrawBuffersARB(static_cast<GLsizei>(framebuffer->getNumberOfColorTextures()), OPENGL_DRAW_BUFFER);
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
				// Evaluate the render target type
				if (Renderer::ResourceType::FRAMEBUFFER == mRenderTarget->getResourceType())
				{
					// We do not render into a OpenGL framebuffer
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

	void OpenGLRenderer::omSetDepthStencilState(Renderer::IDepthStencilState *depthStencilState)
	{
		// New depth stencil state?
		if (mDepthStencilState != depthStencilState)
		{
			// Set a depth stencil state?
			if (nullptr != depthStencilState)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *depthStencilState)

				// Release the depth stencil state reference, in case we have one
				if (nullptr != mDepthStencilState)
				{
					mDepthStencilState->release();
				}

				// Set new depth stencil state and add a reference to it
				mDepthStencilState = static_cast<DepthStencilState*>(depthStencilState);
				mDepthStencilState->addReference();

				// Set the OpenGL depth stencil states
				// TODO(co) Reduce state changes: Maybe it's a good idea to have alternative methods allowing to pass through the previous states and then performing per-state-change-comparison in order to reduce graphics-API calls
				mDepthStencilState->setOpenGLDepthStencilStates();
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

	void OpenGLRenderer::omSetBlendState(Renderer::IBlendState *blendState)
	{
		// New blend state?
		if (mBlendState != blendState)
		{
			// Set a blend state?
			if (nullptr != blendState)
			{
				// Security check: Is the given resource owned by this renderer? (calls "return" in case of a mismatch)
				OPENGLRENDERER_RENDERERMATCHCHECK_RETURN(*this, *blendState)

				// Release the blend state reference, in case we have one
				if (nullptr != mBlendState)
				{
					mBlendState->release();
				}

				// Set new blend state and add a reference to it
				mBlendState = static_cast<BlendState*>(blendState);
				mBlendState->addReference();

				// Set the OpenGL blend states
				// TODO(co) Reduce state changes: Maybe it's a good idea to have alternative methods allowing to pass through the previous states and then performing per-state-change-comparison in order to reduce graphics-API calls
				mBlendState->setOpenGLBlendStates();
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
	void OpenGLRenderer::clear(unsigned int flags, const float color[4], float z, unsigned int stencil)
	{
		// Get API flags
		unsigned int flagsApi = 0;
		if (flags & Renderer::ClearFlag::COLOR)
		{
			flagsApi |= GL_COLOR_BUFFER_BIT;
		}
		if (flags & Renderer::ClearFlag::DEPTH)
		{
			flagsApi |= GL_DEPTH_BUFFER_BIT;
		}
		if (flags & Renderer::ClearFlag::STENCIL)
		{
			flagsApi |= GL_STENCIL_BUFFER_BIT;
		}

		// Are API flags set?
		if (0 != flagsApi)
		{
			// Set clear settings
			if (flags & Renderer::ClearFlag::COLOR)
			{
				glClearColor(color[0], color[1], color[2], color[3]);
			}
			if (flags & Renderer::ClearFlag::DEPTH)
			{
				glClearDepth(z);
			}
			if (flags & Renderer::ClearFlag::STENCIL)
			{
				glClearStencil(static_cast<GLint>(stencil));
			}

			// Unlike OpenGL, when using Direct3D 10 & 11 the scissor rectangle(s) do not affect the clear operation
			// -> We have to compensate the OpenGL behaviour in here

			// Disable OpenGL scissor test, in case it's not disabled, yet
			if (mRasterizerState->getRasterizerState().scissorEnable)
			{
				glDisable(GL_SCISSOR_TEST);
			}

			// Clear
			glClear(flagsApi);

			// Restore the previously set OpenGL viewport
			if (mRasterizerState->getRasterizerState().scissorEnable)
			{
				glEnable(GL_SCISSOR_TEST);
			}
		}
	}

	bool OpenGLRenderer::beginScene()
	{
		// Not required when using OpenGL

		// Done
		return true;
	}

	void OpenGLRenderer::endScene()
	{
		// Not required when using OpenGL
	}


	//[-------------------------------------------------------]
	//[ Draw call                                             ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::draw(unsigned int startVertexLocation, unsigned int numberOfVertices)
	{
		// Tessellation support: "glPatchParameteri()" is called within "OpenGLRenderer::iaSetPrimitiveTopology()"

		// Is currently a vertex array set?
		if (nullptr != mVertexArray)
		{
			// Draw
			glDrawArrays(mOpenGLPrimitiveTopology, static_cast<GLint>(startVertexLocation), static_cast<GLsizei>(numberOfVertices));
		}
	}

	void OpenGLRenderer::drawInstanced(unsigned int startVertexLocation, unsigned int numberOfVertices, unsigned int numberOfInstances)
	{
		// Tessellation support: "glPatchParameteri()" is called within "OpenGLRenderer::iaSetPrimitiveTopology()"

		// Is currently a vertex array set? Do also check whether or not the required "GL_ARB_draw_instanced" extension is there.
		if (nullptr != mVertexArray && mContext->getExtensions().isGL_ARB_draw_instanced())
		{
			// Draw
			glDrawArraysInstancedARB(mOpenGLPrimitiveTopology, static_cast<GLint>(startVertexLocation), static_cast<GLsizei>(numberOfVertices), static_cast<GLsizei>(numberOfInstances));
		}
	}

	void OpenGLRenderer::drawIndexed(unsigned int startIndexLocation, unsigned int numberOfIndices, unsigned int baseVertexLocation, unsigned int, unsigned int)
	{
		// Tessellation support: "glPatchParameteri()" is called within "OpenGLRenderer::iaSetPrimitiveTopology()"

		// "minimumIndex" & "numberOfVertices" are currently not used, might be used later on with the "GL_EXT_draw_range_elements"-extension if it's useful

		// Is currently an vertex array set?
		if (nullptr != mVertexArray)
		{
			// Get the used index buffer
			IndexBuffer *indexBuffer = mVertexArray->getIndexBuffer();
			if (nullptr != indexBuffer)
			{
				// Use base vertex location?
				if (baseVertexLocation > 0)
				{
					// Is the "GL_ARB_draw_elements_base_vertex" extension there?
					if (mContext->getExtensions().isGL_ARB_draw_elements_base_vertex())
					{
						// Draw with base vertex location
						glDrawElementsBaseVertex(mOpenGLPrimitiveTopology, static_cast<GLsizei>(numberOfIndices), indexBuffer->getOpenGLType(), reinterpret_cast<const GLvoid*>(startIndexLocation * sizeof(int)), static_cast<GLint>(baseVertexLocation));
					}
					else
					{
						// Error!
					}
				}
				else
				{
					// Draw without base vertex location
					glDrawElements(mOpenGLPrimitiveTopology, static_cast<GLsizei>(numberOfIndices), indexBuffer->getOpenGLType(), reinterpret_cast<const GLvoid*>(startIndexLocation * sizeof(int)));
				}
			}
		}
	}

	void OpenGLRenderer::drawIndexedInstanced(unsigned int startIndexLocation, unsigned int numberOfIndices, unsigned int baseVertexLocation, unsigned int, unsigned int, unsigned int numberOfInstances)
	{
		// Tessellation support: "glPatchParameteri()" is called within "OpenGLRenderer::iaSetPrimitiveTopology()"

		// "minimumIndex" & "numberOfVertices" are currently not used, might be used later on with the "GL_EXT_draw_range_elements"-extension if it's useful

		// Is currently an vertex array set? Do also check for the required "GL_ARB_draw_instanced" extension.
		if (nullptr != mVertexArray && mContext->getExtensions().isGL_ARB_draw_instanced())
		{
			// Get the used index buffer
			IndexBuffer *indexBuffer = mVertexArray->getIndexBuffer();
			if (nullptr != indexBuffer)
			{
				// Use base vertex location?
				if (baseVertexLocation > 0)
				{
					// Is the "GL_ARB_draw_elements_base_vertex" extension there?
					if (mContext->getExtensions().isGL_ARB_draw_elements_base_vertex())
					{
						// Draw with base vertex location
						glDrawElementsInstancedBaseVertex(mOpenGLPrimitiveTopology, static_cast<GLsizei>(numberOfIndices), indexBuffer->getOpenGLType(), reinterpret_cast<const GLvoid*>(startIndexLocation * sizeof(int)), static_cast<GLsizei>(numberOfInstances), static_cast<GLint>(baseVertexLocation));
					}
					else
					{
						// Error!
					}
				}
				else
				{
					// Draw without base vertex location
					glDrawElementsInstancedARB(mOpenGLPrimitiveTopology, static_cast<GLsizei>(numberOfIndices), indexBuffer->getOpenGLType(), reinterpret_cast<const GLvoid*>(startIndexLocation * sizeof(int)), static_cast<GLsizei>(numberOfInstances));
				}
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Synchronization                                       ]
	//[-------------------------------------------------------]
	void OpenGLRenderer::flush()
	{
		glFlush();
	}

	void OpenGLRenderer::finish()
	{
		glFinish();
	}


	//[-------------------------------------------------------]
	//[ Debug                                                 ]
	//[-------------------------------------------------------]
	bool OpenGLRenderer::isDebugEnabled()
	{
		// OpenGL has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)

		// Debug disabled
		return false;
	}

	void OpenGLRenderer::setDebugMarker(const wchar_t *)
	{
		// OpenGL has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)
	}

	void OpenGLRenderer::beginDebugEvent(const wchar_t *)
	{
		// OpenGL has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)
	}

	void OpenGLRenderer::endDebugEvent()
	{
		// OpenGL has nothing that is similar to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Initialize the capabilities
	*/
	void OpenGLRenderer::initializeCapabilities()
	{
		GLint openGLValue = 0;

		// Maximum number of viewports (always at least 1)
		// TODO(co) "GL_ARB_viewport_array" support ("OpenGLRenderer::rsSetViewports()")
		mCapabilities.maximumNumberOfViewports = 1;	// TODO(co) GL_ARB_viewport_array

		// Maximum number of simultaneous render targets (if <1 render to texture is not supported, "GL_ARB_draw_buffers" required)
		if (mContext->getExtensions().isGL_ARB_draw_buffers())
		{
			glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &openGLValue);
			mCapabilities.maximumNumberOfSimultaneousRenderTargets = static_cast<unsigned int>(openGLValue);
		}
		else
		{
			// "GL_ARB_framebuffer_object"-extension for render to texture required
			mCapabilities.maximumNumberOfSimultaneousRenderTargets = static_cast<unsigned int>(mContext->getExtensions().isGL_ARB_framebuffer_object());
		}

		// Maximum texture dimension
		openGLValue = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &openGLValue);
		mCapabilities.maximumTextureDimension = static_cast<unsigned int>(openGLValue);

		// Maximum number of 2D texture array slices (usually 512, in case there's no support for 2D texture arrays it's 0)
		if (mContext->getExtensions().isGL_EXT_texture_array())
		{
			glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS_EXT, &openGLValue);
			mCapabilities.maximumNumberOf2DTextureArraySlices = static_cast<unsigned int>(openGLValue);
		}
		else
		{
			mCapabilities.maximumNumberOf2DTextureArraySlices = 0;
		}

		// Uniform buffer object (UBO, "constant buffer" in Direct3D terminology) supported?
		mCapabilities.uniformBuffer = mContext->getExtensions().isGL_ARB_uniform_buffer_object();

		// Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
		if (mContext->getExtensions().isGL_ARB_texture_buffer_object())
		{
			glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE_EXT, &openGLValue);
			mCapabilities.maximumTextureBufferSize = static_cast<unsigned int>(openGLValue);
		}
		else
		{
			mCapabilities.maximumTextureBufferSize = 0;
		}

		// Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		mCapabilities.individualUniforms = true;

		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
		mCapabilities.instancedArrays = true;

		// Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex, "GL_ARB_instanced_arrays" required)
		mCapabilities.instancedArrays = mContext->getExtensions().isGL_ARB_instanced_arrays();

		// Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID, "GL_ARB_draw_instanced" required)
		mCapabilities.drawInstanced = mContext->getExtensions().isGL_ARB_draw_instanced();

		// Base vertex supported for draw calls?
		mCapabilities.baseVertex = mContext->getExtensions().isGL_ARB_draw_elements_base_vertex();

		// Is there support for vertex shaders (VS)?
		mCapabilities.vertexShader = mContext->getExtensions().isGL_ARB_vertex_shader();

		// Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
		if (mContext->getExtensions().isGL_ARB_tessellation_shader())
		{
			glGetIntegerv(GL_MAX_PATCH_VERTICES, &openGLValue);
			mCapabilities.maximumNumberOfPatchVertices = static_cast<unsigned int>(openGLValue);
		}
		else
		{
			mCapabilities.maximumNumberOfPatchVertices = 0;
		}

		// Maximum number of vertices a geometry shader can emit (usually 0 for no geometry shader support or 1024)
		if (mContext->getExtensions().isGL_ARB_geometry_shader4())
		{
			glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB, &openGLValue);
			mCapabilities.maximumNumberOfGsOutputVertices = static_cast<unsigned int>(openGLValue);
		}
		else
		{
			mCapabilities.maximumNumberOfGsOutputVertices = 0;
		}

		// Is there support for fragment shaders (FS)?
		mCapabilities.fragmentShader = mContext->getExtensions().isGL_ARB_fragment_shader();
	}

	/**
	*  @brief
	*    Unset the currently used vertex array
	*/
	void OpenGLRenderer::iaUnsetVertexArray()
	{
		// Release the currently used vertex array reference, in case we have one
		if (nullptr != mVertexArray)
		{
			// Evaluate the internal array type type of the currently set vertex array
			switch (static_cast<VertexArray*>(mVertexArray)->getInternalResourceType())
			{
				case VertexArray::InternalResourceType::NO_VAO:
					// Disable OpenGL vertex attribute arrays
					static_cast<VertexArrayNoVao*>(mVertexArray)->disableOpenGLVertexAttribArrays();
					break;

				#ifndef OPENGLRENDERER_NO_CG
					case VertexArray::InternalResourceType::CG:
						// Disable OpenGL Cg vertex attribute arrays
						static_cast<VertexArrayCg*>(mVertexArray)->disableOpenGLVertexAttribArrays();
						break;
				#endif

				case VertexArray::InternalResourceType::VAO:
					// Unbind OpenGL vertex array
					// -> No need to check for "GL_ARB_vertex_array_object", in case were in here we know it must exist
					glBindVertexArray(0);
					break;
			}

			// Release reference
			mVertexArray->release();
			mVertexArray = nullptr;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
