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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_STATISTICS
	#include "Renderer/Statistics.h"
#endif
#include "Renderer/Capabilities.h"
#include "Renderer/RendererTypes.h"
#include "Renderer/SmartRefCount.h"
#include "Renderer/Buffer/BufferTypes.h"
#include "Renderer/Buffer/IndexBufferTypes.h"
#include "Renderer/Texture/TextureTypes.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class ITexture;
	class IResource;
	class ISwapChain;
	class IFramebuffer;
	class CommandBuffer;
	class ISamplerState;
	struct SamplerState;
	class IBufferManager;
	class IRootSignature;
	struct RootSignature;
	class IPipelineState;
	struct PipelineState;
	class ITextureManager;
	class IShaderLanguage;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract renderer interface
	*/
	class IRenderer : public RefCount<IRenderer>
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		// Do not add this within the public "Renderer/Public/Renderer.h"-header, it's for the internal implementation only
		friend class IRootSignature;
		friend class IProgram;
		friend class IVertexArray;
		friend class ISwapChain;
		friend class IFramebuffer;
		friend class IIndexBuffer;
		friend class IVertexBuffer;
		friend class IUniformBuffer;
		friend class ITextureBuffer;
		friend class IIndirectBuffer;
		friend class ITexture2D;
		friend class ITexture2DArray;
		friend class IPipelineState;
		friend class ISamplerState;
		friend class IVertexShader;
		friend class ITessellationControlShader;
		friend class ITessellationEvaluationShader;
		friend class IGeometryShader;
		friend class IFragmentShader;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~IRenderer();

		/**
		*  @brief
		*    Return the capabilities of the renderer instance
		*
		*  @return
		*    The capabilities of the renderer instance
		*
		*  @note
		*    - Do not free the memory the returned reference is pointing to
		*/
		inline const Capabilities &getCapabilities() const;

		#ifndef RENDERER_NO_STATISTICS
			/**
			*  @brief
			*    Return the statistics of the renderer instance
			*
			*  @return
			*    The statistics of the renderer instance
			*
			*  @note
			*    - Do not free the memory the returned reference is pointing to
			*    - It's possible that the statistics or part of it are disabled, e.g. due to hight performance constrains
			*/
			inline const Statistics &getStatistics() const;
		#endif


	//[-------------------------------------------------------]
	//[ Public virtual IRenderer methods                      ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the name of the renderer instance
		*
		*  @return
		*    The ASCII name of the renderer instance, null pointer on error
		*
		*  @note
		*    - Do not free the memory the returned pointer is pointing to
		*/
		virtual const char *getName() const = 0;

		/**
		*  @brief
		*    Return whether or not the renderer instance is properly initialized
		*
		*  @return
		*    "true" if the renderer instance is properly initialized, else "false"
		*
		*  @note
		*    - Do never ever use a not properly initialized renderer!
		*/
		virtual bool isInitialized() const = 0;

		/**
		*  @brief
		*    Return whether or not debug is enabled
		*
		*  @remarks
		*    By using
		*      "Renderer::IRenderer::isDebugEnabled();"
		*    it is possible to check whether or not your application is currently running
		*    within a known debug/profile tool like e.g. Direct3D PIX (also works directly within VisualStudio
		*    2012 out-of-the-box). In case you want at least try to protect your asset, you might want to stop
		*    the execution of your application when a debug/profile tool is used which can e.g. record your data.
		*    Please be aware that this will only make it a little bit harder to debug and e.g. while doing so
		*    reading out your asset data. Public articles like
		*    "PIX: How to circumvent D3DPERF_SetOptions" at
		*      http://www.gamedev.net/blog/1323/entry-2250952-pix-how-to-circumvent-d3dperf-setoptions/
		*    describe how to "hack around" this security measurement, so, don't rely on it. Those debug
		*    methods work fine when using a Direct3D renderer implementation. OpenGL on the other hand
		*    has no Direct3D PIX like functions or extensions, use for instance "gDEBugger" (http://www.gremedy.com/)
		*    instead.
		*    -> When using Direct3D <11.1, those methods map to the Direct3D 9 PIX functions (D3DPERF_* functions)
		*    -> The Direct3D 9 PIX functions are also used for Direct3D 10 and Direct3D 11. Lookout! As soon as using
		*       the debug methods within this interface, the Direct3D 9 dll will be loaded.
		*    -> Starting with Direct3D 11.1, the Direct3D 9 PIX functions no longer work. Instead, the new
		*       "D3D11_CREATE_DEVICE_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY"-flag (does not work with <Direct3D 11.1)
		*       is used when creating the device instance, then the "ID3DUserDefinedAnnotation"-API is used.
		*    -> Optimization: You might want to use those methods only via macros to make it easier to avoid using them
		*       within e.g. a final release build
		*
		*  @return
		*    "true" if debug is enabled, else "false"
		*/
		virtual bool isDebugEnabled() = 0;

		/**
		*  @brief
		*    Return the main swap chain
		*
		*  @return
		*    The main swap chain, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*
		*  @remarks
		*    In case the optional native main window handle within the renderer constructor was
		*    not a null handle, this methods returns the instance of the main swap chain.
		*/
		virtual ISwapChain *getMainSwapChain() const = 0;

		//[-------------------------------------------------------]
		//[ Shader language                                       ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Return the number of supported shader languages
		*
		*  @return
		*    The number of supported shader languages
		*/
		virtual uint32_t getNumberOfShaderLanguages() const = 0;

		/**
		*  @brief
		*    Return the name of a supported shader language at the provided index
		*
		*  @param[in] index
		*    Index of the supported shader language to return the name from ([0, getNumberOfShaderLanguages()-1])
		*
		*  @return
		*    The ASCII name (for example "GLSL" or "HLSL") of the supported shader language at the provided index, can be a null pointer
		*
		*  @note
		*    - Do not free the memory the returned pointer is pointing to
		*    - The default shader language is always at index 0
		*/
		virtual const char *getShaderLanguageName(uint32_t index) const = 0;

		/**
		*  @brief
		*    Return a shader language instance
		*
		*  @param[in] shaderLanguageName
		*    The ASCII name of the shader language (for example "GLSL" or "HLSL"), if null pointer or empty string,
		*    the default renderer shader language is used (see "getShaderLanguageName(0)")
		*
		*  @return
		*    The shader language instance, a null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		virtual IShaderLanguage *getShaderLanguage(const char *shaderLanguageName = nullptr) = 0;

		//[-------------------------------------------------------]
		//[ Resource creation                                     ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Create a swap chain instance
		*
		*  @param[in] nativeWindowHandle
		*    Native window handle, must be valid
		*
		*  @return
		*    The created swap chain instance, null pointer on error. Release the returned instance if you no longer need it.
		*/
		virtual ISwapChain *createSwapChain(handle nativeWindowHandle) = 0;

		/**
		*  @brief
		*    Create a framebuffer object (FBO) instance
		*
		*  @param[in] numberOfColorTextures
		*    Number of color render target textures, must be <="Renderer::Capabilities::maximumNumberOfSimultaneousRenderTargets"
		*  @param[in] colorTextures
		*    The color render target textures, can be a null pointer or can contain null pointers, if not a null pointer there must be at
		*    least "numberOfColorTextures" textures in the provided C-array of pointers
		*  @param[in] depthStencilTexture
		*    The optional depth stencil render target texture, can be a null pointer
		*
		*  @return
		*    The created FBO instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumNumberOfSimultaneousRenderTargets" is not 0
		*    - The framebuffer keeps a reference to the provided texture instances
		*    - It's invalid to set the same color texture to multiple render targets at one and the same time
		*    - Depending on the used graphics API and feature set, there might be the requirement that all provided textures have the same size
		*      (in order to be on the save side, ensure that all provided textures have the same size)
		*/
		virtual IFramebuffer *createFramebuffer(uint32_t numberOfColorTextures, ITexture **colorTextures, ITexture *depthStencilTexture = nullptr) = 0;

		/**
		*  @brief
		*    Create a buffer manager instance
		*
		*  @return
		*    The created buffer manager instance, null pointer on error. Release the returned instance if you no longer need it.
		*/
		virtual IBufferManager *createBufferManager() = 0;

		/**
		*  @brief
		*    Create a texture manager instance
		*
		*  @return
		*    The created texture manager instance, null pointer on error. Release the returned instance if you no longer need it.
		*/
		virtual ITextureManager *createTextureManager() = 0;

		/**
		*  @brief
		*    Create a root signature instance
		*
		*  @param[in] rootSignature
		*    Root signature to use
		*
		*  @return
		*    The root signature instance, null pointer on error. Release the returned instance if you no longer need it.
		*/
		virtual IRootSignature *createRootSignature(const RootSignature &rootSignature) = 0;

		/**
		*  @brief
		*    Create a pipeline state instance
		*
		*  @param[in] pipelineState
		*    Pipeline state to use
		*
		*  @return
		*    The pipeline state instance, null pointer on error. Release the returned instance if you no longer need it.
		*/
		virtual IPipelineState *createPipelineState(const PipelineState &pipelineState) = 0;

		/**
		*  @brief
		*    Create a sampler state instance
		*
		*  @param[in] samplerState
		*    Sampler state to use
		*
		*  @return
		*    The sampler state instance, null pointer on error. Release the returned instance if you no longer need it.
		*/
		virtual ISamplerState *createSamplerState(const SamplerState &samplerState) = 0;

		//[-------------------------------------------------------]
		//[ Resource handling                                     ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Map a resource
		*
		*  @param[in]  resource
		*    Resource to map, there's no internal resource validation, so, do only use valid resources in here!
		*  @param[in]  subresource
		*    Subresource
		*  @param[in]  mapType
		*    Map type
		*  @param[in]  mapFlags
		*    Map flags, see "Renderer::MapFlag"-flags
		*  @param[out] mappedSubresource
		*    Receives the mapped subresource information, do only use this data in case this method returns successfully
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		virtual bool map(IResource &resource, uint32_t subresource, MapType mapType, uint32_t mapFlags, MappedSubresource &mappedSubresource) = 0;

		/**
		*  @brief
		*    Unmap a resource
		*
		*  @param[in] resource
		*    Resource to unmap, there's no internal resource validation, so, do only use valid resources in here!
		*  @param[in] subresource
		*    Subresource
		*/
		virtual void unmap(IResource &resource, uint32_t subresource) = 0;

		//[-------------------------------------------------------]
		//[ Operations                                            ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Begin scene rendering
		*
		*  @return
		*    "true" if all went fine, else "false" (In this case: Don't dare to render something)
		*
		*  @note
		*    - In order to be graphics API independent, call this method when starting to render something
		*/
		virtual bool beginScene() = 0;

		/**
		*  @brief
		*    Submit command buffer to renderer
		*
		*  @param[in] commandBuffer
		*    Command buffer to submit
		*/
		virtual void submitCommandBuffer(const CommandBuffer& commandBuffer) = 0;

		/**
		*  @brief
		*    End scene rendering
		*
		*  @note
		*    - In order to be graphics API independent, call this method when you're done with rendering
		*/
		virtual void endScene() = 0;

		//[-------------------------------------------------------]
		//[ Synchronization                                       ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Force the execution of render commands in finite time (synchronization)
		*/
		virtual void flush() = 0;

		/**
		*  @brief
		*    Force the execution of render commands in finite time and wait until it's done (synchronization)
		*/
		virtual void finish() = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Default constructor
		*/
		inline IRenderer();

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit IRenderer(const IRenderer &source);

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		inline IRenderer &operator =(const IRenderer &source);

		#ifndef RENDERER_NO_STATISTICS
			/**
			*  @brief
			*    Return the statistics of the renderer instance
			*
			*  @return
			*    The statistics of the renderer instance
			*
			*  @note
			*    - Do not free the memory the returned reference is pointing to
			*    - It's possible that the statistics or part of it are disabled, e.g. due to hight performance constrains
			*/
			inline Statistics &getStatistics();
		#endif


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		Capabilities mCapabilities;	///< Capabilities


	#ifndef RENDERER_NO_STATISTICS
		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			Statistics mStatistics;	///< Statistics
	#endif


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<IRenderer> IRendererPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/IRenderer.inl"
