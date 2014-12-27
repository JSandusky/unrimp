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
#include "RendererRuntime/Resource/Mesh/Mesh.h"
#include "RendererRuntime/PlatformTypes.h"

#include <stdio.h>
#include <memory.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Mesh::Mesh(Renderer::IProgram &program, const char *filename) :
		mNumberOfVertices(0),
		mNumberOfIndices(0)
	{
		// Open the file
		// TODO(co) At the moment "fopen()" etc. are used directly
		FILE *file = fopen(filename, "rb");
		if (nullptr != file)
		{
			// Read in the mesh header
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
			MeshHeader meshHeader;
			fread(&meshHeader, sizeof(MeshHeader), 1, file);
			mNumberOfVertices = meshHeader.numberOfVertices;
			mNumberOfIndices  = meshHeader.numberOfIndices;

			// Allocate memory for the local vertex and index buffer data
			uint8_t *vertexBufferData = new uint8_t[meshHeader.numberOfBytesPerVertex * mNumberOfVertices];
			uint16_t *indexBufferData = new uint16_t[mNumberOfIndices];

			// Read in the vertex and index buffer
			fread(vertexBufferData, meshHeader.numberOfBytesPerVertex * mNumberOfVertices, 1, file);
			fread(indexBufferData, sizeof(uint16_t) * mNumberOfIndices, 1, file);	// TODO(co) Proper index size

			{ // Create vertex array object (VAO)
				// Get the used renderer instance
				Renderer::IRenderer &renderer = program.getRenderer();

				// Create the vertex buffer object (VBO)
				Renderer::IVertexBufferPtr vertexBuffer(renderer.createVertexBuffer(meshHeader.numberOfBytesPerVertex * mNumberOfVertices, vertexBufferData, Renderer::BufferUsage::STATIC_DRAW));

				// Create the index buffer object (IBO)
				Renderer::IIndexBuffer *indexBuffer = renderer.createIndexBuffer(sizeof(uint16_t) * mNumberOfIndices, static_cast<Renderer::IndexBufferFormat::Enum>(meshHeader.indexBufferFormat), indexBufferData, Renderer::BufferUsage::STATIC_DRAW);

				// TODO(co) If we would use a "IVertexBuffer"-handle instead of pointer inside "Renderer::VertexArrayAttribute", we could just dump the stuff and still be 32/64 bit compatible
				struct VertexArrayAttribute
				{
					uint8_t vertexArrayFormat;
					char	name[32];
					char	semantic[32];
					uint8_t semanticIndex;
					uint8_t offset;
					uint8_t stride;
				};

				// Read in the vertex array attributes
				VertexArrayAttribute *vertexArrayFromFile = new VertexArrayAttribute[meshHeader.numberOfVertexArrayAttributes];
				Renderer::VertexArrayAttribute *vertexArray = new Renderer::VertexArrayAttribute[meshHeader.numberOfVertexArrayAttributes];
				fread(vertexArrayFromFile, sizeof(VertexArrayAttribute) * meshHeader.numberOfVertexArrayAttributes, 1, file);
				for (uint32_t i = 0; i < meshHeader.numberOfVertexArrayAttributes; i++)
				{
					const VertexArrayAttribute &currentVertexArrayFromFile = vertexArrayFromFile[i];
					Renderer::VertexArrayAttribute &currentVertexArray = vertexArray[i];

					// Data destination
					currentVertexArray.vertexArrayFormat = static_cast<Renderer::VertexArrayFormat::Enum>(currentVertexArrayFromFile.vertexArrayFormat);
					memcpy(currentVertexArray.name, currentVertexArrayFromFile.name, 32);
					memcpy(currentVertexArray.semantic, currentVertexArrayFromFile.semantic, 32);
					currentVertexArray.semanticIndex = currentVertexArrayFromFile.semanticIndex;

					// Data source
					currentVertexArray.vertexBuffer = vertexBuffer;
					currentVertexArray.offset = currentVertexArrayFromFile.offset;
					currentVertexArray.stride = currentVertexArrayFromFile.stride;

					// Data source, instancing part
					currentVertexArray.instancesPerElement = 0;
				}

				// Create vertex array object (VAO)
				// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
				// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
				// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
				//    reference of the used vertex buffer objects (VBO). If the reference counter of a
				//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
				mVertexArray = program.createVertexArray(meshHeader.numberOfVertexArrayAttributes, vertexArray, indexBuffer);

				delete [] vertexArrayFromFile;
				delete [] vertexArray;
			}

			// Destroy local vertex and input buffer data
			delete [] vertexBufferData;
			delete [] indexBufferData;

			// Close the file
			fclose(file);
		}
		else
		{
			RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF("Failed to load in \"%s\"", filename)
		}
	}

	Mesh::~Mesh()
	{
		// The renderer resource pointers are released automatically
		// Nothing to do in here
	}

	void Mesh::draw()
	{
		// Valid mesh?
		if (nullptr != mVertexArray)
		{
			// Get the used renderer instance
			Renderer::IRenderer &renderer = mVertexArray->getRenderer();

			{ // Setup input assembly (IA)
				// Set the used vertex array
				renderer.iaSetVertexArray(mVertexArray);

				// Set the primitive topology used for draw calls
				renderer.iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_LIST);
			}

			// Render the specified geometric primitive, based on indexing into an array of vertices
			renderer.drawIndexed(0, mNumberOfIndices, 0, 0, mNumberOfVertices);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
