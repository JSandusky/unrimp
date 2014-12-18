/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#ifndef __DIRECT3D11RENDERER_VERTEXSHADER_CG_H__
#define __DIRECT3D11RENDERER_VERTEXSHADER_CG_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/VertexShader.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct _CGprogram;
struct _CGcontext;
typedef __interface ID3D10Blob *LPD3D10BLOB;	// "__interface" is no keyword of the ISO C++ standard, shouldn't be a problem because this in here is MS Windows only and it's also within the Direct3D headers we have to use
typedef ID3D10Blob ID3DBlob;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Cg vertex shader class
	*/
	class VertexShaderCg : public VertexShader
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D11Renderer
		*    Owner Direct3D 11 renderer instance
		*  @param[in] cgContext
		*    Cg context instance
		*  @param[in] sourceCode
		*    Shader ASCII source code, must be valid
		*/
		VertexShaderCg(Direct3D11Renderer &direct3D11Renderer, _CGcontext &cgContext, const char *sourceCode);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VertexShaderCg();

		/**
		*  @brief
		*    Return the Cg program
		*
		*  @return
		*    The Cg program, can be a null pointer, do not destroy the returned resource
		*/
		inline _CGprogram *getCgProgram() const;

		/**
		*  @brief
		*    Return the Direct3D 11 vertex shader blob
		*
		*  @return
		*    Direct3D 11 vertex shader blob, can be a null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3DBlob *getD3DBlobVertexShader() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	public:
		virtual const char *getShaderLanguageName() const override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		_CGprogram *mCgProgram;				///< Cg program, can be a null pointer
		ID3DBlob   *mD3DBlobVertexShader;	///< Direct3D 11 vertex shader blob, can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/VertexShaderCg.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D11RENDERER_VERTEXSHADER_CG_H__
