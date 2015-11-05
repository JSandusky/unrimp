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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D12Renderer/Shader/ProgramHlsl.h"
#include "Direct3D12Renderer/Shader/VertexShaderHlsl.h"
#include "Direct3D12Renderer/Shader/GeometryShaderHlsl.h"
#include "Direct3D12Renderer/Shader/FragmentShaderHlsl.h"
#include "Direct3D12Renderer/Shader/TessellationControlShaderHlsl.h"
#include "Direct3D12Renderer/Shader/TessellationEvaluationShaderHlsl.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProgramHlsl::ProgramHlsl(Direct3D12Renderer &direct3D12Renderer, VertexShaderHlsl *vertexShaderHlsl, TessellationControlShaderHlsl *tessellationControlShaderHlsl, TessellationEvaluationShaderHlsl *tessellationEvaluationShaderHlsl, GeometryShaderHlsl *geometryShaderHlsl, FragmentShaderHlsl *fragmentShaderHlsl) :
		Program(direct3D12Renderer, InternalResourceType::HLSL),
		mVertexShaderHlsl(vertexShaderHlsl),
		mTessellationControlShaderHlsl(tessellationControlShaderHlsl),
		mTessellationEvaluationShaderHlsl(tessellationEvaluationShaderHlsl),
		mGeometryShaderHlsl(geometryShaderHlsl),
		mFragmentShaderHlsl(fragmentShaderHlsl)
	{
		// Add references to the provided shaders
		if (nullptr != mVertexShaderHlsl)
		{
			mVertexShaderHlsl->addReference();
		}
		if (nullptr != mTessellationControlShaderHlsl)
		{
			mTessellationControlShaderHlsl->addReference();
		}
		if (nullptr != mTessellationEvaluationShaderHlsl)
		{
			mTessellationEvaluationShaderHlsl->addReference();
		}
		if (nullptr != mGeometryShaderHlsl)
		{
			mGeometryShaderHlsl->addReference();
		}
		if (nullptr != mFragmentShaderHlsl)
		{
			mFragmentShaderHlsl->addReference();
		}
	}

	ProgramHlsl::~ProgramHlsl()
	{
		// Release the shader references
		if (nullptr != mVertexShaderHlsl)
		{
			mVertexShaderHlsl->release();
		}
		if (nullptr != mTessellationControlShaderHlsl)
		{
			mTessellationControlShaderHlsl->release();
		}
		if (nullptr != mTessellationEvaluationShaderHlsl)
		{
			mTessellationEvaluationShaderHlsl->release();
		}
		if (nullptr != mGeometryShaderHlsl)
		{
			mGeometryShaderHlsl->release();
		}
		if (nullptr != mFragmentShaderHlsl)
		{
			mFragmentShaderHlsl->release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	uint32_t ProgramHlsl::getUniformBlockIndex(const char *, uint32_t defaultIndex)
	{
		// Not supported by Direct3D 12
		return defaultIndex;
	}

	handle ProgramHlsl::getUniformHandle(const char *)
	{
		// Not supported by Direct3D 12
		return NULL_HANDLE;
	}

	void ProgramHlsl::setUniform1f(handle, float)
	{
		// Not supported by Direct3D 12
	}

	void ProgramHlsl::setUniform2fv(handle, const float *)
	{
		// Not supported by Direct3D 12
	}

	void ProgramHlsl::setUniform3fv(handle, const float *)
	{
		// Not supported by Direct3D 12
	}

	void ProgramHlsl::setUniform4fv(handle, const float *)
	{
		// Not supported by Direct3D 12
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer
