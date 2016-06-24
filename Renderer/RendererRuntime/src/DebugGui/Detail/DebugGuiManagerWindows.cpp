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
#include "RendererRuntime/DebugGui/Detail/DebugGuiManagerWindows.h"

#include <imgui/imgui.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	LRESULT DebugGuiManagerWindows::wndProc(HWND, UINT message, WPARAM wParam, LPARAM lParam)
	{
		ImGuiIO& imGuiIo = ImGui::GetIO();
		switch (message)
		{
			case WM_LBUTTONDOWN:
				imGuiIo.MouseDown[0] = true;
				return true;

			case WM_LBUTTONUP:
				imGuiIo.MouseDown[0] = false;
				return true;

			case WM_RBUTTONDOWN:
				imGuiIo.MouseDown[1] = true;
				return true;

			case WM_RBUTTONUP:
				imGuiIo.MouseDown[1] = false;
				return true;

			case WM_MBUTTONDOWN:
				imGuiIo.MouseDown[2] = true;
				return true;

			case WM_MBUTTONUP:
				imGuiIo.MouseDown[2] = false;
				return true;

			case WM_MOUSEWHEEL:
				imGuiIo.MouseWheel += (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ? 1.0f : -1.0f;
				return true;

			case WM_MOUSEMOVE:
				imGuiIo.MousePos.x = static_cast<signed short>(lParam);
				imGuiIo.MousePos.y = static_cast<signed short>(lParam >> 16);
				return true;

			case WM_KEYDOWN:
				if (wParam < 256)
				{
					imGuiIo.KeysDown[wParam] = 1;
				}
				return true;

			case WM_KEYUP:
				if (wParam < 256)
				{
					imGuiIo.KeysDown[wParam] = 0;
				}
				return true;

			case WM_CHAR:
				// You can also use ToAscii() + GetKeyboardState() to retrieve characters.
				if (wParam > 0 && wParam < 0x10000)
				{
					imGuiIo.AddInputCharacter(static_cast<unsigned short>(wParam));
				}
				return true;
		}

		// Done
		return 0;
	}


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::DebugGuiManager methods ]
	//[-------------------------------------------------------]
	void DebugGuiManagerWindows::startup()
	{
		::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&mTicksPerSecond));
		::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mTime));

		// Keyboard mapping. ImGui will use those indices to peek into the imGuiIo.KeyDown[] array that we will update during the application lifetime.
		ImGuiIO& imGuiIo = ImGui::GetIO();
		imGuiIo.KeyMap[ImGuiKey_Tab]		= VK_TAB;
		imGuiIo.KeyMap[ImGuiKey_LeftArrow]	= VK_LEFT;
		imGuiIo.KeyMap[ImGuiKey_RightArrow]	= VK_RIGHT;
		imGuiIo.KeyMap[ImGuiKey_UpArrow]	= VK_UP;
		imGuiIo.KeyMap[ImGuiKey_DownArrow]	= VK_DOWN;
		imGuiIo.KeyMap[ImGuiKey_PageUp]		= VK_PRIOR;
		imGuiIo.KeyMap[ImGuiKey_PageDown]	= VK_NEXT;
		imGuiIo.KeyMap[ImGuiKey_Home]		= VK_HOME;
		imGuiIo.KeyMap[ImGuiKey_End]		= VK_END;
		imGuiIo.KeyMap[ImGuiKey_Delete]		= VK_DELETE;
		imGuiIo.KeyMap[ImGuiKey_Backspace]	= VK_BACK;
		imGuiIo.KeyMap[ImGuiKey_Enter]		= VK_RETURN;
		imGuiIo.KeyMap[ImGuiKey_Escape]		= VK_ESCAPE;
		imGuiIo.KeyMap[ImGuiKey_A]			= 'A';
		imGuiIo.KeyMap[ImGuiKey_C]			= 'C';
		imGuiIo.KeyMap[ImGuiKey_V]			= 'V';
		imGuiIo.KeyMap[ImGuiKey_X]			= 'X';
		imGuiIo.KeyMap[ImGuiKey_Y]			= 'Y';
		imGuiIo.KeyMap[ImGuiKey_Z]			= 'Z';

		// Call the base implementation
		DebugGuiManager::startup();
	}

	void DebugGuiManagerWindows::onNewFrame(Renderer::IRenderTarget& renderTarget)
	{
		ImGuiIO& imGuiIo = ImGui::GetIO();

		{ // Setup display size (every frame to accommodate for render target resizing)
			uint32_t width = 0;
			uint32_t height = 0;
			renderTarget.getWidthAndHeight(width, height);
			imGuiIo.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
		}

		{ // Setup time step
			INT64 currentTime = 0;
			::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));
			imGuiIo.DeltaTime = static_cast<float>(currentTime - mTime) / mTicksPerSecond;
			mTime = currentTime;
		}

		// Read keyboard modifiers inputs
		imGuiIo.KeyCtrl  = ((::GetKeyState(VK_CONTROL) & 0x8000) != 0);
		imGuiIo.KeyShift = ((::GetKeyState(VK_SHIFT) & 0x8000) != 0);
		imGuiIo.KeyAlt   = ((::GetKeyState(VK_MENU) & 0x8000) != 0);
		imGuiIo.KeySuper = false;
		// imGuiIo.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
		// imGuiIo.MousePos : filled by WM_MOUSEMOVE events
		// imGuiIo.MouseDown : filled by WM_*BUTTON* events
		// imGuiIo.MouseWheel : filled by WM_MOUSEWHEEL events
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	DebugGuiManagerWindows::DebugGuiManagerWindows(IRendererRuntime& rendererRuntime) :
		DebugGuiManager(rendererRuntime),
		mTime(0),
		mTicksPerSecond(0)
	{
		// Nothing here
	}

	DebugGuiManagerWindows::~DebugGuiManagerWindows()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime