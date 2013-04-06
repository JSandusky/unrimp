/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
#include "FirstPostProcessing/FirstPostProcessing.h"
#include "Framework/Color4.h"

#include <string.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstPostProcessing::FirstPostProcessing(const char *rendererName) :
	IApplicationRenderer(rendererName)
{
	// Nothing to do in here
}

FirstPostProcessing::~FirstPostProcessing()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstPostProcessing::onInitialization()
{
	// Call the base implementation
	IApplicationRenderer::onInitialization();

	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Create the framebuffer object (FBO) instance by using the current window size
		recreateFramebuffer();

		{ // Create sampler state
		  // -> Our texture does not have any mipmaps, set "Renderer::SamplerState::maxLOD" to zero
		  //    in order to ensure a correct behaviour across the difference graphics APIs
		  // -> When not doing this you usually have no issues when using OpenGL, OpenGL ES 2, Direct 10,
		  //    Direct3D 11 or Direct3D 9 with the "ps_2_0"-profile, but when using Direct3D 9 with the
		  //    "ps_3_0"-profile you might get into trouble due to another internal graphics API behaviour
			Renderer::SamplerState samplerState = Renderer::ISamplerState::getDefaultSamplerState();
			samplerState.maxLOD = 0.0f;	// We don't use mipmaps
			mSamplerState = renderer->createSamplerState(samplerState);
		}

		{ // Depth stencil state
		  // -> By default depth test is enabled
		  // -> In this simple example we don't need depth test, so, disable it so we don't need to care about the depth buffer

			// Create depth stencil state
			Renderer::DepthStencilState depthStencilState = Renderer::IDepthStencilState::getDefaultDepthStencilState();
			depthStencilState.depthEnable = false;
			mDepthStencilState = renderer->createDepthStencilState(depthStencilState);

			// Set the depth stencil state directly within this initialization phase, we don't change it later on
			renderer->omSetDepthStencilState(mDepthStencilState);
		}

		// Decide which shader language should be used (for example "GLSL", "HLSL" or "Cg")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer->getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			{ // Create the programs
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderSourceCode = nullptr;
				const char *fragmentShaderSourceCode_SceneRendering = nullptr;
				const char *fragmentShaderSourceCode_PostProcessing = nullptr;
				#include "FirstPostProcessing_Cg.h"
				#include "FirstPostProcessing_GLSL_110.h"
				#include "FirstPostProcessing_GLSL_ES2.h"
				#include "FirstPostProcessing_HLSL_D3D9.h"
				#include "FirstPostProcessing_HLSL_D3D10_D3D11.h"
				#include "FirstPostProcessing_Null.h"

				// In order to keep this example simple and to show that it's possible, we use the same vertex shader for both programs
				// -> Depending on the used graphics API and whether or not the shader compiler & linker is clever,
				//    the unused texture coordinate might get optimized out
				// -> In a real world application you shouldn't rely on shader compiler & linker behaviour assumptions
				Renderer::IVertexShaderPtr vertexShader (shaderLanguage->createVertexShader(vertexShaderSourceCode));
				mProgramSceneRendering = shaderLanguage->createProgram(vertexShader, shaderLanguage->createFragmentShader(fragmentShaderSourceCode_SceneRendering));
				mProgramPostProcessing = shaderLanguage->createProgram(vertexShader, shaderLanguage->createFragmentShader(fragmentShaderSourceCode_PostProcessing));
			}

			// Is there a valid program for scene rendering?
			if (nullptr != mProgramSceneRendering)
			{
				// Create the vertex buffer object (VBO)
				// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
				static const float VERTEX_POSITION[] =
				{					// Vertex ID	Triangle on screen
					 0.0f, 1.0f,	// 0				0
					 1.0f, 0.0f,	// 1			   .   .
					-0.5f, 0.0f		// 2			  2.......1
				};
				Renderer::IVertexBufferPtr vertexBuffer (renderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

				// Create vertex array object (VAO)
				// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
				// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
				// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
				//    reference of the used vertex buffer objects (VBO). If the reference counter of a
				//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
				const Renderer::VertexArrayAttribute vertexArray[] =
				{
					{ // Attribute 0
						// Data destination
						Renderer::VertexArrayFormat::FLOAT_2,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
						"Position",								// name[64] (char)
						"POSITION",								// semantic[64] (char)
						0,										// semanticIndex (unsigned int)
						// Data source
						vertexBuffer,							// vertexBuffer (Renderer::IVertexBuffer *)
						0,										// offset (unsigned int)
						sizeof(float) * 2,						// stride (unsigned int)
						// Data source, instancing part
						0										// instancesPerElement (unsigned int)
					}
				};
				mVertexArraySceneRendering = mProgramSceneRendering->createVertexArray(sizeof(vertexArray) / sizeof(Renderer::VertexArrayAttribute), vertexArray);
			}

			// Is there a valid program for post-processing?
			if (nullptr != mProgramPostProcessing)
			{
				// Create the vertex buffer object (VBO)
				// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
				static const float VERTEX_POSITION[] =
				{					// Vertex ID	Triangle strip on screen
					-1.0f, -1.0f,	// 0			  1.......3
					-1.0f,  1.0f,	// 1			  .	  .   .
					 1.0f, -1.0f,	// 2			  0.......2
					 1.0f,  1.0f	// 3
				};
				Renderer::IVertexBufferPtr vertexBuffer (renderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

				// Create vertex array object (VAO)
				// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
				// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
				// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
				//    reference of the used vertex buffer objects (VBO). If the reference counter of a
				//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
				const Renderer::VertexArrayAttribute vertexArray[] =
				{
					{ // Attribute 0
						// Data destination
						Renderer::VertexArrayFormat::FLOAT_2,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
						"Position",								// name[64] (char)
						"POSITION",								// semantic[64] (char)
						0,										// semanticIndex (unsigned int)
						// Data source
						vertexBuffer,							// vertexBuffer (Renderer::IVertexBuffer *)
						0,										// offset (unsigned int)
						sizeof(float) * 2,						// stride (unsigned int)
						// Data source, instancing part
						0										// instancesPerElement (unsigned int)
					}
				};
				mVertexArrayPostProcessing = mProgramSceneRendering->createVertexArray(sizeof(vertexArray) / sizeof(Renderer::VertexArrayAttribute), vertexArray);
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstPostProcessing::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Release the used resources
	mVertexArrayPostProcessing = nullptr;
	mProgramPostProcessing = nullptr;
	mVertexArraySceneRendering = nullptr;
	mProgramSceneRendering = nullptr;
	mDepthStencilState = nullptr;
	mSamplerState = nullptr;
	mFramebuffer = nullptr;
	mTexture2D = nullptr;

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())

	// Call the base implementation
	IApplicationRenderer::onDeinitialization();
}

void FirstPostProcessing::onResize()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Call the base implementation
	IApplicationRenderer::onResize();

	// Recreate the framebuffer object (FBO) instance by using the current window size
	recreateFramebuffer();

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())
}

