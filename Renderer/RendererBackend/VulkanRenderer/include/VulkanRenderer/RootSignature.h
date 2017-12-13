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
#include <Renderer/IRootSignature.h>
#include <Renderer/RootSignatureTypes.h>

#include "VulkanRenderer/Vulkan.h"
#include "VulkanRenderer/Helper.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from 'long' to 'unsigned int', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <vector>
PRAGMA_WARNING_POP


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
	*    Vulkan root signature ("pipeline layout" in Vulkan terminology) class
	*/
	class RootSignature : public Renderer::IRootSignature
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] vulkanRenderer
		*    Owner Vulkan renderer instance
		*  @param[in] rootSignature
		*    Root signature to use
		*/
		RootSignature(VulkanRenderer& vulkanRenderer, const Renderer::RootSignature& rootSignature);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~RootSignature() override;

		/**
		*  @brief
		*    Return the root signature data
		*
		*  @return
		*    The root signature data
		*/
		inline const Renderer::RootSignature& getRootSignature() const;

		/**
		*  @brief
		*    Return the Vulkan pipeline layout
		*
		*  @return
		*    The Vulkan pipeline layout
		*/
		inline VkPipelineLayout getVkPipelineLayout() const;

		/**
		*  @brief
		*    Return the Vulkan descriptor pool
		*
		*  @return
		*    The Vulkan descriptor pool
		*/
		inline VkDescriptorPool getVkDescriptorPool() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		DECLARE_SET_DEBUG_NAME	// virtual void setDebugName(const char* name) override;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRootSignature methods       ]
	//[-------------------------------------------------------]
	public:
		virtual Renderer::IResourceGroup* createResourceGroup(uint32_t rootParameterIndex, uint32_t numberOfResources, Renderer::IResource** resources, Renderer::ISamplerState** samplerStates = nullptr) override;


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	protected:
		virtual void selfDestruct() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit RootSignature(const RootSignature& source) = delete;
		RootSignature& operator =(const RootSignature& source) = delete;


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::vector<VkDescriptorSetLayout> VkDescriptorSetLayouts;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Renderer::RootSignature	mRootSignature;
		VkDescriptorSetLayouts	mVkDescriptorSetLayouts;
		VkPipelineLayout		mVkPipelineLayout;
		VkDescriptorPool		mVkDescriptorPool;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "VulkanRenderer/RootSignature.inl"
