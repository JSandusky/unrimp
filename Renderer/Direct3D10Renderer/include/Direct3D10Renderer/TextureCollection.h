/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
#ifndef __DIRECT3D10RENDERER_TEXTURECOLLECTION_H__
#define __DIRECT3D10RENDERER_TEXTURECOLLECTION_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/ITextureCollection.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct ID3D10ShaderResourceView;
namespace Renderer
{
	class ITexture;
}
namespace Direct3D10Renderer
{
	class Direct3D10Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 10 texture collection class
	*/
	class TextureCollection : public Renderer::ITextureCollection
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D10Renderer
		*    Owner Direct3D 10 renderer instance
		*  @param[in] numberOfTextures
		*    The number of textures
		*  @param[in] textures
		*    The textures, can be a null pointer or can contain null pointers, if not a null pointer there must be at
		*    least "numberOfTextures" textures in the provided C-array of pointers
		*
		*  @note
		*    - The texture collection keeps a reference to the provided texture instances
		*/
		TextureCollection(Direct3D10Renderer &direct3D10Renderer, unsigned int numberOfTextures, Renderer::ITexture **textures);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~TextureCollection();

		/**
		*  @brief
		*    Return the number of Direct3D shader resource view instances
		*
		*  @return
		*    The number of Direct3D shader resource view instances
		*/
		inline unsigned int getNumberOfD3D10ShaderResourceViews() const;

		/**
		*  @brief
		*    Return the Direct3D shader resource view instances
		*
		*  @return
		*    The Direct3D shader resource view instances, can be a null pointer, do not release the returned instances unless you added an own reference to it
		*/
		inline ID3D10ShaderResourceView **getD3D10ShaderResourceViews() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char *name) override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		unsigned int			   mNumberOfD3D10ShaderResourceViews;	///< The number of Direct3D shader resource view instances
		ID3D10ShaderResourceView **mD3D10ShaderResourceViews;			///< The Direct3D shader resource view instances, can be a null pointer
		Renderer::ITexture		 **mTextures;							///< Texture instances (we keep a reference to it), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D10Renderer/TextureCollection.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D10RENDERER_TEXTURECOLLECTION_H__
