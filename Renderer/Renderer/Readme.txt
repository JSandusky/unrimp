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


== Description ==
Renderer interfaces. Please note that this is a 100% interface project resulting in no binary at all. The one and only goal of this project is to offer unified renderer inferfaces for multiple graphics APIs like OpenGL or Direct3D. Features like resource loading, font rendering or even rendering of complex scenes is out of the scope of this project.

In order to make it easier to use the renderer, the optional comfort header "Renderer.h" puts everything together within a single header without any additional header inclusion. When writing interfaces for other languages like C# or Pascal, you might want to port this header instead of the more complex individual headers.


== Dependencies ==
None.


== Preprocessor Definitions ==
- Set "WIN32" as preprocessor definition when building for MS Windows
- Set "LINUX" as preprocessor definition when building for Linux or similar platforms
	- For Linux or similar platforms: Set "HAVE_VISIBILITY_ATTR" as preprocessor definition to use the visibility attribute (the used compiler must support it)
- Set "X64_ARCHITECTURE" as preprocessor definition when building for x64 instead of x86
- Set "RENDERER_NO_STATISTICS" as preprocessor definition in order to disable the gathering of statistics (it's recommended to only set this preprocessor definition if there are best possible performance requirements)
- Set "RENDERER_NO_DEBUG" as preprocessor definition in order to disable e.g. Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box) debug features (disabling support just reduces the binary size slightly but makes debugging more difficult)
