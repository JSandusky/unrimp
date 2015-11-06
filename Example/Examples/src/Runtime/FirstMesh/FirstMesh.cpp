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
#include "Runtime/FirstMesh/FirstMesh.h"
#include "Framework/Color4.h"

#include <RendererToolkit/Public/RendererToolkit.h>

#include <RendererRuntime/Asset/AssetManager.h>
#include <RendererRuntime/Resource/Mesh/MeshResource.h>
#include <RendererRuntime/Resource/Mesh/MeshResourceManager.h>
#include <RendererRuntime/Resource/Font/FontResourceManager.h>
#include <RendererRuntime/Resource/Texture/TextureResource.h>
#include <RendererRuntime/Resource/Texture/TextureResourceManager.h>

#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/matrix_transform.hpp>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstMesh::FirstMesh(const char *rendererName) :
	IApplicationRendererRuntime(rendererName),
	mFontResource(nullptr),
	mMeshResource(nullptr),
	mDiffuseTextureResource(nullptr),
	mNormalTextureResource(nullptr),
	mSpecularTextureResource(nullptr),
	mEmissiveTextureResource(nullptr),
	mUniformBlockIndex(0),
	mObjectSpaceToClipSpaceMatrixUniformHandle(NULL_HANDLE),
	mObjectSpaceToViewSpaceMatrixUniformHandle(NULL_HANDLE),
	mGlobalTimer(0.0f),
	mProject(nullptr)
{
	// Nothing to do in here
}

