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
#include "RendererRuntime/Core/File/IFile.h"

#include <vector>


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
	*    Memory mapped file
	*
	*  @note
	*    - Supports LZ4 compression ( http://lz4.github.io/lz4/ )
	*    - Designed for instance reusage
	*/
	class MemoryFile : public IFile
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::vector<uint8_t> ByteVector;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline MemoryFile();
		inline virtual ~MemoryFile();
		inline const ByteVector& getByteVector() const;
		RENDERERRUNTIME_API_EXPORT void setLz4CompressedDataByFile(IFile& file, uint32_t numberOfCompressedBytes, uint32_t numberOfDecompressedBytes);
		RENDERERRUNTIME_API_EXPORT void decompress();


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IFile methods         ]
	//[-------------------------------------------------------]
	public:
		inline virtual size_t getNumberOfBytes() override;
		inline virtual void read(void* destinationBuffer, size_t numberOfBytes) override;
		inline virtual void skip(size_t numberOfBytes) override;
		inline virtual void write(const void* sourceBuffer, size_t numberOfBytes) override;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		MemoryFile(const MemoryFile&) = delete;
		MemoryFile& operator=(const MemoryFile&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		ByteVector mCompressedData;		///< Owns the data
		ByteVector mDecompressedData;	///< Owns the data
		uint32_t   mNumberOfDecompressedBytes;
		uint8_t*   mCurrentDataPointer;	///< Pointer to the current uncompressed data position, doesn't own the data


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/File/MemoryFile.inl"
