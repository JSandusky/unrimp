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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Minimalistic quaternion class
*
*  @note
*    - Orientation quaternions are unit quaternions
*/
class Quaternion
{


//[-------------------------------------------------------]
//[ Public static data                                    ]
//[-------------------------------------------------------]
public:
	static const Quaternion ZERO;		///< 0.0, 0.0, 0.0, 0.0
	static const Quaternion IDENTITY;	///< 1.0, 0.0, 0.0, 0.0


//[-------------------------------------------------------]
//[ Public data                                           ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Some direct quaternion element accesses
	*/
	union
	{
		/*
		*  @brief
		*    0 = scalar (w), >0 = vector
		*/
		float values[4];

		/*
		*  @brief
		*    w = scalar, x&y&z = vector
		*/
		struct
		{
			float w, x, y, z;
		};
	};


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Default constructor setting an identity quaternion
	*/
	inline Quaternion();
	inline Quaternion(float w, float x, float y, float z);
	inline explicit Quaternion(const float q[]);
	inline explicit Quaternion(const Quaternion &q);
	inline ~Quaternion();
	inline Quaternion &operator *=(const Quaternion &q);


};


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Quaternion.inl"
