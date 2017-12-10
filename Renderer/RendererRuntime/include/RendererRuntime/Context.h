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
#include "RendererRuntime/Export.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class ILog;
	class IAssert;
	class IMemory;
	class IRenderer;
}
namespace RendererRuntime
{
	class IFileManager;
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
	*    Context class encapsulating all embedding related wirings
	*/
	class Context
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Renderer instance to use, the render instance must stay valid as long as the renderer runtime instance exists
		*  @param[in] fileManager
		*    File manager instance to use, the file manager instance must stay valid as long as the renderer runtime instance exists
		*/
		RENDERERRUNTIME_API_EXPORT Context(Renderer::IRenderer& renderer, IFileManager& fileManager);

		/**
		*  @brief
		*    Destructor
		*/
		inline ~Context();

		/**
		*  @brief
		*    Return the log instance
		*
		*  @return
		*    The log instance
		*/
		inline Renderer::ILog& getLog() const;

		/**
		*  @brief
		*    Return the assert instance
		*
		*  @return
		*    The assert instance
		*/
		inline Renderer::IAssert& getAssert() const;

		/**
		*  @brief
		*    Return the memory instance
		*
		*  @return
		*    The memory instance
		*/
		inline Renderer::IMemory& getMemory() const;

		/**
		*  @brief
		*    Return the used renderer instance
		*
		*  @return
		*    The used renderer instance
		*/
		inline Renderer::IRenderer& getRenderer() const;

		/**
		*  @brief
		*    Return the used file manager instance
		*
		*  @return
		*    The used file manager instance
		*/
		inline IFileManager& getFileManager() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Renderer::ILog&		 mLog;
		Renderer::IAssert&	 mAssert;
		Renderer::IMemory&	 mMemory;
		Renderer::IRenderer& mRenderer;
		IFileManager&		 mFileManager;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Context.inl"
