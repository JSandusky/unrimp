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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Renderer/Statistics.h"
#include "Renderer/BufferTypes.h"
#include "Renderer/Capabilities.h"
#include "Renderer/TextureTypes.h"
#include "Renderer/RendererTypes.h"
#include "Renderer/SmartRefCount.h"
#include "Renderer/IndexBufferTypes.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class ITexture;
	class IProgram;
	class IResource;
	struct Viewport;
	class ITexture2D;
	class ISwapChain;
	class IBlendState;
	struct BlendState;
	class IIndexBuffer;
	class IVertexArray;
	class IFramebuffer;
	class ISamplerState;
	struct SamplerState;
	class IVertexBuffer;
	class IRenderTarget;
	class IUniformBuffer;
	class ITextureBuffer;
	class IShaderLanguage;
	class ITexture2DArray;
	class IRasterizerState;
	struct RasterizerState;
	struct ScissorRectangle;
	class IDepthStencilState;
	struct DepthStencilState;
	class ITextureCollection;
	class ISamplerStateCollection;
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
		friend class IProgram;
		friend class IVertexArray;
		friend class ISwapChain;
		friend class IFramebuffer;
		friend class IIndexBuffer;
		friend class IVertexBuffer;
		friend class IUniformBuffer;
		friend class ITextureBuffer;
		friend class ITexture2D;
		friend class ITexture2DArray;
		friend class IRasterizerState;
		friend class IDepthStencilState;
		friend class IBlendState;
		friend class ISamplerState;
		friend class IVertexShader;
		friend class ITessellationControlShader;
		friend class ITessellationEvaluationShader;
		friend class IGeometryShader;
		friend class IFragmentShader;
		friend class ITextureCollection;
		friend class ISamplerStateCollection;


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
		*    Native window handle, in case of a null handle nothing happens
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
		*    Create a vertex buffer object (VBO, "array buffer" in OpenGL terminology) instance
		*
		*  @param[in] numberOfBytes
		*    Number of bytes within the index buffer, must be valid
		*  @param[in] data
		*    Vertex buffer data, can be a null pointer (empty buffer), the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*
		*  @return
		*    The created VBO instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Vertex array instances are created by using "Renderer::IProgram::createVertexArray()"
		*/
		virtual IVertexBuffer *createVertexBuffer(uint32_t numberOfBytes, const void *data = nullptr, BufferUsage::Enum bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;

		/**
		*  @brief
		*    Create an index buffer object (IBO, "element array buffer" in OpenGL terminology) instance
		*
		*  @param[in] numberOfBytes
		*    Number of bytes within the index buffer, must be valid
		*  @param[in] indexBufferFormat
		*    Index buffer data format
		*  @param[in] data
		*    Index buffer data, can be a null pointer (empty buffer), the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*
		*  @return
		*    The created IBO instance, null pointer on error. Release the returned instance if you no longer need it.
		*/
		virtual IIndexBuffer *createIndexBuffer(uint32_t numberOfBytes, IndexBufferFormat::Enum indexBufferFormat, const void *data = nullptr, BufferUsage::Enum bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;

		/**
		*  @brief
		*    Create an texture buffer object (TBO) instance
		*
		*  @param[in] numberOfBytes
		*    Number of bytes within the texture buffer, must be valid
		*  @param[in] textureFormat
		*    Texture buffer data format
		*  @param[in] data
		*    Texture buffer data, can be a null pointer (empty buffer), the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*
		*  @return
		*    The created TBO instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumTextureBufferSize" is not 0
		*/
		virtual ITextureBuffer *createTextureBuffer(uint32_t numberOfBytes, TextureFormat::Enum textureFormat, const void *data = nullptr, BufferUsage::Enum bufferUsage = BufferUsage::DYNAMIC_DRAW) = 0;

		/**
		*  @brief
		*    Create a 2D texture instance
		*
		*  @param[in] width
		*    Texture width, must be >1 else a null pointer is returned
		*  @param[in] height
		*    Texture height, must be >1 else a null pointer is returned
		*  @param[in] textureFormat
		*    Texture data format
		*  @param[in] data
		*    Texture data, can be a null pointer, the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*  @param[in] textureUsage
		*    Indication of the texture usage (only relevant for Direct3D, OpenGL has no texture usage indication)
		*
		*  @return
		*    The created 2D texture instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @remarks
		*    The texture data has to be in CRN-texture layout, which means organized in mip-major order, like this:
		*    - Mip0: Face0, Face1, Face2, Face3, Face4, Face5
		*    - Mip1: Face0, Face1, Face2, Face3, Face4, Face5
		*    (DDS-texture layout is using face-major order)
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumNumberOf2DTextureArraySlices" is not 0
		*/
		virtual ITexture2D *createTexture2D(uint32_t width, uint32_t height, TextureFormat::Enum textureFormat, void *data = nullptr, uint32_t flags = 0, TextureUsage::Enum textureUsage = TextureUsage::DEFAULT) = 0;

		/**
		*  @brief
		*    Create a 2D array texture instance
		*
		*  @param[in] width
		*    Texture width, must be >1 else a null pointer is returned
		*  @param[in] height
		*    Texture height, must be >1 else a null pointer is returned
		*  @param[in] numberOfSlices
		*    Number of slices, must be >1 else a null pointer is returned
		*  @param[in] textureFormat
		*    Texture format
		*  @param[in] data
		*    Texture data, can be a null pointer, the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*  @param[in] textureUsage
		*    Indication of the texture usage, (only relevant for Direct3D, OpenGL has no texture usage indication)
		*
		*  @return
		*    The created 2D array texture instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @remarks
		*    The texture array data consists of a sequence of texture slices. Each the texture slice data of a single texture slice has to
		*    be in CRN-texture layout, which means organized in mip-major order, like this:
		*    - Mip0: Face0, Face1, Face2, Face3, Face4, Face5
		*    - Mip1: Face0, Face1, Face2, Face3, Face4, Face5
		*    (DDS-texture layout is using face-major order)
		*/
		virtual ITexture2DArray *createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, TextureFormat::Enum textureFormat, void *data = nullptr, uint32_t flags = 0, TextureUsage::Enum textureUsage = TextureUsage::DEFAULT) = 0;

		/**
		*  @brief
		*    Create a rasterizer state instance
		*
		*  @param[in] rasterizerState
		*    Rasterizer state to use
		*
		*  @return
		*    The rasterizer state instance, null pointer on error. Release the returned instance if you no longer need it.
		*/
		virtual IRasterizerState *createRasterizerState(const RasterizerState &rasterizerState) = 0;

		/**
		*  @brief
		*    Create a depth stencil state instance
		*
		*  @param[in] depthStencilState
		*    Depth stencil state to use
		*
		*  @return
		*    The depth stencil state instance, null pointer on error. Release the returned instance if you no longer need it.
		*/
		virtual IDepthStencilState *createDepthStencilState(const DepthStencilState &depthStencilState) = 0;

		/**
		*  @brief
		*    Create a blend state instance
		*
		*  @param[in] blendState
		*    Blend state to use
		*
		*  @return
		*    The blend state instance, null pointer on error. Release the returned instance if you no longer need it.
		*/
		virtual IBlendState *createBlendState(const BlendState &blendState) = 0;

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

		/**
		*  @brief
		*    Create a texture collection instance
		*
		*  @param[in] numberOfTextures
		*    The number of textures
		*  @param[in] textures
		*    The textures, can be a null pointer or can contain null pointers, if not a null pointer there must be at
		*    least "numberOfTextures" textures in the provided C-array of pointers
		*
		*  @return
		*    The texture collection instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - The texture collection keeps a reference to the provided texture instances
		*/
		virtual ITextureCollection *createTextureCollection(uint32_t numberOfTextures, ITexture **textures) = 0;

		/**
		*  @brief
		*    Create a sampler state collection instance
		*
		*  @param[in] numberOfSamplerStates
		*    The number of sampler states
		*  @param[in] samplerStates
		*    The sampler states, can be a null pointer or can contain null pointers, if not a null pointer there must be at
		*    least "numberOfSamplerStates" sampler states in the provided C-array of pointers
		*
		*  @return
		*    The sampler state collection instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - The sampler state collection keeps a reference to the provided sampler state instances
		*/
		virtual ISamplerStateCollection *createSamplerStateCollection(uint32_t numberOfSamplerStates, ISamplerState **samplerStates) = 0;

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
		virtual bool map(IResource &resource, uint32_t subresource, MapType::Enum mapType, uint32_t mapFlags, MappedSubresource &mappedSubresource) = 0;

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
		//[ States                                                ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set the used program
		*
		*  @param[in] program
		*    Program to use, can be an null pointer (default: "nullptr")
		*
		*  @note
		*    - Program instances are created by using "Renderer::IShaderLanguage::createProgram()"
		*/
		virtual void setProgram(IProgram *program) = 0;

		//[-------------------------------------------------------]
		//[ Input-assembler (IA) stage                            ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set the used vertex array
		*
		*  @param[in] vertexArray
		*    Vertex array to use, can be an null pointer (default: "nullptr")
		*
		*  @note
		*    - Vertex array instances are created by using "Renderer::IProgram::createVertexArray()"
		*/
		virtual void iaSetVertexArray(IVertexArray *vertexArray) = 0;

		/**
		*  @brief
		*    Set the primitive topology used for draw calls
		*
		*  @param[in] primitiveTopology
		*    Member of the primitive topology enumerated type, describing the type of primitive to render (default: "Renderer::PrimitiveTopology::UNKNOWN")
		*/
		virtual void iaSetPrimitiveTopology(PrimitiveTopology::Enum primitiveTopology) = 0;

		//[-------------------------------------------------------]
		//[ Vertex-shader (VS) stage                              ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set the used vertex shader texture resource at a certain texture unit
		*
		*  @param[in] unit
		*    Texture unit
		*  @param[in] texture
		*    Texture to use, can be an null pointer (default: "nullptr")
		*/
		virtual void vsSetTexture(uint32_t unit, ITexture *texture) = 0;

		/**
		*  @brief
		*    Set the used vertex shader texture resource at a certain texture unit by using a texture collection
		*
		*  @param[in] startUnit
		*    Start texture unit, incremented automatically internally for every texture collection element
		*  @param[in] textureCollection
		*    Texture collection to use, can be an null pointer
		*/
		virtual void vsSetTextureCollection(uint32_t startUnit, ITextureCollection *textureCollection) = 0;

		/**
		*  @brief
		*    Set the used vertex shader sampler state at a certain texture unit
		*
		*  @param[in] unit
		*    Texture unit
		*  @param[in] samplerState
		*    Sampler state, can be a null pointer (default: "nullptr", see "Renderer::SamplerState" for the default values used in this case)
		*/
		virtual void vsSetSamplerState(uint32_t unit, ISamplerState *samplerState) = 0;

		/**
		*  @brief
		*    Set the used vertex shader sampler state resource at a certain texture unit by using a sampler state collection
		*
		*  @param[in] startUnit
		*    Start texture unit, incremented automatically internally for every sampler state collection element
		*  @param[in] samplerStateCollection
		*    Sampler state collection to use, can be an null pointer
		*/
		virtual void vsSetSamplerStateCollection(uint32_t startUnit, ISamplerStateCollection *samplerStateCollection) = 0;

		/**
		*  @brief
		*    Set the used vertex shader uniform buffer object (UBO, "constant buffer" in Direct3D terminology) at a certain slot
		*
		*  @param[in] slot
		*    Slot which can either be searched by an uniform name via "Renderer::IProgram::getUniformBlockIndex()" (for performance reasons not recommended)
		*    or fixed set inside the shader (in OpenGL the "GL_ARB_explicit_uniform_location"-extension is required for this)
		*  @param[in] uniformBuffer
		*    Uniform buffer, can be a null pointer (default: "nullptr")
		*
		*  @note
		*    - Uniform buffer instances are created by using "Renderer::IShaderLanguage::createUniformBuffer()"
		*    - Only supported if "Renderer::Capabilities::uniformBuffer" is true
		*/
		virtual void vsSetUniformBuffer(uint32_t slot, IUniformBuffer *uniformBuffer) = 0;

		//[-------------------------------------------------------]
		//[ Tessellation-control-shader (TCS) stage               ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set the used tessellation control shader texture resource at a certain texture unit
		*
		*  @param[in] unit
		*    Texture unit
		*  @param[in] texture
		*    Texture to use, can be an null pointer (default: "nullptr")
		*/
		virtual void tcsSetTexture(uint32_t unit, ITexture *texture) = 0;

		/**
		*  @brief
		*    Set the used tessellation control shader texture resource at a certain texture unit by using a texture collection
		*
		*  @param[in] startUnit
		*    Start texture unit, incremented automatically internally for every texture collection element
		*  @param[in] textureCollection
		*    Texture collection to use, can be an null pointer
		*/
		virtual void tcsSetTextureCollection(uint32_t startUnit, ITextureCollection *textureCollection) = 0;

		/**
		*  @brief
		*    Set the used tessellation control shader sampler state at a certain texture unit
		*
		*  @param[in] unit
		*    Texture unit
		*  @param[in] samplerState
		*    Sampler state, can be a null pointer (default: "nullptr", see "Renderer::SamplerState" for the default values used in this case)
		*/
		virtual void tcsSetSamplerState(uint32_t unit, ISamplerState *samplerState) = 0;

		/**
		*  @brief
		*    Set the used tessellation control shader sampler state resource at a certain texture unit by using a sampler state collection
		*
		*  @param[in] startUnit
		*    Start texture unit, incremented automatically internally for every sampler state collection element
		*  @param[in] samplerStateCollection
		*    Sampler state collection to use, can be an null pointer
		*/
		virtual void tcsSetSamplerStateCollection(uint32_t startUnit, ISamplerStateCollection *samplerStateCollection) = 0;

		/**
		*  @brief
		*    Set the used tessellation control shader uniform buffer object (UBO, "constant buffer" in Direct3D terminology) at a certain slot
		*
		*  @param[in] slot
		*    Slot which can either be searched by an uniform name via "Renderer::IProgram::getUniformBlockIndex()" (for performance reasons not recommended)
		*    or fixed set inside the shader (in OpenGL the "GL_ARB_explicit_uniform_location"-extension is required for this)
		*  @param[in] uniformBuffer
		*    Uniform buffer, can be a null pointer (default: "nullptr")
		*
		*  @note
		*    - Uniform buffer instances are created by using "Renderer::IShaderLanguage::createUniformBuffer()"
		*    - Only supported if "Renderer::Capabilities::uniformBuffer" is true
		*/
		virtual void tcsSetUniformBuffer(uint32_t slot, IUniformBuffer *uniformBuffer) = 0;

		//[-------------------------------------------------------]
		//[ Tessellation-evaluation-shader (TES) stage            ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set the used tessellation evaluation shader texture resource at a certain texture unit
		*
		*  @param[in] unit
		*    Texture unit
		*  @param[in] texture
		*    Texture to use, can be an null pointer (default: "nullptr")
		*/
		virtual void tesSetTexture(uint32_t unit, ITexture *texture) = 0;

		/**
		*  @brief
		*    Set the used tessellation evaluation shader texture resource at a certain texture unit by using a texture collection
		*
		*  @param[in] startUnit
		*    Start texture unit, incremented automatically internally for every texture collection element
		*  @param[in] textureCollection
		*    Texture collection to use, can be an null pointer
		*/
		virtual void tesSetTextureCollection(uint32_t startUnit, ITextureCollection *textureCollection) = 0;

		/**
		*  @brief
		*    Set the used tessellation evaluation shader sampler state at a certain texture unit
		*
		*  @param[in] unit
		*    Texture unit
		*  @param[in] samplerState
		*    Sampler state, can be a null pointer (default: "nullptr", see "Renderer::SamplerState" for the default values used in this case)
		*/
		virtual void tesSetSamplerState(uint32_t unit, ISamplerState *samplerState) = 0;

		/**
		*  @brief
		*    Set the used tessellation evaluation shader sampler state resource at a certain texture unit by using a sampler state collection
		*
		*  @param[in] startUnit
		*    Start texture unit, incremented automatically internally for every sampler state collection element
		*  @param[in] samplerStateCollection
		*    Sampler state collection to use, can be an null pointer
		*/
		virtual void tesSetSamplerStateCollection(uint32_t startUnit, ISamplerStateCollection *samplerStateCollection) = 0;

		/**
		*  @brief
		*    Set the used tessellation evaluation shader uniform buffer object (UBO, "constant buffer" in Direct3D terminology) at a certain slot
		*
		*  @param[in] slot
		*    Slot which can either be searched by an uniform name via "Renderer::IProgram::getUniformBlockIndex()" (for performance reasons not recommended)
		*    or fixed set inside the shader (in OpenGL the "GL_ARB_explicit_uniform_location"-extension is required for this)
		*  @param[in] uniformBuffer
		*    Uniform buffer, can be a null pointer (default: "nullptr")
		*
		*  @note
		*    - Uniform buffer instances are created by using "Renderer::IShaderLanguage::createUniformBuffer()"
		*    - Only supported if "Renderer::Capabilities::uniformBuffer" is true
		*/
		virtual void tesSetUniformBuffer(uint32_t slot, IUniformBuffer *uniformBuffer) = 0;

		//[-------------------------------------------------------]
		//[ Geometry-shader (GS) stage                            ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set the used geometry shader texture resource at a certain texture unit
		*
		*  @param[in] unit
		*    Texture unit
		*  @param[in] texture
		*    Texture to use, can be an null pointer (default: "nullptr")
		*/
		virtual void gsSetTexture(uint32_t unit, ITexture *texture) = 0;

		/**
		*  @brief
		*    Set the used geometry shader texture resource at a certain texture unit by using a texture collection
		*
		*  @param[in] startUnit
		*    Start texture unit, incremented automatically internally for every texture collection element
		*  @param[in] textureCollection
		*    Texture collection to use, can be an null pointer
		*/
		virtual void gsSetTextureCollection(uint32_t startUnit, ITextureCollection *textureCollection) = 0;

		/**
		*  @brief
		*    Set the used geometry shader sampler state at a certain texture unit
		*
		*  @param[in] unit
		*    Texture unit
		*  @param[in] samplerState
		*    Sampler state, can be a null pointer (default: "nullptr", see "Renderer::SamplerState" for the default values used in this case)
		*/
		virtual void gsSetSamplerState(uint32_t unit, ISamplerState *samplerState) = 0;

		/**
		*  @brief
		*    Set the used geometry shader sampler state resource at a certain texture unit by using a sampler state collection
		*
		*  @param[in] startUnit
		*    Start texture unit, incremented automatically internally for every sampler state collection element
		*  @param[in] samplerStateCollection
		*    Sampler state collection to use, can be an null pointer
		*/
		virtual void gsSetSamplerStateCollection(uint32_t startUnit, ISamplerStateCollection *samplerStateCollection) = 0;

		/**
		*  @brief
		*    Set the used geometry shader uniform buffer object (UBO, "constant buffer" in Direct3D terminology) at a certain slot
		*
		*  @param[in] slot
		*    Slot which can either be searched by an uniform name via "Renderer::IProgram::getUniformBlockIndex()" (for performance reasons not recommended)
		*    or fixed set inside the shader (in OpenGL the "GL_ARB_explicit_uniform_location"-extension is required for this)
		*  @param[in] uniformBuffer
		*    Uniform buffer, can be a null pointer (default: "nullptr")
		*
		*  @note
		*    - Uniform buffer instances are created by using "Renderer::IShaderLanguage::createUniformBuffer()"
		*    - Only supported if "Renderer::Capabilities::uniformBuffer" is true
		*/
		virtual void gsSetUniformBuffer(uint32_t slot, IUniformBuffer *uniformBuffer) = 0;

		//[-------------------------------------------------------]
		//[ Rasterizer (RS) stage                                 ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set the viewports
		*
		*  @param[in] numberOfViewports
		*    Number of viewports, if <1 nothing happens, must be <="Renderer::Capabilities::maximumNumberOfViewports"
		*  @param[in] viewports
		*    C-array of viewports, there must be at least "numberOfViewports"-viewports, in case of a null pointer nothing happens
		*
		*  @note
		*    - The current viewport(s) does not affect the clear operation
		*/
		virtual void rsSetViewports(uint32_t numberOfViewports, const Viewport *viewports) = 0;

		/**
		*  @brief
		*    Set the scissor rectangles
		*
		*  @param[in] numberOfScissorRectangles
		*    Number of scissor rectangles, if <1 nothing happens, must be <="Renderer::Capabilities::maximumNumberOfViewports"
		*  @param[in] dcissorRectangles
		*    C-array of scissor rectangles, there must be at least "numberOfScissorRectangles" scissor rectangles, in case of a null pointer nothing happens
		*
		*  @note
		*    - Scissor rectangles are only used when "Renderer::RasterizerState::scissorEnable" is true
		*    - The current scissor rectangle(s) does not affect the clear operation
		*/
		virtual void rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const ScissorRectangle *scissorRectangles) = 0;

		/**
		*  @brief
		*    Set the used rasterizer state
		*
		*  @param[in] rasterizerState
		*    Rasterizer state to use, can be an null pointer (in this case nothing happens, default: "nullptr", see "Renderer::RasterizerState" for the default values used in this case)
		*/
		virtual void rsSetState(IRasterizerState *rasterizerState) = 0;

		//[-------------------------------------------------------]
		//[ Fragment-shader (FS) stage                            ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set the used fragment shader texture resource at a certain texture unit
		*
		*  @param[in] unit
		*    Texture unit
		*  @param[in] texture
		*    Texture to use, can be an null pointer (default: "nullptr")
		*/
		virtual void fsSetTexture(uint32_t unit, ITexture *texture) = 0;

		/**
		*  @brief
		*    Set the used fragment shader texture resource at a certain texture unit by using a texture collection
		*
		*  @param[in] startUnit
		*    Start texture unit, incremented automatically internally for every texture collection element
		*  @param[in] textureCollection
		*    Texture collection to use, can be an null pointer
		*/
		virtual void fsSetTextureCollection(uint32_t startUnit, ITextureCollection *textureCollection) = 0;

		/**
		*  @brief
		*    Set the used fragment shader sampler state at a certain texture unit
		*
		*  @param[in] unit
		*    Texture unit
		*  @param[in] samplerState
		*    Sampler state, can be a null pointer (default: "nullptr", see "Renderer::SamplerState" for the default values used in this case)
		*/
		virtual void fsSetSamplerState(uint32_t unit, ISamplerState *samplerState) = 0;

		/**
		*  @brief
		*    Set the used fragment shader sampler state resource at a certain texture unit by using a sampler state collection
		*
		*  @param[in] startUnit
		*    Start texture unit, incremented automatically internally for every sampler state collection element
		*  @param[in] samplerStateCollection
		*    Sampler state collection to use, can be an null pointer
		*/
		virtual void fsSetSamplerStateCollection(uint32_t startUnit, ISamplerStateCollection *samplerStateCollection) = 0;

		/**
		*  @brief
		*    Set the used fragment shader uniform buffer object (UBO, "constant buffer" in Direct3D terminology) at a certain slot
		*
		*  @param[in] slot
		*    Slot which can either be searched by an uniform name via "Renderer::IProgram::getUniformBlockIndex()" (for performance reasons not recommended)
		*    or fixed set inside the shader (in OpenGL the "GL_ARB_explicit_uniform_location"-extension is required for this)
		*  @param[in] uniformBuffer
		*    Uniform buffer, can be a null pointer (default: "nullptr")
		*
		*  @note
		*    - Uniform buffer instances are created by using "Renderer::IShaderLanguage::createUniformBuffer()"
		*    - Only supported if "Renderer::Capabilities::uniformBuffer" is true
		*/
		virtual void fsSetUniformBuffer(uint32_t slot, IUniformBuffer *uniformBuffer) = 0;

		//[-------------------------------------------------------]
		//[ Output-merger (OM) stage                              ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Get the render target to render into
		*
		*  @return
		*    Render target currently bound to the output-merger state, a null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		virtual IRenderTarget *omGetRenderTarget() = 0;

		/**
		*  @brief
		*    Set the render target to render into
		*
		*  @param[in] renderTarget
		*    Render target to render into by binding it to the output-merger state, can be an null pointer to render into the primary window
		*/
		virtual void omSetRenderTarget(IRenderTarget *renderTarget) = 0;

		/**
		*  @brief
		*    Set the used depth stencil state
		*
		*  @param[in] depthStencilState
		*    Depth stencil state to use, can be an null pointer (in this case nothing happens, default: "nullptr", see "Renderer::DepthStencilState" for the default values used in this case)
		*/
		virtual void omSetDepthStencilState(IDepthStencilState *depthStencilState) = 0;

		/**
		*  @brief
		*    Set the used blend state
		*
		*  @param[in] blendState
		*    Blend state to use, can be an null pointer (in this case nothing happens, default: "nullptr", see "Renderer::BlendState" for the default values used in this case)
		*/
		virtual void omSetBlendState(IBlendState *blendState) = 0;

		//[-------------------------------------------------------]
		//[ Operations                                            ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Clears the viewport to a specified RGBA color, clears the depth buffer,
		*    and erases the stencil buffer
		*
		*  @param[in] flags
		*    Flags that indicate what should be cleared. This parameter can be any
		*    combination of the following flags, but at least one flag must be used:
		*    "Renderer::ClearFlag::COLOR", "Renderer::ClearFlag::DEPTH" and "Renderer::ClearFlag::STENCIL, see "Renderer::ClearFlag"-flags
		*  @param[in] color
		*    RGBA clear color (used if "Renderer::ClearFlag::COLOR" is set)
		*  @param[in] z
		*    Z clear value. (if "Renderer::ClearFlag::DEPTH" is set)
		*    This parameter can be in the range from 0.0 through 1.0. A value of 0.0
		*    represents the nearest distance to the viewer, and 1.0 the farthest distance.
		*  @param[in] stencil
		*    Value to clear the stencil-buffer with. This parameter can be in the range from
		*    0 through 2^n–1, where n is the bit depth of the stencil buffer.
		*
		*  @note
		*    - The current viewport(s) (see "Renderer::IRenderer::rsSetViewports()") does not affect the clear operation
		*    - The current scissor rectangle(s) (see "Renderer::IRenderer::rsSetScissorRectangles()") does not affect the clear operation
		*    - In case there are multiple active render targets, all render targets are cleared
		*/
		virtual void clear(uint32_t flags, const float color[4], float z, uint32_t stencil) = 0;

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
		*    End scene rendering
		*
		*  @note
		*    - In order to be graphics API independent, call this method when you're done with rendering
		*/
		virtual void endScene() = 0;

		//[-------------------------------------------------------]
		//[ Draw call                                             ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Render the specified geometric primitive, based on an array of vertices
		*
		*  @param[in] numberOfVertices
		*    Number of vertices to draw
		*  @param[in] startVertexLocation
		*    Index of the first vertex, which is usually an offset in a vertex buffer (usually 0)
		*
		*  @note
		*    - Fails if no vertex array is set
		*/
		virtual void draw(uint32_t startVertexLocation, uint32_t numberOfVertices) = 0;

		/**
		*  @brief
		*    Render the specified geometric primitive, based on an array of vertices and instancing
		*
		*  @param[in] numberOfVertices
		*    Number of vertices to draw
		*  @param[in] startVertexLocation
		*    Index of the first vertex, which is usually an offset in a vertex buffer (usually 0)
		*  @param[in] numberOfInstances
		*    Number of instances, must be >0
		*
		*  @note
		*    - Draw instanced is a shader model 4 feature, only supported if "Renderer::Capabilities::drawInstanced" is true
		*    - In Direct3D 9, instanced arrays with hardware support is only possible when drawing indexed primitives, see
		*      "Efficiently Drawing Multiple Instances of Geometry (Direct3D 9)"-article at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb173349%28v=vs.85%29.aspx#Drawing_Non_Indexed_Geometry
		*    - Fails if no vertex array is set
		*/
		virtual void drawInstanced(uint32_t startVertexLocation, uint32_t numberOfVertices, uint32_t numberOfInstances) = 0;

		/**
		*  @brief
		*    Render the specified geometric primitive, based on indexing into an array of vertices
		*
		*  @param[in] startIndexLocation
		*    The location of the first index read by the GPU from the index buffer (usually 0)
		*  @param[in] numberOfIndices
		*    Number of indices to draw
		*  @param[in] baseVertexLocation
		*    A value added to each index before reading a vertex from the vertex buffer (usually 0), only supported if "Renderer::Capabilities::baseVertex" is true
		*  @param[in] minimumIndex
		*    Minimum vertex index for vertices used during this call (usually 0, relative to "baseVertexLocation")
		*  @param[in] numberOfVertices
		*    Number of vertices used during this call (usually equal to the number of vertices inside the used vertex buffer)
		*
		*  @note
		*    - This method draws indexed primitives from the current set of data input streams
		*    - Fails if no index and/or vertex array is set
		*/
		virtual void drawIndexed(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t minimumIndex, uint32_t numberOfVertices) = 0;

		/**
		*  @brief
		*    Render the specified geometric primitive, based on indexing into an array of vertices and instancing
		*
		*  @param[in] startIndexLocation
		*    The location of the first index read by the GPU from the index buffer (usually 0)
		*  @param[in] numberOfIndices
		*    Number of indices to draw
		*  @param[in] baseVertexLocation
		*    A value added to each index before reading a vertex from the vertex buffer (usually 0), only supported if "Renderer::Capabilities::baseVertex" is true
		*  @param[in] minimumIndex
		*    Minimum vertex index for vertices used during this call (usually 0, relative to "baseVertexLocation")
		*  @param[in] numberOfVertices
		*    Number of vertices used during this call (usually equal to the number of vertices inside the used vertex buffer)
		*  @param[in] numberOfInstances
		*    Number of instances, must be >0
		*
		*  @note
		*    - Instanced arrays is a shader model 3 feature, only supported if "Renderer::Capabilities::instancedArrays" is true
		*    - Draw instanced is a shader model 4 feature, only supported if "Renderer::Capabilities::drawInstanced" is true
		*    - This method draws indexed primitives from the current set of data input streams
		*    - Fails if no index and/or vertex array is set
		*/
		virtual void drawIndexedInstanced(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t minimumIndex, uint32_t numberOfVertices, uint32_t numberOfInstances) = 0;

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
		//[ Debug                                                 ]
		//[-------------------------------------------------------]
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
		*    Set a debug marker
		*
		*  @param[in] name
		*    Unicode name of the debug marker, must be valid (there's no internal null pointer test)
		*
		*  @see
		*    - "isDebugEnabled()"
		*/
		virtual void setDebugMarker(const wchar_t *name) = 0;

		/**
		*  @brief
		*    Begin debug event
		*
		*  @param[in] name
		*    Unicode name of the debug event, must be valid (there's no internal null pointer test)
		*
		*  @see
		*    - "isDebugEnabled()"
		*/
		virtual void beginDebugEvent(const wchar_t *name) = 0;

		/**
		*  @brief
		*    End the last started debug event
		*
		*  @see
		*    - "isDebugEnabled()"
		*/
		virtual void endDebugEvent() = 0;


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


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		Capabilities mCapabilities;	///< Capabilities


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Statistics mStatistics;	///< Statistics


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
