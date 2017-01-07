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
#include "Framework/Stopwatch.h"
#include "Framework/ExampleBase.h"


//[-------------------------------------------------------]
//[ Global definitions                                    ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	typedef uint32_t MeshResourceId;	///< POD mesh resource identifier
	typedef uint32_t TextureResourceId;	///< POD texture resource identifier
}


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    First mesh example
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
*    - Root signature
*    - Pipeline state object (PSO)
*    - Blinn-Phong shading
*    - Diffuse, normal, specular and emissive mapping
*    - Optimization: Cache data to not bother the renderer API to much
*    - Compact vertex format (32 bit texture coordinate, QTangent, 56 bytes vs. 28 bytes per vertex)
*/
class FirstMesh : public ExampleBase
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Constructor
	*/
	FirstMesh();

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~FirstMesh();


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
	Renderer::IRootSignaturePtr			  mRootSignature;			///< Root signature, can be a null pointer
	Renderer::IPipelineStatePtr			  mPipelineState;			///< Pipeline state object (PSO), can be a null pointer
	Renderer::IProgramPtr				  mProgram;					///< Program, can be a null pointer
	RendererRuntime::MeshResourceId		  mMeshResourceId;			///< Mesh resource ID, can be set to uninitialized value
	RendererRuntime::TextureResourceId	  mDiffuseTextureResourceId;
	RendererRuntime::TextureResourceId	  mNormalTextureResourceId;
	RendererRuntime::TextureResourceId	  mSpecularTextureResourceId;
	RendererRuntime::TextureResourceId	  mEmissiveTextureResourceId;
	Renderer::ISamplerStatePtr			  mSamplerState;			///< Sampler state, can be a null pointer
	Renderer::CommandBuffer				  mCommandBuffer;			///< Command buffer
	// Optimization: Cache data to not bother the renderer API to much
	handle	 mObjectSpaceToClipSpaceMatrixUniformHandle;	///< Object space to clip space matrix uniform handle, can be NULL_HANDLE
	handle	 mObjectSpaceToViewSpaceMatrixUniformHandle;	///< Object space to view space matrix uniform handle, can be NULL_HANDLE
	// For timing
	Stopwatch mStopwatch;	///< Stopwatch instance
	float	  mGlobalTimer;	///< Global timer


};
