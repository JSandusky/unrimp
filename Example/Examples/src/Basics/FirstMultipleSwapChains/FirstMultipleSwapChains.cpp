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
#include "PrecompiledHeader.h"
#include "Basics/FirstMultipleSwapChains/FirstMultipleSwapChains.h"
#include "Framework/Color4.h"
#ifdef WIN32
	// This is only a simple and close-to-the-metal example, don't use OS stuff directly in more complex projects
	#include "Framework/WindowsHeader.h"
#elif defined LINUX
	// This is only a simple and close-to-the-metal example, don't use OS stuff directly in more complex projects
	#include "Framework/LinuxHeader.h"
	#include "Framework/X11Application.h"
#endif

#include <string.h>


// This is only a simple and close-to-the-metal example, don't use OS stuff directly in more complex projects
#ifdef WIN32
	//[-------------------------------------------------------]
	//[ Global MS Windows callback function                   ]
	//[-------------------------------------------------------]
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		// Get pointer to the swap chain implementation
		Renderer::ISwapChain *swapChain = (NULL_HANDLE != hWnd) ? reinterpret_cast<Renderer::ISwapChain*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)) : nullptr;

		// Evaluate message
		switch (message)
		{
			case WM_SIZE:
				if (nullptr != swapChain)
				{
					swapChain->resizeBuffers();
				}
				return 0;

			case WM_SYSKEYDOWN:
				// Toggle fullscreen right now? (Alt-Return)
				if (nullptr != swapChain && VK_RETURN == wParam && (lParam & (1 << 29)))	// Bit 29 = the ALT-key
				{
					// Toggle fullscreen mode
					swapChain->setFullscreenState(!swapChain->getFullscreenState());
				}
				return 0;

			// Let the OS handle this message
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
#elif defined LINUX
	class SwapChainWindow : public X11Window
	{
	public:
		SwapChainWindow() :
			mSwapChain(nullptr)
		{
			// Nothing to do in here
		}

		void setSwapChain(Renderer::ISwapChain *swapChain)
		{
			mSwapChain = swapChain;
		}

		virtual bool HandleEvent(XEvent &event)
		{
			X11Window::HandleEvent(event);
			OUTPUT_DEBUG_STRING("Handle Event in SwapChainWindow\n")
			switch (event.type)
			{
				// Window configuration changed
				case ConfigureNotify:
					if (nullptr != mSwapChain)
					{
						mSwapChain->resizeBuffers();
					}
				break;
			}
			return false;
		}
	private:
		Renderer::ISwapChain *mSwapChain;
	};
	// TODO(co) Even inside an example, we might not want to use global variables
	SwapChainWindow *gSwapChainWindow = nullptr;
#endif


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstMultipleSwapChains::FirstMultipleSwapChains(const char *rendererName) :
	IApplicationRenderer(rendererName)
{
	// Nothing to do in here
}

FirstMultipleSwapChains::~FirstMultipleSwapChains()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstMultipleSwapChains::onInitialization()
{
	// Call the base implementation
	IApplicationRenderer::onInitialization();

	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Decide which shader language should be used (for example "GLSL" or "HLSL")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer->getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			{ // Create the root signature
				// Setup
				Renderer::RootSignatureBuilder rootSignature;
				rootSignature.initialize(0, nullptr, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

				// Create the instance
				mRootSignature = renderer->createRootSignature(rootSignature);
			}

			// Create the program
			Renderer::IProgramPtr program;
			{
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderSourceCode = nullptr;
				const char *fragmentShaderSourceCode = nullptr;
				#include "FirstMultipleSwapChains_GLSL_110.h"
				#include "FirstMultipleSwapChains_GLSL_ES2.h"
				#include "FirstMultipleSwapChains_HLSL_D3D9_D3D10_D3D11_D3D12.h"
				#include "FirstMultipleSwapChains_Null.h"

				// Create the program
				program = shaderLanguage->createProgram(
					shaderLanguage->createVertexShaderFromSourceCode(vertexShaderSourceCode),
					shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode));
			}

			// Is there a valid program?
			if (nullptr != program)
			{
				// Vertex input layout
				const Renderer::VertexArrayAttribute vertexArrayAttributes[] =
				{
					{ // Attribute 0
						// Data destination
						Renderer::VertexArrayFormat::FLOAT_2,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
						"Position",								// name[32] (char)
						"POSITION",								// semanticName[32] (char)
						0,										// semanticIndex (uint32_t)
						// Data source
						0,										// inputSlot (uint32_t)
						0,										// alignedByteOffset (uint32_t)
						// Data source, instancing part
						0										// instancesPerElement (uint32_t)
					}
				};
				const uint32_t numberOfVertexAttributes = sizeof(vertexArrayAttributes) / sizeof(Renderer::VertexArrayAttribute);

				{ // Create the pipeline state object (PSO)
					// Setup
					Renderer::PipelineState pipelineState;
					pipelineState.rootSignature = mRootSignature;
					pipelineState.program = program;
					pipelineState.numberOfVertexAttributes = numberOfVertexAttributes;
					pipelineState.vertexAttributes = vertexArrayAttributes;

					// Create the instance
					mPipelineState = renderer->createPipelineState(pipelineState);
				}

				{ // Create vertex array object (VAO)
					// Create the vertex buffer object (VBO)
					// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
					static const float VERTEX_POSITION[] =
					{					// Vertex ID	Triangle on screen
						 0.0f, 1.0f,	// 0				0
						 1.0f, 0.0f,	// 1			   .   .
						-0.5f, 0.0f		// 2			  2.......1
					};
					Renderer::IVertexBufferPtr vertexBuffer(renderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

					// Create vertex array object (VAO)
					// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
					// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
					// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
					//    reference of the used vertex buffer objects (VBO). If the reference counter of a
					//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
					const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] =
					{
						{ // Vertex buffer 0
							vertexBuffer,		// vertexBuffer (Renderer::IVertexBuffer *)
							sizeof(float) * 2	// strideInBytes (uint32_t)
						}
					};
					mVertexArray = program->createVertexArray(numberOfVertexAttributes, vertexArrayAttributes, sizeof(vertexArrayVertexBuffers) / sizeof(Renderer::VertexArrayVertexBuffer), vertexArrayVertexBuffers);
				}
			}
		}

		{ // Create the swap chain
			handle nativeWindowHandle = NULL_HANDLE;

			// Create the OS native window instance
			// -> This is only a simple and close-to-the-metal example, don't use OS stuff directly in more complex projects
			#ifdef WIN32
				{ // Setup and register the window class for the this example window
					WNDCLASS windowDummyClass;
					windowDummyClass.hInstance		= ::GetModuleHandle(nullptr);
					windowDummyClass.lpszClassName	= TEXT("FirstMultipleSwapChains");
					windowDummyClass.lpfnWndProc	= WndProc;
					windowDummyClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
					windowDummyClass.hIcon			= nullptr;
					windowDummyClass.hCursor		= ::LoadCursor(nullptr, IDC_ARROW);
					windowDummyClass.lpszMenuName	= nullptr;
					windowDummyClass.cbClsExtra		= 0;
					windowDummyClass.cbWndExtra		= 0;
					windowDummyClass.hbrBackground	= nullptr;
					::RegisterClass(&windowDummyClass);
				}

				// Create the OS native window instance
				HWND hWnd = ::CreateWindowA("FirstMultipleSwapChains", "Another window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, nullptr, nullptr, ::GetModuleHandle(nullptr), nullptr);
				if (NULL_HANDLE != hWnd)
				{
					// Backup the native window handle
					nativeWindowHandle = reinterpret_cast<handle>(hWnd);

					// Show the created OS window
					::ShowWindow(hWnd, SW_SHOWDEFAULT);
				}
			#elif defined LINUX
				gSwapChainWindow = new SwapChainWindow();
				gSwapChainWindow->setTitle("Another window");
				gSwapChainWindow->show();
				XSync(X11Application::instance()->getDisplay(), False);
				nativeWindowHandle = gSwapChainWindow->winId();
			#else
				#error "Unsupported platform"
			#endif

			// Create the swap chain
			mSwapChain = renderer->createSwapChain(nativeWindowHandle);

			// This is only a simple and close-to-the-metal example, don't use OS stuff directly in more complex projects
			#ifdef WIN32
				// Is there a valid OS native window instance?
				if (NULL_HANDLE != hWnd)
				{
					// In case of an error, destroy the OS native window instance at once
					if (nullptr == mSwapChain)
					{
						// Destroy the native OS window instance
						::DestroyWindow(hWnd);

						// Unregister the window class for this example window
						::UnregisterClass(TEXT("FirstMultipleSwapChains"), ::GetModuleHandle(nullptr));
					}
					else
					{
						// Set window pointer and handle (SetWindowLongPtr is the 64bit equivalent to SetWindowLong)
						::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(mSwapChain->getPointer()));
					}
				}
			#elif defined LINUX
				// Is there a valid OS native window instance?
				if (NULL_HANDLE != nativeWindowHandle)
				{
					// In case of an error, destroy the OS native window instance at once
					if (nullptr == mSwapChain)
					{
						// Destroy the native OS window instance
						delete gSwapChainWindow;
						gSwapChainWindow = nullptr;
					}
				}
			#endif
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstMultipleSwapChains::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Release the used resources
	if (nullptr != mSwapChain)
	{
		// This is only a simple and close-to-the-metal example, don't use OS stuff directly in more complex projects
		#ifdef WIN32
			// Destroy the native OS window instance
			const HWND hWnd = reinterpret_cast<HWND>(mSwapChain->getNativeWindowHandle());
			if (NULL_HANDLE != hWnd)
			{
				::DestroyWindow(hWnd);
			}

			// Unregister the window class for this example window
			::UnregisterClass(TEXT("FirstMultipleSwapChains"), ::GetModuleHandle(nullptr));
		#elif defined LINUX
			if (nullptr != gSwapChainWindow)
			{
				delete gSwapChainWindow;
				gSwapChainWindow = nullptr;
			}
		#endif

		// Release the swap chain
		mSwapChain = nullptr;
	}
	mVertexArray = nullptr;
	mPipelineState = nullptr;
	mRootSignature = nullptr;

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())

	// Call the base implementation
	IApplicationRenderer::onDeinitialization();
}

