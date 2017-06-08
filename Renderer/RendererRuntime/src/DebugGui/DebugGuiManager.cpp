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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/DebugGui/DebugGuiManager.h"
#include "RendererRuntime/DebugGui/DebugGuiHelper.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/Core/File/IFileManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <imguizmo/ImGuizmo.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/detail/setup.hpp>	// For "glm::countof()"


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		// Vertex input layout
		const Renderer::VertexAttribute VertexAttributesLayout[] =
		{
			{ // Attribute 0
				// Data destination
				Renderer::VertexAttributeFormat::FLOAT_2,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
				"Position",									// name[32] (char)
				"POSITION",									// semanticName[32] (char)
				0,											// semanticIndex (uint32_t)
				// Data source
				0,											// inputSlot (uint32_t)
				0,											// alignedByteOffset (uint32_t)
				// Data source, instancing part
				0											// instancesPerElement (uint32_t)
			},
			{ // Attribute 1
				// Data destination
				Renderer::VertexAttributeFormat::FLOAT_2,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
				"TexCoord",									// name[32] (char)
				"TEXCOORD",									// semanticName[32] (char)
				0,											// semanticIndex (uint32_t)
				// Data source
				0,											// inputSlot (uint32_t)
				sizeof(float) * 2,							// alignedByteOffset (uint32_t)
				// Data source, instancing part
				0											// instancesPerElement (uint32_t)
			},
			{ // Attribute 2
				// Data destination
				Renderer::VertexAttributeFormat::R8G8B8A8_UNORM,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
				"Color",											// name[32] (char)
				"COLOR",											// semanticName[32] (char)
				0,													// semanticIndex (uint32_t)
				// Data source
				0,													// inputSlot (uint32_t)
				sizeof(float) * 2 * 2,								// alignedByteOffset (uint32_t)
				// Data source, instancing part
				0													// instancesPerElement (uint32_t)
			}
		};
		const Renderer::VertexAttributes VertexAttributes(static_cast<uint32_t>(glm::countof(VertexAttributesLayout)), VertexAttributesLayout);


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void DebugGuiManager::newFrame(Renderer::IRenderTarget& renderTarget)
	{
		// Startup the debug GUI manager now?
		if (!mIsRunning)
		{
			startup();
			mIsRunning = true;
		}

		// Call the platform specific implementation
		onNewFrame(renderTarget);

		// Start the frame
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		DebugGuiHelper::beginFrame();
	}

	Renderer::IVertexArrayPtr DebugGuiManager::getFillVertexArrayPtr()
	{
		if (GImGui->Initialized)
		{
			// Ask ImGui to render into the internal command buffer and then request the resulting draw data
			ImGui::Render();
			const ImDrawData* imDrawData = ImGui::GetDrawData();
			Renderer::IRenderer& renderer = mRendererRuntime.getRenderer();
			Renderer::IBufferManager& bufferManager = mRendererRuntime.getBufferManager();

			{ // Vertex and index buffers
				// Create and grow vertex/index buffers if needed
				if (nullptr == mVertexBufferPtr || mNumberOfAllocatedVertices < static_cast<uint32_t>(imDrawData->TotalVtxCount))
				{
					mNumberOfAllocatedVertices = static_cast<uint32_t>(imDrawData->TotalVtxCount + 5000);	// Add some reserve to reduce reallocations
					mVertexBufferPtr = bufferManager.createVertexBuffer(mNumberOfAllocatedVertices * sizeof(ImDrawVert), nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
					RENDERER_SET_RESOURCE_DEBUG_NAME(mVertexBufferPtr, "Debug GUI")
					mVertexArrayPtr = nullptr;
				}
				if (nullptr == mIndexBufferPtr || mNumberOfAllocatedIndices < static_cast<uint32_t>(imDrawData->TotalIdxCount))
				{
					mNumberOfAllocatedIndices = static_cast<uint32_t>(imDrawData->TotalIdxCount + 10000);	// Add some reserve to reduce reallocations
					mIndexBufferPtr = bufferManager.createIndexBuffer(mNumberOfAllocatedIndices * sizeof(ImDrawIdx), Renderer::IndexBufferFormat::UNSIGNED_SHORT, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
					RENDERER_SET_RESOURCE_DEBUG_NAME(mIndexBufferPtr, "Debug GUI")
					mVertexArrayPtr = nullptr;
				}
				if (nullptr == mVertexArrayPtr)
				{
					assert(nullptr != mVertexBufferPtr);
					assert(nullptr != mIndexBufferPtr);

					// Create vertex array object (VAO)
					const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] =
					{
						{ // Vertex buffer 0
							mVertexBufferPtr,	// vertexBuffer (Renderer::IVertexBuffer *)
							sizeof(ImDrawVert)	// strideInBytes (uint32_t)
						}
					};
					mVertexArrayPtr = bufferManager.createVertexArray(::detail::VertexAttributes, static_cast<uint32_t>(glm::countof(vertexArrayVertexBuffers)), vertexArrayVertexBuffers, mIndexBufferPtr);
					RENDERER_SET_RESOURCE_DEBUG_NAME(mVertexArrayPtr, "Debug GUI")
				}

				{ // Copy and convert all vertices and indices into a single contiguous buffer
					// TODO(co) Not compatible with command buffer: This certainly is going to be changed
					Renderer::MappedSubresource vertexBufferMappedSubresource;
					if (renderer.map(*mVertexBufferPtr, 0, Renderer::MapType::WRITE_DISCARD, 0, vertexBufferMappedSubresource))
					{
						Renderer::MappedSubresource indexBufferMappedSubresource;
						if (renderer.map(*mIndexBufferPtr, 0, Renderer::MapType::WRITE_DISCARD, 0, indexBufferMappedSubresource))
						{
							ImDrawVert* imDrawVert = static_cast<ImDrawVert*>(vertexBufferMappedSubresource.data);
							ImDrawIdx* imDrawIdx = static_cast<ImDrawIdx*>(indexBufferMappedSubresource.data);
							for (int i = 0; i < imDrawData->CmdListsCount; ++i)
							{
								const ImDrawList* imDrawList = imDrawData->CmdLists[i];
								memcpy(imDrawVert, &imDrawList->VtxBuffer[0], imDrawList->VtxBuffer.size() * sizeof(ImDrawVert));
								memcpy(imDrawIdx, &imDrawList->IdxBuffer[0], imDrawList->IdxBuffer.size() * sizeof(ImDrawIdx));
								imDrawVert += imDrawList->VtxBuffer.size();
								imDrawIdx += imDrawList->IdxBuffer.size();
							}

							// Unmap the index buffer
							renderer.unmap(*mIndexBufferPtr, 0);
						}

						// Unmap the vertex buffer
						renderer.unmap(*mVertexBufferPtr, 0);
					}
				}
			}
		}

		// Done
		return mVertexArrayPtr;
	}

	void DebugGuiManager::fillCommandBuffer(Renderer::CommandBuffer& commandBuffer)
	{
		if (GImGui->Initialized)
		{
			// Begin debug event
			COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(commandBuffer)

			// Render command lists
			// -> There's no need to try to gather draw calls and batch them into multi-draw-indirect buffers, ImGui does already a pretty good job
			int vertexOffset = 0;
			int indexOffset = 0;
			const ImDrawData* imDrawData = ImGui::GetDrawData();
			for (int commandListIndex = 0; commandListIndex < imDrawData->CmdListsCount; ++commandListIndex)
			{
				const ImDrawList* imDrawList = imDrawData->CmdLists[commandListIndex];
				for (int commandIndex = 0; commandIndex < imDrawList->CmdBuffer.size(); ++commandIndex)
				{
					const ImDrawCmd* pcmd = &imDrawList->CmdBuffer[commandIndex];
					if (nullptr != pcmd->UserCallback)
					{
						pcmd->UserCallback(imDrawList, pcmd);
					}
					else
					{
						// Set scissor rectangle
						Renderer::Command::SetScissorRectangles::create(commandBuffer, static_cast<long>(pcmd->ClipRect.x), static_cast<long>(pcmd->ClipRect.y), static_cast<long>(pcmd->ClipRect.z), static_cast<long>(pcmd->ClipRect.w));

						// Draw
						Renderer::Command::DrawIndexed::create(commandBuffer, static_cast<uint32_t>(pcmd->ElemCount), 1, static_cast<uint32_t>(indexOffset), static_cast<int32_t>(vertexOffset));
					}
					indexOffset += pcmd->ElemCount;
				}
				vertexOffset += imDrawList->VtxBuffer.size();
			}

			// End debug event
			COMMAND_END_DEBUG_EVENT(commandBuffer)
		}
	}

	void DebugGuiManager::fillCommandBufferUsingFixedBuildInRendererConfiguration(Renderer::CommandBuffer& commandBuffer)
	{
		if (GImGui->Initialized)
		{
			// Begin debug event
			COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(commandBuffer)

			// Create fixed build in renderer configuration resources, if required
			if (nullptr == mRootSignature)
			{
				createFixedBuildInRendererConfigurationResources();
			}

			{ // Setup orthographic projection matrix into our vertex shader uniform buffer
				const ImGuiIO& imGuiIo = ImGui::GetIO();
				float objectSpaceToClipSpaceMatrix[4][4] =
				{
					{  2.0f / imGuiIo.DisplaySize.x, 0.0f,                          0.0f, 0.0f },
					{  0.0f,                         2.0f / -imGuiIo.DisplaySize.y, 0.0f, 0.0f },
					{  0.0f,                         0.0f,                          0.5f, 0.0f },
					{ -1.0f,                         1.0f,                          0.5f, 1.0f }
				};

				// Copy data
				if (nullptr != mVertexShaderUniformBuffer)
				{
					Renderer::Command::CopyUniformBufferData::create(commandBuffer, mVertexShaderUniformBuffer, sizeof(objectSpaceToClipSpaceMatrix), objectSpaceToClipSpaceMatrix);
				}
				else
				{
					// TODO(co) Not compatible with command buffer: This certainly is going to be removed, we need to implement internal uniform buffer emulation
					mProgram->setUniformMatrix4fv(mObjectSpaceToClipSpaceMatrixUniformHandle, &objectSpaceToClipSpaceMatrix[0][0]);
				}
			}

			{ // Renderer configuration
				// Set the used graphics root signature
				Renderer::Command::SetGraphicsRootSignature::create(commandBuffer, mRootSignature);

				// Set graphics root descriptors
				Renderer::Command::SetGraphicsRootDescriptorTable::create(commandBuffer, 0, mVertexShaderUniformBuffer);
				Renderer::Command::SetGraphicsRootDescriptorTable::create(commandBuffer, 1, mSamplerState);
				Renderer::Command::SetGraphicsRootDescriptorTable::create(commandBuffer, 2, mTexture2D);

				// Set the used pipeline state object (PSO)
				Renderer::Command::SetPipelineState::create(commandBuffer, mPipelineState);
			}

			{ // Setup input assembly (IA)
				// Set the used vertex array
				Renderer::Command::SetVertexArray::create(commandBuffer, getFillVertexArrayPtr());

				// Set the primitive topology used for draw calls
				Renderer::Command::SetPrimitiveTopology::create(commandBuffer, Renderer::PrimitiveTopology::TRIANGLE_LIST);
			}

			// Render command lists
			fillCommandBuffer(commandBuffer);

			// End debug event
			COMMAND_END_DEBUG_EVENT(commandBuffer)
		}
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::DebugGuiManager methods ]
	//[-------------------------------------------------------]
	void DebugGuiManager::startup()
	{
		assert(!mIsRunning);

		{ // Create texture instance
			// Build texture atlas
			unsigned char* pixels = nullptr;
			int width = 0;
			int height = 0;
			ImGui::GetIO().Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

			// Upload texture to renderer
			mTexture2D = mRendererRuntime.getTextureManager().createTexture2D(static_cast<uint32_t>(width), static_cast<uint32_t>(height), Renderer::TextureFormat::R8, pixels, Renderer::TextureFlag::GENERATE_MIPMAPS);
			RENDERER_SET_RESOURCE_DEBUG_NAME(mTexture2D, "Debug 2D GUI glyph texture atlas")

			// Tell the texture resource manager about our render target texture so it can be referenced inside e.g. compositor nodes
			mRendererRuntime.getTextureResourceManager().createTextureResourceByAssetId("Unrimp/Texture/DynamicByCode/ImGuiGlyphMap2D", *mTexture2D);
		}
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	DebugGuiManager::DebugGuiManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mIsRunning(false),
		mObjectSpaceToClipSpaceMatrixUniformHandle(NULL_HANDLE),
		mNumberOfAllocatedVertices(0),
		mNumberOfAllocatedIndices(0)
	{
		// Change ImGui filenames so one is able to guess where those files come from when using Unrimp
		const IFileManager& fileManager = rendererRuntime.getFileManager();
		const char* absoluteLocalDataDirectoryName = fileManager.getAbsoluteLocalDataDirectoryName();
		ImGuiIO& imGuiIo = ImGui::GetIO();
		if (nullptr != absoluteLocalDataDirectoryName)
		{
			// TODO(sw) These files doesn't get read/written via an file interface -> can break on mobile devices
			const std::string debugGuiDirectoryName = std::string(absoluteLocalDataDirectoryName) + "/DebugGui";
			mIniFilename = debugGuiDirectoryName + "/UnrimpDebugGuiLayout.ini";
			mLogFilename = debugGuiDirectoryName + "/UnrimpDebugGuiLog.txt";
			fileManager.createDirectories(debugGuiDirectoryName.c_str());
			imGuiIo.IniFilename = mIniFilename.c_str();
			imGuiIo.LogFilename = mLogFilename.c_str();
		}
		else
		{
			// Writing local data isn't allowed, unset standard ImGui filenames
			imGuiIo.IniFilename = nullptr;
			imGuiIo.LogFilename = nullptr;
		}

		// TODO(co) When having ImGui window rounding and anti-aliased shapes active (ImGui default) and using
		//          "unrimp\bin\DataSource\Content\ShaderBlueprint\Debug\GuiFragment.shader_blueprint" for a blurred
		//          GUI background, there's a nasty bright line at the left and top side. When disabling anti-aliased
		//          shapes or window rounding this artifact is gone. Decided to disable window rounding to keep the
		//          nicely blurred GUI background without the visual artifact. Might be worth digging into
		//          "ImDrawList::AddConvexPolyFilled()" in detail so see whether or not we can change something inside ImGui.
		ImGui::GetStyle().WindowRounding = 0.0f;
	}

	DebugGuiManager::~DebugGuiManager()
	{
		ImGui::Shutdown();
	}

	void DebugGuiManager::createFixedBuildInRendererConfigurationResources()
	{
		Renderer::IRenderer& renderer = mRendererRuntime.getRenderer();
		assert(nullptr == mRootSignature);

		{ // Create the root signature instance
			// Create the root signature
			Renderer::DescriptorRangeBuilder ranges[3];
			ranges[0].initialize(Renderer::DescriptorRangeType::UBV, 1, 0, "UniformBlockDynamicVs", 0);
			ranges[1].initializeSampler(1, 0);
			ranges[2].initialize(Renderer::DescriptorRangeType::SRV, 1, 0, "GlyphMap", 1);

			Renderer::RootParameterBuilder rootParameters[3];
			rootParameters[0].initializeAsDescriptorTable(1, &ranges[0], Renderer::ShaderVisibility::VERTEX);
			rootParameters[1].initializeAsDescriptorTable(1, &ranges[1], Renderer::ShaderVisibility::FRAGMENT);
			rootParameters[2].initializeAsDescriptorTable(1, &ranges[2], Renderer::ShaderVisibility::FRAGMENT);

			// Setup
			Renderer::RootSignatureBuilder rootSignature;
			rootSignature.initialize(static_cast<uint32_t>(glm::countof(rootParameters)), rootParameters, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			// Create the instance
			mRootSignature = renderer.createRootSignature(rootSignature);
			RENDERER_SET_RESOURCE_DEBUG_NAME(mRootSignature, "Debug GUI")
		}

		// Decide which shader language should be used (for example "GLSL" or "HLSL")
		Renderer::IShaderLanguage* shaderLanguage = renderer.getShaderLanguage();
		if (nullptr != shaderLanguage)
		{
			{ // Create the pipeline state instance
				{ // Create the program
					// Get the shader source code (outsourced to keep an overview)
					const char *vertexShaderSourceCode = nullptr;
					const char *fragmentShaderSourceCode = nullptr;
					#include "Detail/Shader/DebugGui_GLSL_410.h"
					#include "Detail/Shader/DebugGui_GLSL_ES3.h"
					#include "Detail/Shader/DebugGui_HLSL_D3D9.h"
					#include "Detail/Shader/DebugGui_HLSL_D3D10_D3D11_D3D12.h"
					#include "Detail/Shader/DebugGui_Null.h"

					// Create the shaders
					Renderer::IVertexShader* vertexShader = shaderLanguage->createVertexShaderFromSourceCode(::detail::VertexAttributes, vertexShaderSourceCode);
					RENDERER_SET_RESOURCE_DEBUG_NAME(vertexShader, "Debug GUI")
					Renderer::IFragmentShader* fragmentShader = shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode);
					RENDERER_SET_RESOURCE_DEBUG_NAME(fragmentShader, "Debug GUI")

					// Create the program
					mProgram = shaderLanguage->createProgram(
						*mRootSignature,
						::detail::VertexAttributes,
						vertexShader,
						fragmentShader);
					RENDERER_SET_RESOURCE_DEBUG_NAME(mProgram, "Debug GUI")
				}

				// Create the pipeline state object (PSO)
				if (nullptr != mProgram)
				{
					Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder(mRootSignature, mProgram, ::detail::VertexAttributes);
					pipelineState.rasterizerState.cullMode				   = Renderer::CullMode::NONE;
					pipelineState.rasterizerState.scissorEnable			   = 1;
					pipelineState.depthStencilState.depthEnable			   = false;
					pipelineState.depthStencilState.depthWriteMask		   = Renderer::DepthWriteMask::ZERO;
					pipelineState.blendState.renderTarget[0].blendEnable   = true;
					pipelineState.blendState.renderTarget[0].srcBlend	   = Renderer::Blend::SRC_ALPHA;
					pipelineState.blendState.renderTarget[0].destBlend	   = Renderer::Blend::INV_SRC_ALPHA;
					pipelineState.blendState.renderTarget[0].srcBlendAlpha = Renderer::Blend::INV_SRC_ALPHA;
					mPipelineState = renderer.createPipelineState(pipelineState);
					RENDERER_SET_RESOURCE_DEBUG_NAME(mPipelineState, "Debug GUI")
				}
			}
		}

		// Create vertex uniform buffer instance
		// -> Don't use "renderer.getCapabilities().maximumUniformBufferSize > 0" here since for OpenGL we're not using uniform buffers in here
		if (0 != strcmp(renderer.getName(), "OpenGL") && 0 != strcmp(renderer.getName(), "OpenGLES3"))
		{
			mVertexShaderUniformBuffer = mRendererRuntime.getBufferManager().createUniformBuffer(sizeof(float) * 4 * 4, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
			RENDERER_SET_RESOURCE_DEBUG_NAME(mVertexShaderUniformBuffer, "Debug GUI")
		}
		else if (nullptr != mProgram)
		{
			mObjectSpaceToClipSpaceMatrixUniformHandle = mProgram->getUniformHandle("ObjectSpaceToClipSpaceMatrix");
		}

		{ // Create sampler state instance
			Renderer::SamplerState samplerState = Renderer::ISamplerState::getDefaultSamplerState();
			samplerState.addressU = Renderer::TextureAddressMode::WRAP;
			samplerState.addressV = Renderer::TextureAddressMode::WRAP;
			mSamplerState = renderer.createSamplerState(samplerState);
			RENDERER_SET_RESOURCE_DEBUG_NAME(mSamplerState, "Debug GUI")
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
