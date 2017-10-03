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


cmake_minimum_required(VERSION 3.3.0)


##################################################
## Preprocessor definitions
##################################################
add_definitions(-DGLM_FORCE_RADIANS -DGLM_FORCE_LEFT_HANDED -DGLM_FORCE_DEPTH_ZERO_TO_ONE -DGLM_ENABLE_EXPERIMENTAL)
if(NOT RENDERER_DEBUG)
	add_definitions(-DRENDERER_NO_DEBUG)
endif()

if (Boost_FILESYSTEM_FOUND)
	add_definitions(-DUNRIMP_USE_BOOST_FILESYSTEM)
endif()


##################################################
## Includes
##################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Renderer/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../RendererRuntime/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/mikktspace/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/glm/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/RapidJSON/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/SimpleFileWatcher/src/)


##################################################
## Source codes
##################################################
# Cross-platform source codes
set(SOURCE_CODES
	# SimpleFileWatcher external library
	${CMAKE_CURRENT_SOURCE_DIR}/../../External/SimpleFileWatcher/src/FileWatcher/FileWatcher.cpp
	# mikktspace external library
	${CMAKE_CURRENT_SOURCE_DIR}/../../External/mikktspace/src/mikktspace.c
	# crunch external library
		#api
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crnlib.cpp
		# comp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_dxt.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_dxt_endpoint_refiner.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_dxt_fast.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_dxt_hc_common.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_dxt1.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_dxt5a.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_etc.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_huffman_codes.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_lzma_codec.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_miniz.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_prefix_coding.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_qdxt1.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_qdxt5.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_rg_etc1.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_ryg_dxt.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_symbol_codec.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_zeng.cpp
		# console
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_colorized_console.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_console.cpp
		# containers
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_sparse_bit_array.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_value.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_vector.cpp
		# crn
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_comp.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_dds_comp.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_decomp.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_dxt_hc.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_texture_comp.cpp
		# file
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_file_utils.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_find_files.cpp
		# image
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_image_utils.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_jpgd.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_jpge.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_pixel_format.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_resample_filters.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_resampler.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_stb_image.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_threaded_resampler.cpp
		# lzmalib
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_7zBuf.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_7zBuf2.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_7zCrc.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_7zFile.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_7zStream.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_Alloc.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_Bcj2.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_Bra.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_Bra86.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_BraIA64.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_LzFind.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_LzFindMt.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_LzmaDec.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_LzmaEnc.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_LzmaLib.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/lzma_Threads.cpp
		# math
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_math.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_rand.cpp
		# stream
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_data_stream.cpp
		# string
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_dynamic_string.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_strutils.cpp
		# texture
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_dxt_image.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_ktx_texture.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_mipmapped_texture.cpp
		# texture_conversion
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_texture_conversion.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_texture_file_types.cpp
		# utils
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_arealist.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_checksum.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_command_line_params.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_hash.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_timer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_utils.cpp
		# root
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_assert.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_core.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_mem.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_platform.cpp
	# Assimp external library
		# Common
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/BaseImporter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/BaseProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/Bitmap.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/CInterfaceIOWrapper.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/CreateAnimMesh.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/DefaultIOStream.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/DefaultIOSystem.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/Importer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ImporterRegistry.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/PostStepRegistry.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/RemoveComments.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/scene.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/SceneCombiner.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ScenePreprocessor.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/SGSpatialSort.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/SkeletonMeshBuilder.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/SpatialSort.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/SplitByBoneCountProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/StandardShapes.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/Subdivision.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/TargetAnimation.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/Version.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/VertexTriangleAdjacency.cpp
		# contrib
			# ConvertUTF
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/ConvertUTF/ConvertUTF.c
			# irrXML (at least OGRE importer is using it)
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/irrXML/irrXML.cpp
			# poly2tri (at least BLEND importer is using it)
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/poly2tri/poly2tri/common/shapes.cc
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/poly2tri/poly2tri/sweep/advancing_front.cc
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/poly2tri/poly2tri/sweep/cdt.cc
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/poly2tri/poly2tri/sweep/sweep.cc
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/poly2tri/poly2tri/sweep/sweep_context.cc
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/poly2tri/poly2tri/sweep/advancing_front.cc
			# zlib (at least FBX importer is using it)
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/adler32.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/compress.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/crc32.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/deflate.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/gzclose.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/gzlib.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/gzread.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/gzwrite.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/infback.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/inffast.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/inflate.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/inftrees.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/trees.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/uncompr.c
			${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/contrib/zlib/zutil.c
		# Logging
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/DefaultLogger.cpp
		# MaterialSystem
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/MaterialSystem.cpp
		# MD5
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/MD5Loader.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/MD5Parser.cpp
		# GLTF
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/glTFExporter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/glTFImporter.cpp
		# FBX
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXAnimation.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXBinaryTokenizer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXConverter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXDeformer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXDocument.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXDocumentUtil.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXImporter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXMaterial.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXMeshGeometry.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXModel.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXNodeAttribute.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXParser.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXProperties.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXTokenizer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FBXUtil.cpp
		# BLEND
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/BlenderBMesh.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/BlenderDNA.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/BlenderLoader.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/BlenderModifier.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/BlenderScene.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/BlenderTessellator.cpp
		# COLLADA
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ColladaExporter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ColladaLoader.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ColladaParser.cpp
		# OBJ
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ObjExporter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ObjFileImporter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ObjFileMtlImporter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ObjFileParser.cpp
		# OGRE
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/OgreBinarySerializer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/OgreImporter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/OgreMaterial.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/OgreStructs.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/OgreXmlSerializer.cpp
		# PostProcessing
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/CalcTangentsProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ComputeUVMappingProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ConvertToLHProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/DeboneProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FindDegenerates.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FindInstancesProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FindInvalidDataProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/FixNormalsStep.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/GenFaceNormalsProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/GenVertexNormalsProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ImproveCacheLocality.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/JoinVerticesProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/LimitBoneWeightsProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/MakeVerboseFormat.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/OptimizeGraph.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/OptimizeMeshes.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/PretransformVertices.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ProcessHelper.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/RemoveRedundantMaterials.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/RemoveVCProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/SortByPTypeProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/SplitLargeMeshes.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/TextureTransform.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/TriangulateProcess.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/Assimp/code/ValidateDataStructure.cpp
	# Renderer toolkit
	src/AssetCompiler/IAssetCompiler.cpp
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
	src/AssetImporter/OgreAssetImporter.cpp
	src/AssetImporter/SketchfabAssetImporter.cpp
	src/Helper/JsonHelper.cpp
	src/Helper/JsonMaterialBlueprintHelper.cpp
	src/Helper/JsonMaterialHelper.cpp
	src/Helper/StringHelper.cpp
	src/Helper/AssimpHelper.cpp
	src/Helper/AssimpLogStream.cpp
	src/Helper/CacheManager.cpp
	src/Project/ProjectAssetMonitor.cpp
	src/Project/ProjectImpl.cpp
	src/RendererToolkitImpl.cpp
)

if(WIN32)
	# Windows source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		# SimpleFileWatcher external library
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/SimpleFileWatcher/src/FileWatcher/FileWatcherWin32.cpp
		# crunch external library
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_threading_win32.cpp
	)
