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
#include "PrecompiledHeader.h"


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_RUNTIME


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Runtime/FirstCommandBucket/FirstCommandBucket.h"
#include "Framework/Color4.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstCommandBucket::FirstCommandBucket(const char *rendererName) :
	IApplicationRendererRuntime(rendererName),
	mSolidCommandBucket(4),
	mTransparentCommandBucket(2)
{
	// Nothing to do in here
}

FirstCommandBucket::~FirstCommandBucket()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstCommandBucket::onInitialization()
{
	// Call the base implementation
	IApplicationRendererRuntime::onInitialization();

	// Get and check the renderer runtime instance
	RendererRuntime::IRendererRuntimePtr rendererRuntime(getRendererRuntime());
	if (nullptr != rendererRuntime)
	{
		// Get the renderer instance (at this point in time we know it must be valid)
		Renderer::IRendererPtr renderer(getRenderer());

		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

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
				// Data source, instancing part
				0											// instancesPerElement (uint32_t)
			}
		};
		const Renderer::VertexAttributes vertexAttributes(sizeof(vertexAttributesLayout) / sizeof(Renderer::VertexAttribute), vertexAttributesLayout);

		{ // Create vertex array object (VAO)
			// Create the vertex buffer object (VBO)
			// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
			static const float VERTEX_POSITION[] =
			{					// Vertex ID	Triangle on screen
				 0.0f, 1.0f,	// 0				0
				 1.0f, 0.0f,	// 1			   .   .
				-0.5f, 0.0f		// 2			  2.......1
			};
			Renderer::IVertexBufferPtr vertexBuffer(renderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));
			RENDERER_SET_RESOURCE_DEBUG_NAME(vertexBuffer, "Triangle VBO")

			// Create the index buffer object (IBO)
			static const uint16_t INDICES[] =
			{
				0, 1, 2
			};
			Renderer::IIndexBufferPtr indexBuffer(renderer->createIndexBuffer(sizeof(INDICES), Renderer::IndexBufferFormat::UNSIGNED_SHORT, INDICES, Renderer::BufferUsage::STATIC_DRAW));

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
			mSolidVertexArray = renderer->createVertexArray(vertexAttributes, sizeof(vertexArrayVertexBuffers) / sizeof(Renderer::VertexArrayVertexBuffer), vertexArrayVertexBuffers);
			RENDERER_SET_RESOURCE_DEBUG_NAME(mSolidVertexArray, "Solid triangle VAO")
			mTransparentVertexArray = renderer->createVertexArray(vertexAttributes, sizeof(vertexArrayVertexBuffers) / sizeof(Renderer::VertexArrayVertexBuffer), vertexArrayVertexBuffers, indexBuffer);
			RENDERER_SET_RESOURCE_DEBUG_NAME(mTransparentVertexArray, "Transparent triangle VAO")
		}

		{ // Create the root signature
			// Setup
			Renderer::DescriptorRangeBuilder ranges[1];
			ranges[0].initialize(Renderer::DescriptorRangeType::UBV, 1, 0, "UniformBlockDynamicVs", 0);

			Renderer::RootParameterBuilder rootParameters[1];
			rootParameters[0].initializeAsDescriptorTable(1, &ranges[0], Renderer::ShaderVisibility::VERTEX);

			// Setup
			Renderer::RootSignatureBuilder rootSignature;
			rootSignature.initialize(sizeof(rootParameters) / sizeof(Renderer::RootParameter), rootParameters, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			// Create the instance
			mRootSignature = renderer->createRootSignature(rootSignature);
		}

		// Decide which shader language should be used (for example "GLSL" or "HLSL")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer->getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			// Create the program
			Renderer::IProgramPtr program;
			{
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderSourceCode = nullptr;
				const char *fragmentShaderSourceCode = nullptr;
				#include "FirstCommandBucket_GLSL_110.h"
				#include "FirstCommandBucket_GLSL_140.h"
				#include "FirstCommandBucket_GLSL_ES2.h"
				#include "FirstCommandBucket_HLSL_D3D9.h"
				#include "FirstCommandBucket_HLSL_D3D10_D3D11_D3D12.h"
				#include "FirstCommandBucket_Null.h"

				// Create the vertex shader
				Renderer::IVertexShader *vertexShader = shaderLanguage->createVertexShaderFromSourceCode(vertexShaderSourceCode);
				RENDERER_SET_RESOURCE_DEBUG_NAME(vertexShader, "Triangle VS")

				// Create the fragment shader
				Renderer::IFragmentShader *fragmentShader = shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode);
				RENDERER_SET_RESOURCE_DEBUG_NAME(fragmentShader, "Triangle FS")

				// Create the program
				program = shaderLanguage->createProgram(*mRootSignature, vertexAttributes, vertexShader, fragmentShader);
				RENDERER_SET_RESOURCE_DEBUG_NAME(program, "Triangle program")
			}

			// Uniform buffer object (UBO, "constant buffer" in Direct3D terminology) supported?
			// -> If they are there, we really want to use them (performance and ease of use)
			if (renderer->getCapabilities().uniformBuffer)
			{
				// Create dynamic uniform buffer
				mUniformBufferDynamicVs = shaderLanguage->createUniformBuffer(sizeof(float) * 2, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
			}

			// Create solid material
			mSolidMaterial.pipelineState = renderer->createPipelineState(Renderer::PipelineStateBuilder(mRootSignature, program, vertexAttributes));

			{ // Create transparent material
				Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder(mRootSignature, program, vertexAttributes);
				pipelineState.blendState.renderTarget[0].blendEnable = true;
				pipelineState.blendState.renderTarget[0].srcBlend    = Renderer::Blend::SRC_ALPHA;
				pipelineState.blendState.renderTarget[0].destBlend   = Renderer::Blend::ONE;
				mTransparentMaterial.pipelineState = renderer->createPipelineState(pipelineState);
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstCommandBucket::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Release the used resources
	mRootSignature			= nullptr;
	mUniformBufferDynamicVs = nullptr;
	mSolidVertexArray		= nullptr;
	mTransparentVertexArray = nullptr;
	mSolidMaterial.clear();
	mTransparentMaterial.clear();

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())

	// Call the base implementation
	IApplicationRendererRuntime::onDeinitialization();
}

