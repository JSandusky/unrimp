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


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IFile;
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
	*    Abstract file manager interface
	*/
	class IFileManager : public Manager
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		enum class FileMode
		{
			READ,
			WRITE
		};


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IFileManager methods  ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the absolute name of the directory were to write local data to
		*
		*  @return
		*    The absolute ASCII name of the directory were to write local data to (usually a user directory), has to end without /, if null pointer writing local data isn't allowed
		*
		*  @remarks
		*    Examples for local data
		*    - "DebugGui": ImGui "ini"-files storing session information
		*    - "PipelineStateObjectCache": Locally updated and saved pipeline state object cache in case the shipped one had cache misses
		*    - "RendererToolkitCache": Renderer toolkit cache used to detect source data changes for incremental asset compilation instead of time consuming full asset compilation
		*    - "Log": Log files, Unrimp itself won't save log files
		*/
		virtual const char* getAbsoluteLocalDataDirectoryName() const = 0;

		/**
		*  @brief
		*    Create directories recursive
		*
		*  @param[in] directoryName
		*    Name of the directory to create, including all parent directories if necessary, never ever a null pointer and always finished by a terminating zero
		*/
		virtual void createDirectories(const char* directoryName) const = 0;

		/**
		*  @brief
		*    Check whether or not a file exists
		*
		*  @param[in] filename
		*    ASCII name of the file to check for existence, never ever a null pointer and always finished by a terminating zero
		*
		*  @return
		*    "true" if the file does exist, else "false"
		*/
		virtual bool doesFileExist(const char* filename) const = 0;

		/**
		*  @brief
		*    Open a file
		*
		*  @param[in] fileMode
		*    File mode
		*  @param[in] filename
		*    ASCII name of the file to open for reading, never ever a null pointer and always finished by a terminating zero
		*
		*  @return
		*    The file interface, can be a null pointer if horrible things are happening (total failure)
		*/
		virtual IFile* openFile(FileMode fileMode, const char* filename) const = 0;

		/**
		*  @brief
		*    Close a file
		*
		*  @param[in] file
		*    File to close
		*/
		virtual void closeFile(IFile& file) const = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline IFileManager();
		inline virtual ~IFileManager();
		explicit IFileManager(const IFileManager&) = delete;
		IFileManager& operator=(const IFileManager&) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/File/IFileManager.inl"
