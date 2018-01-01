/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline Transform::Transform() :
		position(Math::VEC3_ZERO),
		rotation(Math::QUAT_IDENTITY),
		scale(Math::VEC3_ONE)
	{
		// Nothing here
	}

	inline Transform::Transform(const glm::vec3& _position) :
		position(_position),
		rotation(Math::QUAT_IDENTITY),
		scale(Math::VEC3_ONE)
	{
		// Nothing here
	}

	inline Transform::Transform(const glm::mat4& transformMatrix)
	{
		setByMatrix(transformMatrix);
	}

	inline Transform::Transform(const glm::vec3& _position, const glm::quat& _rotation) :
		position(_position),
		rotation(_rotation),
		scale(Math::VEC3_ONE)
	{
		// Nothing here
	}

	inline Transform::Transform(const glm::vec3& _position, const glm::quat& _rotation, const glm::vec3& _scale) :
		position(_position),
		rotation(_rotation),
		scale(_scale)
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
