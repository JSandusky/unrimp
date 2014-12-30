#/*********************************************************\
# * Copyright (c) 2012-2014 Christian Ofenberg
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
## Preprocessor definitions
##################################################
if(NOT RENDERER_DEBUG)
	add_definitions(-DRENDERER_NO_DEBUG)
endif()


##################################################
## Includes
##################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Renderer/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


##################################################
## Source codes
##################################################
# Cross-platform source codes
set(SOURCE_CODES
	src/RendererRuntimeImpl.cpp
	# Font
	src/Resource/Font/FontImpl.cpp
)


##################################################
## Libraries
##################################################
if(STATIC_LIBRARY)
	add_library(RendererRuntimeStatic STATIC ${SOURCE_CODES})
endif()
if(SHARED_LIBRARY)
	add_library(RendererRuntime SHARED ${SOURCE_CODES})
	set_target_properties(RendererRuntime PROPERTIES COMPILE_FLAGS "-DSHARED_LIBRARIES -DRENDERERRUNTIME_EXPORTS")
	if(UNIX)
		target_link_libraries(RendererRuntime X11 dl)
	endif()
endif()


##################################################
## Post build
##################################################
if(STATIC_LIBRARY)
	# Copy static library
	add_custom_command(TARGET RendererRuntimeStatic
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}RendererRuntimeStatic${CMAKE_STATIC_LIBRARY_SUFFIX}" ${OUTPUT_LIB_DIR}
	)
endif()
if(SHARED_LIBRARY)
	# Copy shared library
	add_custom_command(TARGET RendererRuntime
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}RendererRuntime${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
	)
	if(WIN32)
		add_custom_command(TARGET RendererRuntime
			COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/RendererRuntime.lib" ${OUTPUT_LIB_DIR}
		)
	endif()
endif()

##################################################
## Post build
##################################################
# Copy header
if(SHARED_LIBRARY)
	add_custom_command(TARGET RendererRuntime
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/include/RendererRuntime/RendererRuntime.h" ${OUTPUT_INCLUDE_DIR}
	)
elseif(STATIC_LIBRARY)
	add_custom_command(TARGET RendererRuntimeStatic
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/include/RendererRuntime/RendererRuntime.h" ${OUTPUT_INCLUDE_DIR}
	)
endif()
