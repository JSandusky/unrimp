/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Shader start                                          ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_OPENGL
if (0 == strcmp(mRenderer->getName(), "OpenGL"))
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
"#version 110\n"	// OpenGL 2.0
STRINGIFY(
// Attribute input/output
attribute vec3 Position;	// Object space vertex position as input, left/bottom is (0,0) and right/top is (1,1)
varying vec2 TexCoord;		// Normalized texture coordinate as output

// Uniforms
uniform vec4 GlyphSizePenPosition;			// Object space glyph size (xy) and object space pen position (zw) => scale & bias
uniform vec4 TextureCoordinateMinMax;		// The normalized minimum (xy) and maximum (zw) glyph texture coordinate inside the glyph texture atlas
uniform mat4 ObjectSpaceToClipSpaceMatrix;	// Object space to clip space matrix

// Programs
void main()
{
	// Calculate the object space vertex position
	vec4 position = vec4(Position.xy * GlyphSizePenPosition.xy + GlyphSizePenPosition.zw, 0.0, 1.0);

	// Calculate the clip space vertex position, lower/left is (-1,-1) and upper/right is (1,1)
	gl_Position = ObjectSpaceToClipSpaceMatrix * position;

	// Set the normalized vertex texture coordinate
	// Vertex ID	Triangle strip on screen
	// 0			1.......3
	// 1			.   .   .
	// 2			0.......2
	// 3
	if (int(Position.z) == 0)
	{
		TexCoord = vec2(TextureCoordinateMinMax.x, TextureCoordinateMinMax.w);
	}
	else if (int(Position.z) == 1)
	{
		TexCoord = TextureCoordinateMinMax.xy;
	}
	else if (int(Position.z) == 2)
	{
		TexCoord = TextureCoordinateMinMax.zw;
	}
	else if (int(Position.z) == 3)
	{
		TexCoord = vec2(TextureCoordinateMinMax.z, TextureCoordinateMinMax.y);
	}
}
);	// STRINGIFY


//[-------------------------------------------------------]
//[ Fragment shader source code                           ]
//[-------------------------------------------------------]
// One fragment shader invocation per fragment
fragmentShaderSourceCode =
"#version 110\n"	// OpenGL 2.0
STRINGIFY(
// Attribute input/output
varying vec2 TexCoord;	// Normalized texture coordinate as input

// Uniforms
uniform sampler2D GlyphMap;	// Glyph atlas texture map
uniform vec4	  Color;	// Text color

// Programs
void main()
{
	// Fetch the texel at the given texture coordinate and return it's color
	gl_FragColor = vec4(Color.r, Color.g, Color.b, texture2D(GlyphMap, TexCoord).r * Color.a);
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
