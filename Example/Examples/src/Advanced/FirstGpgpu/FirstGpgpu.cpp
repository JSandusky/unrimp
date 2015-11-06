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
#include "Framework/IApplicationRenderer.h"

#include <string.h>


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Renderer application
*
*  @note
*    - We only use this class in order to create and manage the renderer instance easily in this example
*/
class ApplicationRenderer : public IApplicationRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] rendererName
		*    Case sensitive ASCII name of the renderer to instance, if null pointer or unknown renderer no renderer will be used.
		*    Example renderer names: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
		*/
		explicit ApplicationRenderer(const char *rendererName) :
			IApplicationRenderer(rendererName)
		{
			// Call application implementation initialization method
			onInitialization();
		}

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ApplicationRenderer()
		{
			// Call application implementation de-initialization method
			onDeinitialization();
		}


};


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstGpgpu::FirstGpgpu(const char *rendererName)
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
	// Create an instance of a renderer application on the C runtime stack
	// -> We only use this instance in order to create and manage the renderer instance easily in this example
	ApplicationRenderer applicationRenderer(mRendererName);

	// Get the renderer instance and ensure it's valid
	mRenderer = applicationRenderer.getRenderer();
	if (nullptr != mRenderer)
	{
		// Call initialization method
		onInitialization();

		// Let the application to it's job
		onDoJob();

		// Call de-initialization method
		onDeinitialization();

		// Release our renderer reference
		mRenderer = nullptr;
	}

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
		// -> Required for Direct3D 9, Direct3D 10 and Direct3D 11
		// -> Not required for OpenGL and OpenGL ES 2
		Renderer::ITexture *texture2D = mTexture2D[i] = mRenderer->createTexture2D(64, 64, Renderer::TextureFormat::R8G8B8A8, nullptr, Renderer::TextureFlag::RENDER_TARGET);

		// Create the framebuffer object (FBO) instance
		mFramebuffer[i] = mRenderer->createFramebuffer(1, &texture2D);
	}

	{ // Create sampler state
		// -> Our texture does not have any mipmaps, set "Renderer::SamplerState::maxLOD" to zero
		//    in order to ensure a correct behaviour across the difference graphics APIs
		// -> When not doing this you usually have no issues when using OpenGL, OpenGL ES 2, Direct 10,
		//    Direct3D 11 or Direct3D 9 with the "ps_2_0"-profile, but when using Direct3D 9 with the
		//    "ps_3_0"-profile you might get into trouble due to another internal graphics API behaviour
		Renderer::SamplerState samplerState = Renderer::ISamplerState::getDefaultSamplerState();
		samplerState.maxLOD = 0.0f;	// We don't use mipmaps
		mSamplerState = mRenderer->createSamplerState(samplerState);
	}

	{ // Create the root signature
		// TODO(co) Correct root signature

		// Setup
		Renderer::RootSignatureBuilder rootSignature;
		rootSignature.initialize(0, nullptr, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// Create the instance
		mRootSignature = mRenderer->createRootSignature(rootSignature);
	}

	{ // Depth stencil state
		// -> By default depth test is enabled
		// -> In this simple example we don't need depth test, so, disable it so we don't need to care about the depth buffer

		// Create depth stencil state
		Renderer::DepthStencilState depthStencilState = Renderer::IDepthStencilState::getDefaultDepthStencilState();
		depthStencilState.depthEnable = false;
		mDepthStencilState = mRenderer->createDepthStencilState(depthStencilState);

		// Set the depth stencil state directly within this initialization phase, we don't change it later on
		mRenderer->omSetDepthStencilState(mDepthStencilState);
	}

	// Vertex input layout
	const Renderer::VertexAttribute vertexAttributesLayout[] =
	{
		{ // Attribute 0
			// Data destination
			Renderer::VertexAttributeFormat::FLOAT_2,	// vertexAttributeFormat (Renderer::VertexAttributeFormat::Enum)
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
		// Get the shader source code (outsourced to keep an overview)
		const char *vertexShaderSourceCode = nullptr;
		const char *fragmentShaderSourceCode_ContentGeneration = nullptr;
		const char *fragmentShaderSourceCode_ContentProcessing = nullptr;
		#include "FirstGpgpu_GLSL_110.h"
		#include "FirstGpgpu_GLSL_ES2.h"
		#include "FirstGpgpu_HLSL_D3D9.h"
		#include "FirstGpgpu_HLSL_D3D10_D3D11.h"
		#include "FirstGpgpu_Null.h"

		// In order to keep this example simple and to show that it's possible, we use the same vertex shader for both programs
		// -> Depending on the used graphics API and whether or not the shader compiler & linker is clever,
		//    the unused texture coordinate might get optimized out
		// -> In a real world application you shouldn't rely on shader compiler & linker behaviour assumptions
		Renderer::IVertexShaderPtr vertexShader(shaderLanguage->createVertexShaderFromSourceCode(vertexShaderSourceCode));
		mProgramContentGeneration = shaderLanguage->createProgram(*mRootSignature, vertexAttributes, vertexShader, shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode_ContentGeneration));
		mProgramContentProcessing = shaderLanguage->createProgram(*mRootSignature, vertexAttributes, vertexShader, shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode_ContentProcessing));
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
	mProgramContentProcessing = nullptr;
	mVertexArrayContentGeneration = nullptr;
	mProgramContentGeneration = nullptr;
	mDepthStencilState = nullptr;
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
	if (nullptr != mProgramContentGeneration && nullptr != mProgramContentProcessing)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT(mRenderer, L"Generate the content of the 2D texture to process later on")

		// Unbind our texture from the texture unit before rendering into it
		// -> Direct3D 9, OpenGL and OpenGL ES 2 don't mind as long as the texture is not used inside the shader while rendering into it
		// -> Direct3D 10 & 11 go crazy if you're going to render into a texture which is still bound at a texture unit:
		//    "D3D11: WARNING: ID3D11DeviceContext::OMSetRenderTargets: Resource being set to OM RenderTarget slot 0 is still bound on input! [ STATE_SETTING WARNING #9: DEVICE_OMSETRENDERTARGETS_HAZARD ]"
		//    "D3D11: WARNING: ID3D11DeviceContext::OMSetRenderTargets[AndUnorderedAccessViews]: Forcing PS shader resource slot 0 to NULL. [ STATE_SETTING WARNING #7: DEVICE_PSSETSHADERRESOURCES_HAZARD ]"
		mRenderer->fsSetTexture(mProgramContentProcessing->setTextureUnit(mProgramContentProcessing->getUniformHandle("ContentMap"), 0), nullptr);

		// Backup the currently used render target
		Renderer::IRenderTargetPtr previousRenderTarget(mRenderer->omGetRenderTarget());

		// Set the render target to render into
		mRenderer->omSetRenderTarget(mFramebuffer[0]);

		// Begin scene rendering
		// -> Required for Direct3D 9
		// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
		if (mRenderer->beginScene())
		{
			// Clear the color buffer of the current render target with blue
			mRenderer->clear(Renderer::ClearFlag::COLOR, Color4::BLUE, 1.0f, 0);

			// Set the used graphics root signature
			mRenderer->setGraphicsRootSignature(mRootSignature);

			{ // Set the viewport
				// Get the render target with and height
				uint32_t width  = 1;
				uint32_t height = 1;
				Renderer::IRenderTarget *renderTarget = mRenderer->omGetRenderTarget();
				if (nullptr != renderTarget)
				{
					renderTarget->getWidthAndHeight(width, height);
				}

				// Set the viewport
				const Renderer::Viewport viewport =
				{
					0.0f,						// topLeftX (float)
					0.0f,						// topLeftY (float)
					static_cast<float>(width),	// width (float)
					static_cast<float>(height),	// height (float)
					0.0f,						// minDepth (float)
					1.0f						// maxDepth (float)
				};
				mRenderer->rsSetViewports(1, &viewport);
			}

			// Set the used program
			mRenderer->setProgram(mProgramContentGeneration);

			{ // Setup input assembly (IA)
				// Set the used vertex array
				mRenderer->iaSetVertexArray(mVertexArrayContentGeneration);

				// Set the primitive topology used for draw calls
				mRenderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_LIST);
			}

			// Render the specified geometric primitive, based on indexing into an array of vertices
			mRenderer->draw(0, 3);

			// End scene rendering
			// -> Required for Direct3D 9
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			mRenderer->endScene();
		}

		// Restore the previously set render target
		mRenderer->omSetRenderTarget(previousRenderTarget);

		// End debug event
		RENDERER_END_DEBUG_EVENT(mRenderer)
	}
}

