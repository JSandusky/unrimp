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


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#ifndef RENDERER_NO_RUNTIME


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Runtime/FirstCompositor/CompositorPassFactoryFirst.h"
#include "Runtime/FirstCompositor/CompositorResourcePassFirst.h"
#include "Runtime/FirstCompositor/CompositorInstancePassFirst.h"


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
CompositorPassFactoryFirst::CompositorPassFactoryFirst()
{
	// Nothing here
}

CompositorPassFactoryFirst::~CompositorPassFactoryFirst()
{
	// Nothing here
}


//[-------------------------------------------------------]
//[ Protected virtual RendererRuntime::ICompositorPassFactory methods ]
//[-------------------------------------------------------]
RendererRuntime::ICompositorResourcePass* CompositorPassFactoryFirst::createCompositorResourcePass(RendererRuntime::CompositorPassTypeId compositorPassTypeId) const
{
	// First, let the base implementation try to create an instance
	RendererRuntime::ICompositorResourcePass* compositorResourcePass = CompositorPassFactory::createCompositorResourcePass(compositorPassTypeId);
	if (nullptr == compositorResourcePass)
	{
		// Evaluate the compositor pass type
		if (compositorPassTypeId == CompositorResourcePassFirst::TYPE_ID)
		{
			compositorResourcePass = new CompositorResourcePassFirst();
		}
	}

	// Done
	return compositorResourcePass;
}

RendererRuntime::ICompositorInstancePass* CompositorPassFactoryFirst::createCompositorInstancePass(const RendererRuntime::ICompositorResourcePass& compositorResourcePass, const RendererRuntime::CompositorInstanceNode& compositorInstanceNode) const
{
	// First, let the base implementation try to create an instance
	RendererRuntime::ICompositorInstancePass* compositorInstancePass = CompositorPassFactory::createCompositorInstancePass(compositorResourcePass, compositorInstanceNode);

	// Evaluate the compositor pass type
	if (nullptr == compositorInstancePass && compositorResourcePass.getTypeId() == CompositorResourcePassFirst::TYPE_ID)
	{
		compositorInstancePass = new CompositorInstancePassFirst(static_cast<const CompositorResourcePassFirst&>(compositorResourcePass), compositorInstanceNode);
	}

	// Done
	return compositorInstancePass;
}


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#endif // RENDERER_NO_RUNTIME
