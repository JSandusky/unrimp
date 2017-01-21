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
#include "PrecompiledHeader.h"
#include "Framework/StdFileManager.h"

#include <RendererRuntime/Core/Platform/PlatformTypes.h>
#include <RendererRuntime/Core/File/IFile.h>

#include <fstream>
#include <cassert>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Classes                                               ]
		//[-------------------------------------------------------]
		class StdFile : public RendererRuntime::IFile
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			explicit StdFile(const char* filename) :
				mFileStream(filename, std::ios::binary)
			{
				// Nothing here
			}

			virtual ~StdFile()
			{
				// Nothing here
			}

			bool isInvalid() const
			{
				return !mFileStream;
			}


		//[-------------------------------------------------------]
		//[ Public virtual RendererRuntime::IFile methods         ]
		//[-------------------------------------------------------]
		public:
			virtual size_t getNumberOfBytes() override
			{
				size_t numberOfBytes = 0;
				mFileStream.seekg(0, std::istream::end);
				numberOfBytes = static_cast<size_t>(mFileStream.tellg());
				mFileStream.seekg(0, std::istream::beg);
				return numberOfBytes;
			}

			virtual void read(void* destinationBuffer, size_t numberOfBytes) override
			{
				mFileStream.read(reinterpret_cast<char*>(destinationBuffer), static_cast<std::streamsize>(numberOfBytes));
			}

			virtual void skip(size_t numberOfBytes) override
			{
				mFileStream.ignore(static_cast<std::streamsize>(numberOfBytes));
			}


		//[-------------------------------------------------------]
		//[ Protected methods                                     ]
		//[-------------------------------------------------------]
		protected:
			StdFile(const StdFile&) = delete;
			StdFile& operator=(const StdFile&) = delete;


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			std::ifstream mFileStream;


		};


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Public virtual RendererRuntime::IFileManager methods  ]
//[-------------------------------------------------------]
RendererRuntime::IFile* StdFileManager::openFile(const char* filename)
{
	assert(nullptr != filename);
	::detail::StdFile* file = new ::detail::StdFile(filename);
	if (file->isInvalid())
	{
		RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Failed to open file %s", filename);
		delete file;
		file = nullptr;
	}
	return file;
}

void StdFileManager::closeFile(RendererRuntime::IFile& file)
{
	delete static_cast< ::detail::StdFile*>(&file);
}
