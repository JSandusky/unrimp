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


// TODO(co) Work in progress


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Command/CommandBucket.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	namespace BackendDispatch
	{
		void Draw(const void* data, Renderer::IRenderer& renderer)
		{
			const Command::Draw* realData = static_cast<const Command::Draw*>(data);

			// Input-assembler (IA) stage
			renderer.iaSetVertexArray(realData->iaVertexArray);
			renderer.iaSetPrimitiveTopology(realData->iaPrimitiveTopology);

			// Material
			realData->material->bind(renderer);

			// Draw call specific
			renderer.draw(realData->startVertexLocation, realData->numberOfVertices);
		}

		void DrawIndexed(const void* data, Renderer::IRenderer& renderer)
		{
			const Command::DrawIndexed* realData = static_cast<const Command::DrawIndexed*>(data);

			// Input-assembler (IA) stage
			renderer.iaSetVertexArray(realData->iaVertexArray);
			renderer.iaSetPrimitiveTopology(realData->iaPrimitiveTopology);

			// Material
			realData->material->bind(renderer);

			// Draw call specific
			renderer.drawIndexed(realData->startIndexLocation, realData->numberOfIndices, realData->baseVertexLocation, 0, UINT32_MAX);
		}

		void CopyUniformBufferData(const void* data, Renderer::IRenderer&)
		{
			const Command::CopyUniformBufferData* realData = static_cast<const Command::CopyUniformBufferData*>(data);

			// Copy data into the uniform buffer
			realData->uniformBufferDynamicVs->copyDataFrom(realData->size, realData->data);
		}
	}
	namespace Command
	{
		const BackendDispatchFunction Draw::DISPATCH_FUNCTION = &BackendDispatch::Draw;
		const BackendDispatchFunction DrawIndexed::DISPATCH_FUNCTION = &BackendDispatch::DrawIndexed;
		const BackendDispatchFunction CopyUniformBufferData::DISPATCH_FUNCTION = &BackendDispatch::CopyUniformBufferData;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
