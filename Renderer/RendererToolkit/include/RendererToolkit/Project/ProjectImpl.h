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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/Project/IProject.h"
#include "RendererToolkit/AssetCompiler/IAssetCompiler.h"	// For "RendererToolkit::QualityStrategy"

#include <RendererRuntime/Asset/AssetPackage.h>

#include <thread>
#include <atomic>
#include <memory> // For std::unique_ptr


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IRendererRuntime;
}
namespace RendererToolkit
{
	class Context;
	class CacheManager;
	class ProjectAssetMonitor;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef std::unordered_map<uint32_t, uint32_t> SourceAssetIdToCompiledAssetId;		// Key = source asset ID, value = compiled asset ID ("AssetId"-type not used directly or we would need to define a hash-function for it)
	typedef std::unordered_map<uint32_t, std::string> SourceAssetIdToAbsoluteFilename;	// Key = source asset ID, absolute asset filename


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Project class implementation
	*/
	class ProjectImpl : public IProject
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] context
		*    The renderer toolkit context instance to use
		*/
		explicit ProjectImpl(const Context& context);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ProjectImpl() override;

		inline const std::string& getProjectName() const;
		inline const std::string& getProjectDirectory() const;
		inline const RendererRuntime::AssetPackage& getAssetPackage() const;
		const char* tryGetAssetFilenameByAssetId(RendererRuntime::AssetId assetId) const;
		void compileAsset(const RendererRuntime::Asset& asset, const char* rendererTarget, RendererRuntime::AssetPackage& outputAssetPackage);


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IProject methods      ]
	//[-------------------------------------------------------]
	public:
		virtual void loadByFilename(const char* filename) override;
		virtual void compileAllAssets(const char* rendererTarget) override;
		virtual void startupAssetMonitor(RendererRuntime::IRendererRuntime& rendererRuntime, const char* rendererTarget) override;
		virtual void shutdownAssetMonitor() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline explicit ProjectImpl(const ProjectImpl& source) = delete;
		inline ProjectImpl& operator =(const ProjectImpl& source) = delete;
		void clear();
		void readAssetPackageByDirectory(const std::string& directoryName);
		void readTargetsByFilename(const std::string& filename);
		std::string getRenderTargetDataRootDirectory(const char* rendererTarget) const;
		void buildSourceAssetIdToCompiledAssetId();
		void threadWorker();
		void checkAssetIsChanged(const RendererRuntime::Asset& asset, const char* rendererTarget);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		const Context&					mContext;
		std::string						mProjectName;
		std::string						mProjectDirectory;			///< Includes "/" at the end
		QualityStrategy					mQualityStrategy;
		RendererRuntime::AssetPackage	mAssetPackage;
		std::string						mAssetPackageDirectoryName;	///< Asset package name (includes "/" at the end)
		SourceAssetIdToCompiledAssetId	mSourceAssetIdToCompiledAssetId;
		SourceAssetIdToAbsoluteFilename	mSourceAssetIdToAbsoluteFilename;
		rapidjson::Document*			mRapidJsonDocument;			///< There's no real benefit in trying to store the targets data in custom data structures, so we just stick to the read in JSON object
		ProjectAssetMonitor*			mProjectAssetMonitor;
		std::atomic<bool>				mShutdownThread;
		std::thread						mThread;
		std::unique_ptr<CacheManager>	mCacheManager;
		
		std::unordered_map<uint32_t, std::unique_ptr<IAssetCompiler>> mAssetCompilers; ///< List of asset compilers key "AssetCompilerTypeId" (type not used directly or we would need to define a hash-function for it)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererToolkit/Project/ProjectImpl.inl"
