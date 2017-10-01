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
#include "RendererRuntime/Core/Manager.h"

#include <limits.h>		// For "USHRT_MAX"
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
	*    Packed element manager template
	*
	*  @note
	*    - Basing on "Managing Decoupling Part 4 -- The ID Lookup Table" https://github.com/niklasfrykholm/blog/blob/master/2011/managing-decoupling-4.md by Niklas Frykholm ( http://www.frykholm.se/ )
	*/
	template <class ELEMENT_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS>
	class PackedElementManager : private Manager
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline PackedElementManager();
		inline ~PackedElementManager();
		inline uint32_t getNumberOfElements() const;
		inline ELEMENT_TYPE& getElementByIndex(uint32_t index) const;
		inline bool isElementIdValid(ID_TYPE id) const;
		inline ELEMENT_TYPE& getElementById(ID_TYPE id) const;
		inline ELEMENT_TYPE* tryGetElementById(ID_TYPE id) const;
		inline ELEMENT_TYPE& addElement();
		inline void removeElement(ID_TYPE id);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit PackedElementManager(const PackedElementManager&) = delete;
		PackedElementManager& operator=(const PackedElementManager&) = delete;


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		static const uint32_t INDEX_MASK		= 0xffff;
		static const uint32_t NEW_OBJECT_ID_ADD	= 0x10000;

		struct Index
		{
			ID_TYPE  id;
			uint16_t index;
			uint16_t next;
		};


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		uint32_t			 mNumberOfElements;
		mutable ELEMENT_TYPE mElements[MAXIMUM_NUMBER_OF_ELEMENTS];
		Index				 mIndices[MAXIMUM_NUMBER_OF_ELEMENTS];
		uint16_t			 mFreeListEnqueue;
		uint16_t			 mFreeListDequeue;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/PackedElementManager.inl"
