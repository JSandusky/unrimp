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


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/Project/ProjectAssetMonitor.h"
#include "RendererToolkit/Project/ProjectImpl.h"

#include <RendererRuntime/Public/RendererRuntime.h>

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
#pragma warning(pop)

#include <FileWatcher/FileWatcher.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	namespace detail
	{
		class FileWatchListener : public FW::FileWatchListener
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			explicit FileWatchListener(ProjectAssetMonitor& projectAssetMonitor) :
				mProjectAssetMonitor(projectAssetMonitor)
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Public virtual FW::FileWatchListener methods          ]
		//[-------------------------------------------------------]
		public:
			void handleFileAction(FW::WatchID watchId, const FW::String& directory, const FW::String& filename, FW::Action action) override
			{
				if (FW::Action::Modified == action)
				{
					// Get the corresponding asset
					// TODO(co) The current simple solution is not sufficient for larg scale projects having ten thousands of assets: Add more efficient asset search
					// TODO(co) Add support for asset "FileDependencies". The current solution is just a quick'n'dirty prototype which will not work when multiple or other named asset data files are involved.
					std::string test = Poco::Path(filename).setExtension("").toString(Poco::Path::PATH_UNIX) + ".asset";

					const RendererRuntime::AssetPackage::SortedAssetVector& sortedAssetVector = mProjectAssetMonitor.mProjectImpl.getAssetPackage().getSortedAssetVector();
					const size_t numberOfAssets = sortedAssetVector.size();
					for (size_t i = 0; i < numberOfAssets; ++i)
					{
						const RendererRuntime::Asset& asset = sortedAssetVector[i];
						if (test == asset.assetFilename)
						{
							// TODO(co) Performance: Add asset compiler queue so we can compile more then one asset at a time in background
							// TODO(co) At the moment, we only support modifying already existing asset data, we should add support for changes inside the runtime asset package as well
							RendererRuntime::AssetPackage outputAssetPackage;
							mProjectAssetMonitor.mProjectImpl.compileAsset(asset, mProjectAssetMonitor.mRendererTarget.c_str(), outputAssetPackage);

							// Inform the asset manager about the modified assets (just pass them individually, there's no real benefit in trying to apply "were's one, there are many" in this situation)
							const RendererRuntime::AssetPackage::SortedAssetVector& sortedOutputAssetVector = outputAssetPackage.getSortedAssetVector();
							const size_t numberOfOutputAssets = sortedOutputAssetVector.size();
							for (size_t outputAssetIndex = 0; outputAssetIndex < numberOfOutputAssets; ++outputAssetIndex)
							{
								mProjectAssetMonitor.mRendererRuntime.reloadResourceByAssetId(sortedOutputAssetVector[outputAssetIndex].assetId);
							}
							break;
						}
					}
				}
			}


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			ProjectAssetMonitor& mProjectAssetMonitor;


		};
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProjectAssetMonitor::ProjectAssetMonitor(ProjectImpl& projectImpl, RendererRuntime::IRendererRuntime& rendererRuntime, const std::string& rendererTarget) :
		mProjectImpl(projectImpl),
		mRendererRuntime(rendererRuntime),
		mRendererTarget(rendererTarget),
		mShutdownWorkerThread(false),
		mThread(&ProjectAssetMonitor::workerThread, this)
	{
		// Nothing here
	}

	ProjectAssetMonitor::~ProjectAssetMonitor()
	{
		mShutdownWorkerThread = true;
		mThread.join();
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void ProjectAssetMonitor::workerThread()
	{
		// Create the file watcher object
		FW::FileWatcher fileWatcher;
		detail::FileWatchListener fileWatchListener(*this);
		const FW::WatchID watchID = fileWatcher.addWatch(mProjectImpl.getProjectDirectory(), &fileWatchListener, true);

		// Update the file watcher object as long as the project asset monitor is up-and-running
		while (!mShutdownWorkerThread)
		{
			fileWatcher.update();
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
