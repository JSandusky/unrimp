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


== Description ==
Direct3D 9 renderer implementation.


== Dependencies ==
Direct3D 9 runtime and Direct3D 9 capable grahpics driver, nothing else.


== Preprocessor Definitions ==
- Set "DIRECT3D9RENDERER_EXPORTS" as preprocessor definition when building this library as shared library
- Set "DIRECT3D9RENDERER_NO_RENDERERMATCHCHECK" as preprocessor definition when building this library to remove renderer<->resource matches (better performance, reduces the binary size slightly, but you will receive no hint when using a resource from another renderer instance)
- Set "DIRECT3D9RENDERER_NO_CG" as preprocessor definition in order to disable Cg support (always linked at runtime, disabling Cg support just reduces the binary size slightly)
- Set "DIRECT3D9RENDERER_NO_DEBUG" as preprocessor definition in order to disable Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box) debug features (always linked at runtime, disabling support just reduces the binary size slightly but makes debugging more difficult)
- Do also have a look into the renderer readme file


== Direct3D 9 Debugging ==
- Unlike Direct3D 10 & 11, the Direct3D debug layer is not application controlled in Direct3D 9
- This means that it has to be configured outside of our application
- Use the tool "dxcpl.exe" from the DirectX SDK to switch to the debug version of Direct3D 9, do also setup the desired debug output level
- When running the application by using Visual Studio, you can now see Direct3D 9 debug information inside the output window
