/*********************************************************\
* Copyright (c) 2013 Stephan Wezel
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


// Comfort header putting some items together within a single header without any additional header inclusion which might be useful for an user of unrimp 
// TODO(co) Review whether or not we really need and want such a header. Was added by SW. Will get hard to mantain it. The one and only point of "Renderer.h" was, to have a single header to access the complete API (no backend related information).


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __OPENGLRENDERE_SINGLE__
#define __OPENGLRENDERE_SINGLE__


//OpenGLRenderer/TextureCollection.h
#ifndef __OPENGLRENDERER_TEXTURECOLLECTION_H__
#define __OPENGLRENDERER_TEXTURECOLLECTION_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class ITexture;
}
namespace OpenGLRenderer
{
	class OpenGLRenderer;
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
	*    OpenGL texture collection class
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
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] numberOfTextures
		*    The number of textures
		*  @param[in] textures
		*    The textures, can be a null pointer or can contain null pointers, if not a null pointer there must be at
		*    least "numberOfTextures" textures in the provided C-array of pointers
		*
		*  @note
		*    - The texture collection keeps a reference to the provided texture instances
		*/
		TextureCollection(OpenGLRenderer &openGLRenderer, unsigned int numberOfTextures, Renderer::ITexture **textures);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~TextureCollection();

		/**
		*  @brief
		*    Return the number of texture instances
		*
		*  @return
		*    The number of texture instances
		*/
		inline unsigned int getNumberOfTextures() const;

		/**
		*  @brief
		*    Return the texture instances
		*
		*  @return
		*    The texture instances, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline Renderer::ITexture **getTextures() const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		unsigned int		 mNumberOfTextures;	///< The number of texture instances
		Renderer::ITexture **mTextures;			///< Texture instances (we keep a reference to it), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
//OpenGLRenderer/TextureCollection.inl
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline unsigned int TextureCollection::getNumberOfTextures() const
	{
		return mNumberOfTextures;
	}

	inline Renderer::ITexture **TextureCollection::getTextures() const
	{
		return mTextures;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_TEXTURECOLLECTION_H__
//OpenGLRenderer/TextureBuffer.h
#ifndef __OPENGLRENDERER_TEXTUREBUFFER_H__
#define __OPENGLRENDERER_TEXTUREBUFFER_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{
	class OpenGLRenderer;
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
	*    Abstract OpenGL texture buffer object (TBO) interface
	*/
	class TextureBuffer : public Renderer::ITextureBuffer
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		virtual ~TextureBuffer();

		/**
		*  @brief
		*    Return the OpenGL texture buffer instance
		*
		*  @return
		*    The OpenGL texture buffer instance, can be zero if no resource is allocated, do not destroy the returned resource (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline unsigned int getOpenGLTextureBuffer() const;

		/**
		*  @brief
		*    Return the OpenGL texture instance
		*
		*  @return
		*    The OpenGL texture instance, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline unsigned int getOpenGLTexture() const;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*/
		explicit TextureBuffer(OpenGLRenderer &openGLRenderer);


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		unsigned int mOpenGLTextureBuffer;	///< OpenGL texture buffer, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)
		unsigned int mOpenGLTexture;		///< OpenGL texture, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
//OpenGLRenderer/TextureBuffer.inl
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline unsigned int TextureBuffer::getOpenGLTextureBuffer() const
	{
		return mOpenGLTextureBuffer;
	}

	inline unsigned int TextureBuffer::getOpenGLTexture() const
	{
		return mOpenGLTexture;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_TEXTUREBUFFER_H__
//OpenGLRenderer/TextureBufferDsa.h
#ifndef __OPENGLRENDERER_TEXTUREBUFFER_DSA_H__
#define __OPENGLRENDERER_TEXTUREBUFFER_DSA_H__



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
	*    OpenGL texture buffer object (UBO, "constant buffer" in Direct3D terminology) class, effective direct state access (DSA)
	*/
	class TextureBufferDsa : public TextureBuffer
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] numberOfBytes
		*    Number of bytes within the texture buffer, must be valid
		*  @param[in] textureFormat
		*    Texture buffer data format
		*  @param[in] data
		*    Texture buffer data, can be a null pointer (empty buffer)
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*/
		TextureBufferDsa(OpenGLRenderer &openGLRenderer, unsigned int numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~TextureBufferDsa();


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureBuffer methods       ]
	//[-------------------------------------------------------]
	public:
		virtual void copyDataFrom(unsigned int numberOfBytes, const void *data) override;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_TEXTUREBUFFER_DSA_H__
