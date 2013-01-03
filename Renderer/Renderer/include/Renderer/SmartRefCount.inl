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
#include "Renderer/RefCount.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Default constructor
	*/
	template <class AType>
	inline SmartRefCount<AType>::SmartRefCount() :
		mPtr(nullptr)
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Constructor
	*/
	template <class AType>
	inline SmartRefCount<AType>::SmartRefCount(AType *ptr) :
		mPtr(nullptr)
	{
		setPtr(ptr);
	}

	/**
	*  @brief
	*    Copy constructor
	*/
	template <class AType>
	inline SmartRefCount<AType>::SmartRefCount(const SmartRefCount<AType> &ptr) :
		mPtr(nullptr)
	{
		setPtr(ptr.getPtr());
	}

	/**
	*  @brief
	*    Destructor
	*/
	template <class AType>
	inline SmartRefCount<AType>::~SmartRefCount()
	{
		setPtr(nullptr);
	}

	/**
	*  @brief
	*    Assign a pointer
	*/
	template <class AType>
	inline SmartRefCount<AType> &SmartRefCount<AType>::operator =(AType *ptr)
	{
		if (getPointer() != ptr)
		{
			setPtr(ptr);
		}
		return *this;
	}

	/**
	*  @brief
	*    Assign a smart pointer
	*/
	template <class AType>
	inline SmartRefCount<AType> &SmartRefCount<AType>::operator =(const SmartRefCount<AType> &ptr)
	{
		if (getPointer() != ptr.getPointer())
		{
			setPtr(ptr.getPtr());
		}
		return *this;
	}

	/**
	*  @brief
	*    Get a direct pointer to the object
	*/
	template <class AType>
	inline AType *SmartRefCount<AType>::getPointer() const
	{
		return mPtr ? static_cast<AType*>(mPtr->getPointer()) : nullptr;
	}

	/**
	*  @brief
	*    Get a pointer to access the object
	*/
	template <class AType>
	inline AType *SmartRefCount<AType>::operator ->() const
	{
		return getPointer();
	}

	/**
	*  @brief
	*    Cast to a pointer to the object
	*/
	template <class AType>
	inline SmartRefCount<AType>::operator AType*() const
	{
		return getPointer();
	}

	/**
	*  @brief
	*    Check if the pointer is not a null pointer
	*/
	template <class AType>
	inline bool SmartRefCount<AType>::operator !() const
	{
		return (nullptr == getPointer());
	}

	/**
	*  @brief
	*    Check for equality
	*/
	template <class AType>
	inline bool SmartRefCount<AType>::operator ==(AType *ptr) const
	{
		return (getPointer() == ptr);
	}

	/**
	*  @brief
	*    Check for equality
	*/
	template <class AType>
	inline bool SmartRefCount<AType>::operator ==(const SmartRefCount<AType> &ptr) const
	{
		return (getPointer() == ptr.getPointer());
	}

	/**
	*  @brief
	*    Check for equality
	*/
	template <class AType>
	inline bool SmartRefCount<AType>::operator !=(AType *ptr) const
	{
		return (getPointer() != ptr);
	}

	/**
	*  @brief
	*    Check for equality
	*/
	template <class AType>
	inline bool SmartRefCount<AType>::operator !=(const SmartRefCount<AType> &ptr) const
	{
		return (getPointer() != ptr.getPointer());
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Assign a pointer to an object that implements RefCount
	*/
	template <class AType>
	inline void SmartRefCount<AType>::setPtr(AType *ptr)
	{
		// Release old pointer
		if (nullptr != mPtr)
		{
			mPtr->release();
		}

		// Assign new pointer
		if (nullptr != ptr)
		{
			ptr->addReference();
		}
		mPtr = ptr;
	}

	/**
	*  @brief
	*    Get pointer to the reference counted object
	*/
	template <class AType>
	inline AType *SmartRefCount<AType>::getPtr() const
	{
		// Return pointer
		return mPtr;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
