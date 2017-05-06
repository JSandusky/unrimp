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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../RendererRuntime/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/mikktspace/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/RapidJSON/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/SimpleFileWatcher/src/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/sqlitecpp/SQLiteCpp/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/picosha2/include)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/${OS_ARCHITECTURE}/Assimp/lib)


##################################################
## Source codes
##################################################
# Cross-platform source codes
set(SOURCE_CODES
	${CMAKE_CURRENT_SOURCE_DIR}/../../External/SimpleFileWatcher/src/FileWatcher/FileWatcher.cpp
	src/AssetCompiler/CompositorNodeAssetCompiler.cpp
	src/AssetCompiler/CompositorWorkspaceAssetCompiler.cpp
	src/AssetCompiler/MaterialAssetCompiler.cpp
	src/AssetCompiler/MaterialBlueprintAssetCompiler.cpp
	src/AssetCompiler/MeshAssetCompiler.cpp
	src/AssetCompiler/SceneAssetCompiler.cpp
	src/AssetCompiler/ShaderBlueprintAssetCompiler.cpp
	src/AssetCompiler/ShaderPieceAssetCompiler.cpp
	src/AssetCompiler/SkeletonAssetCompiler.cpp
	src/AssetCompiler/VertexAttributesAssetCompiler.cpp
	src/AssetCompiler/SkeletonAnimationAssetCompiler.cpp
	src/AssetCompiler/TextureAssetCompiler.cpp
	src/Helper/JsonHelper.cpp
	src/Helper/JsonMaterialBlueprintHelper.cpp
	src/Helper/JsonMaterialHelper.cpp
	src/Helper/StringHelper.cpp
	src/Helper/CacheManager.cpp
	src/Project/ProjectAssetMonitor.cpp
	src/Project/ProjectImpl.cpp
	src/RendererToolkitImpl.cpp
)

if(WIN32)
	# Windows source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/SimpleFileWatcher/src/FileWatcher/FileWatcherWin32.cpp
	)
endif()
if(UNIX)
	# Linux source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/SimpleFileWatcher/src/FileWatcher/FileWatcherLinux.cpp
	)
endif()


##################################################
## Libraries
##################################################
if(STATIC_LIBRARY)
	add_library(RendererToolkitStatic STATIC ${SOURCE_CODES})
	if (WIN32)
		set(LIBRARIES ${LIBRARIES} ${CMAKE_STATIC_LIBRARY_PREFIX}assimpStatic${CMAKE_STATIC_LIBRARY_SUFFIX} zlibStatic.lib SQLiteCpp sqlite3)
	elseif(UNIX)
		set(LIBRARIES ${LIBRARIES} assimp tiff minizip crunch stdc++fs SQLiteCpp sqlite3 dl)
	endif()

	target_link_libraries(RendererToolkitStatic ${LIBRARIES})

	add_dependencies(RendererToolkitStatic crunch SQLiteCpp)
endif()
if(SHARED_LIBRARY)
	add_library(RendererToolkit SHARED ${SOURCE_CODES})
	set_target_properties(RendererToolkit PROPERTIES COMPILE_FLAGS "-DSHARED_LIBRARIES -DRENDERERTOOLKIT_EXPORTS")
	if(WIN32)
		target_link_libraries(RendererToolkit assimp.lib crnlib.lib SQLiteCpp sqlite3)
	else()
		target_link_libraries(RendererToolkit ${CMAKE_SHARED_LIBRARY_PREFIX}assimp${CMAKE_SHARED_LIBRARY_SUFFIX} tiff minizip crunch stdc++fs SQLiteCpp sqlite3 dl)
	endif()

	add_dependencies(RendererToolkit crunch SQLiteCpp)
endif()


##################################################
## Post build
##################################################
if(STATIC_LIBRARY)
	# Copy static library
	add_custom_command(TARGET RendererToolkitStatic
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}RendererToolkitStatic${CMAKE_STATIC_LIBRARY_SUFFIX}" ${OUTPUT_LIB_DIR}
	)
endif()
if(SHARED_LIBRARY)
	# Copy shared library
	add_custom_command(TARGET RendererToolkit
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}RendererToolkit${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
	)
	if(WIN32)
		add_custom_command(TARGET RendererToolkit
			COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/RendererToolkit.lib" ${OUTPUT_LIB_DIR}
		)
	endif()
	if(WIN32 AND EXAMPLES_ASSIMP)
		add_custom_command(TARGET Examples
			COMMAND ${CMAKE_COMMAND} -E copy "${OUTPUT_EXTERNAL_OS_ARCHITECTURE_DIR}/Assimp/bin/${CMAKE_SHARED_LIBRARY_PREFIX}assimp${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
		)
		add_custom_command(TARGET Examples
			COMMAND ${CMAKE_COMMAND} -E copy "${OUTPUT_EXTERNAL_OS_ARCHITECTURE_DIR}/VisualStudio/bin/${CMAKE_SHARED_LIBRARY_PREFIX}msvcp110${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
		)
	endif()
endif()


##################################################
## Post build
##################################################
# Copy header
if(SHARED_LIBRARY)
	add_custom_command(TARGET RendererToolkit
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/include/RendererToolkit/Public/RendererToolkit.h" ${OUTPUT_INCLUDE_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/include/RendererToolkit/Public/RendererToolkitInstance.h" ${OUTPUT_INCLUDE_DIR}
	)
elseif(STATIC_LIBRARY)
	add_custom_command(TARGET RendererToolkitStatic
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/include/RendererToolkit/Public/RendererToolkit.h" ${OUTPUT_INCLUDE_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/include/RendererToolkit/Public/RendererToolkitInstance.h" ${OUTPUT_INCLUDE_DIR}
	)
endif()
