#/*********************************************************\
# * Copyright (c) 2012-2016 Christian Ofenberg
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


find_package(Qt4 COMPONENTS QtCore QtGui)

if (QT4_FOUND AND QT_QTGUI_FOUND)
	set(EXAMPLE_RUNNER_QT4 "0" CACHE BOOL "use Qt Gui Runner")
else()
	unset(EXAMPLE_RUNNER_QT4 CACHE)
endif()

if (EXAMPLE_RUNNER_QT4)
	INCLUDE(${QT_USE_FILE})
endif()


##################################################
## Preprocessor definitions
##################################################
if(NOT RENDERER_DEBUG)
	add_definitions(-DRENDERER_NO_DEBUG)
endif()
if(NOT RENDERER_RUNTIME)
	add_definitions(-DRENDERER_NO_RUNTIME)
endif()
if(EXAMPLE_RUNNER_QT4)
	add_definitions(-DEXAMPLE_RUNNER_QT4)
	ADD_DEFINITIONS(${QT_DEFINITIONS})
endif()

if(UNIX)
	add_definitions(-DRENDERER_NO_DIRECT3D10 -DRENDERER_NO_DIRECT3D11 -DRENDERER_NO_DIRECT3D9)
endif()


##################################################
## Includes
##################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Renderer/Renderer/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Renderer/RendererRuntime/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Renderer/RendererToolkit/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${OS_ARCHITECTURE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/glm/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/utf8cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/ImGui/include/)


##################################################
## Source codes
##################################################
# Cross-platform source codes
set(SOURCE_CODES
	# Cross-platform source codes
	src/Advanced/FirstGpgpu/FirstGpgpu.cpp
	src/Advanced/IcosahedronTessellation/IcosahedronTessellation.cpp
	src/Basics/FirstGeometryShader/FirstGeometryShader.cpp
	src/Basics/FirstIndirectBuffer/FirstIndirectBuffer.cpp
	src/Basics/FirstInstancing/FirstInstancing.cpp
	src/Basics/FirstMultipleRenderTargets/FirstMultipleRenderTargets.cpp
	src/Basics/FirstMultipleSwapChains/FirstMultipleSwapChains.cpp
	src/Basics/FirstRenderToTexture/FirstRenderToTexture.cpp
	src/Basics/FirstTessellation/FirstTessellation.cpp
	src/Basics/FirstTexture/FirstTexture.cpp
	src/Basics/FirstTriangle/FirstTriangle.cpp
	src/Basics/VertexBuffer/VertexBuffer.cpp
	src/ConsoleExampleRunner.cpp
	src/ExampleRunner.cpp
	src/Framework/CmdLineArgs.cpp
	src/Framework/Color4.cpp
	src/Framework/EulerAngles.cpp
	src/Framework/IApplication.cpp
	src/Framework/IApplicationImpl.cpp
	src/Framework/IApplicationRenderer.cpp
	src/Framework/Stopwatch.cpp
	src/Main.cpp
)
if(WIN32)
	# Windows source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/Framework/ApplicationImplWindows.cpp
		src/PrecompiledHeader.cpp
	)
elseif(UNIX)
	# Linux source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/Framework/ApplicationImplLinux.cpp
		src/Framework/X11Window.cpp
		src/Framework/X11Application.cpp
	)
endif()
if(RENDERER_RUNTIME)
	# Renderer runtime example source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/Framework/IApplicationRendererRuntime.cpp
		src/Runtime/FirstMesh/FirstMesh.cpp
		src/Runtime/FirstScene/FirstScene.cpp
		src/Runtime/FirstScene/FreeCameraController.cpp
		src/Runtime/FirstCompositor/FirstCompositor.cpp
		src/Runtime/FirstCompositor/CompositorInstancePassFirst.cpp
		src/Runtime/FirstCompositor/CompositorPassFactoryFirst.cpp
		src/Runtime/FirstCompositor/CompositorResourcePassFirst.cpp
		src/Advanced/InstancedCubes/CubeRendererDrawInstanced/BatchDrawInstanced.cpp
		src/Advanced/InstancedCubes/CubeRendererDrawInstanced/CubeRendererDrawInstanced.cpp
		src/Advanced/InstancedCubes/CubeRendererInstancedArrays/BatchInstancedArrays.cpp
		src/Advanced/InstancedCubes/CubeRendererInstancedArrays/CubeRendererInstancedArrays.cpp
		src/Advanced/InstancedCubes/ICubeRenderer.cpp
		src/Advanced/InstancedCubes/InstancedCubes.cpp
	)
