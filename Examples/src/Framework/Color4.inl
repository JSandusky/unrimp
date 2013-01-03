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
//[ Public methods                                        ]
//[-------------------------------------------------------]
/**
*  @brief
*    Default constructor
*/
inline Color4::Color4() :
	r(0.0f),
	g(0.0f),
	b(0.0f),
	a(0.0f)
{
	// Nothing to do in here
}

/**
*  @brief
*    Copy constructor
*/
inline Color4::Color4(const Color4 &source) :
	r(source.r),
	g(source.g),
	b(source.b),
	a(source.a)
{
	// Nothing to do in here
}

/**
*  @brief
*    Constructor
*/
inline Color4::Color4(float _r, float _g, float _b, float _a) :
	r(_r),
	g(_g),
	b(_b),
	a(_a)
{
	// Nothing to do in here
}

/**
*  @brief
*    Destructor
*/
inline Color4::~Color4()
{
	// Nothing to do in here
}

/**
*  @brief
*    Copy operator
*/
inline Color4 &Color4::operator =(const Color4 &source)
{
	r = source.r;
	g = source.g;
	b = source.b;
	a = source.a;
	return *this;
}

inline Color4::operator float *()
{
	return &r;
}

inline Color4::operator const float *() const
{
	return &r;
}
