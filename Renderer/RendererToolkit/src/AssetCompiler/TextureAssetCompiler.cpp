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
#include "RendererToolkit/AssetCompiler/TextureAssetCompiler.h"
#include "RendererToolkit/Helper/StringHelper.h"
#include "RendererToolkit/Helper/CacheManager.h"
#include "RendererToolkit/Helper/JsonHelper.h"
#include "RendererToolkit/PlatformTypes.h"

#include <RendererRuntime/Asset/AssetPackage.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4201)	// warning C4201: nonstandard extension used: nameless struct/union
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4324)	// warning C4324: '<x>': structure was padded due to alignment specifier
	#include <glm/glm.hpp>
	#include <glm/gtc/constants.hpp>
PRAGMA_WARNING_POP

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4005)						// warning C4005: '_HAS_EXCEPTIONS': macro redefinition
	PRAGMA_WARNING_DISABLE_CLANG("-Wunused-value")			// warning: expression result unused [-Wunused-value]
	PRAGMA_WARNING_DISABLE_CLANG("-Warray-bounds")			// warning: array index 1 is past the end of the array (which contains 1 element) [-Warray-bounds]
	PRAGMA_WARNING_DISABLE_GCC("-Wunused-value")			// warning: expression result unused [-Wunused-value]
	PRAGMA_WARNING_DISABLE_GCC("-Wunused-local-typedefs")	// warning: typedef ‘<x>’ locally defined but not used [-Wunused-value]
	#include <crunch/crnlib.h>
	#include <crunch/dds_defs.h>
	#include <crunch/crnlib/crn_texture_conversion.h>
	#include <crunch/crnlib/crn_command_line_params.h>
	#include <crunch/crnlib/crn_console.h>
PRAGMA_WARNING_POP

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#include <rapidjson/document.h>
PRAGMA_WARNING_POP

