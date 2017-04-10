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
#include <cstring>		// For "memcpy()"
#include <inttypes.h>	// For uint32_t, uint64_t etc.


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Definitions                                           ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Geometry shader (GS) input primitive topology
	*
	*  @note
	*    - These constants directly map to OpenGL constants, do not change them
	*/
	enum class GsInputPrimitiveTopology
	{
		POINTS			    = 0x0000,	///< List of point primitives
		LINES			    = 0x0001,	///< List of line or line strip primitives
		LINES_ADJACENCY	    = 0x000A,	///< List of line with adjacency or line strip with adjacency primitives
		TRIANGLES		    = 0x0004,	///< List of triangle or triangle strip primitives
		TRIANGLES_ADJACENCY = 0x000C	///< List of triangle with adjacency or triangle strip with adjacency primitives
	};

	/**
	*  @brief
	*    Geometry shader (GS) primitive topology
	*
	*  @note
	*    - These constants directly map to OpenGL constants, do not change them
	*/
	enum class GsOutputPrimitiveTopology
	{
		POINTS			=  0x0000,	///< A list of point primitives
		LINES			=  0x0001,	///< A list of line primitives
		TRIANGLES_STRIP	=  0x0005	///< A triangle strip primitives
	};

	/**
	*  @brief
	*    Shader bytecode (aka shader microcode, binary large object (BLOB))
	*/
	class ShaderBytecode
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline ShaderBytecode() :
			mNumberOfBytes(0),
			mBytecode(nullptr)
		{
			// Nothing here
		}

		inline ~ShaderBytecode()
		{
			if (nullptr != mBytecode)
			{
				delete [] mBytecode;
			}
		}

		inline uint32_t getNumberOfBytes() const
		{
			return mNumberOfBytes;
		}

		inline const uint8_t* getBytecode() const
		{
			return mBytecode;
		}

		inline void setBytecodeCopy(uint32_t numberOfBytes, uint8_t* bytecode)
		{
			if (nullptr != mBytecode)
			{
				delete [] mBytecode;
			}
			mNumberOfBytes = numberOfBytes;
			mBytecode = new uint8_t[mNumberOfBytes];
			memcpy(mBytecode, bytecode, mNumberOfBytes);
		}


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		uint32_t mNumberOfBytes;	///< Number of bytes in the bytecode
		uint8_t* mBytecode;			///< Shader bytecode, can be a null pointer


	};

	/**
	*  @brief
	*    Shader source code
	*/
	struct ShaderSourceCode
	{
		const char* sourceCode	= nullptr;	///< Shader ASCII source code, if null pointer or empty string a null pointer will be returned
		const char* profile		= nullptr;	///< Shader ASCII profile to use, if null pointer or empty string, a default profile will be used which usually tries to use the best available profile that runs on most hardware (Examples: "glslf", "arbfp1", "ps_3_0")
		const char *arguments	= nullptr;	///< Optional shader compiler ASCII arguments, can be a null pointer or empty string
		const char *entry		= nullptr;	///< ASCII entry point, if null pointer or empty string, "main" is used as default
		inline ShaderSourceCode(const char* _sourceCode) :
			sourceCode(_sourceCode)
		{};
	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
