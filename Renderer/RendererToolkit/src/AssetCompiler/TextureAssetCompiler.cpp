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


// TODO(co) Cleanup


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/AssetCompiler/TextureAssetCompiler.h"
#include "RendererToolkit/PlatformTypes.h"

#include <RendererRuntime/Asset/AssetPackage.h>

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
#include <crunch/stb_image.h>	// TODO(co) This is another library

#include <crunch/crnlib.h>

#include <libtiff/tiffio.h>

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	// CRN/DDS compression callback function.
	namespace detail
	{
		const int cDefaultCRNQualityLevel = 128;

		static crn_bool progress_callback_func(crn_uint32 phase_index, crn_uint32 total_phases, crn_uint32 subphase_index, crn_uint32 total_subphases, void* pUser_data_ptr)
		{
			int percentage_complete = (int)(.5f + (phase_index + float(subphase_index) / total_subphases) * 100.0f) / total_phases;
			//printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bProcessing: %u%%", std::min(100, std::max(0, percentage_complete)));
			return true;
		}
	}


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
	void TextureAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Input, configuration and output
		const std::string&			   assetInputDirectory	= input.assetInputDirectory;
		const std::string&			   assetOutputDirectory	= input.assetOutputDirectory;
		Poco::JSON::Object::Ptr		   jsonAssetRootObject	= configuration.jsonAssetRootObject;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		Poco::JSON::Object::Ptr jsonAssetObject = jsonAssetRootObject->get("Asset").extract<Poco::JSON::Object::Ptr>();

		// Read configuration
		// TODO(co) Add required properties
		std::string inputFile;
		uint32_t test = 0;
		{
			// Read texture asset compiler configuration
			Poco::JSON::Object::Ptr jsonConfigurationObject = jsonAssetObject->get("TextureAssetCompiler").extract<Poco::JSON::Object::Ptr>();
			inputFile = jsonConfigurationObject->getValue<std::string>("InputFile");
			test	  = jsonConfigurationObject->optValue<uint32_t>("Test", test);
		}

		// Open the input file
//		std::ifstream ifstream(assetInputDirectory + inputFile, std::ios::binary);
		const std::string assetName = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetName");
		const std::string assetFilename = assetOutputDirectory + assetName + ".dds";	// TODO(co) Make this dynamic
		std::ofstream ofstream(assetFilename, std::ios::binary);

		// "crunch"-settings
		float	   bitrate = 0.0f;
		int		   quality_level = -1;
		bool	   srgb_colorspace = true;
		bool	   create_mipmaps = true;
		bool	   output_crn = true;	// TODO(co) Just a test
		crn_format fmt = cCRNFmtInvalid;
		bool	   use_adaptive_block_sizes = true;
		bool	   set_alpha_to_luma = false;
		bool	   convert_to_luma = false;
		bool	   enable_dxt1a = false;

		// Load source image
		// TODO(co)
		/*
		// If image loading fails for any reason, the return value will be NULL,
		// and *x, *y, *comp will be unchanged. The function stbi_failure_reason()
		// can be queried for an extremely brief, end-user unfriendly explanation
		// of why the load failed. Define STBI_NO_FAILURE_STRINGS to avoid
		// compiling these strings at all, and STBI_FAILURE_USERMSG to get slightly
		// more user-friendly ones.
		*/
		int width, height, actual_comps;


		crn_uint32 *pSrc_image = nullptr;


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


		// Load the source image file into memory
	/*	crn_uint32 src_file_size = 0;
		crn_uint8 *pSrc_file_data = nullptr;
		{
			// Get file size and file data
			ifstream.seekg(0, std::ifstream::end);
			src_file_size = static_cast<crn_uint32>(ifstream.tellg());
			ifstream.seekg(0, std::ifstream::beg);
			pSrc_file_data = new crn_uint8[static_cast<size_t>(src_file_size)];
			ifstream.read((char*)pSrc_file_data, src_file_size);
		}
		crn_uint32 *pSrc_image = (crn_uint32*)stbi_load_from_memory(pSrc_file_data, src_file_size, &width, &height, &actual_comps, 4);
		if (!pSrc_image)
		{
			free(pSrc_file_data);
			throw std::exception("Failed reading image file");
		}*/




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

		// Now compress to DDS or CRN.
		void *pOutput_file_data = crn_compress(comp_params, mip_params, output_file_size, &actual_quality_level, &actual_bitrate);
		if (!pOutput_file_data)
		{
			stbi_image_free(pSrc_image);
//			free(pSrc_file_data);
			throw std::exception("Compression failed");
		}

		// Write the output file.
		ofstream.write(reinterpret_cast<const char*>(pOutput_file_data), output_file_size);

		crn_free_block(pOutput_file_data);
		stbi_image_free(pSrc_image);



		{ // Update the output asset package
			const std::string assetCategory = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetCategory");
			const std::string assetIdAsString = input.projectName + "/Texture/" + assetCategory + '/' + assetName;

			// Output asset
			RendererRuntime::AssetPackage::Asset outputAsset;
			outputAsset.assetId = RendererRuntime::StringId(assetIdAsString.c_str());
			strcpy(outputAsset.assetFilename, assetFilename.c_str());	// TODO(co) Buffer overflow test
			outputAssetPackage.getWritableSortedAssetVector().push_back(outputAsset);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
