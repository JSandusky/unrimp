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
#include "PrecompiledHeader.h"
#include "ExampleRunner.h"
#include "Framework/PlatformTypes.h"
#include "Framework/CmdLineArgs.h"
// Basics
#include "Basics/FirstTriangle/FirstTriangle.h"
#include "Basics/FirstIndirectBuffer/FirstIndirectBuffer.h"
#include "Basics/VertexBuffer/VertexBuffer.h"
#include "Basics/FirstTexture/FirstTexture.h"
#include "Basics/FirstRenderToTexture/FirstRenderToTexture.h"
#include "Basics/FirstMultipleRenderTargets/FirstMultipleRenderTargets.h"
#include "Basics/FirstMultipleSwapChains/FirstMultipleSwapChains.h"
#include "Basics/FirstInstancing/FirstInstancing.h"
#include "Basics/FirstGeometryShader/FirstGeometryShader.h"
#include "Basics/FirstTessellation/FirstTessellation.h"
// Advanced
#include "Advanced/FirstPostProcessing/FirstPostProcessing.h"
#include "Advanced/Fxaa/Fxaa.h"
#include "Advanced/FirstGpgpu/FirstGpgpu.h"
#include "Advanced/InstancedCubes/InstancedCubes.h"
#include "Advanced/IcosahedronTessellation/IcosahedronTessellation.h"
#ifndef RENDERER_NO_RUNTIME
	#include "Runtime/FirstMesh/FirstMesh.h"
	#include "Runtime/FirstCompositor/FirstCompositor.h"
	#include "Runtime/FirstScene/FirstScene.h"
#endif
#include <algorithm>
#include <array>


//[-------------------------------------------------------]
//[ Helper method template                                ]
//[-------------------------------------------------------]
template <class ExampleClass>
int RunExample(const char* rendererName)
{
	return ExampleClass(rendererName).run();
}

ExampleRunner::~ExampleRunner() {}

ExampleRunner::ExampleRunner()
	:
	// Case sensitive name of the renderer to instance, might be ignored in case e.g. "RENDERER_ONLY_DIRECT3D12" was set as preprocessor definition
	// -> Example renderer names: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11", "Direct3D12", "Vulkan"
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
			#elif RENDERER_ONLY_DIRECT3D11
				"Direct3D11"
			#elif RENDERER_ONLY_DIRECT3D12
				"Direct3D12"
			#elif RENDERER_ONLY_VULKAN
				"Vulkan"
			#endif
		#endif
	)
{
	std::array<std::string, 8> supportsAllRenderer = {{"Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11", "Direct3D12", "Vulkan"}};
	std::array<std::string, 7> doesNotSupportOpenGLES2 = {{"Null", "OpenGL", "Direct3D9", "Direct3D10", "Direct3D11", "Direct3D12", "Vulkan"}};
	std::array<std::string, 6> onlyShaderModel4Plus = {{"Null", "OpenGL", "Direct3D10", "Direct3D11", "Direct3D12", "Vulkan"}};
	std::array<std::string, 5> onlyShaderModel5Plus = {{"Null", "OpenGL", "Direct3D11", "Direct3D12", "Vulkan"}};
	// Basics
	addExample("FirstTriangle",					&RunExample<FirstTriangle>,					supportsAllRenderer);
	addExample("FirstIndirectBuffer",			&RunExample<FirstIndirectBuffer>,			supportsAllRenderer);
	addExample("VertexBuffer",					&RunExample<VertexBuffer>,					supportsAllRenderer);
	addExample("FirstTexture",					&RunExample<FirstTexture>,					supportsAllRenderer);
	addExample("FirstRenderToTexture",			&RunExample<FirstRenderToTexture>,			supportsAllRenderer);
	addExample("FirstMultipleRenderTargets",	&RunExample<FirstMultipleRenderTargets>,	supportsAllRenderer);
	addExample("FirstMultipleSwapChains",		&RunExample<FirstMultipleSwapChains>,		supportsAllRenderer);
	addExample("FirstInstancing",				&RunExample<FirstInstancing>,				doesNotSupportOpenGLES2);
	addExample("FirstGeometryShader",			&RunExample<FirstGeometryShader>,			onlyShaderModel4Plus);
	addExample("FirstTessellation",				&RunExample<FirstTessellation>,				onlyShaderModel5Plus);
	// Advanced
	addExample("FirstPostProcessing",			&RunExample<FirstPostProcessing>,			supportsAllRenderer);
	addExample("Fxaa",							&RunExample<Fxaa>,							supportsAllRenderer);
	addExample("FirstGpgpu",					&RunExample<FirstGpgpu>,					supportsAllRenderer);
	addExample("InstancedCubes",				&RunExample<InstancedCubes>,				doesNotSupportOpenGLES2);
	addExample("IcosahedronTessellation",		&RunExample<IcosahedronTessellation>,		onlyShaderModel5Plus);
	#ifndef RENDERER_NO_RUNTIME
		// Renderer runtime
		addExample("FirstMesh",					&RunExample<FirstMesh>,						supportsAllRenderer);
		addExample("FirstCompositor",			&RunExample<FirstCompositor>,				supportsAllRenderer);
		addExample("FirstScene",				&RunExample<FirstScene>,					supportsAllRenderer);
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
		#ifndef RENDERER_NO_DIRECT3D12
			m_availableRenderer.insert("Direct3D12");
		#endif
	#endif
	#ifndef RENDERER_NO_OPENGL
		m_availableRenderer.insert("OpenGL");
	#endif
	#ifndef RENDERER_NO_OPENGLES2
		m_availableRenderer.insert("OpenGLES2");
	#endif
	#ifndef RENDERER_NO_VULKAN
		m_availableRenderer.insert("Vulkan");
	#endif
}

int ExampleRunner::runExample(const std::string& rendererName, const std::string& exampleName)
{
	AvailableExamplesMap::iterator example = m_availableExamples.find(exampleName);
	AvailableRendererMap::iterator renderer = m_availableRenderer.find(rendererName);
	ExampleToSupportedRendererMap::iterator supportedRenderer = m_supportedRendererForExample.find(exampleName);
	bool rendererNotSupportedByExample = false;
	if (m_supportedRendererForExample.end() != supportedRenderer)
	{
		auto supportedRendererList = supportedRenderer->second;
		rendererNotSupportedByExample =  std::find(supportedRendererList.begin(), supportedRendererList.end(), rendererName) == supportedRendererList.end();
	}
	
	if (m_availableExamples.end() == example || m_availableRenderer.end() == renderer || rendererNotSupportedByExample)
	{
		if (m_availableExamples.end() == example)
			showError("no or unknown example given");
		if (m_availableRenderer.end() == renderer)
			showError("unkown renderer: \"" + rendererName + "\"");
		if (rendererNotSupportedByExample) 
			showError("the example \"" + exampleName + "\" doesn't support renderer: \"" + rendererName + "\"");

		// Print usage
		printUsage(m_availableExamples, m_availableRenderer);
		return 0;
	}
	else
	{
		// Run example
		return example->second(rendererName.c_str());
	}
}

template<typename T>
void ExampleRunner::addExample(const std::string& name, RunnerMethod runnerMethod, T const &supportedRendererList)
{
	m_availableExamples.insert(std::pair<std::string,RunnerMethod>(name, runnerMethod));
	std::vector<std::string> supportedRenderer;
	for (auto renderer = supportedRendererList.begin(); renderer != supportedRendererList.end(); ++renderer)
		supportedRenderer.push_back(*renderer);
	m_supportedRendererForExample.insert(std::pair<std::string, std::vector<std::string>>(name, std::move(supportedRenderer)));
}
