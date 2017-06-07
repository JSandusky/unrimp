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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/GetUninitialized.h"

#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline MaterialBlueprintResourceListener::MaterialBlueprintResourceListener() :
		mIdentityColorCorrectionLookupTable3D(getUninitialized<TextureResourceId>()),
		mSsaoSampleKernelTextureResourceId(getUninitialized<TextureResourceId>()),
		mSsaoNoiseTexture4x4ResourceId(getUninitialized<TextureResourceId>()),
		mRendererRuntime(nullptr),
		mPassData(nullptr),
		mCompositorContextData(nullptr),
		mRenderTargetWidth(1),
		mRenderTargetHeight(1),
		mNearZ(0.0f),
		mFarZ(0.0f),
		mPreviousNumberOfRenderedFrames(getUninitialized<uint64_t>()),
		mHosekWilkieSky(nullptr),
		mObjectSpaceToWorldSpaceTransform(nullptr),
		mMaterialTechnique(nullptr)
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline const PassBufferManager::PassData& MaterialBlueprintResourceListener::getPassData() const
	{
		assert(nullptr != mPassData);
		return *mPassData;
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::IMaterialBlueprintResourceListener methods ]
	//[-------------------------------------------------------]
	inline void MaterialBlueprintResourceListener::beginFillUnknown()
	{
		// Nothing here
	}

	inline bool MaterialBlueprintResourceListener::fillUnknownValue(uint32_t, uint8_t*, uint32_t)
	{
		// Nothing here

		// Value not filled
		return false;
	}

	inline void MaterialBlueprintResourceListener::beginFillMaterial()
	{
		// Nothing here
	}

	inline bool MaterialBlueprintResourceListener::fillMaterialValue(uint32_t, uint8_t*, uint32_t)
	{
		// Nothing here

		// Value not filled
		return false;
	}

	inline void MaterialBlueprintResourceListener::beginFillInstance(const PassBufferManager::PassData& passData, const Transform& objectSpaceToWorldSpaceTransform, MaterialTechnique& materialTechnique)
	{
		// Remember the pass data memory address of the current scope
		// TODO(co) Rethink the evil const-cast on here
		mPassData = &const_cast<PassBufferManager::PassData&>(passData);

		// Remember the instance data of the current scope
		mObjectSpaceToWorldSpaceTransform = &objectSpaceToWorldSpaceTransform;
		mMaterialTechnique				  = &materialTechnique;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
