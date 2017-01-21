/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
struct ID3D12Query;
struct ID3D12Device;
struct IDXGIFactory4;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
namespace Renderer
{
	class IRenderTarget;
}
namespace Direct3D12Renderer
{
	class SwapChain;
	class Direct3D12RuntimeLinking;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 12 renderer class
	*/
	class Direct3D12Renderer : public Renderer::IRenderer
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
		explicit Direct3D12Renderer(handle nativeWindowHandle);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Direct3D12Renderer();

		/**
		*  @brief
		*    Return the DXGI factory instance as pointer
		*
		*  @return
		*    The DXGI factory instance, null pointer on error (but always valid for a correctly initialized renderer), do not release the returned instance unless you added an own reference to it
		*/
		inline IDXGIFactory4 *getDxgiFactory4() const;

		/**
		*  @brief
		*    Return the DXGI factory instance as reference
		*
		*  @return
		*    The DXGI factory instance, do not release the returned instance unless you added an own reference to it
		*/
		inline IDXGIFactory4 &getDxgiFactory4Safe() const;

		/**
		*  @brief
		*    Return the Direct3D 12 device
		*
		*  @return
		*    The Direct3D 12 device, null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D12Device *getD3D12Device() const;

		/**
		*  @brief
		*    Return the Direct3D 12 command queue
		*
		*  @return
		*    The Direct3D 12 command queue, null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D12CommandQueue *getD3D12CommandQueue() const;

		/**
		*  @brief
		*    Return the Direct3D 12 graphics command list
		*
		*  @return
		*    The Direct3D 12 graphics command list, null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D12GraphicsCommandList *getD3D12GraphicsCommandList() const;

		/**
		*  @brief
		*    Get the render target to render into
		*
		*  @return
		*    Render target currently bound to the output-merger state, a null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline Renderer::IRenderTarget *omGetRenderTarget() const;

		//[-------------------------------------------------------]
		//[ States                                                ]
		//[-------------------------------------------------------]
		void setGraphicsRootSignature(Renderer::IRootSignature *rootSignature);
		void setGraphicsRootDescriptorTable(uint32_t rootParameterIndex, Renderer::IResource* resource);
		void setPipelineState(Renderer::IPipelineState *pipelineState);
		//[-------------------------------------------------------]
		//[ Input-assembler (IA) stage                            ]
		//[-------------------------------------------------------]
		void iaSetVertexArray(Renderer::IVertexArray *vertexArray);
		void iaSetPrimitiveTopology(Renderer::PrimitiveTopology primitiveTopology);
		//[-------------------------------------------------------]
		//[ Rasterizer (RS) stage                                 ]
		//[-------------------------------------------------------]
		void rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport *viewports);
		void rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles);
		//[-------------------------------------------------------]
		//[ Output-merger (OM) stage                              ]
		//[-------------------------------------------------------]
		void omSetRenderTarget(Renderer::IRenderTarget *renderTarget);
		//[-------------------------------------------------------]
		//[ Operations                                            ]
		//[-------------------------------------------------------]
		void clear(uint32_t flags, const float color[4], float z, uint32_t stencil);
		void resolveMultisampleFramebuffer(Renderer::IRenderTarget& destinationRenderTarget, Renderer::IFramebuffer& sourceMultisampleFramebuffer);
		void copyResource(Renderer::IResource& destinationResource, Renderer::IResource& sourceResource);
		//[-------------------------------------------------------]
		//[ Draw call                                             ]
		//[-------------------------------------------------------]
		void drawEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset = 0, uint32_t numberOfDraws = 1);
		void drawIndexedEmulated(const uint8_t* emulationData, uint32_t indirectBufferOffset = 0, uint32_t numberOfDraws = 1);
		//[-------------------------------------------------------]
		//[ Debug                                                 ]
		//[-------------------------------------------------------]
		void setDebugMarker(const wchar_t *name);
		void beginDebugEvent(const wchar_t *name);
		void endDebugEvent();


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	public:
		inline virtual const char *getName() const override;
		inline virtual bool isInitialized() const override;
		virtual bool isDebugEnabled() override;
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
		virtual Renderer::IBufferManager *createBufferManager() override;
		virtual Renderer::ITextureManager *createTextureManager() override;
		virtual Renderer::IRootSignature *createRootSignature(const Renderer::RootSignature &rootSignature) override;
		virtual Renderer::IPipelineState *createPipelineState(const Renderer::PipelineState &pipelineState) override;
		virtual Renderer::ISamplerState *createSamplerState(const Renderer::SamplerState &samplerState) override;
		//[-------------------------------------------------------]
		//[ Resource handling                                     ]
		//[-------------------------------------------------------]
		virtual bool map(Renderer::IResource &resource, uint32_t subresource, Renderer::MapType mapType, uint32_t mapFlags, Renderer::MappedSubresource &mappedSubresource) override;
		virtual void unmap(Renderer::IResource &resource, uint32_t subresource) override;
		//[-------------------------------------------------------]
		//[ Operations                                            ]
		//[-------------------------------------------------------]
		virtual bool beginScene() override;
		virtual void submitCommandBuffer(const Renderer::CommandBuffer& commandBuffer) override;
		virtual void endScene() override;
		//[-------------------------------------------------------]
		//[ Synchronization                                       ]
		//[-------------------------------------------------------]
		virtual void flush() override;
		virtual void finish() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Initialize the capabilities
		*/
		void initializeCapabilities();

		#ifndef DIRECT3D12RENDERER_NO_DEBUG
			/**
			*  @brief
			*    Reports information about a device object's lifetime for debugging
			*/
			void debugReportLiveDeviceObjects();
		#endif


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Direct3D12RuntimeLinking	  *mDirect3D12RuntimeLinking;	///< Direct3D 12 runtime linking instance, always valid
		IDXGIFactory4*				   mDxgiFactory4;				///< DXGI factors instance, always valid for a correctly initialized renderer
		ID3D12Device				  *mD3D12Device;				///< The Direct3D 12 device, null pointer on error (we don't check because this would be a total overhead, the user has to use "Renderer::IRenderer::isInitialized()" and is asked to never ever use a not properly initialized renderer!)
		ID3D12CommandQueue*			   mD3D12CommandQueue;			///< The Direct3D 12 command queue, null pointer on error (we don't check because this would be a total overhead, the user has to use "Renderer::IRenderer::isInitialized()" and is asked to never ever use a not properly initialized renderer!)
		ID3D12CommandAllocator*		   mD3D12CommandAllocator;
		ID3D12GraphicsCommandList*	   mD3D12GraphicsCommandList;
		Renderer::IShaderLanguage	  *mShaderLanguageHlsl;			///< HLSL shader language instance (we keep a reference to it), can be a null pointer
		ID3D12Query					  *mD3D12QueryFlush;			///< Direct3D 12 query used for flush, can be a null pointer
		//[-------------------------------------------------------]
		//[ Output-merger (OM) stage                              ]
		//[-------------------------------------------------------]
		SwapChain				*mMainSwapChain;	///< In case the optional native main window handle within the "Direct3D12Renderer"-constructor was not a null handle, this holds the instance of the main swap chain (we keep a reference to it), can be a null pointer
		Renderer::IRenderTarget *mRenderTarget;		///< Currently set render target (we keep a reference to it), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D12Renderer/Direct3D12Renderer.inl"
