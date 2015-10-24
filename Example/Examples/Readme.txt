/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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


== Description ==
Standalone renderer examples.


== Recommended Example Order ==
- Basics
	- "FirstTriangle" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- Vertex shader (VS) and fragment shader (FS)
		- Debug methods: When using Direct3D <11.1, those methods map to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)
	- "VertexBuffer" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- Vertex shader (VS) and fragment shader (FS)
		- Multiple vertex attributes within a single vertex buffer object (VBO), vertex array object (VAO) is only using one vertex buffer object (VBO)
		- One vertex buffer object (VBO) per vertex attribute, vertex array object (VAO) is using multiple vertex buffer objects (VBO)
	- "FirstTexture" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- 2D texture
		- Sampler state object
		- Vertex shader (VS) and fragment shader (FS)
	- "FirstRenderToTexture" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- 2D texture
		- Sampler state object
		- Vertex shader (VS) and fragment shader (FS)
		- Framebuffer object (FBO) used for render to texture
	- "FirstMultipleRenderTargets" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- 2D texture
		- Sampler state object
		- Vertex shader (VS) and fragment shader (FS)
		- Framebuffer object (FBO) used for render to texture
		- Multiple render targets (MRT)
	- "FirstMultipleSwapChains" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- Vertex shader (VS) and fragment shader (FS)
		- Multiple swap chains
	- "FirstInstancing" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- Index buffer object (IBO)
		- Vertex shader (VS) and fragment shader (FS)
		- Instanced arrays (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
		- Draw instanced (shader model 4 feature, build in shader variable holding the current instance ID)
	- "FirstGeometryShader" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- Vertex shader (VS), geometry shader (GS) and fragment shader (FS)
		- Attribute less rendering (aka "drawing without data")
	- "FirstTessellation" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- Rasterizer state object
		- Vertex shader (VS), tessellation control shader (TCS), tessellation evaluation shader (TES) and fragment shader (FS)
- Advanced
	- "FirstPostProcessing" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- 2D texture
		- Sampler state object
		- Vertex shader (VS) and fragment shader (FS)
		- Framebuffer object (FBO) used for render to texture
		- Depth stencil state object
		- Post processing
	- "Fxaa" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- 2D texture
		- Sampler state object
		- Vertex shader (VS) and fragment shader (FS)
		- Framebuffer object (FBO) used for render to texture
		- Depth stencil state object
		- Post processing
		- "Fast Approximate Anti-Aliasing" (FXAA) as described within http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf and
		  "FxaaSamplePack.zip" ( https://docs.google.com/leaf?id=0B2manFVVrzQAMzUzYWQ5MTEtYmE1NC00ZTQzLWI2YWYtNTk2MDcyMmY1ZWNm&sort=name&layout=list&num=50&pli=1 )
	- "FirstGpgpu" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- 2D texture
		- Sampler state object
		- Vertex shader (VS) and fragment shader (FS)
		- Framebuffer object (FBO) used for render to texture
		- Depth stencil state object
		- General Purpose Computation on Graphics Processing Unit (GPGPU) by using the renderer interface and shaders without having any output window
	- "InstancedCubes" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- Index buffer object (IBO)
		- Uniform buffer object (UBO)
		- Texture buffer object (TBO)
		- 2D texture
		- 2D texture array
		- Sampler state object
		- Depth stencil state object
		- Blend state object
		- Vertex shader (VS) and fragment shader (FS)
		- Instanced arrays (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
		- Draw instanced (shader model 4 feature, build in shader variable holding the current instance ID)
		- Renderer runtime fonts
	- "IcosahedronTessellation" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- Index buffer object (IBO)
		- Uniform buffer object (UBO)
		- Vertex shader (VS), tessellation control shader (TCS), tessellation evaluation shader (TES), geometry shader (GS) and fragment shader (FS)
- Runtime
	- "FirstFont" demonstrates:
		- Renderer runtime fonts
		- Renderer runtime command buckets
		- Renderer runtime compositor
	- "FirstMesh" demonstrates:
		- Vertex buffer object (VBO)
		- Vertex array object (VAO)
		- Index buffer object (IBO)
		- Uniform buffer object (UBO)
		- Texture buffer object (TBO)
		- Sampler state object
		- Vertex shader (VS) and fragment shader (FS)
		- Texture collection
		- Sampler state collection
		- Blinn-Phong shading
		- Diffuse, normal, specular and emissive mapping
		- Optimization: Cache data to not bother the renderer API to much
		- Compact vertex format (16 bit texture coordinate, 16 bit QTangent, 56 bytes vs. 24 bytes per vertex)


== Preprocessor Definitions ==
For only supporting a particular renderer:
- "RENDERER_ONLY_NULL":			 Do only support Null
- "RENDERER_ONLY_OPENGL":		 Do only support OpenGL
- "RENDERER_ONLY_OPENGLES2":	 Do only support OpenGL ES 2
- "RENDERER_ONLY_DIRECT3D9":	 Do only support Direct3D 9
- "RENDERER_ONLY_DIRECT3D10":	 Do only support Direct3D 10
- "RENDERER_ONLY_DIRECT3D11":	 Do only support Direct3D 11
- "RENDERER_ONLY_DIRECT3D12":	 Do only support Direct3D 12

For disabling supporting a particular renderer:
- "RENDERER_NO_NULL":		Do not support Null
- "RENDERER_NO_OPENGL":		Do not support OpenGL
- "RENDERER_NO_OPENGLES2":	Do not support OpenGL ES 2
- "RENDERER_NO_DIRECT3D9":	Do not support Direct3D 9
- "RENDERER_NO_DIRECT3D10":	Do not support Direct3D 10
- "RENDERER_NO_DIRECT3D11":	Do not support Direct3D 11
- "RENDERER_NO_DIRECT3D12":	Do not support Direct3D 12

Other
- "WIN32":					Set as preprocessor definition when building for MS Windows
- "LINUX":					Set as preprocessor definition when building for Linux or similar platforms
- "X64_ARCHITECTURE":		Set this as preprocessor definition when building for x64 instead of x86
- "SHARED_LIBRARIES":		Use renderers via shared libaries, if this is not defined, the renderers are statically linked
- "RENDERER_NO_DEBUG":		Disable e.g. Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box) debug features (disabling support just reduces the binary size slightly but makes debugging more difficult)
- "RENDERER_NO_RUNTIME":	Disable renderer runtime support
