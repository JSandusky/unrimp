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
#include "FirstGeometryShader/FirstGeometryShader.h"
#include "Framework/Color4.h"

#include <string.h> // for strcmp()


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstGeometryShader::FirstGeometryShader(const char *rendererName) :
	IApplicationRenderer(rendererName)
{
	// Nothing to do in here
}

FirstGeometryShader::~FirstGeometryShader()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstGeometryShader::onInitialization()
{
	// Call the base implementation
	IApplicationRenderer::onInitialization();

	// Get and check the renderer instance
	// -> Geometry shaders supported?
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && renderer->getCapabilities().maximumNumberOfGsOutputVertices)
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
				const char *geometryShaderSourceCode = nullptr;
				const char *fragmentShaderSourceCode = nullptr;
				#include "FirstGeometryShader_GLSL_330.h"
				#include "FirstGeometryShader_HLSL_D3D10_D3D11.h"
				#include "FirstGeometryShader_Null.h"

				// Create the program
				mProgram = shaderLanguage->createProgram(
					shaderLanguage->createVertexShader(vertexShaderSourceCode),
					shaderLanguage->createGeometryShader(geometryShaderSourceCode, Renderer::GsInputPrimitiveTopology::POINTS, Renderer::GsOutputPrimitiveTopology::TRIANGLE_STRIP, 3),
					shaderLanguage->createFragmentShader(fragmentShaderSourceCode));
			}

			// TODO(co) Attribute less rendering (aka "drawing without data") possible with OpenGL? For me it appears not to work, I see nothing and also get no error...
			// -> Tested with: "Radeon HD 6970M", driver "catalyst_12-7_beta_windows7_20120629.exe"
			// -> According to http://renderingpipeline.com/2012/03/are-vertex-shaders-obsolete/ it should work
			// -> Apparently there are currently some issues when using this approach: http://www.opengl.org/discussion_boards/showthread.php/177372-Rendering-simple-shapes-without-passing-vertices
			if (nullptr != mProgram && 0 == strcmp(renderer->getName(), "OpenGL"))
			{
				// Create the vertex buffer object (VBO)
				static const float VERTEX_POSITION[] =
				{			// Vertex ID
					42.0f	// 0
				};
				Renderer::IVertexBufferPtr vertexBuffer(renderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

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
						Renderer::VertexArrayFormat::FLOAT_1,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
						"Position",								// name[64] (char)
						"POSITION",								// semantic[64] (char)
						0,										// semanticIndex (unsigned int)
						// Data source
						vertexBuffer,							// vertexBuffer (Renderer::IVertexBuffer *)
						0,										// offset (unsigned int)
						sizeof(float),							// stride (unsigned int)
						// Data source, instancing part
						0										// instancesPerElement (unsigned int)
					}
				};
				mVertexArray = mProgram->createVertexArray(sizeof(vertexArray) / sizeof(Renderer::VertexArrayAttribute), vertexArray);
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstGeometryShader::onDeinitialization()
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

void FirstGeometryShader::onDraw()
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
				if (nullptr != mVertexArray)
				{
					renderer->iaSetVertexArray(mVertexArray);
				}

				// Set the primitive topology used for draw calls
				renderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::POINT_LIST);
			}

			// Render the specified geometric primitive, based on an array of vertices
			// -> Emit a single point in order to generate a draw call, the geometry shader does the rest
			// -> Attribute less rendering (aka "drawing without data")
			renderer->draw(0, 1);

			// End scene rendering
			// -> Required for Direct3D 9
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			renderer->endScene();
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}
