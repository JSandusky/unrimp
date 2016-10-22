/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
		// Nothing here
	}

	inline Renderer::IRenderer& IRendererRuntime::getRenderer() const
	{
		return *mRenderer;
	}

	inline Renderer::IBufferManager& IRendererRuntime::getBufferManager() const
	{
		return *mBufferManager;
	}

	inline ThreadManager& IRendererRuntime::getThreadManager() const
	{
		return *mThreadManager;
	}

	inline AssetManager& IRendererRuntime::getAssetManager() const
	{
		return *mAssetManager;
	}

	inline ResourceStreamer& IRendererRuntime::getResourceStreamer() const
	{
		return *mResourceStreamer;
	}

	inline TextureResourceManager& IRendererRuntime::getTextureResourceManager() const
	{
		return *mTextureResourceManager;
	}

	inline ShaderPieceResourceManager& IRendererRuntime::getShaderPieceResourceManager() const
	{
		return *mShaderPieceResourceManager;
	}

	inline ShaderBlueprintResourceManager& IRendererRuntime::getShaderBlueprintResourceManager() const
	{
		return *mShaderBlueprintResourceManager;
	}

	inline MaterialBlueprintResourceManager& IRendererRuntime::getMaterialBlueprintResourceManager() const
	{
		return *mMaterialBlueprintResourceManager;
	}

	inline MaterialResourceManager& IRendererRuntime::getMaterialResourceManager() const
	{
		return *mMaterialResourceManager;
	}

	inline SkeletonResourceManager& IRendererRuntime::getSkeletonResourceManager() const
	{
		return *mSkeletonResourceManager;
	}

	inline MeshResourceManager& IRendererRuntime::getMeshResourceManager() const
	{
		return *mMeshResourceManager;
	}

	inline SceneResourceManager& IRendererRuntime::getSceneResourceManager() const
	{
		return *mSceneResourceManager;
	}

	inline CompositorNodeResourceManager& IRendererRuntime::getCompositorNodeResourceManager() const
	{
		return *mCompositorNodeResourceManager;
	}

	inline CompositorWorkspaceResourceManager& IRendererRuntime::getCompositorWorkspaceResourceManager() const
	{
		return *mCompositorWorkspaceResourceManager;
	}

	inline const IRendererRuntime::ResourceManagers& IRendererRuntime::getResourceManagers() const
	{
		return mResourceManagers;
	}

	inline PipelineStateCompiler& IRendererRuntime::getPipelineStateCompiler() const
	{
		return *mPipelineStateCompiler;
	}

	inline DebugGuiManager& IRendererRuntime::getDebugGuiManager() const
	{
		return *mDebugGuiManager;
	}

	inline IVrManager& IRendererRuntime::getVrManager() const
	{
		return *mVrManager;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline IRendererRuntime::IRendererRuntime() :
		// Core
		mRenderer(nullptr),
		mBufferManager(nullptr),
		mThreadManager(nullptr),
		mAssetManager(nullptr),
		// Resource
		mResourceStreamer(nullptr),
		mTextureResourceManager(nullptr),
		mShaderPieceResourceManager(nullptr),
		mShaderBlueprintResourceManager(nullptr),
		mMaterialBlueprintResourceManager(nullptr),
		mMaterialResourceManager(nullptr),
		mSkeletonResourceManager(nullptr),
		mMeshResourceManager(nullptr),
		mSceneResourceManager(nullptr),
		mCompositorNodeResourceManager(nullptr),
		mCompositorWorkspaceResourceManager(nullptr),
		// Misc
		mPipelineStateCompiler(nullptr),
		// Optional
		mDebugGuiManager(nullptr),
		mVrManager(nullptr)
	{
		// Nothing here
	}

	inline IRendererRuntime::IRendererRuntime(const IRendererRuntime &) :
		// Core
		mRenderer(nullptr),
		mBufferManager(nullptr),
		mThreadManager(nullptr),
		mAssetManager(nullptr),
		// Resource
		mResourceStreamer(nullptr),
		mTextureResourceManager(nullptr),
		mShaderPieceResourceManager(nullptr),
		mShaderBlueprintResourceManager(nullptr),
		mMaterialBlueprintResourceManager(nullptr),
		mMaterialResourceManager(nullptr),
		mSkeletonResourceManager(nullptr),
		mMeshResourceManager(nullptr),
		mSceneResourceManager(nullptr),
		mCompositorNodeResourceManager(nullptr),
		mCompositorWorkspaceResourceManager(nullptr),
		// Misc
		mPipelineStateCompiler(nullptr),
		// Optional
		mDebugGuiManager(nullptr),
		mVrManager(nullptr)
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
