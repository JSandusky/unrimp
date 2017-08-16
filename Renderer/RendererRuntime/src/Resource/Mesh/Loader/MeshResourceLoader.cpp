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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/Mesh/Loader/MeshResourceLoader.h"
#include "RendererRuntime/Resource/Mesh/Loader/MeshFileFormat.h"
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/Resource/Mesh/MeshResource.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/Skeleton/SkeletonResourceManager.h"
#include "RendererRuntime/Resource/Skeleton/SkeletonResource.h"
#include "RendererRuntime/IRendererRuntime.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4201)	// warning C4201: nonstandard extension used: nameless struct/union
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	#include <glm/glm.hpp>
PRAGMA_WARNING_POP


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
	void MeshResourceLoader::onDeserialization(IFile& file)
	{
		// Tell the memory mapped file about the LZ4 compressed data
		mMemoryFile.loadLz4CompressedDataFromFile(v1Mesh::FORMAT_TYPE, v1Mesh::FORMAT_VERSION, file);
	}

	void MeshResourceLoader::onProcessing()
	{
		// Decompress LZ4 compressed data
		mMemoryFile.decompress();

		// Read in the mesh header
		v1Mesh::MeshHeader meshHeader;
		mMemoryFile.read(&meshHeader, sizeof(v1Mesh::MeshHeader));
		mMeshResource->setNumberOfVertices(meshHeader.numberOfVertices);
		mMeshResource->setNumberOfIndices(meshHeader.numberOfIndices);

		// Allocate memory for the local vertex buffer data
		mNumberOfUsedVertexBufferDataBytes = meshHeader.numberOfBytesPerVertex * mMeshResource->getNumberOfVertices();
		if (mNumberOfVertexBufferDataBytes < mNumberOfUsedVertexBufferDataBytes)
		{
			mNumberOfVertexBufferDataBytes = mNumberOfUsedVertexBufferDataBytes;
			delete [] mVertexBufferData;
			mVertexBufferData = new uint8_t[mNumberOfVertexBufferDataBytes];
		}

		// Allocate memory for the local index buffer data
		mIndexBufferFormat = meshHeader.indexBufferFormat;
		mNumberOfUsedIndexBufferDataBytes = Renderer::IndexBufferFormat::getNumberOfBytesPerElement(static_cast<Renderer::IndexBufferFormat::Enum>(mIndexBufferFormat)) * mMeshResource->getNumberOfIndices();
		if (mNumberOfIndexBufferDataBytes < mNumberOfUsedIndexBufferDataBytes)
		{
			mNumberOfIndexBufferDataBytes = mNumberOfUsedIndexBufferDataBytes;
			delete [] mIndexBufferData;
			mIndexBufferData = new uint8_t[mNumberOfIndexBufferDataBytes];
		}

		// Read in the vertex and index buffer
		mMemoryFile.read(mVertexBufferData, mNumberOfUsedVertexBufferDataBytes);
		mMemoryFile.read(mIndexBufferData, mNumberOfUsedIndexBufferDataBytes);

		// Read in the vertex attributes
		mNumberOfUsedVertexAttributes = meshHeader.numberOfVertexAttributes;
		if (mNumberOfVertexAttributes < mNumberOfUsedVertexAttributes)
		{
			mNumberOfVertexAttributes = mNumberOfUsedVertexAttributes;
			delete [] mVertexAttributes;
			mVertexAttributes = new Renderer::VertexAttribute[mNumberOfVertexAttributes];
		}
		mMemoryFile.read(mVertexAttributes, sizeof(Renderer::VertexAttribute) * mNumberOfUsedVertexAttributes);

		// Read in the sub-meshes
		mNumberOfUsedSubMeshes = meshHeader.numberOfSubMeshes;
		if (mNumberOfSubMeshes < mNumberOfUsedSubMeshes)
		{
			mNumberOfSubMeshes = mNumberOfUsedSubMeshes;
			delete [] mSubMeshes;
			mSubMeshes = new v1Mesh::SubMesh[mNumberOfSubMeshes];
		}
		mMemoryFile.read(mSubMeshes, sizeof(v1Mesh::SubMesh) * mNumberOfUsedSubMeshes);

		// Read in optional skeleton
		mNumberOfBones = meshHeader.numberOfBones;
		if (mNumberOfBones > 0)
		{
			// Read in the skeleton data in a single burst
			const uint32_t numberOfSkeletonDataBytes = (sizeof(uint8_t) + sizeof(uint32_t) + sizeof(glm::mat4) * 2) * mNumberOfBones;
			mSkeletonData = new uint8_t[numberOfSkeletonDataBytes + (sizeof(glm::mat4) + sizeof(glm::mat3x4)) * mNumberOfBones];	// "RendererRuntime::SkeletonResource::mGlobalBoneMatrices" & "RendererRuntime::SkeletonResource::mBoneSpaceMatrices" isn't serialized
			mMemoryFile.read(mSkeletonData, numberOfSkeletonDataBytes);
		}

		// Can we create the renderer resource asynchronous as well?
		if (mRendererRuntime.getRenderer().getCapabilities().nativeMultiThreading)
		{
			mVertexArray = createVertexArray();
		}
	}

	bool MeshResourceLoader::onDispatch()
	{
		// Create vertex array object (VAO)
		mMeshResource->setVertexArray(mRendererRuntime.getRenderer().getCapabilities().nativeMultiThreading ? mVertexArray : createVertexArray());

		{ // Create sub-meshes
			MaterialResourceManager& materialResourceManager = mRendererRuntime.getMaterialResourceManager();
			SubMeshes& subMeshes = mMeshResource->getSubMeshes();
			subMeshes.resize(mNumberOfUsedSubMeshes);
			for (uint32_t i = 0; i < mNumberOfUsedSubMeshes; ++i)
			{
				// Get source and destination sub-mesh references
				SubMesh& subMesh = subMeshes[i];
				const v1Mesh::SubMesh& v1SubMesh = mSubMeshes[i];

				// Setup sub-mesh
				MaterialResourceId materialResourceId = getUninitialized<MaterialResourceId>();
				materialResourceManager.loadMaterialResourceByAssetId(v1SubMesh.materialAssetId, materialResourceId);
				subMesh.setMaterialResourceId(materialResourceId);
				subMesh.setPrimitiveTopology(static_cast<Renderer::PrimitiveTopology>(v1SubMesh.primitiveTopology));
				subMesh.setStartIndexLocation(v1SubMesh.startIndexLocation);
				subMesh.setNumberOfIndices(v1SubMesh.numberOfIndices);

				// Sanity check
				assert(isInitialized(subMesh.getMaterialResourceId()));
			}
		}

		// Optional skeleton
		if (mNumberOfBones > 0)
		{
			SkeletonResourceManager& skeletonResourceManager = mRendererRuntime.getSkeletonResourceManager();

			// Get/create skeleton resource
			SkeletonResource* skeletonResource = nullptr;
			if (isInitialized(mMeshResource->getSkeletonResourceId()))
			{
				// Reuse existing skeleton resource
				skeletonResource = &skeletonResourceManager.getById(mMeshResource->getSkeletonResourceId());
				skeletonResource->clearSkeletonData();
			}
			else
			{
				// Create new skeleton resource
				const SkeletonResourceId skeletonResourceId = skeletonResourceManager.createSkeletonResourceByAssetId(getAsset().assetId);
				mMeshResource->setSkeletonResourceId(skeletonResourceId);
				skeletonResource = &skeletonResourceManager.getById(skeletonResourceId);
			}

			// Pass on the skeleton data to the skeleton resource
			skeletonResource->mNumberOfBones = mNumberOfBones;
			skeletonResource->mBoneParentIndices = mSkeletonData;
			mSkeletonData += sizeof(uint8_t) * mNumberOfBones;
			skeletonResource->mBoneIds = reinterpret_cast<uint32_t*>(mSkeletonData);
			mSkeletonData += sizeof(uint32_t) * mNumberOfBones;
			skeletonResource->mLocalBoneMatrices = reinterpret_cast<glm::mat4*>(mSkeletonData);
			mSkeletonData += sizeof(glm::mat4) * mNumberOfBones;
			skeletonResource->mBoneOffsetMatrices = reinterpret_cast<glm::mat4*>(mSkeletonData);
			mSkeletonData += sizeof(glm::mat4) * mNumberOfBones;
			skeletonResource->mGlobalBoneMatrices = reinterpret_cast<glm::mat4*>(mSkeletonData);
			mSkeletonData += sizeof(glm::mat4) * mNumberOfBones;
			skeletonResource->mBoneSpaceMatrices = reinterpret_cast<glm::mat3x4*>(mSkeletonData);
			skeletonResource->localToGlobalPose();

			// Skeleton data has been passed on
			mSkeletonData = nullptr;
		}

		// Fully loaded?
		return isFullyLoaded();
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	MeshResourceLoader::MeshResourceLoader(IResourceManager& resourceManager, IRendererRuntime& rendererRuntime) :
		IMeshResourceLoader(resourceManager, rendererRuntime),
		mBufferManager(rendererRuntime.getBufferManager()),
		mVertexArray(nullptr),
		// Temporary vertex buffer
		mNumberOfVertexBufferDataBytes(0),
		mNumberOfUsedVertexBufferDataBytes(0),
		mVertexBufferData(nullptr),
		// Temporary index buffer
		mNumberOfIndexBufferDataBytes(0),
		mNumberOfUsedIndexBufferDataBytes(0),
		mIndexBufferData(nullptr),
		mIndexBufferFormat(0),
		// Temporary vertex attributes
		mNumberOfVertexAttributes(0),
		mNumberOfUsedVertexAttributes(0),
		mVertexAttributes(nullptr),
		// Temporary sub-meshes
		mNumberOfSubMeshes(0),
		mNumberOfUsedSubMeshes(0),
		mSubMeshes(nullptr),
		// Optional temporary skeleton
		mNumberOfBones(0),
		mSkeletonData(nullptr)
	{
		// Nothing here
	}

	MeshResourceLoader::~MeshResourceLoader()
	{
		delete [] mVertexBufferData;
		delete [] mIndexBufferData;
		delete [] mVertexAttributes;
		delete [] mSubMeshes;
		delete [] mSkeletonData;	// In case the mesh resource loaded was never dispatched
	}

	Renderer::IVertexArray* MeshResourceLoader::createVertexArray() const
	{
		// Create the vertex buffer object (VBO)
		Renderer::IVertexBufferPtr vertexBuffer(mBufferManager.createVertexBuffer(mNumberOfUsedVertexBufferDataBytes, mVertexBufferData, Renderer::BufferUsage::STATIC_DRAW));
		RENDERER_SET_RESOURCE_DEBUG_NAME(vertexBuffer, getAsset().assetFilename)

		// Create the index buffer object (IBO)
		Renderer::IIndexBuffer* indexBuffer = mBufferManager.createIndexBuffer(mNumberOfUsedIndexBufferDataBytes, static_cast<Renderer::IndexBufferFormat::Enum>(mIndexBufferFormat), mIndexBufferData, Renderer::BufferUsage::STATIC_DRAW);
		RENDERER_SET_RESOURCE_DEBUG_NAME(indexBuffer, getAsset().assetFilename)

		// Create vertex array object (VAO)
		const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] = { vertexBuffer, mRendererRuntime.getMeshResourceManager().getDrawIdVertexBufferPtr() };
		Renderer::IVertexArray* vertexArray = mBufferManager.createVertexArray(Renderer::VertexAttributes(mNumberOfUsedVertexAttributes, mVertexAttributes), static_cast<uint32_t>(glm::countof(vertexArrayVertexBuffers)), vertexArrayVertexBuffers, indexBuffer);
		RENDERER_SET_RESOURCE_DEBUG_NAME(vertexArray, getAsset().assetFilename)

		// Done
		return vertexArray;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
