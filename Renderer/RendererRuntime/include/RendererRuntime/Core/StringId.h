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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/Platform/PlatformTypes.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <inttypes.h>	// For uint32_t, uint64_t etc.
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Compile time string ID class generating an identifier by hashing a given string
	*
	*  @remarks
	*    The following example shows how to use the string ID class:
	*    @code
	*    uint32_t id = RendererRuntime::StringId("Example/Mesh/Default/Squirrel");	// Result will be 2906231359
	*    @endcode
	*
	*  @note
	*    - Is using compile-time string hashing as described within the Gamasutra article "In-Depth: Quasi Compile-Time String Hashing"
	*      by Stefan Reinalter ( http://www.gamasutra.com/view/news/38198/InDepth_Quasi_CompileTime_String_Hashing.php#.UG1MpVFQbq4 )
	*    - As hash function FNV-1a is used ( http://isthe.com/chongo/tech/comp/fnv/ )
	*/
	class StringId
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const uint32_t MAXIMUM_UINT32_T_VALUE = 4294967295;	///< We don't want to include "<limits>" in this lightweight core header just to be able to use "std::numeric_limits<uint32_t>::max()"

		/**
		*  @brief
		*    "const char*"-wrapper enabling the compiler to distinguish between static and dynamic strings
		*/
		struct ConstCharWrapper
		{
			/**
			*  @brief
			*    "const char*"-wrapper constructor
			*
			*  @param[in] string
			*    Null terminated ASCII string to calculate the hash value for, must be valid
			*
			*  @note
			*    - Not explicit by intent
			*/
			inline ConstCharWrapper(const char* string) :
				mString(string)
			{
				// Nothing here
			}

			const char* mString;	///< Null terminated ASCII string to calculate the hash value for, must be valid
		};


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Calculate the hash value of the given string
		*
		*  @param[in] string
		*    Null terminated ASCII string to calculate the hash value for, must be valid
		*
		*  @return
		*    The hash value of the given string
		*/
		static inline uint32_t calculateFNV(const char* string);


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		inline FORCEINLINE StringId();

		/**
		*  @brief
		*    Constructor for calculating the hash value of a given static string
		*
		*  @param[in] string
		*    Static string to calculate the hash value for, must be valid
		*/
		template <uint32_t N>
		inline FORCEINLINE StringId(const char (&string)[N]);

		/**
		*  @brief
		*    Constructor for calculating the hash value of a given dynamic string
		*
		*  @param[in] string
		*    Dynamic string to calculate the hash value for, must be valid
		*/
		inline FORCEINLINE explicit StringId(const ConstCharWrapper& string);

		/**
		*  @brief
		*    Constructor for directly setting an uint32_t value as string ID
		*
		*  @param[in] id
		*    ID value to set
		*
		*  @note
		*    - By intent not explicit for best possible usability
		*/
		inline FORCEINLINE StringId(uint32_t id);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] stringId
		*    String ID instance to copy from
		*
		*  @remarks
		*    This constructor is by intent not explicit because otherwise GCC 4.9 throws an error when "RendererRuntime::StringId" is used
		*    e.g. as a function parameter and the function is called with a string literal. Example:
		*    "
		*      typedef StringId AssetId;
		*      void function(StringId assetId) {}
		*      void functionAssetId(AssetId assetId) {}
		*    "
		*    Function call results:
		*    - function("BlaBlub"); <-- GCC 4.9 error: no matching function for call to "RendererRuntime::StringId::StringId(RendererRuntime::StringId)"
		*    - functionAssetId("BlaBlub"); <-- GCC 4.9 error: no matching function for call to "RendererRuntime::StringId::StringId(AssetId)"
		*/
		inline FORCEINLINE StringId(const StringId& stringId);

		/**
		*  @brief
		*    Return the generated ID
		*
		*  @return
		*    The generated FNV-1a hash value used as identifier
		*/
		inline FORCEINLINE uint32_t getId() const;

		/**
		*  @brief
		*    Return the generated ID
		*
		*  @return
		*    The generated FNV-1a hash value used as identifier
		*/
		inline FORCEINLINE operator uint32_t() const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		uint32_t mId;	///< The generated FNV-1a hash value which is used as identifier


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/StringId.inl"
