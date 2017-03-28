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
	*    Abstract texture interface
	*/
	class ITexture : public IResource
	{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Calculate the number of mipmaps for a 1D texture
		*
		*  @param[in] width
		*    Texture width
		*
		*  @return
		*    Number of mipmaps
		*
		*  @note
		*    - Do not add this within the public "Renderer/Public/Renderer.h"-header, it's for the internal implementation only
		*/
		static inline uint32_t getNumberOfMipmaps(uint32_t width);

		/**
		*  @brief
		*    Calculate the number of mipmaps for a 2D texture
		*
		*  @param[in] width
		*    Texture width
		*  @param[in] height
		*    Texture height
		*
		*  @return
		*    Number of mipmaps
		*
		*  @note
		*    - Do not add this within the public "Renderer/Public/Renderer.h"-header, it's for the internal implementation only
		*/
		static inline uint32_t getNumberOfMipmaps(uint32_t width, uint32_t height);

		/**
		*  @brief
		*    Calculate the number of mipmaps for a 3D texture
		*
		*  @param[in] width
		*    Texture width
		*  @param[in] height
		*    Texture height
		*  @param[in] depth
		*    Texture depth
		*
		*  @return
		*    Number of mipmaps
		*
		*  @note
		*    - Do not add this within the public "Renderer/Public/Renderer.h"-header, it's for the internal implementation only
		*/
		static inline uint32_t getNumberOfMipmaps(uint32_t width, uint32_t height, uint32_t depth);


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~ITexture();


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] resourceType
		*    The resource type
		*  @param[in] renderer
		*    Owner renderer instance
		*/
		inline ITexture(ResourceType resourceType, IRenderer &renderer);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit ITexture(const ITexture &source);

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
		inline ITexture &operator =(const ITexture &source);


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<ITexture> ITexturePtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/Texture/ITexture.inl"
