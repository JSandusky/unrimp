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
#include "RendererToolkit/AssetCompiler/IAssetCompiler.h"

#include <assimp/vector3.h>	// Required by "assimp/matrix4x4.h" below
#include <assimp/matrix4x4.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct aiNode;
struct aiScene;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class MeshAssetCompiler : public IAssetCompiler
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const AssetCompilerTypeId TYPE_ID;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		MeshAssetCompiler();
		virtual ~MeshAssetCompiler();


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	public:
		virtual AssetCompilerTypeId getAssetCompilerTypeId() const override;
		virtual void compile(const Input& input, const Configuration& configuration, Output& output) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Get the total number of vertices and indices by using a given Assimp node
		*
		*  @param[in]  assimpScene
		*    Assimp scene
		*  @param[in]  assimpNode
		*    Assimp node to gather the data from
		*  @param[out] numberOfVertices
		*    Receives the number of vertices
		*  @param[out] numberOfIndices
		*    Receives the number of indices
		*/
		void getNumberOfVerticesAndIndicesRecursive(const aiScene &assimpScene, const aiNode &assimpNode, uint32_t &numberOfVertices, uint32_t &numberOfIndices);

		/**
		*  @brief
		*    Fill the mesh data recursively
		*
		*  @param[in]  assimpScene
		*    Assimp scene
		*  @param[in]  assimpNode
		*    Assimp node to gather the data from
		*  @param[in]  vertexBuffer
		*    Vertex buffer to fill
		*  @param[in]  indexBuffer
		*    Index buffer to fill
		*  @param[in]  assimpTransformation
		*    Current absolute Assimp transformation matrix (local to global space)
		*  @param[out] numberOfVertices
		*    Receives the number of processed vertices
		*  @param[out] numberOfIndices
		*    Receives the number of processed indices
		*/
		void fillMeshRecursive(const aiScene &assimpScene, const aiNode &assimpNode, uint8_t *vertexBuffer, uint16_t *indexBuffer, const aiMatrix4x4 &assimpTransformation, uint32_t &numberOfVertices, uint32_t &numberOfIndices);


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		static const uint32_t NUMBER_OF_BYTES_PER_VERTEX = 24;	///< Number of bytes per vertex (3 float position, 2 short texture coordinate, 4 short qtangent)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
