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
#include "Direct3D12Renderer/ProgramHlsl.h"
#include "Direct3D12Renderer/VertexArray.h"
#include "Direct3D12Renderer/IndexBuffer.h"
#include "Direct3D12Renderer/VertexBuffer.h"
#include "Direct3D12Renderer/VertexShaderHlsl.h"
#include "Direct3D12Renderer/Direct3D12Renderer.h"
#include "Direct3D12Renderer/GeometryShaderHlsl.h"
#include "Direct3D12Renderer/FragmentShaderHlsl.h"
#include "Direct3D12Renderer/TessellationControlShaderHlsl.h"
#include "Direct3D12Renderer/TessellationEvaluationShaderHlsl.h"

#include <Renderer/VertexArrayTypes.h>


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
	Renderer::IVertexArray *ProgramHlsl::createVertexArray(uint32_t numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, Renderer::IIndexBuffer *indexBuffer)
	{
		// There must be a compiled vertex shader BLOB
		if (nullptr != mVertexShaderHlsl && nullptr != mVertexShaderHlsl->getD3DBlobVertexShader())
		{
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			return new VertexArray(getDirect3D12Renderer(), *mVertexShaderHlsl->getD3DBlobVertexShader(), numberOfAttributes, attributes, static_cast<IndexBuffer*>(indexBuffer));
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

	int ProgramHlsl::getAttributeLocation(const char *)
	{
		// Not supported by Direct3D 12
		return -1;
	}

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
