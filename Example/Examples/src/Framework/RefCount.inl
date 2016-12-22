/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
//[ Public functions                                      ]
//[-------------------------------------------------------]
template <class AType>
RefCount<AType>::RefCount() :
	mRefCount(0)
{
	// Nothing here
}

template <class AType>
RefCount<AType>::~RefCount()
{
	// Nothing here
}

template <class AType>
const AType *RefCount<AType>::getPointer() const
{
	return reinterpret_cast<const AType*>(this);
}

template <class AType>
AType *RefCount<AType>::getPointer()
{
	return reinterpret_cast<AType*>(this);
}

template <class AType>
uint32_t RefCount<AType>::addReference()
{
	// Increment reference count
	++mRefCount;

	// Return current reference count
	return mRefCount;
}

template <class AType>
uint32_t RefCount<AType>::releaseReference()
{
	// Decrement reference count
	if (mRefCount > 1)
	{
		--mRefCount;

		// Return current reference count
		return mRefCount;

	// Destroy object when no references are left
	}
	else
	{
		delete this;

		// This object is no longer
		return 0;
	}
}

template <class AType>
uint32_t RefCount<AType>::getRefCount() const
{
	// Return current reference count
	return mRefCount;
}
