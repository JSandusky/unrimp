/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
Renderer runtime.


== Dependencies ==
- GLM (header only library)
- ImGui (directly compiled and linked in)
- crunch (header only part of the library, doesn't depend on crn compression library)


== Preprocessor Definitions ==
- Set "RENDERERRUNTIME_EXPORTS" as preprocessor definition when building this library as shared library
- Set "RENDERERRUNTIME_NO_RENDERERMATCHCHECK" as preprocessor definition when building this library to remove renderer<->resource matches (better performance, reduces the binary size slightly, but you will receive no hint when using a resource from another renderer instance)
- Do also have a look into the renderer readme file
