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

extern "C"
{
	#include <physicsfs/physfs.h>
}

#include <tuple>	// For "std::ignore"


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
		static const char* PHYSICSFS_LOCAL_DATA_MOUNT_POINT = "LocalData";


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		void writePhysicsFSErrorToLog(Renderer::ILog& log)
		{
			const char* errorAsString = PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
			if (nullptr != errorAsString)
			{
				log.print(Renderer::ILog::Type::CRITICAL, "PhysicsFS error: %s", errorAsString);
			}
			else
			{
				assert(false && "Failed to map PhysicsFS error code to error as string");
			}
		}


		//[-------------------------------------------------------]
		//[ Classes                                               ]
		//[-------------------------------------------------------]
		class PhysicsFSFile : public RendererRuntime::IFile
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			PhysicsFSFile()
			{
				// Nothing here
			}

			virtual ~PhysicsFSFile() override
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Public virtual PhysicsFSFile methods                  ]
		//[-------------------------------------------------------]
		public:
			virtual bool isInvalid() const = 0;


		//[-------------------------------------------------------]
		//[ Protected methods                                     ]
		//[-------------------------------------------------------]
		protected:
			explicit PhysicsFSFile(const PhysicsFSFile&) = delete;
			PhysicsFSFile& operator=(const PhysicsFSFile&) = delete;


		};

		class PhysicsFSReadFile : public PhysicsFSFile
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			explicit PhysicsFSReadFile(const char* virtualFilename) :
				mPhysicsFSFile(PHYSFS_openRead(virtualFilename))
			{
				assert((nullptr != mPhysicsFSFile) && "Failed to open PhysicsFS file for reading");
			}

			virtual ~PhysicsFSReadFile() override
			{
				if (nullptr != mPhysicsFSFile)
				{
					const int result = PHYSFS_close(mPhysicsFSFile);
					std::ignore = result;
					assert((0 != result) && "Failed to close read PhysicsFS file");
				}
			}


		//[-------------------------------------------------------]
		//[ Public virtual PhysicsFSFile methods                  ]
		//[-------------------------------------------------------]
		public:
			virtual bool isInvalid() const override
			{
				return (nullptr == mPhysicsFSFile);
			}


		//[-------------------------------------------------------]
		//[ Public virtual RendererRuntime::IFile methods         ]
		//[-------------------------------------------------------]
		public:
			virtual size_t getNumberOfBytes() override
			{
				assert(nullptr != mPhysicsFSFile && "Invalid PhysicsFS file access");
				const PHYSFS_sint64 fileLength = PHYSFS_fileLength(mPhysicsFSFile);
				assert((-1 != fileLength) && "PhysicsFS failed to determine the file size");
				return static_cast<size_t>(fileLength);
			}

			virtual void read(void* destinationBuffer, size_t numberOfBytes) override
			{
				assert(nullptr != mPhysicsFSFile && "Invalid PhysicsFS file access");
				const PHYSFS_sint64 numberOfReadBytes = PHYSFS_readBytes(mPhysicsFSFile, destinationBuffer, numberOfBytes);
				std::ignore = numberOfReadBytes;
				assert((static_cast<size_t>(numberOfReadBytes) == numberOfBytes) && "PhysicsFS failed to read all requested bytes");	// We're restrictive by intent
			}

			virtual void skip(size_t numberOfBytes) override
			{
				assert(nullptr != mPhysicsFSFile && "Invalid PhysicsFS file access");
				const PHYSFS_sint64 currentOffset = PHYSFS_tell(mPhysicsFSFile);
				assert(-1 != currentOffset && "PhysicsFS failed to retrieve the current file offset");
				const int result = PHYSFS_seek(mPhysicsFSFile, static_cast<PHYSFS_uint64>(currentOffset + numberOfBytes));
				std::ignore = result;
				assert((0 != result) && "PhysicsFS failed seek file");
			}

			virtual void write(const void*, size_t) override
			{
				assert(nullptr != mPhysicsFSFile && "Invalid PhysicsFS file access");
				assert(false && "File write method not supported by the PhysicsFS implementation");
			}


		//[-------------------------------------------------------]
		//[ Protected methods                                     ]
		//[-------------------------------------------------------]
		protected:
			explicit PhysicsFSReadFile(const PhysicsFSReadFile&) = delete;
			PhysicsFSReadFile& operator=(const PhysicsFSReadFile&) = delete;


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			PHYSFS_File* mPhysicsFSFile;


		};

		class PhysicsFSWriteFile : public PhysicsFSFile
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			explicit PhysicsFSWriteFile(const char* virtualFilename) :
				mPhysicsFSFile(PHYSFS_openWrite(virtualFilename))
			{
				assert((nullptr != mPhysicsFSFile) && "Failed to open PhysicsFS file for writing");
			}

			virtual ~PhysicsFSWriteFile() override
			{
				if (nullptr != mPhysicsFSFile)
				{
					const int result = PHYSFS_close(mPhysicsFSFile);
					std::ignore = result;
					assert((0 != result) && "Failed to close written PhysicsFS file");
				}
			}


		//[-------------------------------------------------------]
		//[ Public virtual PhysicsFSFile methods                  ]
		//[-------------------------------------------------------]
		public:
			virtual bool isInvalid() const override
			{
				return (nullptr == mPhysicsFSFile);
			}


		//[-------------------------------------------------------]
		//[ Public virtual RendererRuntime::IFile methods         ]
		//[-------------------------------------------------------]
		public:
			virtual size_t getNumberOfBytes() override
			{
				assert(nullptr != mPhysicsFSFile && "Invalid PhysicsFS file access");
				assert(false && "File get number of bytes method not supported by the PhysicsFS implementation");
				return 0;
			}

			virtual void read(void*, size_t) override
			{
				assert(nullptr != mPhysicsFSFile && "Invalid PhysicsFS file access");
				assert(false && "File read method not supported by the PhysicsFS implementation");
			}

			virtual void skip(size_t) override
			{
				assert(nullptr != mPhysicsFSFile && "Invalid PhysicsFS file access");
				assert(false && "File skip method not supported by the PhysicsFS implementation");
			}

			virtual void write(const void* sourceBuffer, size_t numberOfBytes) override
			{
				assert(nullptr != mPhysicsFSFile && "Invalid PhysicsFS file access");
				const PHYSFS_sint64 numberOfWrittenBytes = PHYSFS_writeBytes(mPhysicsFSFile, sourceBuffer, numberOfBytes);
				std::ignore = numberOfWrittenBytes;
				assert((static_cast<size_t>(numberOfWrittenBytes) == numberOfBytes) && "PhysicsFS failed to write all requested bytes");	// We're restrictive by intent
			}


		//[-------------------------------------------------------]
		//[ Protected methods                                     ]
		//[-------------------------------------------------------]
		protected:
			explicit PhysicsFSWriteFile(const PhysicsFSWriteFile&) = delete;
			PhysicsFSWriteFile& operator=(const PhysicsFSWriteFile&) = delete;


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			PHYSFS_File* mPhysicsFSFile;


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
	inline PhysicsFSFileManager::PhysicsFSFileManager(Renderer::ILog& log, const std::string& relativeRootDirectory, bool ownsPhysicsFSInstance) :
		IFileManager(relativeRootDirectory),
		mLog(log),
		mOwnsPhysicsFSInstance(ownsPhysicsFSInstance)
	{
		// Initialize the PhysicsFS library
		if (!mOwnsPhysicsFSInstance || 0 != PHYSFS_init(nullptr))
		{
			std_filesystem::path absoluteDirectoryName = std_filesystem::canonical(std_filesystem::current_path());
			if (!relativeRootDirectory.empty())
			{
				absoluteDirectoryName /= relativeRootDirectory;
			}
			if (mOwnsPhysicsFSInstance && 0 == PHYSFS_setWriteDir(absoluteDirectoryName.generic_string().c_str()))
			{
				// Error!
				::detail::writePhysicsFSErrorToLog(mLog);
			}
			absoluteDirectoryName /= ::detail::PHYSICSFS_LOCAL_DATA_MOUNT_POINT;
			const std::string absoluteLocalDataDirectoryName = absoluteDirectoryName.generic_string();

			// Setup local data mount point
			createDirectories(::detail::PHYSICSFS_LOCAL_DATA_MOUNT_POINT);
			mountDirectory(absoluteLocalDataDirectoryName.c_str(), ::detail::PHYSICSFS_LOCAL_DATA_MOUNT_POINT);
		}
		else
		{
			// Error!
			::detail::writePhysicsFSErrorToLog(mLog);
		}
	}

	inline PhysicsFSFileManager::~PhysicsFSFileManager()
	{
		// Deinitialize the PhysicsFS library
		if (mOwnsPhysicsFSInstance && 0 == PHYSFS_deinit())
		{
			// Error!
			::detail::writePhysicsFSErrorToLog(mLog);
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IFileManager methods  ]
	//[-------------------------------------------------------]
	inline const char* PhysicsFSFileManager::getLocalDataMountPoint() const
	{
		return ::detail::PHYSICSFS_LOCAL_DATA_MOUNT_POINT;
	}

	inline bool PhysicsFSFileManager::mountDirectory(AbsoluteDirectoryName absoluteDirectoryName, const char* mountPoint, bool appendToPath)
	{
		// Sanity check
		assert(nullptr != absoluteDirectoryName);
		assert(nullptr != mountPoint);

		// Mount directory
		if (0 == PHYSFS_mount(absoluteDirectoryName, mountPoint, appendToPath))
		{
			// Error!
			::detail::writePhysicsFSErrorToLog(mLog);
			return false;
		}

		// Done
		return true;
	}

	inline bool PhysicsFSFileManager::doesFileExist(VirtualFilename virtualFilename) const
	{
		// Sanity check
		assert(nullptr != virtualFilename);

		// Check for file existence
		return (0 != PHYSFS_exists(virtualFilename));
	}

	inline void PhysicsFSFileManager::enumerateFiles(VirtualDirectoryName virtualDirectoryName, EnumerationMode enumerationMode, std::vector<std::string>& virtualFilenames) const
	{
		char** physicsFsFilenames = PHYSFS_enumerateFiles(virtualDirectoryName);
		switch (enumerationMode)
		{
			case EnumerationMode::ALL:
			{
				for (char** physicsFsFilename = physicsFsFilenames; nullptr != *physicsFsFilename; ++physicsFsFilename)
				{
					virtualFilenames.push_back(*physicsFsFilename);
				}
				break;
			}

			case EnumerationMode::FILES:
			{
				const std::string stdVirtualDirectoryName = virtualDirectoryName;
				for (char** physicsFsFilename = physicsFsFilenames; nullptr != *physicsFsFilename; ++physicsFsFilename)
				{
					if (PHYSFS_isDirectory((stdVirtualDirectoryName + '/' + *physicsFsFilename).c_str()) == 0)
					{
						virtualFilenames.push_back(*physicsFsFilename);
					}
				}
				break;
			}

			case EnumerationMode::DIRECTORIES:
			{
				const std::string stdVirtualDirectoryName = virtualDirectoryName;
				for (char** physicsFsFilename = physicsFsFilenames; nullptr != *physicsFsFilename; ++physicsFsFilename)
				{
					if (PHYSFS_isDirectory((stdVirtualDirectoryName + '/' + *physicsFsFilename).c_str()) != 0)
					{
						virtualFilenames.push_back(*physicsFsFilename);
					}
				}
				break;
			}
		}
		PHYSFS_freeList(physicsFsFilenames);
	}

	inline std::string PhysicsFSFileManager::mapVirtualToAbsoluteFilename(FileMode fileMode, VirtualFilename virtualFilename) const
	{
		// Figure out where in the search path a file resides (e.g. "LocalData/DebugGui/UnrimpDebugGuiLayout.ini" -> "c:/MyProject/bin/LocalData")
		const char* realDirectory = PHYSFS_getRealDir(virtualFilename);
		if (nullptr != realDirectory)
		{
			// Determine a mounted archive mount point (e.g. "c:/MyProject/bin/LocalData" -> "LocalData")
			const char* mountPoint = PHYSFS_getMountPoint(realDirectory);
			if (nullptr == mountPoint)
			{
				// The mount point is the root, so, determining the absolute filename is trivial
				return std::string(realDirectory) + '/' + virtualFilename;
			}
			else
			{
				// Find the mount point part inside the given virtual filename
				// TODO(co) Use "std::string_view" as soon as its available
				const size_t index = std::string(virtualFilename).find_first_of(mountPoint);
				if (std::string::npos != index)
				{
					// Now that we have all information we need, transform the given virtual filename into an absolute filename
					// -> Example: The virtual filename "LocalData/DebugGui/UnrimpDebugGuiLayout.ini" will result in the absolute filename "c:/MyProject/bin/LocalData/DebugGui/UnrimpDebugGuiLayout.ini"
					std::string absoluteFilename = virtualFilename;
					absoluteFilename.erase(index, strlen(mountPoint));	// Example: "LocalData/DebugGui/UnrimpDebugGuiLayout.ini" to "DebugGui/UnrimpDebugGuiLayout.ini"
					absoluteFilename = std::string(realDirectory) + '/' + absoluteFilename;

					// Done
					return absoluteFilename;
				}
			}
		}

		// File not found, guess location of a newly created file?
		else if (FileMode::WRITE == fileMode)
		{
			// Get the absolute filename of the directory a newly created file would be in
			// -> Example: The virtual filename "LocalData/DebugGui/UnrimpDebugGuiLayout.ini" will result in the absolute directory name "c:/MyProject/bin/LocalData/DebugGui"
			const std_filesystem::path path(virtualFilename);
			const std::string absoluteDirectoryName = mapVirtualToAbsoluteFilename(fileMode, path.parent_path().generic_string().c_str());
			if (!absoluteDirectoryName.empty())
			{
				// Construct the absolute filename
				return absoluteDirectoryName + '/' + path.filename().string();
			}
		}

		// Error!
		assert(false && "Failed to map virtual to PhysicsFS absolute filename");
		return "";
	}

	inline int64_t PhysicsFSFileManager::getLastModificationTime(VirtualFilename virtualFilename) const
	{
		// Sanity check
		assert(nullptr != virtualFilename);

		// Ask PhysicsFS
		PHYSFS_Stat physicsFSStat = {};
		if (0 == PHYSFS_stat(virtualFilename, &physicsFSStat))
		{
			// Error!
			assert(false && "Failed to get PhysicsFS last file modification time");
			::detail::writePhysicsFSErrorToLog(mLog);
			return 0;
		}
		else
		{
			return physicsFSStat.modtime;
		}
	}

	inline int64_t PhysicsFSFileManager::getFileSize(VirtualFilename virtualFilename) const
	{
		// Sanity check
		assert(nullptr != virtualFilename);

		// Ask PhysicsFS
		PHYSFS_Stat physicsFSStat = {};
		if (0 == PHYSFS_stat(virtualFilename, &physicsFSStat))
		{
			// Error!
			assert(false && "Failed to get PhysicsFS file size");
			::detail::writePhysicsFSErrorToLog(mLog);
			return 0;
		}
		else
		{
			return physicsFSStat.filesize;
		}
	}

	inline bool PhysicsFSFileManager::createDirectories(VirtualDirectoryName virtualDirectoryName) const
	{
		// Sanity check
		assert(nullptr != virtualDirectoryName);

		// Create directories
		const int result = PHYSFS_mkdir(virtualDirectoryName);
		assert(0 != result && "PhysicsFS failed to create the directories");
		return (result != 0);
	}

	inline IFile* PhysicsFSFileManager::openFile(FileMode fileMode, VirtualFilename virtualFilename) const
	{
		// Sanity check
		assert(nullptr != virtualFilename);

		// Open file
		::detail::PhysicsFSFile* file = nullptr;
		if (FileMode::READ == fileMode)
		{
			file = new ::detail::PhysicsFSReadFile(virtualFilename);
		}
		else
		{
			file = new ::detail::PhysicsFSWriteFile(virtualFilename);
		}
		if (file->isInvalid())
		{
			mLog.print(Renderer::ILog::Type::CRITICAL, "Failed to open file %s", virtualFilename);
			delete file;
			file = nullptr;
		}
		return file;
	}

	inline void PhysicsFSFileManager::closeFile(IFile& file) const
	{
		delete static_cast< ::detail::PhysicsFSFile*>(&file);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
