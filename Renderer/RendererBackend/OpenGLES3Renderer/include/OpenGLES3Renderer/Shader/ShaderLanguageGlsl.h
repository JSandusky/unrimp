/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
namespace OpenGLES3Renderer
{
	class OpenGLES3Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLES3Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    GLSL shader language class
	*/
	class ShaderLanguageGlsl final : public Renderer::IShaderLanguage
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const char* NAME;	///< ASCII name of this shader language, always valid (do not free the memory the returned pointer is pointing to)


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Creates, loads and compiles a shader from source code
		*
		*  @param[in] openGLES3Renderer
		*    Owner OpenGL ES 3 renderer instance
		*  @param[in] shaderType
		*    Shader type (for example "GL_VERTEX_SHADER", type "GLenum" not used in here in order to keep the header slim)
		*  @param[in] sourceCode
		*    Shader ASCII source code, must be a valid pointer (type "GLchar" not used in here in order to keep the header slim)
		*
		*  @return
		*    The OpenGL ES 3 shader, 0 on error, destroy the returned resource if you no longer need it (type "GLuint" not used in here in order to keep the header slim)
		*/
		static uint32_t loadShaderFromSourcecode(OpenGLES3Renderer& openGLES3Renderer, uint32_t shaderType, const char* sourceCode);


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLES3Renderer
		*    Owner OpenGL ES 3 renderer instance
		*/
		explicit ShaderLanguageGlsl(OpenGLES3Renderer& openGLES3Renderer);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ShaderLanguageGlsl() override;


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
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	protected:
		virtual void selfDestruct() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit ShaderLanguageGlsl(const ShaderLanguageGlsl& source) = delete;
		ShaderLanguageGlsl& operator =(const ShaderLanguageGlsl& source) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLES3Renderer