void FirstCommandBucket::onDraw()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Clear the color buffer of the current render target with gray, do also clear the depth buffer
		renderer->clear(Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

		// Set the used graphics root signature
		renderer->setGraphicsRootSignature(mRootSignature);

		// Set the used uniform buffers
		// TODO(co) Has to be part of material binding
		if (nullptr != mUniformBufferDynamicVs)
		{
			// Set the used uniform buffers
			renderer->setGraphicsRootDescriptorTable(0, mUniformBufferDynamicVs);
		}

		{ // Push draw calls into different command buckets (can be done in parallel)
			{ // Solid stuff
				// Update uniform buffer content
				const float offset[] = { 0.0f, 0.0f };
				RendererRuntime::Command::CopyUniformBufferData *copyUniformBufferDataCommand = mSolidCommandBucket.addCommand<RendererRuntime::Command::CopyUniformBufferData>(42, sizeof(offset));
				copyUniformBufferDataCommand->uniformBufferDynamicVs = mUniformBufferDynamicVs;
				copyUniformBufferDataCommand->size = sizeof(offset);
				copyUniformBufferDataCommand->data = RendererRuntime::commandPacket::GetAuxiliaryMemory(copyUniformBufferDataCommand);
				memcpy(copyUniformBufferDataCommand->data, &offset, sizeof(offset));

				// Draw call
				RendererRuntime::Command::Draw *drawCommand = mSolidCommandBucket.appendCommand<RendererRuntime::Command::Draw>(copyUniformBufferDataCommand);
				drawCommand->iaVertexArray		 = mSolidVertexArray;
				drawCommand->iaPrimitiveTopology = Renderer::PrimitiveTopology::TRIANGLE_LIST;
				drawCommand->material			 = &mSolidMaterial;
				drawCommand->startVertexLocation = 0;
				drawCommand->numberOfVertices	 = 3;
			}

			// Transparent stuff
			for (int i = 0; i < 2; ++i)
			{
				// Update uniform buffer content
				const float offset[] = { 0.25f - i * 0.5f, 0.25f - i * 0.5f };
				RendererRuntime::Command::CopyUniformBufferData *copyUniformBufferDataCommand = mTransparentCommandBucket.addCommand<RendererRuntime::Command::CopyUniformBufferData>(42 - i, sizeof(offset));
				copyUniformBufferDataCommand->uniformBufferDynamicVs = mUniformBufferDynamicVs;
				copyUniformBufferDataCommand->size = sizeof(offset);
				copyUniformBufferDataCommand->data = RendererRuntime::commandPacket::GetAuxiliaryMemory(copyUniformBufferDataCommand);
				memcpy(copyUniformBufferDataCommand->data, &offset, sizeof(offset));

				// Draw call
				RendererRuntime::Command::DrawIndexed *drawIndexedCommand = mTransparentCommandBucket.appendCommand<RendererRuntime::Command::DrawIndexed>(copyUniformBufferDataCommand);
				drawIndexedCommand->iaVertexArray		= mTransparentVertexArray;
				drawIndexedCommand->iaPrimitiveTopology = Renderer::PrimitiveTopology::TRIANGLE_LIST;
				drawIndexedCommand->material			= &mTransparentMaterial;
				drawIndexedCommand->startIndexLocation  = 0;
				drawIndexedCommand->numberOfIndices		= 3;
				drawIndexedCommand->baseVertexLocation	= 0;
			}
		}

		// Sort command buckets by command key (can be done in parallel)
		mSolidCommandBucket.sort();
		mTransparentCommandBucket.sort();

		// Submit command buckets to the renderer backend (has to be done sequential)
		mSolidCommandBucket.submit(*renderer);
		mTransparentCommandBucket.submit(*renderer);

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#endif // RENDERER_NO_RUNTIME
