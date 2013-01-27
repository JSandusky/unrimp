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
#ifndef RENDERER_NO_CG
if (0 == strcmp(shaderLanguage->getShaderLanguageName(), "Cg"))
{


//[-------------------------------------------------------]
//[ Define helper macro                                   ]
//[-------------------------------------------------------]
#define STRINGIFY(ME) #ME


//[-------------------------------------------------------]
//[ Vertex shader source code                             ]
//[-------------------------------------------------------]
// One vertex shader invocation per vertex
// Get the proper version of the vertex shader
#if !defined(RENDERER_NO_OPENGL) || !defined(RENDERER_NO_OPENGLES2)
	if (0 == strncmp(shaderLanguage->getRenderer().getName(), "OpenGL", 6))
	{
		// OpenGL version
		static const char *vertexShaderSourceCode_OpenGL = STRINGIFY(
		// Attribute input/output
		struct VS_OUTPUT
		{
			float4 Position : POSITION;		// Clip space vertex position as output, left/bottom is (-1,-1) and right/top is (1,1)
			float2 TexCoord : TEXCOORD0;	// Normalized texture coordinate as output
		};

		// Programs
		VS_OUTPUT main(float2 Position : POSITION)	// Clip space vertex position as input, left/bottom is (-1,-1) and right/top is (1,1)
		{
			VS_OUTPUT output;

			// Pass through the clip space vertex position, left/bottom is (-1,-1) and right/top is (1,1)
			output.Position = float4(Position, 0.0f, 1.0f);

			// Calculate the texture coordinate by mapping the clip space coordinate to a texture space coordinate
			// -> In OpenGL ES 2, the texture origin is left/bottom which maps well to clip space coordinates
			// -> (-1,-1) -> (0,0)
			// -> (1,1) -> (1,1)
			output.TexCoord = Position.xy * 0.5f + 0.5f;

			// Done
			return output;
		}
		);	// STRINGIFY
		vertexShaderSourceCode = vertexShaderSourceCode_OpenGL;
	}
else
#endif
#if !defined(RENDERER_NO_DIRECT3D9) || !defined(RENDERER_NO_DIRECT3D10) || !defined(RENDERER_NO_DIRECT3D11)
	if (0 == strncmp(shaderLanguage->getRenderer().getName(), "Direct3D", 7))
	{
		// Direct3D version
		static const char *vertexShaderSourceCode_Direct3D = STRINGIFY(
		// Attribute input/output
		struct VS_OUTPUT
		{
			float4 Position : POSITION;		// Clip space vertex position as output, left/bottom is (-1,-1) and right/top is (1,1)
			float2 TexCoord : TEXCOORD0;	// Normalized texture coordinate as output
		};

		// Programs
		VS_OUTPUT main(float2 Position : POSITION)	// Clip space vertex position as input, left/bottom is (-1,-1) and right/top is (1,1)
		{
			VS_OUTPUT output;

			// Pass through the clip space vertex position, left/bottom is (-1,-1) and right/top is (1,1)
			output.Position = float4(Position, 0.0f, 1.0f);

			// Calculate the texture coordinate
			// -> Unlike OpenGL or OpenGL ES 2, in Direct3D 9 & 10 & 11 the texture origin is left/top which does not map well to clip space coordinates
			// -> We have to flip the y-axis to map the coordinate system to the Direct3D 9 & 10 & 11 texture coordinate system
			// -> (-1,-1) -> (0,1)
			// -> (1,1) -> (1,0)
			output.TexCoord = float2(Position.x * 0.5f + 0.5f, 1.0f - (Position.y * 0.5f + 0.5f));

			// Done
			return output;
		}
		);	// STRINGIFY
		vertexShaderSourceCode = vertexShaderSourceCode_Direct3D;
	}
#endif
{
	// Error! (unsupported renderer)
	OUTPUT_DEBUG_STRING("Error: Unsupported renderer\n")
}


//[-------------------------------------------------------]
//[ Fragment shader source code                           ]
//[-------------------------------------------------------]
// One fragment shader invocation per fragment
fragmentShaderSourceCode_MultipleRenderTargets = STRINGIFY(
// Attribute output
struct FS_OUTPUT
{
	float4 Color0 : COLOR0;
	float4 Color1 : COLOR1;
};

// Programs
FS_OUTPUT main(float4 Position : POSITION, float2 TexCoord : TEXCOORD0)
{
	FS_OUTPUT ouput;
	ouput.Color0 = float4(1.0f, 0.0f, 0.0f, 0.0f);	// Red
	ouput.Color1 = float4(0.0f, 0.0f, 1.0f, 0.0f);	// Blue
	return ouput;
}
);	// STRINGIFY


//[-------------------------------------------------------]
//[ Fragment shader source code                           ]
//[-------------------------------------------------------]
// One fragment shader invocation per fragment
fragmentShaderSourceCode = STRINGIFY(
// Uniforms
uniform sampler2D DiffuseMap0 : register(s0);
uniform sampler2D DiffuseMap1 : register(s1);

// Programs
float4 main(float4 Position : POSITION, float2 TexCoord : TEXCOORD0) : COLOR0
{
	// Fetch the texel at the given texture coordinate from render target 0 (which should contain a red triangle)
	float4 color0 = tex2D(DiffuseMap0, TexCoord);

	// Fetch the texel at the given texture coordinate from render target 1 (which should contain a blue triangle)
	float4 color1 = tex2D(DiffuseMap1, TexCoord);

	// Calculate the final color by subtracting the colors of the both render targets from white
	// -> The result should be white or green
	return float4(1.0f, 1.0f, 1.0f, 1.0f) - color0 - color1;
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
