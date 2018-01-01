/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#if !defined(RENDERER_NO_DIRECT3D10) || !defined(RENDERER_NO_DIRECT3D11) || !defined(RENDERER_NO_DIRECT3D12)
if (0 == strcmp(renderer.getName(), "Direct3D10") || 0 == strcmp(renderer.getName(), "Direct3D11") || 0 == strcmp(renderer.getName(), "Direct3D12"))
{


//[-------------------------------------------------------]
//[ Vertex shader source code                             ]
//[-------------------------------------------------------]
// One vertex shader invocation per vertex
vertexShaderSourceCode = R"(
// Attribute input/output
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;	// Clip space vertex position as output, left/bottom is (-1,-1) and right/top is (1,1)
	float2 TexCoord : TEXCOORD0;	// Normalized texture coordinate as output
	float4 Color    : COLOR0;
};

// Uniforms
cbuffer UniformBlockDynamicVs : register(b0)
{
	float4x4 ObjectSpaceToClipSpaceMatrix;
}

// Programs
VS_OUTPUT main(float2 Position : POSITION,	// Clip space vertex position as input, left/bottom is (-1,-1) and right/top is (1,1)
			   float2 TexCoord : TEXCOORD0,	// Normalized texture coordinate as input
			   float4 Color    : COLOR0)
{
	VS_OUTPUT output;

	// Calculate the clip space vertex position, lower/left is (-1,-1) and upper/right is (1,1)
	output.Position = mul(ObjectSpaceToClipSpaceMatrix, float4(Position, 0.0f, 1.0f));

	// Pass through the vertex texture coordinate
	output.TexCoord = TexCoord;

	// Pass through the vertex color
	output.Color = Color;

	// Done
	return output;
}
)";


//[-------------------------------------------------------]
//[ Fragment shader source code                           ]
//[-------------------------------------------------------]
// One fragment shader invocation per fragment
// "pixel shader" in Direct3D terminology
fragmentShaderSourceCode = R"(
// Uniforms
SamplerState SamplerLinear : register(s0);
Texture2D GlyphMap : register(t0);	// Glyph atlas texture map

// Programs
float4 main(float4 Position : SV_POSITION, float2 TexCoord : TEXCOORD0, float4 Color : COLOR0) : SV_TARGET
{
	// Fetch the texel at the given texture coordinate and return its color
	return Color * GlyphMap.Sample(SamplerLinear, TexCoord).rrrr;
}
)";


//[-------------------------------------------------------]
//[ Shader end                                            ]
//[-------------------------------------------------------]
}
else
#endif
