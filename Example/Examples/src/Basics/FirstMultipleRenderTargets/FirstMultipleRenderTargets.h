/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "Framework/IApplicationRenderer.h"


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    A first example showing how to render into multiple render targets (MRT)
*
*  @remarks
*    Demonstrates:
*    - Vertex buffer object (VBO)
*    - Vertex array object (VAO)
*    - 2D texture
*    - Sampler state object
*    - Vertex shader (VS) and fragment shader (FS)
*    - Root signature
*    - Pipeline state object (PSO)
*    - Framebuffer object (FBO) used for render to texture
*    - Multiple render targets (MRT)
*/
class FirstMultipleRenderTargets : public IApplicationRenderer
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Constructor
	*
	*  @param[in] rendererName
	*    Case sensitive ASCII name of the renderer to instance, if null pointer or unknown renderer no renderer will be used.
	*    Example renderer names: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11", "Direct3D12", "Vulkan"
	*/
	explicit FirstMultipleRenderTargets(const char *rendererName);

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~FirstMultipleRenderTargets();


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
public:
	virtual void onInitialization() override;
	virtual void onDeinitialization() override;
	virtual void onDraw() override;


//[-------------------------------------------------------]
//[ Private definitions                                   ]
//[-------------------------------------------------------]
private:
	static const uint32_t TEXTURE_SIZE		 = 16;	///< Texture size
	static const uint32_t NUMBER_OF_TEXTURES = 2;	///< Number of textures


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	Renderer::IBufferManagerPtr  mBufferManager;						///< Buffer manager, can be a null pointer
	Renderer::ITextureManagerPtr mTextureManager;						///< Texture manager, can be a null pointer
	Renderer::ITexture2DPtr		 mTexture2D[NUMBER_OF_TEXTURES];		///< 2D textures, can be a null pointer
	Renderer::IFramebufferPtr	 mFramebuffer;							///< Framebuffer object (FBO), can be a null pointer
	Renderer::ISamplerStatePtr	 mSamplerState;							///< Sampler state, can be a null pointer
	Renderer::IRootSignaturePtr	 mRootSignature;						///< Root signature, can be a null pointer
	Renderer::IPipelineStatePtr  mPipelineStateMultipleRenderTargets;	///< Pipeline state object (PSO) multiple render targets, can be a null pointer
	Renderer::IPipelineStatePtr  mPipelineState;						///< Pipeline state object (PSO), can be a null pointer
	Renderer::IVertexArrayPtr	 mVertexArray;							///< Vertex array object (VAO), can be a null pointer


};
