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
## Includes
##################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Renderer/Renderer/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Renderer/RendererRuntime/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Renderer/RendererToolkit/include)


##################################################
## Source codes
##################################################
# Cross-platform source codes
set(SOURCE_CODES
	# Cross-platform source codes
	src/Main.cpp
)


##################################################
## Executables
##################################################
if(STATIC_LIBRARY)
	# Static
	if(WIN32)
		add_executable(ExampleProjectCompilerStatic WIN32 ${SOURCE_CODES})
	else()
		add_executable(ExampleProjectCompilerStatic ${SOURCE_CODES})
	endif()
	set(LIBRARIES ${LIBRARIES} RendererToolkitStatic RendererRuntimeStatic)
	if(UNIX)
		set(LIBRARIES ${LIBRARIES} pthread)
	endif()
	target_link_libraries(ExampleProjectCompilerStatic ${LIBRARIES})
	add_dependencies(ExampleProjectCompilerStatic Renderer RendererToolkitStatic RendererRuntimeStatic)
endif()
if(SHARED_LIBRARY)
	# Shared
	if(WIN32)
		add_executable(ExampleProjectCompiler WIN32 ${SOURCE_CODES})
	else()
		add_executable(ExampleProjectCompiler ${SOURCE_CODES})
	endif()
	if(RENDERER_RUNTIME)
		if(WIN32)
			target_link_libraries(ExampleProjectCompiler RendererRuntime.lib)
		else()
			target_link_libraries(ExampleProjectCompiler RendererRuntime)
		endif()
	endif()
	if(EXAMPLE_RUNNER_QT4)
		TARGET_LINK_LIBRARIES(Examples ${QT_LIBRARIES})
	endif()
	set_target_properties(ExampleProjectCompiler PROPERTIES COMPILE_FLAGS -DSHARED_LIBRARIES)
	add_dependencies(ExampleProjectCompiler Renderer RendererRuntime)
endif()


##################################################
## Post build
##################################################
if(STATIC_LIBRARY)
	# Copy static library version
	add_custom_command(TARGET ExampleProjectCompilerStatic
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/ExampleProjectCompilerStatic${CMAKE_EXECUTABLE_SUFFIX}" ${OUTPUT_BIN_DIR}
	)
endif()
if(SHARED_LIBRARY)
	# Copy shared library version
	add_custom_command(TARGET ExampleProjectCompiler
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/ExampleProjectCompiler${CMAKE_EXECUTABLE_SUFFIX}" ${OUTPUT_BIN_DIR}
	)
endif()
