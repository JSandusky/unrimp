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
#include "Renderer/Texture/TextureTypes.h"


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
	*    Capabilities class
	*
	*  @note
	*    - The data is public by intent in order to make it easier to use this class,
	*      no issues involved because the user only gets a constant instance
	*/
	class Capabilities
	{


	//[-------------------------------------------------------]
	//[ Public data                                           ]
	//[-------------------------------------------------------]
	public:
		TextureFormat::Enum preferredSwapChainColorTextureFormat;			///< Preferred swap chain color texture format
		TextureFormat::Enum preferredSwapChainDepthStencilTextureFormat;	///< Preferred swap chain depth stencil texture format
		uint32_t			maximumNumberOfViewports;						///< Maximum number of viewports (always at least 1)
		uint32_t			maximumNumberOfSimultaneousRenderTargets;		///< Maximum number of simultaneous render targets (if <1 render to texture is not supported)
		uint32_t			maximumTextureDimension;						///< Maximum texture dimension (usually 2048, 4096, 8192 or 16384)
		uint32_t			maximumNumberOf2DTextureArraySlices;			///< Maximum number of 2D texture array slices (usually 512 up to 8192, in case there's no support for 2D texture arrays it's 0)
		uint32_t			maximumUniformBufferSize;						///< Maximum uniform buffer (UBO) size in bytes (usually at least 4096 *16 bytes, in case there's no support for uniform buffer it's 0)
		uint32_t			maximumTextureBufferSize;						///< Maximum texture buffer (TBO) size in texel (>65536, typically much larger than that of one-dimensional texture, in case there's no support for texture buffer it's 0)
		uint32_t			maximumIndirectBufferSize;						///< Maximum indirect buffer size in bytes (in case there's no support for indirect buffer it's 0)
		uint8_t				maximumNumberOfMultisamples;					///< Maximum number of multisamples (always at least 1, usually 8)
		uint8_t				maximumAnisotropy;								///< Maximum anisotropy (always at least 1, usually 16)
		bool				individualUniforms;								///< Individual uniforms ("constants" in Direct3D terminology) supported? If not, only uniform buffer objects are supported.
		bool				instancedArrays;								///< Instanced arrays supported? (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
		bool				drawInstanced;									///< Draw instanced supported? (shader model 4 feature, build in shader variable holding the current instance ID)
		bool				baseVertex;										///< Base vertex supported for draw calls?
		bool				nativeMultiThreading;							///< Does the renderer support native multi-threading? For example Direct3D 11 does meaning we can also create renderer resources asynchronous while for OpenGL we have to create an separate OpenGL context (less efficient, more complex to implement).
		bool				shaderBytecode;									///< Shader bytecode supported?
		// Vertex-shader (VS) stage
		bool				vertexShader;									///< Is there support for vertex shaders (VS)?
		// Tessellation-control-shader (TCS) stage and tessellation-evaluation-shader (TES) stage
		uint32_t			maximumNumberOfPatchVertices;					///< Maximum number of vertices per patch (usually 0 for no tessellation support or 32 which is the maximum number of supported vertices per patch)
		// Geometry-shader (GS) stage
		uint32_t			maximumNumberOfGsOutputVertices;				///< Maximum number of vertices a geometry shader (GS) can emit (usually 0 for no geometry shader support or 1024)
		// Fragment-shader (FS) stage
		bool				fragmentShader;									///< Is there support for fragment shaders (FS)?


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		inline Capabilities();

		/**
		*  @brief
		*    Destructor
		*/
		inline ~Capabilities();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit Capabilities(const Capabilities& source) = delete;
		Capabilities& operator =(const Capabilities& source) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/Capabilities.inl"
