/*********************************************************\
 * Copyright (c) 2013-2013 Stephan Wezel
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
#include "Framework/PlatformTypes.h"
#include "ExampleRunner.h"
#include "CmdLineArgs.h"
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
//[ Helper method template                                ]
//[-------------------------------------------------------]
template <class ExampleClass>
int RunExample(const char* rendererName)
{
	return ExampleClass(rendererName).run();
}

ExampleRunner::ExampleRunner()
	:
	// Case sensitive name of the renderer to instance, might be ignored in case e.g. "RENDERER_ONLY_DIRECT3D11" was set as preprocessor definition
	// -> Example renderer names: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
	// -> In case the graphics driver supports it, the OpenGL ES 2 renderer can automatically also run on a desktop PC without an emulator (perfect for testing/debugging)
	m_defaultRendererName(
		#ifdef RENDERER_ONLY_NULL
		"Null"
		#elif defined(RENDERER_ONLY_OPENGL) || defined(LINUX)
		"OpenGL"
		#elif RENDERER_ONLY_OPENGLES2
		"OpenGLES2"
		#elif WIN32
			#ifdef RENDERER_ONLY_DIRECT3D9
			"Direct3D9"
			#elif RENDERER_ONLY_DIRECT3D10
			"Direct3D10"
			#else 
			"Direct3D11"
			#endif
		#endif
	)
{
	// Basics
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("FirstTriangle", 				&RunExample<FirstTriangle>));
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("VertexBuffer", 					&RunExample<VertexBuffer>));
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("FirstTexture", 					&RunExample<FirstTexture>));
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("FirstRenderToTexture",			&RunExample<FirstRenderToTexture>));
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("FirstMultipleRenderTargets",	&RunExample<FirstMultipleRenderTargets>));
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("FirstMultipleSwapChains", 		&RunExample<FirstMultipleSwapChains>));
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("FirstInstancing", 				&RunExample<FirstInstancing>));
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("FirstGeometryShader", 			&RunExample<FirstGeometryShader>));
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("FirstTessellation", 			&RunExample<FirstTessellation>));
	// Advanced
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("FirstPostProcessing", 			&RunExample<FirstPostProcessing>));
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("Fxaa", 							&RunExample<Fxaa>));
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("FirstGpgpu", 					&RunExample<FirstGpgpu>));
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("InstancedCubes", 				&RunExample<InstancedCubes>));
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("IcosahedronTessellation", 		&RunExample<IcosahedronTessellation>));
	#ifndef RENDERER_NO_TOOLKIT
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("FirstFont", 					&RunExample<FirstFont>));
	#endif
	#ifndef NO_ASSIMP
	// Assimp
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("FirstAssimp", 					&RunExample<InstancedCubes>));
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>("AssimpMesh", 					&RunExample<IcosahedronTessellation>));
	#endif

	#ifndef RENDERER_NO_NULL
	m_availableRenderer.insert("Null");
	#endif
	#ifdef WIN32
		#ifndef RENDERER_NO_DIRECT3D9
		m_availableRenderer.insert("Direct3D9");
		#endif
		#ifndef RENDERER_NO_DIRECT3D10
		m_availableRenderer.insert("Direct3D10");
		#endif
		#ifndef RENDERER_NO_DIRECT3D11
		m_availableRenderer.insert("Direct3D11");
		#endif
	#endif
	#ifndef RENDERER_NO_OPENGL
	m_availableRenderer.insert("OpenGL");
	#endif
	#ifndef RENDERER_NO_OPENGLES2
	m_availableRenderer.insert("OpenGLES2");
	#endif
}

int ExampleRunner::runExample(const std::string rendererName, const std::string exampleName) {
	AvailableExamplesMap::iterator example = m_availableExamples.find(exampleName);
	AvailableRendererMap::iterator renderer = m_availableRenderer.find(rendererName);
	if(m_availableExamples.end() == example || m_availableRenderer.end() == renderer)
	{
		if (m_availableExamples.end() == example)
			showError("no or unknown example given");
		if	(m_availableRenderer.end() == renderer)
			showError("unkown renderer: \""+rendererName+"\"");
			
		// print usage
		printUsage(m_availableExamples, m_availableRenderer);
		return 0;
	}
	else
	{
		// run example
		return example->second(rendererName.c_str());
	}
}