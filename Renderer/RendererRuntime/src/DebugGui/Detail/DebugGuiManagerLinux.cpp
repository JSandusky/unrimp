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


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]



	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::DebugGuiManager methods ]
	//[-------------------------------------------------------]
	void DebugGuiManagerLinux::startup()
	{
		// TODO(sw) implement me
// 		::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&mTicksPerSecond));
// 		::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mTime));
// 
// 		// Keyboard mapping. ImGui will use those indices to peek into the imGuiIo.KeyDown[] array that we will update during the application lifetime.
// 		ImGuiIO& imGuiIo = ImGui::GetIO();
// 		imGuiIo.KeyMap[ImGuiKey_Tab]		= VK_TAB;
// 		imGuiIo.KeyMap[ImGuiKey_LeftArrow]	= VK_LEFT;
// 		imGuiIo.KeyMap[ImGuiKey_RightArrow]	= VK_RIGHT;
// 		imGuiIo.KeyMap[ImGuiKey_UpArrow]	= VK_UP;
// 		imGuiIo.KeyMap[ImGuiKey_DownArrow]	= VK_DOWN;
// 		imGuiIo.KeyMap[ImGuiKey_PageUp]		= VK_PRIOR;
// 		imGuiIo.KeyMap[ImGuiKey_PageDown]	= VK_NEXT;
// 		imGuiIo.KeyMap[ImGuiKey_Home]		= VK_HOME;
// 		imGuiIo.KeyMap[ImGuiKey_End]		= VK_END;
// 		imGuiIo.KeyMap[ImGuiKey_Delete]		= VK_DELETE;
// 		imGuiIo.KeyMap[ImGuiKey_Backspace]	= VK_BACK;
// 		imGuiIo.KeyMap[ImGuiKey_Enter]		= VK_RETURN;
// 		imGuiIo.KeyMap[ImGuiKey_Escape]		= VK_ESCAPE;
// 		imGuiIo.KeyMap[ImGuiKey_A]			= 'A';
// 		imGuiIo.KeyMap[ImGuiKey_C]			= 'C';
// 		imGuiIo.KeyMap[ImGuiKey_V]			= 'V';
// 		imGuiIo.KeyMap[ImGuiKey_X]			= 'X';
// 		imGuiIo.KeyMap[ImGuiKey_Y]			= 'Y';
// 		imGuiIo.KeyMap[ImGuiKey_Z]			= 'Z';

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
		
// 		{ // Setup time step
// 			INT64 currentTime = 0;
// 			::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));
// 			imGuiIo.DeltaTime = static_cast<float>(currentTime - mTime) / mTicksPerSecond;
// 			mTime = currentTime;
// 		}
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
		DebugGuiManager(rendererRuntime)
	{
		// Nothing here
	}

	DebugGuiManagerLinux::~DebugGuiManagerLinux()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
