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
#include "FirstTriangle/FirstTriangle.h"
#include "Framework/Color4.h"

#include <string.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstTriangle::FirstTriangle(const char *rendererName) :
	IApplicationRenderer(rendererName)
{
	// Nothing to do in here
}

FirstTriangle::~FirstTriangle()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstTriangle::onInitialization()
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
			{ // Create the program
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderSourceCode = nullptr;
				const char *fragmentShaderSourceCode = nullptr;
				#include "FirstTriangle_GLSL_110.h"
				#include "FirstTriangle_GLSL_ES2.h"
				#include "FirstTriangle_HLSL_D3D9_D3D10_D3D11.h"
				#include "FirstTriangle_Null.h"

				// Create the vertex shader
				Renderer::IVertexShader *vertexShader = shaderLanguage->createVertexShader(vertexShaderSourceCode);
				RENDERER_SET_RESOURCE_DEBUG_NAME(vertexShader, "Triangle VS")

				// Create the fragment shader
				Renderer::IFragmentShader *fragmentShader = shaderLanguage->createFragmentShader(fragmentShaderSourceCode);
				RENDERER_SET_RESOURCE_DEBUG_NAME(fragmentShader, "Triangle FS")

				// Create the program
				mProgram = shaderLanguage->createProgram(vertexShader, fragmentShader);
				RENDERER_SET_RESOURCE_DEBUG_NAME(mProgram, "Triangle program")
			}

			// Is there a valid program?
			if (nullptr != mProgram)
			{
				// Create the vertex buffer object (VBO)
				// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
				static const float VERTEX_POSITION[] =
				{					// Vertex ID	Triangle on screen
					 0.0f, 1.0f,	// 0				0
					 1.0f, 0.0f,	// 1			   .   .
					-0.5f, 0.0f		// 2			  2.......1
				};
				Renderer::IVertexBufferPtr vertexBuffer(renderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));
				RENDERER_SET_RESOURCE_DEBUG_NAME(vertexBuffer, "Triangle VBO")

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
				mVertexArray = mProgram->createVertexArray(sizeof(vertexArray) / sizeof(Renderer::VertexArrayAttribute), vertexArray);
				RENDERER_SET_RESOURCE_DEBUG_NAME(mVertexArray, "Triangle VAO")
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstTriangle::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Release the used resources
	mVertexArray = nullptr;
	mProgram = nullptr;

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())

	// Call the base implementation
	IApplicationRenderer::onDeinitialization();
}

void FirstTriangle::onDraw()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && nullptr != mProgram)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Clear the color buffer of the current render target with gray, do also clear the depth buffer
		renderer->clear(Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

		// Begin scene rendering
		// -> Required for Direct3D 9
		// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
		if (renderer->beginScene())
		{
			// Set the used program
			renderer->setProgram(mProgram);

			{ // Setup input assembly (IA)
				// Set the used vertex array
				renderer->iaSetVertexArray(mVertexArray);

				// Set the primitive topology used for draw calls
				renderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_LIST);
			}

			// Set debug marker
			// -> Debug methods: When using Direct3D <11.1, these methods map to the Direct3D 9 PIX functions
			//    (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)
			RENDERER_SET_DEBUG_MARKER(renderer, L"Everyone ready for the upcoming triangle?")

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT(renderer, L"Drawing the fancy triangle")

			// Render the specified geometric primitive, based on an array of vertices
			renderer->draw(0, 3);

			// End debug event
			RENDERER_END_DEBUG_EVENT(renderer)

			// End scene rendering
			// -> Required for Direct3D 9
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			renderer->endScene();
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}
