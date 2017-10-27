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
#include "RendererRuntime/Core/File/FileSystemHelper.h"

#include <Renderer/Public/Renderer.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	#include <fstream>
	#include <cassert>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		static const char* STD_LOCAL_DATA_MOUNT_POINT = "LocalData";


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

			virtual ~StdFile() override
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
			explicit StdFile(const StdFile&) = delete;
			StdFile& operator=(const StdFile&) = delete;


		};

		class StdReadFile : public StdFile
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			explicit StdReadFile(const std::string& absoluteFilename) :
				mFileStream(std_filesystem::u8path(absoluteFilename), std::ios::binary)
			{
				assert(mFileStream && "Failed to open STD file for reading");
			}

			virtual ~StdReadFile() override
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
				assert(mFileStream && "Invalid STD file access");
				size_t numberOfBytes = 0;
				mFileStream.seekg(0, std::istream::end);
				numberOfBytes = static_cast<size_t>(mFileStream.tellg());
				mFileStream.seekg(0, std::istream::beg);
				return numberOfBytes;
			}

			virtual void read(void* destinationBuffer, size_t numberOfBytes) override
			{
				assert(mFileStream && "Invalid STD file access");
				mFileStream.read(reinterpret_cast<char*>(destinationBuffer), static_cast<std::streamsize>(numberOfBytes));
			}

			virtual void skip(size_t numberOfBytes) override
			{
				assert(mFileStream && "Invalid STD file access");
				mFileStream.ignore(static_cast<std::streamsize>(numberOfBytes));
			}

			virtual void write(const void*, size_t) override
			{
				assert(mFileStream && "Invalid STD file access");
				assert(false && "File write method not supported by the STD implementation");
			}


		//[-------------------------------------------------------]
		//[ Protected methods                                     ]
		//[-------------------------------------------------------]
		protected:
			explicit StdReadFile(const StdReadFile&) = delete;
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
			explicit StdWriteFile(const std::string& absoluteFilename) :
				mFileStream(std_filesystem::u8path(absoluteFilename), std::ios::binary)
			{
				assert(mFileStream && "Failed to open STD file for writing");
			}

			virtual ~StdWriteFile() override
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
				assert(mFileStream && "Invalid STD file access");
				assert(false && "File get number of bytes method not supported by the STD implementation");
				return 0;
			}

			virtual void read(void*, size_t) override
			{
				assert(mFileStream && "Invalid STD file access");
				assert(false && "File read method not supported by the STD implementation");
			}

			virtual void skip(size_t) override
			{
				assert(mFileStream && "Invalid STD file access");
				assert(false && "File skip method not supported by the STD implementation");
			}

			virtual void write(const void* sourceBuffer, size_t numberOfBytes) override
			{
				assert(mFileStream && "Invalid STD file access");
				mFileStream.write(reinterpret_cast<const char*>(sourceBuffer), static_cast<std::streamsize>(numberOfBytes));
			}


		//[-------------------------------------------------------]
		//[ Protected methods                                     ]
		//[-------------------------------------------------------]
		protected:
			explicit StdWriteFile(const StdWriteFile&) = delete;
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
	inline StdFileManager::StdFileManager(Renderer::ILog& log, const std::string& relativeRootDirectory) :
		IFileManager(relativeRootDirectory),
		mLog(log)
	{
		// Setup local data mount point
		std_filesystem::path absoluteDirectoryName = std_filesystem::canonical(std_filesystem::current_path());
		if (!relativeRootDirectory.empty())
		{
			absoluteDirectoryName /= relativeRootDirectory;
		}
		mAbsoluteBaseDirectory.push_back(absoluteDirectoryName.generic_string());
		absoluteDirectoryName /= ::detail::STD_LOCAL_DATA_MOUNT_POINT;
		createDirectories(::detail::STD_LOCAL_DATA_MOUNT_POINT);
		mountDirectory(absoluteDirectoryName.generic_string().c_str(), ::detail::STD_LOCAL_DATA_MOUNT_POINT);
	}

	inline StdFileManager::~StdFileManager()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IFileManager methods  ]
	//[-------------------------------------------------------]
	inline const char* StdFileManager::getLocalDataMountPoint() const
	{
		return ::detail::STD_LOCAL_DATA_MOUNT_POINT;
	}

	inline bool StdFileManager::mountDirectory(AbsoluteDirectoryName absoluteDirectoryName, const char* mountPoint, bool appendToPath)
	{
		// Sanity check
		assert(nullptr != absoluteDirectoryName);
		assert(nullptr != mountPoint);
		#ifdef _DEBUG
			// Additional sanity check: The same absolute directory name shouldn't be added to too different mount points
			for (const auto& pair : mMountedDirectories)
			{
				if (pair.first != mountPoint)
				{
					const AbsoluteDirectoryNames& absoluteDirectoryNames = pair.second;
					assert(absoluteDirectoryNames.cend() == std::find(absoluteDirectoryNames.begin(), absoluteDirectoryNames.end(), absoluteDirectoryName) && "The same absolute directory name shouldn't be added to too different STD mount points");
				}
			}
		#endif

		// Mount directory
		MountedDirectories::iterator mountedDirectoriesIterator = mMountedDirectories.find(mountPoint);
		if (mMountedDirectories.cend() == mountedDirectoriesIterator)
		{
			// The mount point is unknown so far, register it
			mMountedDirectories.emplace(mountPoint, AbsoluteDirectoryNames{absoluteDirectoryName});
		}
		else
		{
			// The mount point is already known, append or prepend?
			AbsoluteDirectoryNames& absoluteDirectoryNames = mountedDirectoriesIterator->second;
			AbsoluteDirectoryNames::const_iterator absoluteDirectoryNamesIterator = std::find(absoluteDirectoryNames.begin(), absoluteDirectoryNames.end(), absoluteDirectoryName);
			if (absoluteDirectoryNames.cend() == absoluteDirectoryNamesIterator)
			{
				if (appendToPath)
				{
					// Append
					absoluteDirectoryNames.push_back(absoluteDirectoryName);
				}
				else
				{
					// Prepend
					absoluteDirectoryNames.insert(absoluteDirectoryNames.begin(), absoluteDirectoryName);
				}
			}
			else
			{
				assert(false && "Duplicate absolute STD directory name detected, this situation should be avoided by the caller");
			}
		}

		// Done
		return true;
	}

	inline bool StdFileManager::doesFileExist(VirtualFilename virtualFilename) const
	{
		return !mapVirtualToAbsoluteFilename(FileMode::READ, virtualFilename).empty();
	}

	inline void StdFileManager::enumerateFiles(VirtualDirectoryName virtualDirectoryName, EnumerationMode enumerationMode, std::vector<std::string>& virtualFilenames) const
	{
		std::string mountPoint;
		const std::string absoluteDirectoryName = mapVirtualToAbsoluteFilenameAndMountPoint(FileMode::READ, virtualDirectoryName, mountPoint);
		if (!absoluteDirectoryName.empty())
		{
			const size_t numberOfSkippedBytes = absoluteDirectoryName.length() + 1;	// +1 for '/'-slash at the end of the absolute directory name
			switch (enumerationMode)
			{
				case EnumerationMode::ALL:
					for (const std_filesystem::directory_entry& iterator: std_filesystem::recursive_directory_iterator(absoluteDirectoryName))
					{
						virtualFilenames.push_back(mountPoint + '/' + iterator.path().generic_string().erase(0, numberOfSkippedBytes));
					}
					break;

				case EnumerationMode::FILES:
					for (const std_filesystem::directory_entry& iterator: std_filesystem::recursive_directory_iterator(absoluteDirectoryName))
					{
						if (std_filesystem::is_regular_file(iterator))
						{
							virtualFilenames.push_back(mountPoint + '/' + iterator.path().generic_string().erase(0, numberOfSkippedBytes));
						}
					}
					break;

				case EnumerationMode::DIRECTORIES:
					for (const std_filesystem::directory_entry& iterator: std_filesystem::recursive_directory_iterator(absoluteDirectoryName))
					{
						if (std_filesystem::is_directory(iterator))
						{
							virtualFilenames.push_back(mountPoint + '/' + iterator.path().generic_string().erase(0, numberOfSkippedBytes));
						}
					}
					break;
			}
		}
	}

	inline std::string StdFileManager::mapVirtualToAbsoluteFilename(FileMode fileMode, VirtualFilename virtualFilename) const
	{
		std::string mountPoint;
		return mapVirtualToAbsoluteFilenameAndMountPoint(fileMode, virtualFilename, mountPoint);
	}

	inline int64_t StdFileManager::getLastModificationTime(VirtualFilename virtualFilename) const
	{
		const std::string absoluteFilename = mapVirtualToAbsoluteFilename(FileMode::READ, virtualFilename);
		if (!absoluteFilename.empty())
		{
			const std_filesystem::file_time_type lastWriteTime = std_filesystem::last_write_time(std_filesystem::u8path(absoluteFilename));
			return static_cast<int64_t>(decltype(lastWriteTime)::clock::to_time_t(lastWriteTime));
		}

		// Error!
		return -1;
	}

	inline int64_t StdFileManager::getFileSize(VirtualFilename virtualFilename) const
	{
		const std::string absoluteFilename = mapVirtualToAbsoluteFilename(FileMode::READ, virtualFilename);
		return absoluteFilename.empty() ? -1 : static_cast<int64_t>(std_filesystem::file_size(std_filesystem::u8path(absoluteFilename)));
	}

	inline bool StdFileManager::createDirectories(VirtualDirectoryName virtualDirectoryName) const
	{
		// Sanity check
		assert(nullptr != virtualDirectoryName);

		// Create directories
		const AbsoluteDirectoryNames* absoluteDirectoryNames = nullptr;
		std::string relativeFilename;
		std::string mountPoint;
		if (getAbsoluteDirectoryNamesByMountPoint(virtualDirectoryName, &absoluteDirectoryNames, relativeFilename, mountPoint) && absoluteDirectoryNames != nullptr)
		{
			for (const std::string& absoluteDirectoryName : *absoluteDirectoryNames)
			{
				const std_filesystem::path absoluteFilename = std_filesystem::u8path(absoluteDirectoryName + '/' + relativeFilename);
				if (!std_filesystem::exists(absoluteFilename) && !std_filesystem::create_directories(absoluteFilename))
				{
					// Failed to create the directories
					return false;
				}
			}
		}

		// Directories have been created successfully
		return true;
	}

	inline IFile* StdFileManager::openFile(FileMode fileMode, VirtualFilename virtualFilename) const
	{
		::detail::StdFile* file = nullptr;
		const std::string absoluteFilename = mapVirtualToAbsoluteFilename(fileMode, virtualFilename);
		if (!absoluteFilename.empty())
		{
			if (FileMode::READ == fileMode)
			{
				file = new ::detail::StdReadFile(absoluteFilename);
			}
			else
			{
				file = new ::detail::StdWriteFile(absoluteFilename);
			}
			if (file->isInvalid())
			{
				mLog.print(Renderer::ILog::Type::CRITICAL, "Failed to open file %s", virtualFilename);
				delete file;
				file = nullptr;
			}
		}

		// Done
		return file;
	}

	inline void StdFileManager::closeFile(IFile& file) const
	{
		delete static_cast< ::detail::StdFile*>(&file);
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline bool StdFileManager::getAbsoluteDirectoryNamesByMountPoint(VirtualFilename virtualFilename, const AbsoluteDirectoryNames** absoluteDirectoryNames, std::string& relativeFilename, std::string& mountPoint) const
	{
		assert(nullptr != absoluteDirectoryNames);

		// Get mount point
		// TODO(co) Use "std::string_view" as soon as its available
		const std::string stdVirtualFilename = virtualFilename;
		const size_t slashIndex = stdVirtualFilename.find("/");
		if (std::string::npos != slashIndex)
		{
			mountPoint = stdVirtualFilename.substr(0, slashIndex);
			MountedDirectories::const_iterator iterator = mMountedDirectories.find(mountPoint);
			if (mMountedDirectories.cend() != iterator)
			{
				*absoluteDirectoryNames = &iterator->second;
				relativeFilename = stdVirtualFilename.substr(slashIndex + 1);

				// Done
				return true;
			}
			else
			{
				// Error!
				return false;
			}
		}
		else
		{
			// Use base directory
			*absoluteDirectoryNames = &mAbsoluteBaseDirectory;
			relativeFilename = virtualFilename;
			mountPoint.clear();

			// Done
			return true;
		}
	}

	inline std::string StdFileManager::mapVirtualToAbsoluteFilenameAndMountPoint(FileMode fileMode, VirtualFilename virtualFilename, std::string& mountPoint) const
	{
		// Sanity check
		assert(nullptr != virtualFilename);

		// Get absolute directory names
		const AbsoluteDirectoryNames* absoluteDirectoryNames = nullptr;
		std::string relativeFilename;
		if (getAbsoluteDirectoryNamesByMountPoint(virtualFilename, &absoluteDirectoryNames, relativeFilename, mountPoint) && absoluteDirectoryNames != nullptr)
		{
			if (mountPoint.empty())
			{
				// Support for absolute filenames
				return virtualFilename;
			}
			else
			{
				for (const std::string& absoluteDirectoryName : *absoluteDirectoryNames)
				{
					const std::string absoluteFilename = absoluteDirectoryName + '/' + relativeFilename;
					if (std_filesystem::exists(std_filesystem::u8path(absoluteFilename)))
					{
						return absoluteFilename;
					}
				}

				// Still here and writing a file?
				if (FileMode::WRITE == fileMode && !absoluteDirectoryNames->empty())
				{
					return (*absoluteDirectoryNames)[0] + '/' + relativeFilename;
				}
			}
		}

		// Error!
		return "";
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
