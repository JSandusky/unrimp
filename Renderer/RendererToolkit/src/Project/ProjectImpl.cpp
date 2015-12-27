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
#include "RendererToolkit/Project/ProjectImpl.h"
#include "RendererToolkit/Project/ProjectAssetMonitor.h"
#include "RendererToolkit/AssetCompiler/FontAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/MeshAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/SceneAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/TextureAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/MaterialAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/SkeletonAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/CompositorAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/ShaderBlueprintAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/MaterialBlueprintAssetCompiler.h"

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4127)	// warning C4127: conditional expression is constant
	#pragma warning(disable: 4244)	// warning C4244: 'argument': conversion from '<x>' to '<y>', possible loss of data
	#pragma warning(disable: 4251)	// warning C4251: '<x>': class '<y>' needs to have dll-interface to be used by clients of class '<x>'
	#pragma warning(disable: 4266)	// warning C4266: '<x>': no override available for virtual member function from base '<y>'; function is hidden
	#pragma warning(disable: 4365)	// warning C4365: 'return': conversion from '<x>' to '<y>', signed/unsigned mismatch
	#pragma warning(disable: 4548)	// warning C4548: expression before comma has no effect; expected expression with side-effect
	#pragma warning(disable: 4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	#pragma warning(disable: 4619)	// warning C4619: #pragma warning: there is no warning number '<x>'
	#pragma warning(disable: 4668)	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#define POCO_NO_UNWINDOWS
	#include <Poco/Path.h>
	#include <Poco/File.h>
#pragma warning(pop)

