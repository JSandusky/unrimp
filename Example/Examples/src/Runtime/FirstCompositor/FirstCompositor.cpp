/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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

#include <RendererRuntime/Core/Vector3.h>
#include <RendererRuntime/Resource/Font/FontResourceManager.h>


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_RUNTIME


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Runtime/FirstCompositor/FirstCompositor.h"
#include "Framework/Color4.h"

#include <RendererRuntime/Asset/AssetManager.h>
#include <RendererRuntime/Resource/Compositor/CompositorWorkspace.h>

#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/matrix_transform.hpp>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstCompositor::FirstCompositor(const char *rendererName) :
	IApplicationRendererRuntime(rendererName),
	mCompositorWorkspace(nullptr),
	mFontResource(nullptr)
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
	RendererRuntime::IRendererRuntimePtr rendererRuntime(getRendererRuntime());
	if (nullptr != rendererRuntime)
	{
		// TODO(co) Under construction: Will probably become "mount asset package"
		// Add used asset package
		rendererRuntime->getAssetManager().addAssetPackageByFilename("../DataPc/Content/AssetPackage.assets");

		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

		// Create the compositor workspace
		mCompositorWorkspace = new RendererRuntime::CompositorWorkspace(*rendererRuntime, "Example/Compositor/Default/FirstCompositor");

		// Create the font resource
		mFontResource = rendererRuntime->getFontResourceManager().loadFontResourceByAssetId("Example/Font/Default/LinBiolinum_R");

		// End debug event
		RENDERER_END_DEBUG_EVENT(getRenderer())
	}
}

void FirstCompositor::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// TODO(co) Implement decent resource handling
	delete mCompositorWorkspace;
	mFontResource = nullptr;

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())

	// Call the base implementation
	IApplicationRendererRuntime::onDeinitialization();
}

void FirstCompositor::onDraw()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// TODO(co)
		// mCompositorWorkspace->update();

		// Clear the color buffer of the current render target with gray, do also clear the depth buffer
		renderer->clear(Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

		// Draw text
		if (nullptr != mFontResource)
		{
			mFontResource->drawText("42", Color4::GREEN, glm::value_ptr(glm::translate(glm::mat4(1.0f), RendererRuntime::Vector3(0.0f, 0.3f, 0.0f))), 0.005f, 0.005f);
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#endif // RENDERER_NO_RUNTIME
