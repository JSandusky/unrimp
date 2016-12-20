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
#include "Runtime/FirstCompositor/CompositorResourcePassFirst.h"


//[-------------------------------------------------------]
//[ Public definitions                                    ]
//[-------------------------------------------------------]
const RendererRuntime::CompositorPassTypeId CompositorResourcePassFirst::TYPE_ID("FirstCompositorExample");


//[-------------------------------------------------------]
//[ Public virtual RendererRuntime::ICompositorResourcePass methods ]
//[-------------------------------------------------------]
RendererRuntime::CompositorPassTypeId CompositorResourcePassFirst::getTypeId() const
{
	return TYPE_ID;
}

void CompositorResourcePassFirst::deserialize(uint32_t, const uint8_t*)
{
	// Nothing here
}


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
CompositorResourcePassFirst::CompositorResourcePassFirst(const RendererRuntime::CompositorTarget& compositorTarget) :
	RendererRuntime::ICompositorResourcePass(compositorTarget)
{
	// Nothing here
}

CompositorResourcePassFirst::~CompositorResourcePassFirst()
{
	// Nothing here
}


//[-------------------------------------------------------]
//[ Preprocessor                                          ]
//[-------------------------------------------------------]
#endif // RENDERER_NO_RUNTIME
