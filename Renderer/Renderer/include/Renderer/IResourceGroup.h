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
#include "Renderer/IResource.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract resource group interface
	*
	*  @note
	*    - A resource group groups resources (Vulkan descriptor set, Direct3D 12 descriptor table)
	*    - A resource group is an instance of a root descriptor table
	*    - Descriptor set comes from Vulkan while root descriptor table comes from Direct3D 12; both APIs have a similar but not identical binding model
	*    - Overview of the binding models of explicit APIs: "Choosing a binding model" - https://github.com/gpuweb/gpuweb/issues/19
	*    - Performance hint: Group resources by binding frequency and set resource groups with a low binding frequency at a low index (e.g. bind a per-pass resource group at index 0)
	*    - Compatibility hint: The number of simultaneous bound resource groups is rather low; try to stick to less or equal to four simultaneous bound resource groups, see http://vulkan.gpuinfo.org/listfeatures.php to check out GPU hardware capabilities
	*    - Compatibility hint: In Direct3D 12, samplers are not allowed in the same descriptor table as CBV/UAV/SRVs, put them into a sampler resource group
	*    - Compatibility hint: In Vulkan, one is usually using a combined image sampler, as a result a sampler resource group doesn't translate to a Vulkan sampler descriptor set.
	*                          Use sampler resource group at the highest binding indices to compensate for this.
	*/
	class IResourceGroup : public IResource
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~IResourceGroup();


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Owner renderer instance
		*/
		inline explicit IResourceGroup(IRenderer& renderer);

		explicit IResourceGroup(const IResourceGroup& source) = delete;
		IResourceGroup& operator =(const IResourceGroup& source) = delete;


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<IResourceGroup> IResourceGroupPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/IResourceGroup.inl"
