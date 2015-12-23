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
#include <Renderer/IRenderer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class RenderTarget;
}
namespace NullRenderer
{
	class SwapChain;
	class RootSignature;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace NullRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Null renderer class
	*/
	class NullRenderer : public Renderer::IRenderer
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
		explicit NullRenderer(handle nativeWindowHandle);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~NullRenderer();


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	public:
		virtual const char *getName() const override;
		virtual bool isInitialized() const override;
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
		virtual Renderer::IVertexBuffer *createVertexBuffer(uint32_t numberOfBytes, const void *data = nullptr, Renderer::BufferUsage bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) override;
		virtual Renderer::IIndexBuffer *createIndexBuffer(uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data = nullptr, Renderer::BufferUsage bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) override;
		virtual Renderer::IVertexArray *createVertexArray(const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer *vertexBuffers, Renderer::IIndexBuffer *indexBuffer = nullptr) override;
		virtual Renderer::ITextureBuffer *createTextureBuffer(uint32_t numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data = nullptr, Renderer::BufferUsage bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) override;
		virtual Renderer::ITexture2D *createTexture2D(uint32_t width, uint32_t height, Renderer::TextureFormat::Enum textureFormat, void *data = nullptr, uint32_t flags = 0, Renderer::TextureUsage textureUsage = Renderer::TextureUsage::DEFAULT, const Renderer::OptimizedTextureClearValue* optimizedTextureClearValue = nullptr) override;
		virtual Renderer::ITexture2DArray *createTexture2DArray(uint32_t width, uint32_t height, uint32_t numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data = nullptr, uint32_t flags = 0, Renderer::TextureUsage textureUsage = Renderer::TextureUsage::DEFAULT) override;
		virtual Renderer::IRootSignature *createRootSignature(const Renderer::RootSignature &rootSignature) override;
		virtual Renderer::IPipelineState *createPipelineState(const Renderer::PipelineState &pipelineState) override;
		virtual Renderer::ISamplerState *createSamplerState(const Renderer::SamplerState &samplerState) override;
		//[-------------------------------------------------------]
		//[ Resource handling                                     ]
		//[-------------------------------------------------------]
		virtual bool map(Renderer::IResource &resource, uint32_t subresource, Renderer::MapType mapType, uint32_t mapFlags, Renderer::MappedSubresource &mappedSubresource) override;
		virtual void unmap(Renderer::IResource &resource, uint32_t subresource) override;
		//[-------------------------------------------------------]
		//[ States                                                ]
		//[-------------------------------------------------------]
		virtual void setGraphicsRootSignature(Renderer::IRootSignature *rootSignature) override;
		virtual void setGraphicsRootDescriptorTable(uint32_t rootParameterIndex, Renderer::IResource* resource) override;
		virtual void setPipelineState(Renderer::IPipelineState *pipelineState) override;
		//[-------------------------------------------------------]
		//[ Input-assembler (IA) stage                            ]
		//[-------------------------------------------------------]
		virtual void iaSetVertexArray(Renderer::IVertexArray *vertexArray) override;
		virtual void iaSetPrimitiveTopology(Renderer::PrimitiveTopology primitiveTopology) override;
		//[-------------------------------------------------------]
		//[ Rasterizer (RS) stage                                 ]
		//[-------------------------------------------------------]
		virtual void rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport *viewports) override;
		virtual void rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles) override;
		//[-------------------------------------------------------]
		//[ Output-merger (OM) stage                              ]
		//[-------------------------------------------------------]
		virtual Renderer::IRenderTarget *omGetRenderTarget() override;
		virtual void omSetRenderTarget(Renderer::IRenderTarget *renderTarget) override;
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
		Renderer::IShaderLanguage *mShaderLanguage;			///< Null shader language instance (we keep a reference to it), can be a null pointer
		SwapChain				  *mMainSwapChain;			///< In case the optional native main window handle within the "NullRenderer"-constructor was not a null handle, this holds the instance of the main swap chain (we keep a reference to it), can be a null pointer
		Renderer::IRenderTarget   *mRenderTarget;			///< Currently set render target (we keep a reference to it), can be a null pointer
		RootSignature			  *mGraphicsRootSignature;	///< Currently set graphics root signature (we keep a reference to it), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // NullRenderer
