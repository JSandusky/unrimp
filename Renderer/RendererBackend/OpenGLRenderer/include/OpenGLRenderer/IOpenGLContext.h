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
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{
	class OpenGLRuntimeLinking;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract OpenGL context interface
	*
	*  @remarks
	*    While the OpenGL specification is platform independent, creating an OpenGL context is not.
	*
	*  @note
	*    - Every native OS window needs its own context instance
	*/
	class IOpenGLContext
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		virtual ~IOpenGLContext();


	//[-------------------------------------------------------]
	//[ Public virtual IOpenGLContext methods                 ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return whether or not the content is initialized
		*
		*  @return
		*    "true" if the context is initialized, else "false"
		*/
		virtual bool isInitialized() const = 0;

		/**
		*  @brief
		*    Make the context current
		*/
		virtual void makeCurrent() const = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRuntimeLinking
		*    OpenGL runtime linking instance, if null pointer this isn't a primary context
		*/
		explicit IOpenGLContext(OpenGLRuntimeLinking* openGLRuntimeLinking);

		explicit IOpenGLContext(const IOpenGLContext& source) = delete;
		IOpenGLContext& operator =(const IOpenGLContext& source) = delete;

		/**
		*  @brief
		*    Load the >= OpenGL 3.0 entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*
		*  @return
		*    "true" if all went fine, else "false"
		*
		*  @note
		*    - This method is only allowed to be called after an >= OpenGL context has been created and set
		*/
		bool loadOpenGL3EntryPoints() const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		OpenGLRuntimeLinking* mOpenGLRuntimeLinking;	///< OpenGL runtime linking instance, if null pointer this isn't a primary context


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
