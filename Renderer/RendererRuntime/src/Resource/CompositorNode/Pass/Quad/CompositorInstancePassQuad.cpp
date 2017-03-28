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
#include "RendererRuntime/Resource/CompositorNode/Pass/Quad/CompositorInstancePassQuad.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Quad/CompositorResourcePassQuad.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeInstance.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceInstance.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global variables                                      ]
		//[-------------------------------------------------------]
		static Renderer::IVertexArrayPtr VertexArrayPtr;	///< Vertex array object (VAO), can be a null pointer, shared between all compositor instance pass quad instances


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		Renderer::IVertexArray* createVertexArray(Renderer::IBufferManager& bufferManager)
		{
			// Vertex input layout
			const Renderer::VertexAttribute vertexAttributesLayout[] =
			{
				{ // Attribute 0
					// Data destination
					Renderer::VertexAttributeFormat::FLOAT_4,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
					"PositionTexCoord",							// name[32] (char)
					"POSITION",									// semanticName[32] (char)
					0,											// semanticIndex (uint32_t)
					// Data source
					0,											// inputSlot (size_t)
					0,											// alignedByteOffset (uint32_t)
					// Data source, instancing part
					0											// instancesPerElement (uint32_t)
				}
			};
			const Renderer::VertexAttributes vertexAttributes(static_cast<uint32_t>(glm::countof(vertexAttributesLayout)), vertexAttributesLayout);

			// Create the vertex buffer object (VBO)
			// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
			static const float VERTEX_POSITION[] =
			{								// Vertex ID	Triangle on screen
				-1.0f,  1.0f, 0.0f, 0.0f,	// 0			  0.......1
				 3.0f,  1.0f, 2.0f, 0.0f,	// 1			  .   .
				-1.0f, -3.0f, 0.0f, 2.0f	// 2			  2
			};
			Renderer::IVertexBufferPtr vertexBuffer(bufferManager.createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));
			RENDERER_SET_RESOURCE_DEBUG_NAME(vertexBuffer, "Compositor instance pass quad")

			// Create vertex array object (VAO)
			// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
			// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
			// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
			//    reference of the used vertex buffer objects (VBO). If the reference counter of a
			//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
			const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] =
			{
				{ // Vertex buffer 0
					vertexBuffer,		// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 4	// strideInBytes (uint32_t)
				}
			};
			Renderer::IVertexArray* vertexArray = bufferManager.createVertexArray(vertexAttributes, static_cast<uint32_t>(glm::countof(vertexArrayVertexBuffers)), vertexArrayVertexBuffers);
			RENDERER_SET_RESOURCE_DEBUG_NAME(vertexArray, "Compositor instance pass quad")

			// Done
			return vertexArray;
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::ICompositorInstancePass methods ]
	//[-------------------------------------------------------]
	void CompositorInstancePassQuad::onFillCommandBuffer(const Renderer::IRenderTarget& renderTarget, const CompositorContextData& compositorContextData, Renderer::CommandBuffer& commandBuffer)
	{
		if (!mRenderableManager.getRenderables().empty())
		{
			// Begin debug event
			COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(commandBuffer)

			// Fill command buffer
			mRenderQueue.addRenderablesFromRenderableManager(mRenderableManager);
			mRenderQueue.fillCommandBuffer(renderTarget, static_cast<const CompositorResourcePassQuad&>(getCompositorResourcePass()).getMaterialTechniqueId(), compositorContextData, commandBuffer);

			// End debug event
			COMMAND_END_DEBUG_EVENT(commandBuffer)
		}
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CompositorInstancePassQuad::CompositorInstancePassQuad(const CompositorResourcePassQuad& compositorResourcePassQuad, const CompositorNodeInstance& compositorNodeInstance) :
		ICompositorInstancePass(compositorResourcePassQuad, compositorNodeInstance),
		mRenderQueue(compositorNodeInstance.getCompositorWorkspaceInstance().getIndirectBufferManager(), 0, 0, false, false),
		mMaterialResourceId(getUninitialized<MaterialResourceId>())
	{
		// Sanity checks
		assert(!compositorResourcePassQuad.isMaterialDefinitionMandatory() || isInitialized(compositorResourcePassQuad.getMaterialAssetId()) || isInitialized(compositorResourcePassQuad.getMaterialBlueprintAssetId()));
		assert(!(isInitialized(compositorResourcePassQuad.getMaterialAssetId()) && isInitialized(compositorResourcePassQuad.getMaterialBlueprintAssetId())));

		// Get parent material resource ID and initiate creating the compositor instance pass quad material resource
		MaterialResourceManager& materialResourceManager = compositorNodeInstance.getCompositorWorkspaceInstance().getRendererRuntime().getMaterialResourceManager();
		if (isInitialized(compositorResourcePassQuad.getMaterialAssetId()))
		{
			// Get or load material resource
			MaterialResourceId materialResourceId = getUninitialized<MaterialResourceId>();
			materialResourceManager.loadMaterialResourceByAssetId(compositorResourcePassQuad.getMaterialAssetId(), materialResourceId, this);
		}
		else
		{
			// Get or load material blueprint resource
			const AssetId materialBlueprintAssetId = compositorResourcePassQuad.getMaterialBlueprintAssetId();
			if (isInitialized(materialBlueprintAssetId))
			{
				MaterialResourceId parentMaterialResourceId = materialResourceManager.getMaterialResourceIdByAssetId(materialBlueprintAssetId);
				if (isUninitialized(parentMaterialResourceId))
				{
					parentMaterialResourceId = materialResourceManager.createMaterialResourceByAssetId(materialBlueprintAssetId, materialBlueprintAssetId, compositorResourcePassQuad.getMaterialTechniqueId());
				}
				createMaterialResource(parentMaterialResourceId);
			}
		}
	}

	CompositorInstancePassQuad::~CompositorInstancePassQuad()
	{
		if (isInitialized(mMaterialResourceId))
		{
			// Clear the renderable manager right now so we have no more references to the shared vertex array
			mRenderableManager.getRenderables().clear();

			// Release reference to vertex array object (VAO) shared between all compositor instance pass quad instances
			if (nullptr != ::detail::VertexArrayPtr && 1 == ::detail::VertexArrayPtr->releaseReference())	// +1 for reference to global shared pointer
			{
				::detail::VertexArrayPtr = nullptr;
			}

			// Destroy the material resource the compositor instance pass quad created
			getCompositorNodeInstance().getCompositorWorkspaceInstance().getRendererRuntime().getMaterialResourceManager().destroyMaterialResource(mMaterialResourceId);
		}
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::IResourceListener methods ]
	//[-------------------------------------------------------]
	void CompositorInstancePassQuad::onLoadingStateChange(const IResource& resource)
	{
		assert(resource.getId() == mMaterialResourceId);
		createMaterialResource(resource.getId());
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::CompositorInstancePassQuad methods ]
	//[-------------------------------------------------------]
	void CompositorInstancePassQuad::createMaterialResource(MaterialResourceId parentMaterialResourceId)
	{
		// Sanity checks
		assert(isUninitialized(mMaterialResourceId));
		assert(isInitialized(parentMaterialResourceId));

		// Each compositor instance pass quad must have its own material resource since material property values might vary
		const IRendererRuntime& rendererRuntime = getCompositorNodeInstance().getCompositorWorkspaceInstance().getRendererRuntime();
		MaterialResourceManager& materialResourceManager = rendererRuntime.getMaterialResourceManager();
		mMaterialResourceId = materialResourceManager.createMaterialResourceByCloning(parentMaterialResourceId);

		{ // Set compositor resource pass quad material properties
			const MaterialProperties::SortedPropertyVector& sortedPropertyVector = static_cast<const CompositorResourcePassQuad&>(getCompositorResourcePass()).getMaterialProperties().getSortedPropertyVector();
			if (!sortedPropertyVector.empty())
			{
				MaterialResource& materialResource = materialResourceManager.getById(mMaterialResourceId);
				for (const MaterialProperty& materialProperty : sortedPropertyVector)
				{
					if (materialProperty.isOverwritten())
					{
						materialResource.setPropertyById(materialProperty.getMaterialPropertyId(), materialProperty);
					}
				}
			}
		}

		// Add reference to vertex array object (VAO) shared between all compositor instance pass quad instances
		if (nullptr == ::detail::VertexArrayPtr)
		{
			::detail::VertexArrayPtr = ::detail::createVertexArray(rendererRuntime.getBufferManager());
			assert(nullptr != ::detail::VertexArrayPtr);
		}
		::detail::VertexArrayPtr->addReference();

		// Setup renderable manager, shared between all compositor instance pass quad instances
		mRenderableManager.getRenderables().emplace_back(mRenderableManager, ::detail::VertexArrayPtr, Renderer::PrimitiveTopology::TRIANGLE_LIST, false, 0, 3, materialResourceManager, mMaterialResourceId, getUninitialized<SkeletonResourceId>());
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
