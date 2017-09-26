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
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class ILog;
}
namespace RendererRuntime
{
	class IFileManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*   Interface for progress logging
	*/
	class IProgressLog
	{


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IProgressLog methods  ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Prints a single formatted message
		* 
		*  @param[in] format
		*    "sprintf"-style formatted log message nullptr can be given
		* 
		*  @note
		*    No new line is appended to the output
		*/
		virtual void print(const char* format, ...) = 0;

		/**
		*  @brief
		*    Prints a single formatted message as line
		* 
		*  @param[in] format
		*    "sprintf"-style formatted log message nullptr can be given
		* 
		*  @note
		*    A new line is appended to the output
		*/
		virtual void printLine(const char* format, ...) = 0;

		/**
		*  @brief
		*    Prints an progress
		* 
		*  @param[in] current
		*    The current progress value
		*  @param[in] max
		*    The max progress value
		*/
		virtual void printProgress(uint32_t current, uint32_t max) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline IProgressLog()
		{}
		inline virtual ~IProgressLog()
		{}
		explicit IProgressLog(const IProgressLog&) = delete;
		IProgressLog& operator=(const IProgressLog&) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
