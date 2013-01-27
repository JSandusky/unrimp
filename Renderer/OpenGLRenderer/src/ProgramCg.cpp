/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/ProgramCg.h"
#include "OpenGLRenderer/IndexBuffer.h"
#include "OpenGLRenderer/VertexBuffer.h"
#include "OpenGLRenderer/VertexArrayCg.h"
#include "OpenGLRenderer/VertexShaderCg.h"
#include "OpenGLRenderer/GeometryShaderCg.h"
#include "OpenGLRenderer/FragmentShaderCg.h"
#include "OpenGLRenderer/CgRuntimeLinking.h"
#include "OpenGLRenderer/TessellationControlShaderCg.h"
#include "OpenGLRenderer/TessellationEvaluationShaderCg.h"

#include <Renderer/PlatformTypes.h>	// For "RENDERER_OUTPUT_DEBUG_STRING()"
#include <Renderer/VertexArrayTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	ProgramCg::ProgramCg(OpenGLRenderer &openGLRenderer, VertexShaderCg *vertexShaderCg, TessellationControlShaderCg *tessellationControlShaderCg, TessellationEvaluationShaderCg *tessellationEvaluationShaderCg, GeometryShaderCg *geometryShaderCg, FragmentShaderCg *fragmentShaderCg) :
		Program(openGLRenderer, InternalResourceType::CG),
		mCgProgram(nullptr)
	{
		// Collect the pointers to the Cg programs
		CGprogram cgPrograms[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
		int numberOfCgPrograms = 0;

		// We don't keep a reference to the shaders in here
		// -> Ensure a correct reference counter behaviour
		if (nullptr != vertexShaderCg)
		{
			vertexShaderCg->addReference();
			cgPrograms[numberOfCgPrograms] = vertexShaderCg->getCgProgram();
			++numberOfCgPrograms;
		}
		if (nullptr != tessellationControlShaderCg)
		{
			tessellationControlShaderCg->addReference();
			cgPrograms[numberOfCgPrograms] = tessellationControlShaderCg->getCgProgram();
			++numberOfCgPrograms;
		}
		if (nullptr != tessellationEvaluationShaderCg)
		{
			tessellationEvaluationShaderCg->addReference();
			cgPrograms[numberOfCgPrograms] = tessellationEvaluationShaderCg->getCgProgram();
			++numberOfCgPrograms;
		}
		if (nullptr != geometryShaderCg)
		{
			geometryShaderCg->addReference();
			cgPrograms[numberOfCgPrograms] = geometryShaderCg->getCgProgram();
			++numberOfCgPrograms;
		}
		if (nullptr != fragmentShaderCg)
		{
			fragmentShaderCg->addReference();
			cgPrograms[numberOfCgPrograms] = fragmentShaderCg->getCgProgram();
			++numberOfCgPrograms;
		}

		// Create the combined Cg program
		mCgProgram = cgCombinePrograms(numberOfCgPrograms, cgPrograms);
		if (nullptr != mCgProgram)
		{
			// Load the Cg combined program
			cgGLLoadProgram(mCgProgram);
			// TODO(co) More error details
		}
		else
		{
			// Error!
			RENDERER_OUTPUT_DEBUG_STRING("OpenGL Cg error: Failed to combine the Cg programs\n")
		}

		// We don't keep a reference to the shaders in here
		// -> Ensure a correct reference counter behaviour
		if (nullptr != vertexShaderCg)
		{
			vertexShaderCg->release();
		}
		if (nullptr != tessellationControlShaderCg)
		{
			tessellationControlShaderCg->release();
		}
		if (nullptr != tessellationEvaluationShaderCg)
		{
			tessellationEvaluationShaderCg->release();
		}
		if (nullptr != geometryShaderCg)
		{
			geometryShaderCg->release();
		}
		if (nullptr != fragmentShaderCg)
		{
			fragmentShaderCg->release();
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	ProgramCg::~ProgramCg()
	{
		// Destroy the Cg program
		if (nullptr != mCgProgram)
		{
			cgDestroyProgram(mCgProgram);
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	Renderer::IVertexArray *ProgramCg::createVertexArray(unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, Renderer::IIndexBuffer *indexBuffer)
	{
		// Is there a valid Cg program?
		if (nullptr != mCgProgram)
		{
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			return new VertexArrayCg(*this, numberOfAttributes, attributes, static_cast<IndexBuffer*>(indexBuffer));
		}
		else
		{
			// Ensure a correct reference counter behaviour, even in the situation of an error
			const Renderer::VertexArrayAttribute *attributeEnd = attributes + numberOfAttributes;
			for (const Renderer::VertexArrayAttribute *attribute = attributes; attribute < attributeEnd; ++attribute)
			{
				attribute->vertexBuffer->addReference();
				attribute->vertexBuffer->release();
			}
			if (nullptr != indexBuffer)
			{
				indexBuffer->addReference();
				indexBuffer->release();
			}

			// Error!
			return nullptr;
		}
	}

	int ProgramCg::getAttributeLocation(const char *)
	{
		// TODO(co) Implement me
		return -1;
	}

	unsigned int ProgramCg::getUniformBlockIndex(const char *uniformBlockName, unsigned int defaultIndex)
	{
		const CGparameter cgParameter = cgGetNamedParameter(mCgProgram, uniformBlockName);
		return (nullptr != cgParameter) ? cgGetParameterBufferIndex(cgParameter) : defaultIndex;
	}

	handle ProgramCg::getUniformHandle(const char *uniformName)
	{
		return reinterpret_cast<handle>(cgGetNamedParameter(mCgProgram, uniformName));
	}

	unsigned int ProgramCg::setTextureUnit(handle uniformHandle, unsigned int unit)
	{
		// Telling Cg which texture unit to use is not possible, so, instead we have to ask Cg which texture unit a sampler is using
		const CGparameter cgParameter = reinterpret_cast<CGparameter>(uniformHandle);
		return (CG_PARAMETERCLASS_SAMPLER == cgGetParameterClass(cgParameter)) ? (cgGLGetTextureEnum(cgParameter) - GL_TEXTURE0_ARB) : unit;
	}

	void ProgramCg::setUniform1f(handle uniformHandle, float value)
	{
		cgSetParameter1f(reinterpret_cast<CGparameter>(uniformHandle), value);
	}

	void ProgramCg::setUniform2fv(handle uniformHandle, const float *value)
	{
		cgSetParameter2fv(reinterpret_cast<CGparameter>(uniformHandle), value);
	}

	void ProgramCg::setUniform3fv(handle uniformHandle, const float *value)
	{
		cgSetParameter3fv(reinterpret_cast<CGparameter>(uniformHandle), value);
	}

	void ProgramCg::setUniform4fv(handle uniformHandle, const float *value)
	{
		cgSetParameter4fv(reinterpret_cast<CGparameter>(uniformHandle), value);
	}

	void ProgramCg::setUniformMatrix3fv(handle uniformHandle, const float *value)
	{
		cgSetMatrixParameterfc(reinterpret_cast<CGparameter>(uniformHandle), value);
	}

	void ProgramCg::setUniformMatrix4fv(handle uniformHandle, const float *value)
	{
		cgSetMatrixParameterfc(reinterpret_cast<CGparameter>(uniformHandle), value);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
