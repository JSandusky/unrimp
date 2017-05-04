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
#include "RendererRuntime/Resource/Detail/IResource.h"

#include <glm/fwd.hpp>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	template <class ELEMENT_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS> class PackedElementManager;
	template <class TYPE, class LOADER_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS> class ResourceManagerTemplate;
	class SkeletonResourceLoader;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t SkeletonResourceId;	///< POD skeleton resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Skeleton resource
	*
	*  @note
	*    - Each skeleton must have at least one bone
	*    - Bone data is cache friendly depth-first rolled up, see "Molecular Musings" - "Adventures in data-oriented design – Part 2: Hierarchical data" - https://blog.molecular-matters.com/2013/02/22/adventures-in-data-oriented-design-part-2-hierarchical-data/
	*    - The complete skeleton data is sequential in memory
	*/
	class SkeletonResource : public IResource
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class MeshResourceLoader;
		friend class SkeletonResourceLoader;
		friend PackedElementManager<SkeletonResource, SkeletonResourceId, 2048>;							// Type definition of template class
		friend ResourceManagerTemplate<SkeletonResource, SkeletonResourceLoader, SkeletonResourceId, 2048>;	// Type definition of template class


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline uint8_t getNumberOfBones() const;
		inline const uint8_t* getBoneParentIndices() const;
		inline const uint32_t* getBoneIds() const;
		inline glm::mat4* getLocalBoneMatrices();
		inline const glm::mat4* getLocalBoneMatrices() const;
		inline const glm::mat4* getBoneOffsetMatrices() const;
		inline const glm::mat4* getGlobalBoneMatrices() const;
		inline const glm::mat3x4* getBoneSpaceMatrices() const;
		uint32_t getBoneIndexByBoneId(uint32_t boneId) const;	// Bone IDs = "RendererRuntime::StringId" on bone name, "RendererRuntime::getUninitialized<uint32_t>()" if unknown bone ID
		void localToGlobalPose();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline SkeletonResource();
		inline virtual ~SkeletonResource();
		explicit SkeletonResource(const SkeletonResource&) = delete;
		SkeletonResource& operator=(const SkeletonResource&) = delete;
		inline void clearSkeletonData();

		//[-------------------------------------------------------]
		//[ "RendererRuntime::PackedElementManager" management    ]
		//[-------------------------------------------------------]
		inline void initializeElement(SkeletonResourceId skeletonResourceId);
		inline void deinitializeElement();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		uint8_t		 mNumberOfBones;		///< Number of bones
		// Structure-of-arrays (SoA)
		uint8_t*	 mBoneParentIndices;	///< Cache friendly depth-first rolled up bone parent indices, null pointer only in case of horrible error, free the memory if no longer required
		uint32_t*	 mBoneIds;				///< Cache friendly depth-first rolled up bone IDs ("RendererRuntime::StringId" on bone name), null pointer only in case of horrible error, don't free the memory because it's owned by "mBoneParentIndices"
		glm::mat4*	 mLocalBoneMatrices;	///< Cache friendly depth-first rolled up local bone matrices, null pointer only in case of horrible error, don't free the memory because it's owned by "mBoneParentIndices"
		glm::mat4*	 mBoneOffsetMatrices;	///< Cache friendly depth-first rolled up bone offset matrices (object space to bone space), null pointer only in case of horrible error, don't free the memory because it's owned by "mBoneParentIndices"
		glm::mat4*	 mGlobalBoneMatrices;	///< Cache friendly depth-first rolled up global bone matrices, null pointer only in case of horrible error, don't free the memory because it's owned by "mBoneParentIndices"
		glm::mat3x4* mBoneSpaceMatrices;	///< Cache friendly depth-first rolled up bone space matrices, null pointer only in case of horrible error, don't free the memory because it's owned by "mBoneParentIndices"


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Skeleton/SkeletonResource.inl"
