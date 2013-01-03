/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __INSTANCEDCUBES_CUBERENDERER_INSTANCEDARRAYS_H__
#define __INSTANCEDCUBES_CUBERENDERER_INSTANCEDARRAYS_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "InstancedCubes/ICubeRenderer.h"

#include <Renderer/Renderer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
class BatchInstancedArrays;


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Cube renderer class using instancing
*
*  @remarks
*    Required renderer features:
*    - Instanced arrays (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
*/
class CubeRendererInstancedArrays : public ICubeRenderer
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Constructor
	*
	*  @param[in] renderer
	*    Renderer instance to use
	*  @param[in] numberOfTextures
	*    Number of textures, must be <ICubeRenderer::MAXIMUM_NUMBER_OF_TEXTURES
	*  @param[in] sceneRadius
	*    Scene radius
	*/
	CubeRendererInstancedArrays(Renderer::IRenderer &renderer, unsigned int numberOfTextures, unsigned int sceneRadius);

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~CubeRendererInstancedArrays();


//[-------------------------------------------------------]
//[ Public virtual ICubeRenderer methods                  ]
//[-------------------------------------------------------]
public:
	virtual void setNumberOfCubes(unsigned int numberOfCubes) override;
	virtual void draw(float globalTimer, float globalScale, float lightPositionX, float lightPositionY, float lightPositionZ) override;


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	Renderer::IRendererPtr		  mRenderer;							/**< Renderer instance to use, always valid */
	unsigned int				  mNumberOfTextures;					/**< Number of textures */
	unsigned int				  mSceneRadius;							/**< Scene radius */
	unsigned int				  mMaximumNumberOfInstancesPerBatch;	/**< Maximum number of instances per batch */
	Renderer::ITexture2DPtr		  mTexture2D;							/**< 2D texture, can be a null pointer */
	Renderer::ISamplerStatePtr	  mSamplerState;						/**< Sampler state, can be a null pointer */
	Renderer::IUniformBufferPtr	  mUniformBufferStaticVs;				/**< Static vertex shader uniform buffer object (UBO), can be a null pointer */
	Renderer::IUniformBufferPtr	  mUniformBufferDynamicVs;				/**< Dynamic vertex shader uniform buffer object (UBO), can be a null pointer */
	Renderer::IUniformBufferPtr	  mUniformBufferDynamicFs;				/**< Dynamic fragment shader uniform buffer object (UBO), can be a null pointer */
	Renderer::IProgramPtr		  mProgram;								/**< Program, can be a null pointer */
	Renderer::IVertexBufferPtr	  mVertexBuffer;						/**< Vertex buffer object (VBO), can be a null pointer */
	Renderer::IIndexBufferPtr	  mIndexBuffer;							/**< Index buffer object (IBO), can be a null pointer */
	unsigned int				  mNumberOfBatches;						/**< Current number of batches */
	BatchInstancedArrays		 *mBatches;								/**< Batches, can be a null pointer */


};


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __INSTANCEDCUBES_CUBERENDERER_INSTANCEDARRAYS_H__
