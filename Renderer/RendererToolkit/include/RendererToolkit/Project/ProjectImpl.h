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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/Project/IProject.h"

#include <RendererRuntime/Asset/AssetPackage.h>

#include <rapidjson/fwd.h>

// Disable warnings in external headers, we can't fix them
#ifdef WIN32
#pragma warning(push)

	#pragma warning(disable: 4265)	// warning C4265: '<x>': class has virtual functions, but destructor is not virtual
#endif
	#include <thread>
#ifdef WIN32
#pragma warning(pop)
#endif

#include <atomic>
#include <unordered_map>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IRendererRuntime;
}
namespace RendererToolkit
{
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
		*/
		explicit ProjectImpl();

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ProjectImpl();

		inline const std::string& getProjectName() const;
		inline const std::string& getProjectDirectory() const;
		inline const RendererRuntime::AssetPackage& getAssetPackage() const;
		const char* getAssetFilenameByAssetId(RendererRuntime::AssetId assetId) const;
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
		inline explicit ProjectImpl(const ProjectImpl &source) = delete;
		inline ProjectImpl &operator =(const ProjectImpl &source) = delete;
		void clear();
		void readAssetsByFilename(const std::string& filename);
		void readTargetsByFilename(const std::string& filename);
		std::string getRenderTargetDataRootDirectory(const char* rendererTarget) const;
		void buildSourceAssetIdToCompiledAssetId();
		void threadWorker();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		std::string						mProjectName;
		std::string						mProjectDirectory;
		RendererRuntime::AssetPackage	mAssetPackage;
		std::string						mAssetPackageDirectoryName;	///< Asset package name (includes "/" at the end)
		SourceAssetIdToCompiledAssetId	mSourceAssetIdToCompiledAssetId;
		SourceAssetIdToAbsoluteFilename	mSourceAssetIdToAbsoluteFilename;
		rapidjson::Document*			mRapidJsonDocument;	///< There's no real benefit in trying to store the targets data in custom data structures, so we just stick to the read in JSON object
		ProjectAssetMonitor*			mProjectAssetMonitor;
		std::atomic<bool>				mShutdownThread;
		std::thread						mThread;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererToolkit/Project/ProjectImpl.inl"
