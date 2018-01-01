/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "RendererRuntime/Resource/VertexAttributes/Loader/VertexAttributesResourceLoader.h"
#include "RendererRuntime/Resource/VertexAttributes/Loader/VertexAttributesFileFormat.h"
#include "RendererRuntime/Resource/VertexAttributes/VertexAttributesResource.h"
#include "RendererRuntime/Resource/Scene/Item/Terrain/TerrainSceneItem.h"
#include "RendererRuntime/Resource/Mesh/MeshResource.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	#include <glm/detail/setup.hpp>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId VertexAttributesResourceLoader::TYPE_ID("vertex_attributes");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void VertexAttributesResourceLoader::initialize(const Asset& asset, bool reload, IResource& resource)
	{
		IResourceLoader::initialize(asset, reload);
		mVertexAttributesResource = static_cast<VertexAttributesResource*>(&resource);
	}

	void VertexAttributesResourceLoader::onDeserialization(IFile& file)
	{
		// Tell the memory mapped file about the LZ4 compressed data
		mMemoryFile.loadLz4CompressedDataFromFile(v1VertexAttributes::FORMAT_TYPE, v1VertexAttributes::FORMAT_VERSION, file);
	}

	void VertexAttributesResourceLoader::onProcessing()
	{
		// Decompress LZ4 compressed data
		mMemoryFile.decompress();

		// Read in the vertex attributes header
		v1VertexAttributes::VertexAttributesHeader vertexAttributesHeader;
		mMemoryFile.read(&vertexAttributesHeader, sizeof(v1VertexAttributes::VertexAttributesHeader));

		// TODO(co) Implement vertex attributes file format. This here is just a fixed build in dummy.
		if (593231539 == getAsset().assetId)	// "VertexAttributes/Default/Compositor"
		{
			Renderer::VertexAttributes& vertexAttributes = const_cast<Renderer::VertexAttributes&>(mVertexAttributesResource->mVertexAttributes);
			static const Renderer::VertexAttribute vertexAttributesLayout[] =
			{
				{ // Attribute 0
					// Data destination
					Renderer::VertexAttributeFormat::FLOAT_4,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
					"PositionTexCoord",							// name[32] (char)
					"POSITION",									// semanticName[32] (char)
					0,											// semanticIndex (uint32_t)
					// Data source
					0,											// inputSlot (uint32_t)
					0,											// alignedByteOffset (uint32_t)
					sizeof(float) * 4,							// strideInBytes (uint32_t)
					0											// instancesPerElement (uint32_t)
				}
			};
			vertexAttributes.numberOfAttributes = static_cast<uint32_t>(glm::countof(vertexAttributesLayout));
			vertexAttributes.attributes = vertexAttributesLayout;
		}
		else if (4054156081 == getAsset().assetId)	// "VertexAttributes/Default/Position"
		{
			Renderer::VertexAttributes& vertexAttributes = const_cast<Renderer::VertexAttributes&>(mVertexAttributesResource->mVertexAttributes);
			static const Renderer::VertexAttribute vertexAttributesLayout[] =
			{
				{ // Attribute 0
					// Data destination
					Renderer::VertexAttributeFormat::FLOAT_3,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
					"Position",									// name[32] (char)
					"POSITION",									// semanticName[32] (char)
					0,											// semanticIndex (uint32_t)
					// Data source
					0,											// inputSlot (uint32_t)
					0,											// alignedByteOffset (uint32_t)
					sizeof(float) * 3,							// strideInBytes (uint32_t)
					0											// instancesPerElement (uint32_t)
				}
			};
			vertexAttributes.numberOfAttributes = static_cast<uint32_t>(glm::countof(vertexAttributesLayout));
			vertexAttributes.attributes = vertexAttributesLayout;
		}
		else if (4212832814 == getAsset().assetId)	// "VertexAttributes/Default/DebugGui"
		{
			Renderer::VertexAttributes& vertexAttributes = const_cast<Renderer::VertexAttributes&>(mVertexAttributesResource->mVertexAttributes);
			static const Renderer::VertexAttribute vertexAttributesLayout[] =
			{
				{ // Attribute 0
					// Data destination
					Renderer::VertexAttributeFormat::FLOAT_2,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
					"Position",									// name[32] (char)
					"POSITION",									// semanticName[32] (char)
					0,											// semanticIndex (uint32_t)
					// Data source
					0,											// inputSlot (uint32_t)
					0,											// alignedByteOffset (uint32_t)
					sizeof(float) * 4 + sizeof(uint8_t) * 4,	// strideInBytes (uint32_t)
					0											// instancesPerElement (uint32_t)
				},
				{ // Attribute 1
					// Data destination
					Renderer::VertexAttributeFormat::FLOAT_2,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
					"TexCoord",									// name[32] (char)
					"TEXCOORD",									// semanticName[32] (char)
					0,											// semanticIndex (uint32_t)
					// Data source
					0,											// inputSlot (uint32_t)
					sizeof(float) * 2,							// alignedByteOffset (uint32_t)
					sizeof(float) * 4 + sizeof(uint8_t) * 4,	// strideInBytes (uint32_t)
					0											// instancesPerElement (uint32_t)
				},
				{ // Attribute 2
					// Data destination
					Renderer::VertexAttributeFormat::R8G8B8A8_UNORM,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
					"Color",											// name[32] (char)
					"COLOR",											// semanticName[32] (char)
					0,													// semanticIndex (uint32_t)
					// Data source
					0,													// inputSlot (uint32_t)
					sizeof(float) * 4,									// alignedByteOffset (uint32_t)
					sizeof(float) * 4 + sizeof(uint8_t) * 4,			// strideInBytes (uint32_t)
					0													// instancesPerElement (uint32_t)
				}
			};
			vertexAttributes.numberOfAttributes = static_cast<uint32_t>(glm::countof(vertexAttributesLayout));
			vertexAttributes.attributes = vertexAttributesLayout;
		}
		else if (1156289207 == getAsset().assetId)	// "VertexAttributes/Default/Mesh"
		{
			mVertexAttributesResource->mVertexAttributes = Renderer::VertexAttributes(MeshResource::VERTEX_ATTRIBUTES.numberOfAttributes, MeshResource::VERTEX_ATTRIBUTES.attributes);
		}
		else if (871545499 == getAsset().assetId)	// "VertexAttributes/Default/SkinnedMesh"
		{
			mVertexAttributesResource->mVertexAttributes = Renderer::VertexAttributes(MeshResource::SKINNED_VERTEX_ATTRIBUTES.numberOfAttributes, MeshResource::SKINNED_VERTEX_ATTRIBUTES.attributes);
		}
		else if (684507259 == getAsset().assetId)	// "VertexAttributes/Default/Terrain"
		{
			mVertexAttributesResource->mVertexAttributes = Renderer::VertexAttributes(TerrainSceneItem::VERTEX_ATTRIBUTES.numberOfAttributes, TerrainSceneItem::VERTEX_ATTRIBUTES.attributes);
		}
		else
		{
			assert(false && "Unknown vertex attributes asset");
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
