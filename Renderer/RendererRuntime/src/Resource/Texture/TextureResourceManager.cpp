/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/Resource/Texture/CrnTextureResourceSerializer.h"
#include "RendererRuntime/Resource/Texture/EtcTextureResourceSerializer.h"
#include "RendererRuntime/Resource/Texture/DdsTextureResourceSerializer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/IRendererRuntime.h"

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4548)	// warning C4548: expression before comma has no effect; expected expression with side-effect
	#include <fstream>
#pragma warning(pop)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Renderer::ITexture* TextureResourceManager::loadTextureByAssetId(AssetId assetId)
	{
		try
		{
			// TODO(co) Just an experiment
			std::ifstream ifstream(mRendererRuntime.getAssetManager().getAssetFilenameByAssetId(assetId), std::ios::binary);
		//	return mEtcTextureResourceSerializer->loadEtcTexture(ifstream);
			return mCrnTextureResourceSerializer->loadCrnTexture(ifstream);
		//	return mDdsTextureResourceSerializer->loadDdsTexture(ifstream);
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load texture asset %d: %s", assetId, e.what());
			return nullptr;
		}
	}

	void TextureResourceManager::reloadTextureByAssetId(AssetId assetId) const
	{
		// TODO(co) Implement me
		assetId = assetId;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	TextureResourceManager::TextureResourceManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mCrnTextureResourceSerializer(new CrnTextureResourceSerializer(rendererRuntime)),
		mEtcTextureResourceSerializer(new EtcTextureResourceSerializer(rendererRuntime)),
		mDdsTextureResourceSerializer(new DdsTextureResourceSerializer(rendererRuntime))
	{
		// Nothing in here
	}

	TextureResourceManager::~TextureResourceManager()
	{
		delete mCrnTextureResourceSerializer;
		delete mEtcTextureResourceSerializer;
		delete mDdsTextureResourceSerializer;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
