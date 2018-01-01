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
#include <Renderer/Public/Renderer.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'std::_Generic_error_category': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'std::_Generic_error_category': assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5026)	// warning C5026: 'std::_Generic_error_category': move constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5027)	// warning C5027: 'std::_Generic_error_category': move assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5039)	// warning C5039: '_Thrd_start': pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior may occur if this function throws an exception.
	#include <mutex>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    STD log implementation class one can use
	*
	*  @note
	*    - Designed to be instanced and used inside a single C++ file
	*    - On MS Windows it will print to the Visual Studio output console, on critical message the debugger will break
	*    - On Linux it will print on the console
	*/
	class StdLog : public ILog
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline StdLog();
		inline virtual ~StdLog() override;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ILog methods                 ]
	//[-------------------------------------------------------]
	public:
		inline virtual bool print(Type type, const char* attachment, const char* file, uint32_t line, const char* format, ...) override;


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::StdLog methods            ]
	//[-------------------------------------------------------]
	protected:
		/*
		*  @brief
		*    Receives an already formatted message for further processing
		*
		*  @param[in] type
		*    Log message type
		*  @param[in] attachment
		*    Optional attachment (for example build shader source code), can be a null pointer
		*  @param[in] file
		*    File as ASCII string
		*  @param[in] line
		*    Line number
		*  @param[in] message
		*    UTF-8 message
		*  @param[in] numberOfCharacters
		*    Number of characters inside the message, does not include the terminating zero character
		*
		*  @return
		*    "true" to request debug break, else "false"
		*/
		inline virtual bool printInternal(Type type, const char* attachment, const char* file, uint32_t line, const char* message, uint32_t numberOfCharacters);


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline const char* typeToString(Type type) const;


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		std::mutex mMutex;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit StdLog(const StdLog&) = delete;
		StdLog& operator=(const StdLog&) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include <Renderer/Public/StdLog.inl>
