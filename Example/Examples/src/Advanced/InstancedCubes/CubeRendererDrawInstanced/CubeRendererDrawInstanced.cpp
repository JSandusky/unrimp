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
#include "PrecompiledHeader.h"
#include "Advanced/InstancedCubes/CubeRendererDrawInstanced/CubeRendererDrawInstanced.h"
#include "Advanced/InstancedCubes/CubeRendererDrawInstanced/BatchDrawInstanced.h"
#include "Framework/PlatformTypes.h"	// For "OUTPUT_DEBUG_STRING()"

#include <math.h>
#include <string.h>
#include <float.h> // For FLT_MAX
#include <stdlib.h> // For rand()


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
CubeRendererDrawInstanced::CubeRendererDrawInstanced(Renderer::IRenderer &renderer, uint32_t numberOfTextures, uint32_t sceneRadius) :
	mRenderer(&renderer),
	mNumberOfTextures(numberOfTextures),
	mSceneRadius(sceneRadius),
	mMaximumNumberOfInstancesPerBatch(0),
	mNumberOfBatches(0),
	mBatches(nullptr)
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

	// Check number of textures (limit of this implementation and renderer limit)
	if (mNumberOfTextures > MAXIMUM_NUMBER_OF_TEXTURES)
	{
		mNumberOfTextures = MAXIMUM_NUMBER_OF_TEXTURES;
	}
	if (mNumberOfTextures > mRenderer->getCapabilities().maximumNumberOf2DTextureArraySlices)
	{
		mNumberOfTextures = mRenderer->getCapabilities().maximumNumberOf2DTextureArraySlices;
	}

	// Get the maximum number of instances per batch
	// -> In this application, this depends on the maximum texture buffer size
	// -> /2 -> One instance requires two texels
	mMaximumNumberOfInstancesPerBatch = mRenderer->getCapabilities().maximumTextureBufferSize / 2;

	{ // Create the textures
		static const uint32_t TEXTURE_WIDTH   = 128;
		static const uint32_t TEXTURE_HEIGHT  = 128;
		static const uint32_t NUMBER_OF_BYTES = TEXTURE_WIDTH * TEXTURE_HEIGHT * 4;

		// Allocate memory for the 2D texture array
		uint8_t *data = new uint8_t[NUMBER_OF_BYTES * mNumberOfTextures];

		{ // Fill the texture content
			// TODO(co) Be a little bit more creative while filling the texture data
			uint8_t *dataCurrent = data;
			const float colors[][MAXIMUM_NUMBER_OF_TEXTURES] =
			{
				{ 1.0f, 0.0f, 0.0f},
				{ 0.0f, 0.1f, 0.0f},
				{ 0.0f, 0.0f, 0.1f},
				{ 0.5f, 0.5f, 0.5f},
				{ 1.0f, 1.0f, 1.0f},
				{ 0.1f, 0.2f, 0.2f},
				{ 0.2f, 0.5f, 0.5f},
				{ 0.1f, 0.8f, 0.2f}
			};
			for (uint32_t j = 0; j < mNumberOfTextures; ++j)
			{
				// Random content
				for (uint32_t i = 0; i < TEXTURE_WIDTH * TEXTURE_HEIGHT; ++i)
				{
					*dataCurrent = static_cast<uint8_t>((rand() % 255) * colors[j][0]);
					++dataCurrent;
					*dataCurrent = static_cast<uint8_t>((rand() % 255) * colors[j][1]);
					++dataCurrent;
					*dataCurrent = static_cast<uint8_t>((rand() % 255) * colors[j][2]);
					++dataCurrent;
					*dataCurrent = 255;
					++dataCurrent;
				}
			}
		}

		// Create the texture instance
		// -> By using 2D array textures together with OpenGL/Direct3D 11 instancing we get a handy implementation
		// -> This limits of course the cross platform support, fallback solutions might be a good idea in productive code
		// -> A fallback is not really required in our example situation because we're using draw instanced which already requires a more modern graphics card
		mTexture2DArray = mRenderer->createTexture2DArray(TEXTURE_WIDTH, TEXTURE_HEIGHT, mNumberOfTextures, Renderer::TextureFormat::R8G8B8A8, data, Renderer::TextureFlag::MIPMAPS);

		// Free texture memory
		delete [] data;
	}

	// Create sampler state
	mSamplerState = mRenderer->createSamplerState(Renderer::ISamplerState::getDefaultSamplerState());

	// Decide which shader language should be used (for example "GLSL" or "HLSL")
	Renderer::IShaderLanguagePtr shaderLanguage(mRenderer->getShaderLanguage());
	if (nullptr != shaderLanguage)
	{
		// Uniform buffer object (UBO, "constant buffer" in Direct3D terminology) supported?
		// -> If they are there, we really want to use them (performance and ease of use)
		if (mRenderer->getCapabilities().uniformBuffer)
		{
			{ // Create and set constant program uniform buffer at once
				// TODO(co) Uggly fixed hacked in model-view-projection matrix
				// TODO(co) OpenGL matrix, Direct3D has minor differences within the projection matrix we have to compensate
				static const float MVP[] =
				{
					 1.2803299f,	-0.97915620f,	-0.58038759f,	-0.57922798f,
					 0.0f,			 1.9776078f,	-0.57472473f,	-0.573576453f,
					-1.2803299f,	-0.97915620f,	-0.58038759f,	-0.57922798f,
					 0.0f,			 0.0f,			 9.8198195f,	 10.0f
				};
				mUniformBufferStaticVs = shaderLanguage->createUniformBuffer(sizeof(MVP), MVP, Renderer::BufferUsage::STATIC_DRAW);
			}

			// Create dynamic uniform buffers
			mUniformBufferDynamicVs = shaderLanguage->createUniformBuffer(sizeof(float) * 2, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
			mUniformBufferDynamicFs = shaderLanguage->createUniformBuffer(sizeof(float) * 3, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
		}

		{ // Create the program
			// Get the shader source code (outsourced to keep an overview)
			const char *vertexShaderSourceCode = nullptr;
			const char *fragmentShaderSourceCode = nullptr;
			#include "CubeRendererDrawInstanced_GLSL_140.h"
			#include "CubeRendererDrawInstanced_HLSL_D3D10_D3D11.h"
			#include "CubeRendererDrawInstanced_Null.h"

			// Create the program
			mProgram = shaderLanguage->createProgram(
				shaderLanguage->createVertexShader(vertexShaderSourceCode),
				shaderLanguage->createFragmentShader(fragmentShaderSourceCode));
		}

		// Is there a valid program?
		if (nullptr != mProgram)
		{
			// Create the vertex buffer object (VBO)
			static const float VERTEX_POSITION[] =
			{
				// Front face
				// Position					TexCoord		 Normal				// Vertex ID
				-0.5f, -0.5f,  0.5f,		0.0f, 0.0f,		 0.0f, 0.0f, 1.0f,	// 0
				 0.5f, -0.5f,  0.5f,		1.0f, 0.0f,		 0.0f, 0.0f, 1.0f,	// 1
				 0.5f,  0.5f,  0.5f,		1.0f, 1.0f,		 0.0f, 0.0f, 1.0f,	// 2
				-0.5f,  0.5f,  0.5f,		0.0f, 1.0f,		 0.0f, 0.0f, 1.0f,	// 3
				// Back face
				-0.5f, -0.5f, -0.5f,		1.0f, 0.0f,		 0.0f, 0.0f,-1.0f,	// 4
				-0.5f,  0.5f, -0.5f,		1.0f, 1.0f,		 0.0f, 0.0f,-1.0f,	// 5
				 0.5f,  0.5f, -0.5f,		0.0f, 1.0f,		 0.0f, 0.0f,-1.0f, 	// 6
				 0.5f, -0.5f, -0.5f,		0.0f, 0.0f,		 0.0f, 0.0f,-1.0f,	// 7
				// Top face
				-0.5f,  0.5f, -0.5f,		0.0f, 1.0f,		 0.0f, 1.0f, 0.0f,	// 8
				-0.5f,  0.5f,  0.5f,		0.0f, 0.0f,		 0.0f, 1.0f, 0.0f,	// 9
				 0.5f,  0.5f,  0.5f,		1.0f, 0.0f,		 0.0f, 1.0f, 0.0f,	// 10
				 0.5f,  0.5f, -0.5f,		1.0f, 1.0f,		 0.0f, 1.0f, 0.0f,	// 11
				// Bottom face
				-0.5f, -0.5f, -0.5f,		1.0f, 1.0f,		 0.0f,-1.0f, 0.0f,	// 12
				 0.5f, -0.5f, -0.5f,		0.0f, 1.0f,		 0.0f,-1.0f, 0.0f,	// 13
				 0.5f, -0.5f,  0.5f,		0.0f, 0.0f,		 0.0f,-1.0f, 0.0f,	// 14
				-0.5f, -0.5f,  0.5f,		1.0f, 0.0f,		 0.0f,-1.0f, 0.0f,	// 15
				// Right face
				 0.5f, -0.5f, -0.5f,		1.0f, 0.0f,		 1.0f, 0.0f, 0.0f,	// 16
				 0.5f,  0.5f, -0.5f,		1.0f, 1.0f,		 1.0f, 0.0f, 0.0f,	// 17
				 0.5f,  0.5f,  0.5f,		0.0f, 1.0f,		 1.0f, 0.0f, 0.0f,	// 18
				 0.5f, -0.5f,  0.5f,		0.0f, 0.0f,		 1.0f, 0.0f, 0.0f,	// 19
				// Left face
				-0.5f, -0.5f, -0.5f,		0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,	// 20
				-0.5f, -0.5f,  0.5f,		1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,	// 21
				-0.5f,  0.5f,  0.5f,		1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,	// 22
				-0.5f,  0.5f, -0.5f,		0.0f, 1.0f,		-1.0f, 0.0f, 0.0f	// 23
			};
			Renderer::IVertexBufferPtr vertexBuffer(mRenderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

			// Create the index buffer object (IBO)
			static const uint16_t INDICES[] =
			{
				// Front face	Triangle ID
				 1,  0,  2,		// 0
				 3,  2,  0,		// 1
				// Back face
				 6,  5,  4,		// 2
				 4,  7,  6,		// 3
				// Top face
				 9,  8, 10,		// 4
				11, 10,  8,		// 5
				// Bottom face
				13, 12, 14,		// 6
				15, 14, 12,		// 7
				// Right face
				17, 16, 18,		// 8
				19, 18, 16,		// 9
				// Left face
				21, 20, 22,		// 10
				23, 22, 20		// 11
			};
			Renderer::IIndexBuffer *indexBuffer = mRenderer->createIndexBuffer(sizeof(INDICES), Renderer::IndexBufferFormat::UNSIGNED_SHORT, INDICES, Renderer::BufferUsage::STATIC_DRAW);

			// Create vertex array object (VAO)
			// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
			// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
			// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
			//    reference of the used vertex buffer objects (VBO). If the reference counter of a
			//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
			const Renderer::VertexArrayAttribute vertexArray[] =
			{
				{ // Attribute 0
					// Data destination
					Renderer::VertexArrayFormat::FLOAT_3,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
					"Position",								// name[64] (char)
					"POSITION",								// semantic[64] (char)
					0,										// semanticIndex (uint32_t)
					// Data source
					vertexBuffer,							// vertexBuffer (Renderer::IVertexBuffer *)
					0,										// offset (uint32_t)
					sizeof(float) * (3 + 2 + 3),			// stride (uint32_t)
					// Data source, instancing part
					0										// instancesPerElement (uint32_t)
				},
				{ // Attribute 1
					// Data destination
					Renderer::VertexArrayFormat::FLOAT_2,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
					"TexCoord",								// name[64] (char)
					"TEXCOORD",								// semantic[64] (char)
					0,										// semanticIndex (uint32_t)
					// Data source
					vertexBuffer,							// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 3,						// offset (uint32_t)
					sizeof(float) * (3 + 2 + 3),			// stride (uint32_t)
					// Data source, instancing part
					0										// instancesPerElement (uint32_t)
				},
				{ // Attribute 2
					// Data destination
					Renderer::VertexArrayFormat::FLOAT_3,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
					"Normal",								// name[64] (char)
					"NORMAL",								// semantic[64] (char)
					0,										// semanticIndex (uint32_t)
					// Data source
					vertexBuffer,							// pertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * (3 + 2),				// offset (uint32_t)
					sizeof(float) * (3 + 2 + 3),			// stride (uint32_t)
					// Data source, instancing part
					0										// instancesPerElement (uint32_t)
				}
			};
			mVertexArray = mProgram->createVertexArray(sizeof(vertexArray) / sizeof(Renderer::VertexArrayAttribute), vertexArray, indexBuffer);
		}
	}

	// End debug event
	RENDERER_END_DEBUG_EVENT(&renderer)
}

CubeRendererDrawInstanced::~CubeRendererDrawInstanced()
{
	// The renderer resource pointers are released automatically

	// Destroy the batches
	if (nullptr != mBatches)
	{
		delete [] mBatches;
	}
}


//[-------------------------------------------------------]
//[ Public virtual ICubeRenderer methods                  ]
//[-------------------------------------------------------]
void CubeRendererDrawInstanced::setNumberOfCubes(uint32_t numberOfCubes)
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(mRenderer)

	// Destroy previous batches, in case there are any
	if (nullptr != mBatches)
	{
		delete [] mBatches;
		mNumberOfBatches = 0;
		mBatches = nullptr;
	}

	// A third of the cubes should be rendered using alpha blending
	const uint32_t numberOfTransparentCubes = numberOfCubes / 3;
	const uint32_t numberOfSolidCubes       = numberOfCubes - numberOfTransparentCubes;

	// There's a limitation how many instances can be created per draw call
	// -> If required, create multiple batches
	const uint32_t numberOfSolidBatches       = static_cast<uint32_t>(ceil(static_cast<float>(numberOfSolidCubes)       / mMaximumNumberOfInstancesPerBatch));
	const uint32_t numberOfTransparentBatches = static_cast<uint32_t>(ceil(static_cast<float>(numberOfTransparentCubes) / mMaximumNumberOfInstancesPerBatch));

	// Create a batch instances
	mNumberOfBatches = numberOfSolidBatches + numberOfTransparentBatches;
	mBatches = new BatchDrawInstanced[mNumberOfBatches];

	// Initialize the solid batches
	BatchDrawInstanced *batch     = mBatches;
	BatchDrawInstanced *lastBatch = mBatches + numberOfSolidBatches;
	for (int remaningNumberOfCubes = static_cast<int>(numberOfSolidCubes); batch < lastBatch; ++batch, remaningNumberOfCubes -= mMaximumNumberOfInstancesPerBatch)
	{
		const uint32_t currentNumberOfCubes = (remaningNumberOfCubes > static_cast<int>(mMaximumNumberOfInstancesPerBatch)) ? mMaximumNumberOfInstancesPerBatch : remaningNumberOfCubes;
		batch->initialize(*mRenderer, currentNumberOfCubes, false, mNumberOfTextures, mSceneRadius);
	}

	// Initialize the transparent batches
	// -> TODO(co) For correct alpha blending, the transparent instances should be sorted from back to front.
	lastBatch = batch + numberOfTransparentBatches;
	for (int remaningNumberOfCubes = static_cast<int>(numberOfTransparentCubes); batch < lastBatch; ++batch, remaningNumberOfCubes -= mMaximumNumberOfInstancesPerBatch)
	{
		const uint32_t currentNumberOfCubes = (remaningNumberOfCubes > static_cast<int>(mMaximumNumberOfInstancesPerBatch)) ? mMaximumNumberOfInstancesPerBatch : remaningNumberOfCubes;
		batch->initialize(*mRenderer, currentNumberOfCubes, true, mNumberOfTextures, mSceneRadius);
	}

	// End debug event
	RENDERER_END_DEBUG_EVENT(mRenderer)
}

void CubeRendererDrawInstanced::draw(float globalTimer, float globalScale, float lightPositionX, float lightPositionY, float lightPositionZ)
{
	// Is there a valid program?
	if (nullptr != mProgram)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(mRenderer)

		// Set the used program
		mRenderer->setProgram(mProgram);

		{ // Setup input assembly (IA)
			// Set the used vertex array
			mRenderer->iaSetVertexArray(mVertexArray);

			// Set the primitive topology used for draw calls
			mRenderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_LIST);
		}

		// Set the used texture at a certain texture unit
		mRenderer->fsSetTexture(0, mTexture2DArray);

		// Set the used sampler state at a certain texture unit
		mRenderer->fsSetSamplerState(0, mSamplerState);

		// Set the used uniform buffers
		if (nullptr != mUniformBufferStaticVs)
		{
			mRenderer->vsSetUniformBuffer(mProgram->getUniformBlockIndex("UniformBlockStaticVs", 0), mUniformBufferStaticVs);
		}
		if (nullptr != mUniformBufferDynamicVs)
		{
			mRenderer->vsSetUniformBuffer(mProgram->getUniformBlockIndex("UniformBlockDynamicVs", 1), mUniformBufferDynamicVs);
		}
		if (nullptr != mUniformBufferDynamicFs)
		{
			mRenderer->fsSetUniformBuffer(mProgram->getUniformBlockIndex("UniformBlockDynamicFs", 0), mUniformBufferDynamicFs);
		}

		// Set constant program uniform
		if (nullptr == mUniformBufferStaticVs)
		{
			// TODO(co) Uggly fixed hacked in model-view-projection matrix
			// TODO(co) OpenGL matrix, Direct3D has minor differences within the projection matrix we have to compensate
			static const float MVP[] =
			{
				 1.2803299f,	-0.97915620f,	-0.58038759f,	-0.57922798f,
				 0.0f,			 1.9776078f,	-0.57472473f,	-0.573576453f,
				-1.2803299f,	-0.97915620f,	-0.58038759f,	-0.57922798f,
				 0.0f,			 0.0f,			 9.8198195f,	 10.0f
			};

			// There's no uniform buffer: We have to set individual uniforms
			mProgram->setUniform4fv(mProgram->getUniformHandle("MVP"), MVP);
		}

		{ // Set program uniforms
			// Some counting timer, we don't want to touch the buffers on the GPU
			const float timerAndGlobalScale[] = { globalTimer, globalScale };

			// Animate the point light world space position
			const float lightPosition[] = { lightPositionX, lightPositionY, lightPositionZ };

			// Use uniform buffer?
			if (nullptr != mUniformBufferDynamicVs)
			{
				// Copy data into the uniform buffer
				mUniformBufferDynamicVs->copyDataFrom(sizeof(timerAndGlobalScale), timerAndGlobalScale);
				if (nullptr != mUniformBufferDynamicFs)
				{
					// Copy data into the uniform buffer
					mUniformBufferDynamicFs->copyDataFrom(sizeof(lightPosition), lightPosition);
				}
			}
			else
			{
				// Set individual program uniforms
				// -> Using uniform buffers (aka constant buffers in Direct3D) would be more efficient, but Direct3D 9 doesn't support it (neither does e.g. OpenGL ES 2.0)
				// -> To keep it simple in here, I just use a less performant string to identity the uniform (does not really hurt in here)
				mProgram->setUniform2fv(mProgram->getUniformHandle("TimerAndGlobalScale"), timerAndGlobalScale);
				mProgram->setUniform3fv(mProgram->getUniformHandle("LightPosition"), lightPosition);
			}
		}

		// Draw the batches
		if (nullptr != mBatches)
		{
			// Loop though all batches
			BatchDrawInstanced *batch     = mBatches;
			BatchDrawInstanced *lastBatch = mBatches + mNumberOfBatches;
			for (; batch < lastBatch; ++batch)
			{
				// Draw this batch
				batch->draw();
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(mRenderer)
	}
}
