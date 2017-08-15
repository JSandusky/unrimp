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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract log interface
	*
	*  @note
	*    - The implementation must be multi-threading safe since the renderer is allowed to internally use multiple threads
	*/
	class ILog
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Log message type
		*/
		enum class Type
		{
			TRACE,					///< Trace
			DEBUG,					///< Debug
			INFORMATION,			///< Information
			WARNING,				///< General warning
			PERFORMANCE_WARNING,	///< Performance related warning
			COMPATIBILITY_WARNING,	///< Compatibility related warning
			CRITICAL				///< Critical
		};


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ILog methods                 ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Print a formated log message
		*
		*  @param[in] type
		*    Log message type
		*  @param[in] format
		*    "sprintf"-style formated log message
		*/
		virtual void print(Type type, const char* format, ...) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline ILog();
		inline virtual ~ILog();
		explicit ILog(const ILog&) = delete;
		ILog& operator=(const ILog&) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Macros & definitions                                  ]
//[-------------------------------------------------------]
/**
*  @brief
*    Ease-of-use log macro
*
*  @param[in] context
*    Renderer context to ask for the log interface
*  @param[in] type
*    Log message type
*  @param[in] format
*    "sprintf"-style formated log message
*
*  @note
*    - Example: RENDERER_LOG(mContext, DEBUG, "Direct3D 11 renderer backend startup")
*/
#define RENDERER_LOG(context, type, format, ...) (context).getLog().print(Renderer::ILog::Type::type, format, ##__VA_ARGS__);


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/ILog.inl"
