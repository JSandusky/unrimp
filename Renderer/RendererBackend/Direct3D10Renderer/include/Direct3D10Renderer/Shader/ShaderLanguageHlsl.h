/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
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
#include <Renderer/Shader/IShaderLanguage.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
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
	*    HLSL shader language class
	*/
	class ShaderLanguageHlsl : public Renderer::IShaderLanguage
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const char *NAME;	///< ASCII name of this shader language, always valid (do not free the memory the returned pointer is pointing to)


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D10Renderer
		*    Owner Direct3D 10 renderer instance
		*/
		explicit ShaderLanguageHlsl(Direct3D10Renderer &direct3D10Renderer);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ShaderLanguageHlsl();

		/**
		*  @brief
		*    Creates, loads and compiles a shader
		*
		*  @param[in] shaderModel
		*    ASCII shader model (for example "vs_4_0", "gs_4_0", "ps_4_0"), must be a valid pointer
		*  @param[in] shaderSource
		*    ASCII shader ASCII source code, must be a valid pointer
		*  @param[in] entryPoint
		*    Optional ASCII entry point, if null pointer "main" is used
		*
		*  @return
		*    The loaded and compiled shader, can be a null pointer, release the instance if you no longer need it
		*/
		ID3DBlob *loadShader(const char *shaderModel, const char *shaderSource, const char *entryPoint) const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	public:
		virtual const char *getShaderLanguageName() const override;
		virtual Renderer::IVertexShader *createVertexShaderFromBytecode(const Renderer::VertexAttributes& vertexAttributes, const uint8_t *bytecode, uint32_t numberOfBytes) override;
		virtual Renderer::IVertexShader *createVertexShaderFromSourceCode(const Renderer::VertexAttributes& vertexAttributes, const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::ITessellationControlShader *createTessellationControlShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes) override;
		virtual Renderer::ITessellationControlShader *createTessellationControlShaderFromSourceCode(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::ITessellationEvaluationShader *createTessellationEvaluationShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes) override;
		virtual Renderer::ITessellationEvaluationShader *createTessellationEvaluationShaderFromSourceCode(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IGeometryShader *createGeometryShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IGeometryShader *createGeometryShaderFromSourceCode(const char *sourceCode, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IFragmentShader *createFragmentShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes) override;
		virtual Renderer::IFragmentShader *createFragmentShaderFromSourceCode(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IProgram *createProgram(const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader) override;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
