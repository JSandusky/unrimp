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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Command/CommandBuffer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	namespace BackendDispatch
	{


		//[-------------------------------------------------------]
		//[ Resource handling                                     ]
		//[-------------------------------------------------------]
		void CopyUniformBufferData(const void* data, IRenderer&)
		{
			const Command::CopyUniformBufferData* realData = static_cast<const Command::CopyUniformBufferData*>(data);
			realData->uniformBuffer->copyDataFrom(realData->size, (nullptr != realData->data) ? realData->data : CommandPacketHelper::getAuxiliaryMemory(realData));
		}

		void CopyTextureBufferData(const void* data, IRenderer&)
		{
			const Command::CopyTextureBufferData* realData = static_cast<const Command::CopyTextureBufferData*>(data);
			realData->textureBuffer->copyDataFrom(realData->size, (nullptr != realData->data) ? realData->data : CommandPacketHelper::getAuxiliaryMemory(realData));
		}

		//[-------------------------------------------------------]
		//[ Graphics root                                         ]
		//[-------------------------------------------------------]
		void SetGraphicsRootSignature(const void* data, IRenderer& renderer)
		{
			const Command::SetGraphicsRootSignature* realData = static_cast<const Command::SetGraphicsRootSignature*>(data);
			renderer.setGraphicsRootSignature(realData->rootSignature);
		}

		void SetGraphicsRootDescriptorTable(const void* data, IRenderer& renderer)
		{
			const Command::SetGraphicsRootDescriptorTable* realData = static_cast<const Command::SetGraphicsRootDescriptorTable*>(data);
			renderer.setGraphicsRootDescriptorTable(realData->rootParameterIndex, realData->resource);
		}

		//[-------------------------------------------------------]
		//[ States                                                ]
		//[-------------------------------------------------------]
		void SetPipelineState(const void* data, IRenderer& renderer)
		{
			const Command::SetPipelineState* realData = static_cast<const Command::SetPipelineState*>(data);
			renderer.setPipelineState(realData->pipelineState);
		}

		//[-------------------------------------------------------]
		//[ Input-assembler (IA) stage                            ]
		//[-------------------------------------------------------]
		void SetVertexArray(const void* data, IRenderer& renderer)
		{
			const Command::SetVertexArray* realData = static_cast<const Command::SetVertexArray*>(data);
			renderer.iaSetVertexArray(realData->vertexArray);
		}

		void SetPrimitiveTopology(const void* data, IRenderer& renderer)
		{
			const Command::SetPrimitiveTopology* realData = static_cast<const Command::SetPrimitiveTopology*>(data);
			renderer.iaSetPrimitiveTopology(realData->primitiveTopology);
		}

		//[-------------------------------------------------------]
		//[ Rasterizer (RS) stage                                 ]
		//[-------------------------------------------------------]
		void SetViewports(const void* data, IRenderer& renderer)
		{
			const Command::SetViewports* realData = static_cast<const Command::SetViewports*>(data);
			renderer.rsSetViewports(realData->numberOfViewports, (nullptr != realData->viewports) ? realData->viewports : reinterpret_cast<const Viewport*>(CommandPacketHelper::getAuxiliaryMemory(realData)));
		}

		void SetScissorRectangles(const void* data, IRenderer& renderer)
		{
			const Command::SetScissorRectangles* realData = static_cast<const Command::SetScissorRectangles*>(data);
			renderer.rsSetScissorRectangles(realData->numberOfScissorRectangles, (nullptr != realData->scissorRectangles) ? realData->scissorRectangles : reinterpret_cast<const ScissorRectangle*>(CommandPacketHelper::getAuxiliaryMemory(realData)));
		}

		//[-------------------------------------------------------]
		//[ Output-merger (OM) stage                              ]
		//[-------------------------------------------------------]
		void SetRenderTarget(const void* data, IRenderer& renderer)
		{
			const Command::SetRenderTarget* realData = static_cast<const Command::SetRenderTarget*>(data);
			renderer.omSetRenderTarget(realData->renderTarget);
		}

		//[-------------------------------------------------------]
		//[ Operations                                            ]
		//[-------------------------------------------------------]
		void Clear(const void* data, IRenderer& renderer)
		{
			const Command::Clear* realData = static_cast<const Command::Clear*>(data);
			renderer.clear(realData->flags, realData->color, realData->z, realData->stencil);
		}

		//[-------------------------------------------------------]
		//[ Draw call                                             ]
		//[-------------------------------------------------------]
		void Draw(const void* data, IRenderer& renderer)
		{
			const Command::Draw* realData = static_cast<const Command::Draw*>(data);
			renderer.draw(*((nullptr != realData->indirectBuffer) ? realData->indirectBuffer : reinterpret_cast<const IndirectBuffer*>(CommandPacketHelper::getAuxiliaryMemory(realData))), realData->indirectBufferOffset, realData->numberOfDraws);
		}

		void DrawIndexed(const void* data, IRenderer& renderer)
		{
			const Command::Draw* realData = static_cast<const Command::Draw*>(data);
			renderer.drawIndexed(*((nullptr != realData->indirectBuffer) ? realData->indirectBuffer : reinterpret_cast<const IndirectBuffer*>(CommandPacketHelper::getAuxiliaryMemory(realData))), realData->indirectBufferOffset, realData->numberOfDraws);
		}

		//[-------------------------------------------------------]
		//[ Debug                                                 ]
		//[-------------------------------------------------------]
		void SetDebugMarker(const void* data, IRenderer& renderer)
		{
			const Command::SetDebugMarker* realData = static_cast<const Command::SetDebugMarker*>(data);
			renderer.setDebugMarker(realData->name);
		}

		void BeginDebugEvent(const void* data, IRenderer& renderer)
		{
			const Command::BeginDebugEvent* realData = static_cast<const Command::BeginDebugEvent*>(data);
			renderer.beginDebugEvent(realData->name);
		}

		void EndDebugEvent(const void*, IRenderer& renderer)
		{
			renderer.endDebugEvent();
		}


	}


	namespace Command
	{


		// Resource handling
		const BackendDispatchFunction CopyUniformBufferData::DISPATCH_FUNCTION = &BackendDispatch::CopyUniformBufferData;
		const BackendDispatchFunction CopyTextureBufferData::DISPATCH_FUNCTION = &BackendDispatch::CopyTextureBufferData;
		// Graphics root
		const BackendDispatchFunction SetGraphicsRootSignature::DISPATCH_FUNCTION		= &BackendDispatch::SetGraphicsRootSignature;
		const BackendDispatchFunction SetGraphicsRootDescriptorTable::DISPATCH_FUNCTION = &BackendDispatch::SetGraphicsRootDescriptorTable;
		// States
		const BackendDispatchFunction SetPipelineState::DISPATCH_FUNCTION = &BackendDispatch::SetPipelineState;
		// Input-assembler (IA) stage
		const BackendDispatchFunction SetVertexArray::DISPATCH_FUNCTION		  = &BackendDispatch::SetVertexArray;
		const BackendDispatchFunction SetPrimitiveTopology::DISPATCH_FUNCTION = &BackendDispatch::SetPrimitiveTopology;
		// Rasterizer (RS) stage
		const BackendDispatchFunction SetViewports::DISPATCH_FUNCTION		  = &BackendDispatch::SetViewports;
		const BackendDispatchFunction SetScissorRectangles::DISPATCH_FUNCTION = &BackendDispatch::SetScissorRectangles;
		// Output-merger (OM) stage
		const BackendDispatchFunction SetRenderTarget::DISPATCH_FUNCTION = &BackendDispatch::SetRenderTarget;
		// Operations
		const BackendDispatchFunction Clear::DISPATCH_FUNCTION = &BackendDispatch::Clear;
		// Draw call
		const BackendDispatchFunction Draw::DISPATCH_FUNCTION		 = &BackendDispatch::Draw;
		const BackendDispatchFunction DrawIndexed::DISPATCH_FUNCTION = &BackendDispatch::DrawIndexed;
		// Debug
		const BackendDispatchFunction SetDebugMarker::DISPATCH_FUNCTION	 = &BackendDispatch::SetDebugMarker;
		const BackendDispatchFunction BeginDebugEvent::DISPATCH_FUNCTION = &BackendDispatch::BeginDebugEvent;
		const BackendDispatchFunction EndDebugEvent::DISPATCH_FUNCTION	 = &BackendDispatch::EndDebugEvent;


	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
