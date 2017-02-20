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
#include "PrecompiledHeader.h"
#include "Runtime/FirstMesh/FirstMesh.h"
#include "Framework/Color4.h"

#include <RendererRuntime/Resource/Mesh/MeshResource.h>
#include <RendererRuntime/Resource/Mesh/MeshResourceManager.h>
#include <RendererRuntime/Resource/Texture/TextureResource.h>
#include <RendererRuntime/Resource/Texture/TextureResourceManager.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstMesh::FirstMesh() :
	mMeshResourceId(RendererRuntime::getUninitialized<RendererRuntime::MeshResourceId>()),
	mDiffuseTextureResourceId(RendererRuntime::getUninitialized<RendererRuntime::TextureResourceId>()),
	mNormalTextureResourceId(RendererRuntime::getUninitialized<RendererRuntime::TextureResourceId>()),
	mSpecularTextureResourceId(RendererRuntime::getUninitialized<RendererRuntime::TextureResourceId>()),
	mEmissiveTextureResourceId(RendererRuntime::getUninitialized<RendererRuntime::TextureResourceId>()),
	mObjectSpaceToClipSpaceMatrixUniformHandle(NULL_HANDLE),
	mObjectSpaceToViewSpaceMatrixUniformHandle(NULL_HANDLE),
	mGlobalTimer(0.0f)
{
	// Nothing here
}

