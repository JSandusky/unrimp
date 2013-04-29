/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
#include "RendererToolkit/RendererToolkitImpl.h"
#include "RendererToolkit/FreeTypeContext.h"
#include "RendererToolkit/FontTextureFreeType.h"

#include <Renderer/IProgram.h>
#include <Renderer/IRenderer.h>
#include <Renderer/IBlendState.h>
#include <Renderer/IVertexArray.h>
#include <Renderer/ISamplerState.h>
#include <Renderer/IUniformBuffer.h>
#include <Renderer/PlatformTypes.h>
#include <Renderer/IShaderLanguage.h>
#include <Renderer/VertexArrayTypes.h>

#include <string.h>	// For "strcmp()"


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef RENDERERTOOLKIT_EXPORTS
	#define RENDERERTOOLKIT_API_EXPORT GENERIC_API_EXPORT
#else
	#define RENDERERTOOLKIT_API_EXPORT
#endif
RENDERERTOOLKIT_API_EXPORT RendererToolkit::IRendererToolkit *createRendererToolkitInstance(Renderer::IRenderer &renderer)
{
	return new RendererToolkit::RendererToolkitImpl(renderer);
}
#undef RENDERERTOOLKIT_API_EXPORT


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RendererToolkitImpl::RendererToolkitImpl(Renderer::IRenderer &renderer) :
		mFreeTypeContext(nullptr),
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
	}

	RendererToolkitImpl::~RendererToolkitImpl()
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

		// Release the FreeType context instance
		if (nullptr != mFreeTypeContext)
		{
			mFreeTypeContext->release();
		}

		// Release our renderer reference
		mRenderer->release();
	}

	FreeTypeContext &RendererToolkitImpl::getFreeTypeContext()
	{
		// Create the FreeType context instance right now?
		if (nullptr == mFreeTypeContext)
		{
			mFreeTypeContext = new FreeTypeContext(*mRenderer);
			mFreeTypeContext->addReference();	// Add our internal reference
		}

		// Return the FreeType context instance
		return *mFreeTypeContext;
	}

	Renderer::IProgram *RendererToolkitImpl::getFontProgram()
	{
		// Create the font program instance right now?
		if (nullptr == mFontProgram)
		{
			// Decide which shader language should be used (for example "GLSL", "HLSL" or "Cg")
			Renderer::IShaderLanguage *shaderLanguage = mRenderer->getShaderLanguage();
			if (nullptr != shaderLanguage)
			{
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderSourceCode = nullptr;
				const char *fragmentShaderSourceCode = nullptr;
				#include "Font_Cg.h"
				#include "Font_GLSL_110.h"
				#include "Font_GLSL_ES2.h"
				#include "Font_HLSL_D3D9.h"
				#include "Font_HLSL_D3D10_D3D11.h"
				#include "Font_Null.h"

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

	Renderer::IUniformBuffer *RendererToolkitImpl::getFontVertexShaderUniformBuffer()
	{
		// Create the font vertex shader uniform buffer instance right now?
		if (nullptr == mFontVertexShaderUniformBuffer)
		{
			// Decide which shader language should be used (for example "GLSL", "HLSL" or "Cg")
			Renderer::IShaderLanguage *shaderLanguage = mRenderer->getShaderLanguage();
			if (nullptr != shaderLanguage)
			{
				// Create uniform buffer
				// -> Direct3D 9 and OpenGL ES 2 do not support uniform buffers
				// -> Direct3D 10 and Direct3D 11 do not support individual uniforms
				// -> The renderer is just a light weight abstraction layer, so we need to handle the differences
				if (0 != strcmp(shaderLanguage->getShaderLanguageName(), "Cg") && (0 == strcmp(mRenderer->getName(), "Direct3D10") || 0 == strcmp(mRenderer->getName(), "Direct3D11")))
				{
					// Allocate enough memory
					mFontVertexShaderUniformBuffer = shaderLanguage->createUniformBuffer(sizeof(float) * 4 * 2 + sizeof(float) * 4 * 4, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
				}
			}
		}

		// Return the instance of the font vertex shader uniform buffer
		return mFontVertexShaderUniformBuffer;
	}

	Renderer::IUniformBuffer *RendererToolkitImpl::getFontFragmentShaderUniformBuffer()
	{
		// Create the font fragment shader uniform buffer instance right now?
		if (nullptr == mFontFragmentShaderUniformBuffer)
		{
			// Decide which shader language should be used (for example "GLSL", "HLSL" or "Cg")
			Renderer::IShaderLanguage *shaderLanguage = mRenderer->getShaderLanguage();
			if (nullptr != shaderLanguage)
			{
				// Create uniform buffer
				// -> Direct3D 9 and OpenGL ES 2 do not support uniform buffers
				// -> Direct3D 10 and Direct3D 11 do not support individual uniforms
				// -> The renderer is just a light weight abstraction layer, so we need to handle the differences
				if (0 != strcmp(shaderLanguage->getShaderLanguageName(), "Cg") && (0 == strcmp(mRenderer->getName(), "Direct3D10") || 0 == strcmp(mRenderer->getName(), "Direct3D11")))
				{
					// Allocate enough memory
					mFontFragmentShaderUniformBuffer = shaderLanguage->createUniformBuffer(sizeof(float) * 4, nullptr, Renderer::BufferUsage::DYNAMIC_DRAW);
				}
			}
		}

		// Return the instance of the font fragment shader uniform buffer
		return mFontFragmentShaderUniformBuffer;
	}

	Renderer::IVertexArray *RendererToolkitImpl::getVertexArray()
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
						"Position",								// name[64] (char)
						"POSITION",								// semantic[64] (char)
						0,										// semanticIndex (unsigned int)
						// Data source
						vertexBuffer,							// vertexBuffer (Renderer::IVertexBuffer *)
						0,										// offset (unsigned int)
						sizeof(float) * 3,						// stride (unsigned int)
						// Data source, instancing part
						0										// instancesPerElement (unsigned int)
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

	Renderer::ISamplerState *RendererToolkitImpl::getFontSamplerState()
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

	Renderer::IBlendState *RendererToolkitImpl::getFontBlendState()
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
	//[ Public virtual RendererToolkit::IRendererToolkit methods ]
	//[-------------------------------------------------------]
	IFont *RendererToolkitImpl::createFontTexture(const char *filename, unsigned int size, unsigned int resolution)
	{
		IFont *font = nullptr;

		// Is the given ASCII font filename valid?
		if (nullptr != filename && '\0' != filename[0])
		{
			// Create the font instance
			font = new FontTextureFreeType(*this, filename);

			// Is the font valid? If yes, should we set the font size and resolution right now?
			if (font->isValid() && size > 0 && resolution > 0)
			{
				// Set the font size
				font->setSize(size, resolution);
			}
		}

		// Return the created font instance
		return font;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
