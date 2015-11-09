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
#include "Advanced/InstancedCubes/CubeRendererInstancedArrays/BatchInstancedArrays.h"
#include "Framework/PlatformTypes.h"
#include "Framework/Quaternion.h"
#include "Framework/EulerAngles.h"

#include <stdlib.h> // For rand()


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
BatchInstancedArrays::BatchInstancedArrays() :
	mNumberOfCubeInstances(0)
{
	// Nothing to do in here
}

BatchInstancedArrays::~BatchInstancedArrays()
{
	// The renderer resource pointers are released automatically
}

void BatchInstancedArrays::initialize(Renderer::IRootSignature &rootSignature, const Renderer::VertexAttributes& vertexAttributes, Renderer::IVertexBuffer &vertexBuffer, Renderer::IIndexBuffer &indexBuffer, Renderer::IProgram &program, uint32_t numberOfCubeInstances, bool alphaBlending, uint32_t numberOfTextures, uint32_t sceneRadius)
{
	// Set owner renderer instance
	mRenderer = &program.getRenderer();

	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(mRenderer)

	// Release previous data if required
	mVertexArray = nullptr;

	// Set the number of cube instance
	mNumberOfCubeInstances = numberOfCubeInstances;

	{ // Create the texture buffer instance
		// Allocate the local per instance data
		const uint32_t numberOfElements = mNumberOfCubeInstances * 2 * 4;
		float *data = new float[numberOfElements];
		float *dataCurrent = data;

		// Set data
		// -> Layout: [Position][Rotation][Position][Rotation]...
		//    - Position: xyz=Position, w=Slice of the 2D texture array to use
		//    - Rotation: Rotation quaternion (xyz) and scale (w)
		//      -> We don't need to store the w component of the quaternion. It's normalized and storing
		//         three components while recomputing the fourths component is be sufficient.
		Quaternion quaternion;	// Identity rotation quaternion
		for (uint32_t i = 0; i < mNumberOfCubeInstances; ++i)
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

		// Create the vertex buffer object (VBO) instance containing the per-instance-data
		Renderer::IVertexBuffer *vertexBufferPerInstanceData = mRenderer->createVertexBuffer(sizeof(float) * numberOfElements, data, Renderer::BufferUsage::STATIC_DRAW);

		{ // Create vertex array object (VAO)
			// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
			// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
			// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
			//    reference of the used vertex buffer objects (VBO). If the reference counter of a
			//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
			const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] =
			{
				{ // Vertex buffer 0
					&vertexBuffer,				// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * (3 + 2 + 3)	// strideInBytes (uint32_t)
				},
				{ // Vertex buffer 1
					vertexBufferPerInstanceData,	// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 4 * 2			// strideInBytes (uint32_t)
				}
			};
			mVertexArray = mRenderer->createVertexArray(vertexAttributes, sizeof(vertexArrayVertexBuffers) / sizeof(Renderer::VertexArrayVertexBuffer), vertexArrayVertexBuffers, &indexBuffer);
		}

		// Free local per instance data
		delete [] data;
	}

	{ // Create the pipeline state object (PSO)
		Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder(&rootSignature, &program, vertexAttributes);
		pipelineState.blendState.renderTarget[0].blendEnable = alphaBlending;
		pipelineState.blendState.renderTarget[0].srcBlend    = Renderer::Blend::SRC_ALPHA;
		pipelineState.blendState.renderTarget[0].destBlend   = Renderer::Blend::ONE;
		mPipelineState = mRenderer->createPipelineState(pipelineState);
	}

	// End debug event
	RENDERER_END_DEBUG_EVENT(mRenderer)
}

void BatchInstancedArrays::draw()
{
	// Is there a valid renderer owner instance?
	if (nullptr != mRenderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(mRenderer)

		{ // Setup input assembly (IA)
			// Set the used vertex array
			mRenderer->iaSetVertexArray(mVertexArray);
		}

		// Set the used pipeline state object (PSO)
		mRenderer->setPipelineState(mPipelineState);

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
BatchInstancedArrays::BatchInstancedArrays(const BatchInstancedArrays &)
{
	// Not supported
}

BatchInstancedArrays &BatchInstancedArrays::operator =(const BatchInstancedArrays &)
{
	// Not supported
	return *this;
}
