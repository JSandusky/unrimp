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
#include "RendererToolkit/PlatformTypes.h"
#include "RendererToolkit/Helper/JsonHelper.h"

#include <RendererRuntime/Asset/AssetPackage.h>

#include <glm/detail/setup.hpp>	// For "glm::countof()"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4005)	// warning C4005: '_HAS_EXCEPTIONS': macro redefinition
	#include <crunch/crnlib.h>
	#include <crunch/dds_defs.h>
	#include <crunch/crnlib/crn_texture_conversion.h>
	#include <crunch/crnlib/crn_command_line_params.h>
PRAGMA_WARNING_POP

#include <libtiff/tiffio.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#include <rapidjson/document.h>
PRAGMA_WARNING_POP

#include <fstream>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
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
			REFLECTION_CUBE_MAP,
			COLOR_CORRECTION_LOOKUP_TABLE,
			UNKNOWN
		};


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		std::string widthHeightToString(uint32 width, uint32 height)
		{
			return std::to_string(width) + 'x' + std::to_string(height);
		}

		void optionalTextureSemanticProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, TextureSemantic& value)
		{
			if (rapidJsonValue.HasMember(propertyName))
			{
				const rapidjson::Value& rapidJsonValueValue = rapidJsonValue[propertyName];
				const char* valueAsString = rapidJsonValueValue.GetString();
				const rapidjson::SizeType valueStringLength = rapidJsonValueValue.GetStringLength();

				// Define helper macros
				#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = TextureSemantic::name;
				#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = TextureSemantic::name;

				// Evaluate value
				IF_VALUE(DIFFUSE_MAP)
				ELSE_IF_VALUE(ALPHA_MAP)
				ELSE_IF_VALUE(NORMAL_MAP)
				ELSE_IF_VALUE(ROUGHNESS_MAP)
				ELSE_IF_VALUE(METALLIC_MAP)
				ELSE_IF_VALUE(EMISSIVE_MAP)
				ELSE_IF_VALUE(REFLECTION_CUBE_MAP)
				ELSE_IF_VALUE(COLOR_CORRECTION_LOOKUP_TABLE)
				else
				{
					throw std::runtime_error(std::string("Unknown texture semantic \"") + valueAsString + '\"');
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
			}
		}

		void convertFile(const RendererToolkit::IAssetCompiler::Configuration& configuration, const rapidjson::Value& rapidJsonValueTextureAssetCompiler, const char* pSrc_filename, const char* pDst_filename, crnlib::texture_file_types::format out_file_type, TextureSemantic textureSemantic, bool createMipmaps, float mipmapBlurriness)
		{
			crnlib::texture_conversion::convert_params crunchConvertParams;

			crnlib::mipmapped_texture crunchMipmappedTexture;
			if (TextureSemantic::REFLECTION_CUBE_MAP == textureSemantic)
			{
				// The face order must be: +X, -X, -Y, +Y, +Z, -Z
				static const std::string FACE_NAMES[6] = { "PositiveXInputFile", "NegativeXInputFile", "NegativeYInputFile", "PositiveYInputFile", "PositiveZInputFile", "NegativeZInputFile" };
				for (int faceIndex = 0; faceIndex < 6; ++faceIndex)
				{
					// Load the 2D source image
					crnlib::image_u8* source2DImage = crnlib::crnlib_new<crnlib::image_u8>();
					const std::string inputFile = std::string(pSrc_filename) + rapidJsonValueTextureAssetCompiler[FACE_NAMES[faceIndex].c_str()].GetString();
					if (!crnlib::image_utils::read_from_file(*source2DImage, inputFile.c_str()))
					{
						throw std::runtime_error(std::string("Failed to load image \"") + inputFile + '\"');
					}

					// Sanity check
					const uint32 width = source2DImage->get_width();
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
			else
			{
				crnlib::texture_file_types::format crunchSourceFileFormat = crnlib::texture_file_types::determine_file_format(pSrc_filename);
				if (crunchSourceFileFormat == crnlib::texture_file_types::cFormatInvalid)
				{
					throw std::runtime_error("Unrecognized file type \"" + std::string(pSrc_filename) + '\"');
				}

				if (!crunchMipmappedTexture.read_from_file(pSrc_filename, crunchSourceFileFormat))
				{
					if (crunchMipmappedTexture.get_last_error().is_empty())
					{
						throw std::runtime_error("Failed reading source file \"" + std::string(pSrc_filename) + '\"');
					}
					else
					{
						throw std::runtime_error(crunchMipmappedTexture.get_last_error().get_ptr());
					}
				}

				if (crnlib::texture_file_types::supports_mipmaps(crunchSourceFileFormat))
				{
					crunchConvertParams.m_mipmap_params.m_mode = cCRNMipModeUseSourceMips;
				}
			}

			crunchConvertParams.m_texture_type = crunchMipmappedTexture.determine_texture_type();
			crunchConvertParams.m_pInput_texture = &crunchMipmappedTexture;
			crunchConvertParams.m_dst_filename = pDst_filename;
			crunchConvertParams.m_dst_file_type = out_file_type;
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
					crunchConvertParams.m_comp_params.set_flag(cCRNCompFlagPerceptual, false);
					crunchConvertParams.m_mipmap_params.m_gamma_filtering = false;
					crunchConvertParams.m_mipmap_params.m_gamma = 1.0f;	// Mipmap gamma correction value, default=2.2, use 1.0 for linear
					break;

				case TextureSemantic::EMISSIVE_MAP:
					// Nothing here, just a regular texture
					break;

				case TextureSemantic::REFLECTION_CUBE_MAP:
					crunchConvertParams.m_texture_type = crnlib::cTextureTypeCubemap;
					break;

				case TextureSemantic::COLOR_CORRECTION_LOOKUP_TABLE:
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
				uint32 width = crunchMipmappedTexture.get_width();
				uint32 height = crunchMipmappedTexture.get_height();
				if (0 != (width % 4) || 0 != (height % 4))
				{
					throw std::runtime_error("4x4 block size based DXT compression used, but the texture dimension " + widthHeightToString(width, height) + " is no multiple of four");
				}
				else if (createMipmaps)
				{
					// Check mipmaps and at least inform in case dynamic texture resolution scale will be limited
					uint32 mipmap = 0;
					while (width > 4 && height > 4)
					{
						// Check mipmap
						if (0 != (width % 4) || 0 != (height % 4))
						{
							// TODO(co) Performance warning via log
							std::string warning = "4x4 block size based DXT compression used, but the texture dimension " + widthHeightToString(width, height) +
												  " at mipmap level " + std::to_string(mipmap) + " is no multiple of four. Texture dimension is " +
												  widthHeightToString(crunchMipmappedTexture.get_width(), crunchMipmappedTexture.get_height()) + ". Dynamic texture resolution scale will be limited to mipmap level " + std::to_string(mipmap - 1) + '.';
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

			// Compress now
			crnlib::texture_conversion::convert_stats stats;
			if (!crnlib::texture_conversion::process(crunchConvertParams, stats))
			{
				if (crunchConvertParams.m_error_message.is_empty())
				{
					throw std::runtime_error("Failed writing output file \"" + std::string(pDst_filename) + '\"');
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
			outputFileStream.write("DDS ", sizeof(uint32));
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
		// Nothing here
	}

	TextureAssetCompiler::~TextureAssetCompiler()
	{
		// Nothing here
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

			// Texture semantic overrules manual settings
			if (::detail::TextureSemantic::COLOR_CORRECTION_LOOKUP_TABLE == textureSemantic)
			{
				assetFileFormat = "dds";
				createMipmaps = false;
			}
		}

		// Sanity checks
		if (::detail::TextureSemantic::REFLECTION_CUBE_MAP != textureSemantic && inputFile.empty())
		{
			throw std::runtime_error("Input file must be defined");
		}

		const std::string inputAssetFilename = assetInputDirectory + inputFile;
		const std::string assetName = rapidJsonValueAsset["AssetMetadata"]["AssetName"].GetString();

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

		// Ask cache manager if we need to compile the source file (e.g. source changed or target not there)
		if (CacheManager::needsToBeCompiled(configuration.rendererTarget, input.assetFilename, inputAssetFilename, outputAssetFilename))
		{
			if (::detail::TextureSemantic::COLOR_CORRECTION_LOOKUP_TABLE == textureSemantic)
			{
				detail::convertColorCorrectionLookupTable(inputAssetFilename.c_str(), outputAssetFilename.c_str());
			}
			else
			{
				detail::convertFile(configuration, rapidJsonValueTextureAssetCompiler, inputAssetFilename.c_str(), outputAssetFilename.c_str(), crunchOutputTextureFileType, textureSemantic, createMipmaps, mipmapBlurriness);
			}
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/Texture/" + assetCategory + '/' + assetName;

			// Output asset
			RendererRuntime::Asset outputAsset;
			outputAsset.assetId = StringHelper::getAssetIdByString(assetIdAsString.c_str());
			strcpy(outputAsset.assetFilename, outputAssetFilename.c_str());	// TODO(co) Buffer overflow test
			outputAssetPackage.getWritableSortedAssetVector().push_back(outputAsset);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
