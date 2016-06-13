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

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4251)	// warning C4251: 'Poco::StringTokenizer::_tokens': class 'std::vector<std::string,std::allocator<_Kty>>' needs to have dll-interface to be used by clients of class 'Poco::StringTokenizer'
	#include <Poco/StringTokenizer.h>
#pragma warning(pop)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	void JsonHelper::optionalBooleanProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, int& value)
	{
		Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
		if (!jsonDynamicVar.isEmpty())
		{
			const std::string valueAsString = jsonDynamicVar.convert<std::string>();

			if ("FALSE" == valueAsString)
			{
				value = 0;
			}
			else if ("TRUE" == valueAsString)
			{
				value = 1;
			}
			else
			{
				// TODO(co) Error handling
			}
		}
	}

	void JsonHelper::optionalIntegerProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, int& value)
	{
		Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
		if (!jsonDynamicVar.isEmpty())
		{
			const std::string valueAsString = jsonDynamicVar.convert<std::string>();
			value = std::atoi(valueAsString.c_str());
		}
	}

	void JsonHelper::optionalIntegerProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, unsigned int& value)
	{
		Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
		if (!jsonDynamicVar.isEmpty())
		{
			const std::string valueAsString = jsonDynamicVar.convert<std::string>();
			value = static_cast<unsigned int>(std::atoi(valueAsString.c_str()));
		}
	}

	void JsonHelper::optionalIntegerNProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, int value[], uint32_t numberOfComponents)
	{
		Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
		if (!jsonDynamicVar.isEmpty())
		{
			Poco::StringTokenizer stringTokenizer(jsonDynamicVar.convert<std::string>(), " ");
			if (stringTokenizer.count() == numberOfComponents)
			{
				for (size_t i = 0; i < numberOfComponents; ++i)
				{
					value[i] = std::atoi(stringTokenizer[i].c_str());
				}
			}
			else
			{
				// TODO(co) Error handling
			}
		}
	}

	void JsonHelper::optionalFloatProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, float& value)
	{
		Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
		if (!jsonDynamicVar.isEmpty())
		{
			const std::string valueAsString = jsonDynamicVar.convert<std::string>();
			value = std::stof(valueAsString.c_str());
		}
	}

	void JsonHelper::optionalFloatNProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, float value[], uint32_t numberOfComponents)
	{
		Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
		if (!jsonDynamicVar.isEmpty())
		{
			Poco::StringTokenizer stringTokenizer(jsonDynamicVar.convert<std::string>(), " ");
			if (stringTokenizer.count() == numberOfComponents)
			{
				for (size_t i = 0; i < numberOfComponents; ++i)
				{
					value[i] = std::stof(stringTokenizer[i].c_str());
				}
			}
			else
			{
				// TODO(co) Error handling
			}
		}
	}

	void JsonHelper::optionalStringProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, char* value, uint32_t maximumLength)
	{
		Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
		if (!jsonDynamicVar.isEmpty())
		{
			const std::string valueAsString = jsonDynamicVar.convert<std::string>();
			const size_t valueLength = valueAsString.length();

			// +1 for the terminating zero
			if (valueLength + 1 <= maximumLength)
			{
				memcpy(value, valueAsString.data(), valueLength);
				value[valueLength] = '\0';
			}
			else
			{
				// TODO(co) Error handling
			}
		}
	}

	void JsonHelper::optionalStringNProperty(Poco::JSON::Object::Ptr jsonObject, const std::string& propertyName, std::string value[], uint32_t numberOfComponents, const std::string& separator)
	{
		Poco::Dynamic::Var jsonDynamicVar = jsonObject->get(propertyName);
		if (!jsonDynamicVar.isEmpty())
		{
			Poco::StringTokenizer stringTokenizer(jsonDynamicVar.convert<std::string>(), separator);
			if (stringTokenizer.count() == numberOfComponents)
			{
				for (size_t i = 0; i < numberOfComponents; ++i)
				{
					value[i] = stringTokenizer[i];
				}
			}
			else
			{
				// TODO(co) Error handling
			}
		}
	}

	uint32_t JsonHelper::getCompiledAssetId(const IAssetCompiler::Input& input, Poco::JSON::Object::Ptr jsonShaderBlueprintsObject, const std::string& propertyName)
	{
		return input.getCompiledAssetIdBySourceAssetId(static_cast<uint32_t>(std::atoi(jsonShaderBlueprintsObject->get(propertyName).convert<std::string>().c_str())));
	}

	std::string JsonHelper::getAbsoluteAssetFilename(const IAssetCompiler::Input& input, uint32_t sourceAssetId)
	{
		SourceAssetIdToAbsoluteFilename::const_iterator iterator = input.sourceAssetIdToAbsoluteFilename.find(sourceAssetId);
		const std::string absoluteFilename = (iterator != input.sourceAssetIdToAbsoluteFilename.cend()) ? iterator->second : "";
		// TODO(co) Error handling: Compiled asset ID not found (meaning invalid source asset ID given)
		return absoluteFilename;
	}

	std::string JsonHelper::getAbsoluteAssetFilename(const IAssetCompiler::Input& input, Poco::JSON::Object::Ptr jsonShaderBlueprintsObject, const std::string& propertyName)
	{
		const uint32_t sourceAssetId = static_cast<uint32_t>(std::atoi(jsonShaderBlueprintsObject->get(propertyName).convert<std::string>().c_str()));
		SourceAssetIdToAbsoluteFilename::const_iterator iterator = input.sourceAssetIdToAbsoluteFilename.find(sourceAssetId);
		const std::string absoluteFilename = (iterator != input.sourceAssetIdToAbsoluteFilename.cend()) ? iterator->second : "";
		// TODO(co) Error handling: Compiled asset ID not found (meaning invalid source asset ID given)
		return absoluteFilename;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
