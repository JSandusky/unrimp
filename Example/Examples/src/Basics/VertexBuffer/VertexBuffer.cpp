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
#include "Basics/VertexBuffer/VertexBuffer.h"
#include "Framework/Color4.h"

#include <string.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
VertexBuffer::VertexBuffer(const char *rendererName) :
	IApplicationRenderer(rendererName)
{
	// Nothing here
}

VertexBuffer::~VertexBuffer()
{
	// The resources are released within "onDeinitialization()"
	// Nothing here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void VertexBuffer::onInitialization()
{
	// Call the base implementation
	IApplicationRenderer::onInitialization();

	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Create the buffer manager
		mBufferManager = renderer->createBufferManager();

		{ // Create the root signature
			// Setup
			Renderer::RootSignatureBuilder rootSignature;
			rootSignature.initialize(0, nullptr, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			// Create the instance
			mRootSignature = renderer->createRootSignature(rootSignature);
		}

		// Vertex input layout
		const Renderer::VertexAttribute vertexAttributesLayoutVBO[] =
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
				Renderer::VertexAttributeFormat::FLOAT_3,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
				"Color",									// name[32] (char)
				"COLOR",									// semanticName[32] (char)
				0,											// semanticIndex (uint32_t)
				// Data source
				0,											// inputSlot (uint32_t)
				sizeof(float) * 2,							// alignedByteOffset (uint32_t)
				// Data source, instancing part
				0											// instancesPerElement (uint32_t)
			}
		};
		const Renderer::VertexAttributes vertexAttributesVBO(glm::countof(vertexAttributesLayoutVBO), vertexAttributesLayoutVBO);
		const Renderer::VertexAttribute vertexAttributesLayoutVBOs[] =
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
				Renderer::VertexAttributeFormat::FLOAT_3,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
				"Color",									// name[32] (char)
				"COLOR",									// semanticName[32] (char)
				0,											// semanticIndex (uint32_t)
				// Data source
				1,											// inputSlot (uint32_t)
				0,											// alignedByteOffset (uint32_t)
				// Data source, instancing part
				0											// instancesPerElement (uint32_t)
			}
		};
		const Renderer::VertexAttributes vertexAttributesVBOs(glm::countof(vertexAttributesLayoutVBOs), vertexAttributesLayoutVBOs);

		// Vertex array object (VAO)
		// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
		// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
		// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
		//    reference of the used vertex buffer objects (VBO). If the reference counter of a
		//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.

		{ // Create vertex array object (VAO)
			// Create the vertex buffer object (VBO) holding position and color data
			// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
			// -> Traditional normalized RGB vertex colors
			static const float VERTEX_POSITION_COLOR[] =
			{	 // Position     Color				// Vertex ID	Triangle on screen
				 0.0f, 1.0f,	1.0f, 0.0f, 0.0f,	// 0				0
				 1.0f, 0.0f,	0.0f, 1.0f, 0.0f,	// 1			   .   .
				-0.5f, 0.0f,	0.0f, 0.0f, 1.0f	// 2			  2.......1
			};
			Renderer::IVertexBufferPtr vertexBufferPositionColor(mBufferManager->createVertexBuffer(sizeof(VERTEX_POSITION_COLOR), VERTEX_POSITION_COLOR, Renderer::BufferUsage::STATIC_DRAW));

			// Create vertex array object (VAO)
			const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] =
			{
				{ // Vertex buffer 0
					vertexBufferPositionColor,	// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * (2 + 3)		// strideInBytes (uint32_t)
				}
			};
			mVertexArrayVBO = mBufferManager->createVertexArray(vertexAttributesVBO, glm::countof(vertexArrayVertexBuffers), vertexArrayVertexBuffers);
		}

		{ // Create vertex array object (VAO) using multiple vertex buffer object (VBO)
			// Create the vertex buffer object (VBO) holding color data
			// -> Traditional normalized RGB vertex colors
			static const float VERTEX_COLOR[] =
			{					// Vertex ID	Triangle on screen
				1.0f, 0.0f, 0.0f,	// 0			  0.......1
				0.0f, 1.0f, 0.0f,	// 1			   .   .
				0.0f, 0.0f, 1.0f	// 2			  	2
			};
			Renderer::IVertexBufferPtr vertexBufferColor(mBufferManager->createVertexBuffer(sizeof(VERTEX_COLOR), VERTEX_COLOR, Renderer::BufferUsage::STATIC_DRAW));

			// Create the vertex buffer object (VBO) holding position data
			// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
			static const float VERTEX_POSITION[] =
			{					// Vertex ID	Triangle on screen
				-0.5f,  0.0f,	// 0			  0.......1
				 1.0f,  0.0f,	// 1			   .   .
				 0.0f, -1.0f	// 2			  	2
			};
			Renderer::IVertexBufferPtr vertexBufferPosition(mBufferManager->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

			// Create vertex array object (VAO)
			const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] =
			{
				{ // Vertex buffer 0
					vertexBufferPosition,	// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 2		// strideInBytes (uint32_t)
				},
				{ // Vertex buffer 1
					vertexBufferColor,		// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 3		// strideInBytes (uint32_t)
				}
			};
			mVertexArrayVBOs = mBufferManager->createVertexArray(vertexAttributesVBOs, glm::countof(vertexArrayVertexBuffers), vertexArrayVertexBuffers);
		}

		// Decide which shader language should be used (for example "GLSL" or "HLSL")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer->getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			// Get the shader source code (outsourced to keep an overview)
			const char *vertexShaderSourceCode = nullptr;
			const char *fragmentShaderSourceCode = nullptr;
			#include "VertexBuffer_GLSL_410.h"
			#include "VertexBuffer_GLSL_ES2.h"
			#include "VertexBuffer_HLSL_D3D9_D3D10_D3D11_D3D12.h"
			#include "VertexBuffer_Null.h"

			{ // Create pipeline state objects (PSO) using one vertex buffer object (VBO)
				// Create the program
				Renderer::IProgramPtr program;
				program = shaderLanguage->createProgram(
					*mRootSignature,
					vertexAttributesVBO,
					shaderLanguage->createVertexShaderFromSourceCode(vertexAttributesVBO, vertexShaderSourceCode),
					shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode));

				// Create the pipeline state objects (PSO)
				if (nullptr != program)
				{
					mPipelineStateVBO = renderer->createPipelineState(Renderer::PipelineStateBuilder(mRootSignature, program, vertexAttributesVBO));
				}
			}

			{ // Create pipeline state objects (PSO) using multiple vertex buffer object (VBO)
				// Create the program
				Renderer::IProgramPtr program;
				program = shaderLanguage->createProgram(
					*mRootSignature,
					vertexAttributesVBOs,
					shaderLanguage->createVertexShaderFromSourceCode(vertexAttributesVBOs, vertexShaderSourceCode),
					shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode));

				// Create the pipeline state objects (PSO)
				if (nullptr != program)
				{
					mPipelineStateVBOs = renderer->createPipelineState(Renderer::PipelineStateBuilder(mRootSignature, program, vertexAttributesVBOs));
				}
			}
		}

		// Since we're always submitting the same commands to the renderer, we can fill the command buffer once during initialization and then reuse it multiple times during runtime
		fillCommandBuffer();
	}
}

