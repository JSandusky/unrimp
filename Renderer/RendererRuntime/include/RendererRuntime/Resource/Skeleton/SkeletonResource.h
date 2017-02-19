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

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4201)	// warning C4201: nonstandard extension used: nameless struct/union
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4324)	// warning C4324: '<x>': structure was padded due to alignment specifier
	#include <glm/fwd.hpp>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class SkeletonResource;
	template <class ELEMENT_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS> class PackedElementManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t														 SkeletonResourceId;	///< POD skeleton resource identifier
	typedef PackedElementManager<SkeletonResource, SkeletonResourceId, 2048> SkeletonResources;


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
		friend SkeletonResources;	// Type definition of template class
		friend class MeshResourceLoader;
		friend class SkeletonResourceLoader;
		friend class SkeletonResourceManager;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline uint8_t getNumberOfBones() const;
		inline const uint8_t* getBoneParents() const;
		inline const uint32_t* getBoneIds() const;
		inline const glm::mat4* getLocalBonePoses() const;
		inline const glm::mat4* getGlobalBonePoses() const;
		uint32_t getBoneIndexByBoneId(uint32_t boneId) const;	// Bone IDs = "RendererRuntime::StringId" on bone name, "RendererRuntime::getUninitialized<uint32_t>()" if unknown bone ID
		void localToGlobalPose();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline SkeletonResource();
		inline virtual ~SkeletonResource();
		SkeletonResource(const SkeletonResource&) = delete;
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
		uint8_t		mNumberOfBones;		///< Number of bones
		// Structure-of-arrays (SoA)
		uint8_t*	mBoneParents;		///< Cache friendly depth-first rolled up bone parents, null pointer only in case of horrible error, free the memory if no longer required
		uint32_t*	mBoneIds;			///< Cache friendly depth-first rolled up bone IDs ("RendererRuntime::StringId" on bone name), null pointer only in case of horrible error, don't free the memory because it's owned by "mBoneParents"
		glm::mat4*	mLocalBonePoses;	///< Cache friendly depth-first rolled up local bone poses, null pointer only in case of horrible error, don't free the memory because it's owned by "mBoneParents"
		glm::mat4*	mGlobalBonePoses;	///< Cache friendly depth-first rolled up global bone poses, null pointer only in case of horrible error, don't free the memory because it's owned by "mBoneParents"


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Skeleton/SkeletonResource.inl"
