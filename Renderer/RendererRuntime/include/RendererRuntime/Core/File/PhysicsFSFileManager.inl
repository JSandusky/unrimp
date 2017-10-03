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

#include <Renderer/Public/Renderer.h>

extern "C"
{
	#include <physicsfs/physfs.h>
}

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	#include <fstream>
	#include <cassert>
PRAGMA_WARNING_POP
#ifdef WIN32
	// Disable warnings in external headers, we can't fix them
	__pragma(warning(push))
		__pragma(warning(disable: 4365))	// warning C4365: 'return': conversion from 'int' to 'std::_Rand_urng_from_func::result_type', signed/unsigned mismatch
		__pragma(warning(disable: 4548))	// warning C4548: expression before comma has no effect; expected expression with side-effect
		__pragma(warning(disable: 4571))	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
		__pragma(warning(disable: 4625))	// warning C4625: 'std::messages_base': copy constructor was implicitly defined as deleted
		__pragma(warning(disable: 4626))	// warning C4626: 'std::messages<char>': assignment operator was implicitly defined as deleted
		__pragma(warning(disable: 5026))	// warning C5026: 'std::messages_base': move constructor was implicitly defined as deleted
		__pragma(warning(disable: 5027))	// warning C5027: 'std::messages_base': move assignment operator was implicitly defined as deleted
		#include <filesystem>
	__pragma(warning(pop))
#else
	#ifdef UNRIMP_USE_BOOST_FILESYSTEM
		#include <boost/filesystem.hpp>
	#else
		#include <experimental/filesystem>
	#endif
#endif


//[-------------------------------------------------------]
//[ Global definitions                                    ]
//[-------------------------------------------------------]
#ifdef WIN32
	namespace std_filesystem = std::tr2::sys;
#else
	#ifdef UNRIMP_USE_BOOST_FILESYSTEM
		namespace std_filesystem = boost::filesystem;
	#else
		namespace std_filesystem = std::experimental::filesystem;
	#endif
#endif


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Definitions                                           ]
		//[-------------------------------------------------------]
		static const char* PHYSICSFS_LOCAL_DATA_MOUNT_POINT = "LocalData";


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
				mFileStream(virtualFilename, std::ios::binary)
			{
				// Nothing here
			}

			virtual ~PhysicsFSReadFile() override
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Public virtual PhysicsFSFile methods                  ]
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
			explicit PhysicsFSReadFile(const PhysicsFSReadFile&) = delete;
			PhysicsFSReadFile& operator=(const PhysicsFSReadFile&) = delete;


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			std::ifstream mFileStream;


		};

		class PhysicsFSWriteFile : public PhysicsFSFile
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			explicit PhysicsFSWriteFile(const char* virtualFilename) :
				mFileStream(virtualFilename, std::ios::binary)
			{
				// Nothing here
			}

			virtual ~PhysicsFSWriteFile() override
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Public virtual PhysicsFSFile methods                  ]
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
			explicit PhysicsFSWriteFile(const PhysicsFSWriteFile&) = delete;
			PhysicsFSWriteFile& operator=(const PhysicsFSWriteFile&) = delete;


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
			absoluteDirectoryName /= ::detail::PHYSICSFS_LOCAL_DATA_MOUNT_POINT;
			const char* absoluteLocalDataDirectoryName = absoluteDirectoryName.generic_string().c_str();
			if (mOwnsPhysicsFSInstance && 0 == PHYSFS_setWriteDir(absoluteLocalDataDirectoryName))
			{
				// TODO(co) Error: Use PHYSFS_getLastErrorCode() and PHYSFS_getErrorByCode() instead.
			}

			// Setup local data mount point
			mountDirectory(absoluteLocalDataDirectoryName, ::detail::PHYSICSFS_LOCAL_DATA_MOUNT_POINT);
		}
		else
		{
			// TODO(co) Error: Use PHYSFS_getLastErrorCode() and PHYSFS_getErrorByCode() instead.
		}
	}

	inline PhysicsFSFileManager::~PhysicsFSFileManager()
	{
		// Deinitialize the PhysicsFS library
		if (mOwnsPhysicsFSInstance && 0 == PHYSFS_deinit())
		{
			// TODO(co) Error: Use PHYSFS_getLastErrorCode() and PHYSFS_getErrorByCode() instead.
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
			assert(false && "Failed to mount the new directory");
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

	inline std::string PhysicsFSFileManager::mapVirtualToAbsoluteFilename(FileMode, VirtualFilename) const
	{
		// TODO(co) Implement me
		return "";
	}

	inline int64_t PhysicsFSFileManager::getLastModificationTime(VirtualFilename virtualFilename) const
	{
		// Sanity check
		assert(nullptr != virtualFilename);

		// Ask PhysicsFS
		return PHYSFS_getLastModTime(virtualFilename);
	}

	inline int64_t PhysicsFSFileManager::getFileSize(VirtualFilename virtualFilename) const
	{
		// Sanity check
		assert(nullptr != virtualFilename);

		// Ask PhysicsFS
		int64_t fileSize = -1;	// Error by default
		PHYSFS_file* physicsFSFile = PHYSFS_openRead(virtualFilename);
		if (nullptr != physicsFSFile)
		{
			fileSize = PHYSFS_fileLength(physicsFSFile);
			PHYSFS_close(physicsFSFile);
		}

		// Done
		return fileSize;
	}

	inline bool PhysicsFSFileManager::createDirectories(VirtualDirectoryName virtualDirectoryName) const
	{
		// Sanity check
		assert(nullptr != virtualDirectoryName);

		// Create directories
		return (PHYSFS_mkdir(virtualDirectoryName) != 0);
	}

	inline IFile* PhysicsFSFileManager::openFile(FileMode fileMode, VirtualFilename virtualFilename) const
	{
		// Sanity check
		assert(nullptr != virtualFilename);

		// TODO(co) Implement me

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
		// TODO(co) Implement me
		delete static_cast< ::detail::PhysicsFSFile*>(&file);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
