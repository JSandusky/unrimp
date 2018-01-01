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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ShadowMap/CompositorResourcePassShadowMap.h"
#include "RendererRuntime/Resource/CompositorNode/Loader/CompositorNodeFileFormat.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const CompositorPassTypeId CompositorResourcePassShadowMap::TYPE_ID("ShadowMap");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::ICompositorResourcePass methods ]
	//[-------------------------------------------------------]
	void CompositorResourcePassShadowMap::deserialize(uint32_t numberOfBytes, const uint8_t* data)
	{
		// Sanity check
		assert(sizeof(v1CompositorNode::PassShadowMap) == numberOfBytes);
		std::ignore = numberOfBytes;

		// Call the base implementation
		CompositorResourcePassScene::deserialize(sizeof(v1CompositorNode::PassScene), data);

		// Read data
		const v1CompositorNode::PassShadowMap* passShadowMap = reinterpret_cast<const v1CompositorNode::PassShadowMap*>(data);
		mTextureAssetId = passShadowMap->textureAssetId;
		mDepthToExponentialVarianceMaterialBlueprintAssetId = passShadowMap->depthToExponentialVarianceMaterialBlueprintAssetId;
		mBlurMaterialBlueprintAssetId = passShadowMap->blurMaterialBlueprintAssetId;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
