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
#include <algorithm>


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
	addExample("FirstTriangle", 				&RunExample<FirstTriangle>,					{"Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"});
	addExample("VertexBuffer", 					&RunExample<VertexBuffer>,					{"Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"});
	addExample("FirstTexture", 					&RunExample<FirstTexture>,					{"Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"});
	addExample("FirstRenderToTexture",			&RunExample<FirstRenderToTexture>,			{"Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"});
	addExample("FirstMultipleRenderTargets",	&RunExample<FirstMultipleRenderTargets>,	{"Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"});
	addExample("FirstMultipleSwapChains", 		&RunExample<FirstMultipleSwapChains>,		{"Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"});
	addExample("FirstInstancing", 				&RunExample<FirstInstancing>,				{"Null", "OpenGL", "Direct3D9", "Direct3D10", "Direct3D11"});
	addExample("FirstGeometryShader", 			&RunExample<FirstGeometryShader>,			{"Null", "OpenGL", "Direct3D10", "Direct3D11"});
	addExample("FirstTessellation", 			&RunExample<FirstTessellation>,				{"Null", "OpenGL", "Direct3D11"});
	// Advanced
	addExample("FirstPostProcessing", 			&RunExample<FirstPostProcessing>,			{"Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"});
	addExample("Fxaa", 							&RunExample<Fxaa>,							{"Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"});
	addExample("FirstGpgpu", 					&RunExample<FirstGpgpu>,					{"Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"});
	addExample("InstancedCubes", 				&RunExample<InstancedCubes>,				{"Null", "OpenGL", "Direct3D9", "Direct3D10", "Direct3D11"});
	addExample("IcosahedronTessellation", 		&RunExample<IcosahedronTessellation>,		{"Null", "OpenGL", "Direct3D11"});
	#ifndef RENDERER_NO_TOOLKIT
	addExample("FirstFont", 					&RunExample<FirstFont>,						{"Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"});
	#endif
	#ifndef NO_ASSIMP
	// Assimp
	addExample("FirstAssimp", 					&RunExample<InstancedCubes>,				{"Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"});
	addExample("AssimpMesh", 					&RunExample<IcosahedronTessellation>,		{"Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"});
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
	ExampleToSupportedRendererMap::iterator supportedRenderer = m_supportedRendererForExample.find(exampleName);
	bool rendererNotSupportedByExample = false;
	if (m_supportedRendererForExample.end() != supportedRenderer)
	{
		auto supportedRendererList = supportedRenderer->second;
		rendererNotSupportedByExample =  std::find(supportedRendererList.begin(), supportedRendererList.end(), rendererName) == supportedRendererList.end();
	}
	
	if(m_availableExamples.end() == example || m_availableRenderer.end() == renderer || rendererNotSupportedByExample)
	{
		if (m_availableExamples.end() == example)
			showError("no or unknown example given");
		if (m_availableRenderer.end() == renderer)
			showError("unkown renderer: \""+rendererName+"\"");
		if (rendererNotSupportedByExample) 
			showError("the example \""+exampleName+"\" doesn't support renderer: \""+rendererName+"\"");
			
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

void ExampleRunner::addExample(const std::string& name, RunnerMethod runnerMethod, std::initializer_list<std::string> supportedRendererList)
{
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>(name, runnerMethod));
	std::vector<std::string> supportedRenderer;
	// VS2012 doesn't support initializer_list for std container such as vector (even with Update 2 :()
	// -> we have to do it by hand
	for(auto renderer : supportedRendererList)
		supportedRenderer.push_back(renderer);
	m_supportedRendererForExample.insert(std::pair<std::string, std::vector<std::string>>(name, std::move(supportedRenderer)));
}
