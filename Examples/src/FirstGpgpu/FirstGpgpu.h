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
#ifndef __FIRSTGPGPU_H__
#define __FIRSTGPGPU_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/Renderer.h>


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    A first example showing how to use do General Purpose Computation on Graphics Processing Unit (GPGPU) by using the renderer interface and shaders without having any output window
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
*    - General Purpose Computation on Graphics Processing Unit (GPGPU) by using the renderer interface and shaders without having any output window
*/
class FirstGpgpu
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
	explicit FirstGpgpu(const char *rendererName);

	/**
	*  @brief
	*    Destructor
	*/
	~FirstGpgpu();

	/**
	*  @brief
	*    Run the application
	*
	*  @return
	*    Program return code, 0 to indicate that no error has occurred
	*/
	int run();


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
private:
	/**
	*  @brief
	*    Called on application initialization
	*
	*  @note
	*    - When this method is called it's ensured that the renderer instance "mRenderer" is valid
	*/
	void onInitialization();

	/**
	*  @brief
	*    Called on application de-initialization
	*
	*  @note
	*    - When this method is called it's ensured that the renderer instance "mRenderer" is valid
	*/
	void onDeinitialization();

	/**
	*  @brief
	*    Called on application should to it's job
	*
	*  @note
	*    - When this method is called it's ensured that the renderer instance "mRenderer" is valid
	*/
	void onDoJob();

	/**
	*  @brief
	*    Generate the content of the 2D texture to process later on
	*
	*  @note
	*    - When this method is called it's ensured that the renderer instance "mRenderer" is valid
	*/
	void generate2DTextureContent();

	/**
	*  @brief
	*    Content processing
	*
	*  @note
	*    - When this method is called it's ensured that the renderer instance "mRenderer" is valid
	*/
	void contentProcessing();


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	char							mRendererName[64];	///< Case sensitive ASCII name of the renderer to instance
	Renderer::IRendererPtr			mRenderer;			///< Renderer instance, can be a null pointer
	Renderer::ITexture2DPtr			mTexture2D[2];		///< 2D texture, can be a null pointer
	Renderer::IFramebufferPtr		mFramebuffer[2];	///< Framebuffer object (FBO), can be a null pointer
	Renderer::ISamplerStatePtr		mSamplerState;		///< Sampler state, can be a null pointer
	Renderer::IDepthStencilStatePtr	mDepthStencilState;	///< Depth stencil state, can be a null pointer
	// Content generation
	Renderer::IProgramPtr	   mProgramContentGeneration;		///< Program for content generation, can be a null pointer
	Renderer::IVertexArrayPtr  mVertexArrayContentGeneration;	///< Vertex array object (VAO) for content generation, can be a null pointer
	// Content processing
	Renderer::IProgramPtr	   mProgramContentProcessing;		///< Program for content processing, can be a null pointer
	Renderer::IVertexArrayPtr  mVertexArrayContentProcessing;	///< Vertex array object (VAO) for content processing, can be a null pointer


};


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __FIRSTGPGPU_H__
