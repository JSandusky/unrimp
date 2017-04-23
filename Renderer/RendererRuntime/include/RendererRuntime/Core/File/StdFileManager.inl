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
#include "RendererRuntime/Core/File/IFile.h"
#include "RendererRuntime/Core/Platform/PlatformTypes.h"

#include <fstream>
#include <cassert>
#ifdef WIN32
	// Disable warnings in external headers, we can't fix them
	__pragma(warning(push))
		__pragma(warning(disable: 4548))	// warning C4548: expression before comma has no effect; expected expression with side-effect
		#include <filesystem>
	__pragma(warning(pop))
#else
	#include <experimental/filesystem>
#endif


//[-------------------------------------------------------]
//[ Global definitions                                    ]
//[-------------------------------------------------------]
#ifdef WIN32
	namespace std_filesystem = std::tr2::sys;
#else
	namespace std_filesystem = std::experimental::filesystem;
#endif


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
			StdFile()
			{
				// Nothing here
			}

			virtual ~StdFile()
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Public virtual StdFile methods                        ]
		//[-------------------------------------------------------]
		public:
			virtual bool isInvalid() const = 0;


		//[-------------------------------------------------------]
		//[ Protected methods                                     ]
		//[-------------------------------------------------------]
		protected:
			StdFile(const StdFile&) = delete;
			StdFile& operator=(const StdFile&) = delete;


		};

		class StdReadFile : public StdFile
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			explicit StdReadFile(const char* filename) :
				mFileStream(filename, std::ios::binary)
			{
				// Nothing here
			}

			virtual ~StdReadFile()
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Public virtual StdFile methods                        ]
		//[-------------------------------------------------------]
		public:
			virtual bool isInvalid() const override
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

			virtual void write(const void*, size_t) override
			{
				assert(false && "File write method not supported by the implementation");
			}


		//[-------------------------------------------------------]
		//[ Protected methods                                     ]
		//[-------------------------------------------------------]
		protected:
			StdReadFile(const StdReadFile&) = delete;
			StdReadFile& operator=(const StdReadFile&) = delete;


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			std::ifstream mFileStream;


		};

		class StdWriteFile : public StdFile
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			explicit StdWriteFile(const char* filename) :
				mFileStream(filename, std::ios::binary)
			{
				// Nothing here
			}

			virtual ~StdWriteFile()
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Public virtual StdFile methods                        ]
		//[-------------------------------------------------------]
		public:
			virtual bool isInvalid() const override
			{
				return !mFileStream;
			}


		//[-------------------------------------------------------]
		//[ Public virtual RendererRuntime::IFile methods         ]
		//[-------------------------------------------------------]
		public:
			virtual size_t getNumberOfBytes() override
			{
				assert(false && "File get number of bytes method not supported by the implementation");
				return 0;
			}

			virtual void read(void*, size_t) override
			{
				assert(false && "File read method not supported by the implementation");
			}

			virtual void skip(size_t) override
			{
				assert(false && "File skip method not supported by the implementation");
			}

			virtual void write(const void* sourceBuffer, size_t numberOfBytes) override
			{
				mFileStream.write(reinterpret_cast<const char*>(sourceBuffer), static_cast<std::streamsize>(numberOfBytes));
			}


		//[-------------------------------------------------------]
		//[ Protected methods                                     ]
		//[-------------------------------------------------------]
		protected:
			StdWriteFile(const StdWriteFile&) = delete;
			StdWriteFile& operator=(const StdWriteFile&) = delete;


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			std::ofstream mFileStream;


		};


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline StdFileManager::StdFileManager() :
		mAbsoluteLocalDataDirectoryName(std_filesystem::canonical(std_filesystem::current_path() / "/../LocalData").generic_string())
	{
		// Nothing here
	}

	inline StdFileManager::~StdFileManager()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IFileManager methods  ]
	//[-------------------------------------------------------]
	inline const char* StdFileManager::getAbsoluteLocalDataDirectoryName() const
	{
		return mAbsoluteLocalDataDirectoryName.c_str();
	}

	inline void StdFileManager::createDirectories(const char* directoryName) const
	{
		std_filesystem::create_directories(directoryName);
	}

	inline IFile* StdFileManager::openFile(FileMode fileMode, const char* filename)
	{
		assert(nullptr != filename);
		::detail::StdFile* file = nullptr;
		if (FileMode::READ == fileMode)
		{
			file = new ::detail::StdReadFile(filename);
		}
		else
		{
			file = new ::detail::StdWriteFile(filename);
		}
		if (file->isInvalid())
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Failed to open file %s", filename);
			delete file;
			file = nullptr;
		}
		return file;
	}

	inline void StdFileManager::closeFile(IFile& file)
	{
		delete static_cast< ::detail::StdFile*>(&file);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
