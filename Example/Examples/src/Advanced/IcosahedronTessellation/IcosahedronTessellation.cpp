/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "Advanced/IcosahedronTessellation/IcosahedronTessellation.h"
#include "Framework/Color4.h"

#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/matrix_transform.hpp>

#include <float.h> // For FLT_MAX
#include <string.h>
#include <stdlib.h> // For rand()


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
IcosahedronTessellation::IcosahedronTessellation(const char *rendererName) :
	IApplicationRenderer(rendererName),
	mTessellationLevelOuter(2.0f),
	mTessellationLevelInner(3.0f)
{
	// Nothing to do in here
}

IcosahedronTessellation::~IcosahedronTessellation()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void IcosahedronTessellation::onInitialization()
{
	// Call the base implementation
	IApplicationRenderer::onInitialization();

	// Get and check the renderer instance
	// -> Uniform buffer object (UBO, "constant buffer" in Direct3D terminology) supported?
	// -> Geometry shaders supported?
	// -> Tessellation control and tessellation evaluation shaders supported?
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && renderer->getCapabilities().uniformBuffer && renderer->getCapabilities().maximumNumberOfGsOutputVertices > 0 && renderer->getCapabilities().maximumNumberOfPatchVertices > 0)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		{ // Create the root signature
			// Setup
			Renderer::DescriptorRangeBuilder ranges[4];
			ranges[0].initialize(Renderer::DescriptorRangeType::CBV, 1, 0, "UniformBlockDynamicTcs", 0);
			ranges[1].initialize(Renderer::DescriptorRangeType::CBV, 1, 0, "UniformBlockStaticTes", 0);
			ranges[2].initialize(Renderer::DescriptorRangeType::CBV, 1, 0, "UniformBlockStaticGs", 0);
			ranges[3].initialize(Renderer::DescriptorRangeType::CBV, 1, 0, "UniformBlockStaticFs", 0);

			Renderer::RootParameterBuilder rootParameters[4];
			rootParameters[0].initializeAsDescriptorTable(1, &ranges[0], Renderer::ShaderVisibility::TESSELLATION_CONTROL);
			rootParameters[1].initializeAsDescriptorTable(1, &ranges[1], Renderer::ShaderVisibility::TESSELLATION_EVALUATION);
			rootParameters[2].initializeAsDescriptorTable(1, &ranges[2], Renderer::ShaderVisibility::GEOMETRY);
			rootParameters[3].initializeAsDescriptorTable(1, &ranges[3], Renderer::ShaderVisibility::FRAGMENT);

			// Setup
			Renderer::RootSignatureBuilder rootSignature;
			rootSignature.initialize(sizeof(rootParameters) / sizeof(Renderer::RootParameter), rootParameters, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			// Create the instance
			mRootSignature = renderer->createRootSignature(rootSignature);
		}

		// Vertex input layout
		const Renderer::VertexAttribute vertexAttributesLayout[] =
		{
			{ // Attribute 0
				// Data destination
				Renderer::VertexAttributeFormat::FLOAT_3,	// vertexAttributeFormat (Renderer::VertexAttributeFormat::Enum)
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
			// -> Geometry is from: http://prideout.net/blog/?p=48 (Philip Rideout, "The Little Grasshopper - Graphics Programming Tips")
			static const float VERTEX_POSITION[] =
			{								// Vertex ID
				 0.000f,  0.000f,  1.000f,	// 0
				 0.894f,  0.000f,  0.447f,	// 1
				 0.276f,  0.851f,  0.447f,	// 2
				-0.724f,  0.526f,  0.447f,	// 3
				-0.724f, -0.526f,  0.447f,	// 4
				 0.276f, -0.851f,  0.447f,	// 5
				 0.724f,  0.526f, -0.447f,	// 6
				-0.276f,  0.851f, -0.447f,	// 7
				-0.894f,  0.000f, -0.447f,	// 8
				-0.276f, -0.851f, -0.447f,	// 9
				 0.724f, -0.526f, -0.447f,	// 10
				 0.000f,  0.000f, -1.000f	// 11
			};
			Renderer::IVertexBufferPtr vertexBuffer(renderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

			// Create the index buffer object (IBO)
			// -> Geometry is from: http://prideout.net/blog/?p=48 (Philip Rideout, "The Little Grasshopper - Graphics Programming Tips")
			static const uint16_t INDICES[] =
			{				// Triangle ID
				2,  1,  0,	// 0
				3,  2,  0,	// 1
				4,  3,  0,	// 2
				5,  4,  0,	// 3
				1,  5,  0,	// 4
				11,  6,  7,	// 5
				11,  7,  8,	// 6
				11,  8,  9,	// 7
				11,  9, 10,	// 8
				11, 10,  6,	// 9
				1,  2,  6,	// 10
				2,  3,  7,	// 11
				3,  4,  8,	// 12
				4,  5,  9,	// 13
				5,  1, 10,	// 14
				2,  7,  6,	// 15
				3,  8,  7,	// 16
				4,  9,  8,	// 17
				5, 10,  9,	// 18
				1,  6, 10	// 19
			};
			Renderer::IIndexBuffer *indexBuffer = renderer->createIndexBuffer(sizeof(INDICES), Renderer::IndexBufferFormat::UNSIGNED_SHORT, INDICES, Renderer::BufferUsage::STATIC_DRAW);

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
					sizeof(float) * 3	// strideInBytes (uint32_t)
				}
			};
			mVertexArray = renderer->createVertexArray(vertexAttributes, sizeof(vertexArrayVertexBuffers) / sizeof(Renderer::VertexArrayVertexBuffer), vertexArrayVertexBuffers, indexBuffer);
		}

		// Decide which shader language should be used (for example "GLSL" or "HLSL")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer->getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			// Create uniform buffers and fill the static buffers at once
			mUniformBufferDynamicTcs = shaderLanguage->createUniformBuffer(sizeof(float) * 2, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
			{	// "ObjectSpaceToClipSpaceMatrix"
				// TODO(co) Cleanup, correct aspect ratio
				glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
				glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
				glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.001f, 1000.0f);
				glm::mat4 MVP = Projection * View; 
//				glm::mat4 MVP = Projection * View * Model; 
				mUniformBufferStaticTes = shaderLanguage->createUniformBuffer(sizeof(float) * 4 * 4, glm::value_ptr(MVP), Renderer::BufferUsage::STATIC_DRAW);
				{	// "NormalMatrix"

					View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
					Model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
					Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.001f, 1000.0f);
					MVP = Projection * View; 
					glm::mat3 nMVP(MVP);
					glm::mat4 tMVP(nMVP);
					mUniformBufferStaticGs = shaderLanguage->createUniformBuffer(sizeof(float) * 4 * 4, glm::value_ptr(tMVP), Renderer::BufferUsage::STATIC_DRAW);
				}
			}
			{ // Light and material
				static const float LIGHT_AND_MATERIAL[] =
				{
					0.25f, 0.25f, 1.0f, 1.0,	// "LightPosition"
					 0.0f, 0.75f, 0.75f,1.0, 	// "DiffuseMaterial"
					0.04f, 0.04f, 0.04f,1.0,	// "AmbientMaterial"
				};
				mUniformBufferStaticFs = shaderLanguage->createUniformBuffer(sizeof(LIGHT_AND_MATERIAL), LIGHT_AND_MATERIAL, Renderer::BufferUsage::STATIC_DRAW);
			}

			// Create the program
			Renderer::IProgramPtr program;
			{
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderSourceCode = nullptr;
				const char *tessellationControlShaderSourceCode = nullptr;
				const char *tessellationEvaluationShaderSourceCode = nullptr;
				const char *geometryShaderSourceCode = nullptr;
				const char *fragmentShaderSourceCode = nullptr;
				#include "IcosahedronTessellation_GLSL_400.h"
				#include "IcosahedronTessellation_HLSL_D3D11_D3D12.h"
				#include "IcosahedronTessellation_Null.h"

				// Create the program
				program = shaderLanguage->createProgram(
					*mRootSignature,
					vertexAttributes,
					shaderLanguage->createVertexShaderFromSourceCode(vertexShaderSourceCode),
					shaderLanguage->createTessellationControlShaderFromSourceCode(tessellationControlShaderSourceCode),
					shaderLanguage->createTessellationEvaluationShaderFromSourceCode(tessellationEvaluationShaderSourceCode),
					shaderLanguage->createGeometryShaderFromSourceCode(geometryShaderSourceCode, Renderer::GsInputPrimitiveTopology::TRIANGLES, Renderer::GsOutputPrimitiveTopology::TRIANGLE_STRIP, 3),
					shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode));
			}

			// Create the pipeline state object (PSO)
			if (nullptr != program)
			{
				Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder(mRootSignature, program, vertexAttributes);
				pipelineState.primitiveTopologyType = Renderer::PrimitiveTopologyType::PATCH;
				mPipelineState = renderer->createPipelineState(pipelineState);
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void IcosahedronTessellation::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Release the used resources
	mUniformBufferStaticFs = nullptr;
	mUniformBufferStaticGs = nullptr;
	mUniformBufferStaticTes = nullptr;
	mUniformBufferDynamicTcs = nullptr;
	mVertexArray = nullptr;
	mPipelineState = nullptr;
	mRootSignature = nullptr;

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())

	// Call the base implementation
	IApplicationRenderer::onDeinitialization();
}

