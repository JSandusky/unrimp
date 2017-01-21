/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
#include "RendererRuntime/Core/NonCopyable.h"

#include <inttypes.h>	// For uint32_t, uint64_t etc.


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
	*    Abstract file interface
	*
	*  @note
	*    - There's no "seek()" or "tell()" by intent, the renderer toolkit can transform the data so during runtime the most simple file access pattern is sufficient
	*    - It's only called "file interface" because this is usually the name for such a concept, doesn't mean the concrete implementation needs to work with files
	*    - Renderer runtime loaders try to read bigger chunks as often as possible instead of too fine granular byte wise read calls
	*/
	class IFile : public NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IFile methods         ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the number of bytes inside the file
		*
		*  @return
		*    The number of bytes inside the file
		*/
		virtual size_t getNumberOfBytes() = 0;

		/**
		*  @brief
		*    Read a requested number of bytes from the file
		*
		*  @param[out] destinationBuffer
		*    Destination buffer were to write to, must be at least "numberOfBytes" long, never ever a null pointer
		*  @param[in] numberOfBytes
		*    Number of bytes to write into the destination buffer, it's the callers responsibility that this number of byte is correct
		*/
		virtual void read(void* destinationBuffer, size_t numberOfBytes) = 0;

		/**
		*  @brief
		*    Skip a requested number of bytes
		*
		*  @param[in] numberOfBytes
		*    Number of bytes to skip, it's the callers responsibility that this number of byte is correct
		*/
		virtual void skip(size_t numberOfBytes) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline IFile();
		inline virtual ~IFile();
		IFile(const IFile&) = delete;
		IFile& operator=(const IFile&) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/File/IFile.inl"
