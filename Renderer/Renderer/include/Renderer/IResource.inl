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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline IResource::~IResource()
	{
		// Nothing here
	}

	inline ResourceType IResource::getResourceType() const
	{
		return mResourceType;
	}

	inline IRenderer& IResource::getRenderer() const
	{
		return *mRenderer;
	}


	//[-------------------------------------------------------]
	//[ Public virtual IResource methods                      ]
	//[-------------------------------------------------------]
	inline void IResource::setDebugName(const char*)
	{
		// Nothing to do by default
	}

	inline void* IResource::getInternalResourceHandle() const
	{
		return nullptr;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline IResource::IResource(ResourceType resourceType) :
		mResourceType(resourceType),
		mRenderer(nullptr)	// Only used for rare border cases, use the constructor with the renderer reference whenever possible. Normally the renderer pointer should never ever be a null pointer. So if you're in here, you're considered to be evil.
	{
		// Nothing here
	}

	inline IResource::IResource(ResourceType resourceType, IRenderer& renderer) :
		mResourceType(resourceType),
		mRenderer(&renderer)
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
