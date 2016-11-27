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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/Helper/JsonHelper.h"
#include "RendererToolkit/Helper/StringHelper.h"

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4464)	// warning C4464: relative include path contains '..'
	#pragma warning(disable: 4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#pragma warning(disable: 4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	#pragma warning(disable: 4619)	// warning C4619: #pragma warning: there is no warning number '4351'
	#pragma warning(disable: 4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#pragma warning(disable: 4061)	// warning C4061: enumerator 'rapidjson::GenericReader<rapidjson::UTF8<char>,rapidjson::UTF8<char>,rapidjson::CrtAllocator>::IterativeParsingStartState' in switch of enum 'rapidjson::GenericReader<rapidjson::UTF8<char>,rapidjson::UTF8<char>,rapidjson::CrtAllocator>::IterativeParsingState' is not explicitly handled by a case label
	#pragma warning(disable: 4770)	// warning C4770: partially validated enum 'type' used as index"
	#include <rapidjson/document.h>
	#include <rapidjson/istreamwrapper.h>
	#include <rapidjson/error/en.h>
#pragma warning(pop)

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	void JsonHelper::parseDocumentByInputFileStream(rapidjson::Document& rapidJsonDocument, std::ifstream& inputFileStream, const std::string& inputFilename, const std::string& formatType, const std::string& formatVersion)
	{
		rapidjson::IStreamWrapper rapidJsonIStreamWrapper(inputFileStream);
		const rapidjson::ParseResult rapidJsonParseResult = rapidJsonDocument.ParseStream(rapidJsonIStreamWrapper);
		if (rapidJsonParseResult.Code() != rapidjson::kParseErrorNone)
		{
			// Get the line number
			// TODO(co) This is nuts. There must be a much simpler solution to get the line number.
			std::string fileContent;
			{
				inputFileStream.seekg(0, std::ifstream::end);
				const std::streampos numberOfBytes = inputFileStream.tellg();
				inputFileStream.seekg(0, std::ifstream::beg);
				fileContent.resize(static_cast<size_t>(numberOfBytes));
				inputFileStream.read(const_cast<char*>(fileContent.c_str()), numberOfBytes);
			}
			const std::streamoff lineNumber = std::count(fileContent.begin(), fileContent.begin() + static_cast<std::streamoff>(rapidJsonParseResult.Offset()), '\n');

			// Throw exception with human readable error message
			throw std::runtime_error("Failed to parse JSON file \"" + inputFilename + "\": " + rapidjson::GetParseError_En(rapidJsonParseResult.Code()) + " (line " + std::to_string(lineNumber) + ')');
		}

		{ // Mandatory format header: Check whether or not the file format matches
			const rapidjson::Value& rapidJsonValueFormat = rapidJsonDocument["Format"];
			if (formatType != rapidJsonValueFormat["Type"].GetString())
			{
				throw std::runtime_error("Invalid JSON format type, must be \"" + formatType + "\"");
			}
			if (formatVersion != rapidJsonValueFormat["Version"].GetString())
			{
				throw std::runtime_error("Invalid JSON format version, must be " + formatVersion);
			}
		}
	}

	void JsonHelper::optionalBooleanProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, bool& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const rapidjson::Value& rapidJsonValueValue = rapidJsonValue[propertyName];
			const char* valueAsString = rapidJsonValueValue.GetString();
			const rapidjson::SizeType valueStringLength = rapidJsonValueValue.GetStringLength();

			if (strncmp(valueAsString, "FALSE", valueStringLength) == 0)
			{
				value = false;
			}
			else if (strncmp(valueAsString, "TRUE", valueStringLength) == 0)
			{
				value = true;
			}
			else
			{
				throw std::runtime_error(std::string("The value of property \"") + propertyName + "\" is \"" + valueAsString + "\", but it must be \"FALSE\" or \"TRUE\"");
			}
		}
	}

	void JsonHelper::optionalBooleanProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, int& value)
	{
		bool booleanValue = (0 != value);
		optionalBooleanProperty(rapidJsonValue, propertyName, booleanValue);
		value = booleanValue;
	}

	void JsonHelper::optionalByteProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, uint8_t& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const int integerValue = std::atoi(rapidJsonValue[propertyName].GetString());
			if (integerValue < 0)
			{
				throw std::runtime_error(std::string("The value of property \"") + propertyName + "\" can't be negative");
			}
			if (integerValue > 255)
			{
				throw std::runtime_error(std::string("The value of property \"") + propertyName + "\" can't be above 255");
			}
			value = static_cast<uint8_t>(integerValue);
		}
	}

	void JsonHelper::optionalIntegerProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, int& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			value = std::atoi(rapidJsonValue[propertyName].GetString());
		}
	}

	void JsonHelper::optionalIntegerProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, unsigned int& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			value = static_cast<unsigned int>(std::atoi(rapidJsonValue[propertyName].GetString()));
		}
	}

	void JsonHelper::optionalIntegerNProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, int value[], uint32_t numberOfComponents)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			std::vector<std::string> elements;
			RendererToolkit::StringHelper::splitString(rapidJsonValue[propertyName].GetString(), ' ', elements);
			if (elements.size() == numberOfComponents)
			{
				for (size_t i = 0; i < numberOfComponents; ++i)
				{
					value[i] = std::atoi(elements[i].c_str());
				}
			}
			else
			{
				// TODO(co) Error handling
			}
		}
	}

	void JsonHelper::optionalFloatProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, float& value)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			value = std::stof(rapidJsonValue[propertyName].GetString());
		}
	}

	void JsonHelper::optionalFloatNProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, float value[], uint32_t numberOfComponents)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			std::vector<std::string> elements;
			RendererToolkit::StringHelper::splitString(rapidJsonValue[propertyName].GetString(), ' ', elements);
			if (elements.size() == numberOfComponents)
			{
				for (size_t i = 0; i < numberOfComponents; ++i)
				{
					value[i] = std::stof(elements[i].c_str());
				}
			}
			else
			{
				// TODO(co) Error handling
			}
		}
	}

	void JsonHelper::optionalStringProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, char* value, uint32_t maximumLength)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			const rapidjson::Value& rapidJsonValueFound = rapidJsonValue[propertyName];
			const char* valueAsString = rapidJsonValueFound.GetString();
			const rapidjson::SizeType valueLength = rapidJsonValueFound.GetStringLength();

			// +1 for the terminating zero
			if (valueLength + 1 <= maximumLength)
			{
				memcpy(value, valueAsString, valueLength);
				value[valueLength] = '\0';
			}
			else
			{
				// TODO(co) Error handling
			}
		}
	}

	void JsonHelper::optionalStringNProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, std::string value[], uint32_t numberOfComponents, char separator)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			std::vector<std::string> elements;
			RendererToolkit::StringHelper::splitString(rapidJsonValue[propertyName].GetString(), separator, elements);
			if (elements.size() == numberOfComponents)
			{
				for (size_t i = 0; i < numberOfComponents; ++i)
				{
					value[i] = elements[i];
				}
			}
			else
			{
				// TODO(co) Error handling
			}
		}
	}

	void JsonHelper::optionalCompiledAssetId(const IAssetCompiler::Input& input, const rapidjson::Value& rapidJsonValue, const char* propertyName, RendererRuntime::AssetId& compiledAssetId)
	{
		if (rapidJsonValue.HasMember(propertyName))
		{
			compiledAssetId = input.getCompiledAssetIdBySourceAssetId(static_cast<uint32_t>(std::atoi(rapidJsonValue[propertyName].GetString())));
		}
	}

	RendererRuntime::AssetId JsonHelper::getCompiledAssetId(const IAssetCompiler::Input& input, const rapidjson::Value& rapidJsonValue, const char* propertyName)
	{
		return input.getCompiledAssetIdBySourceAssetId(static_cast<uint32_t>(std::atoi(rapidJsonValue[propertyName].GetString())));
	}

	Renderer::TextureFormat::Enum JsonHelper::mandatoryTextureFormat(const rapidjson::Value& rapidJsonValue)
	{
		const rapidjson::Value& rapidJsonValueUsage = rapidJsonValue["TextureFormat"];
		const char* valueAsString = rapidJsonValueUsage.GetString();
		const rapidjson::SizeType valueStringLength = rapidJsonValueUsage.GetStringLength();

		// Define helper macros
		#define IF_VALUE(name)			 if (strncmp(valueAsString, #name, valueStringLength) == 0) return Renderer::TextureFormat::name;
		#define ELSE_IF_VALUE(name) else if (strncmp(valueAsString, #name, valueStringLength) == 0) return Renderer::TextureFormat::name;

		// Evaluate value
		IF_VALUE(A8)
		ELSE_IF_VALUE(R8G8B8)
		ELSE_IF_VALUE(R8G8B8A8)
		ELSE_IF_VALUE(R16G16B16A16F)
		ELSE_IF_VALUE(R32G32B32A32F)
		ELSE_IF_VALUE(BC1)
		ELSE_IF_VALUE(BC2)
		ELSE_IF_VALUE(BC3)
		ELSE_IF_VALUE(BC4)
		ELSE_IF_VALUE(BC5)
		ELSE_IF_VALUE(ETC1)
		ELSE_IF_VALUE(D32_FLOAT)
		ELSE_IF_VALUE(UNKNOWN)
		else
		{
			// TODO(co) Error handling
			return Renderer::TextureFormat::UNKNOWN;
		}

		// Undefine helper macros
		#undef IF_VALUE
		#undef ELSE_IF_VALUE
	}

	std::string JsonHelper::getAbsoluteAssetFilename(const IAssetCompiler::Input& input, uint32_t sourceAssetId)
	{
		SourceAssetIdToAbsoluteFilename::const_iterator iterator = input.sourceAssetIdToAbsoluteFilename.find(sourceAssetId);
		const std::string absoluteFilename = (iterator != input.sourceAssetIdToAbsoluteFilename.cend()) ? iterator->second : "";
		// TODO(co) Error handling: Compiled asset ID not found (meaning invalid source asset ID given)
		return absoluteFilename;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
