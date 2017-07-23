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
if (0 == strcmp(renderer.getName(), "Vulkan"))
{


//[-------------------------------------------------------]
//[ Vertex shader source code                             ]
//[-------------------------------------------------------]
// One vertex shader invocation per vertex
vertexShaderSourceCode = R"(#version 450 core	// OpenGL 4.5
#extension GL_ARB_separate_shader_objects : enable	// The "GL_ARB_separate_shader_objects"-extension is required for Vulkan shaders to work

// Attribute input/output
layout(location = 0) in vec2 Position;	// Object space vertex position as input, left/bottom is (0,0) and right/top is (1,1)
layout(location = 0) out gl_PerVertex
{
	vec4 gl_Position;
};
layout(location = 1) in  vec2 TexCoord;		// Normalized texture coordinate as input
layout(location = 1) out vec2 TexCoordVs;	// Normalized texture coordinate as output
layout(location = 2) in  vec4 Color;		// Color as input
layout(location = 2) out vec4 ColorVs;		// Color as output

// Uniforms
layout(std140, binding = 0) uniform UniformBlockDynamicVs
{
	mat4 ObjectSpaceToClipSpaceMatrix;
};

// Programs
void main()
{
	// Calculate the clip space vertex position, lower/left is (-1,-1) and upper/right is (1,1)
	// -> Compensate for different Vulkan coordinate system
	gl_Position = ObjectSpaceToClipSpaceMatrix * vec4(Position, 0.0, 1.0);
	gl_Position.y = -gl_Position.y;

	// Pass through the vertex texture coordinate
	TexCoordVs = TexCoord;

	// Pass through the vertex color
	ColorVs = Color;
}
)";


//[-------------------------------------------------------]
//[ Fragment shader source code                           ]
//[-------------------------------------------------------]
// One fragment shader invocation per fragment
fragmentShaderSourceCode = R"(#version 450 core	// OpenGL 4.5
#extension GL_ARB_separate_shader_objects : enable	// The "GL_ARB_separate_shader_objects"-extension is required for Vulkan shaders to work

// Attribute input/output
layout(location = 1) in vec2 TexCoordVs;	// Normalized texture coordinate as input
layout(location = 2) in vec4 ColorVs;
layout(location = 0) out vec4 OutFragmentColor;

// Uniforms
layout(binding = 2) uniform sampler2D GlyphMap;	// Glyph atlas texture map

// Programs
void main()
{
	// Fetch the texel at the given texture coordinate and return its color
	OutFragmentColor = ColorVs * texture(GlyphMap, TexCoordVs).rrrr;
}
)";


//[-------------------------------------------------------]
//[ Shader end                                            ]
//[-------------------------------------------------------]
}
else
#endif