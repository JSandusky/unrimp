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
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class Transform;
	class Renderable;
	class IRendererRuntime;
	class MaterialTechnique;
	class PassBufferManager;
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
	*    Instance buffer manager
	*/
	class InstanceBufferManager : private Manager
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
		*/
		explicit InstanceBufferManager(IRendererRuntime& rendererRuntime);

		/**
		*  @brief
		*    Destructor
		*/
		~InstanceBufferManager();

		/**
		*  @brief
		*    Fill the instance buffer
		*
		*  @param[in] passBufferManager
		*    Pass buffer manager instance to use, can be a null pointer
		*  @param[in] instanceUniformBuffer
		*    Instance uniform buffer instance to use
		*  @param[in] renderable
		*    Renderable to fill the buffer for
		*  @param[in] materialTechnique
		*    Used material technique
		*
		*  @return
		*    Start instance location, used for draw ID (see "17/11/2012 Surviving without gl_DrawID" - https://www.g-truc.net/post-0518.html)
		*/
		uint32_t fillBuffer(PassBufferManager* passBufferManager, const MaterialBlueprintResource::UniformBuffer& instanceUniformBuffer, const Renderable& renderable, MaterialTechnique& materialTechnique);

		/**
		*  @brief
		*    Bind the instance buffer manager into the given commando buffer
		*
		*  @param[in] materialBlueprintResource
		*    Material blueprint resource
		*  @param[out] commandBuffer
		*    Command buffer to fill
		*/
		void fillCommandBuffer(const MaterialBlueprintResource& materialBlueprintResource, Renderer::CommandBuffer& commandBuffer);

		/**
		*  @brief
		*    Called pre command buffer execution
		*/
		void onPreCommandBufferExecution();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit InstanceBufferManager(const InstanceBufferManager&) = delete;
		InstanceBufferManager& operator=(const InstanceBufferManager&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&		  mRendererRuntime;			///< Renderer runtime instance to use
		uint32_t				  mStartInstanceLocation;	///< Start instance location, used for draw ID (see "17/11/2012 Surviving without gl_DrawID" - https://www.g-truc.net/post-0518.html)
		// TODO(co) This is just a placeholder implementation until "RendererRuntime::InstanceBufferManager" is ready
		Renderer::IUniformBuffer* mUniformBuffer;			///< Uniform buffer instance, always valid
		Renderer::ITextureBuffer* mTextureBuffer;			///< Texture buffer instance, always valid
		bool					  mMapped;
		uint8_t*				  mStartUniformBufferPointer;
		uint8_t*				  mCurrentUniformBufferPointer;
		float*					  mStartTextureBufferPointer;
		float*					  mCurrentTextureBufferPointer;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
