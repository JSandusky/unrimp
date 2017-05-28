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
#include <Renderer/PlatformTypes.h>

#include "VulkanRenderer/VulkanRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{
	class VulkanRenderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract Vulkan context interface
	*
	*  @remarks
	*    While the Vulkan specification is platform independent, creating an Vulkan context is not. // TODO(co) Check this statement, copied over from OpenGL
	*
	*  @note
	*    - Every native OS window needs its own context instance
	*/
	class IContext
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		virtual ~IContext();

		/**
		*  @brief
		*    Return the Vulkan physical device this context is using
		*
		*  @return
		*    The Vulkan physical device this context is using
		*/
		inline VkPhysicalDevice getVkPhysicalDevice() const;

		/**
		*  @brief
		*    Return the Vulkan device this context is using
		*
		*  @return
		*    The Vulkan device this context is using
		*/
		inline VkDevice getVkDevice() const;

		/**
		*  @brief
		*    Return the handle to the Vulkan device graphics queue that command buffers are submitted to
		*
		*  @return
		*    Handle to the Vulkan device graphics queue that command buffers are submitted to
		*/
		inline VkQueue getGraphicsVkQueue() const;

		/**
		*  @brief
		*    Return the used Vulkan command buffer pool instance
		*
		*  @return
		*    The used Vulkan command buffer pool instance
		*/
		inline VkCommandPool getVkCommandPool() const;

		/**
		*  @brief
		*    Return the Vulkan command buffer instance used for setup
		*
		*  @return
		*    The Vulkan command buffer instance used for setup
		*/
		inline VkCommandBuffer getSetupVkCommandBuffer() const;

		/**
		*  @brief
		*    Flush the used Vulkan command buffer instance
		*/
		void flushSetupVkCommandBuffer() const;


	//[-------------------------------------------------------]
	//[ Public virtual IContext methods                       ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return whether or not the content is initialized
		*
		*  @return
		*    "true" if the context is initialized, else "false"
		*/
		virtual bool isInitialized() const;

		/**
		*  @brief
		*    Make the context current
		*/
		virtual void makeCurrent() const = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] vulkanRenderer
		*    Owner Vulkan renderer instance
		*/
		explicit IContext(VulkanRenderer& vulkanRenderer);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		explicit IContext(const IContext& source) = delete;

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		IContext& operator =(const IContext& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		VkPhysicalDevice mVkPhysicalDevice;		///< Vulkan physical device this context is using
		VkDevice		 mVkDevice;				///< Vulkan device instance this context is using (equivalent of a OpenGL context or Direct3D 11 device)
		VkQueue			 mGraphicsVkQueue;		///< Handle to the Vulkan device graphics queue that command buffers are submitted to
		VkCommandPool	 mVkCommandPool;		///< Vulkan command buffer pool instance
		VkCommandBuffer  mSetupVkCommandBuffer;	///< Vulkan command buffer instance used for setup


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "VulkanRenderer/IContext.inl"
