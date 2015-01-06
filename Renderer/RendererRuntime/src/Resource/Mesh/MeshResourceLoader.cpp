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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Mesh/MeshResourceLoader.h"
#include "RendererRuntime/Resource/Mesh/MeshResource.h"

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void MeshResourceLoader::onDeserialization()
	{
		// TODO(co) Error handling
		try
		{
			std::ifstream ifstream(mAsset.assetFilename, std::ios::binary);

			// Read in the mesh header
			#pragma pack(push)
			#pragma pack(1)
				struct MeshHeader
				{
					uint32_t formatType;
					uint16_t formatVersion;
					uint8_t  numberOfBytesPerVertex;
					uint32_t numberOfVertices;
					uint8_t  indexBufferFormat;
					uint32_t numberOfIndices;
					uint8_t  numberOfVertexArrayAttributes;
				};
			#pragma pack(pop)
			MeshHeader meshHeader;
			ifstream.read(reinterpret_cast<char*>(&meshHeader), sizeof(MeshHeader));
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
			ifstream.read(reinterpret_cast<char*>(mVertexBufferData), mNumberOfUsedVertexBufferDataBytes);
			ifstream.read(reinterpret_cast<char*>(mIndexBufferData), mNumberOfUsedIndexBufferDataBytes);

			// Read in the vertex array attributes
			mNumberOfUsedVertexArrayAttributes = meshHeader.numberOfVertexArrayAttributes;
			if (mNumberOfVertexArrayAttributes < mNumberOfUsedVertexArrayAttributes)
			{
				mNumberOfVertexArrayAttributes = mNumberOfUsedVertexArrayAttributes;
				delete [] mVertexArrayFromFile;
				delete [] mVertexArray;
				mVertexArrayFromFile = new VertexArrayAttribute[mNumberOfVertexArrayAttributes];
				mVertexArray = new Renderer::VertexArrayAttribute[mNumberOfVertexArrayAttributes];
			}
			ifstream.read(reinterpret_cast<char*>(mVertexArrayFromFile), sizeof(VertexArrayAttribute) * mNumberOfUsedVertexArrayAttributes);
			for (uint32_t i = 0; i < mNumberOfUsedVertexArrayAttributes; ++i)
			{
				const VertexArrayAttribute &currentVertexArrayFromFile = mVertexArrayFromFile[i];
				Renderer::VertexArrayAttribute &currentVertexArray = mVertexArray[i];

				// Data destination
				currentVertexArray.vertexArrayFormat = static_cast<Renderer::VertexArrayFormat::Enum>(currentVertexArrayFromFile.vertexArrayFormat);
				memcpy(currentVertexArray.name, currentVertexArrayFromFile.name, 32);
				memcpy(currentVertexArray.semantic, currentVertexArrayFromFile.semantic, 32);
				currentVertexArray.semanticIndex = currentVertexArrayFromFile.semanticIndex;

				// Data source
				currentVertexArray.vertexBuffer = nullptr;	// Filled in "RendererRuntime::MeshResourceLoader::onRendererBackendDispatch()"
				currentVertexArray.offset = currentVertexArrayFromFile.offset;
				currentVertexArray.stride = currentVertexArrayFromFile.stride;

				// Data source, instancing part
				currentVertexArray.instancesPerElement = 0;
			}
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load mesh asset %d: %s", mAsset.assetId, e.what());
		}
	}

	void MeshResourceLoader::onProcessing()
	{
		// Nothing here
	}

	void MeshResourceLoader::onRendererBackendDispatch()
	{
		// Create vertex array object (VAO)

		// Get the used renderer instance
		Renderer::IRenderer &renderer = mProgram->getRenderer();

		// Create the vertex buffer object (VBO)
		Renderer::IVertexBufferPtr vertexBuffer(renderer.createVertexBuffer(mNumberOfUsedVertexBufferDataBytes, mVertexBufferData, Renderer::BufferUsage::STATIC_DRAW));

		// Create the index buffer object (IBO)
		Renderer::IIndexBuffer *indexBuffer = renderer.createIndexBuffer(mNumberOfUsedIndexBufferDataBytes, static_cast<Renderer::IndexBufferFormat::Enum>(mIndexBufferFormat), mIndexBufferData, Renderer::BufferUsage::STATIC_DRAW);

		// Finalize the vertex array attributes by telling them about the vertex buffer source
		for (uint32_t i = 0; i < mNumberOfUsedVertexArrayAttributes; ++i)
		{
			mVertexArray[i].vertexBuffer = vertexBuffer;
		}

		// Create vertex array object (VAO)
		// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
		// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
		// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
		//    reference of the used vertex buffer objects (VBO). If the reference counter of a
		//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
		mMeshResource->mVertexArray = mProgram->createVertexArray(mNumberOfUsedVertexArrayAttributes, mVertexArray, indexBuffer);
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	MeshResourceLoader::~MeshResourceLoader()
	{
		delete [] mVertexBufferData;
		delete [] mIndexBufferData;
		delete [] mVertexArrayFromFile;
		delete [] mVertexArray;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
