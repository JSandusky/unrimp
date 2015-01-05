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
#ifndef RENDERER_NO_OPENGLES2
if (0 == strcmp(renderer->getName(), "OpenGLES2"))
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
"#version 100\n"	// OpenGL ES 2.0
STRINGIFY(
// Attribute input/output
attribute highp vec3 Position;		// Object space vertex position
attribute highp vec2 TexCoord;		// 16 bit texture coordinate
varying   highp vec2 TexCoordVs;	// Texture coordinate
attribute highp vec4 QTangent;		// 16 bit QTangent
varying   vec3 TangentVs;			// Tangent space to view space, x-axis
varying   vec3 BinormalVs;			// Tangent space to view space, y-axis
varying   vec3 NormalVs;			// Tangent space to view space, z-axis

// Uniforms
uniform highp mat4 ObjectSpaceToClipSpaceMatrix;	// Object space to clip space matrix
uniform highp mat3 ObjectSpaceToViewSpaceMatrix;	// Object space to view space matrix

// Functions
mat3 GetTangentFrame(mat3 objectSpaceToViewSpaceMatrix, vec4 q)
{
	// Quaternion to tangent bitangent
	vec3 tBt0 = vec3(1.0 - 2.0*(q.y*q.y + q.z*q.z),	2.0*(q.x*q.y + q.w*q.z),		2.0*(q.x*q.z - q.w*q.y));
	vec3 tBt1 = vec3(2.0*(q.x*q.y - q.w*q.z),		1.0 - 2.0*(q.x*q.x + q.z*q.z),	2.0*(q.y*q.z + q.w*q.x));

	vec3 t = objectSpaceToViewSpaceMatrix * tBt0;
	vec3 b = objectSpaceToViewSpaceMatrix * tBt1;

	return mat3(
		t,
		b,
		cross(t, b) * ((q.w < 0.0) ? -1.0 : 1.0)
	);
}

// Programs
void main()
{
	// Calculate the clip space vertex position, left/bottom is (-1,-1) and right/top is (1,1)
	gl_Position = ObjectSpaceToClipSpaceMatrix * vec4(Position, 1.0);

	// Pass through the decoded 16 bit texture coordinate
	TexCoordVs = TexCoord / 32767.0;

	// Calculate the tangent space to view space tangent, binormal and normal
	// - 16 bit QTangent basing on http://dev.theomader.com/qtangents/ "QTangents" which is basing on
	//   http://www.crytek.com/cryengine/presentations/spherical-skinning-with-dual-quaternions-and-qtangents "Spherical Skinning with Dual-Quaternions and QTangents"
	mat3 tangentFrame = GetTangentFrame(ObjectSpaceToViewSpaceMatrix, QTangent / 32767.0);
	TangentVs = tangentFrame[0];
	BinormalVs = tangentFrame[1];
	NormalVs = tangentFrame[2];
}
);	// STRINGIFY


//[-------------------------------------------------------]
//[ Fragment shader source code                           ]
//[-------------------------------------------------------]
// One fragment shader invocation per fragment
fragmentShaderSourceCode =
"#version 100\n"	// OpenGL ES 2.0
STRINGIFY(
// Attribute input/output
varying mediump vec2 TexCoordVs;	// Texture coordinate
varying mediump vec3 TangentVs;		// Tangent space to view space, x-axis
varying mediump vec3 BinormalVs;	// Tangent space to view space, y-axis
varying mediump vec3 NormalVs;		// Tangent space to view space, z-axis

// Uniforms
uniform mediump sampler2D DiffuseMap;
uniform mediump sampler2D EmissiveMap;
uniform mediump sampler2D NormalMap;	// Tangent space normal map
uniform mediump sampler2D SpecularMap;

// Programs
void main()
{
	// Build in variables
	mediump vec3 ViewSpaceLightDirection = normalize(vec3(0.5, 0.5, 1.0));	// View space light direction
	mediump vec3 ViewSpaceViewVector     = vec3(0.0, 0.0, 1.0);				// In view space, we always look along the positive z-axis

	// Get the per fragment normal [0..1] by using a tangent space normal map
	mediump vec3 normal = texture2D(NormalMap, TexCoordVs).rgb;

	// Transform the normal from [0..1] to [-1..1]
	normal = (normal - 0.5) * 2.0;

	// Transform the tangent space normal into view space
	normal = normalize(normal.x * TangentVs + normal.y * BinormalVs + normal.z * NormalVs);

	// Perform standard Blinn-Phong diffuse and specular lighting

	// Calculate the diffuse lighting
	mediump float diffuseLight = max(dot(normal, ViewSpaceLightDirection), 0.0);

	// Calculate the specular lighting
	mediump vec3 viewSpaceHalfVector = normalize(ViewSpaceLightDirection + ViewSpaceViewVector);
	mediump float specularLight = (diffuseLight > 0.0) ? pow(max(dot(normal, viewSpaceHalfVector), 0.0), 128.0) : 0.0;

	// Calculate the fragment color
	mediump vec4 color = diffuseLight * texture2D(DiffuseMap, TexCoordVs);	// Diffuse term
	color.rgb += specularLight * texture2D(SpecularMap, TexCoordVs).rgb;	// Specular term
	color.rgb += texture2D(EmissiveMap, TexCoordVs).rgb;					// Emissive term

	// Done
	gl_FragColor = min(color, vec4(1.0, 1.0, 1.0, 1.0));
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