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
#ifndef RENDERER_NO_STATISTICS
	#include "Renderer/IRenderer.h"
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline IUniformBuffer::~IUniformBuffer()
	{
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			--getRenderer().getStatistics().currentNumberOfUniformBuffers;
		#endif
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline IUniformBuffer::IUniformBuffer(IRenderer &renderer) :
		IBuffer(ResourceType::UNIFORM_BUFFER, renderer)
	{
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			++getRenderer().getStatistics().numberOfCreatedUniformBuffers;
			++getRenderer().getStatistics().currentNumberOfUniformBuffers;
		#endif
	}

	inline IUniformBuffer::IUniformBuffer(const IUniformBuffer &source) :
		IBuffer(source)
	{
		// Not supported
		#ifndef RENDERER_NO_STATISTICS
			// Update the statistics
			++getRenderer().getStatistics().numberOfCreatedUniformBuffers;
			++getRenderer().getStatistics().currentNumberOfUniformBuffers;
		#endif
	}

	inline IUniformBuffer &IUniformBuffer::operator =(const IUniformBuffer &)
	{
		// Not supported
		return *this;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
