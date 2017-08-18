/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
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
//[ Shader start                                          ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_VULKAN
if (0 == strcmp(renderer->getName(), "Vulkan"))
{


//[-------------------------------------------------------]
//[ Vertex shader source code                             ]
//[-------------------------------------------------------]
// One vertex shader invocation per vertex
vertexShaderSourceCode = R"(#version 450 core	// OpenGL 4.5
#extension GL_ARB_separate_shader_objects : enable	// The "GL_ARB_separate_shader_objects"-extension is required for Vulkan shaders to work

// Attribute input/output
layout(location = 0) in float Position;	// Dummy
layout(location = 0) out gl_PerVertex
{
	vec4 gl_Position;
};

// Programs
void main()
{
	// Pass through the dummy
	gl_Position = vec4(Position, 0.0, 0.0, 1.0);
}
)";


//[-------------------------------------------------------]
//[ Geometry shader source code                           ]
//[-------------------------------------------------------]
// One geometry shader invocation per primitive
geometryShaderSourceCode = R"(#version 450 core	// OpenGL 4.5
#extension GL_ARB_separate_shader_objects : enable	// The "GL_ARB_separate_shader_objects"-extension is required for Vulkan shaders to work

// Attribute input/output
layout(points) in;
layout(triangle_strip, max_vertices = 3) out;
layout(location = 0) out gl_PerVertex
{
	vec4 gl_Position;
};

// Programs
void main()
{
	//					Vertex ID	Triangle on screen
	//  0.0f, -1.0f,	// 0			0
	//  1.0f,  0.0f,	// 1		   .   .
	// -0.5f,  0.0f		// 2		  2.......1

	// Emit vertex 0 clip space position, left/bottom is (-1,-1) and right/top is (1,1)
	gl_Position = vec4(0.0, 1.0, 0.0, 1.0);
	EmitVertex();

	// Emit vertex 1 clip space position, left/bottom is (-1,-1) and right/top is (1,1)
	gl_Position = vec4(1.0, 0.0, 0.0, 1.0);
	EmitVertex();

	// Emit vertex 2 clip space position, left/bottom is (-1,-1) and right/top is (1,1)
	gl_Position = vec4(-0.5, 0.0, 0.0, 1.0);
	EmitVertex();

	// Done
	EndPrimitive();
}
)";


//[-------------------------------------------------------]
//[ Fragment shader source code                           ]
//[-------------------------------------------------------]
// One fragment shader invocation per fragment
fragmentShaderSourceCode = R"(#version 450 core	// OpenGL 4.5
#extension GL_ARB_separate_shader_objects : enable	// The "GL_ARB_separate_shader_objects"-extension is required for Vulkan shaders to work

// Attribute input/output
layout(location = 0, index = 0) out vec4 Color0;

// Programs
void main()
{
	// Return white
	Color0 = vec4(1.0, 1.0, 1.0, 1.0);
}
)";


//[-------------------------------------------------------]
//[ Shader end                                            ]
//[-------------------------------------------------------]
}
else
#endif