endif()
if(UNIX)
	# Linux source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		# SimpleFileWatcher external library
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/SimpleFileWatcher/src/FileWatcher/FileWatcherLinux.cpp
		# crunch external library
		${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/src/crn_threading_pthreads.cpp
	)
endif()


##################################################
## Libraries
##################################################
if(STATIC_LIBRARY)
	add_library(RendererToolkitStatic STATIC ${SOURCE_CODES})
	if (WIN32)
		# Nothing here
	elseif(UNIX)
		set(LIBRARIES ${LIBRARIES} stdc++fs dl)

		if (HAVE_CXX_FILESYSTEM_TS_SUPPORT)
			set(LIBRARIES ${LIBRARIES} stdc++fs)
		elseif(Boost_FILESYSTEM_FOUND)
			set(LIBRARIES ${LIBRARIES} Boost::filesystem)
		endif()
	endif()

	target_link_libraries(RendererToolkitStatic ${LIBRARIES})

	add_dependencies(RendererToolkitStatic)
endif()
if(SHARED_LIBRARY)
	add_library(RendererToolkit SHARED ${SOURCE_CODES})
	set_target_properties(RendererToolkit PROPERTIES COMPILE_FLAGS "-DSHARED_LIBRARIES -DRENDERERTOOLKIT_EXPORTS")
	if(WIN32)
		Nothing here
	else()
		target_link_libraries(RendererToolkit dl)
		if (HAVE_CXX_FILESYSTEM_TS_SUPPORT)
			target_link_libraries(RendererToolkit stdc++fs)
		elseif(Boost_FILESYSTEM_FOUND)
			target_link_libraries(RendererToolkit Boost::filesystem)
		endif()
	endif()

	add_dependencies(RendererToolkit)
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
