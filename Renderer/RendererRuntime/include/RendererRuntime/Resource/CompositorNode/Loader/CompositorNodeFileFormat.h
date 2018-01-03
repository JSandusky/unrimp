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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/StringId.h"
#include "RendererRuntime/Core/Renderer/FramebufferSignature.h"
#include "RendererRuntime/Core/Renderer/RenderTargetTextureSignature.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId AssetId;					///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>"
	typedef StringId CompositorChannelId;		///< Compositor channel identifier, internally just a POD "uint32_t"
	typedef StringId CompositorFramebufferId;	///< Compositor framebuffer identifier, internally just a POD "uint32_t"
	typedef StringId CompositorPassTypeId;		///< Compositor pass type identifier, internally just a POD "uint32_t"
	typedef StringId MaterialTechniqueId;		///< Material technique identifier, internally just a POD "uint32_t", result of hashing the material technique name


	// Compositor node file format content:
	// - File format header
	// - Compositor node header
	namespace v1CompositorNode
	{


		//[-------------------------------------------------------]
		//[ Definitions                                           ]
		//[-------------------------------------------------------]
		static const uint32_t FORMAT_TYPE	 = StringId("CompositorNode");
		static const uint32_t FORMAT_VERSION = 8;

		#pragma pack(push)
		#pragma pack(1)
			struct CompositorNodeHeader
			{
				uint32_t numberOfInputChannels;
				uint32_t numberOfRenderTargetTextures;
				uint32_t numberOfFramebuffers;
				uint32_t numberOfTargets;
				uint32_t numberOfOutputChannels;
			};

			struct Channel
			{
				CompositorChannelId id;
			};

			struct RenderTargetTexture
			{
				AssetId						 assetId;
				RenderTargetTextureSignature renderTargetTextureSignature;
			};

			struct Framebuffer
			{
				CompositorFramebufferId compositorFramebufferId;
				FramebufferSignature	framebufferSignature;
			};

			struct Target
			{
				CompositorChannelId		compositorChannelId;
				CompositorFramebufferId compositorFramebufferId;
				uint32_t				numberOfPasses;
			};

			struct PassHeader
			{
				CompositorPassTypeId compositorPassTypeId;
				uint32_t			 numberOfBytes;
			};

			// Keep this in sync with "RendererRuntime::ICompositorResourcePass::deserialize() -> PassData"
			struct Pass
			{
				float	 minimumDepth		= 0.0f;
				float	 maximumDepth		= 1.0f;
				uint32_t numberOfExecutions	= RendererRuntime::getUninitialized<uint32_t>();
				bool	 skipFirstExecution	= false;
			};

			struct PassClear : public Pass
			{
				uint32_t flags	  = 0;		///< Combination of "Renderer::ClearFlag"
				float	 color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				float	 z		  = 0.0f;	///< 0 instead of 1 due to usage of Reversed-Z (see e.g. https://developer.nvidia.com/content/depth-precision-visualized and https://nlguillemot.wordpress.com/2016/12/07/reversed-z-in-opengl/)
				uint32_t stencil  = 0;
			};

			struct PassVrHiddenAreaMesh : public Pass
			{
				uint32_t flags	 = 0;	///< Combination of "Renderer::ClearFlag", except for color-flag
				uint32_t stencil = 0;
			};

			struct PassScene : public Pass
			{
				uint8_t				minimumRenderQueueIndex = 0;	///< Inclusive
				uint8_t				maximumRenderQueueIndex = 255;	///< Inclusive
				bool				transparentPass			= false;
				MaterialTechniqueId	materialTechniqueId;
			};

			struct PassShadowMap : public PassScene
			{
				AssetId textureAssetId;
				AssetId depthToExponentialVarianceMaterialBlueprintAssetId;
				AssetId blurMaterialBlueprintAssetId;
			};

			struct PassResolveMultisample : public Pass
			{
				CompositorFramebufferId sourceMultisampleCompositorFramebufferId;
			};

			struct PassCopy : public Pass
			{
				AssetId destinationTextureAssetId;
				AssetId sourceTextureAssetId;
			};

			struct PassQuad : public Pass
			{
				AssetId				materialAssetId;				///< If material blueprint asset ID is set, material asset ID must be uninitialized
				MaterialTechniqueId	materialTechniqueId;			///< Must always be valid
				AssetId				materialBlueprintAssetId;		///< If material asset ID is set, material blueprint asset ID must be uninitialized
				uint32_t			numberOfMaterialProperties = 0;
			};

			// The material definition is not mandatory for the debug GUI, if nothing is defined the fixed build in renderer configuration resources will be used instead
			struct PassDebugGui : public PassQuad
			{
			};
		#pragma pack(pop)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
	} // v1CompositorNode
} // RendererRuntime
