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
#include "RendererRuntime/Resource/Detail/IResource.h"
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/ShaderProperties.h"

#include <string>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class ShaderBlueprintResource;
	template <class ELEMENT_TYPE, typename ID_TYPE, uint32_t MAXIMUM_NUMBER_OF_ELEMENTS> class PackedElementManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t ShaderPieceResourceId;		///< POD shader piece resource identifier
	typedef uint32_t ShaderBlueprintResourceId;	///< POD shader blueprint resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Shader blueprint resource
	*/
	class ShaderBlueprintResource : public IResource
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend PackedElementManager<ShaderBlueprintResource, ShaderBlueprintResourceId, 64>;	// Type definition of template class
		friend class ShaderBlueprintResourceLoader;
		friend class ShaderBlueprintResourceManager;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::vector<ShaderPieceResourceId> IncludeShaderPieceResourceIds;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the IDs of the shader piece resources to include
		*
		*  @return
		*    The IDs of the shader piece resources to include
		*/
		inline const IncludeShaderPieceResourceIds& getIncludeShaderPieceResourceIds() const;

		/**
		*  @brief
		*    Return the referenced shader properties
		*
		*  @return
		*    The referenced shader properties
		*/
		inline const ShaderProperties& getReferencedShaderProperties() const;

		/**
		*  @brief
		*    Return the shader source code
		*
		*  @return
		*    The shader ASCII source code
		*/
		inline const std::string& getShaderSourceCode() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline ShaderBlueprintResource();
		inline virtual ~ShaderBlueprintResource();
		ShaderBlueprintResource(const ShaderBlueprintResource&) = delete;
		ShaderBlueprintResource& operator=(const ShaderBlueprintResource&) = delete;

		//[-------------------------------------------------------]
		//[ "RendererRuntime::PackedElementManager" management    ]
		//[-------------------------------------------------------]
		inline void initializeElement(ShaderBlueprintResourceId shaderBlueprintResourceId);
		inline void deinitializeElement();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IncludeShaderPieceResourceIds mIncludeShaderPieceResourceIds;
		ShaderProperties			  mReferencedShaderProperties;	// Directly use "RendererRuntime::ShaderProperties" to keep things simple, although we don't need a shader property value
		std::string					  mShaderSourceCode;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResource.inl"
