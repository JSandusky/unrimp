/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#ifndef NO_ASSIMP


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Assimp/AssimpMesh/AssimpMesh.h"
#include "Assimp/AssimpMesh/Mesh.h"
#include "Framework/Color4.h"
#include "Framework/TGALoader.h"

#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/matrix_transform.hpp>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
AssimpMesh::AssimpMesh(const char *rendererName) :
	IApplicationRenderer(rendererName),
	mMesh(nullptr),
	mUniformBlockIndex(0),
	mObjectSpaceToClipSpaceMatrixUniformHandle(NULL_HANDLE),
	mObjectSpaceToViewSpaceMatrixUniformHandle(NULL_HANDLE),
	mGlobalTimer(0.0f)
{
	// Nothing to do in here
}

AssimpMesh::~AssimpMesh()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void AssimpMesh::onInitialization()
{
	// Call the base implementation
	IApplicationRenderer::onInitialization();

	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

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

			{ // Create the program
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderProfile = nullptr;
				const char *vertexShaderSourceCode = nullptr;
				const char *fragmentShaderProfile = nullptr;
				const char *fragmentShaderSourceCode = nullptr;
				#include "AssimpMesh_GLSL_110.h"
				#include "AssimpMesh_GLSL_ES2.h"
				#include "AssimpMesh_HLSL_D3D9.h"
				#include "AssimpMesh_HLSL_D3D10_D3D11.h"
				#include "AssimpMesh_Null.h"

				// Create the program
				mProgram = shaderLanguage->createProgram(
					shaderLanguage->createVertexShader(vertexShaderSourceCode, vertexShaderProfile),
					shaderLanguage->createFragmentShader(fragmentShaderSourceCode, fragmentShaderProfile));
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

				// Optimization: Cached data to not bother the renderer API to much
				if (nullptr != mUniformBuffer)
				{
					mUniformBlockIndex = mProgram->getUniformBlockIndex("UniformBlockDynamicVs", 0);
				}
				else
				{
					mObjectSpaceToClipSpaceMatrixUniformHandle = mProgram->getUniformHandle("ObjectSpaceToClipSpaceMatrix");
					mObjectSpaceToViewSpaceMatrixUniformHandle = mProgram->getUniformHandle("ObjectSpaceToViewSpaceMatrix");
				}

				// Create mesh instance
				// -> In order to keep it simple, we use simple ASCII strings as filenames which are relative to the executable
				// -> In order to keep it simple, we provide the mesh with the program, usually you want to use a mesh
				//    with multiple programs and therefore using multiple vertex array objects (VAO)
				mMesh = new Mesh(*mProgram, "../DataSource/Imrod/ImrodLowPoly.obj");
			}

			// Use texture collections when you want you exploit renderer API methods like
			// "ID3D10Device::PSSetShaderResources()" from Direct3D 10 or "ID3D11DeviceContext::PSSetShaderResources()" from Direct3D 11.
			// By using a single API call, multiple resources can be set at one and the same time in an efficient way.
			{
				// Load in the diffuse, emissive, normal and specular texture
				// -> In order to keep this sample simple, the texture management is quite primitive
				// -> No texture compression used
				// -> The tangent space normal map is stored with three components, two would be enought to recalculate the third component within the fragment shader
				// -> The specular map could be put into the alpha channel of the diffuse map instead of storing it as an individual texture
				Renderer::ITexture *textures[] =
				{
					loadTGATexture(*renderer, "../DataSource/Imrod/Imrod_Diffuse.tga"),
					loadTGATexture(*renderer, "../DataSource/Imrod/Imrod_Illumination.tga"),
					loadTGATexture(*renderer, "../DataSource/Imrod/Imrod_norm.tga"),
					loadTGATexture(*renderer, "../DataSource/Imrod/Imrod_spec.tga")
				};
				/*
				// TODO(co)
				RendererRuntime::TextureResourceManager& textureResourceManager = rendererRuntime->getTextureResourceManager();
				Renderer::ITexture *textures[] =
				{
					textureResourceManager.loadByLocalAssetName("Example/Texture/Imrod/Imrod_Diffuse")
					textureResourceManager.loadByLocalAssetName("Example/Texture/Imrod/Imrod_Illumination")
					textureResourceManager.loadByLocalAssetName("Example/Texture/Imrod/Imrod_norm")
					textureResourceManager.loadByLocalAssetName("Example/Texture/Imrod/Imrod_spec")
				};
				*/

				// Create the texture collection
				// -> The texture collection keeps a reference to the provided resources, so,
				//    we don't need to care about the resource cleanup in here or later on
				mTextureCollection = renderer->createTextureCollection(sizeof(textures) / sizeof(Renderer::ITexture2D *), textures);
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

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void AssimpMesh::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Release the used texturing resources
	mSamplerStateCollection = nullptr;
	mTextureCollection = nullptr;

	// Destroy mesh instance
	if (nullptr != mMesh)
	{
		delete mMesh;
		mMesh = nullptr;
	}

	// Release the used resources
	mProgram = nullptr;
	mUniformBuffer = nullptr;

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())

	// Call the base implementation
	IApplicationRenderer::onDeinitialization();
}

void AssimpMesh::onUpdate()
{
	// Stop the stopwatch
	mStopwatch.stop();

	// Update the global timer (FPS independent movement)
	mGlobalTimer += mStopwatch.getMilliseconds() * 0.0005f;

	// Start the stopwatch
	mStopwatch.start();
}

void AssimpMesh::onDraw()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && nullptr != mProgram)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Clear the color buffer of the current render target with gray, do also clear the depth buffer
		renderer->clear(Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

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
			renderer->fsSetTextureCollection(0, mTextureCollection);

			// Use the sampler state collection to set the sampler states
			renderer->fsSetSamplerStateCollection(0, mSamplerStateCollection);

			// Draw mesh instance
			if (nullptr != mMesh)
			{
				mMesh->draw();
			}

			// End scene rendering
			// -> Required for Direct3D 9
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			renderer->endScene();
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#endif // NO_ASSIMP
