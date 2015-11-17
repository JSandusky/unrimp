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
#include "RendererRuntime/Resource/Compositor/Pass/Scene/CompositorInstancePassScene.h"
#include "RendererRuntime/Resource/Compositor/Pass/Scene/CompositorResourcePassScene.h"
#include "RendererRuntime/Resource/Compositor/CompositorInstanceNode.h"
#include "RendererRuntime/Resource/Compositor/CompositorInstance.h"
#include "RendererRuntime/Resource/Scene/SceneNode.h"
#include "RendererRuntime/Resource/Scene/SceneResource.h"
#include "RendererRuntime/Resource/Scene/SceneItemMesh.h"
#include "RendererRuntime/Resource/Scene/SceneItemCamera.h"
#include "RendererRuntime/Resource/Mesh/MeshResource.h"
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/Resource/Texture/TextureResource.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <Renderer/Public/Renderer.h>

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4201)	// warning C4201: nonstandard extension used: nameless struct/union
	#include <glm/gtc/type_ptr.hpp>
	#include <glm/gtc/matrix_transform.hpp>
	#include <glm/gtx/quaternion.hpp>
#pragma warning(pop)


//[-------------------------------------------------------]
//[ Global functions in anonymous namespace               ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{
		Renderer::IUniformBufferPtr			  mUniformBuffer;			///< Uniform buffer object (UBO), can be a null pointer
		Renderer::IRootSignaturePtr			  mRootSignature;			///< Root signature, can be a null pointer
		Renderer::IPipelineStatePtr			  mPipelineState;			///< Pipeline state object (PSO), can be a null pointer
		Renderer::IProgramPtr				  mProgram;					///< Program, can be a null pointer
		RendererRuntime::TextureResource*	  mDiffuseTextureResource;
		RendererRuntime::TextureResource*	  mNormalTextureResource;
		RendererRuntime::TextureResource*	  mSpecularTextureResource;
		RendererRuntime::TextureResource*	  mEmissiveTextureResource;
		Renderer::ISamplerStatePtr			  mSamplerState;			///< Sampler state, can be a null pointer
		// Optimization: Cache data to not bother the renderer API to much
		Renderer::handle	 mObjectSpaceToClipSpaceMatrixUniformHandle;	///< Object space to clip space matrix uniform handle, can be NULL_HANDLE
		Renderer::handle	 mObjectSpaceToViewSpaceMatrixUniformHandle;	///< Object space to view space matrix uniform handle, can be NULL_HANDLE

		void initialize(const RendererRuntime::IRendererRuntime& rendererRuntime)
		{
			Renderer::IRenderer& renderer = rendererRuntime.getRenderer();

			// Decide which shader language should be used (for example "GLSL" or "HLSL")
			Renderer::IShaderLanguagePtr shaderLanguage(renderer.getShaderLanguage());
			if (nullptr != shaderLanguage)
			{
				// Create uniform buffers
				// -> Direct3D 9 and OpenGL ES 2 do not support uniform buffers
				// -> Direct3D 10, 11 and 12 do not support individual uniforms
				// -> The renderer is just a light weight abstraction layer, so we need to handle the differences
				if ((0 == strcmp(renderer.getName(), "Direct3D10") || 0 == strcmp(renderer.getName(), "Direct3D11") || 0 == strcmp(renderer.getName(), "Direct3D12")))
				{
					// Allocate enough memory for two 4x4 floating point matrices
					mUniformBuffer = shaderLanguage->createUniformBuffer(2 * 4 * 4 * sizeof(float), nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
				}

				// TODO(co) We need a central vertex input layout management
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
					},
					{ // Attribute 1
						// Data destination
						Renderer::VertexAttributeFormat::SHORT_2,	// vertexAttributeFormat (Renderer::VertexAttributeFormat::Enum)
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
						Renderer::VertexAttributeFormat::SHORT_4,	// vertexAttributeFormat (Renderer::VertexAttributeFormat::Enum)
						"QTangent",									// name[32] (char)
						"NORMAL",									// semanticName[32] (char)
						0,											// semanticIndex (uint32_t)
						// Data source
						0,											// inputSlot (uint32_t)
						sizeof(float) * 3 + sizeof(short) * 2,		// alignedByteOffset (uint32_t)
						// Data source, instancing part
						0											// instancesPerElement (uint32_t)
					}
				};
				const Renderer::VertexAttributes vertexAttributes(sizeof(vertexAttributesLayout) / sizeof(Renderer::VertexAttribute), vertexAttributesLayout);

				{ // Create the root signature
					Renderer::DescriptorRangeBuilder ranges[6];
					ranges[0].initialize(Renderer::DescriptorRangeType::CBV, 1, 0, "UniformBlockDynamicVs", 0);
					ranges[1].initializeSampler(1, 0);
					ranges[2].initialize(Renderer::DescriptorRangeType::SRV, 1, 0, "DiffuseMap", 0);
					ranges[3].initialize(Renderer::DescriptorRangeType::SRV, 1, 1, "EmissiveMap", 0);
					ranges[4].initialize(Renderer::DescriptorRangeType::SRV, 1, 2, "NormalMap", 0);
					ranges[5].initialize(Renderer::DescriptorRangeType::SRV, 1, 3, "SpecularMap", 0);

					Renderer::RootParameterBuilder rootParameters[6];
					rootParameters[0].initializeAsDescriptorTable(1, &ranges[0], Renderer::ShaderVisibility::VERTEX);
					rootParameters[1].initializeAsDescriptorTable(1, &ranges[1], Renderer::ShaderVisibility::FRAGMENT);
					rootParameters[2].initializeAsDescriptorTable(1, &ranges[2], Renderer::ShaderVisibility::FRAGMENT);
					rootParameters[3].initializeAsDescriptorTable(1, &ranges[3], Renderer::ShaderVisibility::FRAGMENT);
					rootParameters[4].initializeAsDescriptorTable(1, &ranges[4], Renderer::ShaderVisibility::FRAGMENT);
					rootParameters[5].initializeAsDescriptorTable(1, &ranges[5], Renderer::ShaderVisibility::FRAGMENT);

					// Setup
					Renderer::RootSignatureBuilder rootSignature;
					rootSignature.initialize(sizeof(rootParameters) / sizeof(Renderer::RootParameter), rootParameters, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

					// Create the instance
					mRootSignature = renderer.createRootSignature(rootSignature);
				}

				// Create the program
				Renderer::IProgramPtr program;
				{
					// Get the shader source code (outsourced to keep an overview)
					const char *vertexShaderProfile = nullptr;
					const char *vertexShaderSourceCode = nullptr;
					const char *fragmentShaderProfile = nullptr;
					const char *fragmentShaderSourceCode = nullptr;
					#include "FirstScene_GLSL_110.h"
					#include "FirstScene_GLSL_ES2.h"
					#include "FirstScene_HLSL_D3D9.h"
					#include "FirstScene_HLSL_D3D10_D3D11_D3D12.h"
					#include "FirstScene_Null.h"

					// Create the program
					mProgram = program = shaderLanguage->createProgram(
						*mRootSignature,
						vertexAttributes,
						shaderLanguage->createVertexShaderFromSourceCode(vertexShaderSourceCode, vertexShaderProfile),
						shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode, fragmentShaderProfile));
				}

				// Is there a valid program?
				if (nullptr != program)
				{
					// Create the pipeline state object (PSO)
					mPipelineState = renderer.createPipelineState(Renderer::PipelineStateBuilder(mRootSignature, program, vertexAttributes));

					// Optimization: Cached data to not bother the renderer API too much
					if (nullptr == mUniformBuffer)
					{
						mObjectSpaceToClipSpaceMatrixUniformHandle = program->getUniformHandle("ObjectSpaceToClipSpaceMatrix");
						mObjectSpaceToViewSpaceMatrixUniformHandle = program->getUniformHandle("ObjectSpaceToViewSpaceMatrix");
					}
				}

				{ // Load in the diffuse, emissive, normal and specular texture
				  // -> The tangent space normal map is stored with three components, two would be enough to recalculate the third component within the fragment shader
				  // -> The specular map could be put into the alpha channel of the diffuse map instead of storing it as an individual texture
					RendererRuntime::TextureResourceManager& textureResourceManager = rendererRuntime.getTextureResourceManager();
					mDiffuseTextureResource  = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_Diffuse");
					mNormalTextureResource   = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_Illumination");
					mSpecularTextureResource = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_norm");
					mEmissiveTextureResource = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_spec");
				}

				{ // Create sampler state
					Renderer::SamplerState samplerStateSettings = Renderer::ISamplerState::getDefaultSamplerState();
					samplerStateSettings.addressU = Renderer::TextureAddressMode::WRAP;
					samplerStateSettings.addressV = Renderer::TextureAddressMode::WRAP;
					mSamplerState = renderer.createSamplerState(samplerStateSettings);
				}
			}
		}

		void deinitialize()
		{
			// Release the used renderer resources
			mSamplerState = nullptr;

			mDiffuseTextureResource = nullptr;
			mNormalTextureResource = nullptr;
			mSpecularTextureResource = nullptr;
			mEmissiveTextureResource = nullptr;

			// Release the used resources
			mPipelineState = nullptr;
			mProgram = nullptr;
			mRootSignature = nullptr;
			mUniformBuffer = nullptr;
		}

		void draw(Renderer::IRenderer& renderer, RendererRuntime::SceneItemCamera& sceneItemCamera)
		{
			// Due to background texture loading, some textures might not be ready, yet
			// TODO(co) Add dummy textures so rendering also works when textures are not ready, yet
			if (nullptr == mDiffuseTextureResource->getTexture() || nullptr == mNormalTextureResource->getTexture() ||
				nullptr == mSpecularTextureResource->getTexture() || nullptr == mEmissiveTextureResource->getTexture())
			{
				return;
			}

			// Get and check the renderer instance
			if (nullptr != mPipelineState)
			{
				// Begin debug event
				RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

				// Set the viewport and get the aspect ratio
				float aspectRatio = 4.0f / 3.0f;
				{
					// Get the render target with and height
					uint32_t width  = 1;
					uint32_t height = 1;
					Renderer::IRenderTarget *renderTarget = renderer.omGetRenderTarget();
					if (nullptr != renderTarget)
					{
						renderTarget->getWidthAndHeight(width, height);

						// Get the aspect ratio
						aspectRatio = static_cast<float>(width) / height;
					}
				}

				// Set the used graphics root signature
				renderer.setGraphicsRootSignature(mRootSignature);

				// Set sampler and textures
				renderer.setGraphicsRootDescriptorTable(0, mUniformBuffer);
				renderer.setGraphicsRootDescriptorTable(1, mSamplerState);
				renderer.setGraphicsRootDescriptorTable(2, mDiffuseTextureResource->getTexture());
				renderer.setGraphicsRootDescriptorTable(3, mNormalTextureResource->getTexture());
				renderer.setGraphicsRootDescriptorTable(4, mSpecularTextureResource->getTexture());
				renderer.setGraphicsRootDescriptorTable(5, mEmissiveTextureResource->getTexture());

				// Set the used pipeline state object (PSO)
				renderer.setPipelineState(mPipelineState);

				// Loop through all scene nodes
				const RendererRuntime::SceneResource::SceneNodes& sceneNodes = sceneItemCamera.getSceneResource().getSceneNodes();
				const size_t numberOfSceneNodes = sceneNodes.size();
				for (size_t sceneNodeIndex = 0; sceneNodeIndex < numberOfSceneNodes; ++sceneNodeIndex)
				{
					RendererRuntime::SceneNode* sceneNode = sceneNodes[sceneNodeIndex];
					const RendererRuntime::Transform& transform = sceneNode->getTransform();

					// Loop through all scene items attached to the current scene node
					const RendererRuntime::SceneNode::AttachedSceneItems& attachedSceneItems = sceneNode->getAttachedSceneItems();
					const size_t numberOfAttachedSceneItems = attachedSceneItems.size();
					for (size_t attachedSceneItemIndex = 0; attachedSceneItemIndex < numberOfAttachedSceneItems; ++attachedSceneItemIndex)
					{
						RendererRuntime::ISceneItem* sceneItem = attachedSceneItems[attachedSceneItemIndex];

						// TODO(co) As mentioned, just a first test
						RendererRuntime::SceneItemMesh* sceneItemMesh = dynamic_cast<RendererRuntime::SceneItemMesh*>(sceneItem);
						if (nullptr != sceneItemMesh)
						{
							{ // Set uniform
								// Calculate the object space to clip space matrix
								glm::mat4 viewSpaceToClipSpace		= glm::perspective(45.0f, aspectRatio, 0.1f, 100.f);
								glm::mat4 viewTranslate				= glm::translate(glm::mat4(1.0f), transform.position);
								glm::mat4 worldSpaceToViewSpace		= viewTranslate * glm::toMat4(transform.rotation);
								glm::mat4 objectSpaceToWorldSpace	= glm::scale(glm::mat4(1.0f), transform.scale);
								glm::mat4 objectSpaceToViewSpace	= worldSpaceToViewSpace * objectSpaceToWorldSpace;
								glm::mat4 objectSpaceToClipSpace	= viewSpaceToClipSpace * objectSpaceToViewSpace;

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
									mUniformBuffer->copyDataFrom(sizeof(UniformBlockDynamicVs), &uniformBlockDynamicVS);
								}
								else
								{
									// Set uniforms
									mProgram->setUniformMatrix4fv(mObjectSpaceToClipSpaceMatrixUniformHandle, glm::value_ptr(objectSpaceToClipSpace));
									mProgram->setUniformMatrix3fv(mObjectSpaceToViewSpaceMatrixUniformHandle, glm::value_ptr(glm::mat3(objectSpaceToViewSpace)));
								}
							}

							// Draw mesh instance
							RendererRuntime::MeshResource* meshResource = sceneItemMesh->getMeshResource();
							if (nullptr != meshResource)
							{
								meshResource->draw();
							}
						}
					}
				}

				// End debug event
				RENDERER_END_DEBUG_EVENT(&renderer)
			}
		}
	}
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::ICompositorInstancePass methods ]
	//[-------------------------------------------------------]
	void CompositorInstancePassScene::execute(SceneItemCamera* sceneItemCamera)
	{
		// TODO(co) Just a first test
		if (nullptr != sceneItemCamera)
		{
			::detail::draw(getCompositorInstanceNode().getCompositorInstance().getRendererRuntime().getRenderer(), *sceneItemCamera);
		}
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CompositorInstancePassScene::CompositorInstancePassScene(const CompositorResourcePassScene& compositorResourcePassScene, const CompositorInstanceNode& compositorInstanceNode) :
		ICompositorInstancePass(compositorResourcePassScene, compositorInstanceNode)
	{
		// TODO(co) Just a first test
		::detail::initialize(getCompositorInstanceNode().getCompositorInstance().getRendererRuntime());
	}

	CompositorInstancePassScene::~CompositorInstancePassScene()
	{
		// TODO(co) Just a first test
		::detail::deinitialize();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
