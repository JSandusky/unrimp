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
#include "RendererRuntime/Resource/Font/FontResourceSerializer.h"
#include "RendererRuntime/Asset/AssetManager.h"
#include "RendererRuntime/RendererRuntimeImpl.h"

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
	// TODO(co) Work-in-progress
	IFont* FontResourceManager::loadFontByAssetId(AssetId assetId)
	{
		try
		{
			std::ifstream ifstream(mRendererRuntimeImpl.getAssetManager().getAssetFilenameByAssetId(assetId), std::ios::binary);
			return mFontResourceSerializer->loadFont(ifstream);
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load font asset %d: %s", assetId, e.what());
			return nullptr;
		}
	}

	void FontResourceManager::reloadFontByAssetId(AssetId assetId) const
	{
		// TODO(co) Implement me
		assetId = assetId;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	FontResourceManager::FontResourceManager(RendererRuntimeImpl& rendererRuntimeImpl) :
		mRendererRuntimeImpl(rendererRuntimeImpl),
		mFontResourceSerializer(new FontResourceSerializer(rendererRuntimeImpl))
	{
		// Nothing in here
	}

	FontResourceManager::~FontResourceManager()
	{
		delete mFontResourceSerializer;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
