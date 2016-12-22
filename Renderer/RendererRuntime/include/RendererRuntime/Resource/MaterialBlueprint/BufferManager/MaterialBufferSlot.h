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

#include <inttypes.h>	// For uint32_t, uint64_t etc.


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class MaterialResource;
	class MaterialResourceManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t MaterialResourceId;	///< POD material resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Material buffer slot
	*/
	class MaterialBufferSlot : protected NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class MaterialBufferManager;	// Manages the slot instances


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] materialResource
		*    Owner material resource, only material resource manager and material resource ID will internally be stored
		*/
		explicit MaterialBufferSlot(MaterialResource& materialResource);

		/**
		*  @brief
		*    Destructor
		*/
		inline ~MaterialBufferSlot();

		/**
		*  @brief
		*    Return the owner material resource manager
		*
		*  @return
		*    The owner material resource manager
		*/
		inline MaterialResourceManager& getMaterialResourceManager() const;

		/**
		*  @brief
		*    Return the owner material resource ID
		*
		*  @return
		*    The owner material resource ID
		*/
		inline MaterialResourceId getMaterialResourceId() const;

		/**
		*  @brief
		*    Return the owner material resource instance
		*
		*  @return
		*    The owner material resource instance
		*
		*  @note
		*    - Ease of use method
		*/
		const MaterialResource& getMaterialResource() const;

		/**
		*  @brief
		*    Return the assigned material slot
		*
		*  @return
		*    The assigned material slot
		*/
		inline uint32_t getAssignedMaterialSlot() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		MaterialBufferSlot(const MaterialBufferSlot&) = delete;
		MaterialBufferSlot& operator=(const MaterialBufferSlot&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		MaterialResourceManager* mMaterialResourceManager;	///< Owner material resource manager, always valid
		MaterialResourceId		 mMaterialResourceId;		///< Owner material resource ID, always valid
		void*					 mAssignedMaterialPool;		///< "RendererRuntime::MaterialBufferManager::BufferPool*", it's a private inner class which we can't forward declare, but we also don't want to expose too much details, so void* it is in here
		uint32_t				 mAssignedMaterialSlot;
		int						 mGlobalIndex;
		bool					 mDirty;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/MaterialBufferSlot.inl"
