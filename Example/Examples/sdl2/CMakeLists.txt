#/*********************************************************\
# * Copyright (c) 2012-2017 The Unrimp Team
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

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(SDL2 REQUIRED)


##################################################
## Preprocessor definitions
##################################################
add_definitions(-DGLM_FORCE_RADIANS -DGLM_FORCE_LEFT_HANDED -DGLM_FORCE_DEPTH_ZERO_TO_ONE)
if(NOT RENDERER_DEBUG)
	add_definitions(-DRENDERER_NO_DEBUG)
endif()
if(NOT RENDERER_RUNTIME)
	add_definitions(-DRENDERER_NO_RUNTIME)
endif()

if(UNIX)
	add_definitions(-DRENDERER_NO_DIRECT3D9 -DRENDERER_NO_DIRECT3D10 -DRENDERER_NO_DIRECT3D11 -DRENDERER_NO_DIRECT3D12)
endif()

if (Boost_FILESYSTEM_FOUND)
	add_definitions(-DUNRIMP_USE_BOOST_FILESYSTEM)
endif()


##################################################
## Includes
##################################################
set(UNRIMP_EXAMPLES_BASE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${UNRIMP_EXAMPLES_BASE_PATH}/src)
include_directories(${UNRIMP_EXAMPLES_BASE_PATH}/../../Renderer/Renderer/include)
include_directories(${UNRIMP_EXAMPLES_BASE_PATH}/../../Renderer/RendererRuntime/include)
include_directories(${UNRIMP_EXAMPLES_BASE_PATH}/../../Renderer/RendererToolkit/include)
link_directories(${CUNRIMP_EXAMPLES_BASE_PATH}/../../lib/${OS_ARCHITECTURE})
include_directories(${UNRIMP_EXAMPLES_BASE_PATH}/../../External/glm/include)
include_directories(${UNRIMP_EXAMPLES_BASE_PATH}/../../External/utf8cpp)
include_directories(${UNRIMP_EXAMPLES_BASE_PATH}/../../External/ImGui/include/)
include_directories(${UNRIMP_EXAMPLES_BASE_PATH}/../../External/OpenVR/include/)


##################################################
## Source codes
##################################################
# Cross-platform source codes
set(SOURCE_CODES
	src/Main.cpp
	src/IApplicationRenderer.cpp
	src/ConsoleExampleRunner.cpp
	src/ExampleRunner.cpp
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Framework/CmdLineArgs.cpp
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Framework/ExampleBase.cpp
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Framework/Color4.cpp
)

# Example source
set(SOURCE_CODES
	${SOURCE_CODES}
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Advanced/FirstGpgpu/FirstGpgpu.cpp
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Advanced/IcosahedronTessellation/IcosahedronTessellation.cpp
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Basics/FirstGeometryShader/FirstGeometryShader.cpp
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Basics/FirstIndirectBuffer/FirstIndirectBuffer.cpp
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Basics/FirstInstancing/FirstInstancing.cpp
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Basics/FirstMultipleRenderTargets/FirstMultipleRenderTargets.cpp
	# Doesn't work with sdl2 frontend
#	${UNRIMP_EXAMPLES_BASE_PATH}/src/Basics/FirstMultipleSwapChains/FirstMultipleSwapChains.cpp
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Basics/FirstRenderToTexture/FirstRenderToTexture.cpp
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Basics/FirstTessellation/FirstTessellation.cpp
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Basics/FirstTexture/FirstTexture.cpp
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Basics/FirstTriangle/FirstTriangle.cpp
	${UNRIMP_EXAMPLES_BASE_PATH}/src/Basics/VertexBuffer/VertexBuffer.cpp
)

