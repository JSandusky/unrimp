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
#include "RendererRuntime/RendererRuntimeImpl.h"
#include "RendererRuntime/asset/AssetManager.h"
#include "RendererRuntime/scene/SceneManager.h"
#include "RendererRuntime/compositor/CompositorManager.h"
#include "RendererRuntime/Resource/Font/FontResourceManager.h"
#include "RendererRuntime/Resource/Mesh/MeshResourceManager.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
RENDERERRUNTIME_FUNCTION_EXPORT RendererRuntime::IRendererRuntime *createRendererRuntimeInstance(Renderer::IRenderer &renderer)
{
	return new RendererRuntime::RendererRuntimeImpl(renderer);
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RendererRuntimeImpl::RendererRuntimeImpl(Renderer::IRenderer &renderer) :
		mFontProgram(nullptr),
		mFontVertexShaderUniformBuffer(nullptr),
		mFontFragmentShaderUniformBuffer(nullptr),
		mFontVertexArray(nullptr),
		mFontSamplerState(nullptr),
		mFontBlendState(nullptr)
	{
		// Backup the given renderer
		mRenderer = &renderer;

		// Add our renderer reference
		mRenderer->addReference();

		// Create the manager instances
		mAssetManager = new AssetManager(*this);
		mCompositorManager = new CompositorManager(*this);
		mSceneManager = new SceneManager(*this);

		// Create the resource manager instances
		mFontResourceManager = new FontResourceManager(*this);
		mTextureResourceManager = new TextureResourceManager(*this);
		mMaterialResourceManager = new MaterialResourceManager(*this);
		mMeshResourceManager = new MeshResourceManager(*this);
	}

	RendererRuntimeImpl::~RendererRuntimeImpl()
	{
		// Release the font blend state instance
		if (nullptr != mFontBlendState)
		{
			mFontBlendState->release();
		}

		// Release the font sampler state instance
		if (nullptr != mFontSamplerState)
		{
			mFontSamplerState->release();
		}

		// Release the font vertex array instance
		if (nullptr != mFontVertexArray)
		{
			mFontVertexArray->release();
		}

		// Release the font fragment shader uniform buffer instance
		if (nullptr != mFontFragmentShaderUniformBuffer)
		{
			mFontFragmentShaderUniformBuffer->release();
		}

		// Release the font vertex shader uniform buffer instance
		if (nullptr != mFontVertexShaderUniformBuffer)
		{
			mFontVertexShaderUniformBuffer->release();
		}

		// Release the font program instance
		if (nullptr != mFontProgram)
		{
			mFontProgram->release();
		}

		// Destroy the manager instances
		delete mAssetManager;
		delete mCompositorManager;
		delete mSceneManager;

		// Destroy the resource manager instances
		delete mFontResourceManager;
		delete mTextureResourceManager;
		delete mMaterialResourceManager;
		delete mMeshResourceManager;

		// Release our renderer reference
		mRenderer->release();
	}

	Renderer::IProgram *RendererRuntimeImpl::getFontProgram()
	{
		// Create the font program instance right now?
		if (nullptr == mFontProgram)
		{
			// Decide which shader language should be used (for example "GLSL" or "HLSL")
			Renderer::IShaderLanguage *shaderLanguage = mRenderer->getShaderLanguage();
			if (nullptr != shaderLanguage)
			{
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderSourceCode = nullptr;
				const char *fragmentShaderSourceCode = nullptr;
				#include "Resource/Font/Font_GLSL_110.h"
				#include "Resource/Font/Font_GLSL_ES2.h"
				#include "Resource/Font/Font_HLSL_D3D9.h"
				#include "Resource/Font/Font_HLSL_D3D10_D3D11.h"
				#include "Resource/Font/Font_Null.h"

				// Create the program
				mFontProgram = shaderLanguage->createProgram(
					shaderLanguage->createVertexShader(vertexShaderSourceCode),
					shaderLanguage->createFragmentShader(fragmentShaderSourceCode));
				if (nullptr != mFontProgram)
				{
					// Add our internal reference
					mFontProgram->addReference();
				}
			}
		}

		// Return the instance of the font program
		return mFontProgram;
	}

	Renderer::IUniformBuffer *RendererRuntimeImpl::getFontVertexShaderUniformBuffer()
	{
		// Create the font vertex shader uniform buffer instance right now?
		if (nullptr == mFontVertexShaderUniformBuffer)
		{
			// Decide which shader language should be used (for example "GLSL" or "HLSL")
			Renderer::IShaderLanguage *shaderLanguage = mRenderer->getShaderLanguage();
			if (nullptr != shaderLanguage)
			{
				// Create uniform buffer
				// -> Direct3D 9 and OpenGL ES 2 do not support uniform buffers
				// -> Direct3D 10 and Direct3D 11 do not support individual uniforms
				// -> The renderer is just a light weight abstraction layer, so we need to handle the differences
				if (0 == strcmp(mRenderer->getName(), "Direct3D10") || 0 == strcmp(mRenderer->getName(), "Direct3D11"))
				{
					// Allocate enough memory
					mFontVertexShaderUniformBuffer = shaderLanguage->createUniformBuffer(sizeof(float) * 4 * 2 + sizeof(float) * 4 * 4, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
				}
			}
		}

		// Return the instance of the font vertex shader uniform buffer
		return mFontVertexShaderUniformBuffer;
	}

	Renderer::IUniformBuffer *RendererRuntimeImpl::getFontFragmentShaderUniformBuffer()
	{
		// Create the font fragment shader uniform buffer instance right now?
		if (nullptr == mFontFragmentShaderUniformBuffer)
		{
			// Decide which shader language should be used (for example "GLSL" or "HLSL")
			Renderer::IShaderLanguage *shaderLanguage = mRenderer->getShaderLanguage();
			if (nullptr != shaderLanguage)
			{
				// Create uniform buffer
				// -> Direct3D 9 and OpenGL ES 2 do not support uniform buffers
				// -> Direct3D 10 and Direct3D 11 do not support individual uniforms
				// -> The renderer is just a light weight abstraction layer, so we need to handle the differences
				if (0 == strcmp(mRenderer->getName(), "Direct3D10") || 0 == strcmp(mRenderer->getName(), "Direct3D11"))
				{
					// Allocate enough memory
					mFontFragmentShaderUniformBuffer = shaderLanguage->createUniformBuffer(sizeof(float) * 4, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
				}
			}
		}

		// Return the instance of the font fragment shader uniform buffer
		return mFontFragmentShaderUniformBuffer;
	}

	Renderer::IVertexArray *RendererRuntimeImpl::getVertexArray()
	{
		// Create the font vertex array instance right now?
		if (nullptr == mFontVertexArray)
		{
			// Get the font program, we need it in order to create the vertex array
			Renderer::IProgram *program = getFontProgram();
			if (nullptr != program)
			{
				// Create the vertex buffer object (VBO)
				// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
				static const float VERTEX_POSITION[] =
				{						// Vertex ID	Triangle strip on screen
					0.0f, 0.0f,	0.0f,	// 0			  1.......3
					0.0f, 1.0f,	1.0f,	// 1			  .	  .   .
					1.0f, 0.0f,	2.0f,	// 2			  0.......2
					1.0f, 1.0f,	3.0f	// 3
				};
				Renderer::IVertexBuffer *vertexBuffer = mRenderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW);

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
						"Position",								// name[32] (char)
						"POSITION",								// semantic[32] (char)
						0,										// semanticIndex (uint32_t)
						// Data source
						vertexBuffer,							// vertexBuffer (Renderer::IVertexBuffer *)
						0,										// offset (uint32_t)
						sizeof(float) * 3,						// stride (uint32_t)
						// Data source, instancing part
						0										// instancesPerElement (uint32_t)
					}
				};
				mFontVertexArray = program->createVertexArray(sizeof(vertexArray) / sizeof(Renderer::VertexArrayAttribute), vertexArray);
				if (nullptr != mFontVertexArray)
				{
					// Add our internal reference
					mFontVertexArray->addReference();
				}
			}
		}

		// Return the instance of the font vertex array
		return mFontVertexArray;
	}

	Renderer::ISamplerState *RendererRuntimeImpl::getFontSamplerState()
	{
		// Create the font sampler state instance right now?
		if (nullptr == mFontSamplerState)
		{
			// Create sampler state
			Renderer::SamplerState samplerState = Renderer::ISamplerState::getDefaultSamplerState();
			samplerState.addressU = Renderer::TextureAddressMode::CLAMP;
			samplerState.addressV = Renderer::TextureAddressMode::CLAMP;
			mFontSamplerState = mRenderer->createSamplerState(samplerState);

			// Add our internal reference
			mFontSamplerState->addReference();
		}

		// Return the instance of the font sampler state
		return mFontSamplerState;
	}

	Renderer::IBlendState *RendererRuntimeImpl::getFontBlendState()
	{
		// Create the font blend state instance right now?
		if (nullptr == mFontBlendState)
		{
			// Create blend state
			Renderer::BlendState blendState = Renderer::IBlendState::getDefaultBlendState();
			blendState.renderTarget[0].blendEnable = true;
			blendState.renderTarget[0].srcBlend	   = Renderer::Blend::SRC_ALPHA;
			blendState.renderTarget[0].destBlend   = Renderer::Blend::ONE;
			mFontBlendState = mRenderer->createBlendState(blendState);

			// Add our internal reference
			mFontBlendState->addReference();
		}

		// Return the instance of the font blend state
		return mFontBlendState;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
