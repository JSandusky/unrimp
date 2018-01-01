/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "RendererRuntime/Core/GetUninitialized.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	inline MaterialPropertyValue MaterialProperty::materialPropertyValueFromReference(ValueType valueType, uint32_t reference)
	{
		MaterialPropertyValue materialPropertyValue;
		materialPropertyValue.mValueType	 = valueType;
		materialPropertyValue.mValue.Integer = static_cast<int>(reference);
		return materialPropertyValue;
	}

	inline bool MaterialProperty::isReferenceUsage(Usage usage)
	{
		return (Usage::TEXTURE_REFERENCE == usage || Usage::GLOBAL_REFERENCE == usage || Usage::UNKNOWN_REFERENCE == usage || Usage::PASS_REFERENCE == usage || Usage::MATERIAL_REFERENCE == usage || Usage::INSTANCE_REFERENCE == usage);
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline MaterialProperty::MaterialProperty() :
		MaterialPropertyValue(fromUnknown()),
		mMaterialPropertyId(getUninitialized<MaterialPropertyId>()),
		mUsage(Usage::UNKNOWN),
		mOverwritten(false)
	{
		// Nothing here
	}

	inline MaterialProperty::MaterialProperty(MaterialPropertyId materialPropertyId, Usage usage, const MaterialPropertyValue& materialPropertyValue) :
		MaterialPropertyValue(materialPropertyValue),
		mMaterialPropertyId(materialPropertyId),
		mUsage(usage),
		mOverwritten(false)
	{
		// Nothing here
	}

	inline MaterialProperty::~MaterialProperty()
	{
		// Nothing here
	}

	inline MaterialPropertyId MaterialProperty::getMaterialPropertyId() const
	{
		return mMaterialPropertyId;
	}

	inline MaterialProperty::Usage MaterialProperty::MaterialProperty::getUsage() const
	{
		return mUsage;
	}

	inline bool MaterialProperty::isOverwritten() const
	{
		return mOverwritten;
	}

	inline void MaterialProperty::setOverwritten(bool overwritten)
	{
		mOverwritten = overwritten;
	}

	inline bool MaterialProperty::isReferenceUsage() const
	{
		return isReferenceUsage(mUsage);
	}

	inline uint32_t MaterialProperty::getReferenceValue() const
	{
		assert(isReferenceUsage());
		return static_cast<uint32_t>(mValue.Integer);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
