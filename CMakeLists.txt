#/*********************************************************\
# * Copyright (c) 2012-2013 Christian Ofenberg
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


cmake_minimum_required(VERSION 2.8.8)


##################################################
## Project
##################################################
project(PLRenderer2)


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
	# TODO(co) Detect correct values
	set(ARCHITECTURE_X86 1)
	set(ARCHITECTURE "x86")
else()
	message("Unsupported architecture")
endif()

# Operation system + architecture
if(WIN32)
	set(OS_ARCHITECTURE "Windows_${ARCHITECTURE}")
elseif(UNIX)
	set(OS_ARCHITECTURE "Linux_${ARCHITECTURE}")
else()
	message("Unsupported operation system")
endif()
message(STATUS "Operation system + architecture: ${OS_ARCHITECTURE}")

# Ouput paths and create the directories
set(OUTPUT_BIN_DIR "${CMAKE_SOURCE_DIR}/bin/${OS_ARCHITECTURE}")
message(STATUS "Output binary directory: "${OUTPUT_BIN_DIR})
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
set(RENDERER_NULL			"1"	CACHE BOOL "Build null renderer?")
set(RENDERER_OPENGL			"1"	CACHE BOOL "Build OpenGL renderer?")
	# Advanced OpenGL renderer options
	set(RENDERER_OPENGL_NO_STATE_CLEANUP		"1"	CACHE BOOL "If true, the previous OpenGL state will not be restored after performing an operation (better performance, reduces the binary size slightly, but might result in unexpected behaviour when using OpenGL directly beside this renderer)")
	set(RENDERER_OPENGL_NO_RENDERERMATCHCHECK	"1"	CACHE BOOL "If true, remove renderer<->resource matches (better performance, reduces the binary size slightly, but you will receive no hint when using a resource from another renderer instance)")
	mark_as_advanced(RENDERER_OPENGL_NO_STATE_CLEANUP)
	mark_as_advanced(RENDERER_OPENGL_NO_RENDERERMATCHCHECK)
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
set(CG_SUPPORT				"1"	CACHE BOOL "Cg support?")
set(STATIC_LIBRARY			"1"	CACHE BOOL "Build static library?")
set(SHARED_LIBRARY			"1"	CACHE BOOL "Build shared library?")
# Renderer toolkit
set(RENDERER_TOOLKIT		"1"	CACHE BOOL "Build renderer toolkit?")
# Examples
set(EXAMPLES				"1"	CACHE BOOL "Build examples?")
	# Advanced examples options
	set(EXAMPLES_ASSIMP		"1"	CACHE BOOL "Build Open Asset Import Library (ASSIMP) examples? (will add approximately 2 MB to the executable size when statically linked)")
	mark_as_advanced(EXAMPLES_ASSIMP)


# We need c++0x/c++11 support:
if(${CMAKE_COMPILER_IS_GNUCXX})
	message(STATUS "enable c++11 support")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()
# At first we treat UNIX = Linux
if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX -DRENDERER_NO_DEBUG")
endif()

##################################################
## Subdirectories
##################################################
add_subdirectory(Renderer)
if(EXAMPLES)
	add_subdirectory(Examples)
endif()
