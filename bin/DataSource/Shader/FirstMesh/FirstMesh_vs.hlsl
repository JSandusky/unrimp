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
//[ Vertex shader source code                             ]
//[-------------------------------------------------------]
// Attribute input/output
struct VS_INPUT
{
	float3 Position : POSITION;		// Object space vertex position
	float2   TexCoord : TEXCOORD0;	// 16 bit texture coordinate
	float4   QTangent : NORMAL;		// 16 bit QTangent
};
struct VS_OUTPUT
{
	float4   Position     : SV_POSITION;	// Clip space vertex position as output, left/bottom is (-1,-1) and right/top is (1,1)
	float2   TexCoord     : TEXCOORD0;		// Texture coordinate
	float3x3 TangentFrame : TEXCOORD1;		// Tangent frame
};

// Uniforms
cbuffer UniformBlockDynamicVs : register(b0)
{
	float4x4 ObjectSpaceToClipSpaceMatrix;	// Object space to clip space matrix
	float4x4 ObjectSpaceToViewSpaceMatrix;	// Object space to view space matrix
}

// Functions
float2x3 QuaternionToTangentBitangent(float4 q)
{
	return float2x3(
		1 - 2*(q.y*q.y + q.z*q.z),	2*(q.x*q.y + q.w*q.z),		2*(q.x*q.z - q.w*q.y),
		2*(q.x*q.y - q.w*q.z),		1 - 2*(q.x*q.x + q.z*q.z),	2*(q.y*q.z + q.w*q.x)
	);
}

float3x3 GetTangentFrame(float3x3 objectSpaceToViewSpaceMatrix, float4 qtangent)
{
	float2x3 tBt = QuaternionToTangentBitangent(qtangent);
	float3 t = mul(objectSpaceToViewSpaceMatrix, tBt[0]);
	float3 b = mul(objectSpaceToViewSpaceMatrix, tBt[1]);

	return float3x3(
		t,
		b,
		cross(t, b) * (qtangent.w < 0 ? -1 : 1)
	);
}

// Programs
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	// Calculate the clip space vertex position, left/bottom is (-1,-1) and right/top is (1,1)
	output.Position = mul(ObjectSpaceToClipSpaceMatrix, float4(input.Position, 1.0f));

	// Pass through the decoded 16 bit texture coordinate
	output.TexCoord = input.TexCoord / 32767.0f;

	// Calculate the tangent space to view space tangent, binormal and normal
	// - 16 bit QTangent basing on http://dev.theomader.com/qtangents/ "QTangents" which is basing on
	//   http://www.crytek.com/cryengine/presentations/spherical-skinning-with-dual-quaternions-and-qtangents "Spherical Skinning with Dual-Quaternions and QTangents"
	// TODO(co) float3x3
	output.TangentFrame = GetTangentFrame((float3x3)ObjectSpaceToViewSpaceMatrix, input.QTangent / 32767.0f);

	// Done
	return output;
}
