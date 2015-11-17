/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "RendererRuntime/Export.h"
#include "RendererRuntime/Resource/IResource.h"
#include "RendererRuntime/Core/Manager.h"

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class Transform;
	class ISceneNode;
	class ISceneItem;
	class ISceneFactory;
	class IRendererRuntime;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId SceneItemTypeId;		///< Scene item type identifier, internally just a POD "uint32_t"
	typedef StringId SceneResourceTypeId;	///< Scene resource type identifier, internally just a POD "uint32_t"


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class ISceneResource : public IResource
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class SceneResourceManager;	// Needs to be able to update the scene factory instance


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::vector<ISceneNode*> SceneNodes;
		typedef std::vector<ISceneItem*> SceneItems;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		virtual ~ISceneResource();
		inline const IRendererRuntime& getRendererRuntime() const;
		inline void destroyAllSceneNodesAndItems();

		//[-------------------------------------------------------]
		//[ Node                                                  ]
		//[-------------------------------------------------------]
		RENDERERRUNTIME_API_EXPORT ISceneNode* createSceneNode(const Transform& transform);
		RENDERERRUNTIME_API_EXPORT void destroySceneNode(ISceneNode& sceneNode);
		RENDERERRUNTIME_API_EXPORT void destroyAllSceneNodes();
		inline const SceneNodes& getSceneNodes() const;

		//[-------------------------------------------------------]
		//[ Item                                                  ]
		//[-------------------------------------------------------]
		RENDERERRUNTIME_API_EXPORT ISceneItem* createSceneItem(SceneItemTypeId sceneItemTypeId, ISceneNode& sceneNode);
		template <typename T> T* createSceneItem(ISceneNode& sceneNode);
		RENDERERRUNTIME_API_EXPORT void destroySceneItem(ISceneItem& sceneItem);
		RENDERERRUNTIME_API_EXPORT void destroyAllSceneItems();
		inline const SceneItems& getSceneItems() const;


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneResource methods        ]
	//[-------------------------------------------------------]
	public:
		virtual SceneResourceTypeId getSceneResourceTypeId() const = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		ISceneResource(IRendererRuntime& rendererRuntime, ResourceId resourceId, IResourceListener* resourceListener = nullptr);
		ISceneResource(const ISceneResource&) = delete;
		ISceneResource& operator=(const ISceneResource&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&	 mRendererRuntime;	///< Renderer runtime instance, do not destroy the instance
		const ISceneFactory* mSceneFactory;		///< Scene factory instance, always valid, do not destroy the instance
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
#include "RendererRuntime/Resource/Scene/ISceneResource.inl"
