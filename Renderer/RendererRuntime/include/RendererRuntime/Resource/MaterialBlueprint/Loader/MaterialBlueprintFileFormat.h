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
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	// -> Material blueprint file format content:
	//    - Material blueprint header
	//    - Material blueprint properties
	//    - Visual importance of shader properties
	//    - Root signature
	//    - Pipeline state object (PSO)
	//      - Shader blueprints, rasterization state etc.
	//    - Resources
	//      - Uniform buffers
	//      - Sampler states
	//      - Textures
	namespace v1MaterialBlueprint
	{


		//[-------------------------------------------------------]
		//[ Definitions                                           ]
		//[-------------------------------------------------------]
		static const uint32_t FORMAT_TYPE	 = StringId("MaterialBlueprint");
		static const uint32_t FORMAT_VERSION = 1;

		#pragma pack(push)
		#pragma pack(1)
			struct Header
			{
				uint32_t formatType;
				uint32_t formatVersion;
				uint32_t numberOfProperties;
				uint32_t numberOfShaderCombinationProperties;
				uint32_t numberOfIntegerShaderCombinationProperties;
				uint32_t numberOfUniformBuffers;
				uint32_t numberOfTextureBuffers;
				uint32_t numberOfSamplerStates;
				uint32_t numberOfTextures;
			};

			struct RootSignatureHeader
			{
				uint32_t numberOfRootParameters;
				uint32_t numberOfDescriptorRanges;
				uint32_t numberOfStaticSamplers;
				uint32_t flags;
			};

			struct UniformBufferHeader
			{
				uint32_t							   rootParameterIndex;
				MaterialBlueprintResource::BufferUsage bufferUsage;
				uint32_t							   numberOfElements;
				uint32_t							   numberOfElementProperties;
				uint32_t							   uniformBufferNumberOfBytes;	///< Includes handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
			};

			struct TextureBufferHeader
			{
				MaterialPropertyValue				   materialPropertyValue;
				uint32_t							   rootParameterIndex;
				MaterialBlueprintResource::BufferUsage bufferUsage;

				TextureBufferHeader() :
					materialPropertyValue(MaterialPropertyValue::fromUnknown()),
					rootParameterIndex(getUninitialized<uint32_t>()),
					bufferUsage(MaterialBlueprintResource::BufferUsage::UNKNOWN)
				{
					// Nothing here
				}
			};

			struct SamplerState : public Renderer::SamplerState
			{
				uint32_t rootParameterIndex;
			};

			struct Texture
			{
				uint32_t		 rootParameterIndex;
				MaterialProperty materialProperty;
				AssetId			 fallbackTextureAssetId;
				bool			 rgbHardwareGammaCorrection;

				Texture() :
					rootParameterIndex(getUninitialized<uint32_t>()),
					rgbHardwareGammaCorrection(false)
				{
					// Nothing here
				}

				Texture(uint32_t _rootParameterIndex, MaterialProperty _materialProperty, AssetId _fallbackTextureAssetId, bool _rgbHardwareGammaCorrection) :
					rootParameterIndex(_rootParameterIndex),
					materialProperty(_materialProperty),
					fallbackTextureAssetId(_fallbackTextureAssetId),
					rgbHardwareGammaCorrection(_rgbHardwareGammaCorrection)
				{
					// Nothing here
				}
			};
		#pragma pack(pop)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
	} // v1Material
} // RendererRuntime