endif()

if(EXAMPLE_RUNNER_QT4)
	# Qt runner source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/Framework/QtRunner/ExampleRunnerQt4.cpp
	)
endif()


##################################################
## Executables
##################################################
if(STATIC_LIBRARY)
	# Static
	if(WIN32)
		add_executable(ExamplesStatic WIN32 ${SOURCE_CODES})
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRENDERER_NO_DIRECT3D9 -DRENDERER_NO_DIRECT3D10 -DRENDERER_NO_DIRECT3D11 -DRENDERER_NO_DIRECT3D12")
		add_executable(ExamplesStatic ${SOURCE_CODES})
		if(EXAMPLE_RUNNER_QT4)
			TARGET_LINK_LIBRARIES(ExamplesStatic ${QT_LIBRARIES})
		endif()
		TARGET_LINK_LIBRARIES(ExamplesStatic dl)
	endif()
	
	set(LIBRARIES NullRendererStatic)
	if(RENDERER_OPENGL)
		set(LIBRARIES ${LIBRARIES} OpenGLRendererStatic)
		if(UNIX)
			set(LIBRARIES ${LIBRARIES} X11)
		endif()
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRENDERER_NO_OPENGL")
	endif()
	if(RENDERER_OPENGLES2)
		set(LIBRARIES ${LIBRARIES} OpenGLES2RendererStatic)
		if(UNIX)
			set(LIBRARIES ${LIBRARIES} X11)
		endif()
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRENDERER_NO_OPENGLES2")
	endif()
	if(WIN32)
		set(LIBRARIES ${LIBRARIES} Direct3D9RendererStatic Direct3D10RendererStatic Direct3D11RendererStatic Direct3D12RendererStatic)
	endif()
	if(RENDERER_RUNTIME)
		set(LIBRARIES ${LIBRARIES} RendererRuntimeStatic)
		if(UNIX)
			set(LIBRARIES ${LIBRARIES} pthread)
		endif()
	endif()
	target_link_libraries(ExamplesStatic ${LIBRARIES})
	
	if(WIN32)
		add_dependencies(ExamplesStatic Renderer NullRendererStatic OpenGLRendererStatic OpenGLES2RendererStatic Direct3D9RendererStatic Direct3D10RendererStatic Direct3D11RendererStatic Direct3D12RendererStatic)
	else()
		add_dependencies(ExamplesStatic Renderer NullRendererStatic OpenGLRendererStatic OpenGLES2RendererStatic)
	endif()
endif()
if(SHARED_LIBRARY)
	# Shared
	if(WIN32)
		add_executable(Examples WIN32 ${SOURCE_CODES})
	else()
		add_executable(Examples ${SOURCE_CODES})
	endif()
	if(RENDERER_RUNTIME)
		if(WIN32)
			target_link_libraries(Examples RendererRuntime.lib)
		else()
			target_link_libraries(Examples RendererRuntime)
		endif()
	endif()
	if(EXAMPLE_RUNNER_QT4)
		TARGET_LINK_LIBRARIES(Examples ${QT_LIBRARIES})
	endif()
	set_target_properties(Examples PROPERTIES COMPILE_FLAGS -DSHARED_LIBRARIES)
	target_link_libraries(Examples X11 dl)
	add_dependencies(Examples Renderer)
endif()


##################################################
## Post build
##################################################
if(STATIC_LIBRARY)
	# Copy static library version
	add_custom_command(TARGET ExamplesStatic
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/ExamplesStatic${CMAKE_EXECUTABLE_SUFFIX}" ${OUTPUT_BIN_DIR}
	)
endif()
if(SHARED_LIBRARY)
	# Copy shared library version
	add_custom_command(TARGET Examples
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Examples${CMAKE_EXECUTABLE_SUFFIX}" ${OUTPUT_BIN_DIR}
	)
endif()
