/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/SamplerStateDsa.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	SamplerStateDsa::SamplerStateDsa(OpenGLRenderer &openGLRenderer, const Renderer::SamplerState &samplerState) :
		SamplerState(openGLRenderer),
		mSamplerState(samplerState)
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Destructor
	*/
	SamplerStateDsa::~SamplerStateDsa()
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Set the OpenGL sampler states
	*/
	void SamplerStateDsa::setOpenGLSamplerStates() const
	{
		// TODO(co) Implement me
		// http://www.opengl.org/registry/specs/ARB/sampler_objects.txt - GL_ARB_sampler_objects
		// http://www.ozone3d.net/blogs/lab/20110908/tutorial-opengl-3-3-sampler-states-configurer-unites-de-texture/#more-701 - sample
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