void FirstPostProcessing::onDraw()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Scene rendering
		sceneRendering();

		// Post-processing
		postProcessing();

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
void FirstPostProcessing::recreateFramebuffer()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Get the window size
		int width  = 0;
		int height = 0;
		getWindowSize(width, height);

		// Due to the usage of smart pointers there's no need to explicitly free the previous resources, this is done automatically

		// Create the texture instance, but without providing texture data (we use the texture as render target)
		// -> Use the "Renderer::TextureFlag::RENDER_TARGET"-flag to mark this texture as a render target
		// -> Required for Direct3D 9, Direct3D 10 and Direct3D 11
		// -> Not required for OpenGL and OpenGL ES 2
		Renderer::ITexture *texture2D = mTexture2D = renderer->createTexture2D(static_cast<unsigned int>(width), static_cast<unsigned int>(height), Renderer::TextureFormat::R8G8B8A8, nullptr, Renderer::TextureFlag::RENDER_TARGET);

		// Create the framebuffer object (FBO) instance
		mFramebuffer = renderer->createFramebuffer(1, &texture2D);

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstPostProcessing::sceneRendering()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && nullptr != mProgramSceneRendering && nullptr != mProgramPostProcessing)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// This in here is of course just an example. In a real application
		// there would be no point in constantly updating texture content
		// without having any real change.

		// Unbind our texture from the texture unit before rendering into it
		// -> Direct3D 9, OpenGL and OpenGL ES 2 don't mind as long as the texture is not used inside the shader while rendering into it
		// -> Direct3D 10 & 11 go crazy if you're going to render into a texture which is still bound at a texture unit:
		//    "D3D11: WARNING: ID3D11DeviceContext::OMSetRenderTargets: Resource being set to OM RenderTarget slot 0 is still bound on input! [ STATE_SETTING WARNING #9: DEVICE_OMSETRENDERTARGETS_HAZARD ]"
		//    "D3D11: WARNING: ID3D11DeviceContext::OMSetRenderTargets[AndUnorderedAccessViews]: Forcing PS shader resource slot 0 to NULL. [ STATE_SETTING WARNING #7: DEVICE_PSSETSHADERRESOURCES_HAZARD ]"
		renderer->fsSetTexture(mProgramPostProcessing->setTextureUnit(mProgramPostProcessing->getUniformHandle("DiffuseMap"), 0), nullptr);

		// Backup the currently used render target
		Renderer::IRenderTargetPtr renderTarget(renderer->omGetRenderTarget());

		// Set the render target to render into
		renderer->omSetRenderTarget(mFramebuffer);

		// Clear the color buffer of the current render target with blue
		renderer->clear(Renderer::ClearFlag::COLOR, Color4::BLUE, 1.0f, 0);

		// Begin scene rendering
		// -> Required for Direct3D 9
		// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
		if (renderer->beginScene())
		{
			// Set the used program
			renderer->setProgram(mProgramSceneRendering);

			{ // Setup input assembly (IA)
				// Set the used vertex array
				renderer->iaSetVertexArray(mVertexArraySceneRendering);

				// Set the primitive topology used for draw calls
				renderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_LIST);
			}

			// Render the specified geometric primitive, based on indexing into an array of vertices
			renderer->draw(0, 3);

			// End scene rendering
			// -> Required for Direct3D 9
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			renderer->endScene();
		}

		// Restore the previously set render target
		renderer->omSetRenderTarget(renderTarget);

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstPostProcessing::postProcessing()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && mProgramPostProcessing)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// We don't need to clear the current render target because our fullscreen quad covers the full screen

		// Begin scene rendering
		// -> Required for Direct3D 9
		// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
		if (renderer->beginScene())
		{
			// Set the used program
			renderer->setProgram(mProgramPostProcessing);

			{ // Setup input assembly (IA)
				// Set the used vertex array
				renderer->iaSetVertexArray(mVertexArrayPostProcessing);

				// Set the primitive topology used for draw calls
				renderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_STRIP);
			}

			{ // Set diffuse map (texture unit 0 by default)
				// Tell the renderer API which texture should be bound to which texture unit
				// -> When using OpenGL or OpenGL ES 2 this is required
				// -> OpenGL 4.2 supports explicit binding points ("layout(binding=0)" in GLSL shader),
				//    for backward compatibility we don't use it in here
				// -> When using Direct3D 9, Direct3D 10, Direct3D 11 or the Cg shader language, the texture unit
				//    to use is usually defined directly within the shader by using the "register"-keyword
				// -> Usually, this should only be done once during initialization, this example does this
				//    every frame to keep it local for better overview
				const unsigned int unit = mProgramPostProcessing->setTextureUnit(mProgramPostProcessing->getUniformHandle("DiffuseMap"), 0);

				// Set the used texture at the texture unit
				renderer->fsSetTexture(unit, mTexture2D);

				// Set the used sampler state at the texture unit
				renderer->fsSetSamplerState(unit, mSamplerState);
			}

			// Render the specified geometric primitive, based on indexing into an array of vertices
			renderer->draw(0, 4);

			// End scene rendering
			// -> Required for Direct3D 9
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			renderer->endScene();
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}