if(RENDERER_RUNTIME)
	# Renderer runtime example source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/IApplicationRendererRuntime.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Runtime/FirstMesh/FirstMesh.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Runtime/FirstScene/FirstScene.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Runtime/FirstScene/IController.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Runtime/FirstScene/FreeCameraController.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Runtime/FirstCompositor/FirstCompositor.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Runtime/FirstCompositor/CompositorInstancePassFirst.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Runtime/FirstCompositor/CompositorPassFactoryFirst.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Runtime/FirstCompositor/CompositorResourcePassFirst.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Advanced/InstancedCubes/CubeRendererDrawInstanced/BatchDrawInstanced.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Advanced/InstancedCubes/CubeRendererDrawInstanced/CubeRendererDrawInstanced.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Advanced/InstancedCubes/CubeRendererInstancedArrays/BatchInstancedArrays.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Advanced/InstancedCubes/CubeRendererInstancedArrays/CubeRendererInstancedArrays.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Advanced/InstancedCubes/ICubeRenderer.cpp
		${UNRIMP_EXAMPLES_BASE_PATH}/src/Advanced/InstancedCubes/InstancedCubes.cpp
	)

	# TODO(sw) OpenVR doesn't support non Windows systems yet
	if(WIN32)
		set(SOURCE_CODES
			${SOURCE_CODES}
			src/Runtime/FirstScene/VrController.cpp
		)
	endif()
endif()


##################################################
## Executables
##################################################
if(STATIC_LIBRARY)
	# Static
	if(WIN32)
		add_executable(ExamplesStatic_sdl2 WIN32 ${SOURCE_CODES})
	else()
		# For SDL2 we setup a different output executable
		add_executable(ExamplesStatic_sdl2 ${SOURCE_CODES})

		target_compile_definitions(ExamplesStatic_sdl2 PRIVATE "-DEXAMPLE_SDL2")

		target_include_directories(ExamplesStatic_sdl2 PRIVATE ${SDL2_INCLUDE_DIR})
		
		TARGET_LINK_LIBRARIES(ExamplesStatic_sdl2 dl)

		if (HAVE_CXX_FILESYSTEM_TS_SUPPORT)
			TARGET_LINK_LIBRARIES(ExamplesStatic_sdl2 stdc++fs)
		elseif(Boost_FILESYSTEM_FOUND)
			TARGET_LINK_LIBRARIES(ExamplesStatic_sdl2 Boost::filesystem)
		endif()
	endif()

	set(LIBRARIES NullRendererStatic)
	if(RENDERER_OPENGL)
		set(LIBRARIES ${LIBRARIES} OpenGLRendererStatic)
		if(UNIX)
			set(LIBRARIES ${LIBRARIES} X11)
		endif()
	else()
		target_compile_definitions(ExamplesStatic_sdl2 PRIVATE "-DRENDERER_NO_OPENGL")
	endif()
	if(RENDERER_OPENGLES3)
		set(LIBRARIES ${LIBRARIES} OpenGLES3RendererStatic)
		if(UNIX)
			set(LIBRARIES ${LIBRARIES} X11)
		endif()
	else()
		target_compile_definitions(ExamplesStatic_sdl2 PRIVATE "-DRENDERER_NO_OPENGLES3")
	endif()
	if(RENDERER_VULKAN)
		set(LIBRARIES ${LIBRARIES} VulkanRendererStatic)
		if(UNIX)
			set(LIBRARIES ${LIBRARIES} X11)
		endif()
	else()
		target_compile_definitions(ExamplesStatic_sdl2 PRIVATE "-DRENDERER_NO_VULKAN")
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

	# For SDL2 we setup a different output executable
	target_link_libraries(ExamplesStatic_sdl2 ${LIBRARIES} ${SDL2_LIBRARY})
	
	if(WIN32)
		add_dependencies(ExamplesStatic_sdl2 Renderer NullRendererStatic Direct3D9RendererStatic Direct3D10RendererStatic Direct3D11RendererStatic Direct3D12RendererStatic)
	else()
		add_dependencies(ExamplesStatic_sdl2 Renderer NullRendererStatic)
	endif()
	if(RENDERER_VULKAN)
		add_dependencies(ExamplesStatic_sdl2 VulkanRendererStatic)
	endif()
	if(RENDERER_OPENGL)
		add_dependencies(ExamplesStatic_sdl2 OpenGLRendererStatic)
	endif()
	if(RENDERER_OPENGLES3)
		add_dependencies(ExamplesStatic_sdl2 OpenGLES3RendererStatic)
	endif()
endif()


##################################################
## Post build
##################################################
if(STATIC_LIBRARY)
	# Copy static library version
	add_custom_command(TARGET ExamplesStatic_sdl2
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/ExamplesStatic_sdl2${CMAKE_EXECUTABLE_SUFFIX}" ${OUTPUT_BIN_DIR}
	)
endif()
