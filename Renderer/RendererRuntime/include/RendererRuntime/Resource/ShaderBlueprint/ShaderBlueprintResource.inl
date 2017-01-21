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
	inline const ShaderBlueprintResource::IncludeShaderPieceResourceIds& ShaderBlueprintResource::getIncludeShaderPieceResourceIds() const
	{
		return mIncludeShaderPieceResourceIds;
	}

	inline const ShaderProperties& ShaderBlueprintResource::getReferencedShaderProperties() const
	{
		return mReferencedShaderProperties;
	}

	inline const std::string& ShaderBlueprintResource::getShaderSourceCode() const
	{
		return mShaderSourceCode;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline ShaderBlueprintResource::ShaderBlueprintResource()
	{
		// Nothing here
	}

	inline ShaderBlueprintResource::~ShaderBlueprintResource()
	{
		// Sanity checks
		assert(mIncludeShaderPieceResourceIds.empty());
		assert(mReferencedShaderProperties.getSortedPropertyVector().empty());
		assert(mShaderSourceCode.empty());
	}

	inline void ShaderBlueprintResource::initializeElement(ShaderBlueprintResourceId shaderBlueprintResourceId)
	{
		// Sanity checks
		assert(mIncludeShaderPieceResourceIds.empty());
		assert(mReferencedShaderProperties.getSortedPropertyVector().empty());
		assert(mShaderSourceCode.empty());

		// Call base implementation
		IResource::initializeElement(shaderBlueprintResourceId);
	}

	inline void ShaderBlueprintResource::deinitializeElement()
	{
		// Reset everything
		mIncludeShaderPieceResourceIds.clear();
		mReferencedShaderProperties.clear();
		mShaderSourceCode.clear();

		// Call base implementation
		IResource::deinitializeElement();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
