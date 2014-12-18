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
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#ifndef NO_ASSIMP


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "FirstAssimp/FirstAssimp.h"
#include "FirstAssimp/PointsMesh.h"
#include "Framework/Color4.h"

#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/matrix_transform.hpp>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstAssimp::FirstAssimp(const char *rendererName) :
	IApplicationRenderer(rendererName),
	mMesh(nullptr),
	mGlobalTimer(0.0f)
{
	// Nothing to do in here
}

FirstAssimp::~FirstAssimp()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstAssimp::onInitialization()
{
	// Call the base implementation
	IApplicationRenderer::onInitialization();

	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Decide which shader language should be used (for example "GLSL", "HLSL" or "Cg")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer->getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			// Create uniform buffers
			// -> Direct3D 9 and OpenGL ES 2 do not support uniform buffers
			// -> Direct3D 10 and Direct3D 11 do not support individual uniforms
			// -> The renderer is just a light weight abstraction layer, so we need to handle the differences
			if (0 != strcmp(shaderLanguage->getShaderLanguageName(), "Cg") && (0 == strcmp(renderer->getName(), "Direct3D10") || 0 == strcmp(renderer->getName(), "Direct3D11")))
			{
				// Allocate enough memory for a 4x4 floating point matrix
				mUniformBuffer = shaderLanguage->createUniformBuffer(sizeof(float) * 4 * 4, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
			}

			{ // Create the program
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderSourceCode = nullptr;
				const char *fragmentShaderSourceCode = nullptr;
				#include "FirstAssimp_Cg.h"
				#include "FirstAssimp_GLSL_110.h"
				#include "FirstAssimp_GLSL_ES2.h"
				#include "FirstAssimp_HLSL_D3D9.h"
				#include "FirstAssimp_HLSL_D3D10_D3D11.h"
				#include "FirstAssimp_Null.h"

				// Create the program
				mProgram = shaderLanguage->createProgram(
					shaderLanguage->createVertexShader(vertexShaderSourceCode),
					shaderLanguage->createFragmentShader(fragmentShaderSourceCode));
			}

			// Is there a valid program?
			if (nullptr != mProgram)
			{
				// Create mesh instance
				// -> In order to keep it simple, we use simple ASCII strings as filenames which are relative to the executable
				// -> In order to keep it simple, we provide the mesh with the program, usually you want to use a mesh
				//    with multiple programs and therefore using multiple vertex array objects (VAO)
				mMesh = new PointsMesh(*mProgram, "../Data/Imrod/ImrodLowPoly.obj");
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstAssimp::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

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

void FirstAssimp::onUpdate()
{
	// Stop the stopwatch
	mStopwatch.stop();

	// Update the global timer (FPS independent movement)
	mGlobalTimer += mStopwatch.getMilliseconds() * 0.01f;

	// Start the stopwatch
	mStopwatch.start();
}

void FirstAssimp::onDraw()
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
			unsigned int width  = 1;
			unsigned int height = 1;
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
				glm::mat4 objectSpaceToClipSpace	= viewSpaceToClipSpace * worldSpaceToViewSpace * objectSpaceToWorldSpace; 

				// Upload the uniform data
				// -> Two versions: One using an uniform buffer and one setting an individual uniform
				if (nullptr != mUniformBuffer)
				{
					// Copy data
					mUniformBuffer->copyDataFrom(sizeof(float) * 4 * 4, glm::value_ptr(objectSpaceToClipSpace));

					// Assign to stage
					renderer->vsSetUniformBuffer(mProgram->getUniformBlockIndex("UniformBlockDynamicVs", 0), mUniformBuffer);
				}
				else
				{
					// Set uniform
					mProgram->setUniformMatrix4fv(mProgram->getUniformHandle("ObjectSpaceToClipSpaceMatrix"), glm::value_ptr(objectSpaceToClipSpace));
				}
			}

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
