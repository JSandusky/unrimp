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

#include <RendererRuntime/IRendererRuntime.h>
#include <RendererRuntime/Resource/Mesh/MeshResource.h>
#include <RendererRuntime/Resource/Mesh/MeshResourceManager.h>
#include <RendererRuntime/Resource/Texture/TextureResource.h>
#include <RendererRuntime/Resource/Texture/TextureResourceManager.h>
#include <RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstMesh::FirstMesh() :
	mMeshResourceId(RendererRuntime::getUninitialized<RendererRuntime::MeshResourceId>()),
	m_drgb_nxaTextureResourceId(RendererRuntime::getUninitialized<RendererRuntime::TextureResourceId>()),
	m_hr_rg_mb_nyaTextureResourceId(RendererRuntime::getUninitialized<RendererRuntime::TextureResourceId>()),
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

		// Don't create initial pipeline state caches after a material blueprint has been loaded since this example isn't using the material blueprint system
		rendererRuntime->getMaterialBlueprintResourceManager().setCreateInitialPipelineStateCaches(false);

		// Decide which shader language should be used (for example "GLSL" or "HLSL")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer->getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			{ // Create the root signature
				Renderer::DescriptorRangeBuilder ranges[5];
				ranges[0].initialize(Renderer::DescriptorRangeType::UBV, 1, 0, "UniformBlockDynamicVs", Renderer::ShaderVisibility::VERTEX);
				ranges[1].initialize(Renderer::DescriptorRangeType::SRV, 1, 0, "_drgb_nxa", Renderer::ShaderVisibility::FRAGMENT);
				ranges[2].initialize(Renderer::DescriptorRangeType::SRV, 1, 1, "_hr_rg_mb_nya", Renderer::ShaderVisibility::FRAGMENT);
				ranges[3].initialize(Renderer::DescriptorRangeType::SRV, 1, 2, "EmissiveMap", Renderer::ShaderVisibility::FRAGMENT);
				ranges[4].initializeSampler(1, 0, Renderer::ShaderVisibility::FRAGMENT);

				Renderer::RootParameterBuilder rootParameters[2];
				rootParameters[0].initializeAsDescriptorTable(4, &ranges[0]);
				rootParameters[1].initializeAsDescriptorTable(1, &ranges[4]);

				// Setup
				Renderer::RootSignatureBuilder rootSignature;
				rootSignature.initialize(static_cast<uint32_t>(glm::countof(rootParameters)), rootParameters, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

				// Create the instance
				mRootSignature = renderer->createRootSignature(rootSignature);
			}

			// Create uniform buffer
			// -> Direct3D 9 does not support uniform buffers
			// -> Direct3D 10, 11 and 12 do not support individual uniforms
			// -> The renderer is just a light weight abstraction layer, so we need to handle the differences
			// -> Allocate enough memory for two 4x4 floating point matrices
			mUniformBuffer = rendererRuntime->getBufferManager().createUniformBuffer(2 * 4 * 4 * sizeof(float), nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);

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
					sizeof(float) * 5 + sizeof(short) * 4,		// strideInBytes (uint32_t)
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
					sizeof(float) * 5 + sizeof(short) * 4,		// strideInBytes (uint32_t)
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
					sizeof(float) * 5,							// alignedByteOffset (uint32_t)
					sizeof(float) * 5 + sizeof(short) * 4,		// strideInBytes (uint32_t)
					0											// instancesPerElement (uint32_t)
				}
			};
			const Renderer::VertexAttributes vertexAttributes(static_cast<uint32_t>(glm::countof(vertexAttributesLayout)), vertexAttributesLayout);

			{ // Create sampler state and wrap it into a resource group instance
				Renderer::SamplerState samplerStateSettings = Renderer::ISamplerState::getDefaultSamplerState();
				samplerStateSettings.addressU = Renderer::TextureAddressMode::WRAP;
				samplerStateSettings.addressV = Renderer::TextureAddressMode::WRAP;
				Renderer::IResource* samplerStateResource = mSamplerStatePtr = renderer->createSamplerState(samplerStateSettings);
				mSamplerStateGroup = mRootSignature->createResourceGroup(1, 1, &samplerStateResource);
			}

			// Create the program
			Renderer::IProgramPtr program;
			{
				// Get the shader source code (outsourced to keep an overview)
				const char* vertexShaderSourceCode = nullptr;
				const char* fragmentShaderSourceCode = nullptr;
				#include "FirstMesh_GLSL_450.h"	// For Vulkan
				#include "FirstMesh_GLSL_410.h"	// macOS 10.11 only supports OpenGL 4.1 hence it's our OpenGL minimum
				#include "FirstMesh_GLSL_ES3.h"
				#include "FirstMesh_HLSL_D3D9.h"
				#include "FirstMesh_HLSL_D3D10_D3D11_D3D12.h"
				#include "FirstMesh_Null.h"

				// Create the program
				mProgram = program = shaderLanguage->createProgram(
					*mRootSignature,
					vertexAttributes,
					shaderLanguage->createVertexShaderFromSourceCode(vertexAttributes, vertexShaderSourceCode),
					shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode));
			}

			// Is there a valid program?
			if (nullptr != program)
			{
				// Create the pipeline state object (PSO)
				mPipelineState = renderer->createPipelineState(Renderer::PipelineStateBuilder(mRootSignature, program, vertexAttributes, getMainRenderTarget()->getRenderPass()));

				// Optimization: Cached data to not bother the renderer API too much
				if (nullptr == mUniformBuffer)
				{
					mObjectSpaceToClipSpaceMatrixUniformHandle = program->getUniformHandle("ObjectSpaceToClipSpaceMatrix");
					mObjectSpaceToViewSpaceMatrixUniformHandle = program->getUniformHandle("ObjectSpaceToViewSpaceMatrix");
				}
			}

			// Create mesh instance
			rendererRuntime->getMeshResourceManager().loadMeshResourceByAssetId("Example/Mesh/Character/Imrod", mMeshResourceId);

			{ // Load in the diffuse, emissive, normal and roughness texture
				RendererRuntime::TextureResourceManager& textureResourceManager = rendererRuntime->getTextureResourceManager();
				textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_drgb_nxa",     "Unrimp/Texture/DynamicByCode/Identity_drgb_nxa2D",     m_drgb_nxaTextureResourceId, this);
				textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_hr_rg_mb_nya", "Unrimp/Texture/DynamicByCode/Identity_hr_rg_mb_nya2D", m_hr_rg_mb_nyaTextureResourceId, this);
				textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_e",            "Unrimp/Texture/DynamicByCode/IdentityEmissiveMap2D",   mEmissiveTextureResourceId, this);
			}
		}
	}
}

