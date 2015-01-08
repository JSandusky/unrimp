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
#ifndef __DIRECT3D11RENDERER_DIRECT3D11RENDERER_H__
#define __DIRECT3D11RENDERER_DIRECT3D11RENDERER_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/IRenderer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct ID3D11Query;
struct ID3D11Device;
struct ID3D11DeviceContext;
namespace Renderer
{
	class RenderTarget;
}
namespace Direct3D11Renderer
{
	class SwapChain;
	class Direct3D9RuntimeLinking;
	class Direct3D11RuntimeLinking;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 11 renderer class
	*/
	class Direct3D11Renderer : public Renderer::IRenderer
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] nativeWindowHandle
		*    Optional native main window handle, can be a null handle
		*
		*  @note
		*    - Do never ever use a not properly initialized renderer! Use "Renderer::IRenderer::isInitialized()" to check the initialization state.
		*/
		explicit Direct3D11Renderer(handle nativeWindowHandle);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Direct3D11Renderer();

		/**
		*  @brief
		*    Return the Direct3D 11 device
		*
		*  @return
		*    The Direct3D 11 device, null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D11Device *getD3D11Device() const;

		/**
		*  @brief
		*    Return the Direct3D 11 device context instance
		*
		*  @return
		*    The Direct3D 11 device context instance, null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D11DeviceContext *getD3D11DeviceContext() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	public:
		inline virtual const char *getName() const override;
		inline virtual bool isInitialized() const override;
		virtual Renderer::ISwapChain *getMainSwapChain() const override;
		//[-------------------------------------------------------]
		//[ Shader language                                       ]
		//[-------------------------------------------------------]
		virtual uint32_t getNumberOfShaderLanguages() const override;
		virtual const char *getShaderLanguageName(uint32_t index) const override;
		virtual Renderer::IShaderLanguage *getShaderLanguage(const char *shaderLanguageName = nullptr) override;
		//[-------------------------------------------------------]
		//[ Resource creation                                     ]
		//[-------------------------------------------------------]
		virtual Renderer::ISwapChain *createSwapChain(handle nativeWindowHandle) override;
		virtual Renderer::IFramebuffer *createFramebuffer(uint32_t numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture = nullptr) override;
		virtual Renderer::IVertexBuffer *createVertexBuffer(uint32_t numberOfBytes, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) override;
		virtual Renderer::IIndexBuffer *createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) override;
		virtual Renderer::ITextureBuffer *createTextureBuffer(uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) override;
		virtual Renderer::ITexture2D *createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, void *data = nullptr, uint32_t flags = 0, Renderer::TextureUsage::Enum textureUsage = Renderer::TextureUsage::DEFAULT) override;
		virtual Renderer::ITexture2DArray *createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data = nullptr, uint32_t flags = 0, Renderer::TextureUsage::Enum textureUsage = Renderer::TextureUsage::DEFAULT) override;
		virtual Renderer::IRasterizerState *createRasterizerState(const Renderer::RasterizerState &rasterizerState) override;
		virtual Renderer::IDepthStencilState *createDepthStencilState(const Renderer::DepthStencilState &depthStencilState) override;
		virtual Renderer::IBlendState *createBlendState(const Renderer::BlendState &blendState) override;
		virtual Renderer::ISamplerState *createSamplerState(const Renderer::SamplerState &samplerState) override;
		virtual Renderer::ITextureCollection *createTextureCollection(uint32_t numberOfTextures, Renderer::ITexture **textures) override;
		virtual Renderer::ISamplerStateCollection *createSamplerStateCollection(uint32_t numberOfSamplerStates, Renderer::ISamplerState **samplerStates) override;
		//[-------------------------------------------------------]
		//[ Resource handling                                     ]
		//[-------------------------------------------------------]
		virtual bool map(Renderer::IResource &resource, uint32_t subresource, Renderer::MapType::Enum mapType, uint32_t mapFlags, Renderer::MappedSubresource &mappedSubresource) override;
		virtual void unmap(Renderer::IResource &resource, uint32_t subresource) override;
		//[-------------------------------------------------------]
		//[ States                                                ]
		//[-------------------------------------------------------]
		virtual void setProgram(Renderer::IProgram *program) override;
		//[-------------------------------------------------------]
		//[ Input-assembler (IA) stage                            ]
		//[-------------------------------------------------------]
		virtual void iaSetVertexArray(Renderer::IVertexArray *vertexArray) override;
		virtual void iaSetPrimitiveTopology(Renderer::PrimitiveTopology::Enum primitiveTopology) override;
		//[-------------------------------------------------------]
		//[ Vertex-shader (VS) stage                              ]
		//[-------------------------------------------------------]
		virtual void vsSetTexture(uint32_t unit, Renderer::ITexture *texture) override;
		virtual void vsSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection) override;
		virtual void vsSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState) override;
		virtual void vsSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection) override;
		virtual void vsSetUniformBuffer(uint32_t slot, Renderer::IUniformBuffer *uniformBuffer) override;
		//[-------------------------------------------------------]
		//[ Tessellation-control-shader (TCS) stage               ]
		//[-------------------------------------------------------]
		virtual void tcsSetTexture(uint32_t unit, Renderer::ITexture *texture) override;
		virtual void tcsSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection) override;
		virtual void tcsSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState) override;
		virtual void tcsSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection) override;
		virtual void tcsSetUniformBuffer(uint32_t slot, Renderer::IUniformBuffer *uniformBuffer) override;
		//[-------------------------------------------------------]
		//[ Tessellation-evaluation-shader (TES) stage            ]
		//[-------------------------------------------------------]
		virtual void tesSetTexture(uint32_t unit, Renderer::ITexture *texture) override;
		virtual void tesSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection) override;
		virtual void tesSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState) override;
		virtual void tesSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection) override;
		virtual void tesSetUniformBuffer(uint32_t slot, Renderer::IUniformBuffer *uniformBuffer) override;
		//[-------------------------------------------------------]
		//[ Geometry-shader (GS) stage                            ]
		//[-------------------------------------------------------]
		virtual void gsSetTexture(uint32_t unit, Renderer::ITexture *texture) override;
		virtual void gsSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection) override;
		virtual void gsSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState) override;
		virtual void gsSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection) override;
		virtual void gsSetUniformBuffer(uint32_t slot, Renderer::IUniformBuffer *uniformBuffer) override;
		//[-------------------------------------------------------]
		//[ Rasterizer (RS) stage                                 ]
		//[-------------------------------------------------------]
		virtual void rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport *viewports) override;
		virtual void rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles) override;
		virtual void rsSetState(Renderer::IRasterizerState *rasterizerState) override;
		//[-------------------------------------------------------]
		//[ Fragment-shader (FS) stage                            ]
		//[-------------------------------------------------------]
		virtual void fsSetTexture(uint32_t unit, Renderer::ITexture *texture) override;
		virtual void fsSetTextureCollection(uint32_t startUnit, Renderer::ITextureCollection *textureCollection) override;
		virtual void fsSetSamplerState(uint32_t unit, Renderer::ISamplerState *samplerState) override;
		virtual void fsSetSamplerStateCollection(uint32_t startUnit, Renderer::ISamplerStateCollection *samplerStateCollection) override;
		virtual void fsSetUniformBuffer(uint32_t slot, Renderer::IUniformBuffer *uniformBuffer) override;
		//[-------------------------------------------------------]
		//[ Output-merger (OM) stage                              ]
		//[-------------------------------------------------------]
		virtual Renderer::IRenderTarget *omGetRenderTarget() override;
		virtual void omSetRenderTarget(Renderer::IRenderTarget *renderTarget) override;
		virtual void omSetDepthStencilState(Renderer::IDepthStencilState *depthStencilState) override;
		virtual void omSetBlendState(Renderer::IBlendState *blendState) override;
		//[-------------------------------------------------------]
		//[ Operations                                            ]
		//[-------------------------------------------------------]
		virtual void clear(uint32_t flags, const float color[4], float z, uint32_t stencil) override;
		virtual bool beginScene() override;
		virtual void endScene() override;
		//[-------------------------------------------------------]
		//[ Draw call                                             ]
		//[-------------------------------------------------------]
		virtual void draw(uint32_t startVertexLocation, uint32_t numberOfVertices) override;
		virtual void drawInstanced(uint32_t startVertexLocation, uint32_t numberOfVertices, uint32_t numberOfInstances) override;
		virtual void drawIndexed(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t minimumIndex, uint32_t numberOfVertices) override;
		virtual void drawIndexedInstanced(uint32_t startIndexLocation, uint32_t numberOfIndices, uint32_t baseVertexLocation, uint32_t minimumIndex, uint32_t numberOfVertices, uint32_t numberOfInstances) override;
		//[-------------------------------------------------------]
		//[ Synchronization                                       ]
		//[-------------------------------------------------------]
		virtual void flush() override;
		virtual void finish() override;
		//[-------------------------------------------------------]
		//[ Debug                                                 ]
		//[-------------------------------------------------------]
		virtual bool isDebugEnabled() override;
		virtual void setDebugMarker(const wchar_t *name) override;
		virtual void beginDebugEvent(const wchar_t *name) override;
		virtual void endDebugEvent() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Initialize the capabilities
		*/
		void initializeCapabilities();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Direct3D9RuntimeLinking		  *mDirect3D9RuntimeLinking;	///< Direct3D 9 runtime linking instance, can be a null pointer
		Direct3D11RuntimeLinking	  *mDirect3D11RuntimeLinking;	///< Direct3D 11 runtime linking instance, always valid
		ID3D11Device				  *mD3D11Device;				///< The Direct3D 11 device, null pointer on error (we don't check because this would be a total overhead, the user has to use "Renderer::IRenderer::isInitialized()" and is asked to never ever use a not properly initialized renderer!)
		ID3D11DeviceContext			  *mD3D11DeviceContext;			///< The Direct3D 11 device context instance, null pointer on error (we don't check because this would be a total overhead, the user has to use "Renderer::IRenderer::isInitialized()" and is asked to never ever use a not properly initialized renderer!)
		Renderer::IShaderLanguage	  *mShaderLanguageHlsl;			///< HLSL shader language instance (we keep a reference to it), can be a null pointer
		ID3D11Query					  *mD3D11QueryFlush;			///< Direct3D 11 query used for flush, can be a null pointer
		//[-------------------------------------------------------]
		//[ Output-merger (OM) stage                              ]
		//[-------------------------------------------------------]
		SwapChain				*mMainSwapChain;	///< In case the optional native main window handle within the "Direct3D11Renderer"-constructor was not a null handle, this holds the instance of the main swap chain (we keep a reference to it), can be a null pointer
		Renderer::IRenderTarget *mRenderTarget;		///< Currently set render target (we keep a reference to it), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/Direct3D11Renderer.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D11RENDERER_DIRECT3D11RENDERER_H__
