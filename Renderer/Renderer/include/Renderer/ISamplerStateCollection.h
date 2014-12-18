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
#ifndef __RENDERER_ISAMPLERSTATECOLLECTION_H__
#define __RENDERER_ISAMPLERSTATECOLLECTION_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Renderer/ICollection.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract sampler state collection interface
	*
	*  @remarks
	*    Use sampler state collections when you want you exploit renderer API methods like
	*    "ID3D10Device::PSSetSamplers()" from Direct3D 10 or "ID3D11DeviceContext::PSSetSamplers()" from Direct3D 11.
	*    By using a single API call, multiple resources can be set at one and the same time in an efficient way.
	*/
	class ISamplerStateCollection : public ICollection
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~ISamplerStateCollection();


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Owner renderer instance
		*/
		inline ISamplerStateCollection(IRenderer &renderer);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit ISamplerStateCollection(const ISamplerStateCollection &source);

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		inline ISamplerStateCollection &operator =(const ISamplerStateCollection &source);


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<ISamplerStateCollection> ISamplerStateCollectionPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/ISamplerStateCollection.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_ISAMPLERSTATECOLLECTION_H__
