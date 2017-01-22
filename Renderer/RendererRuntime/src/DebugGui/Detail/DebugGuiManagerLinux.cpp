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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/DebugGui/Detail/DebugGuiManagerLinux.h"

#include <imgui/imgui.h>

#include <X11/keysym.h>
#include <sys/time.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	void DebugGuiManagerLinux::onXEvent(XEvent& event)
	{
		ImGuiIO& imGuiIo = ImGui::GetIO();
		
		switch (event.type)
		{
			case ConfigureNotify:
				mWindowWidth = event.xconfigure.width;
				mWindowHeigth = event.xconfigure.height;
				break;
			case KeyPress:
			{
				const uint32_t keySym = XLookupKeysym(&event.xkey, 0);
				imGuiIo.KeysDown[keySym] = 1;
				break;
			}

			case KeyRelease:
			{
				const uint32_t keySym = XLookupKeysym(&event.xkey, 0);
				imGuiIo.KeysDown[keySym] = 0;
				break;
			}

			case ButtonPress:
			{
				if (event.xbutton.button == 1)
				{
					imGuiIo.MouseDown[0] = true;
				}
				else if (event.xbutton.button == 3)
				{
					imGuiIo.MouseDown[1] = true;
				}
				else if (event.xbutton.button == 4 || event.xbutton.button == 5) // Wheel buttons
				{
					imGuiIo.MouseWheel += (event.xbutton.button == 4) ? 1.0f : -1.0f;
				}
				break;
			}

			case ButtonRelease:
			{
				if (event.xbutton.button == 1)
					imGuiIo.MouseDown[0] = false;
				else if (event.xbutton.button == 3)
					imGuiIo.MouseDown[1] = false;
				break;
			}

			case MotionNotify:
			{
				updateMousePosition(event.xmotion.x, event.xmotion.y);
				break;
			}
		}
	}



	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::DebugGuiManager methods ]
	//[-------------------------------------------------------]
	void DebugGuiManagerLinux::startup()
	{
		// Keyboard mapping. ImGui will use those indices to peek into the imGuiIo.KeyDown[] array that we will update during the application lifetime.
		ImGuiIO& imGuiIo = ImGui::GetIO();
		// TODO(sw) These keysyms are 16bit values with an value > 512
// 		imGuiIo.KeyMap[ImGuiKey_Tab]		= XK_Tab;
// 		imGuiIo.KeyMap[ImGuiKey_LeftArrow]	= XK_Left;
// 		imGuiIo.KeyMap[ImGuiKey_RightArrow]	= XK_Right;
// 		imGuiIo.KeyMap[ImGuiKey_UpArrow]	= XK_Up;
// 		imGuiIo.KeyMap[ImGuiKey_DownArrow]	= XK_Down;
// 		imGuiIo.KeyMap[ImGuiKey_PageUp]		= XK_Page_Up;
// 		imGuiIo.KeyMap[ImGuiKey_PageDown]	= XK_Page_Down;
// 		imGuiIo.KeyMap[ImGuiKey_Home]		= XK_Home;
// 		imGuiIo.KeyMap[ImGuiKey_End]		= XK_End;
// 		imGuiIo.KeyMap[ImGuiKey_Delete]		= XK_Delete;
// 		imGuiIo.KeyMap[ImGuiKey_Backspace]	= XK_BackSpace;
// 		imGuiIo.KeyMap[ImGuiKey_Enter]		= XK_Return;
// 		imGuiIo.KeyMap[ImGuiKey_Escape]		= XK_Escape;
		imGuiIo.KeyMap[ImGuiKey_A]			= XK_a;
		imGuiIo.KeyMap[ImGuiKey_C]			= XK_c;
		imGuiIo.KeyMap[ImGuiKey_V]			= XK_v;
		imGuiIo.KeyMap[ImGuiKey_X]			= XK_x;
		imGuiIo.KeyMap[ImGuiKey_Y]			= XK_y;
		imGuiIo.KeyMap[ImGuiKey_Z]			= XK_z;

		// Call the base implementation
		DebugGuiManager::startup();
	}

	void DebugGuiManagerLinux::onNewFrame(Renderer::IRenderTarget& renderTarget)
	{
		ImGuiIO& imGuiIo = ImGui::GetIO();

		{ // Setup display size (every frame to accommodate for render target resizing)
			uint32_t width = 0;
			uint32_t height = 0;
			renderTarget.getWidthAndHeight(width, height);
			imGuiIo.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
		}

		// TODO(sw) implement me
		
		{ // Setup time step
			timeval currentTimeValue;
			gettimeofday(&currentTimeValue, nullptr);
			uint64_t currentTime = currentTimeValue.tv_sec * 1000000 + currentTimeValue.tv_usec;
			imGuiIo.DeltaTime = static_cast<float>(currentTime - mTime) / 1000000.0f;
			mTime = currentTime;
		}
// 
// 		// Read keyboard modifiers inputs
// 		imGuiIo.KeyCtrl  = ((::GetKeyState(VK_CONTROL) & 0x8000) != 0);
// 		imGuiIo.KeyShift = ((::GetKeyState(VK_SHIFT) & 0x8000) != 0);
// 		imGuiIo.KeyAlt   = ((::GetKeyState(VK_MENU) & 0x8000) != 0);
// 		imGuiIo.KeySuper = false;
// 		// imGuiIo.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
// 		// imGuiIo.MousePos : filled by WM_MOUSEMOVE events
// 		// imGuiIo.MouseDown : filled by WM_*BUTTON* events
// 		// imGuiIo.MouseWheel : filled by WM_MOUSEWHEEL events
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	DebugGuiManagerLinux::DebugGuiManagerLinux(IRendererRuntime& rendererRuntime) :
		DebugGuiManager(rendererRuntime),
		mWindowWidth(0),
		mWindowHeigth(0),
		mTime(0)
	{
		// Nothing here
	}

	DebugGuiManagerLinux::~DebugGuiManagerLinux()
	{
		// Nothing here
	}
	
	void DebugGuiManagerLinux::updateMousePosition(int x, int y)
	{
		ImGuiIO& imGuiIo = ImGui::GetIO();
		float windowWidth  = 1.0f;
		float windowHeight = 1.0f;
		{
			// Ensure that none of them is ever zero
			if (mWindowWidth >= 1)
			{
				windowWidth = static_cast<float>(mWindowWidth);
			}
			if (mWindowHeigth >= 1)
			{
				windowHeight = static_cast<float>(mWindowHeigth);
			}
		}
		
		imGuiIo.MousePos.x = static_cast<float>(x) * (imGuiIo.DisplaySize.x / windowWidth);
		imGuiIo.MousePos.y = static_cast<float>(y) * (imGuiIo.DisplaySize.y / windowHeight);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
