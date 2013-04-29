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
#ifndef __FXAA_H__
#define __FXAA_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/IApplicationRenderer.h"


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Shows how to integrate "Fast Approximate Anti-Aliasing" (FXAA)
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
*    - "Fast Approximate Anti-Aliasing" (FXAA) as described within http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf and
*      "FxaaSamplePack.zip" ( https://docs.google.com/leaf?id=0B2manFVVrzQAMzUzYWQ5MTEtYmE1NC00ZTQzLWI2YWYtNTk2MDcyMmY1ZWNm&sort=name&layout=list&num=50&pli=1 )
*/
class Fxaa : public IApplicationRenderer
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
	explicit Fxaa(const char *rendererName);

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~Fxaa();


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
	*    Create/recreate the post-processing program instance by using the current window size
	*/
	void recreatePostProcessingProgram();

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


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __FXAA_H__
