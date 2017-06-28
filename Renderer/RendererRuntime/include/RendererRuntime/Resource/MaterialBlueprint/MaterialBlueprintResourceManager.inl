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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline IRendererRuntime& MaterialBlueprintResourceManager::getRendererRuntime() const
	{
		return mRendererRuntime;
	}

	inline IMaterialBlueprintResourceListener& MaterialBlueprintResourceManager::getMaterialBlueprintResourceListener() const
	{
		// We know this pointer must always be valid
		assert(nullptr != mMaterialBlueprintResourceListener);
		return *mMaterialBlueprintResourceListener;
	}

	inline MaterialProperties& MaterialBlueprintResourceManager::getGlobalMaterialProperties()
	{
		return mGlobalMaterialProperties;
	}

	inline const MaterialProperties& MaterialBlueprintResourceManager::getGlobalMaterialProperties() const
	{
		return mGlobalMaterialProperties;
	}

	inline Renderer::FilterMode MaterialBlueprintResourceManager::getDefaultTextureFilterMode() const
	{
		return mDefaultTextureFilterMode;
	}

	inline uint8_t MaterialBlueprintResourceManager::getDefaultMaximumTextureAnisotropy() const
	{
		return mDefaultMaximumTextureAnisotropy;
	}

	inline InstanceBufferManager& MaterialBlueprintResourceManager::getInstanceBufferManager() const
	{
		// We know this pointer must always be valid
		assert(nullptr != mInstanceBufferManager);
		return *mInstanceBufferManager;
	}

	inline IndirectBufferManager& MaterialBlueprintResourceManager::getIndirectBufferManager() const
	{
		// We know this pointer must always be valid
		assert(nullptr != mIndirectBufferManager);
		return *mIndirectBufferManager;
	}

	inline LightBufferManager& MaterialBlueprintResourceManager::getLightBufferManager() const
	{
		// We know this pointer must always be valid
		assert(nullptr != mLightBufferManager);
		return *mLightBufferManager;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
