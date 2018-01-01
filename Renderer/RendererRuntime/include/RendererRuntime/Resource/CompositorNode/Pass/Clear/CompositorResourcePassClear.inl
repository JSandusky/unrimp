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
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline uint32_t CompositorResourcePassClear::getFlags() const
	{
		return mFlags;
	}

	inline const glm::vec4& CompositorResourcePassClear::getClearColor() const
	{
		return mColor;
	}

	inline float CompositorResourcePassClear::getZ() const
	{
		return mZ;
	}

	inline uint32_t CompositorResourcePassClear::getStencil() const
	{
		return mStencil;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::ICompositorResourcePass methods ]
	//[-------------------------------------------------------]
	inline CompositorPassTypeId CompositorResourcePassClear::getTypeId() const
	{
		return TYPE_ID;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline CompositorResourcePassClear::CompositorResourcePassClear(const CompositorTarget& compositorTarget) :
		ICompositorResourcePass(compositorTarget),
		mFlags(0),
		mColor(0.0f, 0.0f, 0.0f, 0.0f),
		mZ(1.0f),
		mStencil(0)
	{
		// Nothing here
	}

	inline CompositorResourcePassClear::~CompositorResourcePassClear()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
