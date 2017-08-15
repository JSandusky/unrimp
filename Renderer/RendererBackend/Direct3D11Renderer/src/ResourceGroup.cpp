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
#include "Direct3D11Renderer/ResourceGroup.h"

#include <Renderer/State/ISamplerState.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ResourceGroup::ResourceGroup(Renderer::IRenderer& renderer, uint32_t rootParameterIndex, uint32_t numberOfResources, Renderer::IResource** resources, Renderer::ISamplerState** samplerStates) :
		IResourceGroup(renderer),
		mRootParameterIndex(rootParameterIndex),
		mNumberOfResources(numberOfResources),
		mResources(new Renderer::IResource*[mNumberOfResources]),
		mSamplerStates(nullptr)
	{
		// Process all resources and add our reference to the renderer resource
		for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex, ++resources)
		{
			Renderer::IResource* resource = *resources;
			mResources[resourceIndex] = resource;
			resource->addReference();
		}
		if (nullptr != samplerStates)
		{
			mSamplerStates = new Renderer::ISamplerState*[mNumberOfResources];
			for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex, ++resources)
			{
				Renderer::ISamplerState* samplerState = mSamplerStates[resourceIndex] = samplerStates[resourceIndex];
				if (nullptr != samplerState)
				{
					samplerState->addReference();
				}
			}
		}
	}

	ResourceGroup::~ResourceGroup()
	{
		// Remove our reference from the renderer resources
		if (nullptr != mSamplerStates)
		{
			for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex)
			{
				Renderer::ISamplerState* samplerState = mSamplerStates[resourceIndex];
				if (nullptr != samplerState)
				{
					samplerState->releaseReference();
				}
			}
			delete [] mSamplerStates;
		}
		for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex)
		{
			mResources[resourceIndex]->releaseReference();
		}
		delete [] mResources;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer
