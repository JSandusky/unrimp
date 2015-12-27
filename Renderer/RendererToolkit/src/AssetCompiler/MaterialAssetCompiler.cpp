/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "RendererToolkit/AssetCompiler/MaterialAssetCompiler.h"
#include "RendererToolkit/Helper/JsonMaterialBlueprintHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Resource/Material/Loader/MaterialFileFormat.h>

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
	#include <Poco/File.h>
#pragma warning(pop)

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	namespace detail
	{
		struct OrderByMaterialPropertyId
		{
			inline bool operator()(const RendererRuntime::MaterialProperty& left, RendererRuntime::MaterialPropertyId right) const
			{
				return (left.getMaterialPropertyId() < right);
			}

			inline bool operator()(RendererRuntime::MaterialPropertyId left, const RendererRuntime::MaterialProperty& right) const
			{
				return (left < right.getMaterialPropertyId());
			}
		};
	}


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const AssetCompilerTypeId MaterialAssetCompiler::TYPE_ID("Material");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	MaterialAssetCompiler::MaterialAssetCompiler()
	{
	}

	MaterialAssetCompiler::~MaterialAssetCompiler()
	{
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId MaterialAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	void MaterialAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Input, configuration and output
		const std::string&			   assetInputDirectory	= input.assetInputDirectory;
		const std::string&			   assetOutputDirectory	= input.assetOutputDirectory;
		Poco::JSON::Object::Ptr		   jsonAssetRootObject	= configuration.jsonAssetRootObject;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		Poco::JSON::Object::Ptr jsonAssetObject = jsonAssetRootObject->get("Asset").extract<Poco::JSON::Object::Ptr>();

		// Read configuration
		// TODO(co) Add required properties
		std::string inputFile;
		uint32_t test = 0;
		{
			// Read material asset compiler configuration
			Poco::JSON::Object::Ptr jsonConfigurationObject = jsonAssetObject->get("MaterialAssetCompiler").extract<Poco::JSON::Object::Ptr>();
			inputFile = jsonConfigurationObject->getValue<std::string>("InputFile");
			test	  = jsonConfigurationObject->optValue<uint32_t>("Test", test);
		}

		// Open the input file
		std::ifstream inputFileStream(assetInputDirectory + inputFile, std::ios::binary);
		const std::string assetName = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetName");
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".material";
		std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

		{ // Material
			// Parse JSON
			Poco::JSON::Parser jsonParser;
			jsonParser.parse(inputFileStream);
			Poco::JSON::Object::Ptr jsonRootObject = jsonParser.result().extract<Poco::JSON::Object::Ptr>();

			{ // Check whether or not the file format matches
				Poco::JSON::Object::Ptr jsonFormatObject = jsonRootObject->get("Format").extract<Poco::JSON::Object::Ptr>();
				if (jsonFormatObject->get("Type").convert<std::string>() != "MaterialAsset")
				{
					throw std::exception("Invalid JSON format type, must be \"MaterialAsset\"");
				}
				if (jsonFormatObject->get("Version").convert<uint32_t>() != 1)
				{
					throw std::exception("Invalid JSON format version, must be 1");
				}
			}

			Poco::JSON::Object::Ptr jsonMaterialObject = jsonRootObject->get("MaterialAsset").extract<Poco::JSON::Object::Ptr>();
			Poco::JSON::Object::Ptr jsonPropertiesObject = jsonMaterialObject->get("Properties").extract<Poco::JSON::Object::Ptr>();

			// Gather all material blueprint properties
			RendererRuntime::MaterialBlueprintResource::SortedMaterialPropertyVector sortedMaterialPropertyVector;
			{
				// TODO(co) Error handling and simplification
				// Parse material blueprint asset JSON
				const std::string absoluteMaterialBlueprintAssetFilename = JsonHelper::getAbsoluteAssetFilename(input, jsonMaterialObject, "MaterialBlueprintAssetId");
				std::ifstream materialBlueprintAssetInputFileStream(absoluteMaterialBlueprintAssetFilename, std::ios::binary);
				Poco::JSON::Parser materialBlueprintAssetJsonParser;
				materialBlueprintAssetJsonParser.parse(materialBlueprintAssetInputFileStream);

				// Read configuration
				std::string materialBlueprintInputFile;
				{
					// Read material blueprint asset compiler configuration
					Poco::JSON::Object::Ptr jsonConfigurationObject = materialBlueprintAssetJsonParser.result().extract<Poco::JSON::Object::Ptr>()->get("Asset").extract<Poco::JSON::Object::Ptr>()->get("MaterialBlueprintAssetCompiler").extract<Poco::JSON::Object::Ptr>();
					materialBlueprintInputFile = jsonConfigurationObject->getValue<std::string>("InputFile");
				}

				// Parse material blueprint JSON
				std::ifstream materialBlueprintInputFileStream(Poco::Path(absoluteMaterialBlueprintAssetFilename).parent().toString(Poco::Path::PATH_UNIX) + materialBlueprintInputFile, std::ios::binary);
				Poco::JSON::Parser materialBlueprintJsonParser;
				materialBlueprintJsonParser.parse(materialBlueprintInputFileStream);
				Poco::JSON::Object::Ptr materialBlueprintJsonRootObject = materialBlueprintJsonParser.result().extract<Poco::JSON::Object::Ptr>();
				Poco::JSON::Object::Ptr jsonMaterialBlueprintObject = materialBlueprintJsonRootObject->get("MaterialBlueprintAsset").extract<Poco::JSON::Object::Ptr>();
				JsonMaterialBlueprintHelper::readProperties(input, jsonMaterialBlueprintObject->get("Properties").extract<Poco::JSON::Object::Ptr>(), sortedMaterialPropertyVector);
			}

			{ // Material header
				RendererRuntime::v1Material::Header materialHeader;
				materialHeader.formatType				= RendererRuntime::v1Material::FORMAT_TYPE;
				materialHeader.formatVersion			= RendererRuntime::v1Material::FORMAT_VERSION;
				materialHeader.materialBlueprintAssetId	= JsonHelper::getCompiledAssetId(input, jsonMaterialObject, "MaterialBlueprintAssetId");
				materialHeader.numberOfProperties		= sortedMaterialPropertyVector.size();

				// Write down the material header
				outputFileStream.write(reinterpret_cast<const char*>(&materialHeader), sizeof(RendererRuntime::v1Material::Header));
			}

			{ // Properties
				// Update material property values were required
				Poco::JSON::Object::ConstIterator propertiesIterator = jsonPropertiesObject->begin();
				Poco::JSON::Object::ConstIterator propertiesIteratorEnd = jsonPropertiesObject->end();
				while (propertiesIterator != propertiesIteratorEnd)
				{
					// Material property ID
					const std::string propertyName = propertiesIterator->first;
					const RendererRuntime::MaterialPropertyId materialPropertyId(propertyName.c_str());

					// Figure out the material property value type by using the material blueprint
					RendererRuntime::MaterialBlueprintResource::SortedMaterialPropertyVector::const_iterator iterator = std::lower_bound(sortedMaterialPropertyVector.cbegin(), sortedMaterialPropertyVector.cend(), materialPropertyId, detail::OrderByMaterialPropertyId());
					if (iterator != sortedMaterialPropertyVector.end())
					{
						RendererRuntime::MaterialProperty* materialProperty = iterator._Ptr;
						if (materialProperty->getMaterialPropertyId() == materialPropertyId)
						{
							// Set the material own property value
							static_cast<RendererRuntime::MaterialPropertyValue&>(*materialProperty) = JsonMaterialBlueprintHelper::mandatoryMaterialPropertyValue(input, jsonPropertiesObject, propertyName, materialProperty->getValueType());
						}
					}

					// Next property, please
					++propertiesIterator;
				}

				// Write down all material properties
				outputFileStream.write(reinterpret_cast<const char*>(sortedMaterialPropertyVector.data()), sizeof(RendererRuntime::MaterialProperty) * sortedMaterialPropertyVector.size());
			}
		}

		{ // Update the output asset package
			const std::string assetCategory = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetCategory");
			const std::string assetIdAsString = input.projectName + "/Material/" + assetCategory + '/' + assetName;

			// Output asset
			RendererRuntime::Asset outputAsset;
			outputAsset.assetId = RendererRuntime::StringId(assetIdAsString.c_str());
			strcpy(outputAsset.assetFilename, outputAssetFilename.c_str());	// TODO(co) Buffer overflow test
			outputAssetPackage.getWritableSortedAssetVector().push_back(outputAsset);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