#include <fstream>
#include <cassert>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	namespace detail
	{
		bool orderByAssetId(const RendererRuntime::Asset& left, const RendererRuntime::Asset& right)
		{
			return (left.assetId < right.assetId);
		}
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProjectImpl::ProjectImpl() :
		mProjectAssetMonitor(nullptr),
		mShutdownThread(false)
	{
		mThread = std::thread(&ProjectImpl::threadWorker, this);
	}

	ProjectImpl::~ProjectImpl()
	{
		// Shutdown worker thread
		mShutdownThread = true;
		mThread.join();

		// Clear
		clear();
	}

	const char* ProjectImpl::getAssetFilenameByAssetId(RendererRuntime::AssetId assetId) const
	{
		return mAssetPackage.getAssetFilenameByAssetId(assetId);
	}

	void ProjectImpl::compileAsset(const RendererRuntime::Asset& asset, const char* rendererTarget, RendererRuntime::AssetPackage& outputAssetPackage)
	{
		// Open the input stream
		const std::string absoluteAssetFilename = mProjectDirectory + asset.assetFilename;
		std::ifstream inputFileStream(absoluteAssetFilename, std::ios::binary);

		// Parse JSON
		Poco::JSON::Parser jsonParser;
		jsonParser.parse(inputFileStream);
		Poco::JSON::Object::Ptr jsonAssetRootObject = jsonParser.result().extract<Poco::JSON::Object::Ptr>();

		{ // Check whether or not the file format matches
			Poco::JSON::Object::Ptr jsonFormatObject = jsonAssetRootObject->get("Format").extract<Poco::JSON::Object::Ptr>();
			if (jsonFormatObject->get("Type").convert<std::string>() != "Asset")
			{
				throw std::exception("Invalid JSON format type, must be \"Asset\"");
			}
			if (jsonFormatObject->get("Version").convert<uint32_t>() != 1)
			{
				throw std::exception("Invalid JSON format version, must be 1");
			}
		}

		// Read asset metadata
		Poco::JSON::Object::Ptr jsonAssetObject = jsonAssetRootObject->get("Asset").extract<Poco::JSON::Object::Ptr>();
		Poco::JSON::Object::Ptr jsonAssetMetadataObject = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>();

		// Check asset ID match: A sanity check in here doesn't hurt
		const RendererRuntime::AssetId assetId = jsonAssetMetadataObject->getValue<uint32_t>("AssetId");
		if (assetId != asset.assetId)
		{
			const std::string message = "Failed to compile asset with filename \"" + std::string(asset.assetFilename) + "\": According to the asset package it should be asset ID " + std::to_string(asset.assetId) + " but inside the asset file it's asset ID " + std::to_string(assetId);
			throw std::exception(message.c_str());
		}

		// Dispatch asset compiler
		// TODO(co) Add multithreading support: Add compiler queue which is processed in the background, ensure compiler instances are reused

		// Get the asset input directory and asset output directory
		const std::string assetInputDirectory = Poco::Path(absoluteAssetFilename).parent().toString(Poco::Path::PATH_UNIX);
		const std::string assetType = jsonAssetMetadataObject->getValue<std::string>("AssetType");
		const std::string assetCategory = jsonAssetMetadataObject->getValue<std::string>("AssetCategory");
		const std::string assetOutputDirectory = "../" + getRenderTargetDataRootDirectory(rendererTarget) + mAssetPackageDirectoryName + assetType + '/' + assetCategory + '/';

		// Ensure that the asset output directory exists, else creating output file streams will fail
		Poco::File(assetOutputDirectory).createDirectories();

		// Asset compiler input
		IAssetCompiler::Input input(mProjectName, assetInputDirectory, assetOutputDirectory, mSourceAssetIdToCompiledAssetId, mSourceAssetIdToAbsoluteFilename);

		// Asset compiler configuration
		IAssetCompiler::Configuration configuration;
		configuration.jsonAssetRootObject = jsonAssetRootObject;
		configuration.jsonTargetsObject	  = mJsonTargetsObject;
		configuration.rendererTarget	  = rendererTarget;

		// Asset compiler output
		IAssetCompiler::Output output;
		output.outputAssetPackage = &outputAssetPackage;

		// Evaluate the asset type and continue with the processing in the asset type specific way
		// TODO(co) Currently this is fixed build in, later on me might want to have this dynamic so we can plugin additional asset compilers
		const AssetCompilerTypeId assetCompilerTypeId(assetType.c_str());
		if (FontAssetCompiler::TYPE_ID == assetCompilerTypeId)
		{
			FontAssetCompiler().compile(input, configuration, output);
		}
		else if (TextureAssetCompiler::TYPE_ID == assetCompilerTypeId)
		{
			TextureAssetCompiler().compile(input, configuration, output);
		}
		else if (ShaderBlueprintAssetCompiler::TYPE_ID == assetCompilerTypeId)
		{
			ShaderBlueprintAssetCompiler().compile(input, configuration, output);
		}
		else if (MaterialBlueprintAssetCompiler::TYPE_ID == assetCompilerTypeId)
		{
			MaterialBlueprintAssetCompiler().compile(input, configuration, output);
		}
		else if (MaterialAssetCompiler::TYPE_ID == assetCompilerTypeId)
		{
			MaterialAssetCompiler().compile(input, configuration, output);
		}
		else if (SkeletonAssetCompiler::TYPE_ID == assetCompilerTypeId)
		{
			SkeletonAssetCompiler().compile(input, configuration, output);
		}
		else if (MeshAssetCompiler::TYPE_ID == assetCompilerTypeId)
		{
			MeshAssetCompiler().compile(input, configuration, output);
		}
		else if (SceneAssetCompiler::TYPE_ID == assetCompilerTypeId)
		{
			SceneAssetCompiler().compile(input, configuration, output);
		}
		else if (CompositorAssetCompiler::TYPE_ID == assetCompilerTypeId)
		{
			CompositorAssetCompiler().compile(input, configuration, output);
		}
		else
		{
			const std::string message = "Failed to compile asset with filename \"" + std::string(asset.assetFilename) + "\" and ID " + std::to_string(asset.assetId) + ": Asset type \"" + assetType + "\" is unknown";
			throw std::exception(message.c_str());
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IProject methods      ]
	//[-------------------------------------------------------]
	void ProjectImpl::loadByFilename(const char* filename)
	{
		// Clear the previous project
		clear();

		// Open the input stream
		std::ifstream inputFileStream(filename, std::ios::binary);

		// Parse JSON
		Poco::JSON::Parser jsonParser;
		jsonParser.parse(inputFileStream);
		Poco::JSON::Object::Ptr jsonRootObject = jsonParser.result().extract<Poco::JSON::Object::Ptr>();
		
		{ // Check whether or not the file format matches
			Poco::JSON::Object::Ptr jsonFormatObject = jsonRootObject->get("Format").extract<Poco::JSON::Object::Ptr>();
			if (jsonFormatObject->get("Type").convert<std::string>() != "Project")
			{
				throw std::exception("Invalid JSON format type, must be \"Project\"");
			}
			if (jsonFormatObject->get("Version").convert<uint32_t>() != 1)
			{
				throw std::exception("Invalid JSON format version, must be 1");
			}
		}

		// Read project metadata
		Poco::JSON::Object::Ptr jsonProjectObject = jsonRootObject->get("Project").extract<Poco::JSON::Object::Ptr>();
		{
			Poco::JSON::Object::Ptr jsonProjectMetadataObject = jsonProjectObject->get("ProjectMetadata").extract<Poco::JSON::Object::Ptr>();
			mProjectName = jsonProjectMetadataObject->getValue<std::string>("Name");
		}

		{ // Read project data
			mProjectDirectory = Poco::Path(filename).parent().toString(Poco::Path::PATH_UNIX);
			readAssetsByFilename(jsonProjectObject->getValue<std::string>("AssetsFilename"));
			readTargetsByFilename(jsonProjectObject->getValue<std::string>("TargetsFilename"));
		}
	}

	void ProjectImpl::compileAllAssets(const char* rendererTarget)
	{
		RendererRuntime::AssetPackage outputAssetPackage;

		{ // Compile all assets
			const RendererRuntime::AssetPackage::SortedAssetVector& sortedAssetVector = mAssetPackage.getSortedAssetVector();
			const size_t numberOfAssets = sortedAssetVector.size();
			for (size_t i = 0; i < numberOfAssets; ++i)
			{
				compileAsset(sortedAssetVector[i], rendererTarget, outputAssetPackage);
			}
		}

		{ // Write runtime asset package
		  // TODO(co) Experimental, make more generic elsewhere
			RendererRuntime::AssetPackage::SortedAssetVector& sortedAssetVector = outputAssetPackage.getWritableSortedAssetVector();

			// Ensure the asset package is sorted
			std::sort(sortedAssetVector.begin(), sortedAssetVector.end(), detail::orderByAssetId);

			// Open the output file
			std::ofstream outputFileStream("../" + getRenderTargetDataRootDirectory(rendererTarget) + mAssetPackageDirectoryName + "AssetPackage.assets", std::ios::binary);

			// Write down the asset package header
			#pragma pack(push)
			#pragma pack(1)
				struct AssetPackageHeader
				{
					uint32_t formatType;
					uint16_t formatVersion;
					uint32_t numberOfAssets;
				};
			#pragma pack(pop)
			AssetPackageHeader assetPackageHeader;
			assetPackageHeader.formatType	  = RendererRuntime::StringId("AssetPackage");
			assetPackageHeader.formatVersion  = 1;
			assetPackageHeader.numberOfAssets = sortedAssetVector.size();
			outputFileStream.write(reinterpret_cast<const char*>(&assetPackageHeader), sizeof(AssetPackageHeader));

			// Write down the asset package content in one single burst
			outputFileStream.write(reinterpret_cast<const char*>(sortedAssetVector.data()), sizeof(RendererRuntime::Asset) * assetPackageHeader.numberOfAssets);
		}
	}

	void ProjectImpl::startupAssetMonitor(RendererRuntime::IRendererRuntime& rendererRuntime, const char* rendererTarget)
	{
		if (nullptr == mProjectAssetMonitor)
		{
			mProjectAssetMonitor = new ProjectAssetMonitor(*this, rendererRuntime, rendererTarget);
		}
	}

	void ProjectImpl::shutdownAssetMonitor()
	{
		if (nullptr != mProjectAssetMonitor)
		{
			delete mProjectAssetMonitor;
			mProjectAssetMonitor = nullptr;
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void ProjectImpl::clear()
	{
		shutdownAssetMonitor();
		mProjectName.clear();
		mProjectDirectory.clear();
		mAssetPackage.clear();
		mAssetPackageDirectoryName.clear();
		mSourceAssetIdToCompiledAssetId.clear();
		mSourceAssetIdToAbsoluteFilename.clear();
		mJsonTargetsObject = nullptr;
	}

	void ProjectImpl::readAssetsByFilename(const std::string& filename)
	{
		// Open the input stream
		std::ifstream inputFileStream(mProjectDirectory + filename, std::ios::binary);

		// Parse JSON
		Poco::JSON::Parser jsonParser;
		jsonParser.parse(inputFileStream);
		Poco::JSON::Object::Ptr jsonRootObject = jsonParser.result().extract<Poco::JSON::Object::Ptr>();
		
		{ // Check whether or not the file format matches
			Poco::JSON::Object::Ptr jsonFormatObject = jsonRootObject->get("Format").extract<Poco::JSON::Object::Ptr>();
			if (jsonFormatObject->get("Type").convert<std::string>() != "Assets")
			{
				throw std::exception("Invalid JSON format type, must be \"Assets\"");
			}
			if (jsonFormatObject->get("Version").convert<uint32_t>() != 1)
			{
				throw std::exception("Invalid JSON format version, must be 1");
			}
		}

		// Get the asset package name (includes "/" at the end)
		mAssetPackageDirectoryName = Poco::Path(filename).parent().toString(Poco::Path::PATH_UNIX);

		// Read project data
		Poco::JSON::Object::Ptr jsonProjectObject = jsonRootObject->get("Assets").extract<Poco::JSON::Object::Ptr>();
		const size_t numberOfAssets = jsonProjectObject->size();
		RendererRuntime::AssetPackage::SortedAssetVector& sortedAssetVector = mAssetPackage.getWritableSortedAssetVector();
		sortedAssetVector.resize(numberOfAssets);
		Poco::JSON::Object::ConstIterator iterator = jsonProjectObject->begin();
		Poco::JSON::Object::ConstIterator iteratorEnd = jsonProjectObject->end();
		size_t currentAssetIndex = 0;
		while (iterator != iteratorEnd)
		{
			// Get asset data
			const RendererRuntime::AssetId assetId = static_cast<uint32_t>(std::stoi(iterator->first));	// TODO(co) Parsing directly to "uint32_t" from string to be on the safe-side?
			const std::string assetFilename = mAssetPackageDirectoryName + iterator->second.convert<std::string>();
			if (assetFilename.length() > RendererRuntime::Asset::MAXIMUM_ASSET_FILENAME_LENGTH)
			{
				const std::string message = "Asset filename \"" + assetFilename + "\" of asset ID " + std::to_string(assetId) + " is too long. Maximum allowed asset filename number of bytes is " + std::to_string(RendererRuntime::Asset::MAXIMUM_ASSET_FILENAME_LENGTH);
				throw std::exception(message.c_str());
			}

			// Copy asset data
			RendererRuntime::Asset& asset = sortedAssetVector[currentAssetIndex];
			asset.assetId = assetId;
			strcpy(asset.assetFilename, assetFilename.c_str());

			// Next asset, please
			++currentAssetIndex;
			++iterator;
		}
		std::sort(sortedAssetVector.begin(), sortedAssetVector.end(), detail::orderByAssetId);

		// Build the source asset ID to compiled asset ID map
		buildSourceAssetIdToCompiledAssetId();
	}

	void ProjectImpl::readTargetsByFilename(const std::string& filename)
	{
		// Open the input stream
		std::ifstream inputFileStream(mProjectDirectory + filename, std::ios::binary);

		// Parse JSON
		Poco::JSON::Parser jsonParser;
		jsonParser.parse(inputFileStream);
		Poco::JSON::Object::Ptr jsonRootObject = jsonParser.result().extract<Poco::JSON::Object::Ptr>();

		{ // Check whether or not the file format matches
			Poco::JSON::Object::Ptr jsonFormatObject = jsonRootObject->get("Format").extract<Poco::JSON::Object::Ptr>();
			if (jsonFormatObject->get("Type").convert<std::string>() != "Targets")
			{
				throw std::exception("Invalid JSON format type, must be \"Targets\"");
			}
			if (jsonFormatObject->get("Version").convert<uint32_t>() != 1)
			{
				throw std::exception("Invalid JSON format version, must be 1");
			}
		}

		// Read project data
		mJsonTargetsObject = jsonRootObject->get("Targets").extract<Poco::JSON::Object::Ptr>();
	}

	std::string ProjectImpl::getRenderTargetDataRootDirectory(const char* rendererTarget) const
	{
		Poco::JSON::Object::Ptr jsonRendererTargetsObject = mJsonTargetsObject->get("RendererTargets").extract<Poco::JSON::Object::Ptr>();
		Poco::JSON::Object::Ptr jsonRendererTargetObject = jsonRendererTargetsObject->get(rendererTarget).extract<Poco::JSON::Object::Ptr>();
		return "Data" + jsonRendererTargetObject->getValue<std::string>("Platform") + '/';
	}

	void ProjectImpl::buildSourceAssetIdToCompiledAssetId()
	{
		assert(0 == mSourceAssetIdToCompiledAssetId.size());
		assert(0 == mSourceAssetIdToAbsoluteFilename.size());

		const RendererRuntime::AssetPackage::SortedAssetVector& sortedAssetVector = mAssetPackage.getSortedAssetVector();
		const size_t numberOfAssets = sortedAssetVector.size();
		for (size_t i = 0; i < numberOfAssets; ++i)
		{
			const RendererRuntime::Asset& asset = sortedAssetVector[i];

			// Open the input stream
			std::ifstream inputFileStream(mProjectDirectory + asset.assetFilename, std::ios::binary);

			// Parse JSON
			Poco::JSON::Parser jsonParser;
			jsonParser.parse(inputFileStream);
			Poco::JSON::Object::Ptr jsonAssetRootObject = jsonParser.result().extract<Poco::JSON::Object::Ptr>();

			{ // Check whether or not the file format matches
				Poco::JSON::Object::Ptr jsonFormatObject = jsonAssetRootObject->get("Format").extract<Poco::JSON::Object::Ptr>();
				if (jsonFormatObject->get("Type").convert<std::string>() != "Asset")
				{
					throw std::exception("Invalid JSON format type, must be \"Asset\"");
				}
				if (jsonFormatObject->get("Version").convert<uint32_t>() != 1)
				{
					throw std::exception("Invalid JSON format version, must be 1");
				}
			}

			// Read asset metadata
			Poco::JSON::Object::Ptr jsonAssetObject = jsonAssetRootObject->get("Asset").extract<Poco::JSON::Object::Ptr>();
			Poco::JSON::Object::Ptr jsonAssetMetadataObject = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>();

			// Get the relevant asset metadata parts
			const std::string assetCategory = jsonAssetMetadataObject->getValue<std::string>("AssetCategory");
			const std::string assetType = jsonAssetMetadataObject->getValue<std::string>("AssetType");
			const std::string assetName = jsonAssetMetadataObject->getValue<std::string>("AssetName");
			
			// Construct the asset ID as string
			const std::string compiledAssetIdAsString = mProjectName + '/' + assetType + '/' + assetCategory + '/' + assetName;

			// Hash the asset ID and put it into the map
			mSourceAssetIdToCompiledAssetId.insert(std::make_pair(asset.assetId, RendererRuntime::StringId(compiledAssetIdAsString.c_str())));
			mSourceAssetIdToAbsoluteFilename.insert(std::make_pair(asset.assetId, mProjectDirectory + asset.assetFilename));
		}
	}

	void ProjectImpl::threadWorker()
	{
		while (!mShutdownThread)
		{
			// TODO(co) Implement me
		}
		NOP;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