void VertexBuffer::onDeinitialization()
{
	// Release the used resources
	mCommandBuffer.clear();
	mVertexArrayVBOs = nullptr;
	mPipelineStateVBOs = nullptr;
	mVertexArrayVBO = nullptr;
	mPipelineStateVBO = nullptr;
	mRootSignature = nullptr;
	mBufferManager = nullptr;

	// Call the base implementation
	IApplicationRenderer::onDeinitialization();
}

void VertexBuffer::onDraw()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Submit command buffer to the renderer backend
		mCommandBuffer.submit(*renderer);
	}
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
void VertexBuffer::fillCommandBuffer()
{
	// Sanity checks
	assert(nullptr != mRootSignature);
	assert(nullptr != mPipelineStateVBO);
	assert(nullptr != mVertexArrayVBO);
	assert(nullptr != mPipelineStateVBOs);
	assert(nullptr != mVertexArrayVBOs);
	assert(mCommandBuffer.isEmpty());

	// Begin debug event
	COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(mCommandBuffer)

	// Clear the color buffer of the current render target with gray, do also clear the depth buffer
	Renderer::Command::Clear::create(mCommandBuffer, Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

	// Set the used graphics root signature
	Renderer::Command::SetGraphicsRootSignature::create(mCommandBuffer, mRootSignature);

	// First lower triangle using one vertex buffer object (VBO)
	if (nullptr != mPipelineStateVBO)
	{
		// Begin debug event
		COMMAND_BEGIN_DEBUG_EVENT(mCommandBuffer, L"Draw using one VBO")

		// Set the used pipeline state object (PSO)
		Renderer::Command::SetPipelineState::create(mCommandBuffer, mPipelineStateVBO);

		{ // Setup input assembly (IA)
			// Set the used vertex array
			Renderer::Command::SetVertexArray::create(mCommandBuffer, mVertexArrayVBO);

			// Set the primitive topology used for draw calls
			Renderer::Command::SetPrimitiveTopology::create(mCommandBuffer, Renderer::PrimitiveTopology::TRIANGLE_LIST);
		}

		// Render the specified geometric primitive, based on an array of vertices
		Renderer::Command::Draw::create(mCommandBuffer, 3);

		// End debug event
		COMMAND_END_DEBUG_EVENT(mCommandBuffer)
	}

	// Second upper triangle using multiple vertex buffer object (VBO)
	if (nullptr != mPipelineStateVBOs)
	{
		// Begin debug event
		COMMAND_BEGIN_DEBUG_EVENT(mCommandBuffer, L"Draw using multiple VBOs")

		// Set the used pipeline state object (PSO)
		Renderer::Command::SetPipelineState::create(mCommandBuffer, mPipelineStateVBOs);

		{ // Setup input assembly (IA)
			// Set the used vertex array
			Renderer::Command::SetVertexArray::create(mCommandBuffer, mVertexArrayVBOs);

			// Set the primitive topology used for draw calls
			Renderer::Command::SetPrimitiveTopology::create(mCommandBuffer, Renderer::PrimitiveTopology::TRIANGLE_LIST);
		}

		// Render the specified geometric primitive, based on an array of vertices
		Renderer::Command::Draw::create(mCommandBuffer, 3);

		// End debug event
		COMMAND_END_DEBUG_EVENT(mCommandBuffer)
	}

	// End debug event
	COMMAND_END_DEBUG_EVENT(mCommandBuffer)
}
