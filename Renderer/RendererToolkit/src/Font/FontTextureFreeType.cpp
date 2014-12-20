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
#include "RendererToolkit/Font/FontTextureFreeType.h"
#include "RendererToolkit/Font/FreeTypeContext.h"
#include "RendererToolkit/Font/FontGlyphTextureFreeType.h"
#include "RendererToolkit/RendererToolkitImpl.h"

#include <Renderer/IProgram.h>
#include <Renderer/IRenderer.h>
#include <Renderer/ITexture2D.h>
#include <Renderer/IUniformBuffer.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <math.h>	// For "powf()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Global functions                                      ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Check whether the given the number is a power of 2
	*
	*  @param[in] number
	*    Number to check
	*
	*  @return
	*    "true" if the number is a power of 2, else "false"
	*/
	// TODO(co) "isPowerOfTwo()" is an generic helper function, move it elsewhere
	bool isPowerOfTwo(uint32_t number)
	{
		// 0 is not a power of 2, so we need to perform an additional test to catch this special case
		return (0 == number) ? false : (0 == (number & (number - 1)));
	}

	/**
	*  @brief
	*    Return the nearest power of 2
	*
	*  @param[in] number
	*    Number to check
	*  @param[in] lower
	*    "true" take the lower nearest number, else "false" takes the higher one
	*
	*  @return
	*    The nearest power of 2, if it couldn't be found "number"
	*/
	// TODO(co) "getNearestPowerOfTwo()" is an generic helper function, move it elsewhere
	uint32_t getNearestPowerOfTwo(uint32_t number, bool lower = true)
	{
		// 0 is not a power of 2, so we need to perform an additional test to catch this special case
		if (0 == number)
		{
			// There's no nearest power of 2 below 0, so return the given 0 as stated within the method documentation, else return 1
			return lower ? 0u : 1;
		}
		else
		{
			// Check whether this number is already a power of 2
			if (!isPowerOfTwo(number))
			{
				// 2^31 is our upper limit
				uint32_t previousNumber = 1;
				for (uint32_t i = 1; i < 32; ++i)
				{
					const uint32_t currentNumber = static_cast<uint32_t>(powf(2.0f, static_cast<float>(i)));
					if (number < currentNumber)
					{
						return lower ? previousNumber : currentNumber;
					}
					previousNumber = currentNumber;
				}
			}

			// Return the found number
			return number;
		}
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	FontTextureFreeType::FontTextureFreeType(RendererToolkitImpl &rendererToolkitImpl, const char *filename) :
		mRendererToolkitImpl(&rendererToolkitImpl),
		mFontFileSize(0),
		mFontFileData(nullptr),
		mFTFace(nullptr),
		mGlyphTextureAtlasPadding(3),
		mGlyphTextureAtlasSizeX(0),
		mGlyphTextureAtlasSizeY(0),
		mNumberOfFontGlyphs(0),
		mFontGlyphs(nullptr),
		mTexture2D(nullptr)
	{
		// Add our renderer toolkit implementation reference
		mRendererToolkitImpl->addReference();

		// Open the file
		// TODO(co) At the moment "fopen()" etc. are used directly
		FILE *file = fopen(filename, "rb");
		if (nullptr != file)
		{
			{ // Get the file size
				const long position = ftell(file);
				fseek(file, 0, SEEK_END);
				mFontFileSize = static_cast<uint32_t>(ftell(file));
				fseek(file, position, SEEK_SET);
			}

			// Read in the whole file
			mFontFileData = new uint8_t[mFontFileSize];
			fread(mFontFileData, sizeof(uint8_t), mFontFileSize, file);

			// Close the file
			fclose(file);

			// Create the FreeType library face
			mFTFace = new FT_Face;
			if (0 != FT_New_Memory_Face(*rendererToolkitImpl.getFreeTypeContext().getFTLibrary(), static_cast<FT_Byte const*>(mFontFileData), static_cast<FT_Long>(mFontFileSize), 0, mFTFace))
			{
				// Error!
				delete mFTFace;
				mFTFace = nullptr;
			}
			else
			{
				// TODO(co) Error handling
			}
		}
		else
		{
			// TODO(co) Error handling
		}
	}

	FontTextureFreeType::~FontTextureFreeType()
	{
		// Destroy the glyph texture atlas
		destroyGlyphTextureAtlas();

		// Destroy the FreeType library face
		if (nullptr != mFTFace)
		{
			FT_Done_Face(*mFTFace);
			delete mFTFace;
		}

		// Destroy the cached font file data
		if (nullptr != mFontFileData)
		{
			delete [] mFontFileData;
		}

		// Release our renderer toolkit implementation reference
		mRendererToolkitImpl->release();
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IFont methods         ]
	//[-------------------------------------------------------]
	bool FontTextureFreeType::setSize(uint32_t size, uint32_t resolution)
	{
		// Destroy the glyph texture atlas - it's now dirty
		destroyGlyphTextureAtlas();

		// Is there a FreeType library face?
		if (nullptr != mFTFace)
		{
			// The FreeType library measures font size in terms of 1/64ths of pixels, so we have to adjust with *64
			if (0 == FT_Set_Char_Size(*mFTFace, 0L, static_cast<FT_F26Dot6>(size * 64), resolution, resolution))
			{
				// All went fine
				mSize       = size;
				mResolution = resolution;
				return true;
			}
		}

		// Error!
		mSize       = 0;
		mResolution = 0;
		return false;
	}

	float FontTextureFreeType::getAscender() const
	{
		// The FreeType library measures font size in terms of 1/64ths of pixels, so we have to adjust with /64
		return (nullptr != mFTFace) ? static_cast<float>((*mFTFace)->size->metrics.ascender) / 64.0f : 0.0f;
	}

	float FontTextureFreeType::getDescender() const
	{
		// The FreeType library measures font size in terms of 1/64ths of pixels, so we have to adjust with /64
		return (nullptr != mFTFace) ? static_cast<float>((*mFTFace)->size->metrics.descender) / 64.0f : 0.0f;
	}

	float FontTextureFreeType::getHeight() const
	{
		// The FreeType library measures font size in terms of 1/64ths of pixels, so we have to adjust with /64
		return (nullptr != mFTFace) ? static_cast<float>((*mFTFace)->size->metrics.height) / 64.0f : 0.0f;
	}

	float FontTextureFreeType::getTextWidth(const char *text)
	{
		float width = 0.0f;

		// Is the text valid?
		if (nullptr != text && '\0' != text[0])
		{
			// Create glyph texture atlas if required
			if (0 == mNumberOfFontGlyphs)
			{
				createGlyphTextureAtlas();
			}

			// Are there any glyphs at all?
			if (mNumberOfFontGlyphs > 0)
			{
				// Get the length of the text, excluding the terminal null
				const size_t textLength = strlen(text);

				// Iterate through all characters of the text
				float penAdvanceX = 0.0f;
				float penAdvanceY = 0.0f;
				for (size_t i = 0; i < textLength; ++i, ++text)
				{
					// Get the character code
					const uint8_t characterCode = static_cast<uint8_t>(*text);

					// Get the glyph instance of the current character
					if (characterCode < mNumberOfFontGlyphs)
					{
						// Let the pen advance to the object space position of the next character
						mFontGlyphs[characterCode].getPenAdvance(penAdvanceX, penAdvanceY);
						width += penAdvanceX;
					}
				}
			}
		}

		// Return the width
		return width;
	}

	void FontTextureFreeType::drawText(const char *text, const float *color, const float objectSpaceToClipSpace[16], float scaleX, float scaleY, float biasX, float biasY, uint32_t flags)
	{
		// Are the text and the text color valid?
		if (nullptr != text && '\0' != text[0] && nullptr != color)
		{
			// Get the used renderer instance
			Renderer::IRenderer &renderer = mRendererToolkitImpl->getRenderer();

			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&renderer)

			// Get the font program
			Renderer::IProgram *program = mRendererToolkitImpl->getFontProgram();
			if (nullptr != program)
			{
				// Create glyph texture atlas if required
				if (0 == mNumberOfFontGlyphs)
				{
					createGlyphTextureAtlas();
				}

				// Set the used program
				renderer.setProgram(program);

				{ // Setup input assembly (IA)
					// Set the used vertex array
					renderer.iaSetVertexArray(mRendererToolkitImpl->getVertexArray());

					// Set the primitive topology used for draw calls
					renderer.iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_STRIP);
				}

				{ // Upload the fragment shader uniform data
				  // -> Two versions: One using an uniform buffer and one setting an individual uniform

					// Try to get the font fragment shader uniform buffer
					Renderer::IUniformBuffer *uniformBuffer = mRendererToolkitImpl->getFontFragmentShaderUniformBuffer();
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
					// -> OpenGL 4.2 supports explicit binding points ("layout(binding=0)" in GLSL shader),
					//    for backward compatibility we don't use it in here
					// -> When using Direct3D 9, Direct3D 10 or Direct3D 11, the texture unit
					//    to use is usually defined directly within the shader by using the "register"-keyword
					// TODO(co) This should only be done once during initialization
					const uint32_t unit = program->setTextureUnit(program->getUniformHandle("GlyphMap"), 0);

					// Set the used texture at the texture unit
					renderer.fsSetTexture(unit, mTexture2D);

					// Set the used sampler state at the texture unit
					renderer.fsSetSamplerState(unit, mRendererToolkitImpl->getFontSamplerState());
				}

				// Set the used blend state
				renderer.omSetBlendState(mRendererToolkitImpl->getFontBlendState());

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
						FontGlyphTextureFreeType &fontGlyphTextureFreeType = mFontGlyphs[characterCode];

						// Uniform block data we need to fill
						struct UniformBlockDynamicVs
						{
							float GlyphSizePenPosition[4];				// Object space glyph size (xy) and object space pen position (zw) => scale & bias
							float TextureCoordinateMinMax[4];			// The normalized minimum (xy) and maximum (zw) glyph texture coordinate inside the glyph texture atlas
							float ObjectSpaceToClipSpaceMatrix[4 * 4];	// Object space to clip space matrix
						};
						UniformBlockDynamicVs uniformBlockDynamicVS;

						{ // Get glyph size and pen position
							int width  = 0;
							int height = 0;
							fontGlyphTextureFreeType.getWidthAndHeight(width, height);
							float cornerX = 0.0f;
							float cornerY = 0.0f;
							fontGlyphTextureFreeType.getCorner(cornerX, cornerY);
							uniformBlockDynamicVS.GlyphSizePenPosition[0] = static_cast<float>(width)  * scaleX;	// Object space glyph x size in points   (x) => x scale
							uniformBlockDynamicVS.GlyphSizePenPosition[1] = static_cast<float>(height) * scaleY;	// Object space glyph y size in points   (y) => y scale
							uniformBlockDynamicVS.GlyphSizePenPosition[2] = (penPositionX + cornerX) * scaleX;	// Object space pen x position in points (z) => x bias
							uniformBlockDynamicVS.GlyphSizePenPosition[3] = (penPositionY + cornerY) * scaleY;	// Object space pen y position in points (w) => y bias
						}

						{ // Get the normalized minimum and maximum glyph texture coordinate inside the glyph texture atlas
							fontGlyphTextureFreeType.getTexCoordMin(uniformBlockDynamicVS.TextureCoordinateMinMax[0], uniformBlockDynamicVS.TextureCoordinateMinMax[1]);
							fontGlyphTextureFreeType.getTexCoordMax(uniformBlockDynamicVS.TextureCoordinateMinMax[2], uniformBlockDynamicVS.TextureCoordinateMinMax[3]);
						}

						{ // Upload the vertex shader uniform data
						  // -> Two versions: One using an uniform buffer and one setting an individual uniform

							// TODO(co) Optimize me by moving stuff out of the loop, "objectSpaceToClipSpace" is static and only needs to be send once
							// Try to get the font vertex shader uniform buffer
							Renderer::IUniformBuffer *uniformBuffer = mRendererToolkitImpl->getFontVertexShaderUniformBuffer();
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
						mFontGlyphs[characterCode].getPenAdvance(penAdvanceX, penAdvanceY);
						penPositionX += penAdvanceX;
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
	void FontTextureFreeType::calculateGlyphTextureAtlasSize(uint32_t &glyphTextureAtlasSizeX, uint32_t &glyphTextureAtlasSizeY)
	{
		// Get the maximum supported renderer texture size
		const uint32_t maximumTextureDimension = mRendererToolkitImpl->getFreeTypeContext().getRenderer().getCapabilities().maximumTextureDimension;
		if (maximumTextureDimension > 0)
		{
			// Get the font height in pixels
			const uint32_t fontHeight = getHeightInPixels();
			if (fontHeight > 0)
			{
				// We've got 256 glyphs, this means there are 16 glyphs per row within the glyph texture atlas
				glyphTextureAtlasSizeX = getNearestPowerOfTwo(mGlyphTextureAtlasPadding + 16 * (fontHeight + mGlyphTextureAtlasPadding), false);
				glyphTextureAtlasSizeY = getNearestPowerOfTwo(mGlyphTextureAtlasPadding + 16 * (fontHeight + mGlyphTextureAtlasPadding), false);

				// Check texture size - we just "cut off" everything that doesn't fit into the texture (... normally, this is not happening... so, keep it simple!)
				if (glyphTextureAtlasSizeX > maximumTextureDimension)
				{
					glyphTextureAtlasSizeX = maximumTextureDimension;
				}
				if (glyphTextureAtlasSizeY > maximumTextureDimension)
				{
					glyphTextureAtlasSizeY = maximumTextureDimension;
				}
			}
			else
			{
				// Error! Set known default values...
				glyphTextureAtlasSizeX = glyphTextureAtlasSizeY = 0;
			}
		}
		else
		{
			// Error! Set known default values...
			glyphTextureAtlasSizeX = glyphTextureAtlasSizeY = 0;
		}
	}

	void FontTextureFreeType::createGlyphTextureAtlas()
	{
		// Destroy the previous glyph texture atlas
		destroyGlyphTextureAtlas();

		// Check the FreeType library face object (aka "The Font")
		if (nullptr != mFTFace)
		{
			// Calculate the glyph texture atlas size using the current glyph settings and hardware capabilities
			calculateGlyphTextureAtlasSize(mGlyphTextureAtlasSizeX, mGlyphTextureAtlasSizeY);

			// Valid glyph texture atlas size?
			if (mGlyphTextureAtlasSizeX > 0 && mGlyphTextureAtlasSizeY > 0 )
			{
				// Allocate memory for the glyph texture atlas and initialize it with zero to avoid sampling artefacts later on
				const uint32_t totalNumberOfBytes = getGlyphTextureAtlasNumberOfBytes(false);
				uint8_t *glyphTextureAtlasData = new uint8_t[totalNumberOfBytes];
				memset(glyphTextureAtlasData, 0, totalNumberOfBytes);

				// Get the font height in pixels (if we're here, we already now that it's valid!)
				const uint32_t fontHeight = getHeightInPixels();

				{ // Fill the glyph texture atlas - We've got 256 glyphs, this means there are 16 glyphs per row within the glyph texture atlas
					const uint32_t numberOfGlyphs		= 256;
					const uint32_t glyphsPerRow			= 16;
					const uint32_t glyphsPerColumn		= 16;
					const uint32_t xDistanceToNextGlyph = fontHeight + mGlyphTextureAtlasPadding;
					const uint32_t yDistanceToNextGlyph = fontHeight + mGlyphTextureAtlasPadding;
					mFontGlyphs = new FontGlyphTextureFreeType[numberOfGlyphs];
					FontGlyphTextureFreeType *currentFontGlyphs = mFontGlyphs;
					for (uint32_t y = 0, i = 0; y < glyphsPerRow; ++y)
					{
						for (uint32_t x = 0; x < glyphsPerColumn; ++x, ++i, ++currentFontGlyphs)
						{
							// Initialize the texture glyph
							currentFontGlyphs->initialize(*this, i, mGlyphTextureAtlasPadding + x * xDistanceToNextGlyph, mGlyphTextureAtlasPadding + y * yDistanceToNextGlyph, glyphTextureAtlasData);
						}
					}
					mNumberOfFontGlyphs = numberOfGlyphs;
				}

				{ // Renderer related part
					// Begin debug event
					RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(&mRendererToolkitImpl->getFreeTypeContext().getRenderer())

					// Create the glyph texture atlas and add our internal reference
					mTexture2D = mRendererToolkitImpl->getFreeTypeContext().getRenderer().createTexture2D(mGlyphTextureAtlasSizeX, mGlyphTextureAtlasSizeY, Renderer::TextureFormat::A8, glyphTextureAtlasData, Renderer::TextureFlag::MIPMAPS);
					if (nullptr != mTexture2D)
					{
						RENDERER_SET_RESOURCE_DEBUG_NAME(mTexture2D, FT_Get_Postscript_Name(*mFTFace))
						mTexture2D->addReference();
					}

					// End debug event
					RENDERER_END_DEBUG_EVENT(&mRendererToolkitImpl->getFreeTypeContext().getRenderer())
				}

				// Free memory of the glyph texture atlas
				delete [] glyphTextureAtlasData;
			}
		}
	}

	uint32_t FontTextureFreeType::getGlyphTextureAtlasNumberOfBytes(bool includeMipmaps) const
	{
		// Include mipmaps?
		if (includeMipmaps)
		{
			uint32_t glyphTextureAtlasSizeX = mGlyphTextureAtlasSizeX;
			uint32_t glyphTextureAtlasSizeY = mGlyphTextureAtlasSizeY;
			uint32_t totalNumOfBytes = 0;

			// Go down the mipmap chain
			while (glyphTextureAtlasSizeX > 1 || glyphTextureAtlasSizeY > 1)
			{
				// Add memory from this mipmap level
				totalNumOfBytes += glyphTextureAtlasSizeX * glyphTextureAtlasSizeY; // Alpha, one byte

				// Next mipmap level, please
				glyphTextureAtlasSizeX >>= 1;	// /=2 using faster bit shifts
				glyphTextureAtlasSizeY >>= 1;	// /=2 using faster bit shifts
				if (glyphTextureAtlasSizeX < 1)
				{
					glyphTextureAtlasSizeX = 1;
				}
				if (glyphTextureAtlasSizeY < 1)
				{
					glyphTextureAtlasSizeY = 1;
				}
			}

			// Don't forget the last tiny one :D
			totalNumOfBytes += glyphTextureAtlasSizeX * glyphTextureAtlasSizeY; // Alpha, one byte

			// Done
			return totalNumOfBytes;
		}
		else
		{
			// Just base map
			return mGlyphTextureAtlasSizeX * mGlyphTextureAtlasSizeY; // Alpha, one byte
		}
	}

	void FontTextureFreeType::destroyGlyphTextureAtlas()
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
} // RendererToolkit
