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
#include "RendererToolkit/Helper/AssimpIOSystem.h"

#include <RendererRuntime/Core/File/IFile.h>
#include <RendererRuntime/Core/File/IFileManager.h>

#include <assimp/IOStream.hpp>

#include <assert.h>


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
		class AssimpIOStream : public Assimp::IOStream
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			inline explicit AssimpIOStream(RendererRuntime::IFile& file) :
				mFile(file),
				mNumberOfBytes(mFile.getNumberOfBytes()),
				mCurrentPosition(0)
			{
				// Nothing here
			}

			inline virtual ~AssimpIOStream() override
			{
				// Nothing here
			}

			inline RendererRuntime::IFile& getFile() const
			{
				return mFile;
			}


		//[-------------------------------------------------------]
		//[ Public virtual Assimp::IOStream methods               ]
		//[-------------------------------------------------------]
		public:
			virtual size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override
			{
				size_t numberOfBytes = pSize * pCount;
				size_t newCurrentPosition = mCurrentPosition + numberOfBytes;
				if (newCurrentPosition > mNumberOfBytes)
				{
					const size_t numberOfOverflowBytes = (newCurrentPosition - mNumberOfBytes);
					numberOfBytes -= numberOfOverflowBytes;
					newCurrentPosition -= numberOfOverflowBytes;
				}
				if (0 != numberOfBytes)
				{
					mFile.read(pvBuffer, numberOfBytes);
					mCurrentPosition = newCurrentPosition;
				}
				return numberOfBytes;
			}

			inline virtual size_t Write(const void*, size_t, size_t) override
			{
				assert(false && "We only support read-only Assimp files");
				return 0;
			}

			virtual aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override
			{
				assert(aiOrigin_END != pOrigin && "We don't support \"aiOrigin_END\" in read-only Assimp files");
				if (aiOrigin_SET == pOrigin)
				{
					assert(pOffset >= mCurrentPosition && "We only support unidirectional sequential byte skipping");
					pOffset = pOffset - mCurrentPosition;
				}
				if (0 != pOffset)
				{
					mFile.skip(pOffset);
					mCurrentPosition += pOffset;
				}
				return aiReturn_SUCCESS;
			}

			inline virtual size_t Tell() const override
			{
				return mCurrentPosition;
			}

			inline virtual size_t FileSize() const override
			{
				return mNumberOfBytes;
			}

			inline virtual void Flush() override
			{
				assert(false && "We only support read-only Assimp files");
			}


		//[-------------------------------------------------------]
		//[ Private methods                                       ]
		//[-------------------------------------------------------]
		private:
			AssimpIOStream(const AssimpIOStream&) = delete;
			AssimpIOStream& operator=(const AssimpIOStream&) = delete;


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			RendererRuntime::IFile& mFile;
			size_t					mNumberOfBytes;
			size_t					mCurrentPosition;	///< Current position inside the file in bytes


		};


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public virtual Assimp::IOSystem methods               ]
	//[-------------------------------------------------------]
	bool AssimpIOSystem::Exists(const char* pFile) const
	{
		return mFileManager.doesFileExist(pFile);
	}

	Assimp::IOStream* AssimpIOSystem::Open(const char* pFile, const char* pMode)
	{
		assert(nullptr != pFile);
		assert(nullptr != pMode);
		if (stricmp(pMode, "rb") != 0)
		{
			throw std::runtime_error("We only support read-only Assimp files");
		}
		RendererRuntime::IFile* file = mFileManager.openFile(RendererRuntime::IFileManager::FileMode::READ, pFile);
		return (nullptr != file) ? new ::detail::AssimpIOStream(*file) : nullptr;
	}

	void AssimpIOSystem::Close(Assimp::IOStream* pFile)
	{
		assert(nullptr != pFile);
		::detail::AssimpIOStream* assimpIOStream = static_cast< ::detail::AssimpIOStream*>(pFile);
		mFileManager.closeFile(assimpIOStream->getFile());
		delete assimpIOStream;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
