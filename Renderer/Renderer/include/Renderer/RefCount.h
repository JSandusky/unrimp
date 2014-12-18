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
#ifndef __RENDERER_REFCOUNT_H__
#define __RENDERER_REFCOUNT_H__


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
	*    Reference counter template
	*
	*  @note
	*    - Initially the reference counter is 0
	*/
	template <class AType>
	class RefCount
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		inline RefCount();

		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~RefCount();

		/**
		*  @brief
		*    Get a pointer to the object
		*
		*  @return
		*    Pointer to the reference counter's object, never a null pointer!
		*/
		inline virtual const AType *getPointer() const;

		/**
		*  @brief
		*    Get a pointer to the object
		*
		*  @return
		*    Pointer to the reference counter's object, never a null pointer!
		*/
		inline virtual AType *getPointer();

		/**
		*  @brief
		*    Increases the reference count
		*
		*  @return
		*    Current reference count
		*/
		inline unsigned int addReference();

		/**
		*  @brief
		*    Decreases the reference count
		*
		*  @return
		*    Current reference count
		*
		*  @note
		*    - When the last reference was released, the instance is destroyed automatically
		*/
		inline unsigned int release();

		/**
		*  @brief
		*    Gets the current reference count
		*
		*  @return
		*    Current reference count
		*/
		inline unsigned int getRefCount() const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		unsigned int mRefCount; ///< Reference count


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/RefCount.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_REFCOUNT_H__
