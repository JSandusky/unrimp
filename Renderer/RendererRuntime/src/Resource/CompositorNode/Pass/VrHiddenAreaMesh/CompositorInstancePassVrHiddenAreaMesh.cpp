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
#include "RendererRuntime/Resource/CompositorNode/Pass/VrHiddenAreaMesh/CompositorInstancePassVrHiddenAreaMesh.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/VrHiddenAreaMesh/CompositorResourcePassVrHiddenAreaMesh.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeInstance.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorContextData.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceInstance.h"
#include "RendererRuntime/Vr/OpenVR/VrManagerOpenVR.h"
#include "RendererRuntime/IRendererRuntime.h"
#include "RendererRuntime/Context.h"

#include <Renderer/Public/Renderer.h>


#ifndef ANDROID
	// TODO(sw) For now no OpenVR support under Android
	//[-------------------------------------------------------]
	//[ Anonymous detail namespace                            ]
	//[-------------------------------------------------------]
	namespace
	{
		namespace detail
		{


			//[-------------------------------------------------------]
			//[ Classes                                               ]
			//[-------------------------------------------------------]
			class Mesh : public Renderer::RefCount<Mesh>
			{


			//[-------------------------------------------------------]
			//[ Public methods                                        ]
			//[-------------------------------------------------------]
			public:
				explicit Mesh(const RendererRuntime::IRendererRuntime& rendererRuntime) :
					mNumberOfTriangles(0)
				{
					Renderer::IRenderer& renderer = rendererRuntime.getRenderer();

					{ // Create the root signature
						// Setup
						Renderer::RootSignatureBuilder rootSignature;
						rootSignature.initialize(0, nullptr, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

						// Create the instance
						mRootSignature = renderer.createRootSignature(rootSignature);
					}

					// Vertex input layout
					const Renderer::VertexAttribute vertexAttributesLayout[] =
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
							sizeof(float) * 2,							// strideInBytes (uint32_t)
							0											// instancesPerElement (uint32_t)
						}
					};
					const Renderer::VertexAttributes vertexAttributes(static_cast<uint32_t>(glm::countof(vertexAttributesLayout)), vertexAttributesLayout);

					{ // Create vertex array and merge both meshes into a single mesh since we're using single pass stereo rendering via instancing as described in "High Performance Stereo Rendering For VR", Timothy Wilson, San Diego, Virtual Reality Meetup
						Renderer::IBufferManager& bufferManager = rendererRuntime.getBufferManager();
						vr::IVRSystem* vrSystem = static_cast<RendererRuntime::VrManagerOpenVR&>(rendererRuntime.getVrManager()).getVrSystem();

						// Get the combined number of vertex buffer bytes and triangles
						uint32_t numberOfBytes = 0;
						for (int vrEyeIndex = 0; vrEyeIndex < 2; ++vrEyeIndex)
						{
							const vr::HiddenAreaMesh_t vrHiddenAreaMesh = vrSystem->GetHiddenAreaMesh(static_cast<vr::EVREye>(vrEyeIndex));
							mNumberOfTriangles += vrHiddenAreaMesh.unTriangleCount;
							numberOfBytes += sizeof(vr::HmdVector2_t) * 3 * vrHiddenAreaMesh.unTriangleCount;
						}
			
						// Allocate temporary vertex buffer memory, if necessary
						// -> For HTC Vive there are tiny 1248 bytes which can be easily put onto the C-runtime-stack to avoid a memory allocation
						static const uint32_t STACK_NUMBER_OF_BYTES = 1248;
						uint8_t stackMemory[STACK_NUMBER_OF_BYTES];
						uint8_t* temporaryMemory = (numberOfBytes <= STACK_NUMBER_OF_BYTES) ? stackMemory : new uint8_t[numberOfBytes];

						{ // Fill temporary vertex buffer memory
							float* currentTemporaryMemory = reinterpret_cast<float*>(temporaryMemory);
							for (int vrEyeIndex = 0; vrEyeIndex < 2; ++vrEyeIndex)
							{
								const vr::HiddenAreaMesh_t vrHiddenAreaMesh = vrSystem->GetHiddenAreaMesh(static_cast<vr::EVREye>(vrEyeIndex));
								const float offset = (0 == vrEyeIndex) ? 0.0f : 0.5f;
								const vr::HmdVector2_t* currentSourceVertexData = vrHiddenAreaMesh.pVertexData;
								const vr::HmdVector2_t* sourceVertexDataEnd = currentSourceVertexData + 3 * vrHiddenAreaMesh.unTriangleCount;
								for (; currentSourceVertexData < sourceVertexDataEnd; ++currentSourceVertexData, currentTemporaryMemory += 2)
								{
									currentTemporaryMemory[0] = currentSourceVertexData->v[0] * 0.5f + offset;
									currentTemporaryMemory[1] = currentSourceVertexData->v[1];
								}
							}
						}

						// Create the vertex buffer object (VBO)
						Renderer::IVertexBufferPtr vertexBuffer(bufferManager.createVertexBuffer(numberOfBytes, temporaryMemory, Renderer::BufferUsage::STATIC_DRAW));
						RENDERER_SET_RESOURCE_DEBUG_NAME(vertexBuffer, "Compositor instance pass VR hidden area mesh")

						// Create vertex array object (VAO)
						const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] = { vertexBuffer };
						mVertexArrayPtr = bufferManager.createVertexArray(vertexAttributes, static_cast<uint32_t>(glm::countof(vertexArrayVertexBuffers)), vertexArrayVertexBuffers);
						RENDERER_SET_RESOURCE_DEBUG_NAME(mVertexArrayPtr, "Compositor instance pass VR hidden area mesh")

						// Free allocated temporary vertex buffer memory, if necessary
						if (temporaryMemory != stackMemory)
						{
							delete [] temporaryMemory;
						}
					}

					// Decide which shader language should be used (for example "GLSL" or "HLSL")
					Renderer::IShaderLanguagePtr shaderLanguage(renderer.getShaderLanguage());
					if (nullptr != shaderLanguage)
					{
						// Create the program
						Renderer::IProgramPtr program;
						{
							// Get the shader source code (outsourced to keep an overview)
							const char* vertexShaderSourceCode = nullptr;
							const char* fragmentShaderSourceCode = nullptr;
							#include "Shader/VrHiddenAreaMesh_GLSL_410.h"
							#include "Shader/VrHiddenAreaMesh_GLSL_ES3.h"
							#include "Shader/VrHiddenAreaMesh_HLSL_D3D9.h"
							#include "Shader/VrHiddenAreaMesh_HLSL_D3D10_D3D11_D3D12.h"
							#include "Shader/VrHiddenAreaMesh_Null.h"

							// Create the vertex shader
							Renderer::IVertexShader* vertexShader = shaderLanguage->createVertexShaderFromSourceCode(vertexAttributes, vertexShaderSourceCode);
							RENDERER_SET_RESOURCE_DEBUG_NAME(vertexShader, "Compositor instance pass VR hidden area mesh VS")

							// Create the fragment shader
							Renderer::IFragmentShader* fragmentShader = shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode);
							RENDERER_SET_RESOURCE_DEBUG_NAME(fragmentShader, "Compositor instance pass VR hidden area mesh FS")

							// Create the program
							program = shaderLanguage->createProgram(*mRootSignature, vertexAttributes, vertexShader, fragmentShader);
							RENDERER_SET_RESOURCE_DEBUG_NAME(program, "Compositor instance pass VR hidden area mesh program")
						}

						// Create the pipeline state object (PSO)
						if (nullptr != program)
						{
							// TODO(co) Render pass related update, the render pass in here is currently just a dummy so the debug compositor works
							Renderer::IRenderPass* renderPass = renderer.createRenderPass(1, &renderer.getCapabilities().preferredSwapChainColorTextureFormat, renderer.getCapabilities().preferredSwapChainDepthStencilTextureFormat);

							Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder(mRootSignature, program, vertexAttributes, *renderPass);
							pipelineState.rasterizerState.cullMode = Renderer::CullMode::NONE;
							mPipelineState = renderer.createPipelineState(pipelineState);
							RENDERER_SET_RESOURCE_DEBUG_NAME(mPipelineState, "Compositor instance pass VR hidden area mesh PSO")
						}
					}
				}

				~Mesh()
				{
					mRootSignature->releaseReference();
					mVertexArrayPtr->releaseReference();
					mPipelineState->releaseReference();
				}

				void onFillCommandBuffer(Renderer::CommandBuffer& commandBuffer)
				{
					// Set the used graphics root signature
					Renderer::Command::SetGraphicsRootSignature::create(commandBuffer, mRootSignature);

					// Set the used pipeline state object (PSO)
					Renderer::Command::SetPipelineState::create(commandBuffer, mPipelineState);

					// Setup input assembly (IA): Set the used vertex array
					Renderer::Command::SetVertexArray::create(commandBuffer, mVertexArrayPtr);

					// Render the specified geometric primitive, based on an array of vertices
					Renderer::Command::Draw::create(commandBuffer, mNumberOfTriangles * 3);
				}


			//[-------------------------------------------------------]
			//[ Private methods                                       ]
			//[-------------------------------------------------------]
			private:
				explicit Mesh(const Mesh&) = delete;
				Mesh& operator=(const Mesh&) = delete;


			//[-------------------------------------------------------]
			//[ Private data                                          ]
			//[-------------------------------------------------------]
			private:
				Renderer::IRootSignaturePtr	mRootSignature;
				Renderer::IVertexArrayPtr	mVertexArrayPtr;
				uint32_t					mNumberOfTriangles;
				Renderer::IPipelineStatePtr	mPipelineState;	// TODO(co) As soon as we support stencil in here, instance might need different pipeline states


			};
			typedef Renderer::SmartRefCount<Mesh> MeshPtr;


			//[-------------------------------------------------------]
			//[ Global variables                                      ]
			//[-------------------------------------------------------]
			static MeshPtr g_MeshPtr;


	//[-------------------------------------------------------]
	//[ Anonymous detail namespace                            ]
	//[-------------------------------------------------------]
		} // detail
	}
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::ICompositorInstancePass methods ]
	//[-------------------------------------------------------]
	void CompositorInstancePassVrHiddenAreaMesh::onFillCommandBuffer(const Renderer::IRenderTarget&, const CompositorContextData& compositorContextData, Renderer::CommandBuffer& commandBuffer)
	{
		#ifndef ANDROID
			// TODO(sw) For now no OpenVR support under Android
			if (nullptr != ::detail::g_MeshPtr)
			{
				// Begin debug event
				COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(commandBuffer)

				// Fill command buffer
				compositorContextData.resetCurrentlyBoundMaterialBlueprintResource();
				::detail::g_MeshPtr->onFillCommandBuffer(commandBuffer);

				// End debug event
				COMMAND_END_DEBUG_EVENT(commandBuffer)
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CompositorInstancePassVrHiddenAreaMesh::CompositorInstancePassVrHiddenAreaMesh(const CompositorResourcePassVrHiddenAreaMesh& compositorResourcePassVrHiddenAreaMesh, const CompositorNodeInstance& compositorNodeInstance) :
		ICompositorInstancePass(compositorResourcePassVrHiddenAreaMesh, compositorNodeInstance)
	{
		#ifndef ANDROID
			// TODO(sw) For now no OpenVR support under Android
			// Add reference to vertex array object (VAO) shared between all compositor instance pass VR hidden area mesh instances
			if (nullptr == ::detail::g_MeshPtr)
			{
				const IRendererRuntime& rendererRuntime = getCompositorNodeInstance().getCompositorWorkspaceInstance().getRendererRuntime();
				IVrManager& vrManager = rendererRuntime.getVrManager();
				if (vrManager.isRunning() && vrManager.getVrManagerTypeId() == RendererRuntime::VrManagerOpenVR::TYPE_ID &&
					0 != static_cast<RendererRuntime::VrManagerOpenVR&>(vrManager).getVrSystem()->GetHiddenAreaMesh(vr::EVREye::Eye_Left).unTriangleCount)
				{
					::detail::g_MeshPtr = new ::detail::Mesh(rendererRuntime);
				}
			}
			if (nullptr != ::detail::g_MeshPtr)
			{
				::detail::g_MeshPtr->addReference();
			}
		#endif
	}

	CompositorInstancePassVrHiddenAreaMesh::~CompositorInstancePassVrHiddenAreaMesh()
	{
		#ifndef ANDROID
			// TODO(sw) For now no OpenVR support under Android
			// Release reference to vertex array object (VAO) shared between all compositor instance pass VR hidden area mesh instances
			if (nullptr != ::detail::g_MeshPtr && 1 == ::detail::g_MeshPtr->releaseReference())	// +1 for reference to global shared pointer
			{
				::detail::g_MeshPtr = nullptr;
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
