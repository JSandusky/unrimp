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

#include <map>
#include <iostream>

typedef std::map<std::string, int(*)(const char*)> KnownExamplesMap;

static void printUsage(const KnownExamplesMap &knownExamples) {
	std::cout<<"Invalid argument!\n";
	std::cout<<"Usage: ./Examples <exampleName>\nAvailable Examples:\n";
	for (KnownExamplesMap::const_iterator it=knownExamples.cbegin(); it!=knownExamples.cend(); ++it)
		std::cout <<"\t"<< it->first<< '\n';
}

template <class ExampleClass>
int RunExample(const char* rendererName)
{
	return ExampleClass(rendererName).run();
}


//[-------------------------------------------------------]
//[ Platform independent program entry point              ]
//[-------------------------------------------------------]
int programEntryPoint(CmdLineArgs &args)
{
	// Program result
	int result = 0;

	// create list of available examples
	KnownExamplesMap knownExamples({
		 // Basics
		 { "FirstTriangle", 				&RunExample<FirstTriangle> }
		,{ "VertexBuffer", 					&RunExample<VertexBuffer> }
		,{ "FirstTexture", 					&RunExample<FirstTexture> }
		,{ "FirstRenderToTexture",			&RunExample<FirstRenderToTexture> }
		,{ "FirstMultipleRenderTargets",	&RunExample<FirstMultipleRenderTargets> }
		,{ "FirstMultipleSwapChains", 		&RunExample<FirstMultipleSwapChains> }
		,{ "FirstInstancing", 				&RunExample<FirstInstancing> }
		,{ "FirstGeometryShader", 			&RunExample<FirstGeometryShader> }
		,{ "FirstTessellation", 			&RunExample<FirstTessellation> }
		// Advanced
		,{ "FirstPostProcessing", 			&RunExample<FirstPostProcessing> }
		,{ "Fxaa", 							&RunExample<Fxaa> }
		,{ "FirstGpgpu", 					&RunExample<FirstGpgpu> }
		,{ "InstancedCubes", 				&RunExample<InstancedCubes> }
		,{ "IcosahedronTessellation", 		&RunExample<IcosahedronTessellation> }
		#ifndef RENDERER_NO_TOOLKIT
		,{ "FirstFont", 					&RunExample<FirstFont> }
		#endif
		#ifndef NO_ASSIMP
		// Assimp
		,{ "FirstAssimp", 					&RunExample<InstancedCubes> }
		,{ "AssimpMesh", 					&RunExample<IcosahedronTessellation> }
		#endif
	}
	);

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
	#ifdef LINUX
		// On Linux, only the OpenGL renderer is supported
		rendererName = "OpenGL";
	#endif

	// Check if the given example is known
	   KnownExamplesMap::iterator example = knownExamples.find(args.GetArg(0));
	if(knownExamples.end() == example)
	{
		// print usage
		printUsage(knownExamples);
	}
	else
	{
		// run example
		result = example->second(rendererName);
	}

	// Done
	return result;
}
