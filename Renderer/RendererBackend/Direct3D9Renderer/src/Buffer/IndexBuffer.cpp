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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D9Renderer/Buffer/IndexBuffer.h"
#include "Direct3D9Renderer/d3d9.h"
#include "Direct3D9Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D9Renderer/Mapping.h"
#include "Direct3D9Renderer/Direct3D9Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	IndexBuffer::IndexBuffer(Direct3D9Renderer &direct3D9Renderer, uint32_t numberOfBytes, Renderer::IndexBufferFormat::Enum indexBufferFormat, const void *data, Renderer::BufferUsage bufferUsage) :
		IIndexBuffer(direct3D9Renderer),
		mDirect3DIndexBuffer9(nullptr)
	{
		// "Renderer::IndexBufferFormat::UNSIGNED_CHAR" is not supported by Direct3D 9
		if (Renderer::IndexBufferFormat::UNSIGNED_CHAR == indexBufferFormat)
		{
			RENDERER_OUTPUT_DEBUG_STRING("Direct3D 9 error: \"Renderer::IndexBufferFormat::UNSIGNED_CHAR\" is not supported by Direct3D 9")
		}
		else
		{
			// Create the Direct3D 9 index buffer
			direct3D9Renderer.getDirect3DDevice9()->CreateIndexBuffer(numberOfBytes, Mapping::getDirect3D9Usage(bufferUsage), static_cast<D3DFORMAT>(Mapping::getDirect3D9Format(indexBufferFormat)), D3DPOOL_DEFAULT, &mDirect3DIndexBuffer9, nullptr);

			// Copy the data, if required
			if (nullptr != data)
			{
				void *indices = nullptr;
				if (SUCCEEDED(mDirect3DIndexBuffer9->Lock(0, numberOfBytes, static_cast<void**>(&indices), 0)))
				{
					memcpy(indices, data, numberOfBytes);
					mDirect3DIndexBuffer9->Unlock();
				}
			}
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			setDebugName("IBO");
		#endif
	}

	IndexBuffer::~IndexBuffer()
	{
		if (nullptr != mDirect3DIndexBuffer9)
		{
			mDirect3DIndexBuffer9->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void IndexBuffer::setDebugName(const char *name)
	{
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			// Valid Direct3D 9 index buffer?
			if (nullptr != mDirect3DIndexBuffer9)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				mDirect3DIndexBuffer9->SetPrivateData(WKPDID_D3DDebugObjectName, nullptr, 0, 0);
				mDirect3DIndexBuffer9->SetPrivateData(WKPDID_D3DDebugObjectName, name, strlen(name), 0);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