FirstMesh::~FirstMesh()
{
	// The resources are released within "onDeinitialization()"
	// Nothing here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstMesh::onInitialization()
{
	// Call the base implementation
	ExampleBase::onInitialization();

	// Get and check the renderer runtime instance
	RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
	if (nullptr != rendererRuntime)
	{
		Renderer::IRendererPtr renderer(getRenderer());

		// Create uniform buffer
		// -> Direct3D 9 does not support uniform buffers
		// -> Direct3D 10, 11 and 12 do not support individual uniforms
		// -> The renderer is just a light weight abstraction layer, so we need to handle the differences
		// -> Allocate enough memory for two 4x4 floating point matrices
		mUniformBuffer = rendererRuntime->getBufferManager().createUniformBuffer(2 * 4 * 4 * sizeof(float), nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);

		// Decide which shader language should be used (for example "GLSL" or "HLSL")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer->getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			// Vertex input layout
			const Renderer::VertexAttribute vertexAttributesLayout[] =
			{
				{ // Attribute 0
					// Data destination
					Renderer::VertexAttributeFormat::FLOAT_3,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
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
					sizeof(float) * 3,							// alignedByteOffset (uint32_t)
					// Data source, instancing part
					0											// instancesPerElement (uint32_t)
				},
				{ // Attribute 2
					// Data destination
					Renderer::VertexAttributeFormat::SHORT_4,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
					"QTangent",									// name[32] (char)
					"TEXCOORD",									// semanticName[32] (char)
					1,											// semanticIndex (uint32_t)
					// Data source
					0,											// inputSlot (uint32_t)
					sizeof(float) * 3 + sizeof(float) * 2,		// alignedByteOffset (uint32_t)
					// Data source, instancing part
					0											// instancesPerElement (uint32_t)
				}
			};
			const Renderer::VertexAttributes vertexAttributes(static_cast<uint32_t>(glm::countof(vertexAttributesLayout)), vertexAttributesLayout);

			{ // Create the root signature
				Renderer::DescriptorRangeBuilder ranges[6];
				ranges[0].initialize(Renderer::DescriptorRangeType::UBV, 1, 0, "UniformBlockDynamicVs", 0);
				ranges[1].initializeSampler(1, 0);
				ranges[2].initialize(Renderer::DescriptorRangeType::SRV, 1, 0, "DiffuseMap", 1);
				ranges[3].initialize(Renderer::DescriptorRangeType::SRV, 1, 1, "EmissiveMap", 1);
				ranges[4].initialize(Renderer::DescriptorRangeType::SRV, 1, 2, "NormalMap", 1);
				ranges[5].initialize(Renderer::DescriptorRangeType::SRV, 1, 3, "SpecularMap", 1);

				Renderer::RootParameterBuilder rootParameters[6];
				rootParameters[0].initializeAsDescriptorTable(1, &ranges[0], Renderer::ShaderVisibility::VERTEX);
				rootParameters[1].initializeAsDescriptorTable(1, &ranges[1], Renderer::ShaderVisibility::FRAGMENT);
				rootParameters[2].initializeAsDescriptorTable(1, &ranges[2], Renderer::ShaderVisibility::FRAGMENT);
				rootParameters[3].initializeAsDescriptorTable(1, &ranges[3], Renderer::ShaderVisibility::FRAGMENT);
				rootParameters[4].initializeAsDescriptorTable(1, &ranges[4], Renderer::ShaderVisibility::FRAGMENT);
				rootParameters[5].initializeAsDescriptorTable(1, &ranges[5], Renderer::ShaderVisibility::FRAGMENT);

				// Setup
				Renderer::RootSignatureBuilder rootSignature;
				rootSignature.initialize(static_cast<uint32_t>(glm::countof(rootParameters)), rootParameters, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

				// Create the instance
				mRootSignature = renderer->createRootSignature(rootSignature);
			}

			// Create the program
			Renderer::IProgramPtr program;
			{
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderProfile = nullptr;
				const char *vertexShaderSourceCode = nullptr;
				const char *fragmentShaderProfile = nullptr;
				const char *fragmentShaderSourceCode = nullptr;
				#include "FirstMesh_GLSL_410.h"
				#include "FirstMesh_GLSL_ES3.h"
				#include "FirstMesh_HLSL_D3D9.h"
				#include "FirstMesh_HLSL_D3D10_D3D11_D3D12.h"
				#include "FirstMesh_Null.h"

				// Create the program
				mProgram = program = shaderLanguage->createProgram(
					*mRootSignature,
					vertexAttributes,
					shaderLanguage->createVertexShaderFromSourceCode(vertexAttributes, vertexShaderSourceCode, vertexShaderProfile),
					shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode, fragmentShaderProfile));
			}

			// Is there a valid program?
			if (nullptr != program)
			{
				// Create the pipeline state object (PSO)
				mPipelineState = renderer->createPipelineState(Renderer::PipelineStateBuilder(mRootSignature, program, vertexAttributes));

				// Optimization: Cached data to not bother the renderer API too much
				if (nullptr == mUniformBuffer)
				{
					mObjectSpaceToClipSpaceMatrixUniformHandle = program->getUniformHandle("ObjectSpaceToClipSpaceMatrix");
					mObjectSpaceToViewSpaceMatrixUniformHandle = program->getUniformHandle("ObjectSpaceToViewSpaceMatrix");
				}
			}

			// Create mesh instance
			mMeshResourceId = rendererRuntime->getMeshResourceManager().loadMeshResourceByAssetId("Example/Mesh/Character/Imrod");

			{ // Load in the diffuse, emissive, normal and specular texture
			  // -> The tangent space normal map is stored with three components, two would be enough to recalculate the third component within the fragment shader
			  // -> The specular map could be put into the alpha channel of the diffuse map instead of storing it as an individual texture
				RendererRuntime::TextureResourceManager& textureResourceManager = rendererRuntime->getTextureResourceManager();
				mDiffuseTextureResourceId  = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/ImrodDiffuseMap",  "Unrimp/Texture/DynamicByCode/IdentityDiffuseMap2D");
				mNormalTextureResourceId   = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/ImrodEmissiveMap", "Unrimp/Texture/DynamicByCode/IdentityEmissiveMap2D");
				mSpecularTextureResourceId = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/ImrodNormalMap",   "Unrimp/Texture/DynamicByCode/IdentityNormalMap2D");
				mEmissiveTextureResourceId = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/ImrodSpecularMap", "Unrimp/Texture/DynamicByCode/IdentitySpecularMap2D");
			}

			{ // Create sampler state
				Renderer::SamplerState samplerStateSettings = Renderer::ISamplerState::getDefaultSamplerState();
				samplerStateSettings.addressU = Renderer::TextureAddressMode::WRAP;
				samplerStateSettings.addressV = Renderer::TextureAddressMode::WRAP;
				mSamplerState = renderer->createSamplerState(samplerStateSettings);
			}
		}
	}
}

