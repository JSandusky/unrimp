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
#include "RendererRuntime/DebugGui/DebugGuiManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/detail/setup.hpp>	// For "glm::countof()"

#include <string>


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
		const Renderer::VertexAttributes VertexAttributes(glm::countof(VertexAttributesLayout), VertexAttributesLayout);


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

		// Reset the draw text counter
		mDrawTextCounter = 0;

		// Call the platform specific implementation
		onNewFrame(renderTarget);

		// Start the frame
		ImGui::NewFrame();
	}

	void DebugGuiManager::drawText(const char* text, float x, float y, bool drawBackground)
	{
		if (!drawBackground)
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 0.0f));
		}
		// TODO(co) Get rid of "std::to_string()" later on in case it's a problem (memory allocations inside)
		ImGui::Begin(("RendererRuntime::DebugGuiManager::drawText_" + std::to_string(mDrawTextCounter)).c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing);
			ImGui::Text(text);
			ImGui::SetWindowPos(ImVec2(x, y));
		ImGui::End();
		if (!drawBackground)
		{
			ImGui::PopStyleColor();
		}
		++mDrawTextCounter;
	}

	void DebugGuiManager::renderFrame()
	{
		if (GImGui->Initialized)
		{
			// Ask ImGui to render into the internal command buffer and then request the resulting draw data
			ImGui::Render();
			const ImDrawData* imDrawData = ImGui::GetDrawData();

			// Begin debug event
			Renderer::IRenderer& renderer = mRendererRuntime.getRenderer();
			Renderer::IBufferManager& bufferManager = mRendererRuntime.getBufferManager();
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

			{ // Vertex and index buffers
				// Create and grow vertex/index buffers if needed
				if (nullptr == mVertexBufferPtr || mNumberOfAllocatedVertices < static_cast<uint32_t>(imDrawData->TotalVtxCount))
				{
					mNumberOfAllocatedVertices = static_cast<uint32_t>(imDrawData->TotalVtxCount + 5000);	// Add some reserve to reduce reallocations
					mVertexBufferPtr = bufferManager.createVertexBuffer(mNumberOfAllocatedVertices * sizeof(ImDrawVert), nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
					mVertexArray = nullptr;
				}
				if (nullptr == mIndexBufferPtr || mNumberOfAllocatedIndices < static_cast<uint32_t>(imDrawData->TotalIdxCount))
				{
					mNumberOfAllocatedIndices = static_cast<uint32_t>(imDrawData->TotalIdxCount + 10000);	// Add some reserve to reduce reallocations
					mIndexBufferPtr = bufferManager.createIndexBuffer(mNumberOfAllocatedIndices * sizeof(ImDrawIdx), Renderer::IndexBufferFormat::UNSIGNED_SHORT, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
					mVertexArray = nullptr;
				}
				if (nullptr == mVertexArray)
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
					mVertexArray = bufferManager.createVertexArray(::detail::VertexAttributes, glm::countof(vertexArrayVertexBuffers), vertexArrayVertexBuffers, mIndexBufferPtr);
				}

				{ // Copy and convert all vertices and indices into a single contiguous buffer
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
					mVertexShaderUniformBuffer->copyDataFrom(sizeof(objectSpaceToClipSpaceMatrix), objectSpaceToClipSpaceMatrix);
				}
				else
				{
					mProgram->setUniformMatrix4fv(mObjectSpaceToClipSpaceMatrixUniformHandle, &objectSpaceToClipSpaceMatrix[0][0]);
				}
			}

			{ // Renderer configuration
				// Set the used graphics root signature
				renderer.setGraphicsRootSignature(mRootSignature);

				// Set graphics root descriptors
				renderer.setGraphicsRootDescriptorTable(0, mVertexShaderUniformBuffer);
				renderer.setGraphicsRootDescriptorTable(1, mSamplerState);
				renderer.setGraphicsRootDescriptorTable(2, mTexture2D);

				// Set the used pipeline state object (PSO)
				renderer.setPipelineState(mPipelineState);

				{ // Setup input assembly (IA)
					// Set the used vertex array
					renderer.iaSetVertexArray(mVertexArray);

					// Set the primitive topology used for draw calls
					renderer.iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_LIST);
				}
			}

			{ // Render command lists
				int vertexOffset = 0;
				int indexOffset = 0;
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
							const Renderer::ScissorRectangle scissorRectangle = { static_cast<long>(pcmd->ClipRect.x), static_cast<long>(pcmd->ClipRect.y), static_cast<long>(pcmd->ClipRect.z), static_cast<long>(pcmd->ClipRect.w) };
							renderer.rsSetScissorRectangles(1, &scissorRectangle);

							// Draw
							renderer.drawIndexed(static_cast<uint32_t>(indexOffset), static_cast<uint32_t>(pcmd->ElemCount), static_cast<uint32_t>(vertexOffset), 0, static_cast<uint32_t>(imDrawData->TotalVtxCount));
						}
						indexOffset += pcmd->ElemCount;
					}
					vertexOffset += imDrawList->VtxBuffer.size();
				}
			}

			// End debug event
			RENDERER_END_DEBUG_EVENT(&renderer)
		}
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::DebugGuiManager methods ]
	//[-------------------------------------------------------]
	void DebugGuiManager::startup()
	{
		assert(!mIsRunning);

		// Begin debug event
		Renderer::IRenderer& renderer = mRendererRuntime.getRenderer();
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

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
			rootSignature.initialize(glm::countof(rootParameters), rootParameters, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			// Create the instance
			mRootSignature = renderer.createRootSignature(rootSignature);
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
					#pragma warning(push)
						#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
						#include "Detail/Shader/DebugGui_GLSL_410.h"
						#include "Detail/Shader/DebugGui_GLSL_ES2.h"
						#include "Detail/Shader/DebugGui_HLSL_D3D9.h"
						#include "Detail/Shader/DebugGui_HLSL_D3D10_D3D11_D3D12.h"
						#include "Detail/Shader/DebugGui_Null.h"
					#pragma warning(pop)

					// Create the program
					mProgram = shaderLanguage->createProgram(
						*mRootSignature,
						::detail::VertexAttributes,
						shaderLanguage->createVertexShaderFromSourceCode(::detail::VertexAttributes, vertexShaderSourceCode),
						shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode));
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
				}
			}
		}

		// Create vertex uniform buffer instance
		if (0 != strcmp(renderer.getName(), "OpenGL") && 0 != strcmp(renderer.getName(), "OpenGLES2"))
		{
			mVertexShaderUniformBuffer = mRendererRuntime.getBufferManager().createUniformBuffer(sizeof(float) * 4 * 4, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
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
		}

		{ // Create texture instance
			// Build texture atlas
			unsigned char* pixels = nullptr;
			int width = 0;
			int height = 0;
			ImGui::GetIO().Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

			// Upload texture to renderer
			mTexture2D = renderer.createTexture2D(static_cast<uint32_t>(width), static_cast<uint32_t>(height), Renderer::TextureFormat::A8, pixels, Renderer::TextureFlag::GENERATE_MIPMAPS);
			RENDERER_SET_RESOURCE_DEBUG_NAME(mTexture2D, "Debug GUI glyph texture atlas")
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(&renderer)
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	DebugGuiManager::DebugGuiManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mIsRunning(false),
		mDrawTextCounter(0),
		mObjectSpaceToClipSpaceMatrixUniformHandle(NULL_HANDLE),
		mNumberOfAllocatedVertices(0),
		mNumberOfAllocatedIndices(0)
	{
		// Change ImGui filenames so one is able to guess where those files come from when using Unrimp
		// TODO(co) Maybe it makes sense to collect such filenames somewhere so one doesn't need to find those one after another when trying to e.g. keep all files inside the user application directory?
		ImGuiIO& imGuiIo = ImGui::GetIO();
		imGuiIo.IniFilename = "UnrimpDebugGuiLayout.ini";
		imGuiIo.LogFilename = "UnrimpDebugGuiLog.txt";
	}

	DebugGuiManager::~DebugGuiManager()
	{
		ImGui::Shutdown();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
