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
#include "RendererToolkit/AssetCompiler/CompositorWorkspaceAssetCompiler.h"
#include "RendererToolkit/Helper/FileSystemHelper.h"
#include "RendererToolkit/Helper/CacheManager.h"
#include "RendererToolkit/Helper/StringHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"
#include "RendererToolkit/Context.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Core/File/MemoryFile.h>
#include <RendererRuntime/Resource/CompositorWorkspace/Loader/CompositorWorkspaceFileFormat.h>
#include <RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceResource.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'std::codecvt_base': assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4774)	// warning C4774: 'sprintf_s' : format string expected in argument 3 is not a string literal
	PRAGMA_WARNING_DISABLE_MSVC(5026)	// warning C5026: 'std::_Generic_error_category': move constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(5027)	// warning C5027: 'std::_Generic_error_category': move assignment operator was implicitly defined as deleted
	#include <rapidjson/document.h>
PRAGMA_WARNING_POP

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'initializing': conversion from 'int' to '::size_t', signed/unsigned mismatch
	#include <fstream>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const AssetCompilerTypeId CompositorWorkspaceAssetCompiler::TYPE_ID("CompositorWorkspace");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	CompositorWorkspaceAssetCompiler::CompositorWorkspaceAssetCompiler()
	{
		// Nothing here
	}

	CompositorWorkspaceAssetCompiler::~CompositorWorkspaceAssetCompiler()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId CompositorWorkspaceAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	bool CompositorWorkspaceAssetCompiler::checkIfChanged(const Input& input, const Configuration& configuration) const
	{
		// Get the JSON asset object
		const rapidjson::Value& rapidJsonValueAsset = configuration.rapidJsonDocumentAsset["Asset"];

		// Read configuration
		std::string inputFile;
		{
			// Read material asset compiler configuration
			const rapidjson::Value& rapidJsonValueMaterialAssetCompiler = rapidJsonValueAsset["CompositorWorkspaceAssetCompiler"];
			inputFile = rapidJsonValueMaterialAssetCompiler["InputFile"].GetString();
		}

		// Let the cache manager check whether or not the files have been changed in order to speed up later checks and to support dependency tracking
		const std::string inputFilename = input.assetInputDirectory + inputFile;
		return input.cacheManager.checkIfFileIsModified(configuration.rendererTarget, input.assetFilename, {inputFilename}, RendererRuntime::v1CompositorWorkspace::FORMAT_VERSION);
	}

	void CompositorWorkspaceAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Input, configuration and output
		const std::string&			   assetInputDirectory	= input.assetInputDirectory;
		const std::string&			   assetOutputDirectory = input.assetOutputDirectory;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		const rapidjson::Value& rapidJsonValueAsset = configuration.rapidJsonDocumentAsset["Asset"];

		// Read configuration
		std::string inputFile;
		{
			// Read compositor asset compiler configuration
			const rapidjson::Value& rapidJsonValueCompositorWorkspaceAssetCompiler = rapidJsonValueAsset["CompositorWorkspaceAssetCompiler"];
			inputFile = rapidJsonValueCompositorWorkspaceAssetCompiler["InputFile"].GetString();
		}

		// Open the input file
		const std::string inputFilename = assetInputDirectory + inputFile;
		const std::string assetName = std_filesystem::path(input.assetFilename).stem().generic_string();
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".compositor_workspace";

		// Ask the cache manager whether or not we need to compile the source file (e.g. source changed or target not there)
		CacheManager::CacheEntries cacheEntries;
		if (input.cacheManager.needsToBeCompiled(configuration.rendererTarget, input.assetFilename, inputFilename, outputAssetFilename, RendererRuntime::v1CompositorWorkspace::FORMAT_VERSION, cacheEntries))
		{
			std::ifstream inputFileStream(inputFilename, std::ios::binary);
			RendererRuntime::MemoryFile memoryFile(0, 4096);

			{ // Compositor workspace
				// Parse JSON
				rapidjson::Document rapidJsonDocument;
				JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, inputFilename, "CompositorWorkspaceAsset", "1");

				{ // Write down the compositor workspace resource header
					RendererRuntime::v1CompositorWorkspace::CompositorWorkspaceHeader compositorWorkspaceHeader;
					compositorWorkspaceHeader.unused = 42;	// TODO(co) Currently the compositor workspace header is unused
					memoryFile.write(&compositorWorkspaceHeader, sizeof(RendererRuntime::v1CompositorWorkspace::CompositorWorkspaceHeader));
				}

				// Mandatory main sections of the compositor workspace
				const rapidjson::Value& rapidJsonValueCompositorWorkspaceAsset = rapidJsonDocument["CompositorWorkspaceAsset"];
				const rapidjson::Value& rapidJsonValueNodes = rapidJsonValueCompositorWorkspaceAsset["Nodes"];

				{ // Write down the compositor resource nodes
					RendererRuntime::v1CompositorWorkspace::Nodes nodes;
					nodes.numberOfNodes = rapidJsonValueNodes.MemberCount();
					memoryFile.write(&nodes, sizeof(RendererRuntime::v1CompositorWorkspace::Nodes));

					// Loop through all compositor workspace resource nodes
					RendererRuntime::CompositorWorkspaceResource::CompositorNodeAssetIds compositorNodeAssetIds;
					compositorNodeAssetIds.reserve(nodes.numberOfNodes);
					for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorNodes = rapidJsonValueNodes.MemberBegin(); rapidJsonMemberIteratorNodes != rapidJsonValueNodes.MemberEnd(); ++rapidJsonMemberIteratorNodes)
					{
						compositorNodeAssetIds.push_back(StringHelper::getAssetIdByString(rapidJsonMemberIteratorNodes->name.GetString(), input));
					}

					// Write down compositor node asset IDs
					memoryFile.write(compositorNodeAssetIds.data(), sizeof(RendererRuntime::AssetId) * nodes.numberOfNodes);
				}
			}

			// Write LZ4 compressed output
			memoryFile.writeLz4CompressedDataToFile(RendererRuntime::v1CompositorWorkspace::FORMAT_TYPE, RendererRuntime::v1CompositorWorkspace::FORMAT_VERSION, outputAssetFilename, input.context.getFileManager());

			// Store new cache entries or update existing ones
			input.cacheManager.storeOrUpdateCacheEntries(cacheEntries);
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/CompositorWorkspace/" + assetCategory + '/' + assetName;
			outputAsset(input.context.getFileManager(), assetIdAsString, outputAssetFilename, outputAssetPackage);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
