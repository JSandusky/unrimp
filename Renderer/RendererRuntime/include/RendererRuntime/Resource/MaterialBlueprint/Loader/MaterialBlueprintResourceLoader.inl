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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/GetUninitialized.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	inline ResourceLoaderTypeId MaterialBlueprintResourceLoader::getResourceLoaderTypeId() const
	{
		return TYPE_ID;
	}

	inline bool MaterialBlueprintResourceLoader::hasDeserialization() const
	{
		return true;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline MaterialBlueprintResourceLoader::MaterialBlueprintResourceLoader(IResourceManager& resourceManager, IRendererRuntime& rendererRuntime) :
		IResourceLoader(resourceManager),
		mRendererRuntime(rendererRuntime),
		mMaterialBlueprintResource(nullptr),
		mMaximumNumberOfRootParameters(0),
		mMaximumNumberOfDescriptorRanges(0),
		mVertexAttributesAssetId(getUninitialized<AssetId>()),
		mMaximumNumberOfMaterialBlueprintSamplerStates(0),
		mMaterialBlueprintSamplerStates(nullptr),
		mMaximumNumberOfMaterialBlueprintTextures(0),
		mMaterialBlueprintTextures(nullptr)
	{
		memset(mShaderBlueprintAssetId, static_cast<int>(getUninitialized<AssetId>()), sizeof(AssetId) * NUMBER_OF_SHADER_TYPES);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
