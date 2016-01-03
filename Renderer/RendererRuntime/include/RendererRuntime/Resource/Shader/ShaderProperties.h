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
#include "RendererRuntime/Core/StringId.h"

#include <vector>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId ShaderPropertyId;	///< Shader property identifier, internally just a POD "uint32_t", result of hashing the property name


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Shader properties
	*/
	class ShaderProperties
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		struct Property
		{
			ShaderPropertyId shaderPropertyId;
			int32_t			 value;

			Property(StringId _shaderPropertyId, int32_t _value) :
				shaderPropertyId(_shaderPropertyId),
				value(_value)
			{
				// Nothing here
			}

			bool operator ==(const Property& property) const
			{
				return (shaderPropertyId == property.shaderPropertyId && value == property.value);
			}
		};

		typedef std::vector<Property> SortedPropertyVector;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*/
		inline ShaderProperties();

		/**
		*  @brief
		*    Destructor
		*/
		inline ~ShaderProperties();

		/**
		*  @brief
		*    Return the properties
		*
		*  @return
		*    The properties
		*/
		inline const SortedPropertyVector& getSortedPropertyVector() const;

		/**
		*  @brief
		*    Return the value of a property
		*
		*  @param[in] shaderPropertyId
		*    ID of the shader property to return the value from
		*  @param[out] value
		*    Receives the property value
		*  @param[in] defaultValue
		*    Default value in case the shader property doesn't exist
		*
		*  @return
		*    "true" if the requested shader property exists, else "false" if the requested shader property doesn't exist and the default value was returned instead
		*/
		bool getPropertyValue(ShaderPropertyId shaderPropertyId, int32_t& value, int32_t defaultValue = 0) const;

		/**
		*  @brief
		*    Set the value of a property
		*
		*  @param[in] shaderPropertyId
		*    ID of the shader property to set the value of
		*  @param[in] value
		*    The shader property value to set
		*/
		void setPropertyValue(ShaderPropertyId shaderPropertyId, int32_t value);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		SortedPropertyVector mSortedPropertyVector;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Shader/ShaderProperties.inl"
