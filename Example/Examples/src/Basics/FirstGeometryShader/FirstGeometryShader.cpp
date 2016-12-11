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
#include "Basics/FirstGeometryShader/FirstGeometryShader.h"
#include "Framework/Color4.h"

#include <string.h> // for strcmp()


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstGeometryShader::FirstGeometryShader(const char *rendererName) :
	IApplicationRenderer(rendererName)
{
	// Nothing here
}

FirstGeometryShader::~FirstGeometryShader()
{
	// The resources are released within "onDeinitialization()"
	// Nothing here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstGeometryShader::onInitialization()
{
	// Call the base implementation
	IApplicationRenderer::onInitialization();

	// Get and check the renderer instance
	// -> Geometry shaders supported?
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && renderer->getCapabilities().maximumNumberOfGsOutputVertices)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

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
		const Renderer::VertexAttribute vertexAttributesLayout[] =
		{
			{ // Attribute 0
				// Data destination
				Renderer::VertexAttributeFormat::FLOAT_1,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
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
		const Renderer::VertexAttributes vertexAttributes(glm::countof(vertexAttributesLayout), vertexAttributesLayout);

		{ // Create vertex array object (VAO)
			// Create the vertex buffer object (VBO)
			static const float VERTEX_POSITION[] =
			{			// Vertex ID
				42.0f	// 0
			};
			Renderer::IVertexBufferPtr vertexBuffer(mBufferManager->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

			// Create vertex array object (VAO)
			// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
			// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
			// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
			//    reference of the used vertex buffer objects (VBO). If the reference counter of a
			//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
			const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] =
			{
				{ // Vertex buffer 0
					vertexBuffer,	// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float)	// strideInBytes (uint32_t)
				}
			};
			mVertexArray = mBufferManager->createVertexArray(vertexAttributes, glm::countof(vertexArrayVertexBuffers), vertexArrayVertexBuffers);
		}

		// Create the program: Decide which shader language should be used (for example "GLSL" or "HLSL")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer->getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			// Create the program
			Renderer::IProgramPtr program;
			{
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderSourceCode = nullptr;
				const char *geometryShaderSourceCode = nullptr;
				const char *fragmentShaderSourceCode = nullptr;
				#include "FirstGeometryShader_GLSL_410.h"
				#include "FirstGeometryShader_HLSL_D3D10_D3D11_D3D12.h"
				#include "FirstGeometryShader_Null.h"

				// Create the program
				program = shaderLanguage->createProgram(
					*mRootSignature,
					vertexAttributes,
					shaderLanguage->createVertexShaderFromSourceCode(vertexAttributes, vertexShaderSourceCode),
					shaderLanguage->createGeometryShaderFromSourceCode(geometryShaderSourceCode, Renderer::GsInputPrimitiveTopology::POINTS, Renderer::GsOutputPrimitiveTopology::TRIANGLE_STRIP, 3),
					shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode));
			}

			// Create the pipeline state object (PSO)
			if (nullptr != program)
			{
				Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder(mRootSignature, program, vertexAttributes);
				pipelineState.primitiveTopologyType = Renderer::PrimitiveTopologyType::POINT;
				mPipelineState = renderer->createPipelineState(pipelineState);
			}
		}

		// Since we're always submitting the same commands to the renderer, we can fill the command buffer once during initialization and then reuse it multiple times during runtime
		fillCommandBuffer();

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstGeometryShader::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Release the used resources
	mCommandBuffer.clear();
	mVertexArray = nullptr;
	mPipelineState = nullptr;
	mRootSignature = nullptr;
	mBufferManager = nullptr;

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())

	// Call the base implementation
	IApplicationRenderer::onDeinitialization();
}

void FirstGeometryShader::onDraw()
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
void FirstGeometryShader::fillCommandBuffer()
{
	// Sanity checks
	assert(nullptr != mRootSignature);
	assert(nullptr != mPipelineState);
	assert(nullptr != mVertexArray);
	assert(mCommandBuffer.isEmpty());

	// Begin debug event
	COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(mCommandBuffer)

	// Clear the color buffer of the current render target with gray, do also clear the depth buffer
	Renderer::Command::Clear::create(mCommandBuffer, Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

	// Set the used graphics root signature
	Renderer::Command::SetGraphicsRootSignature::create(mCommandBuffer, mRootSignature);

	// Set the used pipeline state object (PSO)
	Renderer::Command::SetPipelineState::create(mCommandBuffer, mPipelineState);

	{ // Setup input assembly (IA)
		// Set the used vertex array
		Renderer::Command::SetVertexArray::create(mCommandBuffer, mVertexArray);

		// Set the primitive topology used for draw calls
		Renderer::Command::SetPrimitiveTopology::create(mCommandBuffer, Renderer::PrimitiveTopology::POINT_LIST);
	}

	// Render the specified geometric primitive, based on an array of vertices
	// -> Emit a single point in order to generate a draw call, the geometry shader does the rest
	// -> Attribute less rendering (aka "drawing without data")
	Renderer::Command::Draw::create(mCommandBuffer, 1);

	// End debug event
	COMMAND_END_DEBUG_EVENT(mCommandBuffer)
}
