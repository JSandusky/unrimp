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
#ifndef __ASSIMPMESH_H__
#define __ASSIMPMESH_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/Stopwatch.h"
#include "Framework/IApplicationRenderer.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
class Mesh;


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Assimp mesh examnple
*
*  @remarks
*    Demonstrates:
*    - Vertex buffer object (VBO)
*    - Vertex array object (VAO)
*    - Index buffer object (IBO)
*    - Uniform buffer object (UBO)
*    - Texture buffer object (TBO)
*    - Sampler state object
*    - Vertex shader (VS) and fragment shader (FS)
*    - Texture collection
*    - Sampler state collection
*    - Blinn-Phong shading
*    - Diffuse, normal, specular and emissive mapping
*    - Optimization: Cache data to not bother the renderer API to much
*    - Open Asset Import Library (ASSIMP)
*/
class AssimpMesh : public IApplicationRenderer
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
	explicit AssimpMesh(const char *rendererName);

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~AssimpMesh();


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
public:
	virtual void onInitialization() override;
	virtual void onDeinitialization() override;
	virtual void onUpdate() override;
	virtual void onDraw() override;


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	Renderer::IUniformBufferPtr			  mUniformBuffer;			///< Uniform buffer object (UBO), can be a null pointer
	Renderer::IProgramPtr				  mProgram;					///< Program, can be a null pointer
	Mesh								 *mMesh;					///< Mesh instance, can be a null pointer
	Renderer::ITextureCollectionPtr		  mTextureCollection;		///< Texture collection, can be a null pointer
	Renderer::ISamplerStateCollectionPtr  mSamplerStateCollection;	///< Sampler state collection, can be a null pointer
	// Optimization: Cache data to not bother the renderer API to much
	unsigned int mUniformBlockIndex;							///< Uniform block index
	handle		 mObjectSpaceToClipSpaceMatrixUniformHandle;	///< Object space to clip space matrix uniform handle, can be NULL_HANDLE
	handle		 mObjectSpaceToViewSpaceMatrixUniformHandle;	///< Object space to view space matrix uniform handle, can be NULL_HANDLE
	// For timing
	Stopwatch mStopwatch;	///< Stopwatch instance
	float	  mGlobalTimer;	///< Global timer


};


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __ASSIMPMESH_H__
