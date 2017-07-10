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
struct ID3DXBuffer;
struct ID3DXConstantTable;
namespace Direct3D9Renderer
{
	class Direct3D9Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
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
		static const char* NAME;	///< ASCII name of this shader language, always valid (do not free the memory the returned pointer is pointing to)


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D9Renderer
		*    Owner Direct3D 9 renderer instance
		*/
		explicit ShaderLanguageHlsl(Direct3D9Renderer& direct3D9Renderer);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ShaderLanguageHlsl();

		/**
		*  @brief
		*    Creates, loads and compiles a shader from source code
		*
		*  @param[in]  shaderModel
		*    ASCII shader model (for example "vs_3_0", "ps_3_0")
		*  @param[in] sourceCode
		*    Shader ASCII source code, must be a valid pointer
		*  @param[in]  entryPoint
		*    Optional ASCII entry point, if null pointer "main" is used
		*  @param[out] d3dXConstantTable
		*    Optional constant table, can be a null pointer
		*
		*  @return
		*    The loaded and compiled shader, can be a null pointer, release the instance if you no longer need it
		*/
		ID3DXBuffer* loadShaderFromSourcecode(const char* shaderModel, const char* sourceCode, const char* entryPoint, ID3DXConstantTable** d3dXConstantTable) const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	public:
		virtual const char* getShaderLanguageName() const override;
		virtual Renderer::IVertexShader* createVertexShaderFromBytecode(const Renderer::VertexAttributes& vertexAttributes, const Renderer::ShaderBytecode& shaderBytecode) override;
		virtual Renderer::IVertexShader* createVertexShaderFromSourceCode(const Renderer::VertexAttributes& vertexAttributes, const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode = nullptr) override;
		virtual Renderer::ITessellationControlShader* createTessellationControlShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode) override;
		virtual Renderer::ITessellationControlShader* createTessellationControlShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode = nullptr) override;
		virtual Renderer::ITessellationEvaluationShader* createTessellationEvaluationShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode) override;
		virtual Renderer::ITessellationEvaluationShader* createTessellationEvaluationShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode = nullptr) override;
		virtual Renderer::IGeometryShader* createGeometryShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices) override;
		virtual Renderer::IGeometryShader* createGeometryShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, Renderer::ShaderBytecode* shaderBytecode = nullptr) override;
		virtual Renderer::IFragmentShader* createFragmentShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode) override;
		virtual Renderer::IFragmentShader* createFragmentShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode = nullptr) override;
		virtual Renderer::IProgram* createProgram(const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, Renderer::IVertexShader* vertexShader, Renderer::ITessellationControlShader* tessellationControlShader, Renderer::ITessellationEvaluationShader* tessellationEvaluationShader, Renderer::IGeometryShader* geometryShader, Renderer::IFragmentShader* fragmentShader) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit ShaderLanguageHlsl(const ShaderLanguageHlsl& source) = delete;
		ShaderLanguageHlsl& operator =(const ShaderLanguageHlsl& source) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