void FirstMultipleSwapChains::onDraw()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && nullptr != mPipelineState)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Usually you draw into a swap chain when getting informed by the OS that the
		// used native OS window requests a redraw of it's content. In order to avoid
		// adding to much unnecessary overhead in here we just draw into the created
		// swap chain as soon as the main swap chain gets redrawn.

		// Debug methods: When using Direct3D <11.1, these methods map to the Direct3D 9 PIX functions
		// (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)
		// -> In this example we're using multiple swap chains and calling "Renderer::ISwapChain::present()" twice per application
		// -> Usually, a swap chain present is interpreted by the debug/profile tool as a single frame, which is of course correct
		// -> In this example this behaviour makes it difficult to catch the desired frame of the desired native OS window

		// Render to the swap chain created in this example, but only if it's valid
		if (nullptr != mSwapChain)
		{
			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT(renderer, L"Render to the swap chain created in this example")

			// Backup the currently used render target
			Renderer::IRenderTargetPtr renderTarget(renderer->omGetRenderTarget());

			// Set the render target to render into
			renderer->omSetRenderTarget(mSwapChain);

			{ // Set the viewport
				// Please note that for some graphics APIs its really important that the viewport
				// is inside the bounds of the currently used render target
				// -> For Direct3D 10 and Direct3D 11 TODO(co)(Check OpenGL and OpenGL ES 2 behaviour) it's OK
				//    when using a viewport which is outside the bounds of the currently used render target.
				//    Within this example you can intentionally set no new viewport in order to see what
				//    happens when using a viewport other than one covering the whole native OS window.
				// -> When using Direct3D 9 you will get a
				//      "Direct3D9: (ERROR) :Viewport outside the render target surface"
				//      "D3D9 Helper: IDirect3DDevice9::DrawPrimitive failed: D3DERR_INVALIDCAL"
				//    in case the viewport is inside the bounds of the currently used render target

				// Get the render target with and height
				uint32_t width  = 1;
				uint32_t height = 1;
				mSwapChain->getWidthAndHeight(width, height);

				// Set the viewport
				const Renderer::Viewport viewport =
				{
					0.0f,						// topLeftX (float)
					0.0f,						// topLeftY (float)
					static_cast<float>(width),	// width (float)
					static_cast<float>(height),	// height (float)
					0.0f,						// minDepth (float)
					1.0f						// maxDepth (float)
				};
				renderer->rsSetViewports(1, &viewport);
			}

			// Draw into the swap chain created in this example
			draw(Color4::GREEN);

			// Restore the previously set render target
			renderer->omSetRenderTarget(renderTarget);

			// Present the content of the current back buffer
			mSwapChain->present();

			// End debug event
			RENDERER_END_DEBUG_EVENT(renderer)
		}

		{ // Draw into the main swap chain
			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT(renderer, L"Draw into the main swap chain")

			{ // Set the viewport
				// Get the render target with and height
				uint32_t width  = 1;
				uint32_t height = 1;
				Renderer::IRenderTarget *renderTarget = renderer->omGetRenderTarget();
				if (nullptr != renderTarget)
				{
					renderTarget->getWidthAndHeight(width, height);
				}

				// Set the viewport
				const Renderer::Viewport viewport =
				{
					0.0f,						// topLeftX (float)
					0.0f,						// topLeftY (float)
					static_cast<float>(width),	// width (float)
					static_cast<float>(height),	// height (float)
					0.0f,						// minDepth (float)
					1.0f						// maxDepth (float)
				};
				renderer->rsSetViewports(1, &viewport);
			}

			// Draw into the main swap chain
			draw(Color4::GRAY);

			// End debug event
			RENDERER_END_DEBUG_EVENT(renderer)
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
void FirstMultipleSwapChains::draw(const float color[4])
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && nullptr != mPipelineState)
	{
		// Begin scene rendering
		// -> Required for Direct3D 9
		// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
		if (renderer->beginScene())
		{
			// Clear the color buffer of the current render target with the provided color, do also clear the depth buffer
			renderer->clear(Renderer::ClearFlag::COLOR_DEPTH, color, 1.0f, 0);

			// Set the used graphics root signature
			renderer->setGraphicsRootSignature(mRootSignature);

			// Set the used pipeline state object (PSO)
			renderer->setPipelineState(mPipelineState);

			{ // Setup input assembly (IA)
				// Set the used vertex array
				renderer->iaSetVertexArray(mVertexArray);

				// Set the primitive topology used for draw calls
				renderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_LIST);
			}

			// Render the specified geometric primitive, based on an array of vertices
			renderer->draw(0, 3);

			// End scene rendering
			// -> Required for Direct3D 9
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			renderer->endScene();
		}
	}
}
