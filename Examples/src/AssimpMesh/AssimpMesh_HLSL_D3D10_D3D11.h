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
//[ Shader start                                          ]
//[-------------------------------------------------------]
#if !defined(RENDERER_NO_DIRECT3D10) || !defined(RENDERER_NO_DIRECT3D11)
if (0 == strcmp(renderer->getName(), "Direct3D10") || 0 == strcmp(renderer->getName(), "Direct3D11"))
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
struct VS_INPUT
{
	float3 Position : POSITION;		// Object space vertex position
	float2 TexCoord : TEXCOORD0;	// Texture coordinate
	float3 Tangent  : TEXCOORD1;	// Object space to tangent space, x-axis
	float3 Binormal : TEXCOORD2;	// Object space to tangent space, y-axis
	float3 Normal   : NORMAL;		// Object space to tangent space, z-axis
};
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;	// Clip space vertex position as output, left/bottom is (-1,-1) and right/top is (1,1)
	float2 TexCoord : TEXCOORD0;	// Texture coordinate
	float3 Tangent  : TANGENT;		// Tangent space to view space, x-axis
	float3 Binormal : BINORMAL;		// Tangent space to view space, y-axis
	float3 Normal   : NORMAL;		// Tangent space to view space, z-axis
};

// Uniforms
cbuffer UniformBlockDynamicVs : register(b0)
{
	float4x4 ObjectSpaceToClipSpaceMatrix;	// Object space to clip space matrix
	float4x4 ObjectSpaceToViewSpaceMatrix;	// Object space to view space matrix
}

// Programs
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	// Calculate the clip space vertex position, left/bottom is (-1,-1) and right/top is (1,1)
	output.Position = mul(ObjectSpaceToClipSpaceMatrix, float4(input.Position, 1.0f));

	// Pass through the texture coordinate
	output.TexCoord = input.TexCoord;

	// Calculate the tangent space to view space tangent, binormal and normal
	// TODO(co) float3x3
	output.Binormal = mul(ObjectSpaceToViewSpaceMatrix, float4(input.Binormal, 1.0f)).xyz;
	output.Tangent  = mul(ObjectSpaceToViewSpaceMatrix, float4(input.Tangent,  1.0f)).xyz;
	output.Normal   = mul(ObjectSpaceToViewSpaceMatrix, float4(input.Normal,   1.0f)).xyz;

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
// Attribute input/output
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;	// Clip space vertex position as output, left/bottom is (-1,-1) and right/top is (1,1)
	float2 TexCoord : TEXCOORD0;	// Texture coordinate
	float3 Tangent  : TANGENT;		// Tangent space to view space, x-axis
	float3 Binormal : BINORMAL;		// Tangent space to view space, y-axis
	float3 Normal   : NORMAL;		// Tangent space to view space, z-axis
};

// Uniforms
Texture2D DiffuseMap  : register(t0);
Texture2D EmissiveMap : register(t1);
Texture2D NormalMap   : register(t2);	// Tangent space normal map
Texture2D SpecularMap : register(t3);
SamplerState SamplerLinear : register(s0);

// Programs
float4 main(VS_OUTPUT input) : SV_Target
{
	// Build in variables
	float3 ViewSpaceLightDirection = normalize(float3(0.5f, 0.5f, 1.0f));	// View space light direction
	float3 ViewSpaceViewVector     = float3(0.0f, 0.0f, 1.0f);				// In view space, we always look along the positive z-axis

	// Get the per fragment normal [0..1] by using a tangent space normal map
	float3 normal = NormalMap.Sample(SamplerLinear, input.TexCoord).rgb;

	// Transform the normal from [0..1] to [-1..1]
	normal = (normal - 0.5f) * 2.0f;

	// Transform the tangent space normal into view space
	normal = normalize(normal.x * input.Tangent + normal.y * input.Binormal + normal.z * input.Normal);

	// Perform standard Blinn-Phong diffuse and specular lighting

	// Calculate the diffuse lighting
	float diffuseLight = max(dot(normal, ViewSpaceLightDirection), 0.0f);

	// Calculate the specular lighting
	float3 viewSpaceHalfVector = normalize(ViewSpaceLightDirection + ViewSpaceViewVector);
	float specularLight = (diffuseLight > 0.0f) ? pow(max(dot(normal, viewSpaceHalfVector), 0.0f), 128.0f) : 0.0f;

	// Calculate the fragment color
	float4 color = diffuseLight * DiffuseMap.Sample(SamplerLinear, input.TexCoord);		// Diffuse term
	color.rgb += specularLight * SpecularMap.Sample(SamplerLinear, input.TexCoord).rgb;	// Specular term
	color.rgb += EmissiveMap.Sample(SamplerLinear, input.TexCoord).rgb;					// Emissive term

	// Done
	return min(color, float4(1.0f, 1.0f, 1.0f, 1.0f));
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
