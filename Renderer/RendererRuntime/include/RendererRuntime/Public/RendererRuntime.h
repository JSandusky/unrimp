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


// TODO(co) Rethink the header in here

// Public comfort header putting everything within a single header


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __RENDERERRUNTIME_H__
#define __RENDERERRUNTIME_H__


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
	class IFont;
	class AssetManager;
	class SceneManager;
	class IRendererRuntime;
	class CompositorManager;
	class FontResourceManager;
	class MeshResourceManager;
	class ShaderResourceManager;
	class TextureResourceManager;
	class MaterialResourceManager;
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
		inline CompositorManager& getCompositorManager() const
		{
			return *mCompositorManager;
		}
		inline SceneManager& getSceneManager() const
		{
			return *mSceneManager;
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
		inline MeshResourceManager& getMeshResourceManager() const
		{
			return *mMeshResourceManager;
		}
	public:
		virtual void reloadResourceByAssetId(AssetId assetId) const = 0;
		virtual void update() const = 0;
	protected:
		IRendererRuntime();
		explicit IRendererRuntime(const IRendererRuntime &source);
		IRendererRuntime &operator =(const IRendererRuntime &source);
	private:
		Renderer::IRenderer*	 mRenderer;
		AssetManager*			 mAssetManager;
		CompositorManager*		 mCompositorManager;
		SceneManager*			 mSceneManager;
		TextureResourceManager*	 mTextureResourceManager;
		ShaderResourceManager*   mShaderResourceManager;
		MaterialResourceManager* mMaterialResourceManager;
		FontResourceManager*	 mFontResourceManager;
		MeshResourceManager*	 mMeshResourceManager;
	};
	typedef Renderer::SmartRefCount<IRendererRuntime> IRendererRuntimePtr;

	// RendererRuntime/IFont.h
	class IFont : public Renderer::RefCount<IFont>
	{
	public:
		enum EDrawFlags
		{
			CENTER_TEXT    = 1<<0,
			UNDERLINE_TEXT = 1<<1,
			CROSSOUT_TEXT  = 1<<2,
			MIPMAPPING     = 1<<3
		};
	public:
		virtual ~IFont();
		inline uint32_t getSize() const
		{
			return mSize;
		}
		inline uint32_t getResolution() const
		{
			return mResolution;
		}
		inline uint32_t getSizeInPixels() const
		{
			return static_cast<uint32_t>(mSize / 72.0f * mResolution);
		}
		inline float getAscender() const
		{
			return mAscender;
		}
		inline float getDescender() const
		{
			return mDescender;
		}
		inline float getHeight() const
		{
			return mHeight;
		}
		inline uint32_t getHeightInPixels() const
		{
			return static_cast<uint32_t>(getHeight() / 72.0f * mResolution);
		}
	public:
		virtual bool isValid() const = 0;
		virtual float getTextWidth(const char *text) = 0;
		virtual void drawText(const char *text, const float *color, const float objectSpaceToClipSpace[16], float scaleX = 1.0f, float scaleY = 1.0f, float biasX = 0.0f, float biasY = 0.0f, uint32_t flags = 0) = 0;
	protected:
		IFont();
		explicit IFont(const IFont &source);
		IFont &operator =(const IFont &source);
	protected:
		uint32_t mSize;
		uint32_t mResolution;
		float	 mAscender;
		float	 mDescender;
		float	 mHeight;
	};
	typedef Renderer::SmartRefCount<IFont> IFontPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERERRUNTIME_H__
