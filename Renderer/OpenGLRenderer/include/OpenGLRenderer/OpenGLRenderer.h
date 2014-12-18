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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __OPENGLRENDERER_OPENGLRENDERER_H__
#define __OPENGLRENDERER_OPENGLRENDERER_H__


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
namespace OpenGLRenderer
{
	class IContext;
	class SwapChain;
	class BlendState;
	class VertexArray;
	class RasterizerState;
	class DepthStencilState;
	#ifndef OPENGLRENDERER_NO_CG
		class CgRuntimeLinking;
	#endif
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    OpenGL renderer class
	*/
	class OpenGLRenderer : public Renderer::IRenderer
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
		*  @param[in] useExternalContext
		*    When true the renderer doesn't create an own OpenGL context for rendering
		*
		*  @note
		*    - Do never ever use a not properly initialized renderer! Use "Renderer::IRenderer::isInitialized()" to check the initialization state.
		*/
		explicit OpenGLRenderer(handle nativeWindowHandle, bool useExternalContext);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~OpenGLRenderer();

		/**
		*  @brief
		*    Return the OpenGL context instance
		*
		*  @return
		*    The OpenGL context instance, do not free the memory the reference is pointing to
		*/
		inline const IContext &getContext() const;


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
		virtual unsigned int getNumberOfShaderLanguages() const override;
		virtual const char *getShaderLanguageName(unsigned int index) const override;
		virtual Renderer::IShaderLanguage *getShaderLanguage(const char *shaderLanguageName = nullptr) override;
		//[-------------------------------------------------------]
		//[ Resource creation                                     ]
		//[-------------------------------------------------------]
		virtual Renderer::ISwapChain *createSwapChain(handle nativeWindowHandle) override;
		virtual Renderer::IFramebuffer *createFramebuffer(unsigned int numberOfColorTextures, Renderer::ITexture **colorTextures, Renderer::ITexture *depthStencilTexture = nullptr) override;
		virtual Renderer::IVertexBuffer *createVertexBuffer(unsigned int numberOfBytes, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) override;
		virtual Renderer::IIndexBuffer *createIndexBuffer(unsigned int numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) override;
		virtual Renderer::ITextureBuffer *createTextureBuffer(unsigned int numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) override;
		virtual Renderer::ITexture2D *createTexture2D(unsigned int width, unsigned int height, Renderer::TextureFormat::Enum textureFormat, void *data = nullptr, unsigned int flags = 0, Renderer::TextureUsage::Enum textureUsage = Renderer::TextureUsage::DEFAULT) override;
		virtual Renderer::ITexture2DArray *createTexture2DArray(unsigned int width, unsigned int height, unsigned int numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data = nullptr, unsigned int flags = 0, Renderer::TextureUsage::Enum textureUsage = Renderer::TextureUsage::DEFAULT) override;
		virtual Renderer::IRasterizerState *createRasterizerState(const Renderer::RasterizerState &rasterizerState) override;
		virtual Renderer::IDepthStencilState *createDepthStencilState(const Renderer::DepthStencilState &depthStencilState) override;
		virtual Renderer::IBlendState *createBlendState(const Renderer::BlendState &blendState) override;
		virtual Renderer::ISamplerState *createSamplerState(const Renderer::SamplerState &samplerState) override;
		virtual Renderer::ITextureCollection *createTextureCollection(unsigned int numberOfTextures, Renderer::ITexture **textures) override;
		virtual Renderer::ISamplerStateCollection *createSamplerStateCollection(unsigned int numberOfSamplerStates, Renderer::ISamplerState **samplerStates) override;
		//[-------------------------------------------------------]
		//[ Resource handling                                     ]
		//[-------------------------------------------------------]
		virtual bool map(Renderer::IResource &resource, unsigned int subresource, Renderer::MapType::Enum mapType, unsigned int mapFlags, Renderer::MappedSubresource &mappedSubresource) override;
		virtual void unmap(Renderer::IResource &resource, unsigned int subresource) override;
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
		virtual void vsSetTexture(unsigned int unit, Renderer::ITexture *texture) override;
		virtual void vsSetTextureCollection(unsigned int startUnit, Renderer::ITextureCollection *textureCollection) override;
		virtual void vsSetSamplerState(unsigned int unit, Renderer::ISamplerState *samplerState) override;
		virtual void vsSetSamplerStateCollection(unsigned int startUnit, Renderer::ISamplerStateCollection *samplerStateCollection) override;
		virtual void vsSetUniformBuffer(unsigned int slot, Renderer::IUniformBuffer *uniformBuffer) override;
		//[-------------------------------------------------------]
		//[ Tessellation-control-shader (TCS) stage               ]
		//[-------------------------------------------------------]
		virtual void tcsSetTexture(unsigned int unit, Renderer::ITexture *texture) override;
		virtual void tcsSetTextureCollection(unsigned int startUnit, Renderer::ITextureCollection *textureCollection) override;
		virtual void tcsSetSamplerState(unsigned int unit, Renderer::ISamplerState *samplerState) override;
		virtual void tcsSetSamplerStateCollection(unsigned int startUnit, Renderer::ISamplerStateCollection *samplerStateCollection) override;
		virtual void tcsSetUniformBuffer(unsigned int slot, Renderer::IUniformBuffer *uniformBuffer) override;
		//[-------------------------------------------------------]
		//[ Tessellation-evaluation-shader (TES) stage            ]
		//[-------------------------------------------------------]
		virtual void tesSetTexture(unsigned int unit, Renderer::ITexture *texture) override;
		virtual void tesSetTextureCollection(unsigned int startUnit, Renderer::ITextureCollection *textureCollection) override;
		virtual void tesSetSamplerState(unsigned int unit, Renderer::ISamplerState *samplerState) override;
		virtual void tesSetSamplerStateCollection(unsigned int startUnit, Renderer::ISamplerStateCollection *samplerStateCollection) override;
		virtual void tesSetUniformBuffer(unsigned int slot, Renderer::IUniformBuffer *uniformBuffer) override;
		//[-------------------------------------------------------]
		//[ Geometry-shader (GS) stage                            ]
		//[-------------------------------------------------------]
		virtual void gsSetTexture(unsigned int unit, Renderer::ITexture *texture) override;
		virtual void gsSetTextureCollection(unsigned int startUnit, Renderer::ITextureCollection *textureCollection) override;
		virtual void gsSetSamplerState(unsigned int unit, Renderer::ISamplerState *samplerState) override;
		virtual void gsSetSamplerStateCollection(unsigned int startUnit, Renderer::ISamplerStateCollection *samplerStateCollection) override;
		virtual void gsSetUniformBuffer(unsigned int slot, Renderer::IUniformBuffer *uniformBuffer) override;
		//[-------------------------------------------------------]
		//[ Rasterizer (RS) stage                                 ]
		//[-------------------------------------------------------]
		virtual void rsSetViewports(unsigned int numberOfViewports, const Renderer::Viewport *viewports) override;
		virtual void rsSetScissorRectangles(unsigned int numberOfScissorRectangles, const Renderer::ScissorRectangle *scissorRectangles) override;
		virtual void rsSetState(Renderer::IRasterizerState *rasterizerState) override;
		//[-------------------------------------------------------]
		//[ Fragment-shader (FS) stage                            ]
		//[-------------------------------------------------------]
		virtual void fsSetTexture(unsigned int unit, Renderer::ITexture *texture) override;
		virtual void fsSetTextureCollection(unsigned int startUnit, Renderer::ITextureCollection *textureCollection) override;
		virtual void fsSetSamplerState(unsigned int unit, Renderer::ISamplerState *samplerState) override;
		virtual void fsSetSamplerStateCollection(unsigned int startUnit, Renderer::ISamplerStateCollection *samplerStateCollection) override;
		virtual void fsSetUniformBuffer(unsigned int slot, Renderer::IUniformBuffer *uniformBuffer) override;
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
		virtual void clear(unsigned int flags, const float color[4], float z, unsigned int stencil) override;
		virtual bool beginScene() override;
		virtual void endScene() override;
		//[-------------------------------------------------------]
		//[ Draw call                                             ]
		//[-------------------------------------------------------]
		virtual void draw(unsigned int startVertexLocation, unsigned int numberOfVertices) override;
		virtual void drawInstanced(unsigned int startVertexLocation, unsigned int numberOfVertices, unsigned int numberOfInstances) override;
		virtual void drawIndexed(unsigned int startIndexLocation, unsigned int numberOfIndices, unsigned int baseVertexLocation, unsigned int minimumIndex, unsigned int numberOfVertices) override;
		virtual void drawIndexedInstanced(unsigned int startIndexLocation, unsigned int numberOfIndices, unsigned int baseVertexLocation, unsigned int minimumIndex, unsigned int numberOfVertices, unsigned int numberOfInstances) override;
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

