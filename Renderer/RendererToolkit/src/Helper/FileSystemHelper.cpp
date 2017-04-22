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
#include "RendererToolkit/Helper/FileSystemHelper.h"

#include <RendererRuntime/Resource/Mesh/Loader/MeshFileFormat.h>	// Any header which defines "RendererRuntime::FileFormatHeader" will do here
#include <RendererRuntime/Core/File/MemoryFile.h>

#include <fstream>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <lz4/lz4hc.h>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	void FileSystemHelper::writeCompressedFile(const RendererRuntime::MemoryFile& memoryFile, uint32_t formatType, uint32_t formatVersion, const std::string& outputAssetFilename)
	{
		const RendererRuntime::MemoryFile::ByteVector& byteVector = memoryFile.getByteVector();
		const int destinationCapacity = LZ4_compressBound(static_cast<int>(byteVector.size()));
		char* destination = new char[static_cast<unsigned int>(destinationCapacity)];
		{
			const int numberOfWrittenBytes = LZ4_compress_HC(reinterpret_cast<const char*>(byteVector.data()), destination, static_cast<int>(byteVector.size()), destinationCapacity, LZ4HC_CLEVEL_MAX);
			std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

			{ // Write down the file format header
				RendererRuntime::FileFormatHeader fileFormatHeader;
				fileFormatHeader.formatType				   = formatType;
				fileFormatHeader.formatVersion			   = formatVersion;
				fileFormatHeader.numberOfCompressedBytes   = static_cast<uint32_t>(numberOfWrittenBytes);
				fileFormatHeader.numberOfDecompressedBytes = static_cast<uint32_t>(byteVector.size());
				outputFileStream.write(reinterpret_cast<const char*>(&fileFormatHeader), sizeof(RendererRuntime::FileFormatHeader));
			}

			// Write down the compressed mesh data
			outputFileStream.write(destination, numberOfWrittenBytes);
		}
		delete [] destination;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
