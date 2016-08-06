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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/GetUninitialized.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	template <class ELEMENT_TYPE, typename ID_TYPE>
	inline PackedElementManager<ELEMENT_TYPE, ID_TYPE>::PackedElementManager() :
		mNumberOfElements(0),
		mFreeListDequeue(0),
		mFreeListEnqueue(MAXIMUM_NUMBER_OF_OBJECTS - 1)
	{
		for (uint32_t i = 0; i < MAXIMUM_NUMBER_OF_OBJECTS; ++i)
		{
			mIndices[i].id = i;
			mIndices[i].next = static_cast<uint16_t>(i + 1);
		}
	}

	template <class ELEMENT_TYPE, typename ID_TYPE>
	inline PackedElementManager<ELEMENT_TYPE, ID_TYPE>::~PackedElementManager()
	{
		// Nothing here
	}

	template <class ELEMENT_TYPE, typename ID_TYPE>
	inline uint32_t PackedElementManager<ELEMENT_TYPE, ID_TYPE>::getNumberOfElements() const
	{
		return mNumberOfElements;
	}

	template <class ELEMENT_TYPE, typename ID_TYPE>
	inline ELEMENT_TYPE& PackedElementManager<ELEMENT_TYPE, ID_TYPE>::getElementByIndex(uint32_t index) const
	{
		return mElements[index];
	}

	template <class ELEMENT_TYPE, typename ID_TYPE>
	inline bool PackedElementManager<ELEMENT_TYPE, ID_TYPE>::isElementIdValid(ID_TYPE id) const
	{
		if (isInitialized(id))
		{
			const Index& index = mIndices[id & INDEX_MASK];
			return (index.id == id && index.index != USHRT_MAX);
		}
		return false;
	}

	template <class ELEMENT_TYPE, typename ID_TYPE>
	inline ELEMENT_TYPE& PackedElementManager<ELEMENT_TYPE, ID_TYPE>::getElementById(ID_TYPE id) const
	{
		return mElements[mIndices[id & INDEX_MASK].index];
	}

	template <class ELEMENT_TYPE, typename ID_TYPE>
	inline ELEMENT_TYPE* PackedElementManager<ELEMENT_TYPE, ID_TYPE>::tryGetElementById(ID_TYPE id) const
	{
		if (isInitialized(id))
		{
			const Index& index = mIndices[id & INDEX_MASK];
			return (index.id == id && index.index != USHRT_MAX) ? &mElements[index.index] : nullptr;
		}
		return nullptr;
	}

	template <class ELEMENT_TYPE, typename ID_TYPE>
	inline ELEMENT_TYPE& PackedElementManager<ELEMENT_TYPE, ID_TYPE>::addElement()
	{
		Index& index = mIndices[mFreeListDequeue];
		mFreeListDequeue = index.next;
		index.id += NEW_OBJECT_ID_ADD;
		index.index = static_cast<uint16_t>(mNumberOfElements++);

		// Call the constructor of the added element
		ELEMENT_TYPE& element = mElements[index.index];
		new (static_cast<void*>(&element)) ELEMENT_TYPE(index.id);

		// Return the added element
		return element;
	}

	template <class ELEMENT_TYPE, typename ID_TYPE>
	inline void PackedElementManager<ELEMENT_TYPE, ID_TYPE>::removeElement(ID_TYPE id)
	{
		Index& index = mIndices[id & INDEX_MASK];
		ELEMENT_TYPE& element = mElements[index.index];

		// Call the destructor of the removed element
		element.~ELEMENT_TYPE();

		element = mElements[--mNumberOfElements];
		mIndices[element.id & INDEX_MASK].index = index.index;
		index.index = USHRT_MAX;
		mIndices[mFreeListEnqueue].next = id & INDEX_MASK;
		mFreeListEnqueue = id & INDEX_MASK;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