		/**
		*  @brief
		*    Unset the currently used vertex array
		*/
		void iaUnsetVertexArray();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IContext					  *mContext;				///< OpenGL context instance, always valid
		Renderer::IShaderLanguage	  *mShaderLanguageGlsl;		///< GLSL shader language instance (we keep a reference to it), can be a null pointer
		#ifndef OPENGLRENDERER_NO_CG
			CgRuntimeLinking		  *mCgRuntimeLinking;		///< Cg runtime linking instance, always valid
			Renderer::IShaderLanguage *mShaderLanguageCg;		///< Cg shader language instance (we keep a reference to it), can be a null pointer
		#endif
		Renderer::ISamplerState		  *mDefaultSamplerState;	///< Default rasterizer state (we keep a reference to it), can be a null pointer
		//[-------------------------------------------------------]
		//[ Input-assembler (IA) stage                            ]
		//[-------------------------------------------------------]
		VertexArray	 *mVertexArray;				///< Currently set vertex array (we keep a reference to it), can be a null pointer
		unsigned int  mOpenGLPrimitiveTopology;	///< OpenGL primitive topology describing the type of primitive to render (type "GLenum" not used in here in order to keep the header slim)
		//[-------------------------------------------------------]
		//[ Rasterizer (RS) stage                                 ]
		//[-------------------------------------------------------]
		Renderer::IRasterizerState *mDefaultRasterizerState;	///< Default rasterizer state (we keep a reference to it), can be a null pointer
		RasterizerState			   *mRasterizerState;			///< Currently set rasterizer state (we keep a reference to it), can be a null pointer
		//[-------------------------------------------------------]
		//[ Output-merger (OM) stage                              ]
		//[-------------------------------------------------------]
		SwapChain					 *mMainSwapChain;				///< In case the optional native main window handle within the "OpenGLRenderer"-constructor was not a null handle, this holds the instance of the main swap chain (we keep a reference to it), can be a null pointer
		Renderer::IRenderTarget		 *mRenderTarget;				///< Currently set render target (we keep a reference to it), can be a null pointer
		Renderer::IDepthStencilState *mDefaultDepthStencilState;	///< Default depth stencil state (we keep a reference to it), can be a null pointer
		DepthStencilState			 *mDepthStencilState;			///< Currently set depth stencil state (we keep a reference to it), can be a null pointer
		Renderer::IBlendState		 *mDefaultBlendState;			///< Default blend state (we keep a reference to it), can be a null pointer
		BlendState					 *mBlendState;					///< Currently set blend state (we keep a reference to it), can be a null pointer
		//[-------------------------------------------------------]
		//[ States                                                ]
		//[-------------------------------------------------------]
		unsigned int mOpenGLProgram;	///< Currently set OpenGL program, can be zero if no resource is set (type "GLuint" not used in here in order to keep the header slim)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/OpenGLRenderer.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_OPENGLRENDERER_H__
