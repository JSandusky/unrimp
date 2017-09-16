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
#include <RendererRuntime/Core/StringId.h>

#include <Renderer/Public/Renderer.h>

#include <string>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererToolkit
{
	class Context;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef RendererRuntime::StringId AssetImporterTypeId;	///< Asset importer type identifier, internally just a POD "uint32_t"


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    An asset importer integrates new assets into the source asset database (aka "data source") on which then asset compilers can work in order to transform an asset from a source format into a format the renderer runtime understands
	*
	*  @note
	*    - Asset importers won't manipulate the given source assets to not risk introducing any errors
	*    - The imported assets will be categories by asset type into different directories and decorated with additional asset metadata for asset compilers
	*
	*  @todo
	*    - TODO(co) Under construction
	*/
	class IAssetImporter : public Renderer::RefCount<IAssetImporter>
	{


	//[-------------------------------------------------------]
	//[ Public structures                                     ]
	//[-------------------------------------------------------]
	public:
		struct Input
		{
			const Context&	  context;
			const std::string projectName;
			const std::string assetFilename;
			const std::string assetInputDirectory;
			const std::string assetOutputDirectory;

			Input() = delete;
			Input(const Context& _context, const std::string _projectName, const std::string& _assetFilename, const std::string& _assetInputDirectory, const std::string& _assetOutputDirectory) :
				context(_context),
				projectName(_projectName),
				assetFilename(_assetFilename),
				assetInputDirectory(_assetInputDirectory),
				assetOutputDirectory(_assetOutputDirectory)
			{
				// Nothing here
			}
			Input(const Input&) = delete;
			Input& operator=(const Input&) = delete;
		};


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetImporter methods ]
	//[-------------------------------------------------------]
	public:
		virtual AssetImporterTypeId getAssetImporterTypeId() const = 0;
		virtual void import(const Input& input) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline IAssetImporter() {}
		inline virtual ~IAssetImporter() override {}


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef Renderer::SmartRefCount<IAssetImporter> IAssetImporterPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit