/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/LightBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/Scene/ISceneResource.h"
#include "RendererRuntime/Resource/Scene/Node/ISceneNode.h"
#include "RendererRuntime/Resource/Scene/Item/LightSceneItem.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		// TODO(co) Add support for persistent mapped buffers. For now, the big picture has to be OK so first focus on that.
		static uint32_t DEFAULT_TEXTURE_BUFFER_NUMBER_OF_BYTES = 64 * 1024;	// 64 KiB
		// static uint32_t DEFAULT_TEXTURE_BUFFER_NUMBER_OF_BYTES = 512 * 1024;	// 512 KiB

		static uint32_t NUMBER_OF_BYTES_PER_LIGHT = sizeof(float) * 4 * 2;


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
	LightBufferManager::LightBufferManager(IRendererRuntime& rendererRuntime) :
		mRendererRuntime(rendererRuntime),
		mTextureBuffer(nullptr),
		mNumberOfLights(0)
	{
		// Create texture buffer instance
		mTextureScratchBuffer.resize(std::min(rendererRuntime.getRenderer().getCapabilities().maximumTextureBufferSize, ::detail::DEFAULT_TEXTURE_BUFFER_NUMBER_OF_BYTES));
		mTextureBuffer = rendererRuntime.getBufferManager().createTextureBuffer(mTextureScratchBuffer.size(), Renderer::TextureFormat::R32G32B32A32F, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
	}

	LightBufferManager::~LightBufferManager()
	{
		mTextureBuffer->releaseReference();
	}

	void LightBufferManager::fillBuffer(ISceneResource& sceneResource, Renderer::CommandBuffer& commandBuffer)
	{
		// TODO(co) This is just a placeholder implementation until "RendererRuntime::LightBufferManager" is ready (containing e.g. reasonable optimizations)

		// Loop through all scene nodes and look for point and spot lights
		mNumberOfLights = 0;
		float* scratchBufferPointer = reinterpret_cast<float*>(mTextureScratchBuffer.data());
		for (const RendererRuntime::ISceneNode* sceneNode : sceneResource.getSceneNodes())
		{
			// Loop through all scene items attached to the current scene node
			for (const RendererRuntime::ISceneItem* sceneItem : sceneNode->getAttachedSceneItems())
			{
				if (sceneItem->getSceneItemTypeId() == RendererRuntime::LightSceneItem::TYPE_ID)
				{
					const RendererRuntime::LightSceneItem* lightSceneItem = static_cast<const RendererRuntime::LightSceneItem*>(sceneItem);
					if (lightSceneItem->getLightType() != RendererRuntime::LightSceneItem::LightType::DIRECTIONAL && lightSceneItem->isVisible())
					{
						++mNumberOfLights;

						// xyz position
						memcpy(scratchBufferPointer, glm::value_ptr(sceneNode->getTransform().position), sizeof(float) * 3);
						scratchBufferPointer += 3;

						// Radius
						*scratchBufferPointer = lightSceneItem->getRadius();
						++scratchBufferPointer;

						// rgb color
						memcpy(scratchBufferPointer, glm::value_ptr(lightSceneItem->getColor()), sizeof(float) * 3);
						scratchBufferPointer += 3;

						// Padding
						++scratchBufferPointer;
					}
				}
			}
		}

		// Update the texture buffer by using our scratch buffer
		// TODO(co) This is just a dummy implementation, so full texture buffer size to stress the command buffer
		if (0 != mNumberOfLights)
		{
			Renderer::Command::CopyTextureBufferData::create(commandBuffer, mTextureBuffer, mNumberOfLights * ::detail::NUMBER_OF_BYTES_PER_LIGHT, mTextureScratchBuffer.data());
		}
	}

	void LightBufferManager::fillCommandBuffer(const MaterialBlueprintResource& materialBlueprintResource, Renderer::CommandBuffer& commandBuffer)
	{
		// Light texture buffer
		const MaterialBlueprintResource::TextureBuffer* lightTextureBuffer = materialBlueprintResource.getLightTextureBuffer();
		if (nullptr != lightTextureBuffer)
		{
			Renderer::Command::SetGraphicsRootDescriptorTable::create(commandBuffer, lightTextureBuffer->rootParameterIndex, mTextureBuffer);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
