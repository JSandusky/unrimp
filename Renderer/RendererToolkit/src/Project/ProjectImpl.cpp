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
#include "RendererToolkit/Project/ProjectImpl.h"
#include "RendererToolkit/Project/ProjectAssetMonitor.h"
#include "RendererToolkit/Helper/FileSystemHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"
#include "RendererToolkit/AssetCompiler/MeshAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/SceneAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/TextureAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/MaterialAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/SkeletonAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/ShaderPieceAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/CompositorNodeAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/ShaderBlueprintAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/MaterialBlueprintAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/CompositorWorkspaceAssetCompiler.h"

#include <RendererRuntime/Core/Platform/PlatformManager.h>

// Disable warnings in external headers, we can't fix them
#ifdef WIN32
#pragma warning(push)
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#pragma warning(disable: 4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#pragma warning(disable: 4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	#pragma warning(disable: 4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
#endif
	#include <rapidjson/document.h>
#ifdef WIN32
#pragma warning(pop)
#endif

#include <cassert>
#include <fstream>
#include <algorithm>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		bool orderByAssetId(const RendererRuntime::Asset& left, const RendererRuntime::Asset& right)
		{
			return (left.assetId < right.assetId);
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
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProjectImpl::ProjectImpl() :
		mRapidJsonDocument(nullptr),
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
		rapidjson::Document rapidJsonDocument;
		JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, absoluteAssetFilename, "Asset", "1");

		// Mandatory main sections of the asset
		const rapidjson::Value& rapidJsonValueAsset = rapidJsonDocument["Asset"];
		const rapidjson::Value& rapidJsonValueAssetMetadata = rapidJsonValueAsset["AssetMetadata"];

		// Check asset ID match: A sanity check in here doesn't hurt
		const RendererRuntime::AssetId assetId = static_cast<uint32_t>(std::atoi(rapidJsonValueAssetMetadata["AssetId"].GetString()));
		if (assetId != asset.assetId)
		{
			const std::string message = "Failed to compile asset with filename \"" + std::string(asset.assetFilename) + "\": According to the asset package it should be asset ID " + std::to_string(asset.assetId) + " but inside the asset file it's asset ID " + std::to_string(assetId);
			throw std::runtime_error(message);
		}

		// Dispatch asset compiler
		// TODO(co) Add multithreading support: Add compiler queue which is processed in the background, ensure compiler instances are reused

		// Get the asset input directory and asset output directory
		const std::string assetInputDirectory = STD_FILESYSTEM_PATH(absoluteAssetFilename).parent_path().generic_string() + '/';
		const std::string assetType = rapidJsonValueAssetMetadata["AssetType"].GetString();
		const std::string assetCategory = rapidJsonValueAssetMetadata["AssetCategory"].GetString();
		const std::string assetOutputDirectory = "../" + getRenderTargetDataRootDirectory(rendererTarget) + mAssetPackageDirectoryName + assetType + '/' + assetCategory + '/';

		// Ensure that the asset output directory exists, else creating output file streams will fail
		#ifdef WIN32
			std::tr2::sys::create_directories(assetOutputDirectory);
		#else
			std::experimental::filesystem::create_directories(assetOutputDirectory);
		#endif

		// Asset compiler input
		IAssetCompiler::Input input(mProjectName, assetInputDirectory, assetOutputDirectory, mSourceAssetIdToCompiledAssetId, mSourceAssetIdToAbsoluteFilename);

		// Asset compiler configuration
		assert(nullptr != mRapidJsonDocument);
		const IAssetCompiler::Configuration configuration(rapidJsonDocument, (*mRapidJsonDocument)["Targets"], rendererTarget);

		// Asset compiler output
		IAssetCompiler::Output output;
		output.outputAssetPackage = &outputAssetPackage;

		// Evaluate the asset type and continue with the processing in the asset type specific way
		// TODO(co) Currently this is fixed build in, later on me might want to have this dynamic so we can plugin additional asset compilers
		const AssetCompilerTypeId assetCompilerTypeId(assetType.c_str());
		if (TextureAssetCompiler::TYPE_ID == assetCompilerTypeId)
		{
			TextureAssetCompiler().compile(input, configuration, output);
		}
		else if (ShaderPieceAssetCompiler::TYPE_ID == assetCompilerTypeId)
		{
			ShaderPieceAssetCompiler().compile(input, configuration, output);
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
		else if (CompositorNodeAssetCompiler::TYPE_ID == assetCompilerTypeId)
		{
			CompositorNodeAssetCompiler().compile(input, configuration, output);
		}
		else if (CompositorWorkspaceAssetCompiler::TYPE_ID == assetCompilerTypeId)
		{
			CompositorWorkspaceAssetCompiler().compile(input, configuration, output);
		}
		else
		{
			const std::string message = "Failed to compile asset with filename \"" + std::string(asset.assetFilename) + "\" and ID " + std::to_string(asset.assetId) + ": Asset type \"" + assetType + "\" is unknown";
			throw std::runtime_error(message);
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
		rapidjson::Document rapidJsonDocument;
		JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, filename, "Project", "1");

		// Read project metadata
		const rapidjson::Value& rapidJsonValueProject = rapidJsonDocument["Project"];
		mProjectName = rapidJsonValueProject["ProjectMetadata"]["Name"].GetString();

		{ // Read project data
			mProjectDirectory = STD_FILESYSTEM_PATH(filename).parent_path().generic_string() + '/';
			readAssetsByFilename(rapidJsonValueProject["AssetsFilename"].GetString());
			readTargetsByFilename(rapidJsonValueProject["TargetsFilename"].GetString());
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
			std::sort(sortedAssetVector.begin(), sortedAssetVector.end(), ::detail::orderByAssetId);

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
		if (nullptr != mRapidJsonDocument)
		{
			delete mRapidJsonDocument;
			mRapidJsonDocument = nullptr;
		}
	}

	void ProjectImpl::readAssetsByFilename(const std::string& filename)
	{
		// Open the input stream
		const std::string absoluteFilename = mProjectDirectory + filename;
		std::ifstream inputFileStream(absoluteFilename, std::ios::binary);

		// Parse JSON
		rapidjson::Document rapidJsonDocument;
		JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, absoluteFilename, "Assets", "1");

		// Get the asset package name (includes "/" at the end)
		mAssetPackageDirectoryName = STD_FILESYSTEM_PATH(filename).parent_path().generic_string() + '/';

		// Read project data
		const rapidjson::Value& rapidJsonValueAssets = rapidJsonDocument["Assets"];
		const size_t numberOfAssets = rapidJsonValueAssets.MemberCount();
		RendererRuntime::AssetPackage::SortedAssetVector& sortedAssetVector = mAssetPackage.getWritableSortedAssetVector();
		sortedAssetVector.resize(numberOfAssets);
		size_t currentAssetIndex = 0;
		for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorAssets = rapidJsonValueAssets.MemberBegin(); rapidJsonMemberIteratorAssets != rapidJsonValueAssets.MemberEnd(); ++rapidJsonMemberIteratorAssets)
		{
			// Get asset data
			const RendererRuntime::AssetId assetId = static_cast<uint32_t>(std::atoi(rapidJsonMemberIteratorAssets->name.GetString()));
			const std::string assetFilename = mAssetPackageDirectoryName + rapidJsonMemberIteratorAssets->value.GetString();
			if (assetFilename.length() > RendererRuntime::Asset::MAXIMUM_ASSET_FILENAME_LENGTH)
			{
				const std::string message = "Asset filename \"" + assetFilename + "\" of asset ID " + std::to_string(assetId) + " is too long. Maximum allowed asset filename number of bytes is " + std::to_string(RendererRuntime::Asset::MAXIMUM_ASSET_FILENAME_LENGTH);
				throw std::runtime_error(message);
			}

			// Copy asset data
			RendererRuntime::Asset& asset = sortedAssetVector[currentAssetIndex];
			asset.assetId = assetId;
			strcpy(asset.assetFilename, assetFilename.c_str());

			// Next asset, please
			++currentAssetIndex;
		}
		std::sort(sortedAssetVector.begin(), sortedAssetVector.end(), ::detail::orderByAssetId);

		// Build the source asset ID to compiled asset ID map
		buildSourceAssetIdToCompiledAssetId();
	}

	void ProjectImpl::readTargetsByFilename(const std::string& filename)
	{
		// Open the input stream
		const std::string absoluteFilename = mProjectDirectory + filename;
		std::ifstream inputFileStream(absoluteFilename, std::ios::binary);

		// Parse JSON
		if (nullptr == mRapidJsonDocument)
		{
			mRapidJsonDocument = new rapidjson::Document();
		}
		JsonHelper::parseDocumentByInputFileStream(*mRapidJsonDocument, inputFileStream, absoluteFilename, "Targets", "1");
	}

	std::string ProjectImpl::getRenderTargetDataRootDirectory(const char* rendererTarget) const
	{
		assert(nullptr != mRapidJsonDocument);
		const rapidjson::Value& rapidJsonValueRendererTargets = (*mRapidJsonDocument)["Targets"]["RendererTargets"];
		const rapidjson::Value& rapidJsonValueRendererTarget = rapidJsonValueRendererTargets[rendererTarget];
		return "Data" + std::string(rapidJsonValueRendererTarget["Platform"].GetString()) + '/';
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
			const std::string absoluteAssetFilename = mProjectDirectory + asset.assetFilename;
			std::ifstream inputFileStream(absoluteAssetFilename, std::ios::binary);

			// Parse JSON
			rapidjson::Document rapidJsonDocument;
			JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, absoluteAssetFilename, "Asset", "1");

			// Mandatory main sections of the asset
			const rapidjson::Value& rapidJsonValueAsset = rapidJsonDocument["Asset"];
			const rapidjson::Value& rapidJsonValueAssetMetadata = rapidJsonValueAsset["AssetMetadata"];

			// Get the relevant asset metadata parts
			const std::string assetCategory = rapidJsonValueAssetMetadata["AssetCategory"].GetString();
			const std::string assetType = rapidJsonValueAssetMetadata["AssetType"].GetString();
			const std::string assetName = rapidJsonValueAssetMetadata["AssetName"].GetString();
			
			// Construct the asset ID as string
			const std::string compiledAssetIdAsString = mProjectName + '/' + assetType + '/' + assetCategory + '/' + assetName;

			// Hash the asset ID and put it into the map
			mSourceAssetIdToCompiledAssetId.emplace(asset.assetId, RendererRuntime::StringId(compiledAssetIdAsString.c_str()));
			mSourceAssetIdToAbsoluteFilename.emplace(asset.assetId, mProjectDirectory + asset.assetFilename);
		}
	}

	void ProjectImpl::threadWorker()
	{
		RendererRuntime::PlatformManager::setCurrentThreadName("Project worker", "Renderer toolkit: Project worker");

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
