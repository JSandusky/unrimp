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

#include <rapidjson/fwd.h>

#include <string>
#include <unordered_map>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class AssetPackage;
}
namespace RendererToolkit
{
	class Context;
	class CacheManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef RendererRuntime::StringId AssetCompilerTypeId;	///< Asset compiler type identifier, internally just a POD "uint32_t"
	typedef std::unordered_map<uint32_t, uint32_t> SourceAssetIdToCompiledAssetId;		// Key = source asset ID, value = compiled asset ID ("AssetId"-type not used directly or we would need to define a hash-function for it)
	typedef std::unordered_map<uint32_t, std::string> SourceAssetIdToAbsoluteFilename;	// Key = source asset ID, absolute asset filename

	/**
	*  @brief
	*    Overall quality strategy which is a trade-off between "fast" and "good"
	*/
	enum class QualityStrategy
	{
		DEBUG,		///< Best possible speed, quality doesn't matter as long as things can still be identified
		PRODUCTION,	///< Decent speed and decent quality so e.g. artists can fine tune assets
		SHIPPING	///< Product is about to be shipped to clients, best possible speed as long as it finishes before the sun burns out
	};


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    An asset compiler transforms an asset from a source format into a format the renderer runtime understands
	*
	*  @note
	*    - The asset compiler only crunches data already known to the source asset database (aka "data source"), it doesn't process external assets
	*    - An asset compiler only cares about a single asset, it doesn't for example processes automatically all material assets
	*      a mesh asset is referencing and then all texture assets a material assets is referencing
	*    - Either manually integrate new assets into the source asset database or use an asset importer to do so
	*/
	class IAssetCompiler : public Renderer::RefCount<IAssetCompiler>
	{


	//[-------------------------------------------------------]
	//[ Public structures                                     ]
	//[-------------------------------------------------------]
	public:
		struct Input
		{
			const Context&							context;
			const std::string						projectName;
			CacheManager&							cacheManager;
			const std::string						assetFilename;
			const std::string						assetInputDirectory;
			const std::string						assetOutputDirectory;
			const SourceAssetIdToCompiledAssetId&	sourceAssetIdToCompiledAssetId;
			const SourceAssetIdToAbsoluteFilename&	sourceAssetIdToAbsoluteFilename;

			Input() = delete;
			Input(const Context& _context, const std::string _projectName, CacheManager& _cacheManager, const std::string& _assetFilename, const std::string& _assetInputDirectory, const std::string& _assetOutputDirectory, const SourceAssetIdToCompiledAssetId& _sourceAssetIdToCompiledAssetId, const SourceAssetIdToAbsoluteFilename& _sourceAssetIdToAbsoluteFilename) :
				context(_context),
				projectName(_projectName),
				cacheManager(_cacheManager),
				assetFilename(_assetFilename),
				assetInputDirectory(_assetInputDirectory),
				assetOutputDirectory(_assetOutputDirectory),
				sourceAssetIdToCompiledAssetId(_sourceAssetIdToCompiledAssetId),
				sourceAssetIdToAbsoluteFilename(_sourceAssetIdToAbsoluteFilename)
			{
				// Nothing here
			}
			uint32_t getCompiledAssetIdBySourceAssetId(uint32_t sourceAssetId) const
			{
				SourceAssetIdToCompiledAssetId::const_iterator iterator = sourceAssetIdToCompiledAssetId.find(sourceAssetId);
				if (iterator == sourceAssetIdToCompiledAssetId.cend())
				{
					throw std::runtime_error(std::string("Source asset ID ") + std::to_string(sourceAssetId) + " is unknown");
				}
				return iterator->second;
			}
			uint32_t getCompiledAssetIdBySourceAssetIdAsString(const std::string& sourceAssetIdAsString) const
			{
				SourceAssetIdToCompiledAssetId::const_iterator iterator = sourceAssetIdToCompiledAssetId.find(RendererRuntime::StringId(sourceAssetIdAsString.c_str()));
				if (iterator == sourceAssetIdToCompiledAssetId.cend())
				{
					throw std::runtime_error(std::string("Source asset ID \"") + sourceAssetIdAsString + "\" is unknown");
				}
				return iterator->second;
			}

			Input(const Input&) = delete;
			Input& operator=(const Input&) = delete;
		};
		struct Configuration
		{
			const rapidjson::Document& rapidJsonDocumentAsset;
			const rapidjson::Value&    rapidJsonValueTargets;
			std::string				   rendererTarget;
			QualityStrategy			   qualityStrategy;
			Configuration(const rapidjson::Document& _rapidJsonDocumentAsset, const rapidjson::Value& _rapidJsonValueTargets, const std::string& _rendererTarget, QualityStrategy _qualityStrategy) :
				rapidJsonDocumentAsset(_rapidJsonDocumentAsset),
				rapidJsonValueTargets(_rapidJsonValueTargets),
				rendererTarget(_rendererTarget),
				qualityStrategy(_qualityStrategy)
			{
				// Nothing here
			}
			Configuration(const Configuration&) = delete;
			Configuration& operator =(const Configuration&) = delete;
		};
		struct Output
		{
			RendererRuntime::AssetPackage* outputAssetPackage;
		};


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		static void outputAsset(const std::string& assetIdAsString, const std::string& outputAssetFilename, RendererRuntime::AssetPackage& outputAssetPackage);


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	public:
		virtual AssetCompilerTypeId getAssetCompilerTypeId() const = 0;
		virtual void compile(const Input& input, const Configuration& configuration, Output& output) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline IAssetCompiler() {}
		inline virtual ~IAssetCompiler() override {}


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef Renderer::SmartRefCount<IAssetCompiler> IAssetCompilerPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
