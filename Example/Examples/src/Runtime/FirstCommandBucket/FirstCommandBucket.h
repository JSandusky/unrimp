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
#include "Framework/IApplicationRendererRuntime.h"

#include <RendererRuntime/Command/CommandBucket.h>
#include <RendererRuntime/Resource/Font/FontResource.h>


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Shows how to use command buckets
*
*  @remarks
*    Demonstrates:
*    - Command buckets
*
*  @todo
*    - TODO(co) Under construction
*/
class FirstCommandBucket : public IApplicationRendererRuntime
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
	*    Example renderer names: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11", "Direct3D12"
	*/
	explicit FirstCommandBucket(const char *rendererName);

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~FirstCommandBucket();


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
public:
	virtual void onInitialization() override;
	virtual void onDeinitialization() override;
	virtual void onDraw() override;


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	RendererRuntime::FontResource* mFontResource;	///< Font resource, can be a null pointer
	// Command buckets
	RendererRuntime::CommandBucket<int> mSolidCommandBucket;
	RendererRuntime::CommandBucket<int> mTransparentCommandBucket;
	// Renderer resources
	Renderer::IRootSignaturePtr	mRootSignature;
	Renderer::IPipelineStatePtr	mPipelineState;
	Renderer::IUniformBufferPtr	mUniformBufferDynamicVs;
	Renderer::IVertexArrayPtr	mSolidVertexArray;
	Renderer::IVertexArrayPtr	mTransparentVertexArray;
	// Materials
	RendererRuntime::Material mSolidMaterial;
	RendererRuntime::Material mTransparentMaterial;


};
