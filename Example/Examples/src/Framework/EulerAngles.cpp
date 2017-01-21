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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "PrecompiledHeader.h"
#include "Framework/EulerAngles.h"
#include "Framework/Quaternion.h"

#include <math.h>


//[-------------------------------------------------------]
//[ Definitions                                           ]
//[-------------------------------------------------------]
static const float FLOAT_EPSILON = 1.192092896e-07f; // Smallest such that 1.0f + FLOAT_EPSILON != 1.0
 // EulGetOrd unpacks all useful information about order simultaneously
#define EulSafe "\000\001\002\000"
#define EulNext "\001\002\000\001"
#define EulGetOrd(ord,i,j,k,n,s,f) {int o=ord;f=o&1;o>>=1;s=o&1;o>>=1;\
    n=o&1;o>>=1;i=EulSafe[o&3];j=EulNext[i+n];k=EulNext[i+1-n];}
// Original code was
/* #define EulGetOrd(ord,i,j,k,h,n,s,f) {unsigned o=ord;f=o&1;o>>=1;s=o&1;o>>=1;\
    n=o&1;o>>=1;i=EulSafe[o&3];j=EulNext[i+n];k=EulNext[i+1-n];h=s?k:i;} */
// but we don't need "h", so it was removed to spare the compiler the work *g*


//[-------------------------------------------------------]
//[ Public static functions                               ]
//[-------------------------------------------------------]
void EulerAngles::toQuaternion(float angleX, float angleY, float angleZ, Quaternion &rotation, EOrder order)
{
	int i, j, k, n, s, f;
	EulGetOrd(order, i, j, k, n, s, f);
	if (1 == f)
	{
		const float t = angleX;
		angleX = angleZ;
		angleZ = t;
	}
	if (1 == n)
	{
		angleY = -angleY;
	}

	const double ti = angleX*0.5;
	const double tj = angleY*0.5;
	const double th = angleZ*0.5;
	const double ci = cos(ti);
	const double cj = cos(tj);
	const double ch = cos(th);
	const double si = sin(ti);
	const double sj = sin(tj);
	const double sh = sin(th);
	const double cc = ci*ch;
	const double cs = ci*sh;
	const double sc = si*ch;
	const double ss = si*sh;

	double a[3] = { 0.0, 0.0, 0.0 };
	if (1 == s)
	{
		a[i] = cj*(cs + sc);	// Could speed up with trig identities
		a[j] = sj*(cc + ss);
		a[k] = sj*(cs - sc);
		rotation.w = static_cast<float>(cj*(cc - ss));
	}
	else
	{
		a[i] = cj*sc - sj*cs;
		a[j] = cj*ss + sj*cc;
		a[k] = cj*cs - sj*sc;
		rotation.w = static_cast<float>(cj*cc + sj*ss);
	}
	if (1 == n)
	{
		a[j] = -a[j];
	}

	rotation.x = static_cast<float>(a[0]);
	rotation.y = static_cast<float>(a[1]);
	rotation.z = static_cast<float>(a[2]);
}

void EulerAngles::fromQuaternion(const Quaternion &q, float &angleX, float &angleY, float &angleZ, EOrder order)
{
	// Construct 3x3 matrix
	enum EComponent {X, Y, Z};
	float M[3][3]; // Right-handed, for column vectors
	{
		const double Nq = q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;
		const double s  = (Nq > 0.0) ? (2.0 / Nq) : 0.0;
		const double xs = q.x*s,  ys = q.y*s,  zs = q.z*s;
		const double wx = q.w*xs, wy = q.w*ys, wz = q.w*zs;
		const double xx = q.x*xs, xy = q.x*ys, xz = q.x*zs;
		const double yy = q.y*ys, yz = q.y*zs, zz = q.z*zs;
		M[X][X] = static_cast<float>(1.0-(yy+zz)); M[X][Y] = static_cast<float>(xy-wz);       M[X][Z] = static_cast<float>(xz+wy);
		M[Y][X] = static_cast<float>(xy+wz);       M[Y][Y] = static_cast<float>(1.0-(xx+zz)); M[Y][Z] = static_cast<float>(yz-wx);
		M[Z][X] = static_cast<float>(xz-wy);       M[Z][Y] = static_cast<float>(yz+wx);       M[Z][Z] = static_cast<float>(1.0-(xx+yy));
	}

	// Convert 3x3 matrix to Euler angles (in radians)
	int i, j, k, n, s, f;
	EulGetOrd(order, i, j, k, n, s, f);
	if (1 == s)
	{
		const double sy = sqrt(M[i][j]*M[i][j] + M[i][k]*M[i][k]);
		if (sy > 16*FLOAT_EPSILON)
		{
			angleX = static_cast<float>(atan2(M[i][j], M[i][k]));
			angleY = static_cast<float>(atan2(sy, static_cast<double>(M[i][i])));
			angleZ = static_cast<float>(atan2(M[j][i], -M[k][i]));
		}
		else
		{
			angleX = static_cast<float>(atan2(-M[j][k], M[j][j]));
			angleY = static_cast<float>(atan2(sy, static_cast<double>(M[i][i])));
			angleZ = 0.0f;
		}
	}
	else
	{
		const double cy = sqrt(M[i][i]*M[i][i] + M[j][i]*M[j][i]);
		if (cy > 16*FLOAT_EPSILON)
		{
			angleX = static_cast<float>(atan2(M[k][j], M[k][k]));
			angleY = static_cast<float>(atan2(static_cast<double>(-M[k][i]), cy));
			angleZ = static_cast<float>(atan2(M[j][i], M[i][i]));
		}
		else
		{
			angleX = static_cast<float>(atan2(-M[j][k], M[j][j]));
			angleY = static_cast<float>(atan2(static_cast<double>(-M[k][i]), cy));
			angleZ = 0.0f;
		}
	}
	if (1 == n)
	{
		angleX = -angleX;
		angleY = -angleY;
		angleZ = -angleZ;
	}
	if (1 == f)
	{
		const float t = angleX;
		angleX = angleZ;
		angleZ = t;
	}
}
