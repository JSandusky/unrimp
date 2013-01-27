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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/Main.h"
// Basics
#include "FirstTriangle/FirstTriangle.h"
#include "VertexBuffer/VertexBuffer.h"
#include "FirstTexture/FirstTexture.h"
#include "FirstRenderToTexture/FirstRenderToTexture.h"
#include "FirstMultipleRenderTargets/FirstMultipleRenderTargets.h"
#include "FirstMultipleSwapChains/FirstMultipleSwapChains.h"
#include "FirstInstancing/FirstInstancing.h"
#include "FirstGeometryShader/FirstGeometryShader.h"
#include "FirstTessellation/FirstTessellation.h"
// Advanced
#include "FirstPostProcessing/FirstPostProcessing.h"
#include "Fxaa/Fxaa.h"
#include "FirstGpgpu/FirstGpgpu.h"
#include "InstancedCubes/InstancedCubes.h"
#include "IcosahedronTessellation/IcosahedronTessellation.h"
#ifndef RENDERER_NO_TOOLKIT
	#include "FirstFont/FirstFont.h"
#endif
#ifndef NO_ASSIMP
	#include "FirstAssimp/FirstAssimp.h"
	#include "AssimpMesh/AssimpMesh.h"
#endif


//[-------------------------------------------------------]
//[ Platform independent program entry point              ]
//[-------------------------------------------------------]
int programEntryPoint()
{
	// Program result
	int result = 0;

	// Case sensitive name of the renderer to instance, might be ignored in case e.g. "RENDERER_ONLY_DIRECT3D11" was set as preprocessor definition
	// -> Example renderer names: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
	// -> In case the graphics driver supports it, the OpenGL ES 2 renderer can automatically also run on a desktop PC without an emulator (perfect for testing/debugging)
	const char *rendererName = "Direct3D11";

	// Enforce a certain renderer via preprocessor definition?
	#ifdef RENDERER_ONLY_NULL
		rendererName = "Null";
	#elif RENDERER_ONLY_OPENGL
		rendererName = "OpenGL";
	#elif RENDERER_ONLY_OPENGLES2
		rendererName = "OpenGLES2";
	#elif RENDERER_ONLY_DIRECT3D9
		rendererName = "Direct3D9";
	#elif RENDERER_ONLY_Direct3D10
		rendererName = "Direct3D10";
	#elif RENDERER_ONLY_DIRECT3D11
		rendererName = "Direct3D11";
	#endif
		
	rendererName = "OpenGL";

	// Create an instance of our application on the C runtime stack
	// Basics
//	result = FirstTriangle(rendererName).run();					// Works with: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
//	result = VertexBuffer(rendererName).run();					// Works with: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
	result = FirstTexture(rendererName).run();					// Works with: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
//	result = FirstRenderToTexture(rendererName).run();			// Works with: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
//	result = FirstMultipleRenderTargets(rendererName).run();	// Works with: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
//	result = FirstMultipleSwapChains(rendererName).run();		// Works with: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
//	result = FirstInstancing(rendererName).run();				// Works with: "Null", "OpenGL", "Direct3D9", "Direct3D10", "Direct3D11"
//	result = FirstGeometryShader(rendererName).run();			// Works with: "Null", "OpenGL" (shader model 4), "Direct3D10", "Direct3D11"
//	result = FirstTessellation(rendererName).run();				// Works with: "Null", "OpenGL" (shader model 5), "Direct3D11"
	// Advanced
//	result = FirstPostProcessing(rendererName).run();			// Works with: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
//	result = Fxaa(rendererName).run();							// Works with: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
//	result = FirstGpgpu(rendererName).run();					// Works with: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
//	result = InstancedCubes(rendererName).run();				// Works with: "Null", "OpenGL", "Direct3D9", "Direct3D10", "Direct3D11"
//	result = IcosahedronTessellation(rendererName).run();		// Works with: "Null", "OpenGL" (shader model 5), "Direct3D11"
	#ifndef RENDERER_NO_TOOLKIT
		// Renderer toolkit
//		result = FirstFont(rendererName).run();					// Works with: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
	#endif
	#ifndef NO_ASSIMP
		// Assimp
//		result = FirstAssimp(rendererName).run();				// Works with: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
//		result = AssimpMesh(rendererName).run();				// Works with: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
	#endif

	// Done
	return result;
}