FirstMesh::~FirstMesh()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstMesh::onInitialization()
{
	// Call the base implementation
	IApplicationRendererRuntime::onInitialization();

	// Get and check the renderer runtime instance
	RendererRuntime::IRendererRuntimePtr rendererRuntime(getRendererRuntime());
	if (nullptr != rendererRuntime)
	{
		Renderer::IRendererPtr renderer(getRenderer());

		// TODO(co) Under construction: Will probably become "mount asset package"
		// Add used asset package
	//	rendererRuntime->getAssetManager().addAssetPackageByFilename("../DataMobile/Content/AssetPackage.assets");
		rendererRuntime->getAssetManager().addAssetPackageByFilename("../DataPc/Content/AssetPackage.assets");

		// TODO(co) First asset hot-reloading test
		RendererToolkit::IRendererToolkit* rendererToolkit = getRendererToolkit();
		if (nullptr != rendererToolkit)
		{
			mProject = rendererToolkit->createProject();
			if (nullptr != mProject)
			{
				try
				{
					mProject->loadByFilename("../DataSource/Example.project");
				//	mProject->startupAssetMonitor(*rendererRuntime, "OpenGLES2_100");
					mProject->startupAssetMonitor(*rendererRuntime, "Direct3D11_50");
				}
				catch (const std::exception& e)
				{
					const char* text = e.what();
					text = text;
				}
			}
		}

		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Create the font resource
		mFontResource = rendererRuntime->getFontResourceManager().loadFontResourceByAssetId("Example/Font/Default/LinBiolinum_R");

		// Decide which shader language should be used (for example "GLSL" or "HLSL")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer->getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			// Create uniform buffers
			// -> Direct3D 9 and OpenGL ES 2 do not support uniform buffers
			// -> Direct3D 10 and Direct3D 11 do not support individual uniforms
			// -> The renderer is just a light weight abstraction layer, so we need to handle the differences
			if ((0 == strcmp(renderer->getName(), "Direct3D10") || 0 == strcmp(renderer->getName(), "Direct3D11")))
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
				// TODO(co) Correct root signature

				// Setup
				Renderer::RootSignatureBuilder rootSignature;
				rootSignature.initialize(0, nullptr, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

				// Create the instance
				mRootSignature = renderer->createRootSignature(rootSignature);
			}

			{ // Create the program
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderProfile = nullptr;
				const char *vertexShaderSourceCode = nullptr;
				const char *fragmentShaderProfile = nullptr;
				const char *fragmentShaderSourceCode = nullptr;
				#include "FirstMesh_GLSL_110.h"
				#include "FirstMesh_GLSL_ES2.h"
				#include "FirstMesh_HLSL_D3D9.h"
				#include "FirstMesh_HLSL_D3D10_D3D11.h"
				#include "FirstMesh_Null.h"

				// Create the program
				mProgram = shaderLanguage->createProgram(
					*mRootSignature,
					vertexAttributes,
					shaderLanguage->createVertexShaderFromSourceCode(vertexShaderSourceCode, vertexShaderProfile),
					shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode, fragmentShaderProfile));
			}

			// Is there a valid program?
			if (nullptr != mProgram)
			{
				// Tell the renderer API which texture should be bound to which texture unit
				// -> When using OpenGL or OpenGL ES 2 this is required
				// -> OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension supports explicit binding points ("layout(binding = 0)"
				//    in GLSL shader) , for backward compatibility we don't use it in here
				// -> When using Direct3D 9, Direct3D 10 or Direct3D 11, the texture unit
				//    to use is usually defined directly within the shader by using the "register"-keyword
				mProgram->setTextureUnit(mProgram->getUniformHandle("DiffuseMap"),  0);
				mProgram->setTextureUnit(mProgram->getUniformHandle("EmissiveMap"), 1);
				mProgram->setTextureUnit(mProgram->getUniformHandle("NormalMap"),   2);
				mProgram->setTextureUnit(mProgram->getUniformHandle("SpecularMap"), 3);

				// Optimization: Cached data to not bother the renderer API too much
				if (nullptr != mUniformBuffer)
				{
					mUniformBlockIndex = mProgram->getUniformBlockIndex("UniformBlockDynamicVs", 0);
				}
				else
				{
					mObjectSpaceToClipSpaceMatrixUniformHandle = mProgram->getUniformHandle("ObjectSpaceToClipSpaceMatrix");
					mObjectSpaceToViewSpaceMatrixUniformHandle = mProgram->getUniformHandle("ObjectSpaceToViewSpaceMatrix");
				}
			}

			// Create mesh instance
			mMeshResource = rendererRuntime->getMeshResourceManager().loadMeshResourceByAssetId(*renderer, "Example/Mesh/Character/ImrodLowPoly");

			{ // Load in the diffuse, emissive, normal and specular texture
			  // -> The tangent space normal map is stored with three components, two would be enough to recalculate the third component within the fragment shader
			  // -> The specular map could be put into the alpha channel of the diffuse map instead of storing it as an individual texture
				RendererRuntime::TextureResourceManager& textureResourceManager = rendererRuntime->getTextureResourceManager();
				mDiffuseTextureResource  = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_Diffuse");
				mNormalTextureResource   = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_Illumination");
				mSpecularTextureResource = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_norm");
				mEmissiveTextureResource = textureResourceManager.loadTextureResourceByAssetId("Example/Texture/Character/Imrod_spec");
			}

			// Use sampler state collections when you want you exploit renderer API methods like
			// "ID3D10Device::PSSetShaderResources()" from Direct3D 10 or "ID3D11DeviceContext::PSSetShaderResources()" from Direct3D 11.
			// By using a single API call, multiple resources can be set at one and the same time in an efficient way.
			{
				// Create sampler state
				Renderer::SamplerState samplerStateSettings = Renderer::ISamplerState::getDefaultSamplerState();
				samplerStateSettings.addressU = Renderer::TextureAddressMode::WRAP;
				samplerStateSettings.addressV = Renderer::TextureAddressMode::WRAP;
				Renderer::ISamplerState *samplerState = renderer->createSamplerState(samplerStateSettings);

				// Create the sampler state collection
				// -> The sampler state collection keeps a reference to the provided resources, so,
				//    we don't need to care about the resource cleanup in here
				Renderer::ISamplerState *samplerStates[] =
				{
					samplerState,
					samplerState,
					samplerState,
					samplerState
				};
				mSamplerStateCollection = renderer->createSamplerStateCollection(sizeof(samplerStates) / sizeof(Renderer::ISamplerState *), samplerStates);
			}
		}

		// Create blend state
		mBlendState = renderer->createBlendState(Renderer::IBlendState::getDefaultBlendState());

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstMesh::onDeinitialization()
{
	// TODO(co) First asset hot-reloading test
	if (nullptr != mProject)
	{
		delete mProject;
		mProject = nullptr;
	}

	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Release the used renderer resources
	mBlendState = nullptr;
	mSamplerStateCollection = nullptr;

	// TODO(co) Implement decent resource handling
	mFontResource = nullptr;
	mDiffuseTextureResource = nullptr;
	mNormalTextureResource = nullptr;
	mSpecularTextureResource = nullptr;
	mEmissiveTextureResource = nullptr;
	mMeshResource = nullptr;

	// Release the used resources
	mProgram = nullptr;
	mRootSignature = nullptr;
	mUniformBuffer = nullptr;

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())

	// Call the base implementation
	IApplicationRendererRuntime::onDeinitialization();
}

