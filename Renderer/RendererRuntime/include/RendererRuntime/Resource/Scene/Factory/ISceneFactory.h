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
#include "RendererRuntime/Core/StringId.h"
#include "RendererRuntime/Core/NonCopyable.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class Transform;
	class ISceneNode;
	class ISceneItem;
	class ISceneResource;
	class IRendererRuntime;
	class IResourceListener;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t ResourceId;			///< POD resource identifier
	typedef StringId SceneNodeTypeId;		///< Scene node type identifier, internally just a POD "uint32_t"
	typedef StringId SceneItemTypeId;		///< Scene item type identifier, internally just a POD "uint32_t"
	typedef StringId SceneResourceTypeId;	///< Scene resource type identifier, internally just a POD "uint32_t"


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class ISceneFactory : protected NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class SceneResourceManager;	// Needs to create scene resource instances
		friend class ISceneResource;		// Needs to create scene node and scene item instances


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::ISceneFactory methods ]
	//[-------------------------------------------------------]
	protected:
		virtual ISceneResource* createSceneResource(SceneResourceTypeId sceneResourceTypeId, IRendererRuntime& rendererRuntime, ResourceId resourceId) const = 0;
		virtual ISceneNode* createSceneNode(SceneNodeTypeId sceneNodeTypeId, const Transform& transform) const = 0;
		virtual ISceneItem* createSceneItem(const SceneItemTypeId& sceneItemTypeId, ISceneResource& sceneResource) const = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline ISceneFactory();
		inline virtual ~ISceneFactory();
		ISceneFactory(const ISceneFactory&) = delete;
		ISceneFactory& operator=(const ISceneFactory&) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Scene/Factory/ISceneFactory.inl"
