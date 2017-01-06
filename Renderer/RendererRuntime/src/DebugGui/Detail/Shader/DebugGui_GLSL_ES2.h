/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#ifndef RENDERER_NO_OPENGLES2
if (0 == strcmp(renderer.getName(), "OpenGLES2"))
{


//[-------------------------------------------------------]
//[ Define helper macro                                   ]
//[-------------------------------------------------------]
#define STRINGIFY(ME) #ME


//[-------------------------------------------------------]
//[ Vertex shader source code                             ]
//[-------------------------------------------------------]
// One vertex shader invocation per vertex
vertexShaderSourceCode =
"#version 300 es\n"	// OpenGL ES 3.0
STRINGIFY(
// Attribute input/output
in highp vec2 Position;		// Object space vertex position as input, left/bottom is (0,0) and right/top is (1,1)
in highp vec2 TexCoord;		// Normalized texture coordinate as input
out   highp vec2 TexCoordVs;	// Normalized texture coordinate as output
in highp vec4 Color;			// Color as input
out   highp vec4 ColorVs;		// Color as output

// Uniforms
uniform highp mat4 ObjectSpaceToClipSpaceMatrix;

// Programs
void main()
{
	// Calculate the clip space vertex position, lower/left is (-1,-1) and upper/right is (1,1)
	gl_Position = ObjectSpaceToClipSpaceMatrix * vec4(Position, 0.0f, 1.0f);

	// Pass through the vertex texture coordinate
	TexCoordVs = TexCoord;

	// Pass through the vertex color
	ColorVs = Color;
}
);	// STRINGIFY


//[-------------------------------------------------------]
//[ Fragment shader source code                           ]
//[-------------------------------------------------------]
// One fragment shader invocation per fragment
fragmentShaderSourceCode =
"#version 300 es\n"	// OpenGL ES 3.0
STRINGIFY(
// Attribute input/output
in mediump vec2 TexCoordVs;	// Normalized texture coordinate as input
in mediump vec4 ColorVs;

out highp vec4 FragmentColor;	// Output variable for fragment color

// Uniforms
uniform mediump sampler2D GlyphMap;	// Glyph atlas texture map

// Programs
void main()
{
	// Fetch the texel at the given texture coordinate and return it's color
	FragmentColor = ColorVs * texture(GlyphMap, TexCoordVs).aaaa;
}
);	// STRINGIFY


//[-------------------------------------------------------]
//[ Undefine helper macro                                 ]
//[-------------------------------------------------------]
#undef STRINGIFY


//[-------------------------------------------------------]
//[ Shader end                                            ]
//[-------------------------------------------------------]
}
else
#endif