void FirstGpgpu::contentProcessing()
{
	// Get and check the program instance
	if (nullptr != mProgramContentProcessing)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT(mRenderer, L"Content processing")

		// Backup the currently used render target
		Renderer::IRenderTargetPtr renderTarget(mRenderer->omGetRenderTarget());

		// Set the render target to render into
		mRenderer->omSetRenderTarget(mFramebuffer[1]);

		// We don't need to clear the current render target because our fullscreen quad covers the full screen

		// Begin scene rendering
		// -> Required for Direct3D 9
		// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
		if (mRenderer->beginScene())
		{
			// Set the used graphics root signature
			mRenderer->setGraphicsRootSignature(mRootSignature);

			// Set the used program
			mRenderer->setProgram(mProgramContentProcessing);

			{ // Setup input assembly (IA)
				// Set the used vertex array
				mRenderer->iaSetVertexArray(mVertexArrayContentProcessing);

				// Set the primitive topology used for draw calls
				mRenderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_STRIP);
			}

			{ // Set content map (texture unit 0 by default)
				// Tell the renderer API which texture should be bound to which texture unit
				// -> When using OpenGL or OpenGL ES 2 this is required
				// -> OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension supports explicit binding points ("layout(binding = 0)"
				//    in GLSL shader) , for backward compatibility we don't use it in here
				// -> When using Direct3D 9, Direct3D 10 or Direct3D 11, the texture unit
				//    to use is usually defined directly within the shader by using the "register"-keyword
				// -> Usually, this should only be done once during initialization, this example does this
				//    every frame to keep it local for better overview
				const uint32_t unit = mProgramContentProcessing->setTextureUnit(mProgramContentProcessing->getUniformHandle("ContentMap"), 0);

				// Set the used texture at the texture unit
				mRenderer->fsSetTexture(unit, mTexture2D[0]);

				// Set the used sampler state at the texture unit
				mRenderer->fsSetSamplerState(unit, mSamplerState);
			}

			// Render the specified geometric primitive, based on indexing into an array of vertices
			mRenderer->draw(0, 4);

			// End scene rendering
			// -> Required for Direct3D 9
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			mRenderer->endScene();
		}

		// Restore the previously set render target
		mRenderer->omSetRenderTarget(renderTarget);

		// End debug event
		RENDERER_END_DEBUG_EVENT(mRenderer)
	}
}
