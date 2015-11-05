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
#include "Direct3D9Renderer/Shader/ShaderLanguage.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct ID3DXBuffer;
struct ID3DXConstantTable;


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
	class ShaderLanguageHlsl : public ShaderLanguage
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const char *NAME;	///< ASCII name of this shader language, always valid (do not free the memory the returned pointer is pointing to)


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Creates, loads and compiles a shader
		*
		*  @param[in]  shaderModel
		*    ASCII shader model (for example "vs_3_0", "ps_3_0")
		*  @param[in] shaderSource
		*    Shader ASCII source code, must be a valid pointer
		*  @param[in]  entryPoint
		*    Optional ASCII entry point, if null pointer "main" is used
		*  @param[out] d3dXConstantTable
		*    Optional constant table, can be a null pointer
		*
		*  @return
		*    The loaded and compiled shader, can be a null pointer, release the instance if you no longer need it
		*/
		static ID3DXBuffer *loadShader(const char *shaderModel, const char *shaderSource, const char *entryPoint, ID3DXConstantTable **d3dXConstantTable);


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
		explicit ShaderLanguageHlsl(Direct3D9Renderer &direct3D9Renderer);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ShaderLanguageHlsl();


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	public:
		virtual const char *getShaderLanguageName() const override;
		virtual Renderer::IVertexShader *createVertexShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes) override;
		virtual Renderer::IVertexShader *createVertexShaderFromSourceCode(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::ITessellationControlShader *createTessellationControlShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes) override;
		virtual Renderer::ITessellationControlShader *createTessellationControlShaderFromSourceCode(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::ITessellationEvaluationShader *createTessellationEvaluationShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes) override;
		virtual Renderer::ITessellationEvaluationShader *createTessellationEvaluationShaderFromSourceCode(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IGeometryShader *createGeometryShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes, Renderer::GsInputPrimitiveTopology::Enum gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology::Enum gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IGeometryShader *createGeometryShaderFromSourceCode(const char *sourceCode, Renderer::GsInputPrimitiveTopology::Enum gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology::Enum gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IFragmentShader *createFragmentShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes) override;
		virtual Renderer::IFragmentShader *createFragmentShaderFromSourceCode(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IProgram *createProgram(const Renderer::VertexAttributes& vertexAttributes, Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader) override;
		virtual Renderer::IUniformBuffer *createUniformBuffer(uint32_t numberOfBytes, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Direct3D9Renderer *mDirect3D9Renderer;	///< Owner Direct3D 9 renderer instance, always valid


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
