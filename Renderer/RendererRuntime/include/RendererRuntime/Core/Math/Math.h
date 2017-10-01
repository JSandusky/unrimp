/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
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
#include "RendererRuntime/Export.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4201)	// warning C4201: nonstandard extension used: nameless struct/union
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4324)	// warning C4324: '<x>': structure was padded due to alignment specifier
	#include <glm/glm.hpp>
	#include <glm/gtc/quaternion.hpp>
PRAGMA_WARNING_POP

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(5026)	// warning C5026: 'std::_Generic_error_category': move constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5027)	// warning C5027: 'std::_Generic_error_category': move assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'std::codecvt_base': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'std::codecvt<char16_t,char,_Mbstatet>': assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4774)	// warning C4774: 'sprintf_s' : format string expected in argument 3 is not a string literal
	#include <inttypes.h>	// For uint32_t, uint64_t etc.
	#include <string>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRenderer;
}
namespace RendererRuntime
{
	class IFileManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class Math
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const uint32_t FNV1a_INITIAL_HASH_32 = 0xcbf29ce4;
		static const uint64_t FNV1a_INITIAL_HASH_64 = 0xcbf29ce484222325;
		// "glm::vec3" constants
		RENDERERRUNTIME_API_EXPORT static const glm::vec3 VEC3_ZERO;	///< 0 0 0
		RENDERERRUNTIME_API_EXPORT static const glm::vec3 VEC3_ONE;		///< 1 1 1
		RENDERERRUNTIME_API_EXPORT static const glm::vec3 VEC3_UNIT_X;	///< 1 0 0
		RENDERERRUNTIME_API_EXPORT static const glm::vec3 VEC3_RIGHT;	///< 1 0 0; coordinate system axis terminology, don't remove
		RENDERERRUNTIME_API_EXPORT static const glm::vec3 VEC3_UNIT_Y;	///< 0 1 0
		RENDERERRUNTIME_API_EXPORT static const glm::vec3 VEC3_UP;		///< 0 1 0; coordinate system axis terminology, don't remove
		RENDERERRUNTIME_API_EXPORT static const glm::vec3 VEC3_UNIT_Z;	///< 0 0 1
		RENDERERRUNTIME_API_EXPORT static const glm::vec3 VEC3_FORWARD;	///< 0 0 1; coordinate system axis terminology, don't remove
		// "glm::vec4" constants
		RENDERERRUNTIME_API_EXPORT static const glm::vec4 VEC4_ZERO;	///< 0 0 0 0
		RENDERERRUNTIME_API_EXPORT static const glm::vec4 VEC4_ONE;		///< 1 1 1 1
		// "glm::mat4" constants
		RENDERERRUNTIME_API_EXPORT static const glm::mat4 MAT4_IDENTITY;
		// "glm::quat" constants
		RENDERERRUNTIME_API_EXPORT static const glm::quat QUAT_IDENTITY;


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Calculate tangent frame quaternion (QTangent) basing of a provided 3x3 tangent frame matrix
		*
		*  @param[in, out] tangentFrameMatrix
		*    3x3 tangent frame matrix, will be manipulated during calculation (no internal copy for performance reasons)
		*
		*  @return
		*    The calculated tangent frame quaternion (QTangent)
		*
		*  @note
		*  - QTangent basing on http://dev.theomader.com/qtangents/ "QTangents" which is basing on
		*    http://www.crytek.com/cryengine/presentations/spherical-skinning-with-dual-quaternions-and-qtangents "Spherical Skinning with Dual-Quaternions and QTangents"
		*/
		RENDERERRUNTIME_API_EXPORT static glm::quat calculateTangentFrameQuaternion(glm::mat3& tangentFrameMatrix);

		/**
		*  @brief
		*    Ensure that the given value is within the given interval [minimum, maximum] by wrapping the value
		*
		*  @param[in] value
		*    Value to check
		*  @param[in] minimum
		*    Minimum of the interval, must be < maximum
		*  @param[in] maximum
		*    Maximum of the interval, must be > minimum
		*
		*  @return
		*    The value within the interval [minimum, maximum]
		*
		*  @note
		*    - In case of violating the interface specification by swapping minimum/maximum the result will not be different to the one of the correct order
		*/
		RENDERERRUNTIME_API_EXPORT static float wrapToInterval(float value, float minimum, float maximum);

		/**
		*  @brief
		*    Get renderer dependent texture scale bias matrix
		*
		*  @param[in] renderer
		*    Used renderer
		*
		*  @return
		*    Renderer dependent texture scale bias matrix
		*/
		RENDERERRUNTIME_API_EXPORT static const glm::mat4& getTextureScaleBiasMatrix(const Renderer::IRenderer& renderer);

		//[-------------------------------------------------------]
		//[ Hash                                                  ]
		//[-------------------------------------------------------]
		RENDERERRUNTIME_API_EXPORT static uint32_t calculateFNV1a32(const uint8_t* content, uint32_t numberOfBytes, uint32_t hash = FNV1a_INITIAL_HASH_32);
		RENDERERRUNTIME_API_EXPORT static uint64_t calculateFNV1a64(const uint8_t* content, uint32_t numberOfBytes, uint64_t hash = FNV1a_INITIAL_HASH_64);
		RENDERERRUNTIME_API_EXPORT static uint64_t calculateFileFNV1a64ByFilename(const IFileManager& fileManager, const std::string& filename);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		Math() = delete;
		~Math() = delete;
		explicit Math(const Math&) = delete;
		Math& operator=(const Math&) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
