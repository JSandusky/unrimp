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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/Texture/Loader/Lz4DdsTextureResourceLoader.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId Lz4DdsTextureResourceLoader::TYPE_ID("lz4dds");
	const uint32_t			   Lz4DdsTextureResourceLoader::FORMAT_TYPE    = TYPE_ID;
	const uint32_t			   Lz4DdsTextureResourceLoader::FORMAT_VERSION = 1;


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	void Lz4DdsTextureResourceLoader::onDeserialization(IFile& file)
	{
		// Tell the memory mapped file about the LZ4 compressed data
		mMemoryFile.loadLz4CompressedDataFromFile(FORMAT_TYPE, FORMAT_VERSION, file);
	}

	void Lz4DdsTextureResourceLoader::onProcessing()
	{
		// Decompress LZ4 compressed data
		mMemoryFile.decompress();

		// Call the deserialization base implementation
		DdsTextureResourceLoader::onDeserialization(mMemoryFile);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
