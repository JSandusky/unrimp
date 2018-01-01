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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline AssetId CompositorResourcePassShadowMap::getTextureAssetId() const
	{
		return mTextureAssetId;
	}

	inline uint32_t CompositorResourcePassShadowMap::getShadowMapSize() const
	{
		return mShadowMapSize;
	}

	inline uint8_t CompositorResourcePassShadowMap::getNumberOfShadowCascades() const
	{
		return mNumberOfShadowCascades;
	}

	inline uint8_t CompositorResourcePassShadowMap::getNumberOfShadowMultisamples() const
	{
		return mNumberOfShadowMultisamples;
	}

	inline float CompositorResourcePassShadowMap::getCascadeSplitsLambda() const
	{
		return mCascadeSplitsLambda;
	}

	inline AssetId CompositorResourcePassShadowMap::getDepthToExponentialVarianceMaterialBlueprintAssetId() const
	{
		return mDepthToExponentialVarianceMaterialBlueprintAssetId;
	}

	inline AssetId CompositorResourcePassShadowMap::getBlurMaterialBlueprintAssetId() const
	{
		return mBlurMaterialBlueprintAssetId;
	}

	inline float CompositorResourcePassShadowMap::getShadowFilterSize() const
	{
		return mShadowFilterSize;
	}

	inline bool CompositorResourcePassShadowMap::getStabilizeCascades() const
	{
		return mStabilizeCascades;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::ICompositorResourcePass methods ]
	//[-------------------------------------------------------]
	inline CompositorPassTypeId CompositorResourcePassShadowMap::getTypeId() const
	{
		return TYPE_ID;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline CompositorResourcePassShadowMap::CompositorResourcePassShadowMap(const CompositorTarget& compositorTarget) :
		CompositorResourcePassScene(compositorTarget),
		mShadowMapSize(1024),
		mNumberOfShadowCascades(4),
		mNumberOfShadowMultisamples(4),
		mCascadeSplitsLambda(0.99f),
		mShadowFilterSize(8.0f),
		mStabilizeCascades(true)
	{
		// Nothing here
	}

	inline CompositorResourcePassShadowMap::~CompositorResourcePassShadowMap()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
