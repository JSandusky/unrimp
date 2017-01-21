#/*********************************************************\
# * Copyright (c) 2012-2017 Christian Ofenberg
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
# * and associated documentation files (the "Software"), to deal in the Software without
# * restriction, including without limitation the rights to use, copy, modify, merge, publish,
# * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
# * Software is furnished to do so, subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in all copies or
# * substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
# * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#\*********************************************************/


cmake_minimum_required(VERSION 3.2.2)


##################################################
## Project
##################################################
project(Unrimp)


##################################################
## Platform
##################################################
# Check which architecture has the host system
# X86_64 aka x64
if(${CMAKE_GENERATOR} MATCHES "Visual Studio 10 Win64")
	set(ARCHITECTURE_X64 1)
	set(ARCHITECTURE "x64")
elseif(${CMAKE_GENERATOR} MATCHES "Visual Studio 10")
	set(ARCHITECTURE_X86 1)
	set(ARCHITECTURE "x86")
elseif(${CMAKE_GENERATOR} MATCHES "Unix Makefiles")
	if (ANDROID)
		set(ARCHITECTURE ${ANDROID_ABI})
	else()
		if(CMAKE_SIZEOF_VOID_P MATCHES 8)
			set(ARCHITECTURE_X64 1)
			set(ARCHITECTURE "x64")
		else()
			set(ARCHITECTURE_X86 1)
			set(ARCHITECTURE "x86")
		endif()
	endif()
else()
	message("Unsupported architecture")
endif()

# Operation system + architecture
if(WIN32)
	set(OS_ARCHITECTURE "Windows_${ARCHITECTURE}")
elseif(UNIX AND NOT ANDROID)
	set(OS_ARCHITECTURE "Linux_${ARCHITECTURE}")
elseif(ANDROID)
	set(OS_ARCHITECTURE "Android_${ARCHITECTURE}")
else()
	message("Unsupported operation system")
endif()
message(STATUS "Operation system + architecture: ${OS_ARCHITECTURE}")

