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
//[ Public methods                                        ]
//[-------------------------------------------------------]
inline Renderer::ILog* ExampleBase::getCustomLog() const
{
	return mCustomLog;
}

inline Renderer::IRenderer* ExampleBase::getRenderer() const
{
	return (nullptr != mApplicationFrontend) ? mApplicationFrontend->getRenderer() : nullptr;
}

inline Renderer::IRenderTarget* ExampleBase::getMainRenderTarget() const
{
	return (nullptr != mApplicationFrontend) ? mApplicationFrontend->getMainRenderTarget() : nullptr;
}

inline RendererRuntime::IRendererRuntime* ExampleBase::getRendererRuntime() const
{
	return (nullptr != mApplicationFrontend) ? mApplicationFrontend->getRendererRuntime() : nullptr;
}

inline RendererToolkit::IRendererToolkit* ExampleBase::getRendererToolkit()
{
	return (nullptr != mApplicationFrontend) ? mApplicationFrontend->getRendererToolkit() : nullptr;
}


//[-------------------------------------------------------]
//[ Public virtual ExampleBase methods                    ]
//[-------------------------------------------------------]
inline bool ExampleBase::doesCompleteOwnDrawing() const
{
	// Default implementation does not complete own drawing
	return false;
}
