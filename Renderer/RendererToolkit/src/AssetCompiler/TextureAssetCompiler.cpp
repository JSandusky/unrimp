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


// TODO(co) Cleanup, currently this is the crunch application in order to prototype the integration into the asset toolkit.


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

// General STB image definitions
#define STBI_NO_STDIO
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STB_IMAGE_IMPLEMENTATION
// STB image image format support definitions
#define STBI_ONLY_PSD
#define STBI_ONLY_TGA
/*
#define STBI_NO_JPEG
#define STBI_NO_PNG
#define STBI_NO_BMP
// #define STBI_NO_PSD
// #define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
*/
// STB image
// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4312)	// warning C4312: 'type cast': conversion from 'int' to 'unsigned char *' of greater size
	#include <crunch/stb_image.h>	// TODO(co) This is another library
PRAGMA_WARNING_POP

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
			SPECULAR_MAP,
			EMISSIVE_MAP,
			REFLECTION_CUBE_MAP,
			COLOR_CORRECTION_LOOKUP_TABLE,
			UNKNOWN
		};

		const int cDefaultCRNQualityLevel = 128;
		crnlib::command_line_params m_params;
		enum convert_status
		{
			cCSFailed,
			cCSSucceeded,
			cCSSkipped,
			cCSBadParam,
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
				ELSE_IF_VALUE(SPECULAR_MAP)
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

		static crn_bool progress_callback_func(crn_uint32 phase_index, crn_uint32 total_phases, crn_uint32 subphase_index, crn_uint32 total_subphases, void* pUser_data_ptr)
		{
			int percentage_complete = (int)(.5f + (phase_index + float(subphase_index) / total_subphases) * 100.0f) / total_phases;
			//printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bProcessing: %u%%", std::min(100, std::max(0, percentage_complete)));
			return true;
		}

		bool parse_mipmap_params(crn_mipmap_params& mip_params)
		{
		  crnlib::dynamic_string val;

		  if (m_params.get_value_as_string("mipMode", 0, val))
		  {
			 uint32 i;
			 for (i = 0; i < cCRNMipModeTotal; i++)
			 {
				if (val == crn_get_mip_mode_name( static_cast<crn_mip_mode>(i) ))
				{
				   mip_params.m_mode = static_cast<crn_mip_mode>(i);
				   break;
				}
			 }
			 if (i == cCRNMipModeTotal)
			 {
			//	console::error("Invalid MipMode: \"%s\"", val.get_ptr());
				return false;
			 }
		  }

		  if (m_params.get_value_as_string("mipFilter", 0, val))
		  {
			 uint32 i;
			 for (i = 0; i < cCRNMipFilterTotal; i++)
			 {
				if (val == crnlib::dynamic_string(crn_get_mip_filter_name( static_cast<crn_mip_filter>(i) )) )
				{
				   mip_params.m_filter = static_cast<crn_mip_filter>(i);
				   break;
				}
			 }

			 if (i == cCRNMipFilterTotal)
			 {
			//	console::error("Invalid MipFilter: \"%s\"", val.get_ptr());
				return false;
			 }

			 if (i == cCRNMipFilterBox)
				mip_params.m_blurriness = 1.0f;
		  }

		  mip_params.m_gamma = m_params.get_value_as_float("gamma", 0, mip_params.m_gamma, .1f, 8.0f);
		  mip_params.m_gamma_filtering = (mip_params.m_gamma != 1.0f);

		  mip_params.m_blurriness = m_params.get_value_as_float("blurriness", 0, mip_params.m_blurriness, .01f, 8.0f);

		  mip_params.m_renormalize = m_params.get_value_as_bool("renormalize", 0, mip_params.m_renormalize != 0);
		  mip_params.m_tiled = m_params.get_value_as_bool("wrap");

		  mip_params.m_max_levels = m_params.get_value_as_int("maxmips", 0, cCRNMaxLevels, 1, cCRNMaxLevels);
		  mip_params.m_min_mip_size = m_params.get_value_as_int("minmipsize", 0, 1, 1, cCRNMaxLevelResolution);

		  return true;
	   }

	   bool parse_scale_params(crn_mipmap_params &mipmap_params)
	   {
		  if (m_params.has_key("rescale"))
		  {
			 int w = m_params.get_value_as_int("rescale", 0, -1, 1, cCRNMaxLevelResolution, 0);
			 int h = m_params.get_value_as_int("rescale", 0, -1, 1, cCRNMaxLevelResolution, 1);

			 mipmap_params.m_scale_mode = cCRNSMAbsolute;
			 mipmap_params.m_scale_x = (float)w;
			 mipmap_params.m_scale_y = (float)h;
		  }
		  else if (m_params.has_key("relrescale"))
		  {
			 float w = m_params.get_value_as_float("relrescale", 0, 1, 1, 256, 0);
			 float h = m_params.get_value_as_float("relrescale", 0, 1, 1, 256, 1);

			 mipmap_params.m_scale_mode = cCRNSMRelative;
			 mipmap_params.m_scale_x = w;
			 mipmap_params.m_scale_y = h;
		  }
		  else if (m_params.has_key("rescalemode"))
		  {
			 // nearest | hi | lo

			 crnlib::dynamic_string mode_str(m_params.get_value_as_string_or_empty("rescalemode"));
			 if (mode_str == "nearest")
				mipmap_params.m_scale_mode = cCRNSMNearestPow2;
			 else if (mode_str == "hi")
				mipmap_params.m_scale_mode = cCRNSMNextPow2;
			 else if (mode_str == "lo")
				mipmap_params.m_scale_mode = cCRNSMLowerPow2;
			 else
			 {
			//	console::error("Invalid rescale mode: \"%s\"", mode_str.get_ptr());
				return false;
			 }
		  }

		  if (m_params.has_key("clamp"))
		  {
			 uint32 w = m_params.get_value_as_int("clamp", 0, 1, 1, cCRNMaxLevelResolution, 0);
			 uint32 h = m_params.get_value_as_int("clamp", 0, 1, 1, cCRNMaxLevelResolution, 1);

			 mipmap_params.m_clamp_scale = false;
			 mipmap_params.m_clamp_width = w;
			 mipmap_params.m_clamp_height = h;
		  }
		  else if (m_params.has_key("clampScale"))
		  {
			 uint32 w = m_params.get_value_as_int("clampscale", 0, 1, 1, cCRNMaxLevelResolution, 0);
			 uint32 h = m_params.get_value_as_int("clampscale", 0, 1, 1, cCRNMaxLevelResolution, 1);

			 mipmap_params.m_clamp_scale = true;
			 mipmap_params.m_clamp_width = w;
			 mipmap_params.m_clamp_height = h;
		  }

		  if (m_params.has_key("window"))
		  {
			 uint32 xl = m_params.get_value_as_int("window", 0, 0, 0, cCRNMaxLevelResolution, 0);
			 uint32 yl = m_params.get_value_as_int("window", 0, 0, 0, cCRNMaxLevelResolution, 1);
			 uint32 xh = m_params.get_value_as_int("window", 0, 0, 0, cCRNMaxLevelResolution, 2);
			 uint32 yh = m_params.get_value_as_int("window", 0, 0, 0, cCRNMaxLevelResolution, 3);

			 mipmap_params.m_window_left = crnlib::math::minimum(xl, xh);
			 mipmap_params.m_window_top = crnlib::math::minimum(yl, yh);
			 mipmap_params.m_window_right = crnlib::math::maximum(xl, xh);
			 mipmap_params.m_window_bottom = crnlib::math::maximum(yl, yh);
		  }

		  return true;
	   }

	   bool parse_comp_params(crnlib::texture_file_types::format dst_file_format, crn_comp_params &comp_params)
	   {
		  if (dst_file_format == crnlib::texture_file_types::cFormatCRN)
			 comp_params.m_quality_level = cDefaultCRNQualityLevel;

		  if (m_params.has_key("q") || m_params.has_key("quality"))
		  {
			 const char *pKeyName = m_params.has_key("q") ? "q" : "quality";

			 if ((dst_file_format == crnlib::texture_file_types::cFormatDDS) || (dst_file_format == crnlib::texture_file_types::cFormatCRN) || (dst_file_format == crnlib::texture_file_types::cFormatKTX))
			 {
				uint32 i = m_params.get_value_as_int(pKeyName, 0, cDefaultCRNQualityLevel, 0, cCRNMaxQualityLevel);

				comp_params.m_quality_level = i;
			 }
			 else
			 {
			//	console::error("/quality or /q option is only invalid when writing DDS, KTX or CRN files!");
				return false;
			 }
		  }
		  else
		  {
			 float desired_bitrate = m_params.get_value_as_float("bitrate", 0, 0.0f, .1f, 30.0f);
			 if (desired_bitrate > 0.0f)
			 {
				comp_params.m_target_bitrate = desired_bitrate;
			 }
		  }

		  int color_endpoints = m_params.get_value_as_int("c", 0, 0, cCRNMinPaletteSize, cCRNMaxPaletteSize);
		  int color_selectors = m_params.get_value_as_int("s", 0, 0, cCRNMinPaletteSize, cCRNMaxPaletteSize);
		  int alpha_endpoints = m_params.get_value_as_int("ca", 0, 0, cCRNMinPaletteSize, cCRNMaxPaletteSize);
		  int alpha_selectors = m_params.get_value_as_int("sa", 0, 0, cCRNMinPaletteSize, cCRNMaxPaletteSize);
		  if ( ((color_endpoints > 0) && (color_selectors > 0)) ||
			   ((alpha_endpoints > 0) && (alpha_selectors > 0)) )
		  {
			 comp_params.set_flag(cCRNCompFlagManualPaletteSizes, true);
			 comp_params.m_crn_color_endpoint_palette_size = color_endpoints;
			 comp_params.m_crn_color_selector_palette_size = color_selectors;
			 comp_params.m_crn_alpha_endpoint_palette_size = alpha_endpoints;
			 comp_params.m_crn_alpha_selector_palette_size = alpha_selectors;
		  }

		  if (m_params.has_key("alphaThreshold"))
		  {
			 int dxt1a_alpha_threshold = m_params.get_value_as_int("alphaThreshold", 0, 128, 0, 255);
			 comp_params.m_dxt1a_alpha_threshold = dxt1a_alpha_threshold;
			 if (dxt1a_alpha_threshold > 0)
			 {
				comp_params.set_flag(cCRNCompFlagDXT1AForTransparency, true);
			 }
		  }

		  comp_params.set_flag(cCRNCompFlagPerceptual, !m_params.get_value_as_bool("uniformMetrics"));
		  comp_params.set_flag(cCRNCompFlagHierarchical, !m_params.get_value_as_bool("noAdaptiveBlocks"));

		  if (m_params.has_key("helperThreads"))
			 comp_params.m_num_helper_threads = m_params.get_value_as_int("helperThreads", 0, cCRNMaxHelperThreads, 0, cCRNMaxHelperThreads);
		  else if (crnlib::g_number_of_processors > 1)
			 comp_params.m_num_helper_threads = crnlib::g_number_of_processors - 1;

		  crnlib::dynamic_string comp_name;
		  if (m_params.get_value_as_string("compressor", 0, comp_name))
		  {
			 uint32 i;
			 for (i = 0; i < cCRNTotalDXTCompressors; i++)
			 {
				if (comp_name == crnlib::get_dxt_compressor_name(static_cast<crn_dxt_compressor_type>(i)))
				{
				   comp_params.m_dxt_compressor_type = static_cast<crn_dxt_compressor_type>(i);
				   break;
				}
			 }
			 if (i == cCRNTotalDXTCompressors)
			 {
			//	console::error("Invalid compressor: \"%s\"", comp_name.get_ptr());
				return false;
			 }
		  }

		  crnlib::dynamic_string dxt_quality_str;
		  if (m_params.get_value_as_string("dxtquality", 0, dxt_quality_str))
		  {
			 uint32 i;
			 for (i = 0; i < cCRNDXTQualityTotal; i++)
			 {
				if (dxt_quality_str == crn_get_dxt_quality_string(static_cast<crn_dxt_quality>(i)))
				{
				   comp_params.m_dxt_quality = static_cast<crn_dxt_quality>(i);
				   break;
				}
			 }
			 if (i == cCRNDXTQualityTotal)
			 {
			//	console::error("Invalid DXT quality: \"%s\"", dxt_quality_str.get_ptr());
				return false;
			 }
		  }
		  else
		  {
			 comp_params.m_dxt_quality = cCRNDXTQualityUber;
		  }

		  comp_params.set_flag(cCRNCompFlagDisableEndpointCaching, m_params.get_value_as_bool("noendpointcaching"));
		  comp_params.set_flag(cCRNCompFlagGrayscaleSampling, m_params.get_value_as_bool("grayscalesampling"));
		  comp_params.set_flag(cCRNCompFlagUseBothBlockTypes, !m_params.get_value_as_bool("forceprimaryencoding"));
		  if (comp_params.get_flag(cCRNCompFlagUseBothBlockTypes))
			 comp_params.set_flag(cCRNCompFlagUseTransparentIndicesForBlack, m_params.get_value_as_bool("usetransparentindicesforblack"));
		  else
			 comp_params.set_flag(cCRNCompFlagUseTransparentIndicesForBlack, false);

		  return true;
	   }

		convert_status convert_file(const rapidjson::Value& rapidJsonValueTextureAssetCompiler, const char* pSrc_filename, const char* pDst_filename, crnlib::texture_file_types::format out_file_type, TextureSemantic textureSemantic, bool createMipmaps)
		{
			crnlib::texture_file_types::format src_file_format = crnlib::texture_file_types::cFormatInvalid;
			crnlib::texture_conversion::convert_params params;

			crnlib::mipmapped_texture src_tex;
			if (TextureSemantic::REFLECTION_CUBE_MAP == textureSemantic)
			{
				// The face order must be: +X, –X, –Y, +Y, +Z, –Z
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
						src_tex.init(width, width, 1, 6, pixelFormat, "", crnlib::cDefaultOrientationFlags);
					}
					else if (src_tex.get_format() != pixelFormat)
					{
						throw std::runtime_error("The pixel format of all cube map faces must be identical");
					}
					else if (src_tex.get_width() != source2DImage->get_width())
					{
						throw std::runtime_error("The size of all cube map faces must be identical");
					}
					src_tex.get_level(faceIndex, 0)->assign(source2DImage);
				}
			}
			else
			{
				crnlib::texture_file_types::format src_file_format = crnlib::texture_file_types::determine_file_format(pSrc_filename);
				if (src_file_format == crnlib::texture_file_types::cFormatInvalid)
				{
				//	console::error("Unrecognized file type: %s", pSrc_filename);
					return cCSFailed;
				}

				if (!src_tex.read_from_file(pSrc_filename, src_file_format))
				{
					/*
					if (src_tex.get_last_error().is_empty())
						console::error("Failed reading source file: \"%s\"", pSrc_filename);
					else
						console::error("%s", src_tex.get_last_error().get_ptr());
					*/

					return cCSFailed;
				}

				if (crnlib::texture_file_types::supports_mipmaps(src_file_format))
				{
					params.m_mipmap_params.m_mode = cCRNMipModeUseSourceMips;
				}
			}

			if (m_params.get_value_as_bool("converttoluma"))
				src_tex.convert(crnlib::image_utils::cConversion_Y_To_RGB);
			if (m_params.get_value_as_bool("setalphatoluma"))
				src_tex.convert(crnlib::image_utils::cConversion_Y_To_A);

			params.m_texture_type = src_tex.determine_texture_type();
			params.m_pInput_texture = &src_tex;
			params.m_dst_filename = pDst_filename;
			params.m_dst_file_type = out_file_type;
			params.m_lzma_stats = m_params.has_key("lzmastats");
			params.m_write_mipmaps_to_multiple_files = m_params.has_key("split");
			params.m_always_use_source_pixel_format = m_params.has_key("usesourceformat");
			params.m_y_flip = true;
		//	params.m_y_flip = m_params.has_key("yflip");
		//	params.m_unflip = true;
			params.m_unflip = m_params.has_key("unflip");

		//	if ((!m_params.get_value_as_bool("noprogress")) && (!m_params.get_value_as_bool("quiet")))
		//		params.m_pProgress_func = progress_callback_func;

			if (m_params.get_value_as_bool("debug"))
			{
				params.m_debugging = true;
				params.m_comp_params.set_flag(cCRNCompFlagDebugging, true);
			}

			if (m_params.get_value_as_bool("paramdebug"))
				params.m_param_debugging = true;

			if (m_params.get_value_as_bool("quick"))
				params.m_quick = true;

			params.m_no_stats = m_params.get_value_as_bool("nostats");

			params.m_dst_format = crnlib::PIXEL_FMT_INVALID;

			for (uint32 i = 0; i < crnlib::pixel_format_helpers::get_num_formats(); i++)
			{
				crnlib::pixel_format trial_fmt = crnlib::pixel_format_helpers::get_pixel_format_by_index(i);
				if (m_params.has_key(crnlib::pixel_format_helpers::get_pixel_format_string(trial_fmt)))
				{
					params.m_dst_format = trial_fmt;
					break;
				}
			}

			if (!parse_mipmap_params(params.m_mipmap_params))
				return cCSBadParam;

			if (!parse_comp_params(params.m_dst_file_type, params.m_comp_params))
				return cCSBadParam;

			if (!parse_scale_params(params.m_mipmap_params))
				return cCSBadParam;

			// The 4x4 block size based DXT compression format has no support for 1D textures
			if (src_tex.get_width() == 1 || src_tex.get_height() == 1)
			{
				params.m_dst_format = crnlib::PIXEL_FMT_A8R8G8B8;
			}

			// Evaluate texture semantic and figure out whether or not the destination format will be DXT compressed
			bool dxtCompressed = (crnlib::pixel_format::PIXEL_FMT_INVALID == params.m_dst_format) ? true : crnlib::pixel_format_helpers::is_dxt(params.m_dst_format);
			switch (textureSemantic)
			{
				case TextureSemantic::DIFFUSE_MAP:
					// Nothing here, just a regular texture
					break;

				case TextureSemantic::ALPHA_MAP:
					params.m_comp_params.set_flag(cCRNCompFlagPerceptual, false);
					break;

				case TextureSemantic::NORMAL_MAP:
					params.m_texture_type = crnlib::cTextureTypeNormalMap;
					params.m_comp_params.set_flag(cCRNCompFlagPerceptual, false);
					params.m_mipmap_params.m_renormalize = true;

					// Do never ever store normal maps standard DXT1 compressed to not get horrible artefact's due to compressing vector data using algorithms design for color data
					// -> See "Real-Time Normal Map DXT Compression" -> "3.3 Tangent-Space 3Dc" - http://www.nvidia.com/object/real-time-normal-map-dxt-compression.html
					if (crnlib::pixel_format::PIXEL_FMT_INVALID == params.m_dst_format || crnlib::pixel_format::PIXEL_FMT_DXT1 == params.m_dst_format)
					{
						params.m_dst_format = crnlib::PIXEL_FMT_3DC;
						dxtCompressed = true;
					}
					break;

				case TextureSemantic::SPECULAR_MAP:
					params.m_comp_params.set_flag(cCRNCompFlagPerceptual, false);
					break;

				case TextureSemantic::EMISSIVE_MAP:
					// Nothing here, just a regular texture
					break;

				case TextureSemantic::REFLECTION_CUBE_MAP:
					params.m_texture_type = crnlib::cTextureTypeCubemap;
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
				uint32 width = src_tex.get_width();
				uint32 height = src_tex.get_height();
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
												  widthHeightToString(src_tex.get_width(), src_tex.get_height()) + ". Dynamic texture resolution scale will be limited to mipmap level " + std::to_string(mipmap - 1) + '.';
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
			params.m_mipmap_params.m_mode = createMipmaps ? cCRNMipModeGenerateMips : cCRNMipModeNoMips;

			crnlib::texture_conversion::convert_stats stats;

			bool status = crnlib::texture_conversion::process(params, stats);
			if (!status)
			{
				/*
				if (params.m_error_message.is_empty())
					console::error("Failed writing output file: \"%s\"", pDst_filename);
				else
					console::error(params.m_error_message.get_ptr());
				*/
				return cCSFailed;
			}

			return cCSSucceeded;
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


	/*
      //                -------------------------------------------------------------------------------
      console::message("\nCommand line usage:");
      console::printf("crunch [options] -file filename");
      console::printf("-file filename - Required input filename, wildcards, multiple -file params OK.");
      console::printf("-file @list.txt - List of files to convert.");
      console::printf("Supported source file formats: dds,ktx,crn,tga,bmp,png,jpg/jpeg,psd");
      console::printf("Note: Some file format variants are unsupported.");
      console::printf("See the docs for stb_image.c: http://www.nothings.org/stb_image.c");
      console::printf("Progressive JPEG files are supported, see: http://code.google.com/p/jpeg-compressor/");

      console::message("\nPath/file related parameters:");
      console::printf("-out filename - Output filename");
      console::printf("-outdir dir - Output directory");
      console::printf("-outsamedir - Write output file to input directory");
      console::printf("-deep - Recurse subdirectories, default=false");
      console::printf("-nooverwrite - Don't overwrite existing files");
      console::printf("-timestamp - Update only changed files");
      console::printf("-forcewrite - Overwrite read-only files");
      console::printf("-recreate - Recreate directory structure");
      console::printf("-fileformat [dds,ktx,crn,tga,bmp,png] - Output file format, default=crn or dds");

      console::message("\nModes:");
      console::printf("-compare - Compare input and output files (no output files are written).");
      console::printf("-info - Only display input file statistics (no output files are written).");

      console::message("\nMisc. options:");
      console::printf("-helperThreads # - Set number of helper threads, 0-16, default=(# of CPU's)-1");
      console::printf("-noprogress - Disable progress output");
      console::printf("-quiet - Disable all console output");
      console::printf("-ignoreerrors - Continue processing files after errors. Note: The default");
      console::printf("                behavior is to immediately exit whenever an error occurs.");
      console::printf("-logfile filename - Append output to log file");
      console::printf("-pause - Wait for keypress on error");
      console::printf("-window <left> <top> <right> <bottom> - Crop window before processing");
      console::printf("-clamp <width> <height> - Crop image if larger than width/height");
      console::printf("-clampscale <width> <height> - Scale image if larger than width/height");
      console::printf("-nostats - Disable all output file statistics (faster)");
      console::printf("-imagestats - Print various image qualilty statistics");
      console::printf("-mipstats - Print statistics for each mipmap, not just the top mip");
      console::printf("-lzmastats - Print size of output file compressed with LZMA codec");
      console::printf("-split - Write faces/mip levels to multiple separate output PNG files");
      console::printf("-yflip - Always flip texture on Y axis before processing");
      console::printf("-unflip - Unflip texture if read from source file as flipped");

      console::message("\nImage rescaling (mutually exclusive options)");
      console::printf("-rescale <int> <int> - Rescale image to specified resolution");
      console::printf("-relscale <float> <float> - Rescale image to specified relative resolution");
      console::printf("-rescalemode <nearest | hi | lo> - Auto-rescale non-power of two images");
      console::printf(" nearest - Use nearest power of 2, hi - Use next, lo - Use previous");

      console::message("\nDDS/CRN compression quality control:");
      console::printf("-quality # (or /q #) - Set Clustered DDS/CRN quality factor [0-255] 255=best");
      console::printf("       DDS default quality is best possible.");
      console::printf("       CRN default quality is %u.", cDefaultCRNQualityLevel);
      console::printf("-bitrate # - Set the desired output bitrate of DDS or CRN output files.");
      console::printf("             This option causes crunch to find the quality factor");
      console::printf("             closest to the desired bitrate using a binary search.");

      console::message("\nLow-level CRN specific options:");
      console::printf("-c # - Color endpoint palette size, 32-8192, default=3072");
      console::printf("-s # - Color selector palette size, 32-8192, default=3072");
      console::printf("-ca # - Alpha endpoint palette size, 32-8192, default=3072");
      console::printf("-sa # - Alpha selector palette size, 32-8192, default=3072");

      //                -------------------------------------------------------------------------------
      console::message("\nMipmap filtering options:");
      console::printf("-mipMode [UseSourceOrGenerate,UseSource,Generate,None]");
      console::printf("         Default mipMode is UseSourceOrGenerate");
      console::printf(" UseSourceOrGenerate: Use source mipmaps if possible, or create new mipmaps.");
      console::printf(" UseSource: Always use source mipmaps, if any (never generate new mipmaps)");
      console::printf(" Generate: Always generate a new mipmap chain (ignore source mipmaps)");
      console::printf(" None: Do not output any mipmaps");
      console::printf("-mipFilter [box,tent,lanczos4,mitchell,kaiser], default=kaiser");
      console::printf("-gamma # - Mipmap gamma correction value, default=2.2, use 1.0 for linear");
      console::printf("-blurriness # - Scale filter kernel, >1=blur, <1=sharpen, .01-8, default=.9");
      console::printf("-wrap - Assume texture is tiled when filtering, default=clamping");
      console::printf("-renormalize - Renormalize filtered normal map texels, default=disabled");
      console::printf("-maxmips # - Limit number of generated texture mipmap levels, 1-16, default=16");
      console::printf("-minmipsize # - Smallest allowable mipmap resolution, default=1");

      console::message("\nCompression options:");
      console::printf("-alphaThreshold # - Set DXT1A alpha threshold, 0-255, default=128");
      console::printf(" Note: -alphaThreshold also changes the compressor's behavior to");
      console::printf(" prefer DXT1A over DXT5 for images with alpha channels (.DDS only).");
      console::printf("-uniformMetrics - Use uniform color metrics, default=use perceptual metrics");
      console::printf("-noAdaptiveBlocks - Disable adaptive block sizes (i.e. disable macroblocks).");
      console::printf("-compressor [CRN,CRNF,RYG,ATI] - Set DXTn compressor, default=CRN");
      console::printf("-dxtQuality [superfast,fast,normal,better,uber] - Endpoint optimizer speed.");
      console::printf("            Sets endpoint optimizer's max iteration depth. Default=uber.");
      console::printf("-noendpointcaching - Don't try reusing previous DXT endpoint solutions.");
      console::printf("-grayscalsampling - Assume shader will convert fetched results to luma (Y).");
      console::printf("-forceprimaryencoding - Only use DXT1 color4 and DXT5 alpha8 block encodings.");
      console::printf("-usetransparentindicesforblack - Try DXT1 transparent indices for dark pixels.");

      console::message("\nOuptut pixel format options:");
      console::printf("-usesourceformat - Use input file's format for output format (when possible).");
      console::message("\nAll supported texture formats (Note: .CRN only supports DXTn pixel formats):");
      for (uint32 i = 0; i < pixel_format_helpers::get_num_formats(); i++)
      {
         pixel_format fmt = pixel_format_helpers::get_pixel_format_by_index(i);
         console::printf("-%s", pixel_format_helpers::get_pixel_format_string(fmt));
      }
		All support
		-DXT1
		-DXT2
		-DXT3
		-DXT4
		-DXT5
		-3DC
		-DXN
		-DXT5A
		-DXT5_CCxY
		-DXT5_xGxR
		-DXT5_xGBR
		-DXT5_AGBR
		-DXT1A
		-ETC1
		-R8G8B8
		-L8
		-A8
		-A8L8
		-A8R8G8B8


      console::printf("\nFor bugs, support, or feedback: richgel99@gmail.com");
   }
	*/



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
		// TODO(co) Add required properties
		std::string inputFile;
		std::string assetFileFormat;
		::detail::TextureSemantic textureSemantic = ::detail::TextureSemantic::UNKNOWN;
		bool createMipmaps = true;
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

		// TODO(co) Everything rough prototype: Renderer target dependent stuff
		std::string outputAssetFilename;
		std::string testString;
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
					testString = std::string("-file " + inputAssetFilename + " -out " + outputAssetFilename);
				}
				else if (fileFormat == "dds")
				{
					testString = std::string("-file " + inputAssetFilename + " -out " + outputAssetFilename + " -fileformat dds");
				}
				else if (fileFormat == "ktx")
				{
					testString = std::string("-file " + inputAssetFilename + " -out " + outputAssetFilename + " -fileformat ktx -ETC1");
				}
			}
		}

		// Ask cache manager if we need to compile the source file (e.g. source changed or target not there)
		if (CacheManager::needsToBeCompiled(configuration.rendererTarget, inputAssetFilename, outputAssetFilename))
		{
			// We need to compile the source
			const char* pCommand_line = testString.c_str();
			crnlib::command_line_params::param_desc std_params[] =
			{
				{ "file", 1, true },

				{ "out", 1, false },
				{ "outdir", 1, false },
				{ "outsamedir", 0, false },
				{ "deep", 0, false },
				{ "fileformat", 1, false },

				{ "helperThreads", 1, false },
				{ "noprogress", 0, false },
				{ "quiet", 0, false },
				{ "ignoreerrors", 0, false },
				{ "logfile", 1, false },

				{ "q", 1, false },
				{ "quality", 1, false },

				{ "c", 1, false },
				{ "s", 1, false },
				{ "ca", 1, false },
				{ "sa", 1, false },

				{ "mipMode", 1, false },
				{ "mipFilter", 1, false },
				{ "gamma", 1, false },
				{ "blurriness", 1, false },
				{ "wrap", 0, false },
				{ "renormalize", 0, false },
				{ "noprogress", 0, false },
				{ "paramdebug", 0, false },
				{ "debug", 0, false },
				{ "quick", 0, false },
				{ "imagestats", 0, false },
				{ "nostats", 0, false },
				{ "mipstats", 0, false },

				{ "alphaThreshold", 1, false },
				{ "uniformMetrics", 0, false },
				{ "noAdaptiveBlocks", 0, false },
				{ "compressor", 1, false },
				{ "dxtQuality", 1, false },
				{ "noendpointcaching", 0, false },
				{ "grayscalesampling", 0, false  },
				{ "converttoluma", 0, false  },
				{ "setalphatoluma", 0, false  },
				{ "pause", 0, false  },
				{ "timestamp", 0, false  },
				{ "nooverwrite", 0, false  },
				{ "forcewrite", 0, false  },
				{ "recreate", 0, false  },
				{ "compare", 0, false  },
				{ "info", 0, false  },
				{ "forceprimaryencoding", 0, false },
				{ "usetransparentindicesforblack", 0, false  },
				{ "usesourceformat", 0, false  },

				{ "rescalemode", 1, false },
				{ "rescale", 2, false },
				{ "relrescale", 2, false },
				{ "clamp", 2, false },
				{ "clampScale", 2, false },
				{ "window", 4, false },

				{ "maxmips", 1, false },
				{ "minmipsize", 1, false },

				{ "bitrate", 1, false },

				{ "lzmastats", 0, false },
				{ "split", 0, false },
				{ "csvfile", 1, false },

				{ "yflip", 0, false },
				{ "unflip", 0, false },
			};

			crnlib::vector<crnlib::command_line_params::param_desc> params;
			params.append(std_params, glm::countof(std_params));

			for (uint32 i = 0; i < crnlib::pixel_format_helpers::get_num_formats(); i++)
			{
				crnlib::pixel_format fmt = crnlib::pixel_format_helpers::get_pixel_format_by_index(i);

				crnlib::command_line_params::param_desc desc;
				desc.m_pName = crnlib::pixel_format_helpers::get_pixel_format_string(fmt);
				desc.m_num_values = 0;
				desc.m_support_listing_file = false;
				params.push_back(desc);
			}

			detail::m_params.clear();
			if (!detail::m_params.parse(pCommand_line, params.size(), params.get_ptr(), true))
			{
				// Error!
				return;
			}


			crnlib::texture_file_types::format out_file_type = crnlib::texture_file_types::cFormatCRN;
			crnlib::dynamic_string fmt;
			if (detail::m_params.get_value_as_string("fileformat", 0, fmt))
			{
				if (fmt == "tga")
				out_file_type = crnlib::texture_file_types::cFormatTGA;
				else if (fmt == "bmp")
				out_file_type = crnlib::texture_file_types::cFormatBMP;
				else if (fmt == "dds")
				out_file_type = crnlib::texture_file_types::cFormatDDS;
				else if (fmt == "ktx")
				out_file_type = crnlib::texture_file_types::cFormatKTX;
				else if (fmt == "crn")
				out_file_type = crnlib::texture_file_types::cFormatCRN;
				else if (fmt == "png")
				out_file_type = crnlib::texture_file_types::cFormatPNG;
				else
				{
					// console::error("Unsupported output file type: %s", fmt.get_ptr());
					return;
				}
			}

			// No explicit output format has been specified - try to determine something doable.
			if (!detail::m_params.has_key("fileformat"))
			{
				if (detail::m_params.has_key("split"))
				{
					out_file_type = crnlib::texture_file_types::cFormatPNG;
				}
				else
				{
				crnlib::texture_file_types::format input_file_type = crnlib::texture_file_types::determine_file_format(inputAssetFilename.c_str());
				if (input_file_type == crnlib::texture_file_types::cFormatCRN)
				{
					// Automatically transcode CRN->DXTc and write to DDS files, unless the user specifies either the /fileformat or /split options.
					out_file_type = crnlib::texture_file_types::cFormatDDS;
				}
				else if (input_file_type == crnlib::texture_file_types::cFormatKTX)
				{
					// Default to converting KTX files to PNG
					out_file_type = crnlib::texture_file_types::cFormatPNG;
				}
				}
			}

			if (::detail::TextureSemantic::COLOR_CORRECTION_LOOKUP_TABLE == textureSemantic)
			{
				detail::convertColorCorrectionLookupTable(inputAssetFilename.c_str(), outputAssetFilename.c_str());
			}
			else
			{
				detail::convert_file(rapidJsonValueTextureAssetCompiler, inputAssetFilename.c_str(), outputAssetFilename.c_str(), out_file_type, textureSemantic, createMipmaps);
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



	  /*
		
		// Open the input file
//		std::ifstream inputFileStream(assetInputDirectory + inputFile, std::ios::binary);
		const std::string assetName = rapidJsonValueAsset["AssetMetadata"]["AssetName"].GetString();
		const std::string assetFilename = assetOutputDirectory + assetName + ".dds";	// TODO(co) Make this dynamic
		std::ofstream outputFileStream(assetFilename, std::ios::binary);

		// "crunch"-settings
		float	   bitrate = 0.0f;
		int		   quality_level = -1;
		bool	   srgb_colorspace = true;
		bool	   create_mipmaps = true;
		bool	   output_crn = false;	// TODO(co) Just a test
		crn_format fmt = cCRNFmtInvalid;
		bool	   use_adaptive_block_sizes = true;
		bool	   set_alpha_to_luma = false;
		bool	   convert_to_luma = false;
		bool	   enable_dxt1a = false;

		// Load source image
		// TODO(co)
		// If image loading fails for any reason, the return value will be NULL,
		// and *x, *y, *comp will be unchanged. The function stbi_failure_reason()
		// can be queried for an extremely brief, end-user unfriendly explanation
		// of why the load failed. Define STBI_NO_FAILURE_STRINGS to avoid
		// compiling these strings at all, and STBI_FAILURE_USERMSG to get slightly
		// more user-friendly ones.
		int width, height, actual_comps;


		crn_uint32 *pSrc_image = nullptr;


			// TODO(co) Tif support
			TIFF* tif = TIFFOpen((assetInputDirectory + inputFile).c_str(), "r");
			if (tif) {
				uint32 w, h;
				size_t npixels;
				uint32* raster;
        
				TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
				TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
				npixels = w * h;
				raster = new uint32[npixels];
				if (raster != NULL) {
					if (TIFFReadRGBAImage(tif, w, h, raster, 0)) {
						width = w;
						height = h;
						actual_comps = 3;
					  pSrc_image = raster;
					}
				  //  _TIFFfree(raster);
				}
				TIFFClose(tif);
			}
	*/


		// Load the source image file into memory
	/*	crn_uint32 src_file_size = 0;
		crn_uint8 *pSrc_file_data = nullptr;
		{
			// Get file size and file data
			inputFileStream.seekg(0, std::ifstream::end);
			src_file_size = static_cast<crn_uint32>(inputFileStream.tellg());
			inputFileStream.seekg(0, std::ifstream::beg);
			pSrc_file_data = new crn_uint8[static_cast<size_t>(src_file_size)];
			inputFileStream.read((char*)pSrc_file_data, src_file_size);
		}
		crn_uint32 *pSrc_image = (crn_uint32*)stbi_load_from_memory(pSrc_file_data, src_file_size, &width, &height, &actual_comps, 4);
		if (!pSrc_image)
		{
			free(pSrc_file_data);
			throw std::runtime_error("Failed reading image file");
		}*/

		/*


		// Fill in compression parameters struct.
		bool has_alpha_channel = actual_comps > 3;

		if ((fmt == cCRNFmtDXT5A) && (actual_comps <= 3))
			set_alpha_to_luma = true;

		if ((set_alpha_to_luma) || (convert_to_luma))
		{
			for (int i = 0; i < width * height; i++)
			{
				crn_uint32 r = pSrc_image[i] & 0xFF, g = (pSrc_image[i] >> 8) & 0xFF, b = (pSrc_image[i] >> 16) & 0xFF;

				// Compute CCIR 601 luma.
				crn_uint32 y = (19595U * r + 38470U * g + 7471U * b + 32768) >> 16U;
				crn_uint32 a = (pSrc_image[i] >> 24) & 0xFF;
				if (set_alpha_to_luma) a = y;
				if (convert_to_luma) { r = y; g = y; b = y; }
				pSrc_image[i] = r | (g << 8) | (b << 16) | (a << 24);
			}
		}

		crn_comp_params comp_params;
		comp_params.m_width = width;
		comp_params.m_height = height;
		comp_params.set_flag(cCRNCompFlagPerceptual, srgb_colorspace);
		comp_params.set_flag(cCRNCompFlagDXT1AForTransparency, enable_dxt1a && has_alpha_channel);
		comp_params.set_flag(cCRNCompFlagHierarchical, use_adaptive_block_sizes);
		comp_params.m_file_type = output_crn ? cCRNFileTypeCRN : cCRNFileTypeDDS;
		comp_params.m_format = (fmt != cCRNFmtInvalid) ? fmt : (has_alpha_channel ? cCRNFmtDXT5 : cCRNFmtDXT1);

		// Important note: This example only feeds a single source image to the compressor, and it internaly generates mipmaps from that source image.
		// If you want, there's nothing stopping you from generating the mipmaps on your own, then feeding the multiple source images 
		// to the compressor. Just set the crn_mipmap_params::m_mode member (set below) to cCRNMipModeUseSourceMips.
		comp_params.m_pImages[0][0] = pSrc_image;

		if (bitrate > 0.0f)
			comp_params.m_target_bitrate = bitrate;
		else if (quality_level >= 0)
			comp_params.m_quality_level = quality_level;
		else if (output_crn)
		{
			// Set a default quality level for CRN, otherwise we'll get the default (highest quality) which leads to huge compressed palettes.
			comp_params.m_quality_level = detail::cDefaultCRNQualityLevel;
		}


// TODO(co) Just a test
comp_params.m_dxt_quality = cCRNDXTQualitySuperFast;
comp_params.m_quality_level = cCRNMinQualityLevel;


		// Determine the # of helper threads (in addition to the main thread) to use during compression. NumberOfCPU's-1 is reasonable.
		SYSTEM_INFO g_system_info;
		GetSystemInfo(&g_system_info);  
		int num_helper_threads = std::max<int>(0, (int)g_system_info.dwNumberOfProcessors - 1);
		comp_params.m_num_helper_threads = num_helper_threads;

		comp_params.m_pProgress_func = detail::progress_callback_func;

		// Fill in mipmap parameters struct.
		crn_mipmap_params mip_params;
		mip_params.m_gamma_filtering = srgb_colorspace;
		mip_params.m_mode = create_mipmaps ? cCRNMipModeGenerateMips : cCRNMipModeNoMips;

		crn_uint32 actual_quality_level;
		float actual_bitrate;
		crn_uint32 output_file_size;



		// TODO(co) Just a test
		mip_params.m_mode = cCRNMipModeNoMips;
		comp_params.m_file_type = cCRNFileTypeDDS;
		comp_params.m_format = cCRNFmtETC1;
		comp_params.m_dxt_compressor_type = cCRNDXTCompressorCRN;



		// Now compress to DDS or CRN.
		void *pOutput_file_data = crn_compress(comp_params, mip_params, output_file_size, &actual_quality_level, &actual_bitrate);
		if (!pOutput_file_data)
		{
			stbi_image_free(pSrc_image);
//			free(pSrc_file_data);
			throw std::runtime_error("Compression failed");
		}

		// Write the output file.
		outputFileStream.write(reinterpret_cast<const char*>(pOutput_file_data), output_file_size);

		crn_free_block(pOutput_file_data);
		stbi_image_free(pSrc_image);


		*/

/*
		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/Texture/" + assetCategory + '/' + assetName;

			// Output asset
			RendererRuntime::Asset outputAsset;
			outputAsset.assetId = StringHelper::getAssetIdByString(assetIdAsString.c_str());
			strcpy(outputAsset.outputAssetFilename, outputAssetFilename.c_str());	// TODO(co) Buffer overflow test
			outputAssetPackage.getWritableSortedAssetVector().push_back(outputAsset);
		}
		*/
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
