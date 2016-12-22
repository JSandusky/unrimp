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
#include "RendererRuntime/Resource/CompositorNode/Pass/Quad/CompositorInstancePassQuad.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Quad/CompositorResourcePassQuad.h"
#include "RendererRuntime/Resource/CompositorNode/CompositorNodeInstance.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceInstance.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/RenderQueue/RenderableManager.h"
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
		static Renderer::IVertexArrayPtr		  VertexArrayPtr;		///< Vertex array object (VAO), can be a null pointer, shared between all compositor instance pass quad instances
		static RendererRuntime::RenderableManager RenderableManager;	///< Renderable manager, shared between all compositor instance pass quad instances


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
			const Renderer::VertexAttributes vertexAttributes(glm::countof(vertexAttributesLayout), vertexAttributesLayout);

			// Create the vertex buffer object (VBO)
			// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
			static const float VERTEX_POSITION[] =
			{								// Vertex ID	Triangle on screen
				-1.0f,  1.0f, 0.0f, 0.0f,	// 0			  0.......1
				 3.0f,  1.0f, 2.0f, 0.0f,	// 1			  .   .
				-1.0f, -3.0f, 0.0f, 2.0f	// 2			  2
			};
			Renderer::IVertexBufferPtr vertexBuffer(bufferManager.createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

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
			return bufferManager.createVertexArray(vertexAttributes, glm::countof(vertexArrayVertexBuffers), vertexArrayVertexBuffers);
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
	void CompositorInstancePassQuad::onFillCommandBuffer(const Renderer::IRenderTarget& renderTarget, Renderer::CommandBuffer& commandBuffer)
	{
		mRenderQueue.addRenderablesFromRenderableManager(::detail::RenderableManager);
		mRenderQueue.fillCommandBuffer(renderTarget, commandBuffer);
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
		assert(isInitialized(compositorResourcePassQuad.getMaterialAssetId()) || isInitialized(compositorResourcePassQuad.getMaterialBlueprintAssetId()));
		assert(!(isInitialized(compositorResourcePassQuad.getMaterialAssetId()) && isInitialized(compositorResourcePassQuad.getMaterialBlueprintAssetId())));

		// Get material resource
		const IRendererRuntime& rendererRuntime = compositorNodeInstance.getCompositorWorkspaceInstance().getRendererRuntime();
		MaterialResourceManager& materialResourceManager = rendererRuntime.getMaterialResourceManager();
		if (isInitialized(compositorResourcePassQuad.getMaterialAssetId()))
		{
			// Get or load material resource
			mMaterialResourceId = materialResourceManager.loadMaterialResourceByAssetId(compositorResourcePassQuad.getMaterialAssetId());
		}
		else
		{
			// Get or create material resource
			const AssetId materialBlueprintAssetId = compositorResourcePassQuad.getMaterialBlueprintAssetId();
			mMaterialResourceId = materialResourceManager.getMaterialResourceIdByAssetId(materialBlueprintAssetId);
			if (isUninitialized(mMaterialResourceId))
			{
				mMaterialResourceId = materialResourceManager.createMaterialResourceByAssetId(materialBlueprintAssetId, materialBlueprintAssetId);
			}
		}

		{ // Set compositor resource pass quad material properties
			// TODO(co) "CompositorInstancePassQuad": Probably need to ensure that each pass has it's own material instance with own material property values
			const MaterialProperties::SortedPropertyVector& sortedPropertyVector = compositorResourcePassQuad.getMaterialProperties().getSortedPropertyVector();
			if (!sortedPropertyVector.empty())
			{
				MaterialResource& materialResource = materialResourceManager.getMaterialResources().getElementById(mMaterialResourceId);
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

			// Setup renderable manager, shared between all compositor instance pass quad instances
			assert(::detail::RenderableManager.getRenderables().empty());
			::detail::RenderableManager.getRenderables().emplace_back(::detail::RenderableManager, ::detail::VertexArrayPtr, Renderer::PrimitiveTopology::TRIANGLE_LIST, false, 0, 3, materialResourceManager, mMaterialResourceId);
		}
		::detail::VertexArrayPtr->addReference();
	}

	CompositorInstancePassQuad::~CompositorInstancePassQuad()
	{
		// Release reference to vertex array object (VAO) shared between all compositor instance pass quad instances
		assert(nullptr != ::detail::VertexArrayPtr);
		if (2 == ::detail::VertexArrayPtr->releaseReference())	// 2 due to +1 for global shared pointer and +1 for renderable shared pointer
		{
			{ // Clear renderable manager
				RenderableManager::Renderables& renderables = ::detail::RenderableManager.getRenderables();
				renderables.clear();

				// Try to free memory allocated by the renderable manager so memory leak functions like "_CrtMemDumpAllObjectsSince()" from Windows don't report false-positives
				// -> Just a request, the Visual Studio 2015 STD implementation fulfills it
				renderables.shrink_to_fit();
			}
			::detail::VertexArrayPtr = nullptr;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
