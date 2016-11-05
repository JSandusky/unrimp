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
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class Transform;
	class IRendererRuntime;
	class MaterialTechnique;
	class PassUniformBufferManager;
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
	*    Instance uniform buffer manager
	*/
	class InstanceUniformBufferManager : public NonCopyable
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
		explicit InstanceUniformBufferManager(IRendererRuntime& rendererRuntime);

		/**
		*  @brief
		*    Destructor
		*/
		~InstanceUniformBufferManager();

		/**
		*  @brief
		*    Fill the instance buffer
		*
		*  @param[in] passUniformBufferManager
		*    Pass uniform buffer manager instance to use
		*  @param[in] instanceUniformBuffer
		*    Instance uniform buffer instance to use, can be a null pointer
		*  @param[in] instanceTextureBuffer
		*    Instance texture buffer instance to use, can be a null pointer
		*  @param[in] objectSpaceToWorldSpaceTransform
		*    Object space to world space transform
		*  @param[in] materialTechnique
		*    Used material technique
		*/
		void fillBuffer(PassUniformBufferManager& passUniformBufferManager, const MaterialBlueprintResource::UniformBuffer* instanceUniformBuffer,
						const MaterialBlueprintResource::TextureBuffer* instanceTextureBuffer, const Transform& objectSpaceToWorldSpaceTransform, MaterialTechnique& materialTechnique);

		/**
		*  @brief
		*    Bind instance uniform buffer manager to renderer
		*
		*  @param[in] materialBlueprintResource
		*    Material blueprint resource
		*/
		void bindToRenderer(const MaterialBlueprintResource& materialBlueprintResource);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		InstanceUniformBufferManager(const InstanceUniformBufferManager&) = delete;
		InstanceUniformBufferManager& operator=(const InstanceUniformBufferManager&) = delete;


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
		Renderer::IUniformBuffer* mUniformBuffer;	///< Uniform buffer instance, always valid
		Renderer::ITextureBuffer* mTextureBuffer;	///< Texture buffer instance, always valid
		ScratchBuffer			  mUniformScratchBuffer;
		ScratchBuffer			  mTextureScratchBuffer;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
