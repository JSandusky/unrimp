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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline MaterialResourceId MaterialResource::getParentMaterialResourceId() const
	{
		return mParentMaterialResourceId;
	}

	inline const MaterialResource::SortedMaterialTechniqueVector& MaterialResource::getSortedMaterialTechniqueVector() const
	{
		return mSortedMaterialTechniqueVector;
	}

	inline const MaterialProperties& MaterialResource::getMaterialProperties() const
	{
		return mMaterialProperties;
	}

	inline const MaterialProperties::SortedPropertyVector& MaterialResource::getSortedPropertyVector() const
	{
		return mMaterialProperties.getSortedPropertyVector();
	}

	inline void MaterialResource::removeAllProperties()
	{
		return mMaterialProperties.removeAllProperties();
	}

	inline const MaterialProperty* MaterialResource::getPropertyById(MaterialPropertyId materialPropertyId) const
	{
		return mMaterialProperties.getPropertyById(materialPropertyId);
	}

	inline bool MaterialResource::setPropertyById(MaterialPropertyId materialPropertyId, const MaterialPropertyValue& materialPropertyValue, MaterialProperty::Usage materialPropertyUsage)
	{
		return setPropertyByIdInternal(materialPropertyId, materialPropertyValue, materialPropertyUsage, true);
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline MaterialResource::MaterialResource() :
		mParentMaterialResourceId(getUninitialized<MaterialResourceId>())
	{
		// Nothing here
	}

	inline void MaterialResource::initializeElement(MaterialResourceId materialResourceId)
	{
		// Sanity checks
		assert(isUninitialized(mParentMaterialResourceId));
		assert(mSortedChildMaterialResourceIds.empty());
		assert(mSortedMaterialTechniqueVector.empty());
		assert(mMaterialProperties.getSortedPropertyVector().empty());

		// Call base implementation
		IResource::initializeElement(materialResourceId);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
