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
// Disable some warnings for "std::transform()"-usage
#include <RendererRuntime/Core/Platform/PlatformTypes.h>
PRAGMA_WARNING_DISABLE_MSVC(4242)	// warning C4242: '=': conversion from 'int' to 'char', possible loss of data
PRAGMA_WARNING_DISABLE_MSVC(4244)	// warning C4244: '=': conversion from 'int' to 'char', possible loss of data

#include "RendererToolkit/AssetImporter/SketchfabAssetImporter.h"
#include "RendererToolkit/Helper/JsonHelper.h"
#include "RendererToolkit/Context.h"

#include <RendererRuntime/Core/File/IFile.h>
#include <RendererRuntime/Core/File/IFileManager.h>
#include <RendererRuntime/Core/File/FileSystemHelper.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4061)	// warning C4061: enumerator 'rapidjson::kNumberType' in switch of enum 'rapidjson::Type' is not explicitly handled by a case label
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'std::codecvt_base': assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <rapidjson/document.h>
PRAGMA_WARNING_POP

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__BYTE_ORDER__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <../src/crn_miniz.h>
PRAGMA_WARNING_POP

#include <tuple>	// For "std::ignore"
#include <array>
#include <unordered_map>


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
		typedef std::vector<uint8_t> FileData;
		typedef std::vector<std::string> TextureFilenames;
		typedef std::unordered_map<std::string, TextureFilenames> MaterialTextureFilenames;	// Key = material name
		static const std::string TEXTURE_TYPE = "Texture";
		static const std::string MATERIAL_TYPE = "Material";
		static const std::string MESH_TYPE = "Mesh";

		// Sketchfab supported mesh formats: https://help.sketchfab.com/hc/en-us/articles/202508396-3D-File-Formats
		// -> List is from October 27'th, 2017
		typedef std::array<const char*, 52> SketchfabMeshFormats;
		SketchfabMeshFormats g_SketchfabMeshFormats = {
			".3dc", ".asc",															// 3DC point cloud
			".3ds",																	// 3DS
			".ac",																	// ac3d
			".abc",																	// Alembic
			".obj",																	// Alias Wavefront
			".bvh",																	// Biovision Hierarchy
			".blend",																// Blender
			".geo",																	// Carbon Graphics Inc
			".dae", ".zae",															// Collada
			".dwf",																	// Design Web Format
			".dw",																	// Designer Workbench
			".x",																	// DirectX
			".dxf",																	// Drawing eXchange Format
			".fbx",																	// Autodesk Filmbox, FBX
			".ogr",																	// GDAL vector format
			".gta",																	// Generic Tagged Arrays
			".gltf", ".glb",														// GL Transmission Format
			".igs", ".iges",														// Initial Graphics Exchange Specification, IGES
			".mu", ".craft",														// Kerbal Space Program
			".kmz",																	// Google Earth, Keyhole Markup Language
			".las",																	// LIDAR point clouds
			".lwo", ".lws",															// Lightwave
			".q3d",																	// Mimesys Q3D
			".mc2obj", ".dat",														// Minecraft
			".flt",																	// Open Flight
			".iv",																	// Open Inventor
			".osg", ".osgt", ".osgb", ".osgterrain", ".osgtgz", ".osgx", ".ive",	// OpenSceneGraph
			".ply",																	// Polygon File Format
			".bsp",																	// Quake
			".md2", ".mdl", 														// Quake / Valve source engine
			".shp",																	// Shape
			".stl", ".sta",															// Stereolithography, Standard Tessellation Language
			".txp",																	// Terrapage format database
			".vpk",																	// Valve source engine
			".wrl", ".vrml", ".wrz"													// Virtual Reality Modeling Language, VRML
		};

		// TODO(co) Add more supported mesh formats, but only add tested mesh formats so we know it's working in general
		typedef std::array<const char*, 1> SupportedMeshFormats;
		SupportedMeshFormats g_SupportedMeshFormats = {
			".obj"	// Alias Wavefront
		};

		/*
		Sketchfab texture naming conventions: https://help.sketchfab.com/hc/en-us/articles/202600873-Materials-and-Textures#textures-auto-pbr
		"
		Automatic PBR Mapping

		Use our texture naming conventions to help us automatically choose settings and apply textures to the right shader slots. The format is "MaterialName_suffix.extension". For example, if you have a material named "Material1", you could name your textures like "Material1_diffuse.png", "Material1_metallic.png", etc.

		Avoid names with special characters, especially periods '.', underscores '_', and hyphens '-' because it can break the match.

		These are the strings we look for in the suffix:

		- Diffuse / Albedo / Base Color: 'diffuse', 'albedo', 'basecolor'
		- Metalness: 'metalness', 'metallic', 'metal', 'm'
		- Specular: 'specular', 'spec', 's'
		- Specular F0: 'specularf0', 'f0'
		- Roughness: 'roughness', 'rough', 'r'
		- Glossiness: 'glossiness', 'glossness', 'gloss', 'g', 'glossy'
		- AO: 'ambient occlusion', 'ao', 'occlusion', 'lightmap', 'diffuseintensity'
		- Cavity: 'cavity'
		- Normal Map: '''normal', 'nrm', 'normalmap'
		- Bump Map: 'bump', 'bumpmap', 'heightmap'
		- Emission: 'emission', 'emit', 'emissive'
		- Transparency: 'transparency', 'transparent', 'opacity', 'mask', 'alpha'
		"
		- Found also undocumented semantics in downloaded Sketchfab files:
			- "", "d", "diff" = Diffuse map
			- "n" = Normal map
			- Case variations, of course
		- PBR on Sketchfab: https://help.sketchfab.com/hc/en-us/articles/204429595-Materials-PBR-
		*/
		enum SemanticType
		{
			DIFFUSE_MAP,
			NORMAL_MAP,
			HEIGHT_MAP,
			ROUGHNESS_MAP,
			METALLIC_MAP,
			EMISSIVE_MAP,
			NUMBER_OF_SEMANTICS
		};
		typedef std::vector<const char*> SemanticStrings;
		typedef std::array<SemanticStrings, SemanticType::NUMBER_OF_SEMANTICS> Semantics;
		const Semantics g_Semantics = {{
			// DIFFUSE_MAP
			{ "diffuse", "albedo", "basecolor", "", "d", "diff" },
			// NORMAL_MAP
			{ "normal", "nrm", "normalmap", "n" },
			// HEIGHT_MAP
			{ "bump", "bumpmap", "heightmap" },
			// ROUGHNESS_MAP
			{ "roughness", "rough", "r",
			  "specular", "spec", "s"		// Specular = roughness
			},
			// METALLIC_MAP
			{ "metalness", "metallic", "metal", "m" },
			// EMISSIVE_MAP
			{ "emission", "emit", "emissive" }
		}};


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		void readFileIntoMemory(const RendererToolkit::IAssetImporter::Input& input, FileData& fileData)
		{
			RendererRuntime::IFileManager& fileManager = input.context.getFileManager();
			RendererRuntime::IFile* file = fileManager.openFile(RendererRuntime::IFileManager::FileMode::READ, input.absoluteSourceFilename.c_str());
			if (nullptr != file)
			{
				// Load the whole file content
				const size_t numberOfBytes = file->getNumberOfBytes();
				fileData.resize(numberOfBytes);
				file->read(const_cast<void*>(static_cast<const void*>(fileData.data())), numberOfBytes);	// TODO(co) Get rid of the evil const-cast

				// Close file
				fileManager.closeFile(*file);
			}
			else
			{
				// Error!
				throw std::runtime_error("Failed to open the ZIP-archive \"" + input.absoluteSourceFilename + "\" for reading");
			}
		}

		void importTexture(const RendererToolkit::IAssetImporter::Input& input, mz_zip_archive& zipArchive, mz_uint fileIndex, const char* filename, TextureFilenames& textureFilenames)
		{
			// Ensure the texture directory exists
			RendererRuntime::IFileManager& fileManager = input.context.getFileManager();
			const std::string virtualTextureDirectoryName = input.virtualAssetOutputDirectory + '/' + TEXTURE_TYPE;
			fileManager.createDirectories(virtualTextureDirectoryName.c_str());

			// Write down the uncompressed texture file
			// -> Silently ignore and overwrite already existing files (might be a re-import)
			const std::string textureFilename = std_filesystem::path(filename).filename().generic_string();
			const std::string virtualTextureFilename = virtualTextureDirectoryName + '/' + textureFilename;
			RendererRuntime::IFile* file = fileManager.openFile(RendererRuntime::IFileManager::FileMode::WRITE, virtualTextureFilename.c_str());
			if (nullptr != file)
			{
				// Write down the decompress texture file
				size_t uncompressedFileSize = 0;
				void* fileData = mz_zip_reader_extract_to_heap(&zipArchive, fileIndex, &uncompressedFileSize, 0);
				if (0 == uncompressedFileSize || nullptr == fileData)
				{
					// Error!
					throw std::runtime_error("Failed to extract the texture file \"" + std::string(filename) + "\" from ZIP-archive \"" + input.absoluteSourceFilename + '\"');
				}
				file->write(fileData, uncompressedFileSize);
				mz_free(fileData);

				// Close file
				fileManager.closeFile(*file);

				// Remember the texture filename for creating the texture asset files later on
				if (std::find(textureFilenames.cbegin(), textureFilenames.cend(), textureFilename) != textureFilenames.cend())
				{
					// Error!
					throw std::runtime_error("The ZIP-archive \"" + input.absoluteSourceFilename + "\" contains multiple texture files named \"" + textureFilename + '\"');
				}
				textureFilenames.push_back(textureFilename);
			}
			else
			{
				// Error!
				throw std::runtime_error("Failed to open the file \"" + virtualTextureFilename + "\" for writing");
			}
		}

		void gatherMaterialTextureFilenames(const RendererToolkit::IAssetImporter::Input& input, const TextureFilenames& textureFilenames, MaterialTextureFilenames& materialTextureFilenames)
		{
			// Sanity check
			assert(!textureFilenames.empty());

			// Let's first see which materials and types of texture maps we have
			for (const std::string& textureFilename : textureFilenames)
			{
				// Get the texture filename stem: For example, the stem of "Spino_Head_N.tga.png" is "Spino_Head_N.tga"
				const std::string stem = std_filesystem::path(textureFilename).stem().generic_string();

				// Get the material name as well as the texture semantic
				// -> For example the material name of "Spino_Head_N.tga" is "Spino_Head"
				// -> For example the semantic of "Spino_Head_N.tga" is "N" which means normal map
				std::string materialName;
				std::string semanticAsString;
				const size_t lastSlashIndex = stem.find_last_of('_');
				if (std::string::npos != lastSlashIndex)
				{
					materialName = stem.substr(0, lastSlashIndex);
					const size_t dotIndex = stem.substr(lastSlashIndex + 1).find_first_of('.');
					semanticAsString = (std::string::npos != dotIndex) ? stem.substr(lastSlashIndex + 1, dotIndex) : stem.substr(lastSlashIndex + 1);
					std::transform(semanticAsString.begin(), semanticAsString.end(), semanticAsString.begin(), ::tolower);
				}
				else
				{
					materialName = stem;
				}

				// Get per-material texture filename by semantics mapping
				TextureFilenames* textureFilenameBySemantics = nullptr;
				{
					MaterialTextureFilenames::iterator iterator = materialTextureFilenames.find(materialName);
					if (iterator == materialTextureFilenames.cend())
					{
						// New material discovered
						textureFilenameBySemantics = &materialTextureFilenames.emplace(materialName, TextureFilenames()).first->second;
						textureFilenameBySemantics->resize(SemanticType::NUMBER_OF_SEMANTICS);
					}
					else
					{
						// Material is already known
						textureFilenameBySemantics = &iterator->second;
					}
				}

				// Evaluate the texture semantic
				for (size_t semanticIndex = 0; semanticIndex < SemanticType::NUMBER_OF_SEMANTICS; ++semanticIndex)
				{
					const SemanticStrings& semanticStrings = g_Semantics[semanticIndex];
					if (std::find(semanticStrings.cbegin(), semanticStrings.cend(), semanticAsString) != semanticStrings.cend())
					{
						std::string& textureFilenameBySemantic = textureFilenameBySemantics->at(semanticIndex);
						if (!textureFilenameBySemantic.empty())
						{
							// Error!
							throw std::runtime_error("The ZIP-archive \"" + input.absoluteSourceFilename + "\" contains multiple texture files like \"" + textureFilename + "\" with the same semantic for material \"" + materialName + '\"');
						}
						textureFilenameBySemantic = textureFilename;
						break;
					}
				}
			}
		}

		void createTextureChannelPackingAssetFile(const RendererToolkit::IAssetImporter::Input& input, const std::string& materialName, const TextureFilenames& textureFilenames, const std::string& semantic)
		{
			/* Example for a resulting texture asset JSON file
			{
				"Format": {
					"Type": "Asset",
					"Version": "1"
				},
				"Asset": {
					"AssetMetadata": {
						"AssetType": "Texture",
						"AssetCategory": "Imported"
					},
					"TextureAssetCompiler": {
						"TextureSemantic": "PACKED_CHANNELS",
						"TextureChannelPacking": "_drgb_nxa",
						"InputFiles": {
							"DIFFUSE_MAP": "Spino_Body_D.tga.png",
							"NORMAL_MAP": "Spino_Body_N.tga.png"
						}
					}
				}
			}
			*/
			rapidjson::Document rapidJsonDocumentAsset(rapidjson::kObjectType);
			rapidjson::Document::AllocatorType& rapidJsonAllocatorType = rapidJsonDocumentAsset.GetAllocator();
			rapidjson::Value rapidJsonValueAsset(rapidjson::kObjectType);

			{ // Asset metadata
				rapidjson::Value rapidJsonValueAssetMetadata(rapidjson::kObjectType);
				rapidJsonValueAssetMetadata.AddMember("AssetType", "Texture", rapidJsonAllocatorType);
				rapidJsonValueAssetMetadata.AddMember("AssetCategory", rapidjson::StringRef(input.assetCategory.c_str()), rapidJsonAllocatorType);
				rapidJsonValueAsset.AddMember("AssetMetadata", rapidJsonValueAssetMetadata, rapidJsonAllocatorType);
			}

			{ // Texture asset compiler
				rapidjson::Value rapidJsonValueTextureAssetCompiler(rapidjson::kObjectType);

				// Semantic dependent handling
				if ("_drgb_nxa" == semantic || "_hr_rg_mb_nya" == semantic)
				{
					// Texture channel packing
					rapidJsonValueTextureAssetCompiler.AddMember("TextureSemantic", "PACKED_CHANNELS", rapidJsonAllocatorType);
					rapidJsonValueTextureAssetCompiler.AddMember("TextureChannelPacking", rapidjson::StringRef(semantic.c_str()), rapidJsonAllocatorType);

					// Define helper macro
					#define ADD_MEMBER(semanticType) \
						if (!textureFilenames[SemanticType::semanticType].empty()) \
						{ \
							rapidJsonValueInputFiles.AddMember(#semanticType, rapidjson::StringRef(textureFilenames[SemanticType::semanticType].c_str()), rapidJsonAllocatorType); \
						}

					{ // Input files
						rapidjson::Value rapidJsonValueInputFiles(rapidjson::kObjectType);
						if ("_drgb_nxa" == semantic)
						{
							ADD_MEMBER(DIFFUSE_MAP)
							ADD_MEMBER(NORMAL_MAP)
						}
						else if ("_hr_rg_mb_nya" == semantic)
						{
							ADD_MEMBER(HEIGHT_MAP)
							ADD_MEMBER(ROUGHNESS_MAP)
							ADD_MEMBER(METALLIC_MAP)
							ADD_MEMBER(NORMAL_MAP)
						}
						else
						{
							// Error!
							assert(false && "Broken implementation, we should never ever be in here");
						}
						rapidJsonValueTextureAssetCompiler.AddMember("InputFiles", rapidJsonValueInputFiles, rapidJsonAllocatorType);
					}

					// Undefine helper macro
					#undef ADD_MEMBER
				}
				else if ("_e" == semantic)
				{
					// No texture channel packing
					rapidJsonValueTextureAssetCompiler.AddMember("TextureSemantic", "EMISSIVE_MAP", rapidJsonAllocatorType);
					rapidJsonValueTextureAssetCompiler.AddMember("InputFile", rapidjson::StringRef(textureFilenames[SemanticType::EMISSIVE_MAP].c_str()), rapidJsonAllocatorType);
				}
				else
				{
					// Error!
					assert(false && "Broken implementation, we should never ever be in here");
				}

				// Add texture asset compiler member
				rapidJsonValueAsset.AddMember("TextureAssetCompiler", rapidJsonValueTextureAssetCompiler, rapidJsonAllocatorType);
			}

			// Write down the texture asset JSON file
			// -> Silently ignore and overwrite already existing files (might be a re-import)
			const std::string virtualFilename = input.virtualAssetOutputDirectory + '/' + TEXTURE_TYPE + '/' + materialName + semantic + ".asset";
			RendererToolkit::JsonHelper::saveDocumentByFilename(input.context.getFileManager(), virtualFilename, "Asset", "1", rapidJsonValueAsset);
		}

		void createTextureChannelPackingAssetFiles(const RendererToolkit::IAssetImporter::Input& input, const MaterialTextureFilenames& materialTextureFilenames)
		{
			// Iterate through the materials
			for (const auto& pair : materialTextureFilenames)
			{
				const std::string& materialName = pair.first;
				const TextureFilenames& textureFilenames = pair.second;

				// Texture channel packing "_drgb_nxa"
				if (!textureFilenames[SemanticType::DIFFUSE_MAP].empty() || !textureFilenames[SemanticType::NORMAL_MAP].empty())
				{
					createTextureChannelPackingAssetFile(input, materialName, textureFilenames, "_drgb_nxa");
				}

				// Texture channel packing "_hr_rg_mb_nya"
				if (!textureFilenames[SemanticType::HEIGHT_MAP].empty() || !textureFilenames[SemanticType::ROUGHNESS_MAP].empty() ||
					!textureFilenames[SemanticType::METALLIC_MAP].empty() || !textureFilenames[SemanticType::NORMAL_MAP].empty())
				{
					createTextureChannelPackingAssetFile(input, materialName, textureFilenames, "_hr_rg_mb_nya");
				}

				// Emissive map "_e"
				if (!textureFilenames[SemanticType::EMISSIVE_MAP].empty())
				{
					createTextureChannelPackingAssetFile(input, materialName, textureFilenames, "_e");
				}
			}
		}

		void createMaterialFile(const RendererToolkit::IAssetImporter::Input& input, const std::string& materialName, const TextureFilenames& textureFilenames)
		{
			/* Example for a resulting material JSON file
			{
				"Format": {
					"Type": "MaterialAsset",
					"Version": "1"
				},
				"MaterialAsset": {
					"BaseMaterial": "$ProjectName/Material/Base/Mesh.asset",
					"Properties": {
						"_drgb_nxa": "../Texture/Spino_Body_drgb_nxa.asset",
						"_hr_rg_mb_nya": "../Texture/Spino_Body_hr_rg_mb_nya.asset"
					}
				}
			}
			*/
			rapidjson::Document rapidJsonDocumentAsset(rapidjson::kObjectType);
			rapidjson::Document::AllocatorType& rapidJsonAllocatorType = rapidJsonDocumentAsset.GetAllocator();
			rapidjson::Value rapidJsonValueMaterialAsset(rapidjson::kObjectType);
			const std::string relativeFilename_drgb_nxa = "../" + TEXTURE_TYPE + '/' + materialName + "_drgb_nxa" + ".asset";
			const std::string relativeFilename_hr_rg_mb_nya = "../" + TEXTURE_TYPE + '/' + materialName + "_hr_rg_mb_nya" + ".asset";
			const std::string relativeFilenameEmissiveMap = "../" + TEXTURE_TYPE + '/' + materialName + "_e" + ".asset";

			// Base material
			// TODO(co) Use "$ProjectName/Material/Base/SkinnedMesh.asset" if there's a skeleton
			rapidJsonValueMaterialAsset.AddMember("BaseMaterial", "$ProjectName/Material/Base/Mesh.asset", rapidJsonAllocatorType);

			{ // Properties
				rapidjson::Value rapidJsonValueProperties(rapidjson::kObjectType);

				// Texture channel packing "_drgb_nxa"
				if (!textureFilenames[SemanticType::DIFFUSE_MAP].empty() || !textureFilenames[SemanticType::NORMAL_MAP].empty())
				{
					rapidJsonValueProperties.AddMember("_drgb_nxa", rapidjson::StringRef(relativeFilename_drgb_nxa.c_str()), rapidJsonAllocatorType);
				}

				// Texture channel packing "_hr_rg_mb_nya"
				if (!textureFilenames[SemanticType::HEIGHT_MAP].empty() || !textureFilenames[SemanticType::ROUGHNESS_MAP].empty() ||
					!textureFilenames[SemanticType::METALLIC_MAP].empty() || !textureFilenames[SemanticType::NORMAL_MAP].empty())
				{
					rapidJsonValueProperties.AddMember("_hr_rg_mb_nya", rapidjson::StringRef(relativeFilename_hr_rg_mb_nya.c_str()), rapidJsonAllocatorType);
				}

				// Emissive map "_e"
				if (!textureFilenames[SemanticType::EMISSIVE_MAP].empty())
				{
					rapidJsonValueProperties.AddMember("UseEmissiveMap", "TRUE", rapidJsonAllocatorType);
					rapidJsonValueProperties.AddMember("EmissiveMap", rapidjson::StringRef(relativeFilenameEmissiveMap.c_str()), rapidJsonAllocatorType);
				}

				// Add properties member
				rapidJsonValueMaterialAsset.AddMember("Properties", rapidJsonValueProperties, rapidJsonAllocatorType);
			}

			// Write down the material JSON file
			// -> Silently ignore and overwrite already existing files (might be a re-import)
			const std::string virtualFilename = input.virtualAssetOutputDirectory + '/' + MATERIAL_TYPE + '/' + materialName + ".material";
			RendererToolkit::JsonHelper::saveDocumentByFilename(input.context.getFileManager(), virtualFilename, "MaterialAsset", "1", rapidJsonValueMaterialAsset);
		}

		void createMaterialAssetFile(const RendererToolkit::IAssetImporter::Input& input, const std::string& materialName)
		{
			/* Example for a resulting material asset JSON file
			{
				"Format": {
					"Type": "Asset",
					"Version": "1"
				},
				"Asset": {
					"AssetMetadata": {
						"AssetType": "Material",
						"AssetCategory": "Imported"
					},
					"MaterialAssetCompiler": {
						"InputFile": "SpinosaurusBody.material"
					}
				}
			}
			*/
			rapidjson::Document rapidJsonDocumentAsset(rapidjson::kObjectType);
			rapidjson::Document::AllocatorType& rapidJsonAllocatorType = rapidJsonDocumentAsset.GetAllocator();
			rapidjson::Value rapidJsonValueAsset(rapidjson::kObjectType);
			const std::string filename = materialName + ".material";

			{ // Asset metadata
				rapidjson::Value rapidJsonValueAssetMetadata(rapidjson::kObjectType);
				rapidJsonValueAssetMetadata.AddMember("AssetType", "Material", rapidJsonAllocatorType);
				rapidJsonValueAssetMetadata.AddMember("AssetCategory", rapidjson::StringRef(input.assetCategory.c_str()), rapidJsonAllocatorType);
				rapidJsonValueAsset.AddMember("AssetMetadata", rapidJsonValueAssetMetadata, rapidJsonAllocatorType);
			}

			{ // Material compiler
				rapidjson::Value rapidJsonValueMaterialAssetCompiler(rapidjson::kObjectType);
				rapidJsonValueMaterialAssetCompiler.AddMember("InputFile", rapidjson::StringRef(filename.c_str()), rapidJsonAllocatorType);
				rapidJsonValueAsset.AddMember("MaterialAssetCompiler", rapidJsonValueMaterialAssetCompiler, rapidJsonAllocatorType);
			}

			// Write down the material asset JSON file
			// -> Silently ignore and overwrite already existing files (might be a re-import)
			const std::string virtualFilename = input.virtualAssetOutputDirectory + '/' + MATERIAL_TYPE + '/' + materialName + ".asset";
			RendererToolkit::JsonHelper::saveDocumentByFilename(input.context.getFileManager(), virtualFilename, "Asset", "1", rapidJsonValueAsset);
		}

		void createMaterialAssetFiles(const RendererToolkit::IAssetImporter::Input& input, const MaterialTextureFilenames& materialTextureFilenames)
		{
			// Ensure the material directory exists
			input.context.getFileManager().createDirectories((input.virtualAssetOutputDirectory + '/' + MATERIAL_TYPE).c_str());

			// Iterate through the materials
			for (const auto& pair : materialTextureFilenames)
			{
				const std::string& materialName = pair.first;
				const TextureFilenames& textureFilenames = pair.second;
				createMaterialFile(input, materialName, textureFilenames);
				createMaterialAssetFile(input, materialName);
			}
		}

		/*
		Sketchfab merging conventions: https://help.sketchfab.com/hc/en-us/articles/201766675-Viewer-Performance
		"
		Materials
		- Identical materials are merged together.

		Geometries
		- Meshes that share the same material are merged together.
		- Geometries are not merged for animated objects or objects with transparency!
		"
		*/
		void importMesh(const RendererToolkit::IAssetImporter::Input& input, mz_zip_archive& zipArchive, mz_uint fileIndex, const char* filename)
		{
			// Ensure the mesh directory exists
			RendererRuntime::IFileManager& fileManager = input.context.getFileManager();
			fileManager.createDirectories((input.virtualAssetOutputDirectory + '/' + MESH_TYPE).c_str());

			// TODO(co) Implement me
			std::ignore = zipArchive;
			std::ignore = fileIndex;
			std::ignore = filename;
			NOP;
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
	const AssetImporterTypeId SketchfabAssetImporter::TYPE_ID("Sketchfab");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SketchfabAssetImporter::SketchfabAssetImporter()
	{
		// Nothing here
	}

	SketchfabAssetImporter::~SketchfabAssetImporter()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetImporter methods ]
	//[-------------------------------------------------------]
	AssetImporterTypeId SketchfabAssetImporter::getAssetImporterTypeId() const
	{
		return TYPE_ID;
	}

	void SketchfabAssetImporter::import(const Input& input)
	{
		// Read the ZIP-archive file into memory
		::detail::FileData fileData;
		::detail::readFileIntoMemory(input, fileData);

		// Initialize the ZIP-archive
		mz_zip_archive zipArchive = {};
		if (!mz_zip_reader_init_mem(&zipArchive, static_cast<const void*>(fileData.data()), fileData.size(), MZ_ZIP_FLAG_CASE_SENSITIVE | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
		{
			// Error!
			throw std::runtime_error("Failed to initialize opened ZIP-archive \"" + input.absoluteSourceFilename + "\" for reading");
		}

		// Iterate through the ZIP-archive files
		::detail::TextureFilenames textureFilenames;
		const mz_uint numberOfFiles = mz_zip_reader_get_num_files(&zipArchive);
		for (mz_uint fileIndex = 0; fileIndex < numberOfFiles; ++fileIndex)
		{
			// Get filename and file extension
			const mz_uint FILENAME_BUFFER_SIZE = 256;
			char filename[FILENAME_BUFFER_SIZE];
			const mz_uint filenameSize = mz_zip_reader_get_filename(&zipArchive, fileIndex, filename, FILENAME_BUFFER_SIZE);
			if (0 == filenameSize)
			{
				// Error!
				throw std::runtime_error("Failed to get filename at index " + std::to_string(fileIndex) + " while reading the ZIP-archive \"" + input.absoluteSourceFilename + '\"');
			}
			const std::string extension = std_filesystem::path(filename).extension().generic_string();

			// Evaluate the file extension and proceed accordantly
			// -> Silently ignore unknown files

			// Texture: Sketchfab supported texture formats: https://help.sketchfab.com/hc/en-us/articles/202600873-Materials-and-Textures#textures-file-formats
			// -> "Anything that is not .JPG or .PNG is converted to .PNG."
			if (".jpg" == extension || ".png" == extension)
			{
				::detail::importTexture(input, zipArchive, fileIndex, filename, textureFilenames);
			}

			// Mesh
			else if (".zip" == extension)
			{
				// TODO(co) Implement me
				NOP;
			}
			else if (std::find(::detail::g_SupportedMeshFormats.cbegin(), ::detail::g_SupportedMeshFormats.cend(), extension) != ::detail::g_SupportedMeshFormats.cend())
			{
				::detail::importMesh(input, zipArchive, fileIndex, filename);
			}
			else if (std::find(::detail::g_SketchfabMeshFormats.cbegin(), ::detail::g_SketchfabMeshFormats.cend(), extension) != ::detail::g_SketchfabMeshFormats.cend())
			{
				// Error!
				throw std::runtime_error("Failed to import mesh asset \"" + std::string(filename) + "\" while reading the ZIP-archive \"" + input.absoluteSourceFilename + "\": Mesh format \"" + extension + "\" isn't supported");
			}
		}

		// End the ZIP-archive
		if (!mz_zip_reader_end(&zipArchive))
		{
			// Error!
			throw std::runtime_error("Failed to close the read ZIP-archive \"" + input.absoluteSourceFilename + '\"');
		}

		// Create texture and material asset files
		if (!textureFilenames.empty())
		{
			::detail::MaterialTextureFilenames materialTextureFilenames;
			::detail::gatherMaterialTextureFilenames(input, textureFilenames, materialTextureFilenames);
			if (materialTextureFilenames.empty())
			{
				throw std::runtime_error("Failed to gather material texture filenames for ZIP-archive \"" + input.absoluteSourceFilename + '\"');
			}
			::detail::createTextureChannelPackingAssetFiles(input, materialTextureFilenames);
			::detail::createMaterialAssetFiles(input, materialTextureFilenames);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
