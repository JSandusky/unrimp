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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "NullRenderer/Program.h"
#include "NullRenderer/VertexArray.h"
#include "NullRenderer/IndexBuffer.h"
#include "NullRenderer/VertexBuffer.h"
#include "NullRenderer/VertexShader.h"
#include "NullRenderer/GeometryShader.h"
#include "NullRenderer/FragmentShader.h"
#include "NullRenderer/TessellationControlShader.h"
#include "NullRenderer/TessellationEvaluationShader.h"

#include <Renderer/VertexArrayTypes.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace NullRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	Program::Program(NullRenderer &nullRenderer, VertexShader *vertexShader, TessellationControlShader *tessellationControlShader, TessellationEvaluationShader *tessellationEvaluationShader, GeometryShader *geometryShader, FragmentShader *fragmentShader) :
		IProgram(reinterpret_cast<Renderer::IRenderer&>(nullRenderer))
	{
		// We don't keep a reference to the shaders in here
		// -> Ensure a correct reference counter behaviour
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->release();
		}
		if (nullptr != tessellationControlShader)
		{
			tessellationControlShader->addReference();
			tessellationControlShader->release();
		}
		if (nullptr != tessellationEvaluationShader)
		{
			tessellationEvaluationShader->addReference();
			tessellationEvaluationShader->release();
		}
		if (nullptr != geometryShader)
		{
			geometryShader->addReference();
			geometryShader->release();
		}
		if (nullptr != fragmentShader)
		{
			fragmentShader->addReference();
			fragmentShader->release();
		}
	}

	/**
	*  @brief
	*    Destructor
	*/
	Program::~Program()
	{
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	Renderer::IVertexArray *Program::createVertexArray(unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, Renderer::IIndexBuffer *indexBuffer)
	{
		// We don't keep a reference to the vertex buffers used by the vertex array attributes in here
		// -> Ensure a correct reference counter behaviour
		const Renderer::VertexArrayAttribute *attributeEnd = attributes + numberOfAttributes;
		for (const Renderer::VertexArrayAttribute *attribute = attributes; attribute < attributeEnd; ++attribute)
		{
			attribute->vertexBuffer->addReference();
			attribute->vertexBuffer->release();
		}

		// We don't keep a reference to the index buffer in here
		// -> Ensure a correct reference counter behaviour
		if (nullptr != indexBuffer)
		{
			indexBuffer->addReference();
			indexBuffer->release();
		}

		// Create the vertex array instance
		return new VertexArray(reinterpret_cast<NullRenderer&>(getRenderer()));
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // NullRenderer
