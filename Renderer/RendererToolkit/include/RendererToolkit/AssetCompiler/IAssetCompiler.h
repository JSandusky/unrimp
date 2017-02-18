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


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class IAssetCompiler : public Renderer::RefCount<IAssetCompiler>
	{


	//[-------------------------------------------------------]
	//[ Public structures                                     ]
	//[-------------------------------------------------------]
	public:
		struct Input
		{
			std::string							  projectName;
			std::string							  assetInputDirectory;
			std::string							  assetOutputDirectory;
			const SourceAssetIdToCompiledAssetId& sourceAssetIdToCompiledAssetId;
			const SourceAssetIdToAbsoluteFilename& sourceAssetIdToAbsoluteFilename;

			Input() = delete;
			Input(const std::string _projectName, const std::string& _assetInputDirectory, const std::string& _assetOutputDirectory, const SourceAssetIdToCompiledAssetId& _sourceAssetIdToCompiledAssetId, const SourceAssetIdToAbsoluteFilename& _sourceAssetIdToAbsoluteFilename) :
				projectName(_projectName),
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
			Input(const Input&) = delete;
			Input& operator=(const Input&) = delete;
		};
		struct Configuration
		{
			const rapidjson::Document& rapidJsonDocumentAsset;
			const rapidjson::Value&    rapidJsonValueTargets;
			std::string				   rendererTarget;
			Configuration(const rapidjson::Document& _rapidJsonDocumentAsset, const rapidjson::Value& _rapidJsonValueTargets, const std::string &_rendererTarget) :
				rapidJsonDocumentAsset(_rapidJsonDocumentAsset),
				rapidJsonValueTargets(_rapidJsonValueTargets),
				rendererTarget(_rendererTarget)
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
		inline virtual ~IAssetCompiler() {}


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef Renderer::SmartRefCount<IAssetCompiler> IAssetCompilerPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