//OpenGLRenderer/TextureBufferBind.h
#ifndef __OPENGLRENDERER_TEXTUREBUFFER_BIND_H__
#define __OPENGLRENDERER_TEXTUREBUFFER_BIND_H__



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
	*    OpenGL texture buffer object (TBO) class, traditional bind version
	*/
	class TextureBufferBind : public TextureBuffer
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] numberOfBytes
		*    Number of bytes within the texture buffer, must be valid
		*  @param[in] textureFormat
		*    Texture buffer data format
		*  @param[in] data
		*    Texture buffer data, can be a null pointer (empty buffer)
		*  @param[in] bufferUsage
		*    Indication of the buffer usage
		*/
		TextureBufferBind(OpenGLRenderer &openGLRenderer, unsigned int numberOfBytes, Renderer::TextureFormat::Enum textureFormat, const void *data = nullptr, Renderer::BufferUsage::Enum bufferUsage = Renderer::BufferUsage::DYNAMIC_DRAW);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~TextureBufferBind();


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITextureBuffer methods       ]
	//[-------------------------------------------------------]
	public:
		virtual void copyDataFrom(unsigned int numberOfBytes, const void *data) override;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_TEXTUREBUFFER_BIND_H__
//OpenGLRenderer/Texture2D.h
#ifndef __OPENGLRENDERER_TEXTURE2D_H__
#define __OPENGLRENDERER_TEXTURE2D_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{
	class OpenGLRenderer;
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
	*    Abstract OpenGL 2D texture interface
	*/
	class Texture2D : public Renderer::ITexture2D
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Texture2D();

		/**
		*  @brief
		*    Return the OpenGL texture
		*
		*  @return
		*    The OpenGL texture, can be zero if no resource is allocated, do not destroy the returned resource (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline unsigned int getOpenGLTexture() const;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] width
		*    The width of the texture
		*  @param[in] height
		*    The height of the texture
		*/
		Texture2D(OpenGLRenderer &openGLRenderer, unsigned int width, unsigned int height);


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		unsigned int mOpenGLTexture;	///< OpenGL texture, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
//OpenGLRenderer/Texture2D.inl
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline unsigned int Texture2D::getOpenGLTexture() const
	{
		return mOpenGLTexture;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_TEXTURE2D_H__
//OpenGLRenderer/Texture2DDsa.h
#ifndef __OPENGLRENDERER_TEXTURE2D_DSA_H__
#define __OPENGLRENDERER_TEXTURE2D_DSA_H__


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
	*    OpenGL 2D texture class, effective direct state access (DSA)
	*/
	class Texture2DDsa : public Texture2D
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] width
		*    Texture width, must be >1
		*  @param[in] height
		*    Texture height, must be >1
		*  @param[in] textureFormat
		*    Texture format
		*  @param[in] data
		*    Texture data, can be a null pointer
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*/
		Texture2DDsa(OpenGLRenderer &openGLRenderer, unsigned int width, unsigned int height, Renderer::TextureFormat::Enum textureFormat, void *data, unsigned int flags);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Texture2DDsa();


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_TEXTURE2D_DSA_H__
//OpenGLRenderer/Texture2DBind.h
#ifndef __OPENGLRENDERER_TEXTURE2D_BIND_H__
#define __OPENGLRENDERER_TEXTURE2D_BIND_H__


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
	*    OpenGL 2D texture class, traditional bind version
	*/
	class Texture2DBind : public Texture2D
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] width
		*    Texture width, must be >1
		*  @param[in] height
		*    Texture height, must be >1
		*  @param[in] textureFormat
		*    Texture format
		*  @param[in] data
		*    Texture data, can be a null pointer
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*/
		Texture2DBind(OpenGLRenderer &openGLRenderer, unsigned int width, unsigned int height, Renderer::TextureFormat::Enum textureFormat, void *data, unsigned int flags);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Texture2DBind();


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_TEXTURE2D_BIND_H__
//OpenGLRenderer/Texture2DArray.h
#ifndef __OPENGLRENDERER_TEXTURE2DARRAY_H__
#define __OPENGLRENDERER_TEXTURE2DARRAY_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{
	class OpenGLRenderer;
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
	*    Abstract OpenGL 2D array texture interface
	*/
	class Texture2DArray : public Renderer::ITexture2DArray
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Texture2DArray();

		/**
		*  @brief
		*    Return the OpenGL texture
		*
		*  @return
		*    The OpenGL texture, can be zero if no resource is allocated, do not destroy the returned resource (type "GLuint" not used in here in order to keep the header slim)
		*/
		inline unsigned int getOpenGLTexture() const;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] width
		*    The width of the texture
		*  @param[in] height
		*    The height of the texture
		*  @param[in] numberOfSlices
		*    The number of slices
		*/
		Texture2DArray(OpenGLRenderer &openGLRenderer, unsigned int width, unsigned int height, unsigned int numberOfSlices);


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		unsigned int mOpenGLTexture;	///< OpenGL texture, can be zero if no resource is allocated (type "GLuint" not used in here in order to keep the header slim)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
//OpenGLRenderer/Texture2DArray.inl
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline unsigned int Texture2DArray::getOpenGLTexture() const
	{
		return mOpenGLTexture;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_TEXTURE2DARRAY_H__
//OpenGLRenderer/Texture2DArrayDsa.h
#ifndef __OPENGLRENDERER_TEXTURE2DARRAY_DSA_H__
#define __OPENGLRENDERER_TEXTURE2DARRAY_DSA_H__


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
	*    OpenGL 2D array texture class, effective direct state access (DSA)
	*/
	class Texture2DArrayDsa : public Texture2DArray
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] width
		*    Texture width, must be >1
		*  @param[in] height
		*    Texture height, must be >1
		*  @param[in] numberOfSlices
		*    Number of slices, must be >1
		*  @param[in] textureFormat
		*    Texture format
		*  @param[in] data
		*    Texture data, can be a null pointer
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*/
		Texture2DArrayDsa(OpenGLRenderer &openGLRenderer, unsigned int width, unsigned int height, unsigned int numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data, unsigned int flags);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Texture2DArrayDsa();


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_TEXTURE2DARRAY_DSA_H__
//OpenGLRenderer/Texture2DArrayBind.h
#ifndef __OPENGLRENDERER_TEXTURE2DARRAY_BIND_H__
#define __OPENGLRENDERER_TEXTURE2DARRAY_BIND_H__


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
	*    OpenGL 2D array texture class, traditional bind version
	*/
	class Texture2DArrayBind : public Texture2DArray
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] width
		*    Texture width, must be >1
		*  @param[in] height
		*    Texture height, must be >1
		*  @param[in] numberOfSlices
		*    Number of slices, must be >1
		*  @param[in] textureFormat
		*    Texture format
		*  @param[in] data
		*    Texture data, can be a null pointer
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*/
		Texture2DArrayBind(OpenGLRenderer &openGLRenderer, unsigned int width, unsigned int height, unsigned int numberOfSlices, Renderer::TextureFormat::Enum textureFormat, void *data, unsigned int flags);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Texture2DArrayBind();


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERER_TEXTURE2DARRAY_BIND_H__


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __OPENGLRENDERE_SINGLE__
