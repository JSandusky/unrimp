/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "Runtime/FirstCompositor/FirstCompositor.h"
#include "Runtime/FirstCompositor/CompositorPassFactoryFirst.h"

#include <RendererRuntime/Resource/Compositor/CompositorInstance.h>
#include <RendererRuntime/Resource/Compositor/CompositorResourceManager.h>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global variables                                      ]
		//[-------------------------------------------------------]
		static const CompositorPassFactoryFirst compositorPassFactoryFirst;


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstCompositor::FirstCompositor(const char *rendererName) :
	IApplicationRendererRuntime(rendererName),
	mCompositorInstance(nullptr)
{
	// Nothing to do in here
}

FirstCompositor::~FirstCompositor()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstCompositor::onInitialization()
{
	// Call the base implementation
	IApplicationRendererRuntime::onInitialization();

	// Get and check the renderer runtime instance
	RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
	if (nullptr != rendererRuntime)
	{
		// Set our custom compositor pass factory
		rendererRuntime->getCompositorResourceManager().setCompositorPassFactory(&::detail::compositorPassFactoryFirst);

		// Create the compositor instance
		mCompositorInstance = new RendererRuntime::CompositorInstance(*rendererRuntime, "Example/Compositor/Default/FirstCompositor");
	}
}

void FirstCompositor::onDeinitialization()
{
	// TODO(co) Implement decent resource handling
	delete mCompositorInstance;
	mCompositorInstance = nullptr;

	// Be polite and unset our custom compositor pass factory
	RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
	if (nullptr != rendererRuntime)
	{
		rendererRuntime->getCompositorResourceManager().setCompositorPassFactory(nullptr);
	}

	// Call the base implementation
	IApplicationRendererRuntime::onDeinitialization();
}

void FirstCompositor::onDrawRequest()
{
	// Is there a compositor instance?
	if (nullptr != mCompositorInstance)
	{
		// Get the main swap chain and ensure there's one
		Renderer::ISwapChain* swapChain = getRenderer()->getMainSwapChain();
		if (nullptr != swapChain)
		{
			// Execute the compositor instance
			mCompositorInstance->execute(*swapChain, nullptr);
		}
	}
}


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#endif // RENDERER_NO_RUNTIME
