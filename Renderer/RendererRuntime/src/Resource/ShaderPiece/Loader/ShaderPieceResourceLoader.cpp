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
#include "RendererRuntime/Resource/ShaderPiece/Loader/ShaderPieceResourceLoader.h"
#include "RendererRuntime/Resource/ShaderPiece/Loader/ShaderPieceFileFormat.h"
#include "RendererRuntime/Resource/ShaderPiece/ShaderPieceResource.h"

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId ShaderPieceResourceLoader::TYPE_ID("shader_piece");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	ResourceLoaderTypeId ShaderPieceResourceLoader::getResourceLoaderTypeId() const
	{
		return TYPE_ID;
	}

	void ShaderPieceResourceLoader::onDeserialization()
	{
		// TODO(co) Error handling
		try
		{
			std::ifstream inputFileStream(mAsset.assetFilename, std::ios::binary);

			// Read in the shader piece header
			v1ShaderPiece::Header shaderPieceHeader;
			inputFileStream.read(reinterpret_cast<char*>(&shaderPieceHeader), sizeof(v1ShaderPiece::Header));

			// Allocate more temporary memory, if required
			if (mMaximumNumberOfShaderSourceCodeBytes < shaderPieceHeader.numberOfShaderSourceCodeBytes)
			{
				mMaximumNumberOfShaderSourceCodeBytes = shaderPieceHeader.numberOfShaderSourceCodeBytes;
				delete [] mShaderSourceCode;
				mShaderSourceCode = new char[mMaximumNumberOfShaderSourceCodeBytes];
			}

			// Read the shader piece ASCII source code
			inputFileStream.read(mShaderSourceCode, shaderPieceHeader.numberOfShaderSourceCodeBytes);
			mShaderPieceResource->mShaderSourceCode.assign(mShaderSourceCode, mShaderSourceCode + shaderPieceHeader.numberOfShaderSourceCodeBytes);
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load shader piece asset %d: %s", mAsset.assetId, e.what());
		}
	}

	void ShaderPieceResourceLoader::onProcessing()
	{
		// Nothing here
	}

	void ShaderPieceResourceLoader::onRendererBackendDispatch()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
