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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Detail/IResourceLoader.h"
#include "RendererRuntime/Asset/Asset.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRenderer;
	struct VertexAttribute;
}
namespace RendererRuntime
{
	class MeshResource;
	class IRendererRuntime;
	namespace v1Mesh
	{
		struct SubMesh;
	}
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class MeshResourceLoader : protected IResourceLoader
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class MeshResourceManager;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const ResourceLoaderTypeId TYPE_ID;


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	public:
		virtual ResourceLoaderTypeId getResourceLoaderTypeId() const override;
		virtual void onDeserialization() override;
		virtual void onProcessing() override;
		virtual void onRendererBackendDispatch() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline MeshResourceLoader(IResourceManager& resourceManager, IRendererRuntime& rendererRuntime);
		virtual ~MeshResourceLoader();
		MeshResourceLoader(const MeshResourceLoader&) = delete;
		MeshResourceLoader& operator=(const MeshResourceLoader&) = delete;
		inline void initialize(const Asset& asset, MeshResource& meshResource, Renderer::IRenderer& renderer);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime& mRendererRuntime;	///< Renderer runtime instance, do not destroy the instance
		// Resource source and destination
		Asset				 mAsset;	///< In order to be multi-threading safe in here, we need an asset copy
		MeshResource*		 mMeshResource;
		Renderer::IRenderer* mRenderer;
		// Temporary data
		// Temporary vertex buffer
		uint32_t mNumberOfVertexBufferDataBytes;
		uint32_t mNumberOfUsedVertexBufferDataBytes;
		uint8_t* mVertexBufferData;
		// Temporary index buffer
		uint32_t mNumberOfIndexBufferDataBytes;
		uint32_t mNumberOfUsedIndexBufferDataBytes;
		uint8_t* mIndexBufferData;
		uint8_t  mIndexBufferFormat;	// "Renderer::IndexBufferFormat", don't want to include the header in here
		// Temporary vertex attributes
		uint32_t				   mNumberOfVertexAttributes;
		uint32_t				   mNumberOfUsedVertexAttributes;
		Renderer::VertexAttribute* mVertexAttributes;
		// Temporary sub-meshes
		uint32_t		 mNumberOfSubMeshes;
		uint32_t		 mNumberOfUsedSubMeshes;
		v1Mesh::SubMesh* mSubMeshes;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Mesh/Loader/MeshResourceLoader.inl"
