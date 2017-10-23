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
//[ Includes                                              ]
//[-------------------------------------------------------]
// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'return': conversion from 'int' to 'std::_Rand_urng_from_func::result_type', signed/unsigned mismatch
	#include <cassert>
	#include <cstring>		// For "memcpy()"
	#include <iterator>		// For "std::back_inserter()"
	#include <algorithm>	// For "std::copy()"
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline MemoryFile::MemoryFile() :
		mNumberOfDecompressedBytes(0),
		mCurrentDataPointer(nullptr)
	{
		// Nothing here
	}

	inline MemoryFile::MemoryFile(size_t reserveNumberOfCompressedBytes, size_t reserveNumberOfDecompressedBytes) :
		mNumberOfDecompressedBytes(0),
		mCurrentDataPointer(nullptr)
	{
		mCompressedData.reserve(reserveNumberOfCompressedBytes);
		mDecompressedData.reserve(reserveNumberOfDecompressedBytes);
	}

	inline MemoryFile::~MemoryFile()
	{
		// Nothing here
	}

	inline MemoryFile::ByteVector& MemoryFile::getByteVector()
	{
		return mDecompressedData;
	}

	inline const MemoryFile::ByteVector& MemoryFile::getByteVector() const
	{
		return mDecompressedData;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IFile methods         ]
	//[-------------------------------------------------------]
	inline size_t MemoryFile::getNumberOfBytes()
	{
		return mDecompressedData.size();
	}

	inline void MemoryFile::read(void* destinationBuffer, size_t numberOfBytes)
	{
		assert((mCurrentDataPointer - mDecompressedData.data()) + numberOfBytes <= mDecompressedData.size());
		memcpy(destinationBuffer, mCurrentDataPointer, numberOfBytes);
		mCurrentDataPointer += numberOfBytes;
	}

	inline void MemoryFile::skip(size_t numberOfBytes)
	{
		assert((mCurrentDataPointer - mDecompressedData.data()) + numberOfBytes <= mDecompressedData.size());
		mCurrentDataPointer += numberOfBytes;
	}

	inline void MemoryFile::write(const void* sourceBuffer, size_t numberOfBytes)
	{
		std::copy(static_cast<const uint8_t*>(sourceBuffer), static_cast<const uint8_t*>(sourceBuffer) + numberOfBytes, std::back_inserter(mDecompressedData));
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
