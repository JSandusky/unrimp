/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "RendererRuntime/Core/Manager.h"
#include "RendererRuntime/Core/StringId.h"
#include "RendererRuntime/Core/Renderer/RenderTargetTextureSignature.h"

#include <vector>
#include <unordered_map>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IRendererRuntime;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId AssetId;	///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>"


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class RenderTargetTextureManager : private Manager
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		struct RenderTargetTextureElement
		{
			RenderTargetTextureSignature renderTargetTextureSignature;
			Renderer::ITexture*			 texture;				///< Can be a null pointer, no "Renderer::ITexturePtr" to not have overhead when internally reallocating
			uint32_t					 numberOfReferences;	///< Number of texture references (don't misuse the renderer texture reference counter for this)

			inline RenderTargetTextureElement() :
				texture(nullptr),
				numberOfReferences(0)
			{
				// Nothing here
			}

			inline explicit RenderTargetTextureElement(const RenderTargetTextureSignature& _renderTargetTextureSignature) :
				renderTargetTextureSignature(_renderTargetTextureSignature),
				texture(nullptr),
				numberOfReferences(0)
			{
				// Nothing here
			}

			inline RenderTargetTextureElement(const RenderTargetTextureSignature& _renderTargetTextureSignature, Renderer::ITexture& _texture) :
				renderTargetTextureSignature(_renderTargetTextureSignature),
				texture(&_texture),
				numberOfReferences(0)
			{
				// Nothing here
			}
		};


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline explicit RenderTargetTextureManager(IRendererRuntime& rendererRuntime);
		inline ~RenderTargetTextureManager();
		RenderTargetTextureManager(const RenderTargetTextureManager&) = delete;
		RenderTargetTextureManager& operator=(const RenderTargetTextureManager&) = delete;
		inline IRendererRuntime& getRendererRuntime() const;
		void clear();
		void clearRendererResources();
		void addRenderTargetTexture(AssetId assetId, const RenderTargetTextureSignature& renderTargetTextureSignature);
		Renderer::ITexture* getTextureByAssetId(AssetId assetId, const Renderer::IRenderTarget& renderTarget, float resolutionScale);
		void releaseRenderTargetTextureBySignature(const RenderTargetTextureSignature& renderTargetTextureSignature);


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::vector<RenderTargetTextureElement> SortedRenderTargetTextureVector;
		typedef std::unordered_map<uint32_t, RenderTargetTextureSignatureId> AssetIdToRenderTargetTextureSignatureId;	///< Key = "RendererRuntime::AssetId"
		typedef std::unordered_map<uint32_t, uint32_t> AssetIdToIndex;	///< Key = "RendererRuntime::AssetId"


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&						mRendererRuntime;
		SortedRenderTargetTextureVector			mSortedRenderTargetTextureVector;
		AssetIdToRenderTargetTextureSignatureId	mAssetIdToRenderTargetTextureSignatureId;
		AssetIdToIndex							mAssetIdToIndex;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/Renderer/RenderTargetTextureManager.inl"
