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
#include "RendererRuntime/Resource/Detail/IResource.h"
#include "RendererRuntime/Resource/Mesh/Detail/SubMesh.h"

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	template <class ELEMENT_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS> class PackedElementManager;
	template <class TYPE, class LOADER_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS> class ResourceManagerTemplate;
	class IMeshResourceLoader;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef std::vector<SubMesh> SubMeshes;
	typedef uint32_t			 MeshResourceId;		///< POD mesh resource identifier
	typedef uint32_t			 SkeletonResourceId;	///< POD skeleton resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Mesh resource class
	*/
	class MeshResource : public IResource
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend PackedElementManager<MeshResource, MeshResourceId, 4096>;							// Type definition of template class
		friend ResourceManagerTemplate<MeshResource, IMeshResourceLoader, MeshResourceId, 4096>;	// Type definition of template class


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT static const Renderer::VertexAttributes VERTEX_ATTRIBUTES;			///< Default vertex attributes layout, whenever possible stick to this to be as compatible as possible to the rest
		RENDERERRUNTIME_API_EXPORT static const Renderer::VertexAttributes SKINNED_VERTEX_ATTRIBUTES;	///< Default skinned vertex attributes layout, whenever possible stick to this to be as compatible as possible to the rest


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		//[-------------------------------------------------------]
		//[ Data                                                  ]
		//[-------------------------------------------------------]
		inline uint32_t getNumberOfVertices() const;
		inline void setNumberOfVertices(uint32_t numberOfVertices);
		inline uint32_t getNumberOfIndices() const;
		inline void setNumberOfIndices(uint32_t numberOfIndices);
		inline Renderer::IVertexArrayPtr getVertexArrayPtr() const;
		inline void setVertexArray(Renderer::IVertexArray* vertexArray);
		inline const SubMeshes& getSubMeshes() const;
		inline SubMeshes& getSubMeshes();
		inline SkeletonResourceId getSkeletonResourceId() const;
		inline void setSkeletonResourceId(SkeletonResourceId skeletonResourceId);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline MeshResource();
		inline virtual ~MeshResource();
		MeshResource(const MeshResource&) = delete;
		MeshResource& operator=(const MeshResource&) = delete;

		//[-------------------------------------------------------]
		//[ "RendererRuntime::PackedElementManager" management    ]
		//[-------------------------------------------------------]
		inline void initializeElement(MeshResourceId meshResourceId);
		inline void deinitializeElement();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		uint32_t				  mNumberOfVertices;	///< Number of vertices
		uint32_t				  mNumberOfIndices;		///< Number of indices
		Renderer::IVertexArrayPtr mVertexArray;			///< Vertex array object (VAO), can be a null pointer
		SubMeshes				  mSubMeshes;			///< Sub-meshes
		SkeletonResourceId		  mSkeletonResourceId;	///< Resource ID of the used skeleton, can be uninitialized


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Mesh/MeshResource.inl"
