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
#include "RendererRuntime/Core/Manager.h"

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class CommandBuffer;
	class IUniformBuffer;
	class IBufferManager;
}
namespace RendererRuntime
{
	class IRendererRuntime;
	class MaterialBufferSlot;
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
	*    Material buffer manager
	*
	*  @note
	*    - For material batching
	*    - Concept basing on OGRE 2.1 "Ogre::ConstBufferPool", but more generic and simplified thanks to the material blueprint concept
	*/
	class MaterialBufferManager : private Manager
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
		MaterialBufferManager(IRendererRuntime& rendererRuntime, const MaterialBlueprintResource& materialBlueprintResource);

		/**
		*  @brief
		*    Destructor
		*/
		~MaterialBufferManager();

		/**
		*  @brief
		*    Request a slot and fill the material slot; automatically schedules for update
		*/
		void requestSlot(MaterialBufferSlot& materialBufferSlot);

		/**
		*  @brief
		*    Release a slot requested with "RendererRuntime::MaterialBufferManager::requestSlot()"
		*/
		void releaseSlot(MaterialBufferSlot& materialBufferSlot);

		/**
		*  @brief
		*    Schedule the slot of the given material slot for update
		*/
		void scheduleForUpdate(MaterialBufferSlot& materialBufferSlot);

		/**
		*  @brief
		*    Reset last bound pool and update the dirty slots
		*/
		void resetLastBoundPool();

		/**
		*  @brief
		*    Fill slot to command buffer
		*
		*  @param[in] materialBufferSlot
		*    Slot to bind
		*  @param[out] commandBuffer
		*    Command buffer to fill
		*
		*  @return
		*    "true" on assigned material pool change, else "false"
		*/
		bool fillCommandBuffer(MaterialBufferSlot& materialBufferSlot, Renderer::CommandBuffer& commandBuffer);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit MaterialBufferManager(const MaterialBufferManager&) = delete;
		MaterialBufferManager& operator=(const MaterialBufferManager&) = delete;
		void uploadDirtySlots();


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		struct BufferPool
		{
			std::vector<uint32_t>	  freeSlots;
			Renderer::IUniformBuffer* uniformBuffer;	///< Memory is managed by this buffer pool instance

			BufferPool(uint32_t bufferSize, uint32_t slotsPerPool, Renderer::IBufferManager& bufferManager);
			~BufferPool();
		};

		typedef std::vector<BufferPool*>		 BufferPools;
		typedef std::vector<MaterialBufferSlot*> MaterialBufferSlots;
		typedef std::vector<uint8_t>			 ScratchBuffer;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&				 mRendererRuntime;
		const MaterialBlueprintResource& mMaterialBlueprintResource;
		BufferPools						 mBufferPools;
		uint32_t						 mSlotsPerPool;
		uint32_t						 mBufferSize;
		MaterialBufferSlots				 mDirtyMaterialBufferSlots;
		MaterialBufferSlots				 mMaterialBufferSlots;
		const BufferPool*				 mLastBoundPool;
		ScratchBuffer					 mScratchBuffer;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
