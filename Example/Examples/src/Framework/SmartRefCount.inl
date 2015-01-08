/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "Framework/RefCount.h"


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
template <class AType>
SmartRefCount<AType>::SmartRefCount() :
	mPtr(nullptr)
{
	// Nothing to do in here
}

template <class AType>
SmartRefCount<AType>::SmartRefCount(AType *ptr) :
	mPtr(nullptr)
{
	setPtr(ptr);
}

template <class AType>
SmartRefCount<AType>::SmartRefCount(const SmartRefCount<AType> &ptr) :
	mPtr(nullptr)
{
	setPtr(ptr.getPtr());
}

template <class AType>
SmartRefCount<AType>::~SmartRefCount()
{
	setPtr(nullptr);
}

template <class AType>
SmartRefCount<AType> &SmartRefCount<AType>::operator =(AType *ptr)
{
	if (getPointer() != ptr)
	{
		setPtr(ptr);
	}
	return *this;
}

template <class AType>
SmartRefCount<AType> &SmartRefCount<AType>::operator =(const SmartRefCount<AType> &ptr)
{
	if (getPointer() != ptr.getPointer())
	{
		setPtr(ptr.getPtr());
	}
	return *this;
}

template <class AType>
AType *SmartRefCount<AType>::getPointer() const
{
	return mPtr ? static_cast<AType*>(mPtr->getPointer()) : nullptr;
}

template <class AType>
AType *SmartRefCount<AType>::operator ->() const
{
	return getPointer();
}

template <class AType>
SmartRefCount<AType>::operator AType*() const
{
	return getPointer();
}

template <class AType>
bool SmartRefCount<AType>::operator !() const
{
	return (nullptr == getPointer());
}

template <class AType>
bool SmartRefCount<AType>::operator ==(AType *ptr) const
{
	return (getPointer() == ptr);
}

template <class AType>
bool SmartRefCount<AType>::operator ==(const SmartRefCount<AType> &ptr) const
{
	return (getPointer() == ptr.getPointer());
}

template <class AType>
bool SmartRefCount<AType>::operator !=(AType *ptr) const
{
	return (getPointer() != ptr);
}

template <class AType>
bool SmartRefCount<AType>::operator !=(const SmartRefCount<AType> &ptr) const
{
	return (getPointer() != ptr.getPointer());
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
template <class AType>
void SmartRefCount<AType>::setPtr(AType *ptr)
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

template <class AType>
AType *SmartRefCount<AType>::getPtr() const
{
	// Return pointer
	return mPtr;
}
