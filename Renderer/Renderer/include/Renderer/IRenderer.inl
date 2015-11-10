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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline IRenderer::~IRenderer()
	{
		// Nothing to do in here
	}

	inline const Capabilities &IRenderer::getCapabilities() const
	{
		return mCapabilities;
	}

	inline const Statistics &IRenderer::getStatistics() const
	{
		return mStatistics;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline IRenderer::IRenderer()
	{
		// Nothing to do in here
	}

	inline IRenderer::IRenderer(const IRenderer &)
	{
		// Not supported
	}

	inline IRenderer &IRenderer::operator =(const IRenderer &)
	{
		// Not supported
		return *this;
	}

	inline Statistics &IRenderer::getStatistics()
	{
		return mStatistics;
	}

	inline void IRenderer::rsSetViewportAndScissorRectangle(uint32_t topLeftX, uint32_t topLeftY, uint32_t width, uint32_t height)
	{
		{ // Set the viewport
			const Viewport viewport =
			{
				static_cast<float>(topLeftX),	// topLeftX (float)
				static_cast<float>(topLeftY),	// topLeftY (float)
				static_cast<float>(width),		// width (float)
				static_cast<float>(height),		// height (float)
				0.0f,							// minDepth (float)
				1.0f							// maxDepth (float)
			};
			rsSetViewports(1, &viewport);
		}

		{ // Set the scissor rectangle
			const ScissorRectangle scissorRectangle =
			{
				static_cast<long>(topLeftX),	// topLeftX (long)
				static_cast<long>(topLeftY),	// topLeftY (long)
				static_cast<long>(width),		// bottomRightX (long)
				static_cast<long>(height)		// bottomRightY (long)
			};
			rsSetScissorRectangles(1, &scissorRectangle);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
