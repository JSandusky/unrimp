/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "Direct3D10Renderer/SamplerStateCollection.h"
#include "Direct3D10Renderer/SamplerState.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SamplerStateCollection::SamplerStateCollection(Direct3D10Renderer &direct3D10Renderer, uint32_t numberOfSamplerStates, Renderer::ISamplerState **samplerStates) :
		Renderer::ISamplerStateCollection(direct3D10Renderer),
		mNumberOfD3D10SamplerStates(numberOfSamplerStates),
		mD3D10SamplerStates((numberOfSamplerStates > 0) ? new ID3D10SamplerState*[numberOfSamplerStates] : nullptr),
		mSamplerStates((numberOfSamplerStates > 0) ? new Renderer::ISamplerState*[numberOfSamplerStates] : nullptr)
	{
		// Loop through all given textures and gather the Direct3D 10 shader resource views
		ID3D10SamplerState		**currentD3D10SamplerState = mD3D10SamplerStates;
		Renderer::ISamplerState **currentSamplerState	   = mSamplerStates;
		Renderer::ISamplerState **samplerStateEnd		   = mSamplerStates + mNumberOfD3D10SamplerStates;
		for (; currentSamplerState < samplerStateEnd; ++currentD3D10SamplerState, ++currentSamplerState, ++samplerStates)
		{
			// Get the current sampler state
			Renderer::ISamplerState *samplerState = *samplerStates;

			// Add a sampler state reference
			samplerState->addReference();
			*currentSamplerState = samplerState;

			// Security check: Is the given resource owned by this renderer?
			#ifndef DIRECT3D10RENDERER_NO_RENDERERMATCHCHECK
				if (&direct3D10Renderer != &samplerState->getRenderer())
				{
					// Output an error message and keep on going in order to keep a resonable behaviour even in case on an error
					RENDERER_OUTPUT_DEBUG_PRINTF("Direct3D 10 error: The given resource at index %d is owned by another renderer instance", currentSamplerState - mSamplerStates)
					*currentD3D10SamplerState = nullptr;
					continue;
				}
			#endif

			// Get the Direct3D 10 sampler state
			*currentD3D10SamplerState = static_cast<SamplerState*>(samplerState)->getD3D10SamplerState();
		}
	}

	SamplerStateCollection::~SamplerStateCollection()
	{
		// Release the allocated memory
		if (nullptr != mD3D10SamplerStates)
		{
			delete [] mD3D10SamplerStates;
		}
		if (nullptr != mSamplerStates)
		{
			// Loop through all given sampler states and release our references
			Renderer::ISamplerState **currentSamplerState = mSamplerStates;
			Renderer::ISamplerState **samplerStateEnd	  = mSamplerStates + mNumberOfD3D10SamplerStates;
			for (; currentSamplerState < samplerStateEnd; ++currentSamplerState)
			{
				(*currentSamplerState)->release();
			}

			// Release the allocated memory
			delete [] mSamplerStates;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void SamplerStateCollection::setDebugName(const char *)
	{
		// In here we could assign the given debug name to all sampler states assigned to the
		// sampler state collection, but this might end up within a naming chaos due to overwriting
		// possible already set names... don't do this...
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
