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
#ifndef __OPENGLRENDERER_GEOMETRYSHADER_GLSL_H__
#define __OPENGLRENDERER_GEOMETRYSHADER_GLSL_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/GeometryShaderTypes.h>
#include "OpenGLRenderer/GeometryShader.h"


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
	*    GLSL geometry shader class
	*/
	class GeometryShaderGlsl : public GeometryShader
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor for creating a geometry shader from shader bytecode
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] bytecode
		*    Shader bytecode, must be valid
		*  @param[in] numberOfBytes
		*    Number of bytes in the bytecode
		*  @param[in] gsInputPrimitiveTopology
		*    Geometry shader input primitive topology
		*  @param[in] gsOutputPrimitiveTopology
		*    Geometry shader output primitive topology
		*  @param[in] numberOfOutputVertices
		*    Number of output vertices
		*/
		GeometryShaderGlsl(OpenGLRenderer &openGLRenderer, const uint8_t *bytecode, uint32_t numberOfBytes, Renderer::GsInputPrimitiveTopology::Enum gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology::Enum gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices);

		/**
		*  @brief
		*    Constructor for creating a geometry shader from shader source code
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] sourceCode
		*    Shader ASCII source code, must be valid
		*  @param[in] gsInputPrimitiveTopology
		*    Geometry shader input primitive topology
		*  @param[in] gsOutputPrimitiveTopology
		*    Geometry shader output primitive topology
		*  @param[in] numberOfOutputVertices
		*    Number of output vertices
		*/
		GeometryShaderGlsl(OpenGLRenderer &openGLRenderer, const char *sourceCode, Renderer::GsInputPrimitiveTopology::Enum gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology::Enum gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~GeometryShaderGlsl();

		/**
		*  @brief
		*    Return the OpenGL shader
		*
		*  @return
		*    The OpenGL shader, can be zero if no resource is allocated, do not destroy the returned resource (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline uint32_t getOpenGLShader() const;

		/**
		*  @brief
		*    Return the OpenGL geometry shader input primitive topology
		*
		*  @return
		*    The OpenGL geometry shader input primitive topology (type "GLint" not used in here in order to keep the header slim)
		*/
		inline int getOpenGLGsInputPrimitiveTopology() const;

		/**
		*  @brief
		*    Return the OpenGL geometry shader output primitive topology
		*
		*  @return
		*    The OpenGL geometry shader output primitive topology (type "GLint" not used in here in order to keep the header slim)
		*/
		inline int getOpenGLGsOutputPrimitiveTopology() const;

		/**
		*  @brief
		*    Return the number of output vertices
		*
		*  @return
		*    The number of output vertices
		*/
		inline uint32_t getNumberOfOutputVertices() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	public:
		virtual const char *getShaderLanguageName() const override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		uint32_t mOpenGLShader;						///< OpenGL shader, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)
		int		 mOpenGLGsInputPrimitiveTopology;	///< OpenGL geometry shader input primitive topology (type "GLint" not used in here in order to keep the header slim)
		int		 mOpenGLGsOutputPrimitiveTopology;	///< OpenGL geometry shader output primitive topology (type "GLint" not used in here in order to keep the header slim)
		uint32_t mNumberOfOutputVertices;			///< Number of output vertices


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/GeometryShaderGlsl.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_GEOMETRYSHADER_GLSL_H__
