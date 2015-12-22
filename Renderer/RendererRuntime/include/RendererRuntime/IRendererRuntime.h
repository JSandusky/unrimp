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
namespace Renderer
{
	class IRenderer;
}
namespace RendererRuntime
{
	class AssetManager;
	class IRendererRuntime;
	class ResourceStreamer;
	class IResourceManager;
	class FontResourceManager;
	class MeshResourceManager;
	class SceneResourceManager;
	class ShaderResourceManager;
	class TextureResourceManager;
	class MaterialResourceManager;
	class SkeletonResourceManager;
	class CompositorResourceManager;
	class MaterialBlueprintResourceManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId AssetId;	///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>" (Example: "Example/Font/Default/LinBiolinum_R" will result in asset ID 64363173)


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

		/**
		*  @brief
		*    Return the used renderer instance
		*
		*  @return
		*    The used renderer instance, do not release the returned instance unless you added an own reference to it
		*/
		inline Renderer::IRenderer& getRenderer() const;

		//[-------------------------------------------------------]
		//[ Managers                                              ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Return the asset manager instance
		*
		*  @return
		*    The asset manager instance, do not release the returned instance
		*/
		inline AssetManager& getAssetManager() const;

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
		*    Return the texture resource manager instance
		*
		*  @return
		*    The texture resource manager instance, do not release the returned instance
		*/
		inline TextureResourceManager& getTextureResourceManager() const;

		/**
		*  @brief
		*    Return the shader resource manager instance
		*
		*  @return
		*    The shader resource manager instance, do not release the returned instance
		*/
		inline ShaderResourceManager& getShaderResourceManager() const;

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
		*    Return the font resource manager instance
		*
		*  @return
		*    The font resource manager instance, do not release the returned instance
		*/
		inline FontResourceManager& getFontResourceManager() const;

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
		*    Return the compositor resource manager instance
		*
		*  @return
		*    The compositor resource manager instance, do not release the returned instance
		*/
		inline CompositorResourceManager& getCompositorResourceManager() const;

		/**
		*  @brief
		*    Return a list of all resource manager instances
		*
		*  @return
		*    List of all resource manager instances, do not release the returned instances
		*/
		inline const ResourceManagers& getResourceManagers() const;


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
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Default constructor
		*/
		inline IRendererRuntime();

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit IRendererRuntime(const IRendererRuntime &source);

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		inline IRendererRuntime &operator =(const IRendererRuntime &source);


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		Renderer::IRenderer* mRenderer;	///< The used renderer instance (we keep a reference to it), always valid
		// Managers
		AssetManager* mAssetManager;
		// Resource
		ResourceStreamer*					mResourceStreamer;
		TextureResourceManager*				mTextureResourceManager;
		ShaderResourceManager*				mShaderResourceManager;
		MaterialBlueprintResourceManager*	mMaterialBlueprintResourceManager;
		MaterialResourceManager*			mMaterialResourceManager;
		FontResourceManager*				mFontResourceManager;
		SkeletonResourceManager*			mSkeletonResourceManager;
		MeshResourceManager*				mMeshResourceManager;
		SceneResourceManager*				mSceneResourceManager;
		CompositorResourceManager*			mCompositorResourceManager;
		ResourceManagers					mResourceManagers;


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
