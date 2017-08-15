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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "PrecompiledHeader.h"
#include "Advanced/InstancedCubes/InstancedCubes.h"
#include "Advanced/InstancedCubes/CubeRendererDrawInstanced/CubeRendererDrawInstanced.h"
#include "Advanced/InstancedCubes/CubeRendererInstancedArrays/CubeRendererInstancedArrays.h"
#include "Framework/Color4.h"

#include <RendererRuntime/IRendererRuntime.h>
#include <RendererRuntime/DebugGui/DebugGuiManager.h>
#include <RendererRuntime/DebugGui/DebugGuiHelper.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <math.h>
#include <stdio.h>
#include <limits.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
InstancedCubes::InstancedCubes() :
	mCubeRenderer(nullptr),
	mNumberOfCubeInstances(1000),
	mGlobalTimer(0.0f),
	mGlobalScale(1.0f),
	mDisplayStatistics(true),
	mFPSUpdateTimer(0.0f),
	mFramesSinceCheck(0),
	mFramesPerSecond(0.0f)
{
	// Nothing here
}

InstancedCubes::~InstancedCubes()
{
	// The resources are released within "onDeinitialization()"
	// Nothing here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void InstancedCubes::onInitialization()
{
	// Call the base implementation
	ExampleBase::onInitialization();

	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Create the cube renderer instance
		// -> Evaluate the feature set of the used renderer
		if (renderer->getCapabilities().drawInstanced && renderer->getCapabilities().maximumNumberOf2DTextureArraySlices > 0 && renderer->getCapabilities().maximumTextureBufferSize > 0)
		{
			// Render cubes by using draw instanced (shader model 4 feature, build in shader variable holding the current instance ID)
			mCubeRenderer = new CubeRendererDrawInstanced(*renderer, NUMBER_OF_TEXTURES, SCENE_RADIUS);
		}
		else if (renderer->getCapabilities().instancedArrays)
		{
			// Render cubes by using instanced arrays (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
			mCubeRenderer = new CubeRendererInstancedArrays(*renderer, NUMBER_OF_TEXTURES, SCENE_RADIUS);
		}

		// Tell the cube renderer about the number of cubes
		if (nullptr != mCubeRenderer)
		{
			mCubeRenderer->setNumberOfCubes(mNumberOfCubeInstances);
		}
	}
}

void InstancedCubes::onDeinitialization()
{
	// Destroy the cube renderer, in case there's one
	if (nullptr != mCubeRenderer)
	{
		delete mCubeRenderer;
		mCubeRenderer = nullptr;
	}

	// Call the base implementation
	ExampleBase::onDeinitialization();
}

void InstancedCubes::onKeyDown(uint32_t key)
{
	// Evaluate the key
	switch (key)
	{
		// Add a fixed number of cubes
		case 107:	// Numpad +
		case 187:	// +*~ key
			// Upper limit, just in case someone tries something nasty
			if (mNumberOfCubeInstances < UINT_MAX - NUMBER_OF_CHANGED_CUBES)
			{
				// Update the number of cubes
				mNumberOfCubeInstances += NUMBER_OF_CHANGED_CUBES;

				// Tell the cube renderer about the number of cubes
				if (nullptr != mCubeRenderer)
				{
					mCubeRenderer->setNumberOfCubes(mNumberOfCubeInstances);
				}
			}
			break;

		// Subtract a fixed number of cubes
		case 109:	// Numpad -
		case 189:	// -_ key
			// Lower limit
			if (mNumberOfCubeInstances > 1)
			{
				// Update the number of cubes
				if (mNumberOfCubeInstances > NUMBER_OF_CHANGED_CUBES)
				{
					mNumberOfCubeInstances -= NUMBER_OF_CHANGED_CUBES;
				}
				else
				{
					mNumberOfCubeInstances = 1;
				}

				// Tell the cube renderer about the number of cubes
				if (nullptr != mCubeRenderer)
				{
					mCubeRenderer->setNumberOfCubes(mNumberOfCubeInstances);
				}
			}
			break;

		// Scale cubes up (change the size of all cubes at the same time)
		case 38:	// Key up
			mGlobalScale += 0.1f;
			break;

		// Scale cubes down (change the size of all cubes at the same time)
		case 40:	// Key down
			mGlobalScale -= 0.1f;	// No need to check for negative values, results in entertaining inversed backface culling
			break;

		// Show/hide statistics
		case ' ':	// Space
			// Toggle display of statistics
			mDisplayStatistics = !mDisplayStatistics;
			break;
	}
}

void InstancedCubes::onUpdate()
{
	// Stop the stopwatch
	mStopwatch.stop();

	// Update the global timer (FPS independent movement)
	const float timeDifference = mStopwatch.getMilliseconds();
	mGlobalTimer += timeDifference;

	// Calculate the current FPS
	++mFramesSinceCheck;
	mFPSUpdateTimer += timeDifference;
	if (mFPSUpdateTimer > 1000.0f)
	{
		mFramesPerSecond   = static_cast<float>(mFramesSinceCheck / (mFPSUpdateTimer / 1000.0f));
		mFPSUpdateTimer   -= 1000.0f;
		mFramesSinceCheck  = 0;
	}

	// Start the stopwatch
	mStopwatch.start();
}

void InstancedCubes::onDraw()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Clear the color buffer of the current render target with gray, do also clear the depth buffer
		Renderer::Command::Clear::create(mCommandBuffer, Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

		// Draw the cubes
		if (nullptr != mCubeRenderer)
		{
			mCubeRenderer->fillCommandBuffer(mGlobalTimer, mGlobalScale, sin(mGlobalTimer * 0.001f) * SCENE_RADIUS, sin(mGlobalTimer * 0.0005f) * SCENE_RADIUS, cos(mGlobalTimer * 0.0008f) * SCENE_RADIUS, mCommandBuffer);
		}

		// Display statistics
		if (mDisplayStatistics)
		{
			RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
			if (nullptr != rendererRuntime && nullptr != getMainRenderTarget())
			{
				RendererRuntime::DebugGuiManager& debugGuiManager = rendererRuntime->getDebugGuiManager();
				debugGuiManager.newFrame(*getMainRenderTarget());

				// Is there a cube renderer instance?
				if (nullptr != mCubeRenderer)
				{
					char text[128];

					// Number of cubes
					sprintf(text, "Number of cubes: %d", mNumberOfCubeInstances);
					RendererRuntime::DebugGuiHelper::drawText(text, 10.0f, 10.0f);

					// Frames per second
					sprintf(text, "Frames per second: %.2f", mFramesPerSecond);
					RendererRuntime::DebugGuiHelper::drawText(text, 10.0f, 40.0f);

					// Cubes per second
					// -> In every frame we draw n-cubes...
					// -> TODO(co) This number can get huge... had over 1 million cubes with >25 FPS... million cubes at ~2.4 FPS...
					sprintf(text, "Cubes per second: %u", static_cast<uint32_t>(mFramesPerSecond) * mNumberOfCubeInstances);
					RendererRuntime::DebugGuiHelper::drawText(text, 10.0f, 70.0f);
				}
				else
				{
					RendererRuntime::DebugGuiHelper::drawText("No cube renderer instance", 10.0f, 10.0f);
				}
				debugGuiManager.fillCommandBufferUsingFixedBuildInRendererConfiguration(mCommandBuffer);
			}
		}

		// Submit command buffer to the renderer backend
		mCommandBuffer.submitAndClear(*renderer);
	}
}
