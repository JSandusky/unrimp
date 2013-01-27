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
#ifndef __ICOSAHEDRONTESSELLATION_H__
#define __ICOSAHEDRONTESSELLATION_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/IApplicationRenderer.h"


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    More advanced tessellation examnple
*
*  @remarks
*    Demonstrates:
*    - Vertex buffer object (VBO)
*    - Vertex array object (VAO)
*    - Index buffer object (IBO)
*    - Uniform buffer object (UBO)
*    - Vertex shader (VS), tessellation control shader (TCS), tessellation evaluation shader (TES), geometry shader (GS) and fragment shader (FS)
*/
class IcosahedronTessellation : public IApplicationRenderer
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
	*    Example renderer names: "Null", "OpenGL" (shader model 5), "Direct3D11"
	*/
	explicit IcosahedronTessellation(const char *rendererName);

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~IcosahedronTessellation();


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
	Renderer::IUniformBufferPtr	mUniformBufferDynamicTcs;	/**< Dynamic tessellation control shader uniform buffer object (UBO), can be a null pointer */
	Renderer::IUniformBufferPtr	mUniformBufferStaticTes;	/**< Static tessellation evaluation shader uniform buffer object (UBO), can be a null pointer */
	Renderer::IUniformBufferPtr	mUniformBufferStaticGs;		/**< Static geometry shader uniform buffer object (UBO), can be a null pointer */
	Renderer::IUniformBufferPtr	mUniformBufferStaticFs;		/**< Static fragment shader uniform buffer object (UBO), can be a null pointer */
	Renderer::IProgramPtr		mProgram;					/**< Program, can be a null pointer */
	Renderer::IVertexArrayPtr	mVertexArray;				/**< Vertex array object (VAO), can be a null pointer */
	float						mTessellationLevelOuter;	/**< Outer tessellation level */
	float						mTessellationLevelInner;	/**< Inner tessellation level */


};


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __ICOSAHEDRONTESSELLATION_H__
