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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Core/Math/Math.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const glm::vec3 Math::ZERO_VECTOR	(0.0f, 0.0f, 0.0f);
	const glm::vec3 Math::ONE_VECTOR	(1.0f, 1.0f, 1.0f);
	const glm::vec3 Math::RIGHT_VECTOR	(1.0f, 0.0f, 0.0f);
	const glm::vec3 Math::UP_VECTOR		(0.0f, 1.0f, 0.0f);
	const glm::vec3 Math::FORWARD_VECTOR(0.0f, 0.0f, 1.0f);
	const glm::mat4 Math::IDENTITY_MATRIX;


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	glm::quat Math::calculateTangentFrameQuaternion(glm::mat3& tangentFrameMatrix)
	{
		// Flip y axis in case the tangent frame encodes a reflection
		const float scale = (glm::determinant(tangentFrameMatrix) > 0) ? 1.0f : -1.0f;
		tangentFrameMatrix[2][0] *= scale;
		tangentFrameMatrix[2][1] *= scale;
		tangentFrameMatrix[2][2] *= scale;

		glm::quat tangentFrameQuaternion(tangentFrameMatrix);

		{ // Make sure we don't end up with 0 as w component
			const float threshold = 1.0f / SHRT_MAX; // 16 bit quantization QTangent
			const float renomalization = sqrt(1.0f - threshold * threshold);

			if (std::abs(tangentFrameQuaternion.w) < threshold)
			{
				tangentFrameQuaternion.x *= renomalization;
				tangentFrameQuaternion.y *= renomalization;
				tangentFrameQuaternion.z *= renomalization;
				tangentFrameQuaternion.w =  (tangentFrameQuaternion.w > 0) ? threshold : -threshold;
			}
		}

		{ // Encode reflection into quaternion's w element by making sign of w negative if y axis needs to be flipped, positive otherwise
			const float qs = (scale < 0.0f && tangentFrameQuaternion.w > 0.0f) || (scale > 0.0f && tangentFrameQuaternion.w < 0.0f) ? -1.0f : 1.0f;
			tangentFrameQuaternion.x *= qs;
			tangentFrameQuaternion.y *= qs;
			tangentFrameQuaternion.z *= qs;
			tangentFrameQuaternion.w *= qs;
		}

		// Done
		return tangentFrameQuaternion;
	}

	float Math::wrapToInterval(float value, float minimum, float maximum)
	{
		// Wrap as described at http://en.wikipedia.org/wiki/Wrapping_%28graphics%29
		//   value' = value - rounddown((value-min)/(max-min))*(max-min)
		// -> In here, there's no need to check for swapped minimum/maximum, it's handled correctly
		// -> Check interval in order to avoid an evil division through zero
		const float interval = (maximum - minimum);
		return interval ? (value - floor((value - minimum) / interval) * (maximum - minimum)) : minimum;
	}

	uint32_t Math::calculateFNV1a(const uint8_t* content, uint32_t numberOfBytes, uint32_t hash)
	{
		// 32-bit FNV-1a implementation basing on http://de.wikipedia.org/wiki/FNV_%28Informatik%29
		static const uint32_t MAGIC_PRIME = 2166136261u;
		const uint8_t* end = content + numberOfBytes;
		for ( ; content < end; ++content)
		{
			hash = (hash ^ *content) * MAGIC_PRIME;
		}
		return hash;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
