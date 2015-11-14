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
#include "RendererRuntime/Asset/Asset.h"

#include <Renderer/Public/Renderer.h>


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
	class FontResourceManager;
	class MeshResourceManager;
	class SceneResourceManager;
	class ShaderResourceManager;
	class TextureResourceManager;
	class MaterialResourceManager;
	class CompositorResourceManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


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
		*/
		virtual void reloadResourceByAssetId(AssetId assetId) const = 0;

		/**
		*  @brief
		*    Renderer runtime update
		*
		*  @note
		*    - Call this once per frame
		*/
		virtual void update() const = 0;


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
		ResourceStreamer*			mResourceStreamer;
		TextureResourceManager*		mTextureResourceManager;
		ShaderResourceManager*		mShaderResourceManager;
		MaterialResourceManager*	mMaterialResourceManager;
		FontResourceManager*		mFontResourceManager;
		MeshResourceManager*		mMeshResourceManager;
		SceneResourceManager*		mSceneResourceManager;
		CompositorResourceManager*	mCompositorResourceManager;


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
