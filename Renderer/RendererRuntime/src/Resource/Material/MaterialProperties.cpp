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
#include "RendererRuntime/Resource/Material/MaterialProperties.h"

#include <algorithm>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	const MaterialProperty* MaterialProperties::getPropertyById(MaterialPropertyId materialPropertyId) const
	{
		SortedPropertyVector::const_iterator iterator = std::lower_bound(mSortedPropertyVector.cbegin(), mSortedPropertyVector.cend(), materialPropertyId, detail::OrderByMaterialPropertyId());
		return (iterator != mSortedPropertyVector.end() && iterator._Ptr->getMaterialPropertyId() == materialPropertyId) ? iterator._Ptr : nullptr;
	}

	void MaterialProperties::setPropertyById(MaterialPropertyId materialPropertyId, const MaterialPropertyValue& materialPropertyValue)
	{
		const MaterialProperty materialProperty(materialPropertyId, MaterialProperty::Usage::DYNAMIC, materialPropertyValue);
		SortedPropertyVector::iterator iterator = std::lower_bound(mSortedPropertyVector.begin(), mSortedPropertyVector.end(), materialPropertyId, detail::OrderByMaterialPropertyId());
		if (iterator == mSortedPropertyVector.end() || iterator->getMaterialPropertyId() != materialPropertyId)
		{
			// Add new material property
			mSortedPropertyVector.insert(iterator, materialProperty);
		}
		else
		{
			// Just update the material property value
			*iterator = materialProperty;
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
