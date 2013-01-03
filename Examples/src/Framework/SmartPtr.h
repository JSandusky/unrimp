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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __SMARTPTR_H__
#define __SMARTPTR_H__


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
template <class AType> class RefCount;


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Smart pointer template
*/
template <class AType>
class SmartPtr
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Default constructor
	*/
	SmartPtr();

	/**
	*  @brief
	*    Constructor
	*
	*  @param[in] ptr
	*    Direct pointer to initialize with, can be a null pointer
	*/
	explicit SmartPtr(AType *ptr);

	/**
	*  @brief
	*    Constructor
	*
	*  @param[in] ptr
	*    Smart pointer to initialize with
	*/
	explicit SmartPtr(const SmartPtr<AType> &ptr);

	/**
	*  @brief
	*    Destructor
	*/
	~SmartPtr();

	/**
	*  @brief
	*    Assign a pointer
	*
	*  @param[in] ptr
	*    Direct pointer to assign, can be a null pointer
	*
	*  @return
	*    Reference to the smart pointer
	*/
	SmartPtr<AType> &operator =(AType *ptr);

	/**
	*  @brief
	*    Assign a smart pointer
	*
	*  @param[in] ptr
	*    Smart pointer to assign
	*
	*  @return
	*    Reference to the smart pointer
	*/
	SmartPtr<AType> &operator =(const SmartPtr<AType> &ptr);

	/**
	*  @brief
	*    Get a direct pointer to the object
	*
	*  @return
	*    Pointer to the object, can be a null pointer
	*/
	AType *getPointer() const;

	/**
	*  @brief
	*    Get a pointer to access the object
	*
	*  @return
	*    Pointer to the object, can be a null pointer
	*/
	AType *operator ->() const;

	/**
	*  @brief
	*    Cast to a pointer to the object
	*
	*  @return
	*    Pointer to the object, can be a null pointer
	*/
	operator AType*() const;

	/**
	*  @brief
	*    Check if the pointer is not a null pointer
	*
	*  @return
	*    "true" if the pointer is not a null pointer
	*/
	bool operator !() const;

	/**
	*  @brief
	*    Check for equality
	*
	*  @param[in] ptr
	*    Direct pointer to compare with, can be a null pointer
	*
	*  @return
	*    "true" if the two pointers are equal
	*/
	bool operator ==(AType *ptr) const;

	/**
	*  @brief
	*    Check for equality
	*
	*  @param[in] ptr
	*    Smart pointer to compare with
	*
	*  @return
	*    "true" if the two pointers are equal
	*/
	bool operator ==(const SmartPtr<AType> &ptr) const;

	/**
	*  @brief
	*    Check for equality
	*
	*  @param[in] ptr
	*    Direct pointer to compare with, can be a null pointer
	*
	*  @return
	*    "true" if the two pointers are not equal
	*/
	bool operator !=(AType *ptr) const;

	/**
	*  @brief
	*    Check for equality
	*
	*  @param[in] ptr
	*    Smart pointer to compare with
	*
	*  @return
	*    "true" if the two pointers are not equal
	*/
	bool operator !=(const SmartPtr<AType> &ptr) const;


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
private:
	/**
	*  @brief
	*    Assign a pointer to an object that does not implement RefCount
	*
	*  @param[in] ptr
	*    Pointer to assign, can be a null pointer
	*/
	void setPtr(void *ptr);

	/**
	*  @brief
	*    Assign a pointer to an object that implements RefCount
	*
	*  @param[in] ptr
	*    Pointer to assign, can be a null pointer
	*/
	void setPtr(RefCount<AType> *ptr);

	/**
	*  @brief
	*    Get pointer to the reference counted object
	*
	*  @return
	*    Pointer to the RefCount object, can be a null pointer
	*/
	RefCount<AType> *getPtr() const;


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	RefCount<AType> *mPtr; /**< Pointer to reference counter, can be a null pointer */


};


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "SmartPtr.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __SMARTPTR_H__
