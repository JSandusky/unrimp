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
#include "RendererRuntime/Export.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Scene/CompositorResourcePassScene.h"


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
	/**
	*  @brief
	*    Compositor resource pass shadow map
	*/
	class CompositorResourcePassShadowMap : public CompositorResourcePassScene
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class CompositorPassFactory;	// The only one allowed to create instances of this class


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT static const CompositorPassTypeId TYPE_ID;
		static const uint32_t MAXIMUM_NUMBER_OF_SHADOW_CASCADES = 4;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline AssetId getTextureAssetId() const;
		inline uint32_t getShadowMapSize() const;
		inline uint8_t getNumberOfShadowCascades() const;
		inline uint8_t getNumberOfShadowMultisamples() const;
		inline float getCascadeSplitsLambda() const;
		inline AssetId getDepthToExponentialVarianceMaterialBlueprintAssetId() const;
		inline AssetId getBlurMaterialBlueprintAssetId() const;
		inline float getShadowFilterSize() const;
		inline bool getStabilizeCascades() const;


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::ICompositorResourcePass methods ]
	//[-------------------------------------------------------]
	public:
		inline virtual CompositorPassTypeId getTypeId() const override;
		virtual void deserialize(uint32_t numberOfBytes, const uint8_t* data) override;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline explicit CompositorResourcePassShadowMap(const CompositorTarget& compositorTarget);
		inline virtual ~CompositorResourcePassShadowMap();
		explicit CompositorResourcePassShadowMap(const CompositorResourcePassShadowMap&) = delete;
		CompositorResourcePassShadowMap& operator=(const CompositorResourcePassShadowMap&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		AssetId	 mTextureAssetId;										///< Shadow map texture asset ID
		uint32_t mShadowMapSize;										///< The shadow map size is usually 512, 1024 or 2048
		uint8_t  mNumberOfShadowCascades;								///< Number of shadow cascades, usually 4
		uint8_t  mNumberOfShadowMultisamples;							///< The number of shadow multisamples per pixel (valid values: 1, 2, 4, 8)
		float	 mCascadeSplitsLambda;									///< Cascade splits lambda
		AssetId  mDepthToExponentialVarianceMaterialBlueprintAssetId;	///< Depth to exponential variance material blueprint asset ID
		AssetId  mBlurMaterialBlueprintAssetId;							///< Blur material blueprint asset ID
		float	 mShadowFilterSize;										///< Shadow filter size
		bool	 mStabilizeCascades;									///< Keeps consistent sizes for each cascade, and snaps each cascade so that they move in texel-sized increments. Reduces temporal aliasing artifacts, but reduces the effective resolution of the cascades. See Valient, M., "Stable Rendering of Cascaded Shadow Maps", In: Engel, W. F ., et al., "ShaderX6: Advanced Rendering Techniques", Charles River Media, 2008, ISBN 1-58450-544-3.


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/CompositorNode/Pass/ShadowMap/CompositorResourcePassShadowMap.inl"