void FirstMesh::onDeinitialization()
{
	// Release the used renderer resources
	mCommandBuffer.clear();
	mSamplerState = nullptr;
	RendererRuntime::setUninitialized(mDiffuseTextureResourceId);
	RendererRuntime::setUninitialized(mNormalTextureResourceId);
	RendererRuntime::setUninitialized(mSpecularTextureResourceId);
	RendererRuntime::setUninitialized(mEmissiveTextureResourceId);
	RendererRuntime::setUninitialized(mMeshResourceId);
	mPipelineState = nullptr;
	mProgram	   = nullptr;
	mRootSignature = nullptr;
	mUniformBuffer = nullptr;

	// Call the base implementation
	ExampleBase::onDeinitialization();
}

void FirstMesh::onUpdate()
{
	// Call the base implementation
	ExampleBase::onUpdate();

	// Stop the stopwatch
	mStopwatch.stop();

	// Update the global timer (FPS independent movement)
	mGlobalTimer += mStopwatch.getMilliseconds() * 0.0005f;

	// Start the stopwatch
	mStopwatch.start();
}

void FirstMesh::onDraw()
{
	RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
	if (nullptr == rendererRuntime)
	{
		return;
	}

	// Due to background texture loading, some textures might not be ready, yet
	const RendererRuntime::TextureResources& textureResources = rendererRuntime->getTextureResourceManager().getTextureResources();
	const RendererRuntime::TextureResource* diffuseTextureResource  = textureResources.tryGetElementById(mDiffuseTextureResourceId);
	const RendererRuntime::TextureResource* normalTextureResource   = textureResources.tryGetElementById(mNormalTextureResourceId);
	const RendererRuntime::TextureResource* specularTextureResource = textureResources.tryGetElementById(mSpecularTextureResourceId);
	const RendererRuntime::TextureResource* emissiveTextureResource = textureResources.tryGetElementById(mEmissiveTextureResourceId);
	if (nullptr == diffuseTextureResource || nullptr == diffuseTextureResource->getTexture() ||
		nullptr == normalTextureResource || nullptr == normalTextureResource->getTexture() ||
		nullptr == specularTextureResource || nullptr == specularTextureResource->getTexture() ||
		nullptr == emissiveTextureResource || nullptr == emissiveTextureResource->getTexture())
	{
		return;
	}

	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && nullptr != mPipelineState)
	{
		// Begin debug event
		COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(mCommandBuffer)

		// Set the viewport and get the aspect ratio
		float aspectRatio = 4.0f / 3.0f;
		{
			// Get the render target with and height
			uint32_t width  = 1;
			uint32_t height = 1;
			Renderer::IRenderTarget *renderTarget = getMainRenderTarget();
			if (nullptr != renderTarget)
			{
				renderTarget->getWidthAndHeight(width, height);

				// Get the aspect ratio
				aspectRatio = static_cast<float>(width) / height;
			}
		}

		// Clear the color buffer of the current render target with gray, do also clear the depth buffer
		Renderer::Command::Clear::create(mCommandBuffer, Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

		// Set the used graphics root signature
		Renderer::Command::SetGraphicsRootSignature::create(mCommandBuffer, mRootSignature);

		// Set sampler and textures
		Renderer::Command::SetGraphicsRootDescriptorTable::create(mCommandBuffer, 0, mUniformBuffer);
		Renderer::Command::SetGraphicsRootDescriptorTable::create(mCommandBuffer, 1, mSamplerState);
		Renderer::Command::SetGraphicsRootDescriptorTable::create(mCommandBuffer, 2, diffuseTextureResource->getTexture());
		Renderer::Command::SetGraphicsRootDescriptorTable::create(mCommandBuffer, 3, normalTextureResource->getTexture());
		Renderer::Command::SetGraphicsRootDescriptorTable::create(mCommandBuffer, 4, specularTextureResource->getTexture());
		Renderer::Command::SetGraphicsRootDescriptorTable::create(mCommandBuffer, 5, emissiveTextureResource->getTexture());

		// Set the used pipeline state object (PSO)
		Renderer::Command::SetPipelineState::create(mCommandBuffer, mPipelineState);

		{ // Set uniform
			// Calculate the object space to clip space matrix
			const glm::mat4 viewSpaceToClipSpace	= glm::perspective(45.0f, aspectRatio, 0.1f, 100.f);
			const glm::mat4 viewTranslate			= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -7.0f, -25.0f));
			const glm::mat4 worldSpaceToViewSpace	= glm::rotate(viewTranslate, mGlobalTimer, glm::vec3(0.0f, 1.0f, 0.0f));
			const glm::mat4 objectSpaceToWorldSpace	= glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
				  glm::mat4 objectSpaceToViewSpace	= worldSpaceToViewSpace * objectSpaceToWorldSpace;
			const glm::mat4 objectSpaceToClipSpace	= viewSpaceToClipSpace * objectSpaceToViewSpace;

			// Upload the uniform data
			// -> Two versions: One using an uniform buffer and one setting an individual uniform
			if (nullptr != mUniformBuffer)
			{
				struct UniformBlockDynamicVs
				{
					float objectSpaceToClipSpaceMatrix[4 * 4];	// Object space to clip space matrix
					float objectSpaceToViewSpaceMatrix[4 * 4];	// Object space to view space matrix
				};
				UniformBlockDynamicVs uniformBlockDynamicVS;
				memcpy(uniformBlockDynamicVS.objectSpaceToClipSpaceMatrix, glm::value_ptr(objectSpaceToClipSpace), sizeof(float) * 4 * 4);

				// TODO(co) float3x3 (currently there are alignment issues when using Direct3D, have a look into possible solutions)
				glm::mat3 objectSpaceToViewSpace3x3 = glm::mat3(objectSpaceToViewSpace);
				objectSpaceToViewSpace = glm::mat4(objectSpaceToViewSpace3x3);
				memcpy(uniformBlockDynamicVS.objectSpaceToViewSpaceMatrix, glm::value_ptr(objectSpaceToViewSpace), sizeof(float) * 4 * 4);

				// Copy data
				Renderer::Command::CopyUniformBufferData::create(mCommandBuffer, mUniformBuffer, sizeof(UniformBlockDynamicVs), &uniformBlockDynamicVS);
			}
			else
			{
				// TODO(co) Not compatible with command buffer: This certainly is going to be removed, we need to implement internal uniform buffer emulation
				// Set uniforms
				mProgram->setUniformMatrix4fv(mObjectSpaceToClipSpaceMatrixUniformHandle, glm::value_ptr(objectSpaceToClipSpace));
				mProgram->setUniformMatrix3fv(mObjectSpaceToViewSpaceMatrixUniformHandle, glm::value_ptr(glm::mat3(objectSpaceToViewSpace)));
			}
		}

		{ // Draw mesh instance
			const RendererRuntime::MeshResource* meshResource = rendererRuntime->getMeshResourceManager().getMeshResources().tryGetElementById(mMeshResourceId);
			if (nullptr != meshResource)
			{
				{ // Setup input assembly (IA)
					// Set the used vertex array
					Renderer::Command::SetVertexArray::create(mCommandBuffer, meshResource->getVertexArrayPtr());

					// Set the primitive topology used for draw calls
					Renderer::Command::SetPrimitiveTopology::create(mCommandBuffer, Renderer::PrimitiveTopology::TRIANGLE_LIST);
				}

				// Render the specified geometric primitive, based on indexing into an array of vertices
				Renderer::Command::DrawIndexed::create(mCommandBuffer, meshResource->getNumberOfIndices());
			}
		}

		// End debug event
		COMMAND_END_DEBUG_EVENT(mCommandBuffer)

		// Submit command buffer to the renderer backend
		mCommandBuffer.submitAndClear(*renderer);
	}
}
