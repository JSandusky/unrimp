/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __OPENGLRENDERER_SHADERLANGUAGE_GLSL_H__
#define __OPENGLRENDERER_SHADERLANGUAGE_GLSL_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/ShaderLanguage.h"
// [TODO] put in common header used by all
#ifdef LINUX
	// "override"-definition, see e.g. http://www2.research.att.com/~bs/C++0xFAQ.html#override (C++11 keyword is introduced in GCC 4.7 or clang 2.9)
	#if ((__GNUC__ < 4 || __GNUC_MINOR__ < 7) && !defined(__clang__))
		#define override
	#endif
#endif

//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
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
		static const char *NAME;	/**< ASCII name of this shader language, always valid (do not free the memory the returned pointer is pointing to) */


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Creates, loads and compiles a shader
		*
		*  @param[in] shaderType
		*    Shader type (for example "GL_VERTEX_SHADER_ARB", type GLenum not used in here in order to keep the header slim)
		*  @param[in] shaderSource
		*    Shader ASCII source code, must be a valid pointer (type GLchar not used in here in order to keep the header slim)
		*
		*  @return
		*    The OpenGL shader, 0 on error, destroy the resource if you no longer need it (type "GLuint" not used in here in order to keep the header slim)
		*/
		static unsigned int loadShader(unsigned int shaderType, const char *shaderSource);


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*/
		explicit ShaderLanguageGlsl(OpenGLRenderer &openGLRenderer);

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
		virtual Renderer::IVertexShader *createVertexShader(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::ITessellationControlShader *createTessellationControlShader(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::ITessellationEvaluationShader *createTessellationEvaluationShader(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IGeometryShader *createGeometryShader(const char *sourceCode, Renderer::GsInputPrimitiveTopology::Enum gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology::Enum gsOutputPrimitiveTopology, unsigned int numberOfOutputVertices, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IFragmentShader *createFragmentShader(const char *sourceCode, const char *profile = nullptr, const char *arguments = nullptr, const char *entry = nullptr) override;
		virtual Renderer::IProgram *createProgram(Renderer::IVertexShader *vertexShader, Renderer::ITessellationControlShader *tessellationControlShader, Renderer::ITessellationEvaluationShader *tessellationEvaluationShader, Renderer::IGeometryShader *geometryShader, Renderer::IFragmentShader *fragmentShader) override;
		virtual Renderer::IUniformBuffer *createUniformBuffer(unsigned int numberOfBytes, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW) override;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_SHADERLANGUAGE_GLSL_H__
