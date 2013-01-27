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
#ifndef __RENDERER_RENDERER_TYPES_H__
#define __RENDERER_RENDERER_TYPES_H__


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
	*    Map types
	*
	*  @note
	*    - These constants directly map to Direct3D 10 & 11 constants, do not change them
	*/
	struct MapType
	{
		// TODO(co) Comments
		enum Enum
		{
			READ			   = 1,	/**<  */
			WRITE			   = 2,	/**<  */
			READ_WRITE		   = 3,	/**<  */
			WRITE_DISCARD	   = 4,	/**<  */
			WRITE_NO_OVERWRITE = 5	/**<  */
		};
	};

	/**
	*  @brief
	*    Map flags
	*
	*  @note
	*    - These constants directly map to Direct3D 11 constants, do not change them
	*/
	struct MapFlag
	{
		enum Enum
		{
			DO_NOT_WAIT = 0x100000L	/**< In case the resource is currently used when "Renderer::IRenderer::map()" is called, let the method return with an error, cannot be used with "Renderer::MapType::WRITE_DISCARD" or "Renderer::MapType::WRITE_NO_OVERWRITE" */
		};
	};

	/**
	*  @brief
	*    Clear flags
	*/
	struct ClearFlag
	{
		enum Enum
		{
			COLOR       = 1<<0,			/**< Clear color buffer */
			DEPTH       = 1<<1,			/**< Clear depth buffer */
			STENCIL     = 1<<2,			/**< Clear stencil buffer */
			COLOR_DEPTH = COLOR | DEPTH	/**< Clear color and depth buffer */
		};
	};

	/**
	*  @brief
	*    Primitive topology types
	*
	*  @note
	*    - These constants directly map to Direct3D 9 & 10 & 11 constants, do not change them
	*/
	struct PrimitiveTopology
	{
		enum Enum
		{
			UNKNOWN        = 0,		/**< Unknown primitive type */
			POINT_LIST     = 1,		/**< Point list, use "PATCH_LIST_1" for tessellation */
			LINE_LIST      = 2,		/**< Line list, use "PATCH_LIST_2" for tessellation */
			LINE_STRIP     = 3,		/**< Line strip */
			TRIANGLE_LIST  = 4,		/**< Triangle list, use "PATCH_LIST_3" for tessellation */
			TRIANGLE_STRIP = 5,		/**< Triangle strip */
			PATCH_LIST_1   = 33,	/**< Patch list with 1 vertex per patch (tessellation relevant topology type) - "POINT_LIST" used for tessellation */
			PATCH_LIST_2   = 34,	/**< Patch list with 2 vertices per patch (tessellation relevant topology type) - "LINE_LIST" used for tessellation */
			PATCH_LIST_3   = 35,	/**< Patch list with 3 vertices per patch (tessellation relevant topology type) - "TRIANGLE_LIST" used for tessellation */
			PATCH_LIST_4   = 36,	/**< Patch list with 4 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_5   = 37,	/**< Patch list with 5 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_6   = 38,	/**< Patch list with 6 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_7   = 39,	/**< Patch list with 7 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_8   = 40,	/**< Patch list with 8 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_9   = 41,	/**< Patch list with 9 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_10  = 42,	/**< Patch list with 10 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_11  = 43,	/**< Patch list with 11 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_12  = 44,	/**< Patch list with 12 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_13  = 45,	/**< Patch list with 13 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_14  = 46,	/**< Patch list with 14 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_15  = 47,	/**< Patch list with 15 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_16  = 48,	/**< Patch list with 16 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_17  = 49,	/**< Patch list with 17 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_18  = 50,	/**< Patch list with 18 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_19  = 51,	/**< Patch list with 19 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_20  = 52,	/**< Patch list with 20 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_21  = 53,	/**< Patch list with 21 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_22  = 54,	/**< Patch list with 22 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_23  = 55,	/**< Patch list with 23 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_24  = 56,	/**< Patch list with 24 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_25  = 57,	/**< Patch list with 25 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_26  = 58,	/**< Patch list with 26 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_27  = 59,	/**< Patch list with 27 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_28  = 60,	/**< Patch list with 28 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_29  = 61,	/**< Patch list with 29 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_30  = 62,	/**< Patch list with 30 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_31  = 63,	/**< Patch list with 31 vertices per patch (tessellation relevant topology type) */
			PATCH_LIST_32  = 64		/**< Patch list with 32 vertices per patch (tessellation relevant topology type) */
		};
	};

	/**
	*  @brief
	*    Comparison function
	*
	*  @note
	*    - Original Direct3D comments from http://msdn.microsoft.com/en-us/library/windows/desktop/bb204902%28v=vs.85%29.aspx are used in here
	*    - These constants directly map to Direct3D 10 & 11 constants, do not change them
	*/
	struct ComparisonFunc
	{
		enum Enum
		{
			NEVER		  = 1,	/**< Never pass the comparison */
			LESS		  = 2,	/**< If the source data is less than the destination data, the comparison passes */
			EQUAL		  = 3,	/**< If the source data is equal to the destination data, the comparison passes */
			LESS_EQUAL	  = 4,	/**< If the source data is less than or equal to the destination data, the comparison passes */
			GREATER		  = 5,	/**< If the source data is greater than the destination data, the comparison passes */
			NOT_EQUAL	  = 6,	/**< If the source data is not equal to the destination data, the comparison passes */
			GREATER_EQUAL = 7,	/**< If the source data is greater than or equal to the destination data, the comparison passes */
			ALWAYS		  = 8	/**< Always pass the comparison */
		};
	};

	/**
	*  @brief
	*    Color write enable flags
	*
	*  @note
	*    - These constants directly map to Direct3D 10 & 11 constants, do not change them
	*/
	// TODO(co) Renderer::ColorWriteEnable, document
	// TODO(co) A flags-class would be nice to avoid invalid flags
	struct ColorWriteEnable
	{
		enum Enum
		{
			RED   = 1,
			GREEN = 2,
			BLUE  = 4,
			ALPHA = 8,
			ALL   = (((RED | GREEN) | BLUE) | ALPHA)
		};
	};


	//[-------------------------------------------------------]
	//[ Types                                                 ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Mapped subresource
	*
	*  @note
	*    - This structure directly maps to Direct3D 11, do not change it
	*/
	// TODO(co) Comments
	struct MappedSubresource
	{
		void		 *data;
		unsigned int  rowPitch;
		unsigned int  depthPitch;
	};

	/**
	*  @brief
	*    Viewport
	*
	*  @note
	*    - This structure directly maps to Direct3D 11, do not change it
	*/
	struct Viewport
	{
		float topLeftX;	/**< Top left x start position */
		float topLeftY;	/**< Top left y start position */
		float width;	/**< Viewport width */
		float height;	/**< Viewport height */
		float minDepth;	/**< Minimum depth value, usually 0.0f, between [0, 1] */
		float maxDepth;	/**< Maximum depth value, usually 1.0f, between [0, 1] */
	};

	/**
	*  @brief
	*    Scissor rectangle
	*
	*  @note
	*    - This structure directly maps to Direct3D 9 & 10 & 11, do not change it
	*/
	struct ScissorRectangle
	{
		long topLeftX;		/**< Top left x-coordinate of the scissor rectangle */
		long topLeftY;		/**< Top left y-coordinate of the scissor rectangle */
		long bottomRightX;	/**< Bottom right x-coordinate of the scissor rectangle */
		long bottomRightY;	/**< Bottom right y-coordinate of the scissor rectangle */
	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_RENDERER_TYPES_H__
