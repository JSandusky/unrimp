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
#include "RendererRuntime/Resource/Font/FontResourceManager.h"
#include "RendererRuntime/Resource/Font/FontResourceLoader.h"
#include "RendererRuntime/Resource/Font/FontResource.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/Backend/RendererRuntimeImpl.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	// TODO(co) Work-in-progress
	FontResource* FontResourceManager::loadFontByAssetId(AssetId assetId)
	{
		const Asset* asset = mRendererRuntimeImpl.getAssetManager().getAssetByAssetId(assetId);
		if (nullptr != asset)
		{
			FontResource* fontResource = new FontResource(mRendererRuntimeImpl, assetId);

			mFontResourceLoader->initialize(*asset, *fontResource);
			mFontResourceLoader->onDeserialization();
			mFontResourceLoader->onProcessing();
			mFontResourceLoader->onRendererBackendDispatch();

			return fontResource;
		}

		// Error!
		return nullptr;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::ResourceManager methods ]
	//[-------------------------------------------------------]
	void FontResourceManager::reloadResourceByAssetId(AssetId assetId) const
	{
		// TODO(co) Implement me
		assetId = assetId;
	}

	void FontResourceManager::update()
	{
		// TODO(co) Implement me
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	FontResourceManager::FontResourceManager(RendererRuntimeImpl& rendererRuntimeImpl) :
		mRendererRuntimeImpl(rendererRuntimeImpl),
		mFontResourceLoader(new FontResourceLoader(rendererRuntimeImpl))
	{
		// Nothing in here
	}

	FontResourceManager::~FontResourceManager()
	{
		delete mFontResourceLoader;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
