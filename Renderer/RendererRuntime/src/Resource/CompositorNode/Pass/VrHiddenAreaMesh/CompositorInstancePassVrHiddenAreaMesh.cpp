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
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/IMaterialBlueprintResourceListener.h"
#include "RendererRuntime/Vr/OpenVR/VrManagerOpenVR.h"
#include "RendererRuntime/IRendererRuntime.h"

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
					mNumberOfTriangles{ 0, 0 }
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
							0,											// inputSlot (size_t)
							0,											// alignedByteOffset (uint32_t)
							// Data source, instancing part
							0											// instancesPerElement (uint32_t)
						}
					};
					const Renderer::VertexAttributes vertexAttributes(static_cast<uint32_t>(glm::countof(vertexAttributesLayout)), vertexAttributesLayout);

					{ // Create vertex arrays
						Renderer::IBufferManager& bufferManager = rendererRuntime.getBufferManager();
						vr::IVRSystem* vrSystem = static_cast<RendererRuntime::VrManagerOpenVR&>(rendererRuntime.getVrManager()).getVrSystem();
						for (int i = 0; i < 2; ++i)
						{
							const vr::HiddenAreaMesh_t vrHiddenAreaMesh = vrSystem->GetHiddenAreaMesh(static_cast<vr::EVREye>(i));
							mNumberOfTriangles[i] = vrHiddenAreaMesh.unTriangleCount;

							// Create the vertex buffer object (VBO)
							Renderer::IVertexBufferPtr vertexBuffer(bufferManager.createVertexBuffer(sizeof(vr::HmdVector2_t) * 3 * vrHiddenAreaMesh.unTriangleCount, vrHiddenAreaMesh.pVertexData, Renderer::BufferUsage::STATIC_DRAW));
							RENDERER_SET_RESOURCE_DEBUG_NAME(vertexBuffer, "Compositor instance pass VR hidden area mesh")

							// Create vertex array object (VAO)
							// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
							// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
							// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
							//    reference of the used vertex buffer objects (VBO). If the reference counter of a
							//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
							const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] =
							{
								{ // Vertex buffer 0
									vertexBuffer,		// vertexBuffer (Renderer::IVertexBuffer *)
									sizeof(float) * 2	// strideInBytes (uint32_t)
								}
							};
							mVertexArrayPtr[i] = bufferManager.createVertexArray(vertexAttributes, static_cast<uint32_t>(glm::countof(vertexArrayVertexBuffers)), vertexArrayVertexBuffers);
							RENDERER_SET_RESOURCE_DEBUG_NAME(mVertexArrayPtr[i], "Compositor instance pass VR hidden area mesh")
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
							Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder(mRootSignature, program, vertexAttributes);
							pipelineState.rasterizerState.cullMode = Renderer::CullMode::NONE;
							mPipelineState = renderer.createPipelineState(pipelineState);
							RENDERER_SET_RESOURCE_DEBUG_NAME(mPipelineState, "Compositor instance pass VR hidden area mesh PSO")
						}
					}
				}

				~Mesh()
				{
					mRootSignature->releaseReference();
					mVertexArrayPtr[0]->releaseReference();
					mVertexArrayPtr[1]->releaseReference();
					mPipelineState->releaseReference();
				}

				void onFillCommandBuffer(RendererRuntime::IMaterialBlueprintResourceListener::VrEye vrEye, Renderer::CommandBuffer& commandBuffer)
				{
					// Set the used graphics root signature
					Renderer::Command::SetGraphicsRootSignature::create(commandBuffer, mRootSignature);

					// Set the used pipeline state object (PSO)
					Renderer::Command::SetPipelineState::create(commandBuffer, mPipelineState);

					{ // Setup input assembly (IA)
						// Set the used vertex array
						Renderer::Command::SetVertexArray::create(commandBuffer, mVertexArrayPtr[static_cast<int>(vrEye)]);

						// Set the primitive topology used for draw calls
						Renderer::Command::SetPrimitiveTopology::create(commandBuffer, Renderer::PrimitiveTopology::TRIANGLE_LIST);
					}

					// Render the specified geometric primitive, based on an array of vertices
					Renderer::Command::Draw::create(commandBuffer, mNumberOfTriangles[static_cast<int>(vrEye)] * 3);
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
				Renderer::IVertexArrayPtr	mVertexArrayPtr[2];
				uint32_t					mNumberOfTriangles[2];
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
				::detail::g_MeshPtr->onFillCommandBuffer(getCompositorNodeInstance().getCompositorWorkspaceInstance().getRendererRuntime().getMaterialBlueprintResourceManager().getMaterialBlueprintResourceListener().getCurrentRenderedVrEye(), commandBuffer);

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
