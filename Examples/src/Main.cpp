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
#include <set>
#include <iostream>


template <class ExampleClass>
int RunExample(const char* rendererName)
{
	return ExampleClass(rendererName).run();
}

class ExampleRunner
{
public:
	virtual int run(const CmdLineArgs &args) = 0;

protected:
	typedef std::map<std::string, int(*)(const char*)> AvailableExamplesMap;
	typedef std::set<std::string> AvailableRendererMap;
	
	ExampleRunner()
		:
#ifndef WIN32 // VS2012 doesn't support initializer lists for std::map and std::set (even after update 2) :(
		 m_availableExamples({
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
		})
		, m_availableRenderer({
			#ifndef RENDERER_NO_NULL
			"Null"
			#endif
			#ifdef WIN32
				#ifndef RENDERER_NO_DIRECT3D9
				, "Direct3D9"
				#endif
				#ifndef RENDERER_NO_DIRECT3D10
				, "Direct3D10"
				#endif
				#ifndef RENDERER_NO_DIRECT3D11
				, "Direct3D11"
				#endif
			#endif
			#ifndef RENDERER_NO_OPENGL
			, "OpenGL"
			#endif
			#ifndef RENDERER_NO_OPENGLES2
			, "OpenGLES2"
			#endif
		}) ,
#endif
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
		#ifdef WIN32
			 // Basics
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("FirstTriangle", 				&RunExample<FirstTriangle>));
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("VertexBuffer", 					&RunExample<VertexBuffer>));
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("FirstTexture", 					&RunExample<FirstTexture>));
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("FirstRenderToTexture",			&RunExample<FirstRenderToTexture>));
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("FirstMultipleRenderTargets",	&RunExample<FirstMultipleRenderTargets>));
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("FirstMultipleSwapChains", 		&RunExample<FirstMultipleSwapChains>));
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("FirstInstancing", 				&RunExample<FirstInstancing>));
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("FirstGeometryShader", 			&RunExample<FirstGeometryShader>));
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("FirstTessellation", 			&RunExample<FirstTessellation>));
			 // Advanced
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("FirstPostProcessing", 			&RunExample<FirstPostProcessing>));
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("Fxaa", 							&RunExample<Fxaa>));
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("FirstGpgpu", 					&RunExample<FirstGpgpu>));
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("InstancedCubes", 				&RunExample<InstancedCubes>));
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("IcosahedronTessellation", 		&RunExample<IcosahedronTessellation>));
			#ifndef RENDERER_NO_TOOLKIT
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("FirstFont", 					&RunExample<FirstFont>));
			#endif
			#ifndef NO_ASSIMP
			// Assimp
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("FirstAssimp", 					&RunExample<InstancedCubes>));
			m_availableExamples.insert(std::pair<std::string,int(*)(const char*)>("AssimpMesh", 					&RunExample<IcosahedronTessellation>));
			#endif

			#ifndef RENDERER_NO_NULL
			m_availableRenderer.insert("Null");
			#endif
			#ifndef RENDERER_NO_DIRECT3D9
			m_availableRenderer.insert("Direct3D9");
			#endif
			#ifndef RENDERER_NO_DIRECT3D10
			m_availableRenderer.insert("Direct3D10");
			#endif
			#ifndef RENDERER_NO_DIRECT3D11
			m_availableRenderer.insert("Direct3D11");
			#endif
			#ifndef RENDERER_NO_OPENGL
			m_availableRenderer.insert("OpenGL");
			#endif
			#ifndef RENDERER_NO_OPENGLES2
			m_availableRenderer.insert("OpenGLES2");
			#endif
#endif
	}
	
	virtual void printUsage(const AvailableExamplesMap &knownExamples, const AvailableRendererMap &availableRenderer) = 0;
	virtual void showError(const std::string errorMsg) = 0;
	
	int runExample(const std::string rendererName, const std::string exampleName) {
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
	
protected:
	AvailableExamplesMap m_availableExamples;
	AvailableRendererMap m_availableRenderer;
	const std::string m_defaultRendererName;
};

class ConsoleExampleRunner: public ExampleRunner {
public:
	virtual int run(const CmdLineArgs &args) override {
		parseArgs(args);
		return runExample(m_rendererName.c_str(), m_exampleName.c_str());
	}

protected:
	virtual void printUsage(const AvailableExamplesMap &knownExamples, const AvailableRendererMap &availableRenderer) override {
		std::cout<<"Usage: ./Examples <exampleName> [-r <rendererName>]\n";
		std::cout<<"Available Examples:\n";
		for (AvailableExamplesMap::const_iterator it=knownExamples.cbegin(); it!=knownExamples.cend(); ++it)
			std::cout <<"\t"<< it->first<< '\n';
		std::cout<<"Available Renderer:\n";
		for (AvailableRendererMap::const_iterator it=availableRenderer.cbegin(); it!=availableRenderer.cend(); ++it)
			std::cout <<"\t"<< *it<< '\n';
	}
	virtual void showError(const std::string errorMsg) override {
		std::cout<<errorMsg<<"\n";
	}
private:
	void parseArgs(const CmdLineArgs &args) {
		int length = args.GetCount();
		for(int i = 0; i < length; ++i) {
			std::string arg = args.GetArg(i); 
			if (arg != "-r") {
				m_exampleName = arg;
			}
			else {
				if (i+1 < length) {
					++i;
					m_rendererName = args.GetArg(i);
				}
				else
				{
					showError("missing argument for parameter -r");
					return;
				}
			}
			
		}
		
		if (m_rendererName.empty())
			m_rendererName = m_defaultRendererName;
	}
private:
	std::string m_rendererName;
	std::string m_exampleName;
};


//[-------------------------------------------------------]
//[ Platform independent program entry point              ]
//[-------------------------------------------------------]
int programEntryPoint(CmdLineArgs &args)
{
	return ConsoleExampleRunner().run(args);
}
