/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#ifndef __OPENGLRENDERER_ICONTEXT_H__
#define __OPENGLRENDERER_ICONTEXT_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/PlatformTypes.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{
	class Extensions;
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
	*/
	class IContext
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		virtual ~IContext();

		/**
		*  @brief
		*    Return the available extensions
		*
		*  @return
		*    The available extensions, do not free the memory the reference is pointing to
		*/
		inline const Extensions &getExtensions() const;

		/**
		*  @brief
		*    Return the available extensions
		*
		*  @return
		*    The available extensions, do not free the memory the reference is pointing to
		*/
		inline Extensions &getExtensions();


	//[-------------------------------------------------------]
	//[ Public virtual IContext methods                       ]
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
		*    Make the context current to the given window handle
		*
		*  @param[in] nativeWindowHandle
		*    Optional native main window handle, can be a null handle (to unbind the context from any window)
		*/
		virtual void makeCurrent(handle nativeWindowHandle) const = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Default constructor
		*/
		IContext();

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		explicit IContext(const IContext &source);

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		IContext &operator =(const IContext &source);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Extensions *mExtensions;	///< Extensions instance, always valid


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/IContext.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_ICONTEXT_H__
