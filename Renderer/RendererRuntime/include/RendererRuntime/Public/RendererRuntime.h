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


// TODO(co) Rethink the header in here

// Public comfort header putting everything within a single header


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Asset/Asset.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class AssetManager;
	class IRendererRuntime;
	class ResourceStreamer;
	class FontResourceManager;
	class MeshResourceManager;
	class SceneResourceManager;
	class ShaderResourceManager;
	class TextureResourceManager;
	class SkeletonResourceManager;
	class MaterialResourceManager;
	class CompositorResourceManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Types                                                 ]
	//[-------------------------------------------------------]
	// RendererRuntime/IRendererRuntime.h
	class IRendererRuntime : public Renderer::RefCount<IRendererRuntime>
	{
	public:
		virtual ~IRendererRuntime();
		inline Renderer::IRenderer& getRenderer() const
		{
			return *mRenderer;
		}
		inline AssetManager& getAssetManager() const
		{
			return *mAssetManager;
		}
		inline ResourceStreamer& getResourceStreamer() const
		{
			return *mResourceStreamer;
		}
		inline TextureResourceManager& getTextureResourceManager() const
		{
			return *mTextureResourceManager;
		}
		inline ShaderResourceManager& getShaderResourceManager() const
		{
			return *mShaderResourceManager;
		}
		inline MaterialResourceManager& getMaterialResourceManager() const
		{
			return *mMaterialResourceManager;
		}
		inline FontResourceManager& getFontResourceManager() const
		{
			return *mFontResourceManager;
		}
		inline SkeletonResourceManager& getSkeletonResourceManager() const
		{
			return *mSkeletonResourceManager;
		}
		inline MeshResourceManager& getMeshResourceManager() const
		{
			return *mMeshResourceManager;
		}
		inline SceneResourceManager& getSceneResourceManager() const
		{
			return *mSceneResourceManager;
		}
		inline CompositorResourceManager& getCompositorResourceManager() const
		{
			return *mCompositorResourceManager;
		}
	public:
		virtual void reloadResourceByAssetId(AssetId assetId) const = 0;
		virtual void update() const = 0;
	protected:
		IRendererRuntime();
		explicit IRendererRuntime(const IRendererRuntime &source);
		IRendererRuntime &operator =(const IRendererRuntime &source);
	private:
		Renderer::IRenderer*		mRenderer;
		AssetManager*				mAssetManager;
		ResourceStreamer*			mResourceStreamer;
		TextureResourceManager*		mTextureResourceManager;
		ShaderResourceManager*		mShaderResourceManager;
		MaterialResourceManager*	mMaterialResourceManager;
		FontResourceManager*		mFontResourceManager;
		SkeletonResourceManager*	mSkeletonResourceManager;
		MeshResourceManager*		mMeshResourceManager;
		SceneResourceManager*		mSceneResourceManager;
		CompositorResourceManager*	mCompositorResourceManager;
	};
	typedef Renderer::SmartRefCount<IRendererRuntime> IRendererRuntimePtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