void FirstMesh::onUpdate()
{
	// Call the base implementation
	IApplicationRendererRuntime::onUpdate();

	// Stop the stopwatch
	mStopwatch.stop();

	// Update the global timer (FPS independent movement)
	mGlobalTimer += mStopwatch.getMilliseconds() * 0.0005f;

	// Start the stopwatch
	mStopwatch.start();
}

void FirstMesh::onDraw()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && nullptr != mProgram)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Set the viewport and get the aspect ratio
		float aspectRatio = 4.0f / 3.0f;
		{
			// Get the render target with and height
			uint32_t width  = 1;
			uint32_t height = 1;
			Renderer::IRenderTarget *renderTarget = renderer->omGetRenderTarget();
			if (nullptr != renderTarget)
			{
				renderTarget->getWidthAndHeight(width, height);

				// Get the aspect ratio
				aspectRatio = static_cast<float>(width) / height;
			}
		}

		// Begin scene rendering
		// -> Required for Direct3D 9
		// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
		if (renderer->beginScene())
		{
			// Clear the color buffer of the current render target with gray, do also clear the depth buffer
			renderer->clear(Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

			// Set the used graphics root signature
			renderer->setGraphicsRootSignature(mRootSignature);

			// Set the used program
			renderer->setProgram(mProgram);

			{ // Set uniform
				// Calculate the object space to clip space matrix
				glm::mat4 viewSpaceToClipSpace		= glm::perspective(45.0f, aspectRatio, 0.1f, 100.f);
				glm::mat4 viewTranslate				= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -7.0f, -25.0f));
				glm::mat4 worldSpaceToViewSpace		= glm::rotate(viewTranslate, mGlobalTimer, glm::vec3(0.0f, 1.0f, 0.0f));
				glm::mat4 objectSpaceToWorldSpace	= glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
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

					// Assign to stage
					renderer->vsSetUniformBuffer(mUniformBlockIndex, mUniformBuffer);
				}
				else
				{
					// Set uniforms
					mProgram->setUniformMatrix4fv(mObjectSpaceToClipSpaceMatrixUniformHandle, glm::value_ptr(objectSpaceToClipSpace));
					mProgram->setUniformMatrix3fv(mObjectSpaceToViewSpaceMatrixUniformHandle, glm::value_ptr(glm::mat3(objectSpaceToViewSpace)));
				}
			}

			// Use the texture collection to set the textures
			renderer->fsSetTexture(0, mDiffuseTextureResource->getTexture());
			renderer->fsSetTexture(1, mNormalTextureResource->getTexture());
			renderer->fsSetTexture(2, mSpecularTextureResource->getTexture());
			renderer->fsSetTexture(3, mEmissiveTextureResource->getTexture());

			// Use the sampler state collection to set the sampler states
			renderer->fsSetSamplerStateCollection(0, mSamplerStateCollection);

			// Set the used blend state
			renderer->omSetBlendState(mBlendState);

			// Draw mesh instance
			if (nullptr != mMeshResource)
			{
				mMeshResource->draw();
			}

			// Draw text
			mFontResource->drawText("Imrod", Color4::RED, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.8f, 0.0f))), 0.003f, 0.003f);

			// End scene rendering
			// -> Required for Direct3D 9
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			renderer->endScene();
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}
