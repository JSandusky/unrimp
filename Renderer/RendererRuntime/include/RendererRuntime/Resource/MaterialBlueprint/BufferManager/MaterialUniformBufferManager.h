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
#include "RendererRuntime/Core/NonCopyable.h"

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IUniformBuffer;
	class IBufferManager;
}
namespace RendererRuntime
{
	class IRendererRuntime;
	class MaterialUniformBufferSlot;
	class MaterialBlueprintResource;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Material uniform buffer manager
	*
	*  @note
	*    - For material batching
	*    - Concept basing on OGRE 2.1 "Ogre::ConstBufferPool", but more generic and simplified thanks to the material blueprint concept
	*/
	class MaterialUniformBufferManager : public NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] rendererRuntime
		*    Renderer runtime instance to use
		*  @param[in] materialBlueprintResource
		*    Material blueprint resource
		*/
		MaterialUniformBufferManager(IRendererRuntime& rendererRuntime, const MaterialBlueprintResource& materialBlueprintResource);

		/**
		*  @brief
		*    Destructor
		*/
		~MaterialUniformBufferManager();

		/**
		*  @brief
		*    Request a slot and fill the material slot; automatically schedules for update
		*/
		void requestSlot(MaterialUniformBufferSlot& materialUniformBufferSlot);

		/**
		*  @brief
		*    Release a slot requested with "RendererRuntime::MaterialUniformBufferManager::requestSlot()"
		*/
		void releaseSlot(MaterialUniformBufferSlot& materialUniformBufferSlot);

		/**
		*  @brief
		*    Schedule the slot of the given material slot for update
		*/
		void scheduleForUpdate(MaterialUniformBufferSlot& materialUniformBufferSlot);

		/**
		*  @brief
		*    Reset last bound pool and update the dirty slots
		*/
		void resetLastBoundPool();

		/**
		*  @brief
		*    Bind slot to renderer
		*
		*  @param[in] rendererRuntime
		*    Renderer runtime to use
		*  @param[in] materialUniformBufferSlot
		*    Slot to bind
		*/
		void bindToRenderer(const IRendererRuntime& rendererRuntime, MaterialUniformBufferSlot& materialUniformBufferSlot);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		MaterialUniformBufferManager(const MaterialUniformBufferManager&) = delete;
		MaterialUniformBufferManager& operator=(const MaterialUniformBufferManager&) = delete;
		void uploadDirtySlots();


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		struct BufferPool
		{
			std::vector<uint32_t>	  freeSlots;
			Renderer::IUniformBuffer* uniformBuffer;	///< Memory is managed by this buffer pool instance

			BufferPool(size_t bufferSize, uint32_t slotsPerPool, Renderer::IBufferManager& bufferManager);
			~BufferPool();
		};

		typedef std::vector<BufferPool*>				BufferPools;
		typedef std::vector<MaterialUniformBufferSlot*>	MaterialUniformBufferSlots;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&				 mRendererRuntime;
		const MaterialBlueprintResource& mMaterialBlueprintResource;
		BufferPools						 mBufferPools;
		uint32_t						 mSlotsPerPool;
		size_t							 mBufferSize;
		MaterialUniformBufferSlots		 mDirtyMaterialUniformBufferSlots;
		MaterialUniformBufferSlots		 mMaterialUniformBufferSlots;
		const BufferPool*				 mLastBoundPool;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
