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
	/**
	*  @brief
	*    Debug GUI manager using ImGui ( https://github.com/ocornut/imgui )
	*
	*  @remarks
	*    Supports two command buffer fill modes:
	*    - Using fixed build in renderer configuration, including shaders
	*    - Using a material resource blueprint set by the caller
	*
	*    Automatically creates a texture map  with the name
	*    - "Unrimp/Texture/DynamicByCode/ImGuiGlyphMap2D"
	*/
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
		RENDERERRUNTIME_API_EXPORT Renderer::IVertexArrayPtr getFillVertexArrayPtr();
		RENDERERRUNTIME_API_EXPORT void fillCommandBuffer(Renderer::CommandBuffer& commandBuffer);
		RENDERERRUNTIME_API_EXPORT void fillCommandBufferUsingFixedBuildInRendererConfiguration(Renderer::CommandBuffer& commandBuffer);


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
		void createFixedBuildInRendererConfigurationResources();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&			mRendererRuntime;	///< Renderer runtime instance, do not destroy the instance
		bool						mIsRunning;			///< The debug GUI manager will be initialized lazy when "RendererRuntime::DebugGuiManager::newFrame()" is called for the first time
		Renderer::ITexture2DPtr		mTexture2D;
		// Fixed build in renderer configuration resources
		Renderer::IRootSignaturePtr	mRootSignature;
		Renderer::IProgramPtr		mProgram;
		Renderer::IPipelineStatePtr	mPipelineState;
		Renderer::IUniformBufferPtr	mVertexShaderUniformBuffer;
		Renderer::handle			mObjectSpaceToClipSpaceMatrixUniformHandle;
		Renderer::ISamplerStatePtr	mSamplerState;
		// Vertex and index buffer
		Renderer::IVertexBufferPtr	mVertexBufferPtr;
		uint32_t					mNumberOfAllocatedVertices;
		Renderer::IIndexBufferPtr	mIndexBufferPtr;
		uint32_t					mNumberOfAllocatedIndices;
		Renderer::IVertexArrayPtr	mVertexArrayPtr;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
