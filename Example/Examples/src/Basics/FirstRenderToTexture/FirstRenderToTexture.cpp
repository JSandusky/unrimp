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
#include "Basics/FirstRenderToTexture/FirstRenderToTexture.h"
#include "Framework/Color4.h"

#include <string.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstRenderToTexture::FirstRenderToTexture(const char *rendererName) :
	IApplicationRenderer(rendererName)
{
	// Nothing to do in here
}

FirstRenderToTexture::~FirstRenderToTexture()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstRenderToTexture::onInitialization()
{
	// Call the base implementation
	IApplicationRenderer::onInitialization();

	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Create the texture instance, but without providing texture data (we use the texture as render target)
		// -> Use the "Renderer::TextureFlag::RENDER_TARGET"-flag to mark this texture as a render target
		// -> Required for Direct3D 9, Direct3D 10, Direct3D 11 and Direct3D 12
		// -> Not required for OpenGL and OpenGL ES 2
		// -> The optimized texture clear value is a Direct3D 12 related option
		Renderer::ITexture *texture2D = mTexture2D = renderer->createTexture2D(16, 16, Renderer::TextureFormat::R8G8B8A8, nullptr, Renderer::TextureFlag::RENDER_TARGET, Renderer::TextureUsage::DEFAULT, reinterpret_cast<const Renderer::OptimizedTextureClearValue*>(&Color4::GREEN));

		// Create the framebuffer object (FBO) instance
		mFramebuffer = renderer->createFramebuffer(1, &texture2D);

		{ // Create sampler state: We don't use mipmaps
			Renderer::SamplerState samplerState = Renderer::ISamplerState::getDefaultSamplerState();
			samplerState.maxLOD = 0.0f;
			mSamplerState = renderer->createSamplerState(samplerState);
		}

		{ // Create the root signature
			Renderer::DescriptorRangeBuilder ranges[2];
			ranges[0].initializeSampler(1, 0);
			ranges[1].initialize(Renderer::DescriptorRangeType::SRV, 1, 0, "DiffuseMap", 0);

			Renderer::RootParameterBuilder rootParameters[2];
			rootParameters[0].initializeAsDescriptorTable(1, &ranges[0], Renderer::ShaderVisibility::FRAGMENT);
			rootParameters[1].initializeAsDescriptorTable(1, &ranges[1], Renderer::ShaderVisibility::FRAGMENT);

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
			mVertexArray = renderer->createVertexArray(vertexAttributes, sizeof(vertexArrayVertexBuffers) / sizeof(Renderer::VertexArrayVertexBuffer), vertexArrayVertexBuffers);
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
				const char *fragmentShaderSourceCode = nullptr;
				#include "FirstRenderToTexture_GLSL_110.h"
				#include "FirstRenderToTexture_GLSL_ES2.h"
				#include "FirstRenderToTexture_HLSL_D3D9.h"
				#include "FirstRenderToTexture_HLSL_D3D10_D3D11_D3D12.h"
				#include "FirstRenderToTexture_Null.h"

				// Create the program
				program = shaderLanguage->createProgram(
					*mRootSignature,
					vertexAttributes,
					shaderLanguage->createVertexShaderFromSourceCode(vertexShaderSourceCode),
					shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode));
			}

			// Create the pipeline state object (PSO)
			if (nullptr != program)
			{
				mPipelineState = renderer->createPipelineState(Renderer::PipelineStateBuilder(mRootSignature, program, vertexAttributes));
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstRenderToTexture::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Release the used resources
	mVertexArray = nullptr;
	mPipelineState = nullptr;
	mSamplerState = nullptr;
	mRootSignature = nullptr;
	mFramebuffer = nullptr;
	mTexture2D = nullptr;

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())

	// Call the base implementation
	IApplicationRenderer::onDeinitialization();
}

void FirstRenderToTexture::onDraw()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && nullptr != mPipelineState)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		{ // Render to texture
			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT(renderer, L"Render to texture")

			// This in here is of course just an example. In a real application
			// there would be no point in constantly updating texture content
			// without having any real change.

			// TODO(co) Unbind our texture from the texture unit before rendering into it
			// -> Direct3D 9, OpenGL and OpenGL ES 2 don't mind as long as the texture is not used inside the shader while rendering into it
			// -> Direct3D 10 & 11 go crazy if you're going to render into a texture which is still bound at a texture unit:
			//    "D3D11: WARNING: ID3D11DeviceContext::OMSetRenderTargets: Resource being set to OM RenderTarget slot 0 is still bound on input! [ STATE_SETTING WARNING #9: DEVICE_OMSETRENDERTARGETS_HAZARD ]"
			//    "D3D11: WARNING: ID3D11DeviceContext::OMSetRenderTargets[AndUnorderedAccessViews]: Forcing PS shader resource slot 0 to NULL. [ STATE_SETTING WARNING #7: DEVICE_PSSETSHADERRESOURCES_HAZARD ]"

			// Backup the currently used render target
			Renderer::IRenderTargetPtr renderTarget(renderer->omGetRenderTarget());

			// Set the render target to render into
			renderer->omSetRenderTarget(mFramebuffer);

			// Clear the color buffer of the current render target with green
			renderer->clear(Renderer::ClearFlag::COLOR, Color4::GREEN, 1.0f, 0);

			// Restore the previously set render target
			renderer->omSetRenderTarget(renderTarget);

			// End debug event
			RENDERER_END_DEBUG_EVENT(renderer)
		}

		{ // Use the render to texture result
			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT(renderer, L"Use the render to texture result")

			// Clear the color buffer of the current render target with gray, do also clear the depth buffer
			renderer->clear(Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

			// Set the used graphics root signature
			renderer->setGraphicsRootSignature(mRootSignature);

			// Set diffuse map
			renderer->setGraphicsRootDescriptorTable(0, mSamplerState);
			renderer->setGraphicsRootDescriptorTable(1, mTexture2D);

			// Set the used pipeline state object (PSO)
			renderer->setPipelineState(mPipelineState);

			{ // Setup input assembly (IA)
				// Set the used vertex array
				renderer->iaSetVertexArray(mVertexArray);

				// Set the primitive topology used for draw calls
				renderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_LIST);
			}

			// Render the specified geometric primitive, based on an array of vertices
			renderer->draw(0, 3);

			// End debug event
			RENDERER_END_DEBUG_EVENT(renderer)
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}
