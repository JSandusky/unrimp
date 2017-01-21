/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
//[ Public methods                                        ]
//[-------------------------------------------------------]
inline Quaternion::Quaternion() :
	w(1.0f),
	x(0.0f),
	y(0.0f),
	z(0.0f)
{
	// Nothing here
}

inline Quaternion::Quaternion(float _w, float _x, float _y, float _z) :
	w(_w),
	x(_x),
	y(_y),
	z(_z)
{
	// Nothing here
}

inline Quaternion::Quaternion(const float q[]) :
	w(q[0]),
	x(q[1]),
	y(q[2]),
	z(q[3])
{
	// Nothing here
}

inline Quaternion::Quaternion(const Quaternion &q) :
	w(q.w),
	x(q.x),
	y(q.y),
	z(q.z)
{
	// Nothing here
}

inline Quaternion::~Quaternion()
{
	// Nothing here
}

inline Quaternion &Quaternion::operator *=(const Quaternion &q)
{
	const float qw = w, qx = x, qy = y, qz = z;
	w = qw*q.w - qx*q.x - qy*q.y - qz*q.z;
	x = qw*q.x + qx*q.w + qy*q.z - qz*q.y;
	y = qw*q.y + qy*q.w + qz*q.x - qx*q.z;
	z = qw*q.z + qz*q.w + qx*q.y - qy*q.x;
	return *this;
}
