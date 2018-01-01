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
#include "RendererRuntime/Core/File/IFileManager.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'std::codecvt_base': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'std::codecvt<char16_t,char,_Mbstatet>': assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4774)	// warning C4774: 'sprintf_s' : format string expected in argument 3 is not a string literal
	PRAGMA_WARNING_DISABLE_MSVC(5026)	// warning C5026: 'std::_Generic_error_category': move constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5027)	// warning C5027: 'std::_Generic_error_category': move assignment operator was implicitly defined as deleted
	#include <string>
	#include <unordered_map>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class ILog;
	class IAssert;
	class IAllocator;
}


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
	*    STD file manager implementation class one can use
	*
	*  @note
	*    - Designed to be instanced and used inside a single C++ file
	*    - Primarily for renderer toolkit with more relaxed write access
	*/
	class StdFileManager : public IFileManager
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline StdFileManager(Renderer::ILog& log, Renderer::IAssert& assert, Renderer::IAllocator& allocator, const std::string& relativeRootDirectory);
		inline virtual ~StdFileManager() override;


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IFileManager methods  ]
	//[-------------------------------------------------------]
	public:
		inline virtual const char* getLocalDataMountPoint() const override;
		inline virtual bool mountDirectory(AbsoluteDirectoryName absoluteDirectoryName, const char* mountPoint, bool appendToPath = false) override;
		inline virtual bool doesFileExist(VirtualFilename virtualFilename) const override;
		inline virtual void enumerateFiles(VirtualDirectoryName virtualDirectoryName, EnumerationMode enumerationMode, std::vector<std::string>& virtualFilenames) const override;
		inline virtual std::string mapVirtualToAbsoluteFilename(FileMode fileMode, VirtualFilename virtualFilename) const override;
		inline virtual int64_t getLastModificationTime(VirtualFilename virtualFilename) const override;
		inline virtual int64_t getFileSize(VirtualFilename virtualFilename) const override;
		inline virtual bool createDirectories(VirtualDirectoryName virtualDirectoryName) const override;
		inline virtual IFile* openFile(FileMode fileMode, VirtualFilename virtualFilename) const override;
		inline virtual void closeFile(IFile& file) const override;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		explicit StdFileManager(const StdFileManager&) = delete;
		StdFileManager& operator=(const StdFileManager&) = delete;


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::vector<std::string>								AbsoluteDirectoryNames;
		typedef std::unordered_map<std::string, AbsoluteDirectoryNames>	MountedDirectories;	///< Key = UTF-8 mount point name (example: "MyProject"), value = absolute UTF-8 names of the mounted directories (example: "c:/MyProject")


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline bool getAbsoluteDirectoryNamesByMountPoint(VirtualFilename virtualFilename, const AbsoluteDirectoryNames** absoluteDirectoryNames, std::string& relativeFilename, std::string& mountPoint) const;
		inline std::string mapVirtualToAbsoluteFilenameAndMountPoint(FileMode fileMode, VirtualFilename virtualFilename, std::string& mountPoint) const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Renderer::ILog&		   mLog;
		Renderer::IAssert&	   mAssert;
		Renderer::IAllocator&  mAllocator;
		AbsoluteDirectoryNames mAbsoluteBaseDirectory;	///< Absolute UTF-8 base directory, without "/" at the end
		MountedDirectories	   mMountedDirectories;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/File/StdFileManager.inl"
