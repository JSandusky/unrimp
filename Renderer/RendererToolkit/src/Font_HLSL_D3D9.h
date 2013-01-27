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
//[ Shader start                                          ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_DIRECT3D9
if (0 == strcmp(mRenderer->getName(), "Direct3D9"))
{


//[-------------------------------------------------------]
//[ Define helper macro                                   ]
//[-------------------------------------------------------]
#define STRINGIFY(ME) #ME


//[-------------------------------------------------------]
//[ Vertex shader source code                             ]
//[-------------------------------------------------------]
// One vertex shader invocation per vertex
vertexShaderSourceCode = STRINGIFY(
// Attribute input/output
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;	// Clip space vertex position as output, left/bottom is (-1,-1) and right/top is (1,1)
	float2 TexCoord : TEXCOORD0;	// Normalized texture coordinate as output
};

// Uniforms
uniform float4   GlyphSizePenPosition;			// Object space glyph size (xy) and object space pen position (zw) => scale & bias
uniform float4   TextureCoordinateMinMax;		// The normalized minimum (xy) and maximum (zw) glyph texture coordinate inside the glyph texture atlas
uniform float4x4 ObjectSpaceToClipSpaceMatrix;	// Object space to clip space matrix

// Programs
VS_OUTPUT main(float3 Position : POSITION)	// Clip space vertex position as input, left/bottom is (-1,-1) and right/top is (1,1)
{
	VS_OUTPUT output;

	// Calculate the object space vertex position
	output.Position = float4(Position.xy * GlyphSizePenPosition.xy + GlyphSizePenPosition.zw, 0.0f, 1.0f);

	// Calculate the clip space vertex position, lower/left is (-1,-1) and upper/right is (1,1)
	output.Position = mul(ObjectSpaceToClipSpaceMatrix, output.Position);

	// Set the normalized vertex texture coordinate
	// Vertex ID	Triangle strip on screen
	// 0			1.......3
	// 1			.   .   .
	// 2			0.......2
	// 3
	if (int(Position.z) == 0)
	{
		output.TexCoord = float2(TextureCoordinateMinMax.x, TextureCoordinateMinMax.w);
	}
	else if (int(Position.z) == 1)
	{
		output.TexCoord = TextureCoordinateMinMax.xy;
	}
	else if (int(Position.z) == 2)
	{
		output.TexCoord = TextureCoordinateMinMax.zw;
	}
	else if (int(Position.z) == 3)
	{
		output.TexCoord = float2(TextureCoordinateMinMax.z, TextureCoordinateMinMax.y);
	}

	// Done
	return output;
}
);	// STRINGIFY


//[-------------------------------------------------------]
//[ Fragment shader source code                           ]
//[-------------------------------------------------------]
// One fragment shader invocation per fragment
// "pixel shader" in Direct3D terminology
fragmentShaderSourceCode = STRINGIFY(
// Uniforms
uniform sampler2D GlyphMap : register(s0);	// Glyph atlas texture map
uniform float4	  Color;	// Text color

// Programs
float4 main(float4 Position : SV_POSITION, float2 TexCoord : TEXCOORD0) : SV_Target
{
	// Fetch the texel at the given texture coordinate and return it's color
	return float4(Color.r, Color.g, Color.b, tex2D(GlyphMap, TexCoord).a * Color.a);
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
