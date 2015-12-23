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
#include "Advanced/FirstGpgpu/FirstGpgpu.h"
#include "Framework/Color4.h"
#include "Framework/PlatformTypes.h"

#include <Renderer/Public/RendererInstance.h>

#include <string.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstGpgpu::FirstGpgpu(const char *rendererName) :
	mRendererInstance(nullptr)
{
	// Copy the given renderer name
	if (nullptr != rendererName)
	{
		strncpy(mRendererName, rendererName, 64);
	}
	else
	{
		mRendererName[0] = '\0';
	}
}

FirstGpgpu::~FirstGpgpu()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}

int FirstGpgpu::run()
{
	// Create renderer instance
	mRendererInstance = new Renderer::RendererInstance(mRendererName, 0);

	// Get the renderer instance and ensure it's valid
	mRenderer = mRendererInstance->getRenderer();
	if (nullptr != mRenderer && mRenderer->isInitialized())
	{
		// Call initialization method
		onInitialization();

		// Begin scene rendering
		// -> Required for Direct3D 9 and Direct3D 12
		// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
		if (mRenderer->beginScene())
		{
			// Let the application to it's job
			onDoJob();

			// End scene rendering
			// -> Required for Direct3D 9 and Direct3D 12
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			mRenderer->endScene();
		}

		// Call de-initialization method
		onDeinitialization();
	}

	// Destroy the renderer instance
	mRenderer = nullptr;
	delete mRendererInstance;
	mRendererInstance = nullptr;

	// Done, no error
	return 0;
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
void FirstGpgpu::onInitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(mRenderer)

	// Create the 2D texture and framebuffer object (FBO) instances
	for (int i = 0; i < 2; ++i)
	{
		// Create the texture instance, but without providing texture data (we use the texture as render target)
		// -> Use the "Renderer::TextureFlag::RENDER_TARGET"-flag to mark this texture as a render target
		// -> Required for Direct3D 9, Direct3D 10, Direct3D 11 and Direct3D 12
		// -> Not required for OpenGL and OpenGL ES 2
		// -> The optimized texture clear value is a Direct3D 12 related option
		Renderer::ITexture *texture2D = mTexture2D[i] = mRenderer->createTexture2D(64, 64, Renderer::TextureFormat::R8G8B8A8, nullptr, Renderer::TextureFlag::RENDER_TARGET, Renderer::TextureUsage::DEFAULT, reinterpret_cast<const Renderer::OptimizedTextureClearValue*>(&Color4::BLUE));

		// Create the framebuffer object (FBO) instance
		mFramebuffer[i] = mRenderer->createFramebuffer(1, &texture2D);
	}

	{ // Create sampler state: We don't use mipmaps
		Renderer::SamplerState samplerState = Renderer::ISamplerState::getDefaultSamplerState();
		samplerState.maxLOD = 0.0f;
		mSamplerState = mRenderer->createSamplerState(samplerState);
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
		mRootSignature = mRenderer->createRootSignature(rootSignature);
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

	{ // Create vertex array object (VAO) for content generation
		// Create the vertex buffer object (VBO)
		// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
		static const float VERTEX_POSITION[] =
		{					// Vertex ID	Triangle on screen
			 0.0f, 1.0f,	// 0				0
			 1.0f, 0.0f,	// 1			   .   .
			-0.5f, 0.0f		// 2			  2.......1
		};
		Renderer::IVertexBufferPtr vertexBuffer(mRenderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

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
		mVertexArrayContentGeneration = mRenderer->createVertexArray(vertexAttributes, sizeof(vertexArrayVertexBuffers) / sizeof(Renderer::VertexArrayVertexBuffer), vertexArrayVertexBuffers);
	}

	{ // Create vertex array object (VAO) for content processing
		// Create the vertex buffer object (VBO)
		// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
		static const float VERTEX_POSITION[] =
		{					// Vertex ID	Triangle strip on screen
			-1.0f, -1.0f,	// 0			  1.......3
			-1.0f,  1.0f,	// 1			  .	  .   .
			 1.0f, -1.0f,	// 2			  0.......2
			 1.0f,  1.0f	// 3
		};
		Renderer::IVertexBufferPtr vertexBuffer(mRenderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

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
		mVertexArrayContentProcessing = mRenderer->createVertexArray(vertexAttributes, sizeof(vertexArrayVertexBuffers) / sizeof(Renderer::VertexArrayVertexBuffer), vertexArrayVertexBuffers);
	}

	// Create the programs: Decide which shader language should be used (for example "GLSL" or "HLSL")
	Renderer::IShaderLanguagePtr shaderLanguage(mRenderer->getShaderLanguage());
	if (nullptr != shaderLanguage)
	{
		// Create the programs
		Renderer::IProgramPtr programContentGeneration;
		Renderer::IProgramPtr programContentProcessing;
		{
			// Get the shader source code (outsourced to keep an overview)
			const char *vertexShaderSourceCode = nullptr;
			const char *fragmentShaderSourceCode_ContentGeneration = nullptr;
			const char *fragmentShaderSourceCode_ContentProcessing = nullptr;
			#include "FirstGpgpu_GLSL_110.h"
			#include "FirstGpgpu_GLSL_ES2.h"
			#include "FirstGpgpu_HLSL_D3D9.h"
			#include "FirstGpgpu_HLSL_D3D10_D3D11_D3D12.h"
			#include "FirstGpgpu_Null.h"

			// In order to keep this example simple and to show that it's possible, we use the same vertex shader for both programs
			// -> Depending on the used graphics API and whether or not the shader compiler & linker is clever,
			//    the unused texture coordinate might get optimized out
			// -> In a real world application you shouldn't rely on shader compiler & linker behaviour assumptions
			Renderer::IVertexShaderPtr vertexShader(shaderLanguage->createVertexShaderFromSourceCode(vertexShaderSourceCode));
			programContentGeneration = shaderLanguage->createProgram(*mRootSignature, vertexAttributes, vertexShader, shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode_ContentGeneration));
			programContentProcessing = shaderLanguage->createProgram(*mRootSignature, vertexAttributes, vertexShader, shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode_ContentProcessing));
		}

		// Create the pipeline state objects (PSO)
		if (nullptr != programContentGeneration && nullptr != programContentProcessing)
		{
			{ // Content generation
				Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder(mRootSignature, programContentGeneration, vertexAttributes);
				pipelineState.depthStencilState.depthEnable = false;
				mPipelineStateContentGeneration = mRenderer->createPipelineState(pipelineState);
			}
			{ // Content processing
				Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder(mRootSignature, programContentProcessing, vertexAttributes);
				pipelineState.depthStencilState.depthEnable = false;
				mPipelineStateContentProcessing = mRenderer->createPipelineState(pipelineState);
			}
		}
	}

	// End debug event
	RENDERER_END_DEBUG_EVENT(mRenderer)
}

void FirstGpgpu::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(mRenderer)

	// Release the used resources
	mVertexArrayContentProcessing = nullptr;
	mPipelineStateContentProcessing = nullptr;
	mVertexArrayContentGeneration = nullptr;
	mPipelineStateContentGeneration = nullptr;
	mSamplerState = nullptr;
	mRootSignature = nullptr;
	for (int i = 0; i < 2; ++i)
	{
		mFramebuffer[i] = nullptr;
		mTexture2D[i] = nullptr;
	}

	// End debug event
	RENDERER_END_DEBUG_EVENT(mRenderer)
}

