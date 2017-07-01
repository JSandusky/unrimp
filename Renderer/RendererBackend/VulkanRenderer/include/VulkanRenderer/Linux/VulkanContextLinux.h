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
#include "VulkanRenderer/IVulkanContext.h"

#include <Renderer/PlatformTypes.h>
#include <Renderer/LinuxHeader.h>


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
	*    Linux Vulkan context class
	*/
	class VulkanContextLinux : public IVulkanContext
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] nativeWindowHandle
		*    Optional native main window handle, can be a null handle
		*  @param[in] useExternalContext
		*    When true an own Vulkan context won't be created
		*/
		VulkanContextLinux(handle nativeWindowHandle, bool useExternalContext);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VulkanContextLinux();

		/**
		*  @brief
		*    Return the primary device context
		*
		*  @return
		*    The primary device context, null pointer on error
		*/
		inline Display *getDisplay() const;

		/**
		*  @brief
		*    Return the primary render context
		*
		*  @return
		*    The primary render context, null pointer on error
		*/
		inline GLXContext getRenderContext() const;


	//[-------------------------------------------------------]
	//[ Public virtual VulkanRenderer::IVulkanContext methods ]
	//[-------------------------------------------------------]
	public:
		inline virtual bool isInitialized() const override;
		virtual void makeCurrent() const override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		handle		 mNativeWindowHandle;	///< Vulkan window, can be a null pointer (Window)
		handle		 mDummyWindow;			///< Vulkan dummy window, can be a null pointer (Window)
		Display		*mDisplay;				///< The device context of the Vulkan dummy window, can be a null pointer
		XVisualInfo *m_pDummyVisualInfo;
		bool		 mUseExternalContext;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "VulkanRenderer/Linux/VulkanContextLinux.inl"
