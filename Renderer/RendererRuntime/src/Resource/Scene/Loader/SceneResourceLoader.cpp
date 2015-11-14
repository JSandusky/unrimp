/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "RendererRuntime/Resource/Scene/Loader/SceneResourceLoader.h"
#include "RendererRuntime/Resource/Scene/SceneResource.h"

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId SceneResourceLoader::TYPE_ID("scene");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	ResourceLoaderTypeId SceneResourceLoader::getResourceLoaderTypeId() const
	{
		return TYPE_ID;
	}

	void SceneResourceLoader::onDeserialization()
	{
		// TODO(co) Error handling
		try
		{
			std::ifstream ifstream(mAsset.assetFilename, std::ios::binary);

			// Read in the scene header
			#pragma pack(push)
			#pragma pack(1)
				struct SceneHeader
				{
					uint32_t formatType;
					uint16_t formatVersion;
				};
			#pragma pack(pop)
			SceneHeader sceneHeader;
			ifstream.read(reinterpret_cast<char*>(&sceneHeader), sizeof(SceneHeader));

			// TODO(co)
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load scene asset %d: %s", mAsset.assetId, e.what());
		}
	}

	void SceneResourceLoader::onProcessing()
	{
		// Nothing here
	}

	void SceneResourceLoader::onRendererBackendDispatch()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	SceneResourceLoader::~SceneResourceLoader()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
