/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "InstancedCubes/CubeRendererDrawInstanced/BatchDrawInstanced.h"
#include "Framework/PlatformTypes.h"
#include "Framework/Quaternion.h"
#include "Framework/EulerAngles.h"

#include <stdlib.h> // For rand()


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
BatchDrawInstanced::BatchDrawInstanced() :
	mNumberOfCubeInstances(0)
{
	// Nothing to do in here
}

BatchDrawInstanced::~BatchDrawInstanced()
{
	// The renderer resource pointers are released automatically
}

void BatchDrawInstanced::initialize(Renderer::IRenderer &renderer, unsigned int numberOfCubeInstances, bool alphaBlending, unsigned int numberOfTextures, unsigned int sceneRadius)
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

	// Release previous data if required
	mBlendState = nullptr;
	mTextureBufferPerInstanceData = nullptr;

	// Set owner renderer instance
	mRenderer = &renderer;

	// Set the number of cube instance
	mNumberOfCubeInstances = numberOfCubeInstances;

	{ // Create the texture buffer instance
		// Allocate the local per instance data
		const unsigned int numberOfElements = mNumberOfCubeInstances * 2 * 4;
		float *data = new float[numberOfElements];
		float *dataCurrent = data;

		// Set data
		// -> Layout: [Position][Rotation][Position][Rotation]...
		//    - Position: xyz=Position, w=Slice of the 2D texture array to use
		//    - Rotation: Rotation quaternion (xyz) and scale (w)
		//      -> We don't need to store the w component of the quaternion. It's normalized and storing
		//         three components while recomputing the fourths component is be sufficient.
		Quaternion quaternion;	// Identity rotation quaternion
		for (unsigned int i = 0; i < mNumberOfCubeInstances; ++i)
		{
			{ // Position
				// r=x
				*dataCurrent = -static_cast<int>(sceneRadius) + 2.0f * sceneRadius * (rand() % 65536) / 65536.0f;
				++dataCurrent;
				// g=y
				*dataCurrent = -static_cast<int>(sceneRadius) + 2.0f * sceneRadius * (rand() % 65536) / 65536.0f;
				++dataCurrent;
				// b=z
				*dataCurrent = -static_cast<int>(sceneRadius) + 2.0f * sceneRadius * (rand() % 65536) / 65536.0f;
				++dataCurrent;
				// a=Slice of the 2D texture array to use
				*dataCurrent = static_cast<float>(rand() % numberOfTextures); // Choose a random texture
				++dataCurrent;
			}

			{ // Rotation
				EulerAngles::toQuaternion((rand() % 65536) / 65536.0f, (rand() % 65536) / 65536.0f * 2.0f, (rand() % 65536) / 65536.0f * 3.0f, quaternion);

				// r=x
				*dataCurrent = quaternion.x;
				++dataCurrent;
				// g=y
				*dataCurrent = quaternion.y;
				++dataCurrent;
				// b=z
				*dataCurrent = quaternion.z;
				++dataCurrent;
				// a=scale
				*dataCurrent = 2.0f * (rand() % 65536) / 65536.0f;
				++dataCurrent;
			}
		}

		// Create the texture buffer instance
		mTextureBufferPerInstanceData = mRenderer->createTextureBuffer(sizeof(float) * numberOfElements, Renderer::TextureFormat::R32G32B32A32F, data, Renderer::BufferUsage::STATIC_DRAW);

		// Free local per instance data
		delete [] data;
	}

	{ // Create blend state
		Renderer::BlendState blendState = Renderer::IBlendState::getDefaultBlendState();
		blendState.renderTarget[0].blendEnable = alphaBlending;
		blendState.renderTarget[0].srcBlend    = Renderer::Blend::SRC_ALPHA;
		blendState.renderTarget[0].destBlend   = Renderer::Blend::ONE;
		mBlendState = mRenderer->createBlendState(blendState);
	}

	// End debug event
	RENDERER_END_DEBUG_EVENT(&renderer)
}

void BatchDrawInstanced::draw()
{
	// Is there a valid renderer owner instance?
	if (nullptr != mRenderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(mRenderer)

		// Set the used texture at a certain texture unit
		mRenderer->vsSetTexture(0, mTextureBufferPerInstanceData);

		// Set the used blend state
		mRenderer->omSetBlendState(mBlendState);

		// Use instancing in order to draw multiple cubes with just a single draw call
		// -> Draw calls are one of the most expensive rendering, avoid them if possible
		mRenderer->drawIndexedInstanced(0, 36, 0, 0, 24, mNumberOfCubeInstances);

		// End debug event
		RENDERER_END_DEBUG_EVENT(mRenderer)
	}
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
BatchDrawInstanced::BatchDrawInstanced(const BatchDrawInstanced &)
{
	// Not supported
}

BatchDrawInstanced &BatchDrawInstanced::operator =(const BatchDrawInstanced &)
{
	// Not supported
	return *this;
}
