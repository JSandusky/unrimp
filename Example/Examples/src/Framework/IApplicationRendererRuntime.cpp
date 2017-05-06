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
#include "PrecompiledHeader.h"

#include <exception>


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_RUNTIME


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/IApplicationRendererRuntime.h"

#ifdef SHARED_LIBRARIES
	#include <RendererToolkit/Public/RendererToolkitInstance.h>
#endif

#include <RendererRuntime/Public/RendererRuntimeInstance.h>
#include <RendererRuntime/Core/File/StdFileManager.h>
#include <RendererRuntime/Asset/AssetManager.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
IApplicationRendererRuntime::IApplicationRendererRuntime(const char *rendererName, ExampleBase* exampleBase) :
	IApplicationRenderer(rendererName, exampleBase),
	mFileManager(nullptr),
	mRendererRuntimeInstance(nullptr)
	#ifdef SHARED_LIBRARIES
		, mRendererToolkitInstance(nullptr)
		, mProject(nullptr)
	#endif
{
	// Nothing here
}

IApplicationRendererRuntime::~IApplicationRendererRuntime()
{
	// Nothing here

	// "mFileManager" and "mRendererRuntimeInstance" is destroyed within "onDeinitialization()"
}


//[-------------------------------------------------------]
//[ Public virtual IApplicationFrontend methods           ]
//[-------------------------------------------------------]
RendererRuntime::IRendererRuntime *IApplicationRendererRuntime::getRendererRuntime() const
{
	return (nullptr != mRendererRuntimeInstance) ? mRendererRuntimeInstance->getRendererRuntime() : nullptr;
}

RendererToolkit::IRendererToolkit *IApplicationRendererRuntime::getRendererToolkit()
{
	#ifdef SHARED_LIBRARIES
		// Create the renderer toolkit instance, if required
		if (nullptr == mRendererToolkitInstance)
		{
			assert(nullptr != mRendererRuntimeInstance && "The renderer runtime instance must be valid");
			mRendererToolkitInstance = new RendererToolkit::RendererToolkitInstance(mRendererRuntimeInstance->getRendererRuntime()->getFileManager());
		}
		return (nullptr != mRendererToolkitInstance) ? mRendererToolkitInstance->getRendererToolkit() : nullptr;
	#else
		return nullptr;
	#endif
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void IApplicationRendererRuntime::onInitialization()
{
	// Don't call the base, this would break examples which depends on renderer runtime instance

	// Create the renderer instance
	createRenderer();

	// Is there a valid renderer instance?
	Renderer::IRenderer *renderer = getRenderer();
	if (nullptr != renderer)
	{
		// Create the renderer runtime instance
		mFileManager = new RendererRuntime::StdFileManager();
		mRendererRuntimeInstance = new RendererRuntime::RendererRuntimeInstance(*renderer, *mFileManager);

		{
			RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
			if (nullptr != rendererRuntime)
			{
				// TODO(co) Under construction: Will probably become "mount asset package"
				// Add used asset package
				const bool rendererIsOpenGLES = (0 == strcmp(renderer->getName(), "OpenGLES3"));
				if (rendererIsOpenGLES)
				{
					rendererRuntime->getAssetManager().addAssetPackageByFilename("Example/Content", "../DataMobile/Content/AssetPackage.assets");
				}
				else
				{
					rendererRuntime->getAssetManager().addAssetPackageByFilename("Example/Content", "../DataPc/Content/AssetPackage.assets");
				}
				rendererRuntime->loadPipelineStateObjectCache();

				#ifdef SHARED_LIBRARIES
				{
					// TODO(co) First asset hot-reloading test
					RendererToolkit::IRendererToolkit* rendererToolkit = getRendererToolkit();
					if (nullptr != rendererToolkit)
					{
						mProject = rendererToolkit->createProject();
						if (nullptr != mProject)
						{
							try
							{
								mProject->loadByFilename("../DataSource/Example.project");
								if (rendererIsOpenGLES)
								{
									mProject->startupAssetMonitor(*rendererRuntime, "OpenGLES3_300");
								}
								else
								{
									mProject->startupAssetMonitor(*rendererRuntime, "Direct3D11_50");
								}
							}
							catch (const std::exception& e)
							{
								const char* text = e.what();
								text = text;
							}
						}
					}
				}
				#endif
			}
		}
	}

	// Initialize the example now that the renderer instance should be created successfully
	initializeExample();
}

void IApplicationRendererRuntime::onDeinitialization()
{
	// Deinitinitialize example before we tear down any dependencies
	// -> The base class calls this too but this is safe to do because the deinitialization is only done when the example wasn't already deinitialized
	deinitializeExample();

	// Delete the renderer runtime instance
	delete mRendererRuntimeInstance;
	mRendererRuntimeInstance = nullptr;
	delete static_cast<RendererRuntime::StdFileManager*>(mFileManager);
	mFileManager = nullptr;
	#ifdef SHARED_LIBRARIES
		if (nullptr != mProject)
		{
			delete mProject;
			mProject = nullptr;
		}
		if (nullptr != mRendererToolkitInstance)
		{
			delete mRendererToolkitInstance;
			mRendererToolkitInstance = nullptr;
		}
	#endif

	// Call the base implementation
	IApplicationRenderer::onDeinitialization();
}

void IApplicationRendererRuntime::onUpdate()
{
	RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
	if (nullptr != rendererRuntime)
	{
		rendererRuntime->update();
	}

	// Call base implementation
	IApplicationRenderer::onUpdate();
}


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
IApplicationRendererRuntime::IApplicationRendererRuntime(const char *rendererName) :
	IApplicationRendererRuntime(rendererName, nullptr)
{
	// Nothing here
}


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#endif // RENDERER_NO_RUNTIME
