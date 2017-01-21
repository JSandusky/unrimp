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
#include "Direct3D9Renderer/Buffer/VertexBuffer.h"
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
	VertexBuffer::VertexBuffer(Direct3D9Renderer &direct3D9Renderer, uint32_t numberOfBytes, const void *data, Renderer::BufferUsage bufferUsage) :
		IVertexBuffer(direct3D9Renderer),
		mDirect3DVertexBuffer9(nullptr)
	{
		// Create the Direct3D 9 vertex buffer
		direct3D9Renderer.getDirect3DDevice9()->CreateVertexBuffer(numberOfBytes, Mapping::getDirect3D9Usage(bufferUsage), 0, D3DPOOL_DEFAULT, &mDirect3DVertexBuffer9, nullptr);

		// Copy the data, if required
		if (nullptr != data)
		{
			void *vertices = nullptr;
			if (SUCCEEDED(mDirect3DVertexBuffer9->Lock(0, numberOfBytes, static_cast<void**>(&vertices), 0)))
			{
				memcpy(vertices, data, numberOfBytes);
				mDirect3DVertexBuffer9->Unlock();
			}
		}

		// Assign a default name to the resource for debugging purposes
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			setDebugName("");
		#endif
	}

	VertexBuffer::~VertexBuffer()
	{
		if (nullptr != mDirect3DVertexBuffer9)
		{
			mDirect3DVertexBuffer9->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void VertexBuffer::setDebugName(const char *name)
	{
		#ifndef DIRECT3D9RENDERER_NO_DEBUG
			// Valid Direct3D 9 vertex buffer?
			if (nullptr != mDirect3DVertexBuffer9)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning!
				RENDERER_DECORATED_DEBUG_NAME(name, detailedName, "VBO", 6);	// 6 = "VBO: " including terminating zero
				mDirect3DVertexBuffer9->SetPrivateData(WKPDID_D3DDebugObjectName, nullptr, 0, 0);
				mDirect3DVertexBuffer9->SetPrivateData(WKPDID_D3DDebugObjectName, detailedName, strlen(detailedName), 0);
			}
		#endif
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
