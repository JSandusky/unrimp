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
#include <Renderer/IVertexShader.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct ID3D10VertexShader;
typedef __interface ID3D10Blob *LPD3D10BLOB;	// "__interface" is no keyword of the ISO C++ standard, shouldn't be a problem because this in here is MS Windows only and it's also within the Direct3D headers we have to use
typedef ID3D10Blob ID3DBlob;
namespace Direct3D10Renderer
{
	class Direct3D10Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    HLSL vertex shader class
	*/
	class VertexShaderHlsl : public Renderer::IVertexShader
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor for creating a vertex shader from shader bytecode
		*
		*  @param[in] direct3D10Renderer
		*    Owner Direct3D 10 renderer instance
		*  @param[in] bytecode
		*    Shader bytecode, must be valid
		*  @param[in] numberOfBytes
		*    Number of bytes in the bytecode
		*/
		VertexShaderHlsl(Direct3D10Renderer &direct3D10Renderer, const uint8_t *bytecode, uint32_t numberOfBytes);

		/**
		*  @brief
		*    Constructor for creating a vertex shader from shader source code
		*
		*  @param[in] direct3D10Renderer
		*    Owner Direct3D 10 renderer instance
		*  @param[in] sourceCode
		*    Shader ASCII source code, must be valid
		*/
		VertexShaderHlsl(Direct3D10Renderer &direct3D10Renderer, const char *sourceCode);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VertexShaderHlsl();

		/**
		*  @brief
		*    Return the Direct3D 10 vertex shader blob
		*
		*  @return
		*    Direct3D 10 vertex shader blob, can be a null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3DBlob *getD3DBlobVertexShader() const;

		/**
		*  @brief
		*    Return the Direct3D 10 vertex shader
		*
		*  @return
		*    Direct3D 10 vertex shader, can be a null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D10VertexShader *getD3D10VertexShader() const;


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
		ID3DBlob		   *mD3DBlobVertexShader;	///< Direct3D 10 vertex shader blob, can be a null pointer
		ID3D10VertexShader *mD3D10VertexShader;		///< Direct3D 10 vertex shader, can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D10Renderer/Shader/VertexShaderHlsl.inl"
