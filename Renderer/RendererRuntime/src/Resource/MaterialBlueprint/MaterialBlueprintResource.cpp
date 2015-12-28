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
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResource.h"
#include "RendererRuntime/Resource/ShaderPiece/ShaderPieceResource.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	namespace detail
	{
		bool isFullyLoaded(const ShaderBlueprintResource* shaderBlueprint)
		{
			// Check shader blueprint
			if (nullptr == shaderBlueprint || IResource::LoadingState::LOADED != shaderBlueprint->getLoadingState())
			{
				// Not fully loaded
				return false;
			}

			{ // Check included shader piece resources
				const ShaderBlueprintResource::IncludeShaderPieceResources& includeShaderPieceResources = shaderBlueprint->getIncludeShaderPieceResources();
				const size_t numberOfShaderPieces = includeShaderPieceResources.size();
				for (size_t i = 0; i < numberOfShaderPieces; ++i)
				{
					const ShaderPieceResource* shaderPieceResource = includeShaderPieceResources[i];
					if (nullptr == shaderPieceResource || IResource::LoadingState::LOADED != shaderPieceResource->getLoadingState())
					{
						// Not fully loaded
						return false;
					}
				}
			}

			// Fully loaded
			return true;
		}
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	MaterialBlueprintResource::MaterialBlueprintResource(ResourceId resourceId) :
		IResource(resourceId),
		mRootSignature(nullptr),
		mPipelineState(Renderer::PipelineStateBuilder()),
		mVertexShaderBlueprint(nullptr),
		mTessellationControlShaderBlueprint(nullptr),
		mTessellationEvaluationShaderBlueprint(nullptr),
		mGeometryShaderBlueprint(nullptr),
		mFragmentShaderBlueprint(nullptr)
	{
		// Nothing here
	}

	MaterialBlueprintResource::~MaterialBlueprintResource()
	{
		if (nullptr != mRootSignature)
		{
			mRootSignature->release();
		}
	}

	bool MaterialBlueprintResource::isFullyLoaded() const
	{
		return (IResource::LoadingState::LOADED == getLoadingState() && nullptr != mRootSignature && detail::isFullyLoaded(mVertexShaderBlueprint) && detail::isFullyLoaded(mFragmentShaderBlueprint));
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