void FirstMesh::onDeinitialization()
{
	// Release the used renderer resources
	mObjectSpaceToViewSpaceMatrixUniformHandle = NULL_HANDLE;
	mObjectSpaceToClipSpaceMatrixUniformHandle = NULL_HANDLE;
	mSamplerStateGroup = nullptr;
	mSamplerStatePtr = nullptr;
	mResourceGroup = nullptr;
	RendererRuntime::setUninitialized(mEmissiveTextureResourceId);
	RendererRuntime::setUninitialized(m_hr_rg_mb_nyaTextureResourceId);
	RendererRuntime::setUninitialized(m_drgb_nxaTextureResourceId);
	RendererRuntime::setUninitialized(mMeshResourceId);
	mProgram	   = nullptr;
	mPipelineState = nullptr;
	mUniformBuffer = nullptr;
	mRootSignature = nullptr;
	mCommandBuffer.clear();

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
	const RendererRuntime::TextureResourceManager& textureResourceManager = rendererRuntime->getTextureResourceManager();
	const RendererRuntime::TextureResource* _drgb_nxaTextureResource = textureResourceManager.tryGetById(m_drgb_nxaTextureResourceId);
	const RendererRuntime::TextureResource* _hr_rg_mb_nyaTextureResource = textureResourceManager.tryGetById(m_hr_rg_mb_nyaTextureResourceId);
	const RendererRuntime::TextureResource* emissiveTextureResource = textureResourceManager.tryGetById(mEmissiveTextureResourceId);
	if (nullptr == _drgb_nxaTextureResource || nullptr == _drgb_nxaTextureResource->getTexture() ||
		nullptr == _hr_rg_mb_nyaTextureResource || nullptr == _hr_rg_mb_nyaTextureResource->getTexture() ||
		nullptr == emissiveTextureResource || nullptr == emissiveTextureResource->getTexture())
	{
		return;
	}
	if (nullptr == mResourceGroup)
	{
		// Create resource group
		Renderer::IResource* resources[4] = { mUniformBuffer, _drgb_nxaTextureResource->getTexture(), _hr_rg_mb_nyaTextureResource->getTexture(), emissiveTextureResource->getTexture() };
		Renderer::ISamplerState* samplerStates[4] = { nullptr, mSamplerStatePtr, mSamplerStatePtr, mSamplerStatePtr };
		mResourceGroup = mRootSignature->createResourceGroup(0, static_cast<uint32_t>(glm::countof(resources)), resources, samplerStates);
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
			const Renderer::IRenderTarget* renderTarget = getMainRenderTarget();
			if (nullptr != renderTarget)
			{
				uint32_t width  = 1;
				uint32_t height = 1;
				renderTarget->getWidthAndHeight(width, height);

				// Get the aspect ratio
				aspectRatio = static_cast<float>(width) / height;
			}
		}

		// Clear the color buffer of the current render target with gray, do also clear the depth buffer
		Renderer::Command::Clear::create(mCommandBuffer, Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

		// Set the used graphics root signature
		Renderer::Command::SetGraphicsRootSignature::create(mCommandBuffer, mRootSignature);

		// Set the used pipeline state object (PSO)
		Renderer::Command::SetPipelineState::create(mCommandBuffer, mPipelineState);

		// Set resource groups
		Renderer::Command::SetGraphicsResourceGroup::create(mCommandBuffer, 0, mResourceGroup);
		Renderer::Command::SetGraphicsResourceGroup::create(mCommandBuffer, 1, mSamplerStateGroup);

		{ // Set uniform
			// Calculate the object space to clip space matrix
			const glm::mat4 viewSpaceToClipSpace	= glm::perspective(45.0f, aspectRatio, 0.1f, 100.f);
			const glm::mat4 viewTranslate			= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -7.0f, 25.0f));
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
			const RendererRuntime::MeshResource* meshResource = rendererRuntime->getMeshResourceManager().tryGetById(mMeshResourceId);
			if (nullptr != meshResource && nullptr != meshResource->getVertexArrayPtr())
			{
				// Input assembly (IA): Set the used vertex array
				Renderer::Command::SetVertexArray::create(mCommandBuffer, meshResource->getVertexArrayPtr());

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


//[-------------------------------------------------------]
//[ Protected virtual RendererRuntime::IResourceListener methods ]
//[-------------------------------------------------------]
void FirstMesh::onLoadingStateChange(const RendererRuntime::IResource&)
{
	// Forget about the resource group so it's rebuild
	mResourceGroup = nullptr;
}
