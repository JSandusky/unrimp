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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "VulkanRenderer/Shader/ProgramGlsl.h"
#include "VulkanRenderer/Shader/VertexShaderGlsl.h"
#include "VulkanRenderer/Shader/GeometryShaderGlsl.h"
#include "VulkanRenderer/Shader/FragmentShaderGlsl.h"
#include "VulkanRenderer/Shader/TessellationControlShaderGlsl.h"
#include "VulkanRenderer/Shader/TessellationEvaluationShaderGlsl.h"
#include "VulkanRenderer/IContext.h"
#include "VulkanRenderer/Extensions.h"
#include "VulkanRenderer/RootSignature.h"
#include "VulkanRenderer/VulkanRenderer.h"

#include <Renderer/VertexArrayTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProgramGlsl::ProgramGlsl(VulkanRenderer &vulkanRenderer, const Renderer::IRootSignature&, const Renderer::VertexAttributes&, VertexShaderGlsl *vertexShaderGlsl, TessellationControlShaderGlsl *tessellationControlShaderGlsl, TessellationEvaluationShaderGlsl *tessellationEvaluationShaderGlsl, GeometryShaderGlsl *geometryShaderGlsl, FragmentShaderGlsl *fragmentShaderGlsl) :
		IProgram(vulkanRenderer),
		mNumberOfRootSignatureParameters(0)
	{
		// Attach the shaders to the program
		// -> We don't need to keep a reference to the shader, to add and release at once to ensure a nice behaviour
		if (nullptr != vertexShaderGlsl)
		{
			vertexShaderGlsl->addReference();
			// TODO(co) Implement me
			vertexShaderGlsl->release();
		}
		if (nullptr != tessellationControlShaderGlsl)
		{
			tessellationControlShaderGlsl->addReference();
			// TODO(co) Implement me
			tessellationControlShaderGlsl->release();
		}
		if (nullptr != tessellationEvaluationShaderGlsl)
		{
			tessellationEvaluationShaderGlsl->addReference();
			// TODO(co) Implement me
			tessellationEvaluationShaderGlsl->release();
		}
		if (nullptr != geometryShaderGlsl)
		{
			geometryShaderGlsl->addReference();
			// TODO(co) Implement me
			geometryShaderGlsl->release();
		}
		if (nullptr != fragmentShaderGlsl)
		{
			fragmentShaderGlsl->addReference();
			// TODO(co) Implement me
			fragmentShaderGlsl->release();
		}
	}

	ProgramGlsl::~ProgramGlsl()
	{
		// TODO(co) Implement me		
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
