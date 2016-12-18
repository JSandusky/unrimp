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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/Mesh/Loader/MeshResourceLoader.h"
#include "RendererRuntime/Resource/Mesh/Loader/MeshFileFormat.h"
#include "RendererRuntime/Resource/Mesh/MeshResource.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <glm/detail/setup.hpp>	// For "glm::countof()"

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId MeshResourceLoader::TYPE_ID("mesh");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void MeshResourceLoader::onDeserialization()
	{
		// TODO(co) Error handling
		try
		{
			std::ifstream inputFileStream(mAsset.assetFilename, std::ios::binary);

			// Read in the mesh header
			v1Mesh::Header meshHeader;
			inputFileStream.read(reinterpret_cast<char*>(&meshHeader), sizeof(v1Mesh::Header));
			mMeshResource->mNumberOfVertices = meshHeader.numberOfVertices;
			mMeshResource->mNumberOfIndices  = meshHeader.numberOfIndices;

			// Allocate memory for the local vertex buffer data
			mNumberOfUsedVertexBufferDataBytes = meshHeader.numberOfBytesPerVertex * mMeshResource->mNumberOfVertices;
			if (mNumberOfVertexBufferDataBytes < mNumberOfUsedVertexBufferDataBytes)
			{
				mNumberOfVertexBufferDataBytes = mNumberOfUsedVertexBufferDataBytes;
				delete [] mVertexBufferData;
				mVertexBufferData = new uint8_t[mNumberOfVertexBufferDataBytes];
			}

			// Allocate memory for the local index buffer data
			mIndexBufferFormat = meshHeader.indexBufferFormat;
			mNumberOfUsedIndexBufferDataBytes = Renderer::IndexBufferFormat::getNumberOfBytesPerElement(static_cast<Renderer::IndexBufferFormat::Enum>(mIndexBufferFormat)) * mMeshResource->mNumberOfIndices;
			if (mNumberOfIndexBufferDataBytes < mNumberOfUsedIndexBufferDataBytes)
			{
				mNumberOfIndexBufferDataBytes = mNumberOfUsedIndexBufferDataBytes;
				delete [] mIndexBufferData;
				mIndexBufferData = new uint8_t[mNumberOfIndexBufferDataBytes];
			}

			// Read in the vertex and index buffer
			inputFileStream.read(reinterpret_cast<char*>(mVertexBufferData), mNumberOfUsedVertexBufferDataBytes);
			inputFileStream.read(reinterpret_cast<char*>(mIndexBufferData), mNumberOfUsedIndexBufferDataBytes);

			// Read in the vertex attributes
			mNumberOfUsedVertexAttributes = meshHeader.numberOfVertexAttributes;
			if (mNumberOfVertexAttributes < mNumberOfUsedVertexAttributes)
			{
				mNumberOfVertexAttributes = mNumberOfUsedVertexAttributes;
				delete [] mVertexAttributes;
				mVertexAttributes = new Renderer::VertexAttribute[mNumberOfVertexAttributes];
			}
			inputFileStream.read(reinterpret_cast<char*>(mVertexAttributes), sizeof(Renderer::VertexAttribute) * mNumberOfUsedVertexAttributes);

			// Read in the sub-meshes
			mNumberOfUsedSubMeshes = meshHeader.numberOfSubMeshes;
			if (mNumberOfSubMeshes < mNumberOfUsedSubMeshes)
			{
				mNumberOfSubMeshes = mNumberOfUsedSubMeshes;
				delete [] mSubMeshes;
				mSubMeshes = new v1Mesh::SubMesh[mNumberOfSubMeshes];
			}
			inputFileStream.read(reinterpret_cast<char*>(mSubMeshes), sizeof(v1Mesh::SubMesh) * mNumberOfUsedSubMeshes);
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load mesh asset %d: %s", mAsset.assetId, e.what());
		}
	}

	void MeshResourceLoader::onDispatch()
	{
		{ // Create vertex array object (VAO)
			// Create the vertex buffer object (VBO)
			Renderer::IVertexBufferPtr vertexBuffer(mBufferManager.createVertexBuffer(mNumberOfUsedVertexBufferDataBytes, mVertexBufferData, Renderer::BufferUsage::STATIC_DRAW));

			// Create the index buffer object (IBO)
			Renderer::IIndexBuffer *indexBuffer = mBufferManager.createIndexBuffer(mNumberOfUsedIndexBufferDataBytes, static_cast<Renderer::IndexBufferFormat::Enum>(mIndexBufferFormat), mIndexBufferData, Renderer::BufferUsage::STATIC_DRAW);

			// Create vertex array object (VAO)
			// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
			// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
			// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
			//    reference of the used vertex buffer objects (VBO). If the reference counter of a
			//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
			const uint32_t numberOfVertices = mMeshResource->mNumberOfVertices;
			const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] =
			{
				{ // Vertex buffer 0
					vertexBuffer,																		// vertexBuffer (Renderer::IVertexBuffer *)
					(numberOfVertices > 0) ? mNumberOfUsedVertexBufferDataBytes / numberOfVertices : 0	// strideInBytes (uint32_t)
				}
			};
			mMeshResource->mVertexArray = mBufferManager.createVertexArray(Renderer::VertexAttributes(mNumberOfUsedVertexAttributes, mVertexAttributes), glm::countof(vertexArrayVertexBuffers), vertexArrayVertexBuffers, indexBuffer);
		}

		{ // Create sub-meshes
			MaterialResourceManager& materialResourceManager = mRendererRuntime.getMaterialResourceManager();
			SubMeshes& subMeshes = mMeshResource->mSubMeshes;
			subMeshes.resize(mNumberOfUsedSubMeshes);
			for (uint32_t i = 0; i < mNumberOfUsedSubMeshes; ++i)
			{
				// Get source and destination sub-mesh references
				SubMesh& subMesh = subMeshes[i];
				const v1Mesh::SubMesh& v1SubMesh = mSubMeshes[i];

				// Setup sub-mesh
				subMesh.mMaterialResourceId = materialResourceManager.loadMaterialResourceByAssetId(v1SubMesh.materialAssetId);
				subMesh.mPrimitiveTopology  = static_cast<Renderer::PrimitiveTopology>(v1SubMesh.primitiveTopology);
				subMesh.mStartIndexLocation = v1SubMesh.startIndexLocation;
				subMesh.mNumberOfIndices	= v1SubMesh.numberOfIndices;
			}
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	MeshResourceLoader::MeshResourceLoader(IResourceManager& resourceManager, IRendererRuntime& rendererRuntime) :
		IResourceLoader(resourceManager),
		mRendererRuntime(rendererRuntime),
		mBufferManager(rendererRuntime.getBufferManager()),
		mMeshResource(nullptr),
		mNumberOfVertexBufferDataBytes(0),
		mNumberOfUsedVertexBufferDataBytes(0),
		mVertexBufferData(nullptr),
		mNumberOfIndexBufferDataBytes(0),
		mNumberOfUsedIndexBufferDataBytes(0),
		mIndexBufferData(nullptr),
		mIndexBufferFormat(0),
		mNumberOfVertexAttributes(0),
		mNumberOfUsedVertexAttributes(0),
		mVertexAttributes(nullptr),
		mNumberOfSubMeshes(0),
		mNumberOfUsedSubMeshes(0),
		mSubMeshes(nullptr)
	{
		// Nothing here
	}

	MeshResourceLoader::~MeshResourceLoader()
	{
		delete [] mVertexBufferData;
		delete [] mIndexBufferData;
		delete [] mVertexAttributes;
		delete [] mSubMeshes;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
