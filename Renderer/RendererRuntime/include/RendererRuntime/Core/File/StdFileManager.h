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
#include "RendererRuntime/Core/File/IFileManager.h"

#include <string>


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
	*  @remarks
	*    For the Unrimp examples were using the following directory structure
	*    - "<root directory>/bin/x64_static"
	*    - "<root directory>/bin/DataPc"
	*    - "<root directory>/bin/LocalData"
	*    -> For end-user products, you might want to choose a local user data directory
	*    -> In here we assume that the current directory has not been changed and still points to the directory the running executable is in (e.g. "<root directory>/bin/x64_static")
	*
	*  @note
	*    - Designed to be instanced and used inside a single C++ file
	*/
	class StdFileManager : public IFileManager
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline StdFileManager();
		inline virtual ~StdFileManager();


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IFileManager methods  ]
	//[-------------------------------------------------------]
	public:
		inline virtual const char* getAbsoluteLocalDataDirectoryName() const override;
		inline virtual void createDirectories(const char* directoryName) const override;
		inline virtual IFile* openFile(FileMode fileMode, const char* filename) override;
		inline virtual void closeFile(IFile& file) override;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		explicit StdFileManager(const StdFileManager&) = delete;
		StdFileManager& operator=(const StdFileManager&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		const std::string mAbsoluteLocalDataDirectoryName;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/File/StdFileManager.inl"