#include <fstream>
#include <array>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	// Create Toksvig specular anti-aliasing to reduce shimmering
	// -> Basing on "Specular Showdown in the Wild West" by Stephen Hill - http://blog.selfshadow.com/2011/07/22/specular-showdown/ - http://www.selfshadow.com/sandbox/toksvig.html
	namespace toksvig
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		// Fixed build in values by intent: Don't provide the artists with too many opportunities to introduce editing problems and break consistency
		static const float POWER = 100.0f;	///< Power {label:"Glossiness", default:100, min:0, max:256, step:1}
		static const float SIGMA = 0.5f;	///< Sigma {label:"Filter width", default:0.5, step:0.02}


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		float gaussianWeight(const glm::vec2& offset)
		{
			const float v = 2.0f * SIGMA * SIGMA;
			return exp(-glm::dot(offset, offset) / v) / (glm::pi<float>() * v);
		}

		glm::vec4 fetch(crnlib::image_u8& normalMapCrunchImage, const glm::vec2& position, const glm::vec2& offset)
		{
			const crnlib::color_quad_u8& crunchColor = normalMapCrunchImage.get_clamped(static_cast<int>(position.x + offset.x), static_cast<int>(position.y + offset.y));
			const glm::vec3 n((crunchColor.r / 255.0f) * 2.0f - 1.0f, (crunchColor.g / 255.0f) * 2.0f - 1.0f, (crunchColor.b / 255.0f) * 2.0f - 1.0f);
			return glm::vec4(glm::normalize(n), 1.0f) * gaussianWeight(offset);
		}

		float calculateToksvig(crnlib::image_u8& normalMapCrunchImage, const glm::vec2& position, float power)
		{
			glm::vec4 n;

			// 3x3 filter
			n  = fetch(normalMapCrunchImage, position, glm::vec2(-1.0f, -1.0f));
			n += fetch(normalMapCrunchImage, position, glm::vec2( 0.0f, -1.0f));
			n += fetch(normalMapCrunchImage, position, glm::vec2( 1.0f, -1.0f));

			n += fetch(normalMapCrunchImage, position, glm::vec2(-1.0f,  0.0f));
			n += fetch(normalMapCrunchImage, position, glm::vec2( 0.0f,  0.0f));
			n += fetch(normalMapCrunchImage, position, glm::vec2( 1.0f,  0.0f));

			n += fetch(normalMapCrunchImage, position, glm::vec2(-1.0f,  1.0f));
			n += fetch(normalMapCrunchImage, position, glm::vec2( 0.0f,  1.0f));
			n += fetch(normalMapCrunchImage, position, glm::vec2( 1.0f,  1.0f));

			// Divide by weight sum
			n.x /= n.w;
			n.y /= n.w;
			n.z /= n.w;

			// Toksvig factor
			const float length = glm::length(glm::vec3(n));
			return length / glm::mix(power, 1.0f, length);
		}

		void createToksvigRoughnessMap(const crnlib::mip_level& normalMapCrunchMipLevel, crnlib::mip_level& toksvigCrunchMipLevel)
		{
			const crnlib::uint width = normalMapCrunchMipLevel.get_width();
			const crnlib::uint height = normalMapCrunchMipLevel.get_height();
			crnlib::image_u8* normalMapCrunchImage = normalMapCrunchMipLevel.get_image();
			crnlib::image_u8* crunchImage = toksvigCrunchMipLevel.get_image();
			for (crnlib::uint y = 0; y < height; ++y)
			{
				for (crnlib::uint x = 0; x < width; ++x)
				{
					// Toksvig: Areas in the original normal map that were flat are white (glossy), whereas noisy, bumpy sections are darker
					const float toksvig = glm::clamp(calculateToksvig(*normalMapCrunchImage, glm::vec2(x, y), POWER), 0.0f, 1.0f);

					// Roughness = 1 - glossiness
					(*crunchImage)(x, y) = static_cast<crnlib::uint8>((1.0f - toksvig) * 255.0f);
				}
			}
		}

		void compositeToksvigRoughnessMap(const crnlib::mip_level& roughnessMapCrunchMipLevel, const crnlib::mip_level& normalMapCrunchMipLevel, crnlib::mip_level& crunchMipLevel)
		{
			const crnlib::uint width = normalMapCrunchMipLevel.get_width();
			const crnlib::uint height = normalMapCrunchMipLevel.get_height();
			crnlib::image_u8* roughnessMapCrunchImage = roughnessMapCrunchMipLevel.get_image();
			crnlib::image_u8* normalMapCrunchImage = normalMapCrunchMipLevel.get_image();
			crnlib::image_u8* crunchImage = crunchMipLevel.get_image();
			for (crnlib::uint y = 0; y < height; ++y)
			{
				for (crnlib::uint x = 0; x < width; ++x)
				{
					// Toksvig: Areas in the original normal map that were flat are white (glossy), whereas noisy, bumpy sections are darker
					const float toksvig = glm::clamp(calculateToksvig(*normalMapCrunchImage, glm::vec2(x, y), POWER), 0.0f, 1.0f);

					// Roughness = 1 - glossiness
					const float originalGlossiness = 1.0f - ((*roughnessMapCrunchImage)(x, y).r / 255.0f);
					(*crunchImage)(x, y).r = 255 - static_cast<crnlib::uint8>(originalGlossiness * toksvig * 255.0f);
				}
			}
		}


	} // toksvig

	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Texture asset compiler texture semantic; used to automatically set semantic appropriate texture processing settings like "cCRNCompFlagPerceptual"-flags
		*/
		enum class TextureSemantic
		{
			DIFFUSE_MAP,
			ALPHA_MAP,
			NORMAL_MAP,
			ROUGHNESS_MAP,
			METALLIC_MAP,
			EMISSIVE_MAP,
			HEIGHT_MAP,
			TINT_MAP,
			AMBIENT_OCCLUSION_MAP,
			REFLECTION_2D_MAP,
			REFLECTION_CUBE_MAP,
			COLOR_CORRECTION_LOOKUP_TABLE,
			PACKED_CHANNELS,
			UNKNOWN
		};

		static const uint16_t TEXTURE_FORMAT_VERSION = 0;


		//[-------------------------------------------------------]
		//[ Global variables                                      ]
		//[-------------------------------------------------------]
		static bool g_CrunchInitialized = false;


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		TextureSemantic getTextureSemanticByRapidJsonValue(const rapidjson::Value& rapidJsonValue)
		{
			const char* valueAsString = rapidJsonValue.GetString();
			const rapidjson::SizeType valueStringLength = rapidJsonValue.GetStringLength();

			// Define helper macros
			#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) return TextureSemantic::name;
			#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) return TextureSemantic::name;

			// Evaluate value
			IF_VALUE(DIFFUSE_MAP)
			ELSE_IF_VALUE(ALPHA_MAP)
			ELSE_IF_VALUE(NORMAL_MAP)
			ELSE_IF_VALUE(ROUGHNESS_MAP)
			ELSE_IF_VALUE(METALLIC_MAP)
			ELSE_IF_VALUE(EMISSIVE_MAP)
			ELSE_IF_VALUE(HEIGHT_MAP)
			ELSE_IF_VALUE(TINT_MAP)
			ELSE_IF_VALUE(AMBIENT_OCCLUSION_MAP)
			ELSE_IF_VALUE(REFLECTION_2D_MAP)
			ELSE_IF_VALUE(REFLECTION_CUBE_MAP)
			ELSE_IF_VALUE(COLOR_CORRECTION_LOOKUP_TABLE)
			ELSE_IF_VALUE(PACKED_CHANNELS)
			else
			{
				throw std::runtime_error(std::string("Unknown texture semantic \"") + valueAsString + '\"');
			}

			// Undefine helper macros
			#undef IF_VALUE
			#undef ELSE_IF_VALUE
		}

		void mandatoryTextureSemanticProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, TextureSemantic& value)
		{
			value = getTextureSemanticByRapidJsonValue(rapidJsonValue[propertyName]);
		}

		void load2DCrunchMipmappedTextureInternal(const char* sourceFilename, crnlib::mipmapped_texture& crunchMipmappedTexture)
		{
			crnlib::texture_file_types::format crunchSourceFileFormat = crnlib::texture_file_types::determine_file_format(sourceFilename);
			if (crunchSourceFileFormat == crnlib::texture_file_types::cFormatInvalid)
			{
				throw std::runtime_error("Unrecognized file type \"" + std::string(sourceFilename) + '\"');
			}
			if (!crunchMipmappedTexture.read_from_file(sourceFilename, crunchSourceFileFormat))
			{
				if (crunchMipmappedTexture.get_last_error().is_empty())
				{
					throw std::runtime_error("Failed reading source file \"" + std::string(sourceFilename) + '\"');
				}
				else
				{
					throw std::runtime_error(crunchMipmappedTexture.get_last_error().get_ptr());
				}
			}
		}

		void load2DCrunchMipmappedTexture(const char* sourceFilename, const char* sourceNormalMapFilename, crnlib::mipmapped_texture& crunchMipmappedTexture, crnlib::texture_conversion::convert_params& crunchConvertParams)
		{
			// Load, generate or compose mipmapped Crunch texture
			if (nullptr != sourceFilename && nullptr == sourceNormalMapFilename)
			{
				// Just load source texture
				load2DCrunchMipmappedTextureInternal(sourceFilename, crunchMipmappedTexture);

				// Use source texture mipmaps?
				if (crnlib::texture_file_types::supports_mipmaps(crnlib::texture_file_types::determine_file_format(sourceFilename)))
				{
					crunchConvertParams.m_mipmap_params.m_mode = cCRNMipModeUseSourceMips;
				}
			}
			else if (nullptr == sourceFilename && nullptr != sourceNormalMapFilename)
			{
				// Just generate a roughness map using a given normal map using Toksvig specular anti-aliasing to reduce shimmering

				// Load normal map texture
				crnlib::mipmapped_texture normalMapCrunchMipmappedTexture;
				load2DCrunchMipmappedTextureInternal(sourceNormalMapFilename, normalMapCrunchMipmappedTexture);

				// Create Toksvig specular anti-aliasing to reduce shimmering
				crunchMipmappedTexture.init(normalMapCrunchMipmappedTexture.get_width(), normalMapCrunchMipmappedTexture.get_height(), 1, 1, crnlib::PIXEL_FMT_L8, "Toksvig", crnlib::cDefaultOrientationFlags);
				::toksvig::createToksvigRoughnessMap(*normalMapCrunchMipmappedTexture.get_level(0, 0), *crunchMipmappedTexture.get_level(0, 0));
			}
			else
			{
				// Compose mipmapped Crunch texture

				// Load roughness map
				crnlib::mipmapped_texture roughnessMapCrunchMipmappedTexture;
				load2DCrunchMipmappedTextureInternal(sourceFilename, roughnessMapCrunchMipmappedTexture);

				// Load normal map
				crnlib::mipmapped_texture normalMapCrunchMipmappedTexture;
				load2DCrunchMipmappedTextureInternal(sourceNormalMapFilename, normalMapCrunchMipmappedTexture);

				// Sanity check
				if (roughnessMapCrunchMipmappedTexture.get_width() != normalMapCrunchMipmappedTexture.get_width() ||
					roughnessMapCrunchMipmappedTexture.get_height() != normalMapCrunchMipmappedTexture.get_height())
				{
					throw std::runtime_error("Roughness map and normal map must have the same dimension");
				}

				// Create Toksvig specular anti-aliasing to reduce shimmering
				crunchMipmappedTexture.init(normalMapCrunchMipmappedTexture.get_width(), normalMapCrunchMipmappedTexture.get_height(), 1, 1, crnlib::PIXEL_FMT_L8, "Toksvig", crnlib::cDefaultOrientationFlags);
				::toksvig::compositeToksvigRoughnessMap(*roughnessMapCrunchMipmappedTexture.get_level(0, 0), *normalMapCrunchMipmappedTexture.get_level(0, 0), *crunchMipmappedTexture.get_level(0, 0));
			}
		}


		//[-------------------------------------------------------]
		//[ Global classes                                        ]
		//[-------------------------------------------------------]
		class TextureChannelPacking
		{


		//[-------------------------------------------------------]
		//[ Public definitions                                    ]
		//[-------------------------------------------------------]
		public:
			struct Source
			{
				TextureSemantic			  textureSemantic  = TextureSemantic::UNKNOWN;
				uint8_t					  numberOfChannels = RendererRuntime::getUninitialized<uint8_t>();
				float					  defaultColor[4]  = { 0.0f, 0.0f, 0.0f, 0.0f };
				crnlib::mipmapped_texture crunchMipmappedTexture;
			};
			typedef std::vector<Source> Sources;

			struct Destination
			{
				uint8_t sourceIndex	  = RendererRuntime::getUninitialized<uint8_t>();
				uint8_t sourceChannel = RendererRuntime::getUninitialized<uint8_t>();
			};
			typedef std::vector<Destination> Destinations;


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			TextureChannelPacking(const RendererToolkit::IAssetCompiler::Configuration& configuration, const rapidjson::Value& rapidJsonValueTextureAssetCompiler, const char* basePath, const char* sourceNormalMapFilename, crnlib::texture_conversion::convert_params& crunchConvertParams)
			{
				loadLayout(configuration, rapidJsonValueTextureAssetCompiler, crunchConvertParams);
				loadSourceCrunchMipmappedTextures(rapidJsonValueTextureAssetCompiler, basePath, sourceNormalMapFilename);
			}

			crnlib::uint getDestinationWidth() const
			{
				for (const Source& source : mSources)
				{
					if (source.crunchMipmappedTexture.is_valid())
					{
						return source.crunchMipmappedTexture.get_width();
					}
				}
				throw std::runtime_error("Texture channel packing needs at least one source texture");
			}

			crnlib::uint getDestinationHeight() const
			{
				for (const Source& source : mSources)
				{
					if (source.crunchMipmappedTexture.is_valid())
					{
						return source.crunchMipmappedTexture.get_height();
					}
				}
				throw std::runtime_error("Texture channel packing needs at least one source texture");
			}

			crnlib::pixel_format getDestinationCrunchPixelFormat() const
			{
				switch (mDestinations.size())
				{
					case 1:
						return crnlib::PIXEL_FMT_L8;

					case 3:
						return crnlib::PIXEL_FMT_R8G8B8;

					case 4:
						return crnlib::PIXEL_FMT_A8R8G8B8;

					default:
						throw std::runtime_error("Invalid number of destination channels, must be 1, 3 or 4");
				}
			}

			inline const Sources& getSources() const
			{
				return mSources;
			}

			inline const Destinations& getDestinations() const
			{
				return mDestinations;
			}


		//[-------------------------------------------------------]
		//[ Private methods                                       ]
		//[-------------------------------------------------------]
		private:
			void loadLayout(const RendererToolkit::IAssetCompiler::Configuration& configuration, const rapidjson::Value& rapidJsonValueTextureAssetCompiler, crnlib::texture_conversion::convert_params& crunchConvertParams)
			{
				const std::string textureChannelPacking = rapidJsonValueTextureAssetCompiler["TextureChannelPacking"].GetString();
				const rapidjson::Value& rapidJsonValueTextureChannelPackings = configuration.rapidJsonValueTargets["TextureChannelPackings"];
				const rapidjson::Value& rapidJsonValueTextureChannelPacking = rapidJsonValueTextureChannelPackings[textureChannelPacking.c_str()];

				{ // Sources
					const rapidjson::Value& rapidJsonValueSources = rapidJsonValueTextureChannelPacking["Sources"];
					if (rapidJsonValueSources.Size() > 4)
					{
						throw std::runtime_error("Texture channel packing \"" + textureChannelPacking + "\" has more than four sources which is invalid");
					}
					mSources.resize(rapidJsonValueSources.Size());
					for (rapidjson::size_t i = 0; i < rapidJsonValueSources.Size(); ++i)
					{
						const rapidjson::Value& rapidJsonValueSource = rapidJsonValueSources[i];
						Source& source = mSources[i];
						mandatoryTextureSemanticProperty(rapidJsonValueSource, "TextureSemantic", source.textureSemantic);
						for (rapidjson::size_t k = 0; k < i; ++k)
						{
							if (mSources[k].textureSemantic == source.textureSemantic)
							{
								throw std::runtime_error("Texture channel packing \"" + textureChannelPacking + "\" source " + std::to_string(i) + ": The texture semantic \"" + rapidJsonValueSource["TextureSemantic"].GetString() + "\" is used multiple times which is invalid");
							}
						}
						source.numberOfChannels = rapidJsonValueSource["NumberOfChannels"].GetUint();
						if (source.numberOfChannels > 4)
						{
							throw std::runtime_error("Texture channel packing \"" + textureChannelPacking + "\" source " + std::to_string(i) + ": The number of texture channel packing source channels must not be greater as four");
						}
						RendererToolkit::JsonHelper::optionalFloatNProperty(rapidJsonValueSource, "DefaultColor", source.defaultColor, source.numberOfChannels);
					}
				}

				{ // Destinations
					const rapidjson::Value& rapidJsonValueDestinations = rapidJsonValueTextureChannelPacking["Destinations"];
					if (rapidJsonValueDestinations.Size() > 4)
					{
						throw std::runtime_error("Texture channel packing \"" + textureChannelPacking + "\" has more than four destinations which is invalid");
					}
					mDestinations.resize(rapidJsonValueDestinations.Size());
					for (rapidjson::size_t i = 0; i < rapidJsonValueDestinations.Size(); ++i)
					{
						const rapidjson::Value& rapidJsonValueDestination = rapidJsonValueDestinations[i];
						Destination& destination = mDestinations[i];
						{ // Get source index by texture semantic
							TextureSemantic textureSemantic = TextureSemantic::UNKNOWN;
							mandatoryTextureSemanticProperty(rapidJsonValueDestination, "TextureSemantic", textureSemantic);
							for (uint8_t sourceIndex = 0; sourceIndex < mSources.size(); ++sourceIndex)
							{
								if (mSources[sourceIndex].textureSemantic == textureSemantic)
								{
									destination.sourceIndex = sourceIndex;
									break;
								}
							}
							if (RendererRuntime::isUninitialized(destination.sourceIndex))
							{
								throw std::runtime_error("Texture channel packing \"" + textureChannelPacking + "\" destination " + std::to_string(i) + ": Found no texture channel packing source for the given texture semantic");
							}
						}
						destination.sourceChannel = rapidJsonValueDestination["SourceChannel"].GetUint();
						if (destination.sourceChannel > mSources[destination.sourceIndex].numberOfChannels)
						{
							throw std::runtime_error("Texture channel packing \"" + textureChannelPacking + "\" destination " + std::to_string(i) + " is referencing a source channel which doesn't exist");
						}
					}
				}

				{ // RGB hardware gamma correction used during runtime? (= sRGB)
				  // -> The "RgbHardwareGammaCorrection"-name was chosen to stay consistent to material blueprints (don't use too many different names for more or less the same topic)
					bool rgbHardwareGammaCorrection = false;
					RendererToolkit::JsonHelper::optionalBooleanProperty(rapidJsonValueTextureChannelPacking, "RgbHardwareGammaCorrection", rgbHardwareGammaCorrection);
					if (!rgbHardwareGammaCorrection)
					{
						crunchConvertParams.m_comp_params.set_flag(cCRNCompFlagPerceptual, false);
						crunchConvertParams.m_mipmap_params.m_gamma_filtering = false;
						crunchConvertParams.m_mipmap_params.m_gamma = 1.0f;	// Mipmap gamma correction value, default=2.2, use 1.0 for linear
					}
				}
			}

			std::string getSourceNormalMapFilename(const char* basePath, const char* sourceNormalMapFilename, const rapidjson::Value& rapidJsonValueInputFiles) const
			{
				if (nullptr != sourceNormalMapFilename)
				{
					// Use the normal map we received
					return sourceNormalMapFilename;
				}
				else
				{
					// Search for a normal map inside the texture channel packing layout
					for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorInputFile = rapidJsonValueInputFiles.MemberBegin(); rapidJsonMemberIteratorInputFile != rapidJsonValueInputFiles.MemberEnd(); ++rapidJsonMemberIteratorInputFile)
					{
						if (getTextureSemanticByRapidJsonValue(rapidJsonMemberIteratorInputFile->name) == TextureSemantic::NORMAL_MAP)
						{
							return std::string(basePath) + rapidJsonMemberIteratorInputFile->value.GetString();
						}
					}
				}

				// No normal map filename found
				return "";
			}

			void loadSourceCrunchMipmappedTextures(const rapidjson::Value& rapidJsonValueTextureAssetCompiler, const char* basePath, const char* sourceNormalMapFilename)
			{
				// Load provided source textures
				const rapidjson::Value& rapidJsonValueInputFiles = rapidJsonValueTextureAssetCompiler["InputFiles"];
				if (rapidJsonValueInputFiles.MemberCount() == 0)
				{
					throw std::runtime_error("No input files defined");
				}
				for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorInputFile = rapidJsonValueInputFiles.MemberBegin(); rapidJsonMemberIteratorInputFile != rapidJsonValueInputFiles.MemberEnd(); ++rapidJsonMemberIteratorInputFile)
				{
					const TextureSemantic textureSemantic = getTextureSemanticByRapidJsonValue(rapidJsonMemberIteratorInputFile->name);
					for (Source& source : mSources)
					{
						if (source.textureSemantic == textureSemantic)
						{
							// Support for Toksvig specular anti-aliasing to reduce shimmering
							std::string usedSourceNormalMapFilename;
							if (textureSemantic == TextureSemantic::ROUGHNESS_MAP)
							{
								// Search for normal map
								usedSourceNormalMapFilename = getSourceNormalMapFilename(basePath, sourceNormalMapFilename, rapidJsonValueInputFiles);
							}

							// Load Crunch mipmapped texture
							crnlib::texture_conversion::convert_params crunchConvertParams;
							load2DCrunchMipmappedTexture((std::string(basePath) + rapidJsonMemberIteratorInputFile->value.GetString()).c_str(), usedSourceNormalMapFilename.empty() ? nullptr : usedSourceNormalMapFilename.c_str(), source.crunchMipmappedTexture, crunchConvertParams);

							// Sanity check: Ensure the number of channels matches
							const crnlib::image_u8* crunchImage = source.crunchMipmappedTexture.get_level(0, 0)->get_image();
							for (uint8_t i = 0; i < source.numberOfChannels; ++i)
							{
								if (!crunchImage->is_component_valid(i))
								{
									throw std::runtime_error("Texture input file \"" + std::string(rapidJsonMemberIteratorInputFile->value.GetString()) + "\" has less channels then required by texture semantic \"" + std::string(rapidJsonMemberIteratorInputFile->name.GetString()) + '\"');
								}
							}
							break;
						}
					}
				}

				// Support for Toksvig specular anti-aliasing to reduce shimmering: Handle case if no roughness map to adjust was provided
				for (Source& source : mSources)
				{
					if (source.textureSemantic == TextureSemantic::ROUGHNESS_MAP)
					{
						if (!source.crunchMipmappedTexture.is_valid())
						{
							// Search for normal map
							const std::string usedSourceNormalMapFilename = getSourceNormalMapFilename(basePath, sourceNormalMapFilename, rapidJsonValueInputFiles);
							if (!usedSourceNormalMapFilename.empty())
							{
								// Load Crunch mipmapped texture
								crnlib::texture_conversion::convert_params crunchConvertParams;
								load2DCrunchMipmappedTexture(nullptr, usedSourceNormalMapFilename.c_str(), source.crunchMipmappedTexture, crunchConvertParams);
							}
						}
						break;
					}
				}

				// Sanity check: All source textures must have the same dimension
				for (uint8_t i = 0; i < mSources.size(); ++i)
				{
					const Source& source = mSources[i];
					if (source.crunchMipmappedTexture.is_valid())
					{
						for (uint8_t k = i + 1; k < mSources.size(); ++k)
						{
							const Source& otherSource = mSources[k];
							if (otherSource.crunchMipmappedTexture.is_valid() && (source.crunchMipmappedTexture.get_width() != otherSource.crunchMipmappedTexture.get_width() || source.crunchMipmappedTexture.get_height() != otherSource.crunchMipmappedTexture.get_height()))
							{
								throw std::runtime_error("All input textures must have the same dimension");
							}
						}
						break;
					}
				}
			}


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			Sources		 mSources;
			Destinations mDestinations;


		};


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		bool crunchConsoleOutput(crnlib::eConsoleMessageType type, const char* message, void*)
		{
			// Filter Crunch output to ignore messages like "Flipping texture on Y axis" or "Generated 11 mipmap levels in 3.261s"
			// -> Warnings like "Target bitrate/quality level is not supported for this output file format." don't cause harm either, but just show them so we're aware of possible issues
			// TODO(co) More context information like which asset is compiled right now might be useful. We need to keep in mind that there can be multiple texture compiler instances
			//          running at one and the same time. We could use the Crunch console output data to transport this information, on the other hand we need to ensure that we can
			//          unregister our function when we're done. "crnlib::console::remove_console_output_func() only checks the function pointer.
			if (crnlib::cMessageConsoleMessage == type || crnlib::cWarningConsoleMessage == type || crnlib::cErrorConsoleMessage == type)
			{
				RENDERERTOOLKIT_OUTPUT_DEBUG_STRING(message);
				RENDERERTOOLKIT_OUTPUT_DEBUG_STRING("\n");
			}

			// We handled the console output
			return true;
		}

		void initializeCrunch()
		{
			if (!g_CrunchInitialized)
			{
				// The Crunch console is using "printf()" by default if no console output function handles Crunch console output
				// -> Redirect the Crunch console output into our log so we have an uniform handling of such information
				crnlib::console::add_console_output_func(crunchConsoleOutput, nullptr);
				g_CrunchInitialized = true;
			}
		}

		std::string widthHeightToString(uint32_t width, uint32_t height)
		{
			return std::to_string(width) + 'x' + std::to_string(height);
		}

		void optionalTextureSemanticProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, TextureSemantic& value)
		{
			if (rapidJsonValue.HasMember(propertyName))
			{
				mandatoryTextureSemanticProperty(rapidJsonValue, propertyName, value);
			}
		}

		std::vector<std::string> getCubemapFilenames(const rapidjson::Value& rapidJsonValueTextureAssetCompiler, const std::string& basePath)
		{
			const rapidjson::Value& rapidJsonValueInputFiles = rapidJsonValueTextureAssetCompiler["InputFiles"];
			static const std::array<std::string, 6> FACE_NAMES = { "PositiveX", "NegativeX", "NegativeY", "PositiveY", "PositiveZ", "NegativeZ" };

			// The face order must be: +X, -X, -Y, +Y, +Z, -Z
			std::vector<std::string> filenames;
			filenames.reserve(6);
			for (size_t faceIndex = 0; faceIndex < FACE_NAMES.size(); ++faceIndex)
			{
				filenames.emplace_back(basePath + rapidJsonValueInputFiles[FACE_NAMES[faceIndex].c_str()].GetString());
			}
			return filenames;
		}

		bool checkIfChanged(const RendererToolkit::IAssetCompiler::Input& input, const RendererToolkit::IAssetCompiler::Configuration& configuration, const rapidjson::Value& rapidJsonValueTextureAssetCompiler, TextureSemantic textureSemantic, const std::string& inputAssetFilename, const std::string& outputAssetFilename, std::vector<RendererToolkit::CacheManager::CacheEntries>& cacheEntries)
		{
			if (TextureSemantic::REFLECTION_CUBE_MAP == textureSemantic)
			{
				// A cube map has six source files (for each face one source), so check if any of the six files has been changed
				// -> "inputAssetFilename" specifies the base directory of the faces source files
				const std::vector<std::string> faceFilenames = getCubemapFilenames(rapidJsonValueTextureAssetCompiler, inputAssetFilename);
				RendererToolkit::CacheManager::CacheEntries cacheEntriesCandidate;
				if (input.cacheManager.needsToBeCompiled(configuration.rendererTarget, input.assetFilename, faceFilenames, outputAssetFilename, TEXTURE_FORMAT_VERSION, cacheEntriesCandidate))
				{
					// Changed
					cacheEntries.push_back(cacheEntriesCandidate);
					return true;
				}

				// Not changed
				return false;
			}
			else if (TextureSemantic::ROUGHNESS_MAP == textureSemantic)
			{
				// A roughness map has two source files: First the roughness map itself and second a normal map
				// -> An asset can specify both files or only one of them
				// -> "inputAssetFilename" points to the roughness map
				// -> We need to fetch the name of the input normal map
				std::string normalMapAssetFilename;
				if (rapidJsonValueTextureAssetCompiler.HasMember("NormalMapInputFile"))
				{
					const std::string normalMapInputFile = rapidJsonValueTextureAssetCompiler["NormalMapInputFile"].GetString();
					if (!normalMapInputFile.empty())
					{
						normalMapAssetFilename = input.assetInputDirectory + normalMapInputFile;
					}
				}

				// Setup a list of source files
				std::vector<std::string> inputFilenames;
				if (!inputAssetFilename.empty())
				{
					inputFilenames.emplace_back(inputAssetFilename);
				}
				if (!normalMapAssetFilename.empty())
				{
					inputFilenames.emplace_back(normalMapAssetFilename);
				}

				RendererToolkit::CacheManager::CacheEntries cacheEntriesCandidate;
				if (input.cacheManager.needsToBeCompiled(configuration.rendererTarget, input.assetFilename, inputFilenames, outputAssetFilename, TEXTURE_FORMAT_VERSION, cacheEntriesCandidate))
				{
					// Changed
					cacheEntries.push_back(cacheEntriesCandidate);
					return true;
				}

				// Not changed
				return false;
			}
			else if (TextureSemantic::PACKED_CHANNELS == textureSemantic)
			{
				const rapidjson::Value& rapidJsonValueInputFiles = rapidJsonValueTextureAssetCompiler["InputFiles"];
				std::vector<std::string> filenames;
				filenames.reserve(rapidJsonValueInputFiles.MemberCount());
				for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorInputFile = rapidJsonValueInputFiles.MemberBegin(); rapidJsonMemberIteratorInputFile != rapidJsonValueInputFiles.MemberEnd(); ++rapidJsonMemberIteratorInputFile)
				{
					filenames.emplace_back(inputAssetFilename + rapidJsonMemberIteratorInputFile->value.GetString());
				}
				RendererToolkit::CacheManager::CacheEntries cacheEntriesCandidate;
				if (input.cacheManager.needsToBeCompiled(configuration.rendererTarget, input.assetFilename, filenames, outputAssetFilename, TEXTURE_FORMAT_VERSION, cacheEntriesCandidate))
				{
					// Changed
					cacheEntries.push_back(cacheEntriesCandidate);
					return true;
				}

				// Not changed
				return false;
			}
			else
			{
				// Asset has single source file
				RendererToolkit::CacheManager::CacheEntries cacheEntriesCandidate;
				if (input.cacheManager.needsToBeCompiled(configuration.rendererTarget, input.assetFilename, inputAssetFilename, outputAssetFilename, TEXTURE_FORMAT_VERSION, cacheEntriesCandidate))
				{
					// Changed
					cacheEntries.push_back(cacheEntriesCandidate);
					return true;
				}

				// Not changed
				return false;
			}
		}

		void loadCubeCrunchMipmappedTexture(const rapidjson::Value& rapidJsonValueTextureAssetCompiler, const char* basePath, crnlib::mipmapped_texture& crunchMipmappedTexture)
		{
			// The face order must be: +X, -X, -Y, +Y, +Z, -Z
			const std::vector<std::string> faceFilenames = getCubemapFilenames(rapidJsonValueTextureAssetCompiler, basePath);
			for (size_t faceIndex = 0; faceIndex < faceFilenames.size(); ++faceIndex)
			{
				// Load the 2D source image
				crnlib::image_u8* source2DImage = crnlib::crnlib_new<crnlib::image_u8>();
				const std::string& inputFile = faceFilenames[faceIndex];
				if (!crnlib::image_utils::read_from_file(*source2DImage, inputFile.c_str()))
				{
					throw std::runtime_error(std::string("Failed to load image \"") + inputFile + '\"');
				}

				// Sanity check
				const uint32_t width = source2DImage->get_width();
				if (width != source2DImage->get_height())
				{
					throw std::runtime_error("Cube map faces must have a width which is identical to the height");
				}

				// Process 2D source image
				const crnlib::pixel_format pixelFormat = source2DImage->has_alpha() ? crnlib::PIXEL_FMT_A8R8G8B8 : crnlib::PIXEL_FMT_R8G8B8;
				if (0 == faceIndex)
				{
					crunchMipmappedTexture.init(width, width, 1, 6, pixelFormat, "", crnlib::cDefaultOrientationFlags);
				}
				else if (crunchMipmappedTexture.get_format() != pixelFormat)
				{
					throw std::runtime_error("The pixel format of all cube map faces must be identical");
				}
				else if (crunchMipmappedTexture.get_width() != source2DImage->get_width())
				{
					throw std::runtime_error("The size of all cube map faces must be identical");
				}
				crunchMipmappedTexture.get_level(faceIndex, 0)->assign(source2DImage);
			}
		}

		void loadPackedChannelsCrunchMipmappedTexture(const RendererToolkit::IAssetCompiler::Configuration& configuration, const rapidjson::Value& rapidJsonValueTextureAssetCompiler, const char* basePath, const char* sourceNormalMapFilename, crnlib::mipmapped_texture& crunchMipmappedTexture, crnlib::texture_conversion::convert_params& crunchConvertParams)
		{
			// Load texture channel packing layout and source textures
			TextureChannelPacking textureChannelPacking(configuration, rapidJsonValueTextureAssetCompiler, basePath, sourceNormalMapFilename, crunchConvertParams);

			// Allocate the resulting Crunch mipmapped texture
			const crnlib::uint width = textureChannelPacking.getDestinationWidth();
			const crnlib::uint height = textureChannelPacking.getDestinationHeight();
			crunchMipmappedTexture.init(width, height, 1, 1, textureChannelPacking.getDestinationCrunchPixelFormat(), "Channel Packed Texture", crnlib::cDefaultOrientationFlags);

			// Fill the resulting Crunch mipmapped texture
			const TextureChannelPacking::Sources& sources = textureChannelPacking.getSources();
			const TextureChannelPacking::Destinations& destinations = textureChannelPacking.getDestinations();
			const crnlib::uint numberOfDestinationChannels = destinations.size();
			crnlib::image_u8* destinationCrunchImage = crunchMipmappedTexture.get_level(0, 0)->get_image();
			for (crnlib::uint destinationChannel = 0; destinationChannel < numberOfDestinationChannels; ++destinationChannel)
			{
				const TextureChannelPacking::Destination& destination = destinations[destinationChannel];
				const TextureChannelPacking::Source& source = sources[destination.sourceIndex];
				if (source.crunchMipmappedTexture.is_valid())
				{
					// Fill with source texture channel color
					const crnlib::image_u8* sourceCrunchImage = source.crunchMipmappedTexture.get_level(0, 0)->get_image();
					const uint8_t sourceChannel = destination.sourceChannel;
					for (crnlib::uint y = 0; y < height; ++y)
					{
						for (crnlib::uint x = 0; x < width; ++x)
						{
							(*destinationCrunchImage)(x, y).c[destinationChannel] = (*sourceCrunchImage)(x, y).c[sourceChannel];
						}
					}
				}
				else
				{
					// Fill with uniform default color
					const crnlib::uint8 value = static_cast<crnlib::uint8>(source.defaultColor[destination.sourceChannel] * 255.0f);
					for (crnlib::uint y = 0; y < height; ++y)
					{
						for (crnlib::uint x = 0; x < width; ++x)
						{
							(*destinationCrunchImage)(x, y).c[destinationChannel] = value;
						}
					}
				}
			}
		}

		void convertFile(const RendererToolkit::IAssetCompiler::Configuration& configuration, const rapidjson::Value& rapidJsonValueTextureAssetCompiler, const char* basePath, const char* sourceFilename, const char* destinationFilename, crnlib::texture_file_types::format outputCrunchTextureFileType, TextureSemantic textureSemantic, bool createMipmaps, float mipmapBlurriness, const char* sourceNormalMapFilename)
		{
			crnlib::texture_conversion::convert_params crunchConvertParams;

			// Load mipmapped Crunch texture
			crnlib::mipmapped_texture crunchMipmappedTexture;
			if (TextureSemantic::REFLECTION_CUBE_MAP == textureSemantic)
			{
				loadCubeCrunchMipmappedTexture(rapidJsonValueTextureAssetCompiler, basePath, crunchMipmappedTexture);
				crunchConvertParams.m_texture_type = crnlib::cTextureTypeCubemap;
			}
			else if (TextureSemantic::PACKED_CHANNELS == textureSemantic)
			{
				loadPackedChannelsCrunchMipmappedTexture(configuration, rapidJsonValueTextureAssetCompiler, basePath, sourceNormalMapFilename, crunchMipmappedTexture, crunchConvertParams);
			}
			else
			{
				load2DCrunchMipmappedTexture(sourceFilename, sourceNormalMapFilename, crunchMipmappedTexture, crunchConvertParams);
			}

			crunchConvertParams.m_texture_type = crunchMipmappedTexture.determine_texture_type();
			crunchConvertParams.m_pInput_texture = &crunchMipmappedTexture;
			crunchConvertParams.m_dst_filename = destinationFilename;
			crunchConvertParams.m_dst_file_type = outputCrunchTextureFileType;
			crunchConvertParams.m_y_flip = true;
			crunchConvertParams.m_no_stats = true;
			crunchConvertParams.m_dst_format = crnlib::PIXEL_FMT_INVALID;
			crunchConvertParams.m_comp_params.m_num_helper_threads = crnlib::g_number_of_processors - 1;

			// The 4x4 block size based DXT compression format has no support for 1D textures
			if (crunchMipmappedTexture.get_width() == 1 || crunchMipmappedTexture.get_height() == 1)
			{
				crunchConvertParams.m_dst_format = crnlib::PIXEL_FMT_A8R8G8B8;
			}

			// Evaluate texture semantic and figure out whether or not the destination format will be DXT compressed
			bool dxtCompressed = (crnlib::pixel_format::PIXEL_FMT_INVALID == crunchConvertParams.m_dst_format) ? true : crnlib::pixel_format_helpers::is_dxt(crunchConvertParams.m_dst_format);
			switch (textureSemantic)
			{
				case TextureSemantic::DIFFUSE_MAP:
				case TextureSemantic::REFLECTION_2D_MAP:
					// Nothing here, just a regular texture
					break;

				case TextureSemantic::ALPHA_MAP:
					crunchConvertParams.m_comp_params.set_flag(cCRNCompFlagPerceptual, false);
					crunchConvertParams.m_mipmap_params.m_gamma_filtering = false;
					crunchConvertParams.m_mipmap_params.m_gamma = 1.0f;	// Mipmap gamma correction value, default=2.2, use 1.0 for linear
					break;

				case TextureSemantic::NORMAL_MAP:
					crunchConvertParams.m_texture_type = crnlib::cTextureTypeNormalMap;
					crunchConvertParams.m_comp_params.set_flag(cCRNCompFlagPerceptual, false);
					crunchConvertParams.m_mipmap_params.m_renormalize = true;
					crunchConvertParams.m_mipmap_params.m_gamma_filtering = false;
					crunchConvertParams.m_mipmap_params.m_gamma = 1.0f;	// Mipmap gamma correction value, default=2.2, use 1.0 for linear

					// Do never ever store normal maps standard DXT1 compressed to not get horrible artefact's due to compressing vector data using algorithms design for color data
					// -> See "Real-Time Normal Map DXT Compression" -> "3.3 Tangent-Space 3Dc" - http://www.nvidia.com/object/real-time-normal-map-dxt-compression.html
					if (crnlib::pixel_format::PIXEL_FMT_INVALID == crunchConvertParams.m_dst_format || crnlib::pixel_format::PIXEL_FMT_DXT1 == crunchConvertParams.m_dst_format)
					{
						crunchConvertParams.m_dst_format = crnlib::PIXEL_FMT_3DC;
						dxtCompressed = true;
					}
					break;

				case TextureSemantic::ROUGHNESS_MAP:
				case TextureSemantic::METALLIC_MAP:
				case TextureSemantic::HEIGHT_MAP:
				case TextureSemantic::TINT_MAP:
				case TextureSemantic::AMBIENT_OCCLUSION_MAP:
					crunchConvertParams.m_comp_params.set_flag(cCRNCompFlagPerceptual, false);
					crunchConvertParams.m_mipmap_params.m_gamma_filtering = false;
					crunchConvertParams.m_mipmap_params.m_gamma = 1.0f;	// Mipmap gamma correction value, default=2.2, use 1.0 for linear
					break;

				case TextureSemantic::EMISSIVE_MAP:
					// Nothing here, just a regular texture
					break;

				case TextureSemantic::REFLECTION_CUBE_MAP:
				case TextureSemantic::COLOR_CORRECTION_LOOKUP_TABLE:
				case TextureSemantic::PACKED_CHANNELS:
					// Nothing here, handled elsewhere
					break;

				case TextureSemantic::UNKNOWN:
					// Nothing here, just a regular texture
					break;
			}

			// 4x4 block size based DXT compression means the texture dimension must be a multiple of four, for all mipmaps if mipmaps are used
			if (dxtCompressed)
			{
				// Check base mipmap
				uint32_t width = crunchMipmappedTexture.get_width();
				uint32_t height = crunchMipmappedTexture.get_height();
				if (0 != (width % 4) || 0 != (height % 4))
				{
					throw std::runtime_error("4x4 block size based DXT compression used, but the texture dimension " + widthHeightToString(width, height) + " is no multiple of four");
				}
				else if (createMipmaps)
				{
					// Check mipmaps and at least inform in case dynamic texture resolution scale will be limited
					uint32_t mipmap = 0;
					while (width > 4 && height > 4)
					{
						// Check mipmap
						if (0 != (width % 4) || 0 != (height % 4))
						{
							const std::string warning = "4x4 block size based DXT compression used, but the texture dimension " + widthHeightToString(width, height) +
														" at mipmap level " + std::to_string(mipmap) + " is no multiple of four. Texture dimension is " +
														widthHeightToString(crunchMipmappedTexture.get_width(), crunchMipmappedTexture.get_height()) + ". Dynamic texture resolution scale will be limited to mipmap level " + std::to_string(mipmap - 1) + '.';
							RENDERERTOOLKIT_OUTPUT_DEBUG_STRING(warning.c_str());
							break;
						}

						// Next mipmap
						++mipmap;
						width = std::max(width >> 1, 1u);	// /= 2
						height = std::max(height >> 1, 1u);	// /= 2
					}
				}
			}

			// Create mipmaps?
			crunchConvertParams.m_mipmap_params.m_mode = createMipmaps ? cCRNMipModeGenerateMips : cCRNMipModeNoMips;
			crunchConvertParams.m_mipmap_params.m_blurriness = mipmapBlurriness;

			// Evaluate the quality strategy
			switch (configuration.qualityStrategy)
			{
				case RendererToolkit::QualityStrategy::DEBUG:
					// Most aggressive option: Reduce texture size
					crunchConvertParams.m_mipmap_params.m_scale_mode = cCRNSMRelative;
					crunchConvertParams.m_mipmap_params.m_scale_x = 0.5f;
					crunchConvertParams.m_mipmap_params.m_scale_y = 0.5f;

					// Disable several output file optimizations
					crunchConvertParams.m_comp_params.set_flag(cCRNCompFlagQuick, true);

					// Set endpoint optimizer's maximum iteration depth
					crunchConvertParams.m_comp_params.m_dxt_quality = cCRNDXTQualitySuperFast;

					// Set clustered DDS/CRN quality factor [0-255] 255=best
					crunchConvertParams.m_comp_params.m_quality_level = cCRNMinQualityLevel;
					break;

				case RendererToolkit::QualityStrategy::PRODUCTION:
					// Set endpoint optimizer's maximum iteration depth
					crunchConvertParams.m_comp_params.m_dxt_quality = cCRNDXTQualityNormal;

					// Set clustered DDS/CRN quality factor [0-255] 255=best
					crunchConvertParams.m_comp_params.m_quality_level = (cCRNMaxQualityLevel - cCRNMinQualityLevel) / 2;
					break;

				case RendererToolkit::QualityStrategy::SHIPPING:
					// Set endpoint optimizer's maximum iteration depth
					crunchConvertParams.m_comp_params.m_dxt_quality = cCRNDXTQualityUber;

					// Set clustered DDS/CRN quality factor [0-255] 255=best
					crunchConvertParams.m_comp_params.m_quality_level = cCRNMaxQualityLevel;
					break;
			}

			// Silence "Target bitrate/quality level is not supported for this output file format." warnings
			if (crnlib::texture_file_types::format::cFormatKTX == outputCrunchTextureFileType)
			{
				crunchConvertParams.m_comp_params.m_quality_level = cCRNMaxQualityLevel;
			}

			// Compress now
			crnlib::texture_conversion::convert_stats stats;
			if (!crnlib::texture_conversion::process(crunchConvertParams, stats))
			{
				if (crunchConvertParams.m_error_message.is_empty())
				{
					throw std::runtime_error("Failed writing output file \"" + std::string(destinationFilename) + '\"');
				}
				else
				{
					throw std::runtime_error(crunchConvertParams.m_error_message.get_ptr());
				}
			}
		}

		void convertColorCorrectionLookupTable(const char* sourceFilename, const char* destinationFilename)
		{
			// Load the 2D source image
			crnlib::image_u8 sourceImage;
			crnlib::image_utils::read_from_file(sourceImage, sourceFilename);

			// Sanity checks
			if (sourceImage.get_width() < sourceImage.get_height())
			{
				throw std::runtime_error("Color correction lookup table width must be equal or greater as the height");
			}
			if (!sourceImage.has_rgb() || sourceImage.has_alpha())
			{
				throw std::runtime_error("Color correction lookup table must be RGB");
			}

			// Create the 3D texture destination data which always has four components per texel
			const uint32_t width = sourceImage.get_height();	// Each 3D texture layer is a square
			const uint32_t height = sourceImage.get_height();
			const uint32_t numberOfTexelsPerLayer = width * height;
			const uint32_t depth = sourceImage.get_width() / height;
			crnlib::color_quad_u8* destinationData = new crnlib::color_quad_u8[numberOfTexelsPerLayer * depth];
			{
				crnlib::color_quad_u8* currentDestinationData = destinationData;
				uint32_t sourceX = 0;
				for (uint32_t z = 0; z < depth; ++z, sourceX += width, currentDestinationData += numberOfTexelsPerLayer)
				{
					if (!sourceImage.extract_block(currentDestinationData, sourceX, 0, width, height))
					{
						throw std::runtime_error("Color correction lookup table failed to extract block");
					}
				}
			}

			// Fill dds header ("PIXEL_FMT_A8R8G8B8" pixel format)
			crnlib::DDSURFACEDESC2 ddsSurfaceDesc2 = {};
			ddsSurfaceDesc2.dwSize								= sizeof(crnlib::DDSURFACEDESC2);
			ddsSurfaceDesc2.dwFlags								= crnlib::DDSD_WIDTH | crnlib::DDSD_HEIGHT | crnlib::DDSD_DEPTH | crnlib::DDSD_PIXELFORMAT | crnlib::DDSD_CAPS | crnlib::DDSD_LINEARSIZE;
			ddsSurfaceDesc2.dwHeight							= height;
			ddsSurfaceDesc2.dwWidth								= width;
			ddsSurfaceDesc2.dwBackBufferCount					= depth;
			ddsSurfaceDesc2.ddsCaps.dwCaps						= crnlib::DDSCAPS_TEXTURE | crnlib::DDSCAPS_COMPLEX;
			ddsSurfaceDesc2.ddsCaps.dwCaps2						= crnlib::DDSCAPS2_VOLUME;
			ddsSurfaceDesc2.ddpfPixelFormat.dwSize				= sizeof(crnlib::DDPIXELFORMAT);
			ddsSurfaceDesc2.ddpfPixelFormat.dwFlags			   |= (crnlib::DDPF_RGB | crnlib::DDPF_ALPHAPIXELS);
			ddsSurfaceDesc2.ddpfPixelFormat.dwRGBBitCount		= 32;
			ddsSurfaceDesc2.ddpfPixelFormat.dwRBitMask			= 0xFF0000;
			ddsSurfaceDesc2.ddpfPixelFormat.dwGBitMask			= 0x00FF00;
			ddsSurfaceDesc2.ddpfPixelFormat.dwBBitMask			= 0x0000FF;
			ddsSurfaceDesc2.ddpfPixelFormat.dwRGBAlphaBitMask	= 0xFF000000;
			ddsSurfaceDesc2.lPitch								= (ddsSurfaceDesc2.dwWidth * ddsSurfaceDesc2.ddpfPixelFormat.dwRGBBitCount) >> 3;

			// Write down the 3D destination texture
			std::ofstream outputFileStream(destinationFilename, std::ios::binary);
			outputFileStream.write("DDS ", sizeof(uint32_t));
			outputFileStream.write(reinterpret_cast<const char*>(&ddsSurfaceDesc2), sizeof(crnlib::DDSURFACEDESC2));
			outputFileStream.write(reinterpret_cast<const char*>(destinationData), sizeof(crnlib::color_quad_u8) * numberOfTexelsPerLayer * depth);

			// Done
			delete [] destinationData;
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const AssetCompilerTypeId TextureAssetCompiler::TYPE_ID("Texture");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureAssetCompiler::TextureAssetCompiler()
	{
		::detail::initializeCrunch();
	}

	TextureAssetCompiler::~TextureAssetCompiler()
	{
		// Nothing here, a Crunch de-initialization call wouldn't add any benefit in here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId TextureAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	void TextureAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Input, configuration and output
		const std::string&			   assetInputDirectory	= input.assetInputDirectory;
		const std::string&			   assetOutputDirectory	= input.assetOutputDirectory;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		const rapidjson::Value& rapidJsonValueAsset = configuration.rapidJsonDocumentAsset["Asset"];

		// Read texture asset compiler configuration
		std::string inputFile;
		std::string assetFileFormat;
		::detail::TextureSemantic textureSemantic = ::detail::TextureSemantic::UNKNOWN;
		bool createMipmaps = true;
		float mipmapBlurriness = 0.9f;	// Scale filter kernel, >1=blur, <1=sharpen, .01-8, default=.9, Crunch default "blurriness" factor of 0.9 actually sharpens the output a little
		std::string normalMapInputFile;
		const rapidjson::Value& rapidJsonValueTextureAssetCompiler = rapidJsonValueAsset["TextureAssetCompiler"];
		{
			::detail::optionalTextureSemanticProperty(rapidJsonValueTextureAssetCompiler, "TextureSemantic", textureSemantic);
			if (rapidJsonValueTextureAssetCompiler.HasMember("InputFile"))
			{
				inputFile = rapidJsonValueTextureAssetCompiler["InputFile"].GetString();
			}
			if (rapidJsonValueTextureAssetCompiler.HasMember("FileFormat"))
			{
				assetFileFormat = rapidJsonValueTextureAssetCompiler["FileFormat"].GetString();
			}
			JsonHelper::optionalBooleanProperty(rapidJsonValueTextureAssetCompiler, "CreateMipmaps", createMipmaps);
			JsonHelper::optionalFloatProperty(rapidJsonValueTextureAssetCompiler, "MipmapBlurriness", mipmapBlurriness);
			if (rapidJsonValueTextureAssetCompiler.HasMember("NormalMapInputFile"))
			{
				normalMapInputFile = rapidJsonValueTextureAssetCompiler["NormalMapInputFile"].GetString();
			}

			// Texture semantic overrules manual settings
			if (::detail::TextureSemantic::COLOR_CORRECTION_LOOKUP_TABLE == textureSemantic)
			{
				assetFileFormat = "dds";
				createMipmaps = false;
			}
		}
		const std::string inputAssetFilename = assetInputDirectory + inputFile;
		const std::string normalMapAssetFilename = assetInputDirectory + normalMapInputFile;
		const std::string assetName = rapidJsonValueAsset["AssetMetadata"]["AssetName"].GetString();

		// Sanity checks
		if (inputFile.empty())
		{
			bool throwException = true;
			if (::detail::TextureSemantic::REFLECTION_CUBE_MAP == textureSemantic || ::detail::TextureSemantic::PACKED_CHANNELS == textureSemantic)
			{
				// Reflection cube maps or packed channels don't have a single input file, they're composed of multiple input files
				throwException = false;
			}
			else if (::detail::TextureSemantic::ROUGHNESS_MAP == textureSemantic)
			{
				// If a normal map input file is provided roughness maps can be calculated automatically using Toksvig specular anti-aliasing to reduce shimmering, in this case a input file is optional
				throwException = normalMapInputFile.empty();
			}
			if (throwException)
			{
				throw std::runtime_error("Input file must be defined");
			}
		}
		if (::detail::TextureSemantic::ROUGHNESS_MAP != textureSemantic && ::detail::TextureSemantic::PACKED_CHANNELS != textureSemantic && !normalMapInputFile.empty())
		{
			throw std::runtime_error("Providing a normal map is only valid for roughness maps or packed channels");
		}
		// TODO(co) Need a log for this: Quality warning in case it's a roughness map but no normal map is provided. No error, but chances are high that there will be nasty visible specular aliasing issues.

		// Get output related settings
		std::string outputAssetFilename;
		crnlib::texture_file_types::format crunchOutputTextureFileType = crnlib::texture_file_types::cFormatCRN;
		{
			const rapidjson::Value& rapidJsonValueTargets = configuration.rapidJsonValueTargets;

			// Get the JSON targets object
			std::string textureTargetName;
			{
				const rapidjson::Value& rapidJsonValueRendererTargets = rapidJsonValueTargets["RendererTargets"];
				const rapidjson::Value& rapidJsonValueRendererTarget = rapidJsonValueRendererTargets[configuration.rendererTarget.c_str()];
				textureTargetName = rapidJsonValueRendererTarget["TextureTarget"].GetString();
			}
			{
				std::string fileFormat = assetFileFormat;
				if (fileFormat.empty())
				{
					const rapidjson::Value& rapidJsonValueTextureTargets = rapidJsonValueTargets["TextureTargets"];
					const rapidjson::Value& rapidJsonValueTextureTarget = rapidJsonValueTextureTargets[textureTargetName.c_str()];
					fileFormat = rapidJsonValueTextureTarget["FileFormat"].GetString();
				}
				outputAssetFilename = assetOutputDirectory + assetName + '.' + fileFormat;
				if (fileFormat == "crn")
				{
					crunchOutputTextureFileType = crnlib::texture_file_types::cFormatCRN;
				}
				else if (fileFormat == "dds")
				{
					crunchOutputTextureFileType = crnlib::texture_file_types::cFormatDDS;
				}
				else if (fileFormat == "ktx")
				{
					crunchOutputTextureFileType = crnlib::texture_file_types::cFormatKTX;
				}
			}
		}

		// Ask the cache manager whether or not we need to compile the source file (e.g. source changed or target not there)
		std::vector<CacheManager::CacheEntries> cacheEntries;
		if (::detail::checkIfChanged(input, configuration, rapidJsonValueTextureAssetCompiler, textureSemantic, inputAssetFilename, outputAssetFilename, cacheEntries))
		{
			if (::detail::TextureSemantic::COLOR_CORRECTION_LOOKUP_TABLE == textureSemantic)
			{
				detail::convertColorCorrectionLookupTable(inputAssetFilename.c_str(), outputAssetFilename.c_str());
			}
			else
			{
				detail::convertFile(configuration, rapidJsonValueTextureAssetCompiler, inputAssetFilename.c_str(), inputFile.empty() ? nullptr : inputAssetFilename.c_str(), outputAssetFilename.c_str(), crunchOutputTextureFileType, textureSemantic, createMipmaps, mipmapBlurriness, normalMapInputFile.empty() ? nullptr : normalMapAssetFilename.c_str());
			}

			// Store new cache entries or update existing ones
			for (const CacheManager::CacheEntries& currentCacheEntries : cacheEntries)
			{
				input.cacheManager.storeOrUpdateCacheEntriesInDatabase(currentCacheEntries);
			}
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/Texture/" + assetCategory + '/' + assetName;
			outputAsset(assetIdAsString, outputAssetFilename, outputAssetPackage);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
