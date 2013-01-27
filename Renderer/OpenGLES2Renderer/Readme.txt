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
OpenGL ES 2 renderer implementation.


== Dependencies ==
OpenGL ES 2 capable grahpics driver or emulator, nothing else.


== Preprocessor Definitions ==
- Set "OPENGLES2RENDERER_EXPORTS" as preprocessor definition when building this library as shared library
- If this renderer was compiled with "OPENGLES2RENDERER_NO_STATE_CLEANUP" set as preprocessor definition, the previous OpenGL ES 2 state will not be restored after performing an operation (better performance, reduces the binary size slightly, but might result in unexpected behaviour when using OpenGL ES 2 directly beside this renderer)
- Set "OPENGLES2RENDERER_NO_RENDERERMATCHCHECK" as preprocessor definition when building this library to remove renderer<->resource matches (better performance, reduces the binary size slightly, but you will receive no hint when using a resource from another renderer instance)
- Set "OPENGLES2RENDERER_NO_CG" as preprocessor definition in order to disable Cg support (always linked at runtime, disabling Cg support just reduces the binary size slightly)
- Do also have a look into the renderer readme file
