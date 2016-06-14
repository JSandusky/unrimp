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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Export.h"
#include "RendererRuntime/Core/Manager.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IRendererRuntime;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class DebugGuiManager : private Manager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT void newFrame(Renderer::IRenderTarget& renderTarget);
		RENDERERRUNTIME_API_EXPORT void renderFrame();


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::DebugGuiManager methods ]
	//[-------------------------------------------------------]
	protected:
		virtual void startup();
		virtual void onNewFrame(Renderer::IRenderTarget& renderTarget) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		explicit DebugGuiManager(IRendererRuntime& rendererRuntime);
		virtual ~DebugGuiManager();
		DebugGuiManager(const DebugGuiManager&) = delete;
		DebugGuiManager& operator=(const DebugGuiManager&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&			mRendererRuntime;	///< Renderer runtime instance, do not destroy the instance
		bool						mIsRunning;			///< The debug GUI manager will be initialized lazy when "RendererRuntime::DebugGuiManager::newFrame()" is called for the first time
		// Root signature and pipeline state
		Renderer::IRootSignaturePtr	mRootSignature;
		Renderer::IProgramPtr		mProgram;
		Renderer::IPipelineStatePtr	mPipelineState;
		// Uniform buffers
		Renderer::IUniformBufferPtr	mVertexShaderUniformBuffer;
		Renderer::handle			mObjectSpaceToClipSpaceMatrixUniformHandle;
		// Sampler and texture
		Renderer::ISamplerStatePtr	mSamplerState;
		Renderer::ITexture2DPtr		mTexture2D;
		// Vertex and index buffer
		Renderer::IVertexBufferPtr	mVertexBufferPtr;
		uint32_t					mNumberOfAllocatedVertices;
		Renderer::IIndexBufferPtr	mIndexBufferPtr;
		uint32_t					mNumberOfAllocatedIndices;
		Renderer::IVertexArrayPtr	mVertexArray;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
