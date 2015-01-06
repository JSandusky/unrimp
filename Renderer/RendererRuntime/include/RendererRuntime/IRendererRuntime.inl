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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline IRendererRuntime::~IRendererRuntime()
	{
		// Nothing to do in here
	}

	inline Renderer::IRenderer& IRendererRuntime::getRenderer() const
	{
		return *mRenderer;
	}

	inline AssetManager& IRendererRuntime::getAssetManager() const
	{
		return *mAssetManager;
	}

	inline CompositorManager& IRendererRuntime::getCompositorManager() const
	{
		return *mCompositorManager;
	}

	inline SceneManager& IRendererRuntime::getSceneManager() const
	{
		return *mSceneManager;
	}

	inline ResourceStreamer& IRendererRuntime::getResourceStreamer() const
	{
		return *mResourceStreamer;
	}

	inline TextureResourceManager& IRendererRuntime::getTextureResourceManager() const
	{
		return *mTextureResourceManager;
	}

	inline MaterialResourceManager& IRendererRuntime::getMaterialResourceManager() const
	{
		return *mMaterialResourceManager;
	}

	inline FontResourceManager& IRendererRuntime::getFontResourceManager() const
	{
		return *mFontResourceManager;
	}

	inline MeshResourceManager& IRendererRuntime::getMeshResourceManager() const
	{
		return *mMeshResourceManager;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline IRendererRuntime::IRendererRuntime() :
		mRenderer(nullptr),
		// Managers
		mAssetManager(nullptr),
		mCompositorManager(nullptr),
		mSceneManager(nullptr),
		// Resource managers
		mResourceStreamer(nullptr),
		mTextureResourceManager(nullptr),
		mShaderResourceManager(nullptr),
		mMaterialResourceManager(nullptr),
		mFontResourceManager(nullptr),
		mMeshResourceManager(nullptr)
	{
		// Nothing to do in here
	}

	inline IRendererRuntime::IRendererRuntime(const IRendererRuntime &) :
		mRenderer(nullptr),
		// Managers
		mAssetManager(nullptr),
		mCompositorManager(nullptr),
		mSceneManager(nullptr),
		// Resource managers
		mResourceStreamer(nullptr),
		mTextureResourceManager(nullptr),
		mShaderResourceManager(nullptr),
		mMaterialResourceManager(nullptr),
		mFontResourceManager(nullptr),
		mMeshResourceManager(nullptr)
	{
		// Not supported
	}

	inline IRendererRuntime &IRendererRuntime::operator =(const IRendererRuntime &)
	{
		// Not supported
		return *this;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
