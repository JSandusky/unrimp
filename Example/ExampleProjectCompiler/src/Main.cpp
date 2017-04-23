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


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Main.h"

#include <RendererToolkit/Public/RendererToolkit.h>
#include <RendererToolkit/Public/RendererToolkitInstance.h>

#include <exception>
#ifdef WIN32
	// Disable warnings in external headers, we can't fix them
	__pragma(warning(push))
		__pragma(warning(disable: 4548))	// warning C4548: expression before comma has no effect; expected expression with side-effect
		#include <filesystem>
	__pragma(warning(pop))
#else
	#include <experimental/filesystem>
#endif


//[-------------------------------------------------------]
//[ Global definitions                                    ]
//[-------------------------------------------------------]
#ifdef WIN32
	namespace std_filesystem = std::tr2::sys;
#else
	namespace std_filesystem = std::experimental::filesystem;
#endif


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		const char* getAbsoluteLocalDataDirectoryName()
		{
			// For the Unrimp examples were using the following directory structure
			// - "<root directory>/bin/x64_static"
			// - "<root directory>/bin/DataPc"
			// - "<root directory>/bin/LocalData"
			// -> For end-user products, you might want to choose a local user data directory
			// -> In here we assume that the current directory has not been changed and still points to the directory the running executable is in (e.g. "<root directory>/bin/x64_static")
			static const std::string absoluteLocalDataDirectoryName = std_filesystem::canonical(std_filesystem::current_path() / "/../LocalData").generic_string();
			return absoluteLocalDataDirectoryName.c_str();
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Platform independent program entry point              ]
//[-------------------------------------------------------]
int programEntryPoint()
{
	RendererToolkit::RendererToolkitInstance rendererToolkitInstance(::detail::getAbsoluteLocalDataDirectoryName());
	RendererToolkit::IRendererToolkit* rendererToolkit = rendererToolkitInstance.getRendererToolkit();
	if (nullptr != rendererToolkit)
	{
		// TODO(co) Experiments
		RendererToolkit::IProject* project = rendererToolkit->createProject();
		try
		{
			project->loadByFilename("../DataSource/Example.project");
		//	project->compileAllAssets("Direct3D9_30");
			project->compileAllAssets("Direct3D11_50");
		//	project->compileAllAssets("Direct3D12_50");
		//	project->compileAllAssets("OpenGLES3_300");
		}
		catch (const std::exception& e)
		{
			const char* text = e.what();
			text = text;
		}
		delete project;
	}

	// No error
	return 0;
}
