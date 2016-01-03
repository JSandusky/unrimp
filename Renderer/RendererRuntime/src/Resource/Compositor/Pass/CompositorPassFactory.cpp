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
#include "RendererRuntime/Resource/Compositor/Pass/CompositorPassFactory.h"
#include "RendererRuntime/Resource/Compositor/Pass/Clear/CompositorResourcePassClear.h"
#include "RendererRuntime/Resource/Compositor/Pass/Clear/CompositorInstancePassClear.h"
#include "RendererRuntime/Resource/Compositor/Pass/Quad/CompositorResourcePassQuad.h"
#include "RendererRuntime/Resource/Compositor/Pass/Quad/CompositorInstancePassQuad.h"
#include "RendererRuntime/Resource/Compositor/Pass/Scene/CompositorResourcePassScene.h"
#include "RendererRuntime/Resource/Compositor/Pass/Scene/CompositorInstancePassScene.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	CompositorPassFactory::CompositorPassFactory()
	{
		// Nothing here
	}

	CompositorPassFactory::~CompositorPassFactory()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::ICompositorPassFactory methods ]
	//[-------------------------------------------------------]
	ICompositorResourcePass* CompositorPassFactory::createCompositorResourcePass(CompositorPassTypeId compositorPassTypeId) const
	{
		ICompositorResourcePass* compositorResourcePass = nullptr;

		// Evaluate the compositor pass type
		if (compositorPassTypeId == CompositorResourcePassClear::TYPE_ID)
		{
			compositorResourcePass = new CompositorResourcePassClear();
		}
		else if (compositorPassTypeId == CompositorResourcePassQuad::TYPE_ID)
		{
			compositorResourcePass = new CompositorResourcePassQuad();
		}
		else if (compositorPassTypeId == CompositorResourcePassScene::TYPE_ID)
		{
			compositorResourcePass = new CompositorResourcePassScene();
		}

		// Done
		return compositorResourcePass;
	}

	ICompositorInstancePass* CompositorPassFactory::createCompositorInstancePass(const ICompositorResourcePass& compositorResourcePass, const CompositorInstanceNode& compositorInstanceNode) const
	{
		ICompositorInstancePass* compositorInstancePass = nullptr;

		// Evaluate the compositor pass type
		const CompositorPassTypeId compositorPassTypeId = compositorResourcePass.getTypeId();
		if (compositorPassTypeId == CompositorResourcePassClear::TYPE_ID)
		{
			compositorInstancePass = new CompositorInstancePassClear(static_cast<const CompositorResourcePassClear&>(compositorResourcePass), compositorInstanceNode);
		}
		else if (compositorPassTypeId == CompositorResourcePassQuad::TYPE_ID)
		{
			compositorInstancePass = new CompositorInstancePassQuad(static_cast<const CompositorResourcePassQuad&>(compositorResourcePass), compositorInstanceNode);
		}
		else if (compositorPassTypeId == CompositorResourcePassScene::TYPE_ID)
		{
			compositorInstancePass = new CompositorInstancePassScene(static_cast<const CompositorResourcePassScene&>(compositorResourcePass), compositorInstanceNode);
		}

		// Done
		return compositorInstancePass;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
