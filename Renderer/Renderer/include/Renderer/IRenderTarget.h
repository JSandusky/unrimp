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
#ifndef __RENDERER_IRENDERTARGET_H__
#define __RENDERER_IRENDERTARGET_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Renderer/IResource.h"


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
	*    Abstract render target interface
	*/
	class IRenderTarget : public IResource
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~IRenderTarget();


	//[-------------------------------------------------------]
	//[ Public virtual IRenderTarget methods                  ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the width and height of the render target
		*
		*  @param[out] width
		*    Receives the width of the render target
		*  @param[out] height
		*    Receives the height of the render target
		*/
		virtual void getWidthAndHeight(unsigned int &width, unsigned int &height) const = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] resourceType
		*    The resource type
		*  @param[in] renderer
		*    Owner renderer instance
		*/
		inline IRenderTarget(ResourceType::Enum resourceType, IRenderer &renderer);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit IRenderTarget(const IRenderTarget &source);

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
		inline IRenderTarget &operator =(const IRenderTarget &source);


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<IRenderTarget> IRenderTargetPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/IRenderTarget.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_IRENDERTARGET_H__
