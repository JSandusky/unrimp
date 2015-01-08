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
#include "Framework/IApplicationRenderer.h"


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    A first example showing how to use render into a texture for post-processing
*
*  @remarks
*    Demonstrates:
*    - Vertex buffer object (VBO)
*    - Vertex array object (VAO)
*    - 2D texture
*    - Sampler state object
*    - Vertex shader (VS) and fragment shader (FS)
*    - Framebuffer object (FBO) used for render to texture
*    - Depth stencil state object
*    - Post processing
*/
class FirstPostProcessing : public IApplicationRenderer
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
	*    Example renderer names: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
	*/
	explicit FirstPostProcessing(const char *rendererName);

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~FirstPostProcessing();


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
public:
	virtual void onInitialization() override;
	virtual void onDeinitialization() override;
	virtual void onResize() override;
	virtual void onDraw() override;


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
private:
	/**
	*  @brief
	*    Create/recreate the framebuffer object (FBO) instance by using the current window size
	*/
	void recreateFramebuffer();

	/**
	*  @brief
	*    Scene rendering
	*/
	void sceneRendering();

	/**
	*  @brief
	*    Post-processing
	*/
	void postProcessing();


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	Renderer::ITexture2DPtr			mTexture2D;			///< 2D texture, can be a null pointer
	Renderer::IFramebufferPtr		mFramebuffer;		///< Framebuffer object (FBO), can be a null pointer
	Renderer::ISamplerStatePtr		mSamplerState;		///< Sampler state, can be a null pointer
	Renderer::IDepthStencilStatePtr	mDepthStencilState;	///< Depth stencil state, can be a null pointer
	// Scene rendering
	Renderer::IProgramPtr	   mProgramSceneRendering;		///< Program for scene rendering, can be a null pointer
	Renderer::IVertexArrayPtr  mVertexArraySceneRendering;	///< Vertex array object (VAO) for scene rendering, can be a null pointer
	// Post-processing
	Renderer::IProgramPtr	   mProgramPostProcessing;		///< Program for post-processing, can be a null pointer
	Renderer::IVertexArrayPtr  mVertexArrayPostProcessing;	///< Vertex array object (VAO) for post-processing, can be a null pointer


};
