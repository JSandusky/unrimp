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
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderProperties.h"

#include <map>
#include <string>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class ShaderBlueprintResource;
	class ShaderPieceResourceManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef std::map<StringId, std::string>	DynamicShaderPieces;	// TODO(co) Unordered map might perform better


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Shader builder
	*
	*  @notes
	*   - Heavily basing on the OGRE 2.1 HLMS shader builder which is directly part of the OGRE class "Ogre::Hlms". So for syntax, have a look into the OGRE 2.1 documentation.
	*/
	class ShaderBuilder
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*/
		ShaderBuilder();

		/**
		*  @brief
		*    Destructor
		*/
		~ShaderBuilder();

		/**
		*  @brief
		*    Create shader source code by using the given shader blueprint and shader properties
		*
		*  @param[in] shaderPieceResourceManager
		*    Shader piece resource manager to use
		*  @param[in] shaderBlueprintResource
		*    Shader blueprint resource to use
		*  @param[in] shaderProperties
		*    Shader properties to use
		*
		*  @return
		*    The created shader source code
		*/
		const std::string& createSourceCode(const ShaderPieceResourceManager& shaderPieceResourceManager, const ShaderBlueprintResource& shaderBlueprintResource, const ShaderProperties& shaderProperties);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		ShaderBuilder(const ShaderBuilder&) = delete;
		ShaderBuilder& operator=(const ShaderBuilder&) = delete;
		bool parseMath(const std::string& inBuffer, std::string& outBuffer);
		bool parseForEach(const std::string& inBuffer, std::string& outBuffer) const;
		bool parseProperties(std::string& inBuffer, std::string& outBuffer) const;
		bool collectPieces(const std::string& inBuffer, std::string& outBuffer);
		bool insertPieces(std::string& inBuffer, std::string& outBuffer) const;
		bool parseCounter(const std::string& inBuffer, std::string& outBuffer);
		bool parse(const std::string& inBuffer, std::string& outBuffer) const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		ShaderProperties	mShaderProperties;
		DynamicShaderPieces	mDynamicShaderPieces;
		std::string			mInString;	///< Could be a local variable, but when making it to a member we reduce memory allocations
		std::string			mOutString;	///< Could be a local variable, but when making it to a member we reduce memory allocations


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
