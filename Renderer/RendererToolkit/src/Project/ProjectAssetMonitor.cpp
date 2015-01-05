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
		//[ Public virtual FW::FileWatchListener methods          ]
		//[-------------------------------------------------------]
		public:
			void handleFileAction(FW::WatchID watchId, const FW::String& directory, const FW::String& filename, FW::Action action) override
			{
				// TODO(co) Implement me
				std::cout << "DIR (" << directory + ") FILE (" + filename + ") has event " << action << std::endl;
			}


		};
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ProjectAssetMonitor::ProjectAssetMonitor(ProjectImpl& projectImpl, const std::string& rendererTarget) :
		mProjectImpl(projectImpl),
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
		detail::FileWatchListener fileWatchListener;
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