void FirstGpgpu::onDoJob()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(mRenderer)

	// Generate the content of the 2D texture to process later on
	// -> After this step, "mTexture2D[0]" holds the content we want to process later on
	generate2DTextureContent();

	// Content processing
	// -> After this step, "mTexture2D[1]" holds the processed content
	contentProcessing();

	// TODO(co) "Renderer::IRenderer::map()"/"Renderer::IRenderer::unmap()" are currently under construction
	// Map the texture holding the processed content
	Renderer::MappedSubresource mappedSubresource;
	if (mRenderer->map(*mTexture2D[1], 0, Renderer::MapType::READ, 0, mappedSubresource))
	{
		// Get the processed content pointer
//		const uint8_t *data = static_cast<uint8_t*>(mappedSubresource.data);

		// TODO(co) Write it out as image?

		// Unmap the texture holding the processed content
		mRenderer->unmap(*mTexture2D[1], 0);
	}

	// End debug event
	RENDERER_END_DEBUG_EVENT(mRenderer)
}

void FirstGpgpu::generate2DTextureContent()
{
	// Get and check the program instances
	if (nullptr != mPipelineStateContentGeneration && nullptr != mPipelineStateContentProcessing)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT(mRenderer, L"Generate the content of the 2D texture to process later on")

		// Set the render target to render into
		mRenderer->omSetRenderTarget(mFramebuffer[0]);

		// Clear the color buffer of the current render target with blue
		mRenderer->clear(Renderer::ClearFlag::COLOR, Color4::BLUE, 1.0f, 0);

		// Set the used graphics root signature
		mRenderer->setGraphicsRootSignature(mRootSignature);

		{ // Set the viewport and scissor rectangle
			// Get the render target with and height
			uint32_t width  = 1;
			uint32_t height = 1;
			Renderer::IRenderTarget *renderTarget = mRenderer->omGetRenderTarget();
			if (nullptr != renderTarget)
			{
				renderTarget->getWidthAndHeight(width, height);
			}

			// Set the viewport and scissor rectangle
			mRenderer->rsSetViewportAndScissorRectangle(0, 0, width, height);
		}

		// Set the used pipeline state object (PSO)
		mRenderer->setPipelineState(mPipelineStateContentGeneration);

		{ // Setup input assembly (IA)
			// Set the used vertex array
			mRenderer->iaSetVertexArray(mVertexArrayContentGeneration);

			// Set the primitive topology used for draw calls
			mRenderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_LIST);
		}

		// Render the specified geometric primitive, based on indexing into an array of vertices
		mRenderer->draw(0, 3);

		// End debug event
		RENDERER_END_DEBUG_EVENT(mRenderer)
	}
}

void FirstGpgpu::contentProcessing()
{
	// Get and check the program instance
	if (nullptr != mPipelineStateContentProcessing)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT(mRenderer, L"Content processing")

		// Backup the currently used render target
		Renderer::IRenderTargetPtr renderTarget(mRenderer->omGetRenderTarget());

		// Set the render target to render into
		mRenderer->omSetRenderTarget(mFramebuffer[1]);

		// We don't need to clear the current render target because our fullscreen quad covers the full screen

		// Set the used graphics root signature
		mRenderer->setGraphicsRootSignature(mRootSignature);

		// Set the used pipeline state object (PSO)
		mRenderer->setPipelineState(mPipelineStateContentProcessing);

		// Set content map
		mRenderer->setGraphicsRootDescriptorTable(0, mSamplerState);
		mRenderer->setGraphicsRootDescriptorTable(1, mTexture2D[0]);

		{ // Setup input assembly (IA)
			// Set the used vertex array
			mRenderer->iaSetVertexArray(mVertexArrayContentProcessing);

			// Set the primitive topology used for draw calls
			mRenderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_STRIP);
		}

		// Render the specified geometric primitive, based on indexing into an array of vertices
		mRenderer->draw(0, 4);

		// Restore the previously set render target
		mRenderer->omSetRenderTarget(renderTarget);

		// End debug event
		RENDERER_END_DEBUG_EVENT(mRenderer)
	}
}
