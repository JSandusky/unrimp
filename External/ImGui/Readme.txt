ImGui 1.54 WIP (latest commit 11d0fc8220675e88e117f509e888ed6b7e8c97c6 - February 5, 2018)
- License: "MIT License"
- Online: https://github.com/ocornut/imgui
- Directly compiled and linked inside renderer runtime, hence "src" sub-directory

Changes made:
- "imgui.cpp" -> "SettingsHandlerWindow_WriteAll()" -> TODO(co) Changed this from integer to float since "SettingsHandlerWindow_ReadLine()" is reading floats
