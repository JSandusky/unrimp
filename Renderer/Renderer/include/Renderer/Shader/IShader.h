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
#include "Renderer/IResource.h"


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
	*    Abstract shader interface
	*/
	class IShader : public IResource
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~IShader();


	//[-------------------------------------------------------]
	//[ Public virtual IShader methods                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the name of the shader language the shader is using
		*
		*  @return
		*    The ASCII name of the shader language the shader is using (for example "GLSL" or "HLSL"), always valid
		*
		*  @note
		*    - Do not free the memory the returned pointer is pointing to
		*/
		virtual const char *getShaderLanguageName() const = 0;

		/**
		*  @brief
		*    Return the shader source code
		*
		*  @return
		*    The shader ASCII source code, always valid
		*
		*  @note
		*    - Do not free the memory the returned pointer is pointing to
		*/
		// TODO(co) Think about it: Provide such methods in here?
		//virtual const char *getSourceCode() const = 0;

		/**
		*  @brief
		*    Return the name of the shader profile the shader is using
		*
		*  @return
		*    The ASCII name of the shader profile the shader is using (for example "arbvp1"), always valid
		*
		*  @note
		*    - Do not free the memory the returned pointer is pointing to
		*/
		// TODO(co) Think about it: Provide such methods in here?
		//virtual const char *getProfile() const = 0;

		/**
		*  @brief
		*    Return the optional shader compiler arguments the shader is using
		*
		*  @return
		*    The optional shader compiler ASCII arguments the shader is using, always valid
		*
		*  @note
		*    - Do not free the memory the returned pointer is pointing to
		*/
		// TODO(co) Think about it: Provide such methods in here?
		//virtual const char *getArguments() const = 0;

		/**
		*  @brief
		*    Return the name of the shader entry point the shader is using
		*
		*  @return
		*    The ASCII name of the shader entry point the shader is using (for example "main"), always valid
		*
		*  @note
		*    - Do not free the memory the returned pointer is pointing to
		*/
		// TODO(co) Think about it: Provide such methods in here?
		//virtual const char *getEntry() const = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] resourceType
		*    Resource type
		*  @param[in] renderer
		*    Owner renderer instance
		*/
		inline IShader(ResourceType resourceType, IRenderer &renderer);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit IShader(const IShader &source);

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
		inline IShader &operator =(const IShader &source);


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<IShader> IShaderPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/Shader/IShader.inl"
