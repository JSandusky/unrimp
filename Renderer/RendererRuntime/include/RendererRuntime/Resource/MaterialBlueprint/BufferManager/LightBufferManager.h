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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/Manager.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class CommandBuffer;
	class ITextureBuffer;
}
namespace RendererRuntime
{
	class ISceneResource;
	class IRendererRuntime;
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
	*    Light buffer manager
	*/
	class LightBufferManager : private Manager
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
		explicit LightBufferManager(IRendererRuntime& rendererRuntime);

		/**
		*  @brief
		*    Destructor
		*/
		~LightBufferManager();

		/**
		*  @brief
		*    Fill the light buffer
		*
		*  @param[in] sceneResource
		*    Scene resource to use
		*  @param[out] commandBuffer
		*    Command buffer to fill
		*/
		void fillBuffer(ISceneResource& sceneResource, Renderer::CommandBuffer& commandBuffer);

		/**
		*  @brief
		*    Return the current number of recorded lights inside the texture buffer
		*
		*  @return
		*    The current number of recorded lights inside the texture buffer
		*/
		inline uint32_t getNumberOfLights() const;

		/**
		*  @brief
		*    Bind the light buffer manager into the given commando buffer
		*
		*  @param[in] materialBlueprintResource
		*    Material blueprint resource
		*  @param[out] commandBuffer
		*    Command buffer to fill
		*/
		void fillCommandBuffer(const MaterialBlueprintResource& materialBlueprintResource, Renderer::CommandBuffer& commandBuffer);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		LightBufferManager(const LightBufferManager&) = delete;
		LightBufferManager& operator=(const LightBufferManager&) = delete;


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::vector<uint8_t> ScratchBuffer;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&		  mRendererRuntime;	///< Renderer runtime instance to use
		Renderer::ITextureBuffer* mTextureBuffer;	///< Texture buffer instance, always valid
		ScratchBuffer			  mTextureScratchBuffer;
		uint32_t				  mNumberOfLights;	///< Current number of recorded lights inside the texture buffer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/LightBufferManager.inl"
