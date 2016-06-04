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
#include "Runtime/FirstCompositor/CompositorInstancePassFirst.h"
#include "Runtime/FirstCompositor/CompositorResourcePassFirst.h"
#include "Framework/Color4.h"

#include <RendererRuntime/IRendererRuntime.h>
#include <RendererRuntime/Resource/Font/FontResourceManager.h>
#include <RendererRuntime/Resource/Compositor/CompositorInstance.h>
#include <RendererRuntime/Resource/Compositor/CompositorInstanceNode.h>


//[-------------------------------------------------------]
//[ Protected virtual RendererRuntime::ICompositorInstancePass methods ]
//[-------------------------------------------------------]
void CompositorInstancePassFirst::execute(RendererRuntime::CameraSceneItem*)
{
	// Draw text
	// TODO(co) Get rid of the evil const-cast
	RendererRuntime::FontResource* fontResource = const_cast<RendererRuntime::FontResource*>(getCompositorInstanceNode().getCompositorInstance().getRendererRuntime().getFontResourceManager().getFontResources().tryGetElementById(mFontResourceId));
	if (nullptr != fontResource)
	{
		fontResource->drawText("42", Color4::GREEN, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 0.0f))), 0.005f, 0.005f);
	}
}


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
CompositorInstancePassFirst::CompositorInstancePassFirst(const CompositorResourcePassFirst& compositorResourcePassFirst, const RendererRuntime::CompositorInstanceNode& compositorInstanceNode) :
	ICompositorInstancePass(compositorResourcePassFirst, compositorInstanceNode),
	mFontResourceId(getCompositorInstanceNode().getCompositorInstance().getRendererRuntime().getFontResourceManager().loadFontResourceByAssetId("Example/Font/Default/LinBiolinum_R"))
{
	// Nothing here
}

CompositorInstancePassFirst::~CompositorInstancePassFirst()
{
	// Nothing here
}


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#endif // RENDERER_NO_RUNTIME
