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
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/MaterialUniformBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/IMaterialBlueprintResourceListener.h"
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
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Efficient vector remove
		*
		*  @remarks
		*    Used for efficient removal in "std::vector" and "std::deque" (like an std::list). However it assumes the order of
		*    elements in container is not important or something external to the container holds the index of an element in it
		*    (but still should be kept deterministically across machines). Basically it swaps the iterator with the last iterator,
		*    and pops back. Returns the next iterator.
		*/
		template<typename T>
		typename T::iterator efficientVectorRemove(T& container, typename T::iterator& iterator)
		{
			const int index = iterator - container.begin();
			*iterator = container.back();
			container.pop_back();
			return container.begin() + index;
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
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	MaterialUniformBufferManager::MaterialUniformBufferManager(IRendererRuntime& rendererRuntime, const MaterialBlueprintResource& materialBlueprintResource) :
		mRendererRuntime(rendererRuntime),
		mMaterialBlueprintResource(materialBlueprintResource),
		mSlotsPerPool(0),
		mBufferSize(0),
		mLastBoundPool(nullptr)
	{
		MaterialBlueprintResource::UniformBuffer* materialUniformBuffer = mMaterialBlueprintResource.getMaterialUniformBuffer();
		assert(nullptr != materialUniformBuffer);

		// Get the buffer size
		mBufferSize = std::min<size_t>(rendererRuntime.getRenderer().getCapabilities().maximumUniformBufferSize, 64 * 1024);

		// Calculate the number of slots per pool
		const uint32_t numberOfBytesPerElement = materialUniformBuffer->uniformBufferNumberOfBytes / materialUniformBuffer->numberOfElements;
		mSlotsPerPool = mBufferSize / numberOfBytesPerElement;
	}

	MaterialUniformBufferManager::~MaterialUniformBufferManager()
	{
		for (BufferPool* bufferPool : mBufferPools)
		{
			delete bufferPool;
		}
	}

	void MaterialUniformBufferManager::requestSlot(MaterialUniformBufferSlot& materialUniformBufferSlot)
	{
		// Release slot, if required
		if (isInitialized(materialUniformBufferSlot.mAssignedMaterialPool))
		{
			releaseSlot(materialUniformBufferSlot);
		}

		// Find a buffer pool with a free slot
		BufferPools::iterator iterator = mBufferPools.begin();
		BufferPools::iterator iteratorEnd  = mBufferPools.end();
		while (iterator != iteratorEnd && (*iterator)->freeSlots.empty())
		{
			++iterator;
		}
		if (iterator == iteratorEnd)
		{
			mBufferPools.push_back(new BufferPool(mBufferSize, mSlotsPerPool, mRendererRuntime.getBufferManager()));
			iterator = mBufferPools.end() - 1;
		}

		// Setup received slot
		BufferPool* bufferPool = *iterator;
		materialUniformBufferSlot.mAssignedMaterialPool = bufferPool;
		materialUniformBufferSlot.mAssignedMaterialSlot = bufferPool->freeSlots.back();
		materialUniformBufferSlot.mGlobalIndex			= static_cast<int>(mMaterialUniformBufferSlots.size());
		mMaterialUniformBufferSlots.push_back(&materialUniformBufferSlot);
		bufferPool->freeSlots.pop_back();
		scheduleForUpdate(materialUniformBufferSlot);
	}

	void MaterialUniformBufferManager::releaseSlot(MaterialUniformBufferSlot& materialUniformBufferSlot)
	{
		BufferPool* bufferPool = static_cast<BufferPool*>(materialUniformBufferSlot.mAssignedMaterialPool);

		// Sanity checks
		assert(isInitialized(materialUniformBufferSlot.mAssignedMaterialPool));
		assert(materialUniformBufferSlot.mAssignedMaterialSlot < mSlotsPerPool);
		assert(std::find(bufferPool->freeSlots.begin(), bufferPool->freeSlots.end(), materialUniformBufferSlot.mAssignedMaterialSlot) == bufferPool->freeSlots.end());
		assert(materialUniformBufferSlot.mGlobalIndex < static_cast<int>(mMaterialUniformBufferSlots.size()));
		assert(&materialUniformBufferSlot == *(mMaterialUniformBufferSlots.begin() + materialUniformBufferSlot.mGlobalIndex));

		// If the slot is dirty, remove it from the list of dirty slots
		if (materialUniformBufferSlot.mDirty)
		{
			MaterialUniformBufferSlots::iterator iterator = std::find(mDirtyMaterialUniformBufferSlots.begin(), mDirtyMaterialUniformBufferSlots.end(), &materialUniformBufferSlot);
			if (iterator != mDirtyMaterialUniformBufferSlots.end())
			{
				::detail::efficientVectorRemove(mDirtyMaterialUniformBufferSlots, iterator);
			}
		}

		// Put the slot back to the list of free slots
		bufferPool->freeSlots.push_back(materialUniformBufferSlot.mAssignedMaterialSlot);
		materialUniformBufferSlot.mAssignedMaterialPool = nullptr;
		materialUniformBufferSlot.mAssignedMaterialSlot = getUninitialized<uint32_t>();
		materialUniformBufferSlot.mDirty				= false;
		MaterialUniformBufferSlots::iterator iterator = mMaterialUniformBufferSlots.begin() + materialUniformBufferSlot.mGlobalIndex;
		iterator = ::detail::efficientVectorRemove(mMaterialUniformBufferSlots, iterator);
		if (iterator != mMaterialUniformBufferSlots.end())
		{
			// The node that was at the end got swapped and has now a different index
			(*iterator)->mGlobalIndex = iterator - mMaterialUniformBufferSlots.begin();
		}
	}

	void MaterialUniformBufferManager::scheduleForUpdate(MaterialUniformBufferSlot& materialUniformBufferSlot)
	{
		if (!materialUniformBufferSlot.mDirty)
		{
			mDirtyMaterialUniformBufferSlots.push_back(&materialUniformBufferSlot);
			materialUniformBufferSlot.mDirty = true;
		}
	}

	void MaterialUniformBufferManager::resetLastBoundPool()
	{
		mLastBoundPool = nullptr;
		if (!mDirtyMaterialUniformBufferSlots.empty())
		{
			uploadDirtySlots();
		}
	}

	void MaterialUniformBufferManager::bindToRenderer(const IRendererRuntime& rendererRuntime, MaterialUniformBufferSlot& materialUniformBufferSlot)
	{
		if (mLastBoundPool != materialUniformBufferSlot.mAssignedMaterialPool)
		{
			mLastBoundPool = static_cast<BufferPool*>(materialUniformBufferSlot.mAssignedMaterialPool);
			MaterialBlueprintResource::UniformBuffer* materialUniformBuffer = mMaterialBlueprintResource.getMaterialUniformBuffer();
			assert(nullptr != materialUniformBuffer);
			rendererRuntime.getRenderer().setGraphicsRootDescriptorTable(materialUniformBuffer->rootParameterIndex, mLastBoundPool->uniformBuffer);
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void MaterialUniformBufferManager::uploadDirtySlots()
	{
		assert(!mDirtyMaterialUniformBufferSlots.empty());
		MaterialBlueprintResource::UniformBuffer* materialUniformBuffer = mMaterialBlueprintResource.getMaterialUniformBuffer();
		assert(nullptr != materialUniformBuffer);
		const MaterialBlueprintResourceManager& materialBlueprintResourceManager = mMaterialBlueprintResource.getResourceManager<MaterialBlueprintResourceManager>();
		const MaterialProperties& globalMaterialProperties = materialBlueprintResourceManager.getGlobalMaterialProperties();
		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = materialBlueprintResourceManager.getMaterialBlueprintResourceListener();
		materialBlueprintResourceListener.beginFillMaterial();

		// Update the scratch buffer
		Renderer::IUniformBuffer* uniformBuffer = nullptr;	// TODO(co) Implement proper uniform buffer handling and only update dirty sections
		MaterialBlueprintResource::ScratchBuffer& scratchBuffer = materialUniformBuffer->scratchBuffer;
		{
			const MaterialBlueprintResource::UniformBufferElementProperties& uniformBufferElementProperties = materialUniformBuffer->uniformBufferElementProperties;
			const size_t numberOfUniformBufferElementProperties = uniformBufferElementProperties.size();
			const uint32_t numberOfBytesPerElement = materialUniformBuffer->uniformBufferNumberOfBytes / materialUniformBuffer->numberOfElements;
			for (MaterialUniformBufferSlot* materialUniformBufferSlot : mDirtyMaterialUniformBufferSlots)
			{
				const MaterialResource& materialResource = materialUniformBufferSlot->getMaterialResource();
				uint8_t* scratchBufferPointer = scratchBuffer.data() + numberOfBytesPerElement * materialUniformBufferSlot->mAssignedMaterialSlot;

				// TODO(co) Implement proper uniform buffer handling and only update dirty sections
				uniformBuffer = static_cast<BufferPool*>(materialUniformBufferSlot->mAssignedMaterialPool)->uniformBuffer;

				for (size_t i = 0, numberOfPackageBytes = 0; i < numberOfUniformBufferElementProperties; ++i)
				{
					const MaterialProperty& uniformBufferElementProperty = uniformBufferElementProperties[i];

					// Get value type number of bytes
					const uint32_t valueTypeNumberOfBytes = uniformBufferElementProperty.getValueTypeNumberOfBytes(uniformBufferElementProperty.getValueType());

					// Handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
					if (0 != numberOfPackageBytes && numberOfPackageBytes + valueTypeNumberOfBytes > 16)
					{
						// Move the scratch buffer pointer to the location of the next aligned package and restart the package bytes counter
						scratchBufferPointer += 4 * 4 - numberOfPackageBytes;
						numberOfPackageBytes = 0;
					}
					numberOfPackageBytes += valueTypeNumberOfBytes % 16;

					// Copy the property value into the scratch buffer
					const MaterialProperty::Usage usage = uniformBufferElementProperty.getUsage();
					if (MaterialProperty::Usage::MATERIAL_REFERENCE == usage)	// Most likely the case, so check this first
					{
						// Figure out the material property value
						const MaterialProperty* materialProperty = materialResource.getPropertyById(uniformBufferElementProperty.getReferenceValue());
						if (nullptr != materialProperty)
						{
							// TODO(co) Error handling: Usage mismatch, value type mismatch etc.
							memcpy(scratchBufferPointer, materialProperty->getData(), valueTypeNumberOfBytes);
						}
						else if (!materialBlueprintResourceListener.fillMaterialValue(uniformBufferElementProperty.getReferenceValue(), scratchBufferPointer, valueTypeNumberOfBytes))
						{
							// Error, can't resolve reference
							assert(false);
						}
					}
					else if (MaterialProperty::Usage::GLOBAL_REFERENCE == usage)
					{
						// Referencing a global material property inside a material uniform buffer doesn't make really sense performance wise, but don't forbid it
	
						// Figure out the global material property value
						const MaterialProperty* materialProperty = globalMaterialProperties.getPropertyById(uniformBufferElementProperty.getReferenceValue());
						if (nullptr != materialProperty)
						{
							// TODO(co) Error handling: Usage mismatch, value type mismatch etc.
							memcpy(scratchBufferPointer, materialProperty->getData(), valueTypeNumberOfBytes);
						}
						else
						{
							// Error, can't resolve reference
							assert(false);
						}
					}
					else if (!uniformBufferElementProperty.isReferenceUsage())	// TODO(co) Performance: Think about such tests, the toolkit should already take care of this so we have well known verified runtime data
					{
						// Referencing a static material property inside an material uniform buffer doesn't make really sense performance wise, but don't forbid it

						// Just copy over the property value
						memcpy(scratchBufferPointer, uniformBufferElementProperty.getData(), valueTypeNumberOfBytes);
					}
					else
					{
						// Error, invalid property
						assert(false);
					}

					// Next property
					scratchBufferPointer += valueTypeNumberOfBytes;
				}

				// The material uniform buffer slot is now clean
				materialUniformBufferSlot->mDirty = false;
			}
		}

		// Update the uniform buffer by using our scratch buffer
		if (nullptr != uniformBuffer)
		{
			uniformBuffer->copyDataFrom(scratchBuffer.size(), scratchBuffer.data());
		}

		// Done
		mDirtyMaterialUniformBufferSlots.clear();
	}


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::MaterialUniformBufferManager::BufferPool methods ]
	//[-------------------------------------------------------]
	MaterialUniformBufferManager::BufferPool::BufferPool(size_t bufferSize, uint32_t slotsPerPool, Renderer::IBufferManager& bufferManager) :
		uniformBuffer(bufferManager.createUniformBuffer(bufferSize, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW))
	{
		freeSlots.reserve(slotsPerPool);
		for (uint32_t i = 0; i < slotsPerPool; ++i)
		{
			freeSlots.push_back((slotsPerPool - i) - 1);
		}
	}

	MaterialUniformBufferManager::BufferPool::~BufferPool()
	{
		uniformBuffer->release();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
