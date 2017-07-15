/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
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
#include "VulkanRenderer/Shader/ProgramGlsl.h"
#include "VulkanRenderer/Shader/VertexShaderGlsl.h"
#include "VulkanRenderer/Shader/GeometryShaderGlsl.h"
#include "VulkanRenderer/Shader/FragmentShaderGlsl.h"
#include "VulkanRenderer/Shader/TessellationControlShaderGlsl.h"
#include "VulkanRenderer/Shader/TessellationEvaluationShaderGlsl.h"
#include "VulkanRenderer/RootSignature.h"
#include "VulkanRenderer/VulkanRenderer.h"

#include <Renderer/Buffer/VertexArrayTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProgramGlsl::ProgramGlsl(VulkanRenderer& vulkanRenderer, const Renderer::IRootSignature&, const Renderer::VertexAttributes&, VertexShaderGlsl *vertexShaderGlsl, TessellationControlShaderGlsl *tessellationControlShaderGlsl, TessellationEvaluationShaderGlsl *tessellationEvaluationShaderGlsl, GeometryShaderGlsl *geometryShaderGlsl, FragmentShaderGlsl *fragmentShaderGlsl) :
		IProgram(vulkanRenderer),
		mVertexShaderGlsl(vertexShaderGlsl),
		mTessellationControlShaderGlsl(tessellationControlShaderGlsl),
		mTessellationEvaluationShaderGlsl(tessellationEvaluationShaderGlsl),
		mGeometryShaderGlsl(geometryShaderGlsl),
		mFragmentShaderGlsl(fragmentShaderGlsl)
	{
		// Add references to the provided shaders
		if (nullptr != mVertexShaderGlsl)
		{
			mVertexShaderGlsl->addReference();
		}
		if (nullptr != mTessellationControlShaderGlsl)
		{
			mTessellationControlShaderGlsl->addReference();
		}
		if (nullptr != mTessellationEvaluationShaderGlsl)
		{
			mTessellationEvaluationShaderGlsl->addReference();
		}
		if (nullptr != mGeometryShaderGlsl)
		{
			mGeometryShaderGlsl->addReference();
		}
		if (nullptr != mFragmentShaderGlsl)
		{
			mFragmentShaderGlsl->addReference();
		}
	}

	ProgramGlsl::~ProgramGlsl()
	{
		// Release the shader references
		if (nullptr != mVertexShaderGlsl)
		{
			mVertexShaderGlsl->releaseReference();
		}
		if (nullptr != mTessellationControlShaderGlsl)
		{
			mTessellationControlShaderGlsl->releaseReference();
		}
		if (nullptr != mTessellationEvaluationShaderGlsl)
		{
			mTessellationEvaluationShaderGlsl->releaseReference();
		}
		if (nullptr != mGeometryShaderGlsl)
		{
			mGeometryShaderGlsl->releaseReference();
		}
		if (nullptr != mFragmentShaderGlsl)
		{
			mFragmentShaderGlsl->releaseReference();
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
