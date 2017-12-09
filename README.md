Warning
======
This project is currently still heavy under construction and not usable for productive projects.


Unrimp Description
======
Unified renderer implementation ("Un r imp"). Extensive examples from basic to advanced are provided as well. Originally started as modern renderer replacement for the [PixelLight](https://github.com/PixelLightFoundation/pixellight) engine.

The first public source code release after starting the project on Friday, 1 June 2012 was on January 03, 2013.


Features
======
- General library design
	- C++ 11 and above, no legacy compiler support, compiled with wall warning level
	- Since this is a spare time project for personal fun, some experimental STD features are used which hopefully find a way into the final C++ 17. For example ["std::tr2::sys::path"](https://msdn.microsoft.com/en-us/library/hh874694.aspx) to avoid having to use external libraries for such basics.
	- Compact user-header for the renderer backend API
		- A single all in one header for ease-of-use and best possible compile times
		- No need to links against the renderer library itself, load renderer interface implementations dynamically during runtime
	- Lightweight renderer implementations
		- Designed with AZDO ("Almost Zero Driver Overhead") in mind
		- Implementations try to stick as best as possible close-to-the-metal and as a result are just a few KiB instead of MiB in memory size
		- Implementations load the entry points of Vulkan, Direct3D, OpenGL and so on during runtime, meaning it's possible to react on system failures by e.g. dynamically switching to another renderer implementation
	- Support for static and dynamic build
	- Separation into backend, runtime and toolkit for asset cooking
		- Backend abstracts way the underlying renderer API like Vulkan/OpenGL/DirectX
		- Runtime designed with end-user and middleware-user in mind
			- Efficiency and responsiveness over flexibility (were it isn't useful in practice)
			- Intented to be controlled by a high-level entity-component system, no unused implementation feature overkill in the basic runtime
		- Toolkit designed with developer fast iterations in mind: Asset source flexibility, asset background compilation, hot-reloading
- Cross-platform
	- Microsoft Windows x86 and x64
	- Linux
	- Android
	- Mac OS X in mind (nothing more at the moment, but in mind is important because Mac OS X 10.11 only supports OpenGL 4.1 and has some other nasty issues)


Renderer API and Backends
======
- Renderer implementations for
	- Direct3D 9 (legacy, still maintained for curiosity reasons)
	- Direct3D 10 (legacy, still maintained for curiosity reasons)
	- Direct3D 11
	- Direct3D 12 (early phase)
	- Vulkan (not feature complete, yet)
	- OpenGL (by default a OpenGL 4.1 context is created, the best OpenGL version Mac OS X 10.11 supports, so lowest version we have to support)
	- OpenGL ES 3
	(legacy = critical features like uniform buffer, texture buffer and/or compute shaders are missing which are required for modern efficient renderers which provide e.g. automatic instancing or clustered deferred rendering)
- Render targets
	- Swap chains, render into one or multiple operation system windows
	- Framebuffer object (FBO) used for render to texture, support for multiple render targets (MRT), support for multisample (MSAA)
- Shaders
	- Shader types
		- Vertex shader (VS)
		- Tessellation control shader (TCS, "hull shader" in Direct3D terminology)
		- Tessellation evaluation shader (TES, "domain shader" in Direct3D terminology)
		- Geometry shader (GS)
		- Fragment shader (FS, "pixel shader" in Direct3D terminology)
	- Shader data sources
		- Shader bytecode (aka shader microcode, binary large object (BLOB))
			- Vulkan and OpenGL: SPIR-V support for cross-platform vendor and GPU driver independent shader bytecodes
				- Optional build in online GLSL to SPIR-V compilation using [glslang](https://github.com/KhronosGroup/glslang), offline compilation before shipping a product is preferred of course but not manditory
				- Using [SMOL-V](https://github.com/aras-p/smol-v): like Vulkan/Khronos SPIR-V, but smaller
		- Shader source code
- Buffers
	- Vertex array object (VAO, input-assembler (IA) stage) with support for multiple vertex streams
		- Vertex buffer object (VBO, input-assembler (IA) stage)
		- Index buffer object (IBO, input-assembler (IA) stage)
	- Indirect buffer object with optional internal emulation, draw methods always use an indirect buffer to have an unified draw call API
	- Uniform buffer object (UBO, "constant buffer" in Direct3D terminology)
	- Texture buffer object (TBO)
	- Command buffer mandatory by design, not just build on top
- Textures: 1D, 2D, 2D array, 3D, cube
- State objects with mapping to API specific settings during creation, not runtime
	- Pipeline state object (PSO) which directly maps to Direct3D 12, other backends internally subdivide into
		- Rasterizer state object (rasterizer stage (RS))
		- Depth stencil state object (output-merger (OM) stage)
		- Blend state object (output-merger (OM) stage)
	- Sampler state object (SO)
- Instancing support
	- Instanced arrays (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
	- Draw instanced (shader model 4 feature, build in shader variable holding the current instance ID)
- Debug methods
	- When using Direct3D <11.1, those methods map to the Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2017 out-of-the-box)
	- Used inside the renderer implementations for better renderer debugging
- Renderer implementation specific optimizations
	- OpenGL: Usage of direct state access (DSA), if available
- Interfaces for log, assert and memory so the user has the control over those things, standard implementations are provided


Renderer Runtime (e.g. "The Game")
======
- During runtime, only platform optimized and compressed binary assets are used
	- No inefficient generic stuff, no e.g. endianess handling, primarily raw chunks which can be fed into memory and GPU as efficient as possible
	- Efficient [CRN](https://github.com/BinomialLLC/crunch) textures are used by default, DDS is supported as well
	- Using [LZ4](http://lz4.github.io/lz4/) compression
- Asynchronous loading for all resources: To fight lags, micro stutter / judder, especially for virtual reality applications one needs a constant framerate
- Material and shader blueprint system which was desiged from ground up for pipeline state object (PSO) architecture
	- New material types can be added without a single line of C++ source code, meaning technical artists can create and fine-tune the shaders in realtime
	- Materials reference material blueprints and just are a list of key-value-pairs
	- Shader language specifics are abstracted away: Write shaders once, use them across multiple renderer APIs
	- Support for shader combinations (Uber-shaders)
	- Support for reusable shader pieces
	- Material inheritance for materials which should share common properties, but differ in other properties
	- Using [MojoShader](https://icculus.org/mojoshader/) as shader preprocessor so the resulting shader source codes are compact and easy to debug
	- Asyncrounous pipeline state compilation, including a fallback system to reduce visual artefacts in case of pipeline cache misses
- Compositor: Setup your overal rendering flow without a single line of C++ source code
	- The compositor is using the material blueprint system, meaning compact C++ implementation while offering mighty possibilities
	- Supports resolution scale
	- Blurred stabilized cascaded (CSM) exponential variance (EVSM) shadow mapping
- Scene as most top-level concept: Fancy-feature set kept simple because more complex applications / games usually add an entity-component-system
- Render queue fed with generic renderables to decouple concrete constructs like meshes, particles, terrain etc. from the generic rendering
- Using mathematics library [GLM](https://glm.g-truc.net/) with the C-preprocessor definitions "GLM_FORCE_RADIANS", "GLM_FORCE_LEFT_HANDED", "GLM_FORCE_DEPTH_ZERO_TO_ONE" and "GLM_ENABLE_EXPERIMENTAL"
- [ImGui](https://github.com/ocornut/imgui) is used as debug GUI to be able to quickly add interactive GUI elements for debugging, prototyping or visualization
	- Usage of [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) 3D gizmo extension
- Virtual reality manager which is internally using [OpenVR](https://github.com/ValveSoftware/openvr) for head-mounted display support
	- Animated controller visualization supported
	- Single pass stereo rendering via instancing
	- Hidden area mesh supported
- Abstract file interface so the Unrimp user has control over the file handling
	- Optional header-only standard implementations for UTF-8 STD file streams as well as [PhysicsFS](https://icculus.org/physfs/) for shipping packed asset packages are provided
- Texture top mipmap removal support while loading textures for efficient texture quality reduction
- Light
	- Types: Directional, point and spot
	- High-level sunlight controlled via time-of-day
- Skeleton animation
- Skybox
	- Classic environment cube map
	- Procedural Hosek-Wilkie sky which is also used to derive a sun color


Renderer Toolkit (e.g. "The Editor")
======
- Project compiler will transform source data into runtime data and while doing so tries to detect editing issues at tooltime as early as possible to reduce runtime harm and long debugging seasons
- Asynchronous resource compilation and hot reloading for all resources if the toolkit is enabled (true for production, probably not true for shippped titles)
	- Shader-resource example: It's possible to develop shaders while the application is running and see changes instantly
- Most source file formats are using JSON: [RapidJSON](http://rapidjson.org/) is used for parsing
- Performs optimizations and validations at tooltime instead of runtime. Examples:
	- Strips comments from shader source codes
	- Checks the material blueprint resources for valid uniform buffer packing rules
- Mesh compiler
	- Using [Assimp](http://assimp.sourceforge.net/) (Open Asset Import Library) for mesh import and post processing like joining identical vertices, vertex cache optimization etc.
	- Using [mikktspace](https://wiki.blender.org/index.php/Dev:Shading/Tangent_Space_Normal_Maps) by Morten S. Mikkelsen for semi-standard mesh tangent space generation
- Texture compiler
	- Using [Crunch](https://github.com/BinomialLLC/crunch) for mipmap generation and compression
	- Support for creating a cube-map out of six provided individual textures
	- Support for 2D-LUT to 3D-LUT conversion
	- Support for texture channel packing
	- Toksvig specular anti-aliasing to reduce shimmering/sparkling via texture modifiations during texture asset compilation
- [Sketchfab](https://sketchfab.com/) asset importer without the need to unzip the downloaded meshes first


Examples (just some high level keywords)
======
- Memory leaks: On Microsoft Windows, "_CrtMemCheckpoint()" and "_CrtMemDumpAllObjectsSince()" is used by default to detect memory leaks while developing and not later on. In case something triggers, use third parts tools to locate the memory leak in detail.
- Physically based shading (PBS) using "metallic workflow" (aka "metal-rough-workflow" aka "Albedo/Metallic/Roughness") instead of "specular workflow" (aka "specular-gloss-workflow" aka "Diffuse/Specular/Glossines")
- Microsoft Windows: ["NVIDIA Optimus"](http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf) and ["AMD Dynamic Switchable Graphic"](https://community.amd.com/message/1307599#comment-1307599) awareness to reduce the risk of getting the integrated graphics unit assigned when there's a dedicated graphics card as well
- [Custom resolved MSAA for antialiased deferred rendering and temporal anti-aliasing](https://mynameismjp.wordpress.com/2012/10/28/msaa-resolve-filters/)
- Gaussian blur, used to e.g. blur the transparent ImGui background
- High dynamic range (HDR) rendering with adaptive luminance
- Tangent space BC5/3DC/ATI2N stored normal maps
- [Color correction via 3D lookup table (LUT)](https://developer.nvidia.com/gpugems/GPUGems2/gpugems2_chapter24.html)
- [Fast Approximate Anti-Aliasing (FXAA)](http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf)
- [Screen space ambient occlusion (SSAO)](http://john-chapman-graphics.blogspot.de/2013/01/ssao-tutorial.html)
- [Old CRT post processing effect](https://www.shadertoy.com/view/MsXGD4)
- [Gamma correct rendering](https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch24.html)
- Bloom with dirty lens
- Chromatic aberration
- Clustered shading
- [Pseudo lens flare](http://john-chapman-graphics.blogspot.de/2013/02/pseudo-lens-flare.html)
- [Alpha to coverage](https://medium.com/@bgolus/anti-aliased-alpha-test-the-esoteric-alpha-to-coverage-8b177335ae4f)
- Depth of field
- GPU skinning
- Motion blur
- Film grain
- [RGB dither](http://media.steampowered.com/apps/valve/2015/Alex_Vlachos_Advanced_VR_Rendering_GDC2015.pdf)
- Depth fog
- Vignette


Terminology and Acronyms
======
- Renderer backend
	- Vertex buffer object (VBO)
	- Index buffer object (IBO)
	- Vertex array object (VAO)
	- Uniform buffer object (UBO), "constant buffer" in Direct3D terminology
	- Texture buffer object (TBO)
	- Sampler state object (SO)
	- Root signature (Direct3D terminology) = pipeline layout in Vulkan terminology
	- Pipeline state object (PSO)
	- Vertex shader (VS)
	- Tessellation control shader (TCS), "hull shader" in Direct3D terminology
	- Tessellation evaluation shader (TES), "domain shader" in Direct3D terminology
	- Geometry shader (GS)
	- Fragment shader (FS), "pixel shader" in Direct3D terminology
	- Uniform buffer view (UBV)
	- Shader resource view (SRV)
	- Unordered access view (UAV)
- Renderer runtime
	- Asset: Lightweight content metadata like ID, type and location (texture, mesh, shader etc. - on this abstraction level everything is an asset)
	- Resource: A concrete asset type used during runtime in-memory (texture, mesh, shader etc.)
	- Mesh: 3D-model consisting of a vertex- and index-buffer, geometry subdivided into sub-meshes
	- Material and shader blueprint: High-level rendering description
	- Material: Just a property-set used as shader input
	- HMD: Head mounted display


Microsoft Windows: First Example Kickoff
======
- Build "Release_Dynamic" target with Visual Studio
- Compile the runtime assets by starting "bin\x86_dynamic\ExampleProjectCompiler.exe"
- Run "bin\x86_dynamic\Examples.exe" (is using default command line arguments "bin\x86_dynamic\Examples.exe FirstScene -r Direct3D11")


Useful MS Windows Developer Tools
======
- When profiling a product
	- Memory
		- Free and open-source: [MTuner](https://github.com/milostosic/MTuner)
			- As of October 14, 2017: Doesn't work with Visual Studio 2017 (v141) x86 but works with x64
	- Graphics
		- Direct3D 11 graphics debugging can be done directly inside Visual Studio 2017
		- [RenderDoc](https://renderdoc.org/) for other renderer APIs like Vulkan is working like a charme
	- CPU
		- For CPU profiling the tool [Very Sleepy](http://www.codersnotes.com/sleepy/) is easy to use while providing useful results
- Checking external dependencies of exe and dll: [Dependency Walker](http://www.dependencywalker.com/)
- When there's need to figure out why the binaries are so large
	- ["Sizer - executable size breakdown (2007)"](http://aras-p.info/projSizer.html): "Command line tool that reports size of things (functions, data, classes, templates, object files) in a Visual Studio compiled exe/dll. Extracts info from debug information (.pdb) file."
		- As of October 14, 2017: Doesn't work with Visual Studio 2017 (v141), compile for Visual Studio 2015 (v140) if you want to analys the binaries using Sizer
- When shipping a product, use a static build and e.g. [UPX](https://upx.github.io/) to get executables even more compact on end-user-systems
- OpenGL ES 3 development: PowerVR (SDK 2017 R1 or newer) works well. Just copy "libEGL.dll" and "libGLESv2.dll" e.g. into "unrimp\bin\x86d_dynamic".


Useful Asset Sources
======
- Realtime ready meshes with textures and a web-browser realtime preview: [Sketchfab](https://sketchfab.com/), search for downloadable
- Realtime ready shaders and a web-browser realtime preview: [Shadertoy](https://www.shadertoy.com/)
- Realtime ready post-processing shaders: [reshade-shaders](https://github.com/crosire/reshade-shaders/tree/master/Shaders)


Hints
======
- Assets are referenced by using
	- Source asset ID naming scheme ```"<name>.asset"```
	- Compiled or runtime generated asset ID naming scheme ```"<project name>/<asset type>/<asset category>/<asset name>"```
- Error strategy
	- Inside renderer toolkit: Exceptions in extreme, up to no error tolerance. If something smells odd, blame it to make it possible to detect problems as early as possible in the production pipeline.
	- Inside renderer runtime: The show must go on. If the floor breaks, just keep smiling and continue dancing.

The unified renderer interface can't unify some graphics API behaviour differences. Here's a list of hints you might want to know:
- Texel coordinate system origin
	- OpenGL: Left/bottom
	- Direct3D: Left/top
		- See ["Coordinate Systems (Direct3D 10)"](http://msdn.microsoft.com/en-us/library/windows/desktop/cc308049%28v=vs.85%29.aspx) at MSDN
- Pixel coordinate system 
	- Direct3D: See ["Coordinate Systems (Direct3D 10)"](http://msdn.microsoft.com/en-us/library/windows/desktop/cc308049%28v=vs.85%29.aspx) at MSDN
- Clip space coordinate system
	- OpenGL: [-1, -1, -1] ... [1, 1, 1]
	- Direct3D: [-1, -1, 0] ... [1, 1, 1]
	- Additional information: ["The Cg Tutorial"-book, section "4.1.8 The Projection Transform"](http://http.developer.nvidia.com/CgTutorial/cg_tutorial_chapter04.html)
- Physically based shading (PBS)
	- ["Physically-Based Rendering, And You Can Too!"](https://www.marmoset.co/posts/physically-based-rendering-and-you-can-too/) - By Joe "EarthQuake" Wilson
	- ["Moving Frostbite to PBR"](https://www.ea.com/frostbite/news/moving-frostbite-to-pb)
	- ["Physically Based Shading and Image Based Lighting"](http://www.trentreed.net/blog/physically-based-shading-and-image-based-lighting/)
	- ["The comprehensive PBR guide"](https://www.allegorithmic.com/pbr-guide)


[MIT License](https://opensource.org/licenses/MIT)
======
Copyright (c) 2012-2017 The Unrimp Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.