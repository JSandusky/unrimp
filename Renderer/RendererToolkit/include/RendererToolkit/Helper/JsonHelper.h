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
#include "RendererToolkit/AssetCompiler/IAssetCompiler.h"

#include <RendererRuntime/Core/NonCopyable.h>

#include <rapidjson/fwd.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	#include <glm/fwd.hpp>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Global definitions                                    ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	typedef StringId AssetId;	///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>"
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class JsonHelper : public RendererRuntime::NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		static void parseDocumentByInputFileStream(rapidjson::Document& rapidJsonDocument, std::ifstream& inputFileStream, const std::string& inputFilename, const std::string& formatType, const std::string& formatVersion);
		static void optionalBooleanProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, bool& value);
		static void optionalBooleanProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, int& value);
		static void optionalByteProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, uint8_t& value);
		static void optionalIntegerProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, int& value);
		static void optionalIntegerProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, unsigned int& value);
		static void optionalIntegerNProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, int value[], uint32_t numberOfComponents);
		static void optionalFloatProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, float& value);
		static void optionalFloatNProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, float value[], uint32_t numberOfComponents);
		static void optionalRotationQuaternionProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, glm::quat& value);
		static void optionalStringProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, char* value, uint32_t maximumLength);
		static void optionalStringNProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, std::string value[], uint32_t numberOfComponents, char separator = ' ');
		static void mandatoryStringIdProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, RendererRuntime::StringId& value);
		static void optionalStringIdProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, RendererRuntime::StringId& value);
		static void mandatoryAssetIdProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, RendererRuntime::AssetId& value);
		static void optionalAssetIdProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, RendererRuntime::AssetId& value);
		static void optionalClearFlagsProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, uint32_t& clearFlags);
		static void optionalCompiledAssetId(const IAssetCompiler::Input& input, const rapidjson::Value& rapidJsonValue, const char* propertyName, RendererRuntime::AssetId& compiledAssetId);
		static RendererRuntime::AssetId getCompiledAssetId(const IAssetCompiler::Input& input, const rapidjson::Value& rapidJsonValue, const char* propertyName);
		static Renderer::TextureFormat::Enum mandatoryTextureFormat(const rapidjson::Value& rapidJsonValue);
		static std::string getAbsoluteAssetFilename(const IAssetCompiler::Input& input, uint32_t sourceAssetId);	// TODO(co) Move this elsewhere? (not JSON related)


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		JsonHelper(const JsonHelper&) = delete;
		JsonHelper& operator=(const JsonHelper&) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
