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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __DIRECT3D11RENDERER_SAMPLERSTATECOLLECTION_H__
#define __DIRECT3D11RENDERER_SAMPLERSTATECOLLECTION_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/ISamplerStateCollection.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct ID3D11SamplerState;
namespace Renderer
{
	class ISamplerState;
}
namespace Direct3D11Renderer
{
	class Direct3D11Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 11 sampler state collection class
	*/
	class SamplerStateCollection : public Renderer::ISamplerStateCollection
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D11Renderer
		*    Owner Direct3D 11 renderer instance
		*  @param[in] numberOfSamplerStates
		*    The number of sampler states
		*  @param[in] samplerStates
		*    The sampler states, can be a null pointer or can contain null pointers, if not a null pointer there must be at
		*    least "numberOfSamplerStates" sampler states in the provided C-array of pointers
		*
		*  @note
		*    - The sampler state collection keeps a reference to the provided sampler state instances
		*/
		SamplerStateCollection(Direct3D11Renderer &direct3D11Renderer, uint32_t numberOfSamplerStates, Renderer::ISamplerState **samplerStates);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~SamplerStateCollection();

		/**
		*  @brief
		*    Return the number of Direct3D sampler state instances
		*
		*  @return
		*    The number of Direct3D sampler state instances
		*/
		inline uint32_t getNumberOfD3D11SamplerStates() const;

		/**
		*  @brief
		*    Return the Direct3D sampler state instances
		*
		*  @return
		*    The Direct3D sampler state instances, can be a null pointer, do not release the returned instances unless you added an own reference to it
		*/
		inline ID3D11SamplerState **getD3D11SamplerStates() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char *name) override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		uint32_t				  mNumberOfD3D11SamplerStates;	///< The number of Direct3D sampler state instances
		ID3D11SamplerState		**mD3D11SamplerStates;			///< The Direct3D sampler state instances, can be a null pointer
		Renderer::ISamplerState **mSamplerStates;				///< Sampler state instances (we keep a reference to it), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/SamplerStateCollection.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D11RENDERER_SAMPLERSTATECOLLECTION_H__
