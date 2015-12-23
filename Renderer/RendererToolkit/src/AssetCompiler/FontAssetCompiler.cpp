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
#include "RendererToolkit/AssetCompiler/FontAssetCompiler.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Resource/Font/Loader/FontFileFormat.h>

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4668)	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'

	#include <ft2build.h>
	#include FT_FREETYPE_H
	#include <ftglyph.h>
#pragma warning(pop)

#include <memory>
#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Global functions                                      ]
	//[-------------------------------------------------------]
	namespace detail
	{
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
		//[ Classes                                               ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    FreeType font glyph texture class
		*/
		class FontGlyphTexture
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			/**
			*  @brief
			*    Default constructor
			*/
			inline FontGlyphTexture() :
				// Font glyph
				mWidth(0),
				mHeight(0),
				mCornerX(0.0f),
				mCornerY(0.0f),
				mPenAdvanceX(0.0f),
				mPenAdvanceY(0.0f),
				// Texture font glyph specific
				mTexCoordMinX(0.0f),
				mTexCoordMinY(0.0f),
				mTexCoordMaxX(0.0f),
				mTexCoordMaxY(0.0f)
			{
				// Nothing to do in here
			}

			/**
			*  @brief
			*    Initialize the glyph
			*
			*  @param[in]  characterCode
			*    Character code
			*  @param[in]  positionOffsetX
			*    X position offset (in texel) of the glyph inside the font texture atlas
			*  @param[in]  positionOffsetY
			*    Y position offset (in texel) of the glyph inside the font texture atlas
			*  @param[out] glyphTextureAtlasData
			*    Pointer to the glyph texture atlas data to fill, must be valid! (data is not destroyed or cached inside this method)
			*/
			void initialize(FT_Face& ftFace, uint32_t glyphTextureAtlasSizeX, uint32_t glyphTextureAtlasSizeY, unsigned long characterCode, uint32_t positionOffsetX, uint32_t positionOffsetY, uint8_t *glyphTextureAtlasData)
			{
				// TODO(co) FreeType error code to human readable string

				// Load the Glyph for the character
				if (0 == FT_Load_Glyph(ftFace, FT_Get_Char_Index(ftFace, characterCode), FT_LOAD_DEFAULT))
				{
					// Move the glyph within the glyph slot of the face object into a glyph object
					FT_Glyph ftGlyph;
					if (0 == FT_Get_Glyph(ftFace->glyph, &ftGlyph))
					{
						// Convert the glyph to a bitmap
						FT_Glyph_To_Bitmap(&ftGlyph, ft_render_mode_normal, 0, 1);
						const FT_BitmapGlyph ftBitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(ftGlyph);

						// Get a reference to the glyph bitmap
						const FT_Bitmap &ftBitmap = ftBitmapGlyph->bitmap;

						// Get the size (in pixel) of the glyph
						mWidth  = static_cast<int>(ftBitmap.width);
						mHeight = static_cast<int>(ftBitmap.rows);

						{ // Copy the glyph bitmap into the glyph texture atlas
							// Get the X component of the glyph texture atlas size (in pixel)
							const uint32_t numberOfBytesPerRow = glyphTextureAtlasSizeX;

							// Copy all rows (one byte per texel)
							uint8_t *currentGlyphTextureAtlasData = glyphTextureAtlasData + positionOffsetX + numberOfBytesPerRow * positionOffsetY;
							uint8_t *endGlyphTextureAtlasData	  = currentGlyphTextureAtlasData + numberOfBytesPerRow * mHeight;
							uint8_t *currentFTBitmapBuffer		  = ftBitmap.buffer;
							for (; currentGlyphTextureAtlasData < endGlyphTextureAtlasData; currentGlyphTextureAtlasData += numberOfBytesPerRow, currentFTBitmapBuffer += mWidth)
							{
								// Copy the current row part
								memcpy(currentGlyphTextureAtlasData, currentFTBitmapBuffer, static_cast<size_t>(mWidth));
							}
						}

						// Set the distance (in pixel) from the current pen position to the glyph bitmap
						mCornerX = static_cast<float>(ftBitmapGlyph->left);
						mCornerY = static_cast<float>(ftBitmapGlyph->top) - mHeight;

						// Set the pen advance, the FreeType library measures font size in terms of 1/64ths of pixels, so we have to adjust with /64
						mPenAdvanceX = static_cast<float>(ftFace->glyph->advance.x) / 64.0f;
						mPenAdvanceY = static_cast<float>(ftFace->glyph->advance.y) / 64.0f;

						// Calculate the normalized minimum glyph texture coordinate inside the glyph texture atlas
						mTexCoordMinX = static_cast<float>(positionOffsetX) / static_cast<float>(glyphTextureAtlasSizeX);
						mTexCoordMinY = static_cast<float>(positionOffsetY) / static_cast<float>(glyphTextureAtlasSizeY);

						// Calculate the normalized maximum glyph texture coordinate inside the glyph texture atlas
						mTexCoordMaxX = static_cast<float>(positionOffsetX + mWidth)  / static_cast<float>(glyphTextureAtlasSizeX);
						mTexCoordMaxY = static_cast<float>(positionOffsetY + mHeight) / static_cast<float>(glyphTextureAtlasSizeY);

						// Release the created FreeType glyph
						FT_Done_Glyph(ftGlyph);
					}
				}
			}

			/**
			*  @brief
			*    Destructor
			*/
			inline ~FontGlyphTexture()
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			// Font glyph
			int   mWidth;		///< The width (in pixel) of the glyph
			int   mHeight;		///< The height (in pixel) of the glyph
			float mCornerX;		///< The X distance (in pixel) from the current pen position to the glyph bitmap
			float mCornerY;		///< The Y distance (in pixel) from the current pen position to the glyph bitmap
			float mPenAdvanceX;	///< The object space pen X advance
			float mPenAdvanceY;	///< The object space pen Y advance
			// Texture font glyph specific
			float mTexCoordMinX;	///< X coordinate of the normalized minimum glyph texture coordinate inside the glyph texture atlas
			float mTexCoordMinY;	///< Y coordinate of the normalized minimum glyph texture coordinate inside the glyph texture atlas
			float mTexCoordMaxX;	///< X coordinate of the normalized maximum glyph texture coordinate inside the glyph texture atlas
			float mTexCoordMaxY;	///< Y coordinate of the normalized maximum glyph texture coordinate inside the glyph texture atlas


		};

	}


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const AssetCompilerTypeId FontAssetCompiler::TYPE_ID("Font");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	FontAssetCompiler::FontAssetCompiler() :
		mFtLibrary(new FT_Library)
	{
		// Initialize the FreeType library object
		const FT_Error ftError = FT_Init_FreeType(mFtLibrary);
		if (0 != ftError)
		{
			// Error!
			delete mFtLibrary;
			mFtLibrary = nullptr;
		}
	}

	FontAssetCompiler::~FontAssetCompiler()
	{
		// Destroy the FreeType library object
		if (nullptr != mFtLibrary)
		{
			FT_Done_FreeType(*mFtLibrary);
			delete mFtLibrary;
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId FontAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	void FontAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Input, configuration and output
		const std::string&			   assetInputDirectory	= input.assetInputDirectory;
		const std::string&			   assetOutputDirectory	= input.assetOutputDirectory;
		Poco::JSON::Object::Ptr		   jsonAssetRootObject	= configuration.jsonAssetRootObject;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		Poco::JSON::Object::Ptr jsonAssetObject = jsonAssetRootObject->get("Asset").extract<Poco::JSON::Object::Ptr>();

		// We really need an valid FreeType library instance
		if (nullptr == mFtLibrary)
		{
			throw std::exception("Invalid FreeType library instance");
		}

		// Read configuration
		std::string inputFile;
		uint32_t size					  = 12;
		uint32_t resolution				  = 96;
		uint32_t glyphTextureAtlasPadding = 3;	// Glyph texture atlas gab between glyphs in pixel
		uint32_t glyphsPerRow			  = 16;
		uint32_t glyphsPerColumn		  = 16;
		uint32_t numberOfFontGlyphs		  = 256;
		{
			// Read font asset compiler configuration
			Poco::JSON::Object::Ptr jsonConfigurationObject = jsonAssetObject->get("FontAssetCompiler").extract<Poco::JSON::Object::Ptr>();
			inputFile				 = jsonConfigurationObject->getValue<std::string>("InputFile");
			size					 = jsonConfigurationObject->optValue<uint32_t>("Size", size);
			resolution				 = jsonConfigurationObject->optValue<uint32_t>("Resolution", resolution);
			glyphTextureAtlasPadding = jsonConfigurationObject->optValue<uint32_t>("GlyphTextureAtlasPadding", glyphTextureAtlasPadding);
			glyphsPerRow			 = jsonConfigurationObject->optValue<uint32_t>("GlyphsPerRow", glyphsPerRow);
			glyphsPerColumn			 = jsonConfigurationObject->optValue<uint32_t>("GlyphsPerColumn", glyphsPerColumn);
			numberOfFontGlyphs		 = jsonConfigurationObject->optValue<uint32_t>("NumberOfFontGlyphs", numberOfFontGlyphs);
		}

		// Open the input file and output file
		std::ifstream ifstream(assetInputDirectory + inputFile, std::ios::binary);
		const std::string assetName = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetName");
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".font";
		std::ofstream ofstream(outputAssetFilename, std::ios::binary);

		// Create the FreeType library face (aka "The Font")
		FT_Face ftFace;
		std::unique_ptr<FT_Byte[]> buffer;
		{
			// Get file size and file data
			ifstream.seekg(0, std::ifstream::end);
			const FT_Long numberOfBytes = static_cast<FT_Long>(ifstream.tellg());
			ifstream.seekg(0, std::ifstream::beg);
			buffer = std::unique_ptr<FT_Byte[]>(new FT_Byte[static_cast<size_t>(numberOfBytes)]);
			ifstream.read((char*)buffer.get(), numberOfBytes);

			// Create the FreeType library face
			if (0 != FT_New_Memory_Face(*mFtLibrary, buffer.get(), numberOfBytes, 0, &ftFace))
			{
				// Error!
				// TODO(co) FreeType error code to human readable string
				throw std::exception("Failed to create the FreeType face instance");
			}
			else
			{
				// TODO(co) Error handling
			}

			// The FreeType library measures font size in terms of 1/64ths of pixels, so we have to adjust with *64
			if (0 != FT_Set_Char_Size(ftFace, 0L, static_cast<FT_F26Dot6>(size * 64), resolution, resolution))
			{
				// Error!
				// TODO(co) FreeType error code to human readable string
				throw std::exception("Failed to set the FreeType face character size");
			}
		}

		// Fill the font header
		RendererRuntime::v1Font::Header fontHeader;
		fontHeader.formatType			  = RendererRuntime::v1Font::FORMAT_TYPE;
		fontHeader.formatVersion		  = RendererRuntime::v1Font::FORMAT_VERSION;
		fontHeader.size					  = size;
		fontHeader.resolution			  = resolution;
		fontHeader.ascender				  = static_cast<float>(ftFace->size->metrics.ascender) / 64.0f;
		fontHeader.descender			  = static_cast<float>(ftFace->size->metrics.descender) / 64.0f;
		fontHeader.height				  = static_cast<float>(ftFace->size->metrics.height) / 64.0f;
		fontHeader.numberOfFontGlyphs	  = numberOfFontGlyphs;
		fontHeader.glyphTextureAtlasSizeX = detail::getNearestPowerOfTwo(static_cast<uint32_t>(glyphTextureAtlasPadding + glyphsPerColumn * (fontHeader.height + glyphTextureAtlasPadding)), false);
		fontHeader.glyphTextureAtlasSizeY = detail::getNearestPowerOfTwo(static_cast<uint32_t>(glyphTextureAtlasPadding + glyphsPerRow * (fontHeader.height + glyphTextureAtlasPadding)), false);

		// Write down the font header
		ofstream.write(reinterpret_cast<const char*>(&fontHeader), sizeof(RendererRuntime::v1Font::Header));

		{ // Fill the font glyphs and data
			// Allocate memory for the glyph texture atlas and initialize it with zero to avoid sampling artefacts later on
			const uint32_t totalNumberOfBytes = fontHeader.glyphTextureAtlasSizeX * fontHeader.glyphTextureAtlasSizeY; // Alpha, one byte
			uint8_t* glyphTextureAtlasData = new uint8_t[totalNumberOfBytes];
			memset(glyphTextureAtlasData, 0, totalNumberOfBytes);

			{ // Fill the glyph texture atlas - We've got 256 glyphs, this means there are 16 glyphs per row within the glyph texture atlas
				const uint32_t xDistanceToNextGlyph = static_cast<uint32_t>(fontHeader.height + glyphTextureAtlasPadding);
				const uint32_t yDistanceToNextGlyph = static_cast<uint32_t>(fontHeader.height + glyphTextureAtlasPadding);
				detail::FontGlyphTexture* fontGlyphTextures = new detail::FontGlyphTexture[fontHeader.numberOfFontGlyphs];
				detail::FontGlyphTexture* currentFontGlyphTexture = fontGlyphTextures;
				for (uint32_t y = 0, i = 0; y < glyphsPerRow; ++y)
				{
					for (uint32_t x = 0; x < glyphsPerColumn; ++x, ++i, ++currentFontGlyphTexture)
					{
						// Initialize the texture glyph
						currentFontGlyphTexture->initialize(ftFace, fontHeader.glyphTextureAtlasSizeX, fontHeader.glyphTextureAtlasSizeY, i, glyphTextureAtlasPadding + x * xDistanceToNextGlyph, glyphTextureAtlasPadding + y * yDistanceToNextGlyph, glyphTextureAtlasData);
					}
				}

				// Write down the font glyphs
				ofstream.write(reinterpret_cast<const char*>(fontGlyphTextures), sizeof(detail::FontGlyphTexture) * fontHeader.numberOfFontGlyphs);

				// Free allocated memory
				delete [] fontGlyphTextures;
			}

			// Write down the font data
			ofstream.write(reinterpret_cast<const char*>(glyphTextureAtlasData), totalNumberOfBytes);

			// Free allocated memory
			delete [] glyphTextureAtlasData;
		}

		{ // Update the output asset package
			const std::string assetCategory = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetCategory");
			const std::string assetIdAsString = input.projectName + "/Font/" + assetCategory + '/' + assetName;

			// Output asset
			RendererRuntime::Asset outputAsset;
			outputAsset.assetId = RendererRuntime::StringId(assetIdAsString.c_str());
			strcpy(outputAsset.assetFilename, outputAssetFilename.c_str());	// TODO(co) Buffer overflow test
			outputAssetPackage.getWritableSortedAssetVector().push_back(outputAsset);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
