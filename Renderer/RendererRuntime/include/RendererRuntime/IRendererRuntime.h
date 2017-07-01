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
#include "RendererRuntime/Core/StringId.h"

#include <Renderer/Public/Renderer.h>

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class Context;
	class IVrManager;
	class TimeManager;
	class IFileManager;
	class AssetManager;
	class ThreadManager;
	class DebugGuiManager;
	class IRendererRuntime;
	class ResourceStreamer;
	class IResourceManager;
	class MeshResourceManager;
	class SceneResourceManager;
	class PipelineStateCompiler;
	class TextureResourceManager;
	class MaterialResourceManager;
	class SkeletonResourceManager;
	class ShaderPieceResourceManager;
	class CompositorNodeResourceManager;
	class ShaderBlueprintResourceManager;
	class VertexAttributesResourceManager;
	class SkeletonAnimationResourceManager;
	class MaterialBlueprintResourceManager;
	class CompositorWorkspaceResourceManager;
}


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
	*    Abstract renderer runtime interface
	*/
	class IRendererRuntime : public Renderer::RefCount<IRendererRuntime>
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::vector<IResourceManager*> ResourceManagers;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~IRendererRuntime();

		//[-------------------------------------------------------]
		//[ Core                                                  ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Return the used renderer runtime context instance
		*
		*  @return
		*    The used renderer runtime context instance
		*/
		inline Context& getContext() const;

		/**
		*  @brief
		*    Return the used renderer instance
		*
		*  @return
		*    The used renderer instance, do not release the returned instance unless you added an own reference to it
		*/
		inline Renderer::IRenderer& getRenderer() const;

		/**
		*  @brief
		*    Return the used buffer manager instance
		*
		*  @return
		*    The used buffer manager instance, do not release the returned instance unless you added an own reference to it
		*/
		inline Renderer::IBufferManager& getBufferManager() const;

		/**
		*  @brief
		*    Return the used texture manager instance
		*
		*  @return
		*    The used texture manager instance, do not release the returned instance unless you added an own reference to it
		*/
		inline Renderer::ITextureManager& getTextureManager() const;

		/**
		*  @brief
		*    Return the file manager instance
		*
		*  @return
		*    The file manager instance, do not release the returned instance
		*/
		inline IFileManager& getFileManager() const;

		/**
		*  @brief
		*    Return the thread manager instance
		*
		*  @return
		*    The thread manager instance, do not release the returned instance
		*/
		inline ThreadManager& getThreadManager() const;

		/**
		*  @brief
		*    Return the asset manager instance
		*
		*  @return
		*    The asset manager instance, do not release the returned instance
		*/
		inline AssetManager& getAssetManager() const;

		/**
		*  @brief
		*    Return the time manager instance
		*
		*  @return
		*    The time manager instance, do not release the returned instance
		*/
		inline TimeManager& getTimeManager() const;

		//[-------------------------------------------------------]
		//[ Resource                                              ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Return the resource streamer instance
		*
		*  @return
		*    The resource streamer instance, do not release the returned instance
		*/
		inline ResourceStreamer& getResourceStreamer() const;

		/**
		*  @brief
		*    Return the vertex attributes resource manager instance
		*
		*  @return
		*    The vertex attributes resource manager instance, do not release the returned instance
		*/
		inline VertexAttributesResourceManager& getVertexAttributesResourceManager() const;

		/**
		*  @brief
		*    Return the texture resource manager instance
		*
		*  @return
		*    The texture resource manager instance, do not release the returned instance
		*/
		inline TextureResourceManager& getTextureResourceManager() const;

		/**
		*  @brief
		*    Return the shader piece resource manager instance
		*
		*  @return
		*    The shader piece resource manager instance, do not release the returned instance
		*/
		inline ShaderPieceResourceManager& getShaderPieceResourceManager() const;

		/**
		*  @brief
		*    Return the shader blueprint resource manager instance
		*
		*  @return
		*    The shader blueprint resource manager instance, do not release the returned instance
		*/
		inline ShaderBlueprintResourceManager& getShaderBlueprintResourceManager() const;

		/**
		*  @brief
		*    Return the material blueprint resource manager instance
		*
		*  @return
		*    The material blueprint resource manager instance, do not release the returned instance
		*/
		inline MaterialBlueprintResourceManager& getMaterialBlueprintResourceManager() const;

		/**
		*  @brief
		*    Return the material resource manager instance
		*
		*  @return
		*    The material resource manager instance, do not release the returned instance
		*/
		inline MaterialResourceManager& getMaterialResourceManager() const;

		/**
		*  @brief
		*    Return the skeleton resource manager instance
		*
		*  @return
		*    The skeleton resource manager instance, do not release the returned instance
		*/
		inline SkeletonResourceManager& getSkeletonResourceManager() const;

		/**
		*  @brief
		*    Return the skeleton animation resource manager instance
		*
		*  @return
		*    The skeleton animation resource manager instance, do not release the returned instance
		*/
		inline SkeletonAnimationResourceManager& getSkeletonAnimationResourceManager() const;

		/**
		*  @brief
		*    Return the mesh resource manager instance
		*
		*  @return
		*    The mesh resource manager instance, do not release the returned instance
		*/
		inline MeshResourceManager& getMeshResourceManager() const;

		/**
		*  @brief
		*    Return the scene resource manager instance
		*
		*  @return
		*    The scene resource manager instance, do not release the returned instance
		*/
		inline SceneResourceManager& getSceneResourceManager() const;

		/**
		*  @brief
		*    Return the compositor node resource manager instance
		*
		*  @return
		*    The compositor node resource manager instance, do not release the returned instance
		*/
		inline CompositorNodeResourceManager& getCompositorNodeResourceManager() const;

		/**
		*  @brief
		*    Return the compositor workspace resource manager instance
		*
		*  @return
		*    The compositor workspace resource manager instance, do not release the returned instance
		*/
		inline CompositorWorkspaceResourceManager& getCompositorWorkspaceResourceManager() const;

		/**
		*  @brief
		*    Return a list of all resource manager instances
		*
		*  @return
		*    List of all resource manager instances, do not release the returned instances
		*/
		inline const ResourceManagers& getResourceManagers() const;

		//[-------------------------------------------------------]
		//[ Misc                                                  ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Return the pipeline state compiler instance
		*
		*  @return
		*    The pipeline state compiler instance, do not release the returned instance
		*/
		inline PipelineStateCompiler& getPipelineStateCompiler() const;

		//[-------------------------------------------------------]
		//[ Optional                                              ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Return the debug GUI manager instance
		*
		*  @return
		*    The debug GUI manager instance, do not release the returned instance
		*/
		inline DebugGuiManager& getDebugGuiManager() const;

		/**
		*  @brief
		*    Return the VR manager instance
		*
		*  @return
		*    The VR manager instance, do not release the returned instance
		*/
		inline IVrManager& getVrManager() const;


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IRendererRuntime methods ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Reload resource by using the given asset
		*
		*  @param[in] assetId
		*    ID of the asset which has been changed and hence the according resource needs to be reloaded
		*
		*  @note
		*    - This method is most likely called by a background thread
		*/
		virtual void reloadResourceByAssetId(AssetId assetId) = 0;

		/**
		*  @brief
		*    Renderer runtime update
		*
		*  @note
		*    - Call this once per frame
		*/
		virtual void update() = 0;

		//[-------------------------------------------------------]
		//[ Pipeline state object cache                           ]
		//[-------------------------------------------------------]
		virtual void clearPipelineStateObjectCache() = 0;
		virtual void loadPipelineStateObjectCache() = 0;
		virtual void savePipelineStateObjectCache() = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] context
		*    Renderer runtime context, the renderer runtime context instance must stay valid as long as the renderer runtime instance exists
		*/
		inline explicit IRendererRuntime(Context& context);

		explicit IRendererRuntime(const IRendererRuntime& source) = delete;
		IRendererRuntime& operator =(const IRendererRuntime& source) = delete;


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		// Core
		Context&				   mContext;		///< Renderer runtime context
		Renderer::IRenderer*	   mRenderer;		///< The used renderer instance (we keep a reference to it), always valid
		Renderer::IBufferManager*  mBufferManager;	///< The used buffer manager instance (we keep a reference to it), always valid
		Renderer::ITextureManager* mTextureManager;	///< The used texture manager instance (we keep a reference to it), always valid
		IFileManager*			   mFileManager;	///< The used file manager instance, always valid
		ThreadManager*			   mThreadManager;
		AssetManager*			   mAssetManager;
		TimeManager*			   mTimeManager;
		// Resource
		ResourceStreamer*					mResourceStreamer;
		VertexAttributesResourceManager*	mVertexAttributesResourceManager;
		TextureResourceManager*				mTextureResourceManager;
		ShaderPieceResourceManager*			mShaderPieceResourceManager;
		ShaderBlueprintResourceManager*		mShaderBlueprintResourceManager;
		MaterialBlueprintResourceManager*	mMaterialBlueprintResourceManager;
		MaterialResourceManager*			mMaterialResourceManager;
		SkeletonResourceManager*			mSkeletonResourceManager;
		SkeletonAnimationResourceManager*	mSkeletonAnimationResourceManager;
		MeshResourceManager*				mMeshResourceManager;
		SceneResourceManager*				mSceneResourceManager;
		CompositorNodeResourceManager*		mCompositorNodeResourceManager;
		CompositorWorkspaceResourceManager*	mCompositorWorkspaceResourceManager;
		ResourceManagers					mResourceManagers;
		// Misc
		PipelineStateCompiler* mPipelineStateCompiler;
		// Optional
		DebugGuiManager* mDebugGuiManager;
		IVrManager*		 mVrManager;


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef Renderer::SmartRefCount<IRendererRuntime> IRendererRuntimePtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/IRendererRuntime.inl"
