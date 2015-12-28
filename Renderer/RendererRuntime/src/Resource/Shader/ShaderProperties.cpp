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
#include "RendererRuntime/Resource/Shader/ShaderProperties.h"

#include <algorithm>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	namespace detail
	{
		inline bool orderPropertyByShaderPropertyId( const ShaderProperties::Property& left, const ShaderProperties::Property& right)
		{
			return (left.shaderPropertyId < right.shaderPropertyId);
		}
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	bool ShaderProperties::getPropertyValue(ShaderPropertyId shaderPropertyId, int32_t& value, int32_t defaultValue) const
	{
		const Property property(shaderPropertyId, 0);
		SortedPropertyVector::const_iterator iterator = std::lower_bound(mSortedPropertyVector.begin(), mSortedPropertyVector.end(), property, detail::orderPropertyByShaderPropertyId);
		if (iterator != mSortedPropertyVector.end() && iterator->shaderPropertyId == property.shaderPropertyId)
		{
			value = iterator->value;
			return true;
		}
		else
		{
			value = defaultValue;
			return false;
		}
	}

	void ShaderProperties::setPropertyValue(ShaderPropertyId shaderPropertyId, int32_t value)
	{
		const Property property(shaderPropertyId, value);
		SortedPropertyVector::iterator iterator = std::lower_bound(mSortedPropertyVector.begin(), mSortedPropertyVector.end(), property, detail::orderPropertyByShaderPropertyId);
		if (iterator == mSortedPropertyVector.end() || iterator->shaderPropertyId != property.shaderPropertyId)
		{
			// Add new shader property
			mSortedPropertyVector.insert(iterator, property);
		}
		else
		{
			// Just update the shader property value
			*iterator = property;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