void IcosahedronTessellation::onDraw()
{
	// Update the uniform buffer content
	if (nullptr != mUniformBufferDynamicTcs)
	{
		// Copy data into the uniform buffer
		const float data[] =
		{
			mTessellationLevelOuter,	// "TessellationLevelOuter"
			mTessellationLevelInner		// "TessellationLevelInner"
		};
		mUniformBufferDynamicTcs->copyDataFrom(sizeof(data), data);
	}

	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && nullptr != mPipelineState)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Clear the color buffer of the current render target with gray, do also clear the depth buffer
		renderer->clear(Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

		// Set the used graphics root signature
		renderer->setGraphicsRootSignature(mRootSignature);

		// Set the used uniform buffers
		renderer->setGraphicsRootDescriptorTable(0, mUniformBufferDynamicTcs);
		renderer->setGraphicsRootDescriptorTable(1, mUniformBufferStaticTes);
		renderer->setGraphicsRootDescriptorTable(2, mUniformBufferStaticGs);
		renderer->setGraphicsRootDescriptorTable(3, mUniformBufferStaticFs);

		// Set the used pipeline state object (PSO)
		renderer->setPipelineState(mPipelineState);

		{ // Setup input assembly (IA)
			// Set the used vertex array
			renderer->iaSetVertexArray(mVertexArray);

			// Set the primitive topology used for draw calls
			// -> Patch list with 3 vertices per patch (tessellation relevant topology type) - "Renderer::PrimitiveTopology::TriangleList" used for tessellation
			renderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::PATCH_LIST_3);
		}

		// Render the specified geometric primitive, based on indexing into an array of vertices
		renderer->drawIndexed(0, 60, 0, 0, 12);

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}
