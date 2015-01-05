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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __RENDERERTOOLKIT_PROJECTIMPL_H__
#define __RENDERERTOOLKIT_PROJECTIMPL_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/Project/IProject.h"

#include <RendererRuntime/Asset/AssetPackage.h>

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4127)	// warning C4127: conditional expression is constant
	#pragma warning(disable: 4244)	// warning C4244: 'argument': conversion from '<x>' to '<y>', possible loss of data
	#pragma warning(disable: 4266)	// warning C4266: '<x>': no override available for virtual member function from base '<y>'; function is hidden
	#pragma warning(disable: 4365)	// warning C4365: 'return': conversion from '<x>' to '<y>', signed/unsigned mismatch
	#pragma warning(disable: 4548)	// warning C4548: expression before comma has no effect; expected expression with side-effect
	#pragma warning(disable: 4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	#pragma warning(disable: 4619)	// warning C4619: #pragma warning: there is no warning number '<x>'
	#pragma warning(disable: 4668)	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#define POCO_NO_UNWINDOWS
	#include <Poco/JSON/Parser.h>
#pragma warning(pop)

#include <thread>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
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


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IProject methods      ]
	//[-------------------------------------------------------]
	public:
		virtual void loadByFilename(const char* filename) override;
		virtual void compileAllAssets(const char* rendererTarget) override;
		virtual void startupAssetMonitor(const char* rendererTarget) override;
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
		void compileAsset(const RendererRuntime::AssetPackage::Asset& asset, const char* rendererTarget, RendererRuntime::AssetPackage& outputAssetPackage);
		std::string getRenderTargetDataRootDirectory(const char* rendererTarget) const;
		void workerThread();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		std::string					  mProjectName;
		std::string					  mProjectDirectory;
		RendererRuntime::AssetPackage mAssetPackage;
		Poco::JSON::Object::Ptr		  mJsonTargetsObject;	///< There's no real benefit in trying to store the targets data in custom data structures, so we just stick to the read in JSON object
		ProjectAssetMonitor*		  mProjectAssetMonitor;
		volatile bool				  mShutdownWorkerThread;
		std::thread					  mThread;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererToolkit/Project/ProjectImpl.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERERTOOLKIT_PROJECTIMPL_H__
