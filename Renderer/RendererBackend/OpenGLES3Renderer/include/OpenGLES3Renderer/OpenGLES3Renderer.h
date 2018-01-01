/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
	class IProgram;
	class IVertexArray;
	class IRenderTarget;
	class IIndirectBuffer;
	class IResourceGroup;
}
namespace OpenGLES3Renderer
{
	class SwapChain;
	class VertexArray;
	class RootSignature;
	class PipelineState;
	class IOpenGLES3Context;
}


//[-------------------------------------------------------]
//[ Definitions                                           ]
//[-------------------------------------------------------]
#ifdef WIN32
	#ifndef CALLBACK
		#define CALLBACK __stdcall
	#endif
#elif defined LINUX
	#define CALLBACK
#else
	#error "Unsupported platform"
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    OpenGL ES 3 renderer class
	*/
	class OpenGLES3Renderer : public Renderer::IRenderer
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class PipelineState;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] context
		*    Renderer context, the renderer context instance must stay valid as long as the renderer instance exists
		*
		*  @note
		*    - Do never ever use a not properly initialized renderer! Use "Renderer::IRenderer::isInitialized()" to check the initialization state.
		*/
		explicit OpenGLES3Renderer(const Renderer::Context& context);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~OpenGLES3Renderer() override;

		/**
		*  @brief
		*    Return the OpenGL ES 3 context instance
		*
		*  @return
		*    The OpenGL ES 3 context instance, do not free the memory the reference is pointing to
		*/
		inline IOpenGLES3Context& getOpenGLES3Context() const;

		//[-------------------------------------------------------]
		//[ States                                                ]
		//[-------------------------------------------------------]
		void setGraphicsRootSignature(Renderer::IRootSignature* rootSignature);
		void setGraphicsResourceGroup(uint32_t rootParameterIndex, Renderer::IResourceGroup* resourceGroup);
		void setPipelineState(Renderer::IPipelineState* pipelineState);
		//[-------------------------------------------------------]
		//[ Input-assembler (IA) stage                            ]
		//[-------------------------------------------------------]
		void iaSetVertexArray(Renderer::IVertexArray* vertexArray);
		//[-------------------------------------------------------]
		//[ Rasterizer (RS) stage                                 ]
		//[-------------------------------------------------------]
		void rsSetViewports(uint32_t numberOfViewports, const Renderer::Viewport* viewports);
		void rsSetScissorRectangles(uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle* scissorRectangles);
		//[-------------------------------------------------------]
		//[ Output-merger (OM) stage                              ]
		//[-------------------------------------------------------]
		void omSetRenderTarget(Renderer::IRenderTarget* renderTarget);
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
		void setDebugMarker(const char* name);
		void beginDebugEvent(const char* name);
		void endDebugEvent();


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderer methods            ]
	//[-------------------------------------------------------]
	public:
		virtual const char* getName() const override;
		virtual bool isInitialized() const override;
		virtual bool isDebugEnabled() override;
		//[-------------------------------------------------------]
		//[ Shader language                                       ]
		//[-------------------------------------------------------]
		virtual uint32_t getNumberOfShaderLanguages() const override;
		virtual const char* getShaderLanguageName(uint32_t index) const override;
		virtual Renderer::IShaderLanguage* getShaderLanguage(const char* shaderLanguageName = nullptr) override;
		//[-------------------------------------------------------]
		//[ Resource creation                                     ]
		//[-------------------------------------------------------]
		virtual Renderer::IRenderPass* createRenderPass(uint32_t numberOfColorAttachments, const Renderer::TextureFormat::Enum* colorAttachmentTextureFormats, Renderer::TextureFormat::Enum depthStencilAttachmentTextureFormat = Renderer::TextureFormat::UNKNOWN, uint8_t numberOfMultisamples = 1) override;
		virtual Renderer::ISwapChain* createSwapChain(Renderer::IRenderPass& renderPass, Renderer::WindowHandle windowHandle, bool useExternalContext = false) override;
		virtual Renderer::IFramebuffer* createFramebuffer(Renderer::IRenderPass& renderPass, const Renderer::FramebufferAttachment* colorFramebufferAttachments, const Renderer::FramebufferAttachment* depthStencilFramebufferAttachment = nullptr) override;
		virtual Renderer::IBufferManager* createBufferManager() override;
		virtual Renderer::ITextureManager* createTextureManager() override;
		virtual Renderer::IRootSignature* createRootSignature(const Renderer::RootSignature& rootSignature) override;
		virtual Renderer::IPipelineState* createPipelineState(const Renderer::PipelineState& pipelineState) override;
		virtual Renderer::ISamplerState* createSamplerState(const Renderer::SamplerState& samplerState) override;
		//[-------------------------------------------------------]
		//[ Resource handling                                     ]
		//[-------------------------------------------------------]
		virtual bool map(Renderer::IResource& resource, uint32_t subresource, Renderer::MapType mapType, uint32_t mapFlags, Renderer::MappedSubresource& mappedSubresource) override;
		virtual void unmap(Renderer::IResource& resource, uint32_t subresource) override;
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
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	protected:
		virtual void selfDestruct() override;


	//[-------------------------------------------------------]
	//[ Private static methods                                ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Debug message callback function called by the "GL_KHR_debug"-extension
		*
		*  @param[in] source
		*    Source of the debug message
		*  @param[in] type
		*    Type of the debug message
		*  @param[in] id
		*    ID of the debug message
		*  @param[in] severity
		*    Severity of the debug message
		*  @param[in] length
		*    Length of the debug message
		*  @param[in] message
		*    The debug message
		*  @param[in] userParam
		*    Additional user parameter of the debug message
		*/
		static void CALLBACK debugMessageCallback(uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int length, const char* message, const void* userParam);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit OpenGLES3Renderer(const OpenGLES3Renderer& source) = delete;
		OpenGLES3Renderer& operator =(const OpenGLES3Renderer& source) = delete;

		/**
		*  @brief
		*    Initialize the capabilities
		*/
		void initializeCapabilities();

		/**
		*  @brief
		*    Set program
		*
		*  @param[in] program
		*    Program to set
		*/
		void setProgram(Renderer::IProgram* program);

		/**
		*  @brief
		*    Update "GL_EXT_base_instance" emulation
		*
		*  @param[in] startInstanceLocation
		*    Start instance location
		*/
		void updateGL_EXT_base_instanceEmulation(uint32_t startInstanceLocation);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IOpenGLES3Context*		   mOpenGLES3Context;					///< OpenGL ES 3 context instance, always valid
		Renderer::IShaderLanguage* mShaderLanguageGlsl;					///< GLSL shader language instance (we keep a reference to it), can be a null pointer
		RootSignature*			   mGraphicsRootSignature;				///< Currently set graphics root signature (we keep a reference to it), can be a null pointer
		Renderer::ISamplerState*   mDefaultSamplerState;				///< Default rasterizer state (we keep a reference to it), can be a null pointer
		uint32_t				   mOpenGLES3CopyResourceFramebuffer;	///< OpenGL ES 3 framebuffer ("container" object, not shared between OpenGL ES 3 contexts) used by "OpenGLES3Renderer::OpenGLES3Renderer::copyResource()", can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)
		// States
		PipelineState* mPipelineState;	///< Currently set pipeline state (we keep a reference to it), can be a null pointer
		// Input-assembler (IA) stage
		VertexArray* mVertexArray;					///< Currently set vertex array (we keep a reference to it), can be a null pointer
		uint32_t	 mOpenGLES3PrimitiveTopology;	///< OpenGL ES 3 primitive topology describing the type of primitive to render (type "GLenum" not used in here in order to keep the header slim)
		// Output-merger (OM) stage
		Renderer::IRenderTarget* mRenderTarget;	///< Currently set render target (we keep a reference to it), can be a null pointer
		// State cache to avoid making redundant OpenGL ES 3 calls
		uint32_t mOpenGLES3Program;	///< Currently set OpenGL ES 3 program, can be zero if no resource is set (type "GLuint" not used in here in order to keep the header slim)
		// Draw ID uniform location for "GL_EXT_base_instance"-emulation (see "17/11/2012 Surviving without gl_DrawID" - https://www.g-truc.net/post-0518.html)
		int		 mDrawIdUniformLocation;		///< Draw ID uniform location (type "GLuint" not used in here in order to keep the header slim)
		uint32_t mCurrentStartInstanceLocation;	///< Currently set start instance location


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLES3Renderer/OpenGLES3Renderer.inl"
