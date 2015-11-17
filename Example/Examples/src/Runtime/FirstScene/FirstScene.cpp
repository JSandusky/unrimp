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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "PrecompiledHeader.h"
#include "Runtime/FirstScene/FirstScene.h"

#include <RendererRuntime/Core/Transform.h>
#include <RendererRuntime/Resource/Scene/ISceneResource.h>
#include <RendererRuntime/Resource/Scene/SceneResourceManager.h>
#include <RendererRuntime/Resource/Scene/Node/ISceneNode.h>
#include <RendererRuntime/Resource/Scene/Item/MeshSceneItem.h>
#include <RendererRuntime/Resource/Scene/Item/CameraSceneItem.h>
#include <RendererRuntime/Resource/Compositor/CompositorInstance.h>
#include <RendererRuntime/Resource/Compositor/CompositorResourceManager.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstScene::FirstScene(const char *rendererName) :
	IApplicationRendererRuntime(rendererName),
	mCompositorInstance(nullptr),
	mSceneResource(nullptr),
	mCameraSceneItem(nullptr),
	mSceneNode(nullptr),
	mGlobalTimer(0.0f)
{
	// Nothing to do in here
}

FirstScene::~FirstScene()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstScene::onInitialization()
{
	// Call the base implementation
	IApplicationRendererRuntime::onInitialization();

	// Get and check the renderer runtime instance
	RendererRuntime::IRendererRuntimePtr rendererRuntime(getRendererRuntime());
	if (nullptr != rendererRuntime)
	{
		// Get the main swap chain and ensure there's one
		Renderer::ISwapChainPtr swapChain(getRenderer()->getMainSwapChain());
		if (nullptr != swapChain)
		{
			// Create the compositor instance
			mCompositorInstance = new RendererRuntime::CompositorInstance(*rendererRuntime, "Example/Compositor/Default/FirstScene", *swapChain);
		}

		// TODO(co) Just a first scene test
		{ // Build scene
			// Create the scene resource
			mSceneResource = rendererRuntime->getSceneResourceManager().loadSceneResourceByAssetId("Example/Scene/Default/FirstScene");

			// Camera scene item
			mCameraSceneItem = mSceneResource->createSceneItem<RendererRuntime::CameraSceneItem>();

			{ // First rotating mesh
				// Scene node
				mSceneNode = mSceneResource->createSceneNode(RendererRuntime::Transform(glm::vec3(0.0f, -7.0f, -25.0f), RendererRuntime::Quaternion::IDENTITY, glm::vec3(0.5f)));

				// Mesh scene item
				RendererRuntime::MeshSceneItem* meshSceneItem = mSceneResource->createSceneItem<RendererRuntime::MeshSceneItem>();
				if (nullptr != meshSceneItem)
				{
					meshSceneItem->setMeshResourceByAssetId("Example/Mesh/Character/ImrodLowPoly");
					mSceneNode->attachSceneItem(*meshSceneItem);
				}
			}

			{ // Second fixed mesh
				// Scene node
				RendererRuntime::ISceneNode* sceneNode = mSceneResource->createSceneNode(RendererRuntime::Transform(glm::vec3(0.0f, -15.0f, -25.0f), RendererRuntime::Quaternion::IDENTITY, glm::vec3(0.25f)));

				// Mesh scene item
				RendererRuntime::MeshSceneItem* meshSceneItem = mSceneResource->createSceneItem<RendererRuntime::MeshSceneItem>();
				if (nullptr != meshSceneItem)
				{
					meshSceneItem->setMeshResourceByAssetId("Example/Mesh/Character/ImrodLowPoly");
					sceneNode->attachSceneItem(*meshSceneItem);
				}
			}
		}
	}
}

void FirstScene::onDeinitialization()
{
	// TODO(co) Implement decent resource handling
	delete mCompositorInstance;
	mCompositorInstance = nullptr;

	// Release the used resources
	delete mSceneResource;

	// Call the base implementation
	IApplicationRendererRuntime::onDeinitialization();
}

void FirstScene::onUpdate()
{
	// Call the base implementation
	IApplicationRendererRuntime::onUpdate();

	// Stop the stopwatch
	mStopwatch.stop();

	// Update the global timer (FPS independent movement)
	mGlobalTimer += mStopwatch.getMilliseconds() * 0.0005f;

	// Update the scene node rotation
	if (nullptr != mSceneNode)
	{
		mSceneNode->setRotation(glm::angleAxis(mGlobalTimer, glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	// Start the stopwatch
	mStopwatch.start();
}

void FirstScene::onDrawRequest()
{
	// Is there a compositor instance?
	if (nullptr != mCompositorInstance)
	{
		// Execute the compositor instance
		mCompositorInstance->execute(mCameraSceneItem);
	}
}
