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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Material/MaterialResource.h"

#include <algorithm>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	namespace detail
	{
		struct OrderByMaterialTechniqueId
		{
			inline bool operator()(const MaterialTechnique& left, MaterialTechniqueId right) const
			{
				return (left.getMaterialTechniqueId() < right);
			}

			inline bool operator()(MaterialTechniqueId left, const MaterialTechnique& right) const
			{
				return (left < right.getMaterialTechniqueId());
			}
		};
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	MaterialResource::MaterialResource(ResourceId resourceId) :
		IResource(resourceId)
	{
		// Nothing here
	}

	MaterialTechnique* MaterialResource::getMaterialTechniqueById(MaterialTechniqueId materialTechniqueId) const
	{
		SortedMaterialTechniqueVector::const_iterator iterator = std::lower_bound(mSortedMaterialTechniqueVector.cbegin(), mSortedMaterialTechniqueVector.cend(), materialTechniqueId, detail::OrderByMaterialTechniqueId());
		return (iterator != mSortedMaterialTechniqueVector.end() && iterator._Ptr->getMaterialTechniqueId() == materialTechniqueId) ? iterator._Ptr : nullptr;
	}

	void MaterialResource::releasePipelineState()
	{
		// TODO(co) Cleanup
		for (MaterialTechnique& materialTechnique : mSortedMaterialTechniqueVector)
		{
			materialTechnique.mTextures.clear();
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
