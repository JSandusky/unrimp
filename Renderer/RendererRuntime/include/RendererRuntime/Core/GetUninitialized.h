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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <limits>	// For "std::numeric_limits()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	/**
	*  @brief
	*    Check whether or not the given variable has not the uninitialized default value
	*/
	template <typename TYPE>
	bool isInitialized(const TYPE& value)
	{
		return (getUninitialized<TYPE>() != value);
	}

	/**
	*  @brief
	*    Check whether or not the given variable has the uninitialized default value
	*/
	template <typename TYPE>
	bool isUninitialized(const TYPE& value)
	{
		return (getUninitialized<TYPE>() == value);
	}

	/**
	*  @brief
	*    Return uninitialized default value for a given type
	*/
	template <typename TYPE>
	TYPE getUninitialized()
	{
		return std::numeric_limits<TYPE>::max();
	}

	/**
	*  @brief
	*    Return uninitialized default value for a type provided by a variable
	*/
	template <typename TYPE>
	TYPE getUninitialized(const TYPE&)
	{
		return std::numeric_limits<TYPE>::max();
	}

	/**
	*  @brief
	*    Set the given variable to uninitialized default value
	*/
	template <typename TYPE>
	void setUninitialized(TYPE& value)
	{
		value = getUninitialized<TYPE>();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
