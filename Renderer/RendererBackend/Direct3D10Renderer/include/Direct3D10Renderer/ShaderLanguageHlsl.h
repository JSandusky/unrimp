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
#ifndef __DIRECT3D10RENDERER_SHADERLANGUAGE_HLSL_H__
#define __DIRECT3D10RENDERER_SHADERLANGUAGE_HLSL_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D10Renderer/ShaderLanguage.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
typedef __interface ID3D10Blob *LPD3D10BLOB;	// "__interface" is no keyword of the ISO C++ standard, shouldn't be a problem because this in here is MS Windows only and it's also within the Direct3D headers we have to use
typedef ID3D10Blob ID3DBlob;


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
		static ID3DBlob *loadShader(const char *shaderModel, const char *shaderSource, const char *entryPoint);


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
		virtual Renderer::IProgram *createProgram(Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader) override;
		virtual Renderer::IUniformBuffer *createUniformBuffer(uint32_t numberOfBytes, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) override;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D10RENDERER_SHADERLANGUAGE_HLSL_H__
