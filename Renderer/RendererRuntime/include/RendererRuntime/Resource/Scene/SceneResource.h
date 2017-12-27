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
#include "RendererRuntime/Core/Manager.h"

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
namespace RendererRuntime
{
	class Transform;
	class SceneNode;
	class ISceneItem;
	class ISceneFactory;
	class IRendererRuntime;
	class SceneCullingManager;
	class SceneResourceLoader;
	template <class ELEMENT_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS> class PackedElementManager;
	template <class TYPE, class LOADER_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS> class ResourceManagerTemplate;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t SceneResourceId;	///< POD scene resource identifier
	typedef StringId SceneItemTypeId;	///< Scene item type identifier, internally just a POD "uint32_t"


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class SceneResource : public IResource
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class SceneResourceManager;															// Needs to be able to update the scene factory instance
		friend PackedElementManager<SceneResource, SceneResourceId, 16>;							// Type definition of template class
		friend ResourceManagerTemplate<SceneResource, SceneResourceLoader, SceneResourceId, 16>;	// Type definition of template class


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::vector<SceneNode*> SceneNodes;
		typedef std::vector<ISceneItem*> SceneItems;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT IRendererRuntime& getRendererRuntime() const;
		inline SceneCullingManager& getSceneCullingManager() const;
		inline void destroyAllSceneNodesAndItems();

		//[-------------------------------------------------------]
		//[ Node                                                  ]
		//[-------------------------------------------------------]
		RENDERERRUNTIME_API_EXPORT SceneNode* createSceneNode(const Transform& transform);
		RENDERERRUNTIME_API_EXPORT void destroySceneNode(SceneNode& sceneNode);
		RENDERERRUNTIME_API_EXPORT void destroyAllSceneNodes();
		inline const SceneNodes& getSceneNodes() const;

		//[-------------------------------------------------------]
		//[ Item                                                  ]
		//[-------------------------------------------------------]
		RENDERERRUNTIME_API_EXPORT ISceneItem* createSceneItem(SceneItemTypeId sceneItemTypeId, SceneNode& sceneNode);
		template <typename T> T* createSceneItem(SceneNode& sceneNode);
		RENDERERRUNTIME_API_EXPORT void destroySceneItem(ISceneItem& sceneItem);
		RENDERERRUNTIME_API_EXPORT void destroyAllSceneItems();
		inline const SceneItems& getSceneItems() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline SceneResource();
		inline virtual ~SceneResource() override;
		explicit SceneResource(const SceneResource&) = delete;
		SceneResource& operator=(const SceneResource&) = delete;
		inline SceneResource& operator=(SceneResource&& sceneResource);

		//[-------------------------------------------------------]
		//[ "RendererRuntime::PackedElementManager" management    ]
		//[-------------------------------------------------------]
		void initializeElement(SceneResourceId sceneResourceId);
		void deinitializeElement();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		const ISceneFactory* mSceneFactory;			///< Scene factory instance, always valid, do not destroy the instance
		SceneCullingManager* mSceneCullingManager;	///< Scene culling manager, always valid, destroy the instance if you no longer need it
		SceneNodes			 mSceneNodes;
		SceneItems			 mSceneItems;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Scene/SceneResource.inl"
