/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
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
Standalone renderer project compiler example.


== Preprocessor Definitions ==
Other
- "WIN32":					Set as preprocessor definition when building for MS Windows
- "LINUX":					Set as preprocessor definition when building for Linux or similar platforms
- "X64_ARCHITECTURE":		Set this as preprocessor definition when building for x64 instead of x86
- "SHARED_LIBRARIES":		Use renderers via shared libraries, if this is not defined, the renderers are statically linked
- "RENDERER_NO_DEBUG":		Disable e.g. Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box) debug features (disabling support just reduces the binary size slightly but makes debugging more difficult)
- "RENDERER_NO_RUNTIME":	Disable renderer runtime support
