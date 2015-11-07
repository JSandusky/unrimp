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
#include "OpenGLRenderer/Shader/Program.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRootSignature;
	struct VertexAttributes;
}
namespace OpenGLRenderer
{
	class VertexShaderGlsl;
	class GeometryShaderGlsl;
	class FragmentShaderGlsl;
	class TessellationControlShaderGlsl;
	class TessellationEvaluationShaderGlsl;
}


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
	*    GLSL program class
	*/
	class ProgramGlsl : public Program
	{


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
		*  @param[in] rootSignature
		*    Root signature
		*  @param[in] vertexAttributes
		*    Vertex attributes ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 terminology)
		*  @param[in] vertexShaderGlsl
		*    Vertex shader the program is using, can be a null pointer
		*  @param[in] tessellationControlShaderGlsl
		*    Tessellation control shader the program is using, can be a null pointer
		*  @param[in] tessellationEvaluationShaderGlsl
		*    Tessellation evaluation shader the program is using, can be a null pointer
		*  @param[in] geometryShaderGlsl
		*    Geometry shader the program is using, can be a null pointer
		*  @param[in] fragmentShaderGlsl
		*    Fragment shader the program is using, can be a null pointer
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required
		*/
		ProgramGlsl(OpenGLRenderer &openGLRenderer, const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, VertexShaderGlsl *vertexShaderGlsl, TessellationControlShaderGlsl *tessellationControlShaderGlsl, TessellationEvaluationShaderGlsl *tessellationEvaluationShaderGlsl, GeometryShaderGlsl *geometryShaderGlsl, FragmentShaderGlsl *fragmentShaderGlsl);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ProgramGlsl();

		/**
		*  @brief
		*    Return the OpenGL program
		*
		*  @return
		*    The OpenGL program, can be zero if no resource is allocated, do not destroy the returned resource (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline uint32_t getOpenGLProgram() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	public:
		virtual uint32_t getUniformBlockIndex(const char *uniformBlockName, uint32_t defaultIndex) override;
		virtual handle getUniformHandle(const char *uniformName) override;
		virtual uint32_t setTextureUnit(handle uniformHandle, uint32_t unit) override;
		virtual void setUniform1i(handle uniformHandle, int value) override;
		virtual void setUniform1f(handle uniformHandle, float value) override;
		virtual void setUniform2fv(handle uniformHandle, const float *value) override;
		virtual void setUniform3fv(handle uniformHandle, const float *value) override;
		virtual void setUniform4fv(handle uniformHandle, const float *value) override;
		virtual void setUniformMatrix3fv(handle uniformHandle, const float *value) override;
		virtual void setUniformMatrix4fv(handle uniformHandle, const float *value) override;


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		uint32_t mOpenGLProgram;	///< OpenGL program, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		uint32_t mNumberOfRootSignatureParameters;				///< Number of root signature parameters
		int32_t* mRootSignatureParameterIndexToUniformLocation;	///< Root signature parameter index to OpenGL uniform location mapping, can be a null pointer (type "GLint" not used in here in order to keep the header slim)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/Shader/ProgramGlsl.inl"
