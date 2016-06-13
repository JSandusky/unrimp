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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/AssetCompiler/IAssetCompiler.h"

#include <RendererRuntime/Core/NonCopyable.h>
#include <RendererRuntime/Resource/Material/MaterialProperties.h>

#include <rapidjson/fwd.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class ShaderProperties;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class JsonMaterialBlueprintHelper : public RendererRuntime::NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		static void optionalShaderVisibilityProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::ShaderVisibility& value);
		static RendererRuntime::MaterialProperty::Usage mandatoryMaterialPropertyUsage(const rapidjson::Value& rapidJsonValue);
		static RendererRuntime::MaterialProperty::ValueType mandatoryMaterialPropertyValueType(const rapidjson::Value& rapidJsonValue);
		static RendererRuntime::MaterialPropertyValue mandatoryMaterialPropertyValue(const IAssetCompiler::Input& input, const rapidjson::Value& rapidJsonValue, const char* propertyName, const RendererRuntime::MaterialProperty::ValueType valueType);
		static void readRootSignature(const rapidjson::Value& rapidJsonValueRootSignature, std::ofstream& outputFileStream, RendererRuntime::ShaderProperties& shaderProperties);
		static void readProperties(const IAssetCompiler::Input& input, const rapidjson::Value& rapidJsonValueProperties, RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector, bool sort = true);
		static void readPipelineStateObject(const IAssetCompiler::Input& input, const rapidjson::Value& rapidJsonValuePipelineState, std::ofstream& outputFileStream);
		static void readUniformBuffers(const IAssetCompiler::Input& input, const rapidjson::Value& rapidJsonValueUniformBuffers, std::ofstream& outputFileStream, RendererRuntime::ShaderProperties& shaderProperties);
		static void readSamplerStates(const rapidjson::Value& rapidJsonValueSamplerStates, std::ofstream& outputFileStream, RendererRuntime::ShaderProperties& shaderProperties);
		static void readTextures(const IAssetCompiler::Input& input, const RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector, const rapidjson::Value& rapidJsonValueTextures, std::ofstream& outputFileStream, RendererRuntime::ShaderProperties& shaderProperties);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		JsonMaterialBlueprintHelper(const JsonMaterialBlueprintHelper&) = delete;
		JsonMaterialBlueprintHelper& operator=(const JsonMaterialBlueprintHelper&) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
