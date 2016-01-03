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
#include "OpenGLES2Renderer/Shader/ShaderLanguage.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES2Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    GLSL shader language class
	*/
	class ShaderLanguageGlsl : public ShaderLanguage
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
		*  @param[in] openGLES2Renderer
		*    Owner OpenGL ES 2 renderer instance
		*/
		explicit ShaderLanguageGlsl(OpenGLES2Renderer &openGLES2Renderer);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ShaderLanguageGlsl();


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
		virtual Renderer::IGeometryShader *createGeometryShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IGeometryShader *createGeometryShaderFromSourceCode(const char *sourceCode, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IFragmentShader *createFragmentShaderFromBytecode(const uint8_t *bytecode, uint32_t numberOfBytes) override;
		virtual Renderer::IFragmentShader *createFragmentShaderFromSourceCode(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IProgram *createProgram(const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader) override;
		virtual Renderer::IUniformBuffer *createUniformBuffer(uint32_t numberOfBytes, const void *data = nullptr, Renderer::BufferUsage bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) override;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES2Renderer