# Output paths and create the directories
set(OUTPUT_BIN_DIR "${CMAKE_SOURCE_DIR}/bin/${OS_ARCHITECTURE}")
message(STATUS "Output binary directory: ${OUTPUT_BIN_DIR}")
file(MAKE_DIRECTORY ${OUTPUT_BIN_DIR})
set(OUTPUT_LIB_DIR "${CMAKE_SOURCE_DIR}/lib/${OS_ARCHITECTURE}")
message(STATUS "Output library directory: ${OUTPUT_LIB_DIR}")
file(MAKE_DIRECTORY ${OUTPUT_LIB_DIR})
set(OUTPUT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/Renderer")
message(STATUS "Output include directory: ${OUTPUT_INCLUDE_DIR}")
file(MAKE_DIRECTORY ${OUTPUT_INCLUDE_DIR})
set(OUTPUT_EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/External")
message(STATUS "External directory: ${OUTPUT_EXTERNAL_DIR}")
set(OUTPUT_EXTERNAL_OS_ARCHITECTURE_DIR "${OUTPUT_EXTERNAL_DIR}/${OS_ARCHITECTURE}")
message(STATUS "External OS architecture directory: ${OUTPUT_EXTERNAL_OS_ARCHITECTURE_DIR}")


##################################################
## Options
##################################################
# No standard renderer options
	# Advanced renderer options
	set(RENDERER_DEBUG	"1"	CACHE BOOL "If false, the e.g. Direct3D 9 PIX functions (D3DPERF_* functions) debug features are disabled (disabling support just reduces the binary size slightly but makes debugging more difficult)")
	mark_as_advanced(RENDERER_DEBUG)

# Null renderer doesn't compile under Android for now disable it
if(NOT ANDROID)
	set(RENDERER_NULL			"1"	CACHE BOOL "Build null renderer?")
endif()

# OpenGL renderer is not for Android
if(NOT ANDROID)
	set(RENDERER_OPENGL			"1"	CACHE BOOL "Build OpenGL renderer?")
		# Advanced OpenGL renderer options
		set(RENDERER_OPENGL_NO_STATE_CLEANUP		"1"	CACHE BOOL "If true, the previous OpenGL state will not be restored after performing an operation (better performance, reduces the binary size slightly, but might result in unexpected behaviour when using OpenGL directly beside this renderer)")
		set(RENDERER_OPENGL_NO_RENDERERMATCHCHECK	"1"	CACHE BOOL "If true, remove renderer<->resource matches (better performance, reduces the binary size slightly, but you will receive no hint when using a resource from another renderer instance)")
		mark_as_advanced(RENDERER_OPENGL_NO_STATE_CLEANUP)
		mark_as_advanced(RENDERER_OPENGL_NO_RENDERERMATCHCHECK)
endif()
set(RENDERER_OPENGLES2		"1"	CACHE BOOL "Build OpenGL ES 2 renderer?")
	# Advanced OpenGL ES 2 renderer options
	set(RENDERER_OPENGLES2_NO_STATE_CLEANUP			"1"	CACHE BOOL "If true, the previous OpenGL ES 2 state will not be restored after performing an operation (better performance, reduces the binary size slightly, but might result in unexpected behaviour when using OpenGL ES 2 directly beside this renderer)")
	set(RENDERER_OPENGLES2_NO_RENDERERMATCHCHECK	"1"	CACHE BOOL "If true, remove renderer<->resource matches (better performance, reduces the binary size slightly, but you will receive no hint when using a resource from another renderer instance)")
	mark_as_advanced(RENDERER_OPENGLES2_NO_STATE_CLEANUP)
	mark_as_advanced(RENDERER_OPENGLES2_NO_RENDERERMATCHCHECK)
if(WIN32)
	# Direct3D is Windows only
	set(RENDERER_DIRECT3D9	"1"	CACHE BOOL "Build Direct3D 9 renderer?")
		# Advanced Direct3D 9 renderer options
		set(RENDERER_DIRECT3D9_DEBUG	"1"	CACHE BOOL "If false, the Direct3D 9 PIX functions (D3DPERF_* functions) debug features are disabled (disabling support just reduces the binary size slightly but makes debugging more difficult)")
		mark_as_advanced(RENDERER_DIRECT3D9_DEBUG)
	set(RENDERER_DIRECT3D10	"1"	CACHE BOOL "Build Direct3D 10 renderer?")
		# Advanced Direct3D 10 renderer options
		set(RENDERER_DIRECT3D10_DEBUG	"1"	CACHE BOOL "If false, the Direct3D 9 PIX functions (D3DPERF_* functions) debug features are disabled (disabling support just reduces the binary size slightly but makes debugging more difficult)")
		mark_as_advanced(RENDERER_DIRECT3D10_DEBUG)
	set(RENDERER_DIRECT3D11	"1"	CACHE BOOL "Build Direct3D 11 renderer?")
		# Advanced Direct3D 11 renderer options
		set(RENDERER_DIRECT3D11_DEBUG	"1"	CACHE BOOL "If false, the Direct3D 9 PIX functions (D3DPERF_* functions) debug features are disabled (disabling support just reduces the binary size slightly but makes debugging more difficult)")
		mark_as_advanced(RENDERER_DIRECT3D11_DEBUG)
endif()
set(STATIC_LIBRARY			"1"	CACHE BOOL "Build static library?")
set(SHARED_LIBRARY			"1"	CACHE BOOL "Build shared library?")

# Renderer toolkit is not for Android
if(NOT ANDROID)
	# Renderer toolkit
	set(RENDERER_TOOLKIT	"1"	CACHE BOOL "Build renderer toolkit?")
endif()

# Renderer runtime
set(RENDERER_RUNTIME		"1"	CACHE BOOL "Build renderer runtime?")

# No examples for Android for now
if(NOT ANDROID)
	# Examples
	set(EXAMPLES				"1"	CACHE BOOL "Build examples?")
		# Advanced examples options
		set(EXAMPLES_ASSIMP		"1"	CACHE BOOL "Build Open Asset Import Library (ASSIMP) examples? (will add approximately 2 MB to the executable size when statically linked)")
		mark_as_advanced(EXAMPLES_ASSIMP)
	set(EXAMPLE_PROJECT_COMPILER "1" CACHE BOOL "Build example project compiler?")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	# We need C++1z/C++17 support but only for the renderer toolkit part, runtime currently should be only uses C++11 features
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-std=c++1z" COMPILER_SUPPORTS_CXX1z)
	CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
	CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)

	if(RENDERER_TOOLKIT AND NOT (COMPILER_SUPPORTS_CXX17) AND NOT COMPILER_SUPPORTS_CXX1z)
		message(FATAL_ERROR "Renderer toolkit needs C++17 support but the compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
	endif()

	# All Unrimp parts except renderer toolkit uses only C++14 features, so we can enable here "globally" C++14 compiler settings
	# Renderer toolkit only needs C++17 runtime features (filesystem TS) so no C++17 specific compiler features
	if(COMPILER_SUPPORTS_CXX14)
		add_compile_options("-std=c++14")
	else()
		message(FATAL_ERROR "Unrimp needs at least C++14 support but the compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
	endif()
endif()

include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX("filesystem" HAVE_CXX17_FILESYSTEM)
CHECK_INCLUDE_FILE_CXX("experimental/filesystem" HAVE_CXX1z_FILESYSTEM)
if(RENDERER_TOOLKIT AND NOT HAVE_CXX17_FILESYSTEM AND NOT HAVE_CXX1z_FILESYSTEM)
	message(FATAL_ERROR "Renderer toolkit needs C++ filesystem TS support but the compiler ${CMAKE_CXX_COMPILER} doesn't support C++ filesystem TS. Please use a different C++ compiler.")
endif()

# Check for visibility support: -fvisibility=hidden -fvisibility-inlines-hidden
check_cxx_compiler_flag(-fvisibility=hidden GXX_HAS_VISIBILITY_FLAG)
check_cxx_compiler_flag(-fvisibility-inlines-hidden GXX_HAS_VISIBILITY_INLINES_FLAG)

# if(GXX_HAS_VISIBILITY_FLAG AND NOT APPLE)
# 	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
# endif()
# 
# if(GXX_HAS_VISIBILITY_INLINES_FLAG AND NOT APPLE)
# 	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
# endif()

# At first we treat UNIX = Linux
if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX")
endif()

# We treat Android also as Linux
if(ANDROID)
	# We use exceptions so enable exception feature, is disabled by default by the Android NDK
	set(ANDROID_CPP_FEATURES "exceptions" CACHE STRING "")

	# We only support Android 5.1+
	set(ANDROID_PLATFORM "android-22" CACHE STRING "")

	# We want also the llvm libc++ which is fully C++11 compatible
	# TODO(sw) For qtunrimp we cannot use llvm libc++ because Qt is not yet compilable with llvm libc++ on Android
	#          For now we need to use gnustl_shared
	#set(ANDROID_STL "c++_shared" CACHE STRING "")
	set(ANDROID_STL "gnustl_shared" CACHE STRING "")

	# L"" in macros don't work with Android GCC -> so renderer debug disabled
	add_definitions("-DANDROID -DRENDERER_NO_DEBUG")
endif()

if (${ARCHITECTURE_X64})
	add_definitions("-DX64_ARCHITECTURE")
endif()


##################################################
## Subdirectories
##################################################
add_subdirectory(Renderer)
if(RENDERER_TOOLKIT)
	add_subdirectory(External/crunch/crunch)
endif()
if(EXAMPLES)
	add_subdirectory(Example/Examples)
endif()

if(EXAMPLE_PROJECT_COMPILER AND RENDERER_RUNTIME AND RENDERER_TOOLKIT)
	add_subdirectory(Example/ExampleProjectCompiler)
endif()
