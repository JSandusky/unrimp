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
#ifndef __RENDERER_TEXTURE_TYPES_H__
#define __RENDERER_TEXTURE_TYPES_H__


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Definitions                                           ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Texture format
	*/
	struct TextureFormat
	{
		enum Enum
		{
			A8            = 0,	/**< 8-bit pixel format, all bits alpha */
			R8G8B8        = 1,	/**< 24-bit pixel format, 8 bits for red, green and blue */
			R8G8B8A8      = 2,	/**< 32-bit pixel format, 8 bits for red, green, blue and alpha */
			R16G16B16A16F = 3,	/**< 64-bit float format using 16 bits for the each channel (red, green, blue, alpha) */
			R32G32B32A32F = 4	/**< 128-bit float format using 32 bits for the each channel (red, green, blue, alpha) */
		};
	};

	/**
	*  @brief
	*    Texture flags
	*/
	struct TextureFlag
	{
		enum Enum
		{
			MIPMAPS       = 1<<0,	/**< Use/create mipmaps */
			RENDER_TARGET = 1<<1	/**< This texture can be used as render target */
		};
	};

	/**
	*  @brief
	*    Texture usage indication
	*
	*  @note
	*    - Only relevant for Direct3D, OpenGL has no texture usage indication
	*    - Original Direct3D comments from http://msdn.microsoft.com/en-us/library/windows/desktop/ff476259%28v=vs.85%29.aspx are used in here
	*    - These constants directly map to Direct3D 10 & 11 constants, do not change them
	*/
	struct TextureUsage
	{
		enum Enum
		{
			DEFAULT   = 0,	/**< A resource that requires read and write access by the GPU. This is likely to be the most common usage choice. */
			IMMUTABLE = 1,	/**< A resource that can only be read by the GPU. It cannot be written by the GPU, and cannot be accessed at all by the CPU. This type of resource must be initialized when it is created, since it cannot be changed after creation. */
			DYNAMIC   = 2,	/**< A resource that is accessible by both the GPU (read only) and the CPU (write only). A dynamic resource is a good choice for a resource that will be updated by the CPU at least once per frame. To update a dynamic resource, use a map method. */
			STAGING   = 3	/**< A resource that supports data transfer (copy) from the GPU to the CPU. */
		};
	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_TEXTURE_TYPES_H__
