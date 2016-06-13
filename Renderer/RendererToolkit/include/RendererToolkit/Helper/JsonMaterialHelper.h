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
#include <RendererRuntime/Core/NonCopyable.h>

#include <Renderer/Public/Renderer.h>

#include <rapidjson/fwd.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class JsonMaterialHelper : public RendererRuntime::NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		static void optionalFillModeProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::FillMode& value);
		static void optionalCullModeProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::CullMode& value);
		static void optionalConservativeRasterizationModeProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::ConservativeRasterizationMode& value);
		static void optionalDepthWriteMaskProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::DepthWriteMask& value);
		static void optionalStencilOpProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::StencilOp& value);
		static void optionalBlendProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::Blend& value);
		static void optionalBlendOpProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::BlendOp& value);
		static void optionalFilterProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::FilterMode& value);
		static void optionalTextureAddressModeProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::TextureAddressMode& value);
		static void optionalComparisonFuncProperty(const rapidjson::Value& rapidJsonValue, const char* propertyName, Renderer::ComparisonFunc& value);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		JsonMaterialHelper(const JsonMaterialHelper&) = delete;
		JsonMaterialHelper& operator=(const JsonMaterialHelper&) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
