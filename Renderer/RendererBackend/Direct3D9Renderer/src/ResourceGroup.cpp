/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "Direct3D9Renderer/ResourceGroup.h"

#include <Renderer/IRenderer.h>
#include <Renderer/IAllocator.h>
#include <Renderer/State/ISamplerState.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ResourceGroup::ResourceGroup(Renderer::IRenderer& renderer, uint32_t rootParameterIndex, uint32_t numberOfResources, Renderer::IResource** resources, Renderer::ISamplerState** samplerStates) :
		IResourceGroup(renderer),
		mRootParameterIndex(rootParameterIndex),
		mNumberOfResources(numberOfResources),
		mResources(RENDERER_MALLOC_TYPED(renderer.getContext(), Renderer::IResource*, mNumberOfResources)),
		mSamplerStates(nullptr)
	{
		// Process all resources and add our reference to the renderer resource
		for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex, ++resources)
		{
			// Since Direct3D 9 doesn't support e.g. uniform buffer we need to check for null pointers here
			Renderer::IResource* resource = *resources;
			mResources[resourceIndex] = resource;
			if (nullptr != resource)
			{
				resource->addReference();
			}
		}
		if (nullptr != samplerStates)
		{
			mSamplerStates = RENDERER_MALLOC_TYPED(renderer.getContext(), Renderer::ISamplerState*, mNumberOfResources);
			for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex)
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
		const Renderer::Context& context = getRenderer().getContext();
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
			RENDERER_FREE(context, mSamplerStates);
		}
		for (uint32_t resourceIndex = 0; resourceIndex < mNumberOfResources; ++resourceIndex)
		{
			// Since Direct3D 9 doesn't support e.g. uniform buffer we need to check for null pointers here
			if (nullptr != mResources[resourceIndex])
			{
				mResources[resourceIndex]->releaseReference();
			}
		}
		RENDERER_FREE(context, mResources);
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void ResourceGroup::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), ResourceGroup, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
