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


== Description ==
Direct3D 11 renderer implementation.


== Dependencies ==
- Direct3D 11 runtime and Direct3D 11 capable graphics driver
- Optional AMD AGS for multi-draw-indirect on AMD GPUs ("amd_ags_x64.dll" and "amd_ags_x86.dll" aren't automatically installed with the GPU driver)
- Optional NVIDIA NvAPI for multi-draw-indirect on NVIDIA GPUs ("nvapi.dll" is automatically installed with the GPU driver)


== Preprocessor Definitions ==
- Set "DIRECT3D11RENDERER_EXPORTS" as preprocessor definition when building this library as shared library
- Do also have a look into the renderer readme file
