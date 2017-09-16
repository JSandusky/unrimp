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
#include "RendererToolkit/Project/ProjectImpl.h"
#include "RendererToolkit/Project/ProjectAssetMonitor.h"
#include "RendererToolkit/Helper/FileSystemHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"
#include "RendererToolkit/Helper/StringHelper.h"
#include "RendererToolkit/Helper/CacheManager.h"
#include "RendererToolkit/AssetCompiler/MeshAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/SceneAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/TextureAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/MaterialAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/SkeletonAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/ShaderPieceAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/CompositorNodeAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/ShaderBlueprintAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/VertexAttributesAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/SkeletonAnimationAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/MaterialBlueprintAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/CompositorWorkspaceAssetCompiler.h"
#include "RendererToolkit/Context.h"

#include <RendererRuntime/Core/File/MemoryFile.h>
#include <RendererRuntime/Core/Platform/PlatformManager.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#include <rapidjson/document.h>
PRAGMA_WARNING_POP

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

		void optionalQualityStrategy(const rapidjson::Value& rapidJsonValue, const char* propertyName, RendererToolkit::QualityStrategy& value)
		{
			if (rapidJsonValue.HasMember(propertyName))
			{
				const rapidjson::Value& rapidJsonValueValueType = rapidJsonValue[propertyName];
				const char* valueAsString = rapidJsonValueValueType.GetString();
				const rapidjson::SizeType valueStringLength = rapidJsonValueValueType.GetStringLength();

				// Define helper macros
				#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) value = RendererToolkit::QualityStrategy::name;
				#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) value = RendererToolkit::QualityStrategy::name;

				// Evaluate value
				IF_VALUE(DEBUG)
				ELSE_IF_VALUE(PRODUCTION)
				ELSE_IF_VALUE(SHIPPING)
				else
				{
					throw std::runtime_error("Quality strategy must be \"DEBUG\", \"PRODUCTION\" or \"SHIPPING\"");
				}

				// Undefine helper macros
				#undef IF_VALUE
				#undef ELSE_IF_VALUE
			}
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
	ProjectImpl::ProjectImpl(const Context& context) :
		mContext(context),
		mQualityStrategy(QualityStrategy::PRODUCTION),
		mRapidJsonDocument(nullptr),
		mProjectAssetMonitor(nullptr),
		mShutdownThread(false)
	{
		mThread = std::thread(&ProjectImpl::threadWorker, this);

		// Setup asset compilers map
		// TODO(co) Currently this is fixed build in, later on me might want to have this dynamic so we can plugin additional asset compilers
		mAssetCompilers.emplace(TextureAssetCompiler::TYPE_ID, std::make_unique<TextureAssetCompiler>(mContext));
		mAssetCompilers.emplace(ShaderPieceAssetCompiler::TYPE_ID, std::make_unique<ShaderPieceAssetCompiler>());
		mAssetCompilers.emplace(ShaderBlueprintAssetCompiler::TYPE_ID, std::make_unique<ShaderBlueprintAssetCompiler>());
		mAssetCompilers.emplace(MaterialBlueprintAssetCompiler::TYPE_ID, std::make_unique<MaterialBlueprintAssetCompiler>());
		mAssetCompilers.emplace(MaterialAssetCompiler::TYPE_ID, std::make_unique<MaterialAssetCompiler>());
		mAssetCompilers.emplace(SkeletonAssetCompiler::TYPE_ID, std::make_unique<SkeletonAssetCompiler>());
		mAssetCompilers.emplace(SkeletonAnimationAssetCompiler::TYPE_ID, std::make_unique<SkeletonAnimationAssetCompiler>());
		mAssetCompilers.emplace(MeshAssetCompiler::TYPE_ID, std::make_unique<MeshAssetCompiler>());
		mAssetCompilers.emplace(SceneAssetCompiler::TYPE_ID, std::make_unique<SceneAssetCompiler>());
		mAssetCompilers.emplace(CompositorNodeAssetCompiler::TYPE_ID, std::make_unique<CompositorNodeAssetCompiler>());
		mAssetCompilers.emplace(CompositorWorkspaceAssetCompiler::TYPE_ID, std::make_unique<CompositorWorkspaceAssetCompiler>());
		mAssetCompilers.emplace(VertexAttributesAssetCompiler::TYPE_ID, std::make_unique<VertexAttributesAssetCompiler>());
	}

	ProjectImpl::~ProjectImpl()
	{
		// Shutdown worker thread
		mShutdownThread = true;
		mThread.join();

		// Clear
		clear();
	}

	const char* ProjectImpl::tryGetAssetFilenameByAssetId(RendererRuntime::AssetId assetId) const
	{
		return mAssetPackage.tryGetAssetFilenameByAssetId(assetId);
	}

	void ProjectImpl::compileAsset(const RendererRuntime::Asset& asset, const char* rendererTarget, RendererRuntime::AssetPackage& outputAssetPackage)
	{
		// Open the input stream
		const std::string assetFilename = mProjectDirectory + asset.assetFilename;
		std::ifstream inputFileStream(assetFilename, std::ios::binary);

		// Parse JSON
		rapidjson::Document rapidJsonDocument;
		JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, assetFilename, "Asset", "1");

		// Mandatory main sections of the asset
		const rapidjson::Value& rapidJsonValueAsset = rapidJsonDocument["Asset"];
		const rapidjson::Value& rapidJsonValueAssetMetadata = rapidJsonValueAsset["AssetMetadata"];

		// Dispatch asset compiler
		// TODO(co) Add multi-threading support: Add compiler queue which is processed in the background, ensure compiler instances are reused

		// Get the asset input directory and asset output directory
		const std::string assetPackageInputDirectory = mProjectDirectory + mAssetPackageDirectoryName;
		const std::string assetInputDirectory = std_filesystem::path(assetFilename).parent_path().generic_string() + '/';
		const std::string assetType = rapidJsonValueAssetMetadata["AssetType"].GetString();
		const std::string assetCategory = rapidJsonValueAssetMetadata["AssetCategory"].GetString();
		const std::string assetOutputDirectory = "../" + getRenderTargetDataRootDirectory(rendererTarget) + mAssetPackageDirectoryName + assetType + '/' + assetCategory + '/';

		// Ensure that the asset output directory exists, else creating output file streams will fail
		std_filesystem::create_directories(assetOutputDirectory);

		// Asset compiler input
		IAssetCompiler::Input input(mContext, mProjectName, *mCacheManager.get(), assetPackageInputDirectory, assetFilename, assetInputDirectory, assetOutputDirectory, mSourceAssetIdToCompiledAssetId, mSourceAssetIdToAbsoluteFilename);

		// Asset compiler configuration
		assert(nullptr != mRapidJsonDocument);
		const IAssetCompiler::Configuration configuration(rapidJsonDocument, (*mRapidJsonDocument)["Targets"], rendererTarget, mQualityStrategy);

		// Asset compiler output
		IAssetCompiler::Output output;
		output.outputAssetPackage = &outputAssetPackage;

		// Evaluate the asset type and continue with the processing in the asset type specific way
		// TODO(co) Currently this is fixed build in, later on me might want to have this dynamic so we can plugin additional asset compilers
		try
		{
			AssetCompilers::const_iterator iterator = mAssetCompilers.find(AssetCompilerTypeId(assetType.c_str()));
			if (mAssetCompilers.end() != iterator)
			{
				iterator->second->compile(input, configuration, output);
			}
			else
			{
				throw std::runtime_error("Asset type \"" + assetType + "\" is unknown");
			}
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("Failed to compile asset with filename \"" + std::string(asset.assetFilename) + "\": " + std::string(e.what()));
		}
	}

	void ProjectImpl::onCompilationRunFinished()
	{
		// Compilation run finished clear internal cache of cache manager
		mCacheManager->clearInternalCache();
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
			mProjectDirectory = std_filesystem::path(filename).parent_path().generic_string() + '/';
			RENDERER_LOG(mContext, INFORMATION, "Gather asset from %s...", mProjectDirectory.c_str())
			{ // Asset packages
				const rapidjson::Value& rapidJsonValueAssetPackages = rapidJsonValueProject["AssetPackages"];
				if (rapidJsonValueAssetPackages.Size() > 1)
				{
					throw std::runtime_error("TODO(co) Support for multiple asset packages isn't implemented, yet");
				}
				for (rapidjson::SizeType i = 0; i < rapidJsonValueAssetPackages.Size(); ++i)
				{
					readAssetPackageByDirectory(std::string(rapidJsonValueAssetPackages[i].GetString()) + '/');
				}
			}
			readTargetsByFilename(rapidJsonValueProject["TargetsFilename"].GetString());
			::detail::optionalQualityStrategy(rapidJsonValueProject, "QualityStrategy", mQualityStrategy);
			RENDERER_LOG(mContext, INFORMATION, "Found %u assets", mAssetPackage.getSortedAssetVector().size())

			// Setup project folder for cache manager, it will store there its data
			// TODO(sw) For now only prototype. Change this.
			mCacheManager = std::make_unique<CacheManager>(mContext, mProjectName);
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
				checkAssetIsChanged(sortedAssetVector[i], rendererTarget);
			}
			for (size_t i = 0; i < numberOfAssets; ++i)
			{
				compileAsset(sortedAssetVector[i], rendererTarget, outputAssetPackage);
			}
		}

		{ // Write runtime asset package
			RendererRuntime::AssetPackage::SortedAssetVector& sortedAssetVector = outputAssetPackage.getWritableSortedAssetVector();
			RendererRuntime::MemoryFile memoryFile(0, 4096);

			// Ensure the asset package is sorted
			std::sort(sortedAssetVector.begin(), sortedAssetVector.end(), ::detail::orderByAssetId);

			{ // Write down the asset package header
				#pragma pack(push)
				#pragma pack(1)
					struct AssetPackageHeader
					{
						uint32_t numberOfAssets;
					};
				#pragma pack(pop)
				AssetPackageHeader assetPackageHeader;
				assetPackageHeader.numberOfAssets = static_cast<uint32_t>(sortedAssetVector.size());
				memoryFile.write(&assetPackageHeader, sizeof(AssetPackageHeader));
			}

			// Write down the asset package content in one single burst
			memoryFile.write(sortedAssetVector.data(), sizeof(RendererRuntime::Asset) * sortedAssetVector.size());

			// Write LZ4 compressed output
			memoryFile.writeLz4CompressedDataToFile(RendererRuntime::StringId("AssetPackage"), 2, "../" + getRenderTargetDataRootDirectory(rendererTarget) + mAssetPackageDirectoryName + "AssetPackage.assets", mContext.getFileManager());
		}

		// Compilation run finished clear internal caches/states
		onCompilationRunFinished();
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
		mQualityStrategy = QualityStrategy::PRODUCTION;
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

	void ProjectImpl::readAssetPackageByDirectory(const std::string& directoryName)
	{
		// Get the asset package name (includes "/" at the end)
		mAssetPackageDirectoryName = directoryName;

		// Discover assets
		RendererRuntime::AssetPackage::SortedAssetVector& sortedAssetVector = mAssetPackage.getWritableSortedAssetVector();
		const std::string absoluteDirectoryName = mProjectDirectory + directoryName;
		for (auto& iterator: std_filesystem::recursive_directory_iterator(absoluteDirectoryName))
		{
			if (std_filesystem::is_regular_file(iterator))
			{
				std::string assetIdAsString = iterator.path().generic_string();
				if (StringHelper::isSourceAssetIdAsString(assetIdAsString))
				{
					assetIdAsString.erase(0, absoluteDirectoryName.length());

					// Get asset data
					const std::string assetFilename = mAssetPackageDirectoryName + assetIdAsString;
					if (assetFilename.length() > RendererRuntime::Asset::MAXIMUM_ASSET_FILENAME_LENGTH)
					{
						const std::string message = "Asset filename \"" + assetFilename + "\" is too long. Maximum allowed asset filename number of bytes is " + std::to_string(RendererRuntime::Asset::MAXIMUM_ASSET_FILENAME_LENGTH);
						throw std::runtime_error(message);
					}

					// Copy asset data
					RendererRuntime::Asset asset;
					asset.assetId = StringHelper::getSourceAssetIdByString(assetIdAsString.c_str());
					strcpy(asset.assetFilename, assetFilename.c_str());
					sortedAssetVector.push_back(asset);
				}
			}
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
			const std::string assetName = std_filesystem::path(asset.assetFilename).stem().generic_string();

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

	void ProjectImpl::checkAssetIsChanged(const RendererRuntime::Asset& asset, const char* rendererTarget)
	{
		const std::string assetFilename = mProjectDirectory + asset.assetFilename;
		std::ifstream inputFileStream(assetFilename, std::ios::binary);

		// Parse JSON
		rapidjson::Document rapidJsonDocument;
		JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, assetFilename, "Asset", "1");

		// Mandatory main sections of the asset
		const rapidjson::Value& rapidJsonValueAsset = rapidJsonDocument["Asset"];
		const rapidjson::Value& rapidJsonValueAssetMetadata = rapidJsonValueAsset["AssetMetadata"];

		// Dispatch asset compiler
		// TODO(co) Add multi-threading support: Add compiler queue which is processed in the background, ensure compiler instances are reused

		// Get the asset input directory and asset output directory
		const std::string assetPackageInputDirectory = mProjectDirectory + mAssetPackageDirectoryName;
		const std::string assetInputDirectory = std_filesystem::path(assetFilename).parent_path().generic_string() + '/';
		const std::string assetType = rapidJsonValueAssetMetadata["AssetType"].GetString();
		const std::string assetCategory = rapidJsonValueAssetMetadata["AssetCategory"].GetString();
		const std::string assetOutputDirectory = "../" + getRenderTargetDataRootDirectory(rendererTarget) + mAssetPackageDirectoryName + assetType + '/' + assetCategory + '/';

		// Asset compiler input
		IAssetCompiler::Input input(mContext, mProjectName, *mCacheManager.get(), assetPackageInputDirectory, assetFilename, assetInputDirectory, assetOutputDirectory, mSourceAssetIdToCompiledAssetId, mSourceAssetIdToAbsoluteFilename);

		// Asset compiler configuration
		AssetCompilers::const_iterator iterator = mAssetCompilers.find(AssetCompilerTypeId(assetType.c_str()));
		if (mAssetCompilers.end() != iterator)
		{
			assert(nullptr != mRapidJsonDocument);
			const IAssetCompiler::Configuration configuration(rapidJsonDocument, (*mRapidJsonDocument)["Targets"], rendererTarget, mQualityStrategy);
			iterator->second->checkIfChanged(input, configuration);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
