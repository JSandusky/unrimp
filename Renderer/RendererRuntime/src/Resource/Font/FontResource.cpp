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
#include "RendererRuntime/Resource/Font/FontResource.h"
#include "RendererRuntime/Backend/RendererRuntimeImpl.h"

#include <cstring>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	FontResource::FontResource(RendererRuntimeImpl &rendererRuntimeImpl, ResourceId resourceId) :
		IResource(resourceId),
		mRendererRuntimeImpl(&rendererRuntimeImpl),
		mSize(0),
		mResolution(0),
		mAscender(0.0f),
		mDescender(0.0f),
		mHeight(0.0f),
		mGlyphTextureAtlasPadding(3),
		mGlyphTextureAtlasSizeX(0),
		mGlyphTextureAtlasSizeY(0),
		mNumberOfFontGlyphs(0),
		mFontGlyphs(nullptr),
		mTexture2D(nullptr)
	{
		// Add our renderer runtime implementation reference
		mRendererRuntimeImpl->addReference();
	}

	FontResource::~FontResource()
	{
		// Destroy the glyph texture atlas
		destroyGlyphTextureAtlas();

		// Release our renderer runtime implementation reference
		mRendererRuntimeImpl->release();
	}

	float FontResource::getTextWidth(const char *text)
	{
		float width = 0.0f;

		// Is the text valid?
		if (nullptr != text && '\0' != text[0])
		{
			// Are there any glyphs at all?
			if (mNumberOfFontGlyphs > 0)
			{
				// Get the length of the text, excluding the terminal null
				const size_t textLength = strlen(text);

				// Iterate through all characters of the text
				for (size_t i = 0; i < textLength; ++i, ++text)
				{
					// Get the character code
					const uint8_t characterCode = static_cast<uint8_t>(*text);

					// Get the glyph instance of the current character
					if (characterCode < mNumberOfFontGlyphs)
					{
						// Let the pen advance to the object space position of the next character
						width += mFontGlyphs[characterCode].penAdvanceX;
					}
				}
			}
		}

		// Return the width
		return width;
	}

	void FontResource::drawText(const char *text, const float *color, const float objectSpaceToClipSpace[16], float scaleX, float scaleY, float biasX, float biasY, uint32_t flags)
	{
		// Are the text and the text color valid?
		if (nullptr != text && '\0' != text[0] && nullptr != color)
		{
			// Get the used renderer instance
			Renderer::IRenderer &renderer = mRendererRuntimeImpl->getRenderer();

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

			// Get the font program
			Renderer::IProgram *program = mRendererRuntimeImpl->getFontProgram();
			if (nullptr != program)
			{
				// Set the used program
				renderer.setProgram(program);

				{ // Setup input assembly (IA)
					// Set the used vertex array
					renderer.iaSetVertexArray(mRendererRuntimeImpl->getVertexArray());

					// Set the primitive topology used for draw calls
					renderer.iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_STRIP);
				}

				{ // Upload the fragment shader uniform data
				  // -> Two versions: One using an uniform buffer and one setting an individual uniform

					// Try to get the font fragment shader uniform buffer
					Renderer::IUniformBuffer *uniformBuffer = mRendererRuntimeImpl->getFontFragmentShaderUniformBuffer();
					if (nullptr != uniformBuffer)
					{
						// Copy data
						uniformBuffer->copyDataFrom(sizeof(float) * 4, color);

						// Assign to stage
						renderer.fsSetUniformBuffer(program->getUniformBlockIndex("UniformBlockDynamicFs", 0), uniformBuffer);
					}
					else
					{
						program->setUniform4fv(program->getUniformHandle("Color"), color);
					}
				}

				{ // Set diffuse map (texture unit 0 by default)
					// Tell the renderer API which texture should be bound to which texture unit
					// -> When using OpenGL or OpenGL ES 2 this is required
					// -> OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension supports explicit binding points ("layout(binding = 0)"
					//    in GLSL shader) , for backward compatibility we don't use it in here
					// -> When using Direct3D 9, Direct3D 10 or Direct3D 11, the texture unit
					//    to use is usually defined directly within the shader by using the "register"-keyword
					// TODO(co) This should only be done once during initialization
					const uint32_t unit = program->setTextureUnit(program->getUniformHandle("GlyphMap"), 0);

					// Set the used texture at the texture unit
					renderer.fsSetTexture(unit, mTexture2D);

					// Set the used sampler state at the texture unit
					renderer.fsSetSamplerState(unit, mRendererRuntimeImpl->getFontSamplerState());
				}

				// Set the used blend state
				renderer.omSetBlendState(mRendererRuntimeImpl->getFontBlendState());

				// The current object space pen position
				float penPositionX = biasX;
				float penPositionY = biasY;

				// Center the text?
				if (flags & CENTER_TEXT)
				{
					penPositionX -= getTextWidth(text) / 2.0f;
				}

				// Iterate through all characters of the text to draw
				float penAdvanceX = 0.0f;
				float penAdvanceY = 0.0f;
				const char *textEnd = text + strlen(text);
				const char *currentText = text;
				for (; currentText < textEnd; ++currentText)
				{
					// Get the character code
					const uint8_t characterCode = static_cast<uint8_t>(*currentText);

					// Get the glyph instance of the current character
					if (characterCode < mNumberOfFontGlyphs)
					{
						// Get the glyph instance of the character to draw
						FontGlyphTexture &fontGlyphTexture = mFontGlyphs[characterCode];

						// Uniform block data we need to fill
						struct UniformBlockDynamicVs
						{
							float GlyphSizePenPosition[4];				// Object space glyph size (xy) and object space pen position (zw) => scale & bias
							float TextureCoordinateMinMax[4];			// The normalized minimum (xy) and maximum (zw) glyph texture coordinate inside the glyph texture atlas
							float ObjectSpaceToClipSpaceMatrix[4 * 4];	// Object space to clip space matrix
						};
						UniformBlockDynamicVs uniformBlockDynamicVS;

						{ // Get glyph size and pen position
							uniformBlockDynamicVS.GlyphSizePenPosition[0] = static_cast<float>(fontGlyphTexture.width)  * scaleX;	// Object space glyph x size in points   (x) => x scale
							uniformBlockDynamicVS.GlyphSizePenPosition[1] = static_cast<float>(fontGlyphTexture.height) * scaleY;	// Object space glyph y size in points   (y) => y scale
							uniformBlockDynamicVS.GlyphSizePenPosition[2] = (penPositionX + fontGlyphTexture.cornerX) * scaleX;		// Object space pen x position in points (z) => x bias
							uniformBlockDynamicVS.GlyphSizePenPosition[3] = (penPositionY + fontGlyphTexture.cornerY) * scaleY;		// Object space pen y position in points (w) => y bias
						}

						{ // Get the normalized minimum and maximum glyph texture coordinate inside the glyph texture atlas
							// Minimum
							uniformBlockDynamicVS.TextureCoordinateMinMax[0] = fontGlyphTexture.texCoordMinX;
							uniformBlockDynamicVS.TextureCoordinateMinMax[1] = fontGlyphTexture.texCoordMinY;

							// Maximum
							uniformBlockDynamicVS.TextureCoordinateMinMax[2] = fontGlyphTexture.texCoordMaxX;
							uniformBlockDynamicVS.TextureCoordinateMinMax[3] = fontGlyphTexture.texCoordMaxY;
						}

						{ // Upload the vertex shader uniform data
						  // -> Two versions: One using an uniform buffer and one setting an individual uniform

							// TODO(co) Optimize me by moving stuff out of the loop, "objectSpaceToClipSpace" is static and only needs to be send once
							// Try to get the font vertex shader uniform buffer
							Renderer::IUniformBuffer *uniformBuffer = mRendererRuntimeImpl->getFontVertexShaderUniformBuffer();
							if (nullptr != uniformBuffer)
							{
								// Set object space to clip space matrix
								memcpy(uniformBlockDynamicVS.ObjectSpaceToClipSpaceMatrix, objectSpaceToClipSpace, sizeof(float) * 4 * 4);

								// Copy data
								uniformBuffer->copyDataFrom(sizeof(UniformBlockDynamicVs), &uniformBlockDynamicVS);

								// Assign to stage
								renderer.vsSetUniformBuffer(program->getUniformBlockIndex("UniformBlockDynamicVs", 0), uniformBuffer);
							}
							else
							{
								program->setUniform4fv(program->getUniformHandle("GlyphSizePenPosition"), uniformBlockDynamicVS.GlyphSizePenPosition);
								program->setUniform4fv(program->getUniformHandle("TextureCoordinateMinMax"), uniformBlockDynamicVS.TextureCoordinateMinMax);
								program->setUniformMatrix4fv(program->getUniformHandle("ObjectSpaceToClipSpaceMatrix"), objectSpaceToClipSpace);
							}
						}

						// Render the specified geometric primitive, based on indexing into an array of vertices
						renderer.draw(0, 4);

						// Let the pen advance to the object space position of the next character
						penPositionX += mFontGlyphs[characterCode].penAdvanceX;
					}
				}
			}

			// End debug event
			RENDERER_END_DEBUG_EVENT(&renderer)
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void FontResource::destroyGlyphTextureAtlas()
	{
		// Release the glyph texture atlas
		if (nullptr != mTexture2D)
		{
			mTexture2D->release();
			mTexture2D = nullptr;
		}

		// Destroy the array of currently active glyphs
		if (nullptr != mFontGlyphs)
		{
			delete [] mFontGlyphs;
			mFontGlyphs = nullptr;
			mNumberOfFontGlyphs = 0;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
