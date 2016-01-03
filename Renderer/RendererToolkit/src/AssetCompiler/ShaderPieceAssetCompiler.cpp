/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "RendererToolkit/AssetCompiler/ShaderPieceAssetCompiler.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Resource/ShaderPiece/Loader/ShaderPieceFileFormat.h>

#include <memory>
#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const AssetCompilerTypeId ShaderPieceAssetCompiler::TYPE_ID("ShaderPiece");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderPieceAssetCompiler::ShaderPieceAssetCompiler()
	{
	}

	ShaderPieceAssetCompiler::~ShaderPieceAssetCompiler()
	{
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId ShaderPieceAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	void ShaderPieceAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
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
			// Read shader piece asset compiler configuration
			Poco::JSON::Object::Ptr jsonConfigurationObject = jsonAssetObject->get("ShaderPieceAssetCompiler").extract<Poco::JSON::Object::Ptr>();
			inputFile = jsonConfigurationObject->getValue<std::string>("InputFile");
			test	  = jsonConfigurationObject->optValue<uint32_t>("Test", test);
		}

		// Open the input file
		std::ifstream inputFileStream(assetInputDirectory + inputFile, std::ios::binary);
		const std::string assetName = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetName");
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".shader_piece";
		std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

		{ // Shader piece
			// TODO(co) At the moment, we just copy over the ASCII shader source code. Later on, we might want to perform optimizations like
			// stripping away all comments and unnecessary white spaces.

			// Get file size and file data
			std::string sourceCode;
			inputFileStream.seekg(0, std::ifstream::end);
			const std::streampos numberOfBytes = inputFileStream.tellg();
			inputFileStream.seekg(0, std::ifstream::beg);
			sourceCode.resize(static_cast<size_t>(numberOfBytes));
			inputFileStream.read(const_cast<char*>(sourceCode.c_str()), numberOfBytes);

			{ // Shader piece header
				RendererRuntime::v1ShaderPiece::Header shaderPieceHeader;
				shaderPieceHeader.formatType					= RendererRuntime::v1ShaderPiece::FORMAT_TYPE;
				shaderPieceHeader.formatVersion					= RendererRuntime::v1ShaderPiece::FORMAT_VERSION;
				shaderPieceHeader.numberOfShaderSourceCodeBytes	= static_cast<uint32_t>(numberOfBytes);

				// Write down the shader piece header
				outputFileStream.write(reinterpret_cast<const char*>(&shaderPieceHeader), sizeof(RendererRuntime::v1ShaderPiece::Header));
			}

			// Dump the unchanged content into the output file stream
			outputFileStream.write(sourceCode.c_str(), numberOfBytes);
		}

		{ // Update the output asset package
			const std::string assetCategory = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetCategory");
			const std::string assetIdAsString = input.projectName + "/ShaderPiece/" + assetCategory + '/' + assetName;

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
