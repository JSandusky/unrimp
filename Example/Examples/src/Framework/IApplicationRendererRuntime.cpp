/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
IApplicationRendererRuntime::~IApplicationRendererRuntime()
{
	// Nothing to do in here
	// mRendererRuntimeInstance is destroyed within onDeinitialization()
}

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
			mRendererToolkitInstance = new RendererToolkit::RendererToolkitInstance();
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
	// Call the base implementation
	IApplicationRenderer::onInitialization();

	// Is there a valid renderer instance?
	Renderer::IRenderer *renderer = getRenderer();
	if (nullptr != renderer)
	{
		// Create the renderer runtime instance
		mRendererRuntimeInstance = new RendererRuntime::RendererRuntimeInstance(*renderer);
	}
}

void IApplicationRendererRuntime::onDeinitialization()
{
	// Delete the renderer runtime instance
	delete mRendererRuntimeInstance;
	mRendererRuntimeInstance = nullptr;
	#ifdef SHARED_LIBRARIES
		if (nullptr != mRendererToolkitInstance)
		{
			delete mRendererToolkitInstance;
			mRendererToolkitInstance = nullptr;
		}
	#endif
}

void IApplicationRendererRuntime::onUpdate()
{
	RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
	if (nullptr != rendererRuntime)
	{
		rendererRuntime->update();
	}
}


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
IApplicationRendererRuntime::IApplicationRendererRuntime(const char *rendererName) :
	IApplicationRenderer(rendererName),
	mRendererRuntimeInstance(nullptr)
{
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#endif // RENDERER_NO_RUNTIME
