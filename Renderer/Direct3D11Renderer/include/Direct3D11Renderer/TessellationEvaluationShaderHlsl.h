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
#ifndef __DIRECT3D11RENDERER_TESSELATIONEVALUATIONSHADER_HLSL_H__
#define __DIRECT3D11RENDERER_TESSELATIONEVALUATIONSHADER_HLSL_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/TessellationEvaluationShader.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct ID3D11DomainShader;


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
	*    HLSL tessellation evaluation shader ("domain shader" in Direct3D terminology) class
	*/
	class TessellationEvaluationShaderHlsl : public TessellationEvaluationShader
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
		*  @param[in] sourceCode
		*    Shader ASCII source code, must be valid
		*/
		TessellationEvaluationShaderHlsl(Direct3D11Renderer &direct3D11Renderer, const char *sourceCode);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~TessellationEvaluationShaderHlsl();

		/**
		*  @brief
		*    Return the Direct3D 11 domain shader
		*
		*  @return
		*    Direct3D 11 domain shader, can be a null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D11DomainShader *getD3D11DomainShader() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char *name) override;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	public:
		virtual const char *getShaderLanguageName() const override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		ID3D11DomainShader *mD3D11DomainShader;	///< Direct3D 11 domain shader, can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/TessellationEvaluationShaderHlsl.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D11RENDERER_TESSELATIONEVALUATIONSHADER_HLSL_H__
