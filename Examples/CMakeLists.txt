#/*********************************************************\
# * Copyright (c) 2012-2013 Christian Ofenberg
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
# * and associated documentation files (the “Software”), to deal in the Software without
# * restriction, including without limitation the rights to use, copy, modify, merge, publish,
# * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
# * Software is furnished to do so, subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in all copies or
# * substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
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
if(NOT CG_SUPPORT)
	add_definitions(-DRENDERER_NO_CG)
endif()
if(NOT RENDERER_TOOLKIT)
	add_definitions(-DRENDERER_NO_TOOLKIT)
endif()
if(NOT EXAMPLES_ASSIMP)
	add_definitions(-DNO_ASSIMP)
endif()


##################################################
## Includes
##################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/${OS_ARCHITECTURE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../External/glm/include)
if(EXAMPLES_ASSIMP)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../External/Assimp/include)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../External/${OS_ARCHITECTURE}/Assimp/lib)
endif()


##################################################
## Source codes
##################################################
# Cross-platform source codes
set(SOURCE_CODES
	# Cross-platform source codes
	src/Main.cpp
	src/Framework/Color4.cpp
	src/Framework/EulerAngles.cpp
	src/Framework/IApplication.cpp
	src/Framework/IApplicationImpl.cpp
	src/Framework/IApplicationRenderer.cpp
	src/Framework/Stopwatch.cpp
	src/Framework/TGALoader.cpp
	# Basics
		# "FirstTriangle" source codes
			src/FirstTriangle/FirstTriangle.cpp
		# "VertexBuffer" source codes
			src/VertexBuffer/VertexBuffer.cpp
		# "FirstTexture" source codes
			src/FirstTexture/FirstTexture.cpp
		# "FirstRenderToTexture" source codes
			src/FirstRenderToTexture/FirstRenderToTexture.cpp
		# "FirstMultipleRenderTargets" source codes
			src/FirstMultipleRenderTargets/FirstMultipleRenderTargets.cpp
		# "FirstMultipleSwapChains" source codes
			src/FirstMultipleSwapChains/FirstMultipleSwapChains.cpp
		# "FirstInstancing" source codes
			src/FirstInstancing/FirstInstancing.cpp
		# "FirstGeometryShader" source codes
			src/FirstGeometryShader/FirstGeometryShader.cpp
		# "FirstTessellation" source codes
			src/FirstTessellation/FirstTessellation.cpp
	# Advanced
		# "FirstPostProcessing" source codes
			src/FirstPostProcessing/FirstPostProcessing.cpp
		# "FXAA" source codes
			src/Fxaa/Fxaa.cpp
		# "FirstGpgpu" source codes
			src/FirstGpgpu/FirstGpgpu.cpp
		# "InstancedCubes" source codes
			src/InstancedCubes/InstancedCubes.cpp
			src/InstancedCubes/ICubeRenderer.cpp
			src/InstancedCubes/CubeRendererInstancing/Batch.cpp
			src/InstancedCubes/CubeRendererInstancing/CubeRendererInstancing.cpp
		# "IcosahedronTessellation" source codes
			src/IcosahedronTessellation/IcosahedronTessellation.cpp
)
if(WIN32)
	# Windows source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/Framework/ApplicationImplWindows.cpp
	)
endif()
if(RENDERER_TOOLKIT)
	# Renderer toolkit example source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/Framework/IApplicationRendererToolkit.cpp
		src/FirstFont/FirstFont.cpp
	)
endif()
if(EXAMPLES_ASSIMP)
	# Assimp example source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/FirstAssimp/FirstAssimp.cpp
		src/FirstAssimp/PointsMesh.cpp
		src/AssimpMesh/AssimpMesh.cpp
		src/AssimpMesh/Mesh.cpp
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
		add_executable(ExamplesStatic ${SOURCE_CODES})
	endif()
	set(LIBRARIES NullRendererStatic OpenGLRendererStatic OpenGLES2RendererStatic Direct3D9RendererStatic Direct3D10RendererStatic Direct3D11RendererStatic)
	if(RENDERER_TOOLKIT)
		set(LIBRARIES ${LIBRARIES} ${CMAKE_STATIC_LIBRARY_PREFIX}RendererToolkit${CMAKE_STATIC_LIBRARY_SUFFIX})
	endif()
	if(EXAMPLES_ASSIMP)
		set(LIBRARIES ${LIBRARIES} ${CMAKE_STATIC_LIBRARY_PREFIX}assimpStatic${CMAKE_STATIC_LIBRARY_SUFFIX} zlibStatic.lib)
	endif()
	target_link_libraries(ExamplesStatic ${LIBRARIES})
	add_dependencies(ExamplesStatic Renderer NullRendererStatic OpenGLRendererStatic OpenGLES2RendererStatic Direct3D9RendererStatic Direct3D10RendererStatic Direct3D11RendererStatic)
endif()
if(SHARED_LIBRARY)
	# Shared
	if(WIN32)
		add_executable(Examples WIN32 ${SOURCE_CODES})
	else()
		add_executable(Examples ${SOURCE_CODES})
	endif()
	if(RENDERER_TOOLKIT)
		if(WIN32)
			target_link_libraries(Examples RendererToolkit.lib)
		else()
			target_link_libraries(Examples ${CMAKE_SHARED_LIBRARY_PREFIX}RendererToolkit{CMAKE_SHARED_LIBRARY_SUFFIX})
		endif()
	endif()
	if(EXAMPLES_ASSIMP)
		if(WIN32)
			target_link_libraries(Examples assimp.lib)
		else()
			target_link_libraries(Examples ${CMAKE_SHARED_LIBRARY_PREFIX}assimpStatic${CMAKE_SHARED_LIBRARY_SUFFIX})
		endif()
	endif()
	set_target_properties(Examples PROPERTIES COMPILE_FLAGS -DSHARED_LIBRARIES)
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

	# Copy Visual Studio shared library
	add_custom_command(TARGET Examples
		COMMAND ${CMAKE_COMMAND} -E copy "${OUTPUT_EXTERNAL_OS_ARCHITECTURE_DIR}/VisualStudio/bin/${CMAKE_SHARED_LIBRARY_PREFIX}msvcr110${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
	)

	# Copy ASSIMP shared library
	if(WIN32 AND EXAMPLES_ASSIMP)
		add_custom_command(TARGET Examples
			COMMAND ${CMAKE_COMMAND} -E copy "${OUTPUT_EXTERNAL_OS_ARCHITECTURE_DIR}/Assimp/bin/${CMAKE_SHARED_LIBRARY_PREFIX}assimp${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
		)
		add_custom_command(TARGET Examples
			COMMAND ${CMAKE_COMMAND} -E copy "${OUTPUT_EXTERNAL_OS_ARCHITECTURE_DIR}/VisualStudio/bin/${CMAKE_SHARED_LIBRARY_PREFIX}msvcp110${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
		)
	endif()
endif()
